/*
 * DESCRIPTION: Simple managed-memory allocator that reduces the overhead
 *   of using malloc/calloc/free functions by allocating slabs of memory
 *   and dispatching memory segments in order.
 */

#include "mm_allocator.h"

/*
 * Debug
 */
//#define MM_ALLOCATOR_FORCE_MALLOC /* Delegate all requests to malloc within the mm-allocator handler */
//#define MM_ALLOCATOR_DISABLE      /* Completely disable the mm-allocator (delegate to raw malloc) */

/*
 * Constants
 */
#define MM_ALLOCATOR_SEGMENT_INITIAL_REQUESTS   10000
#define MM_ALLOCATOR_INITIAL_SEGMENTS              10
#define MM_ALLOCATOR_INITIAL_MALLOC_REQUESTS       10
#define MM_ALLOCATOR_INITIAL_STATES                10

/*
 * Allocator Segments Freed Cond
 */
#define MM_ALLOCATOR_FREED_FLAG                 0x80000000ul
#define MM_ALLOCATOR_REQUEST_IS_FREE(request)  ((request)->size & MM_ALLOCATOR_FREED_FLAG)
#define MM_ALLOCATOR_REQUEST_SET_FREE(request) ((request)->size |= MM_ALLOCATOR_FREED_FLAG)
#define MM_ALLOCATOR_REQUEST_SIZE(request)     ((request)->size & ~(MM_ALLOCATOR_FREED_FLAG))

typedef struct{
  // Index(ID)
  uint64_t idx;
  // Memory
  uint64_t size;
  void* memory;
  uint64_t used;
}mm_allocator_segment_t;

typedef struct{
  vector_t* segments_free;        // Completely free segments (mm_allocator_segment_t*)
  vector_t* segment;        //segments (mm_allocator_segment_t* )
  uint64_t segment_size;
  uint64_t current_segment_idx;
}mm_allocator_t;

// Segments

mm_allocator_segment_t* mm_allocator_new(mm_allocator_t* const mm_allocator){
  // Allocate handler
  mm_allocator_segment_t* const segment = (mm_allocator_segment_t*)malloc(sizeof(mm_allocator_segment_t));
  // Index
  const uint64_t segment_idx = vector_get_used(mm_allocator->segments);
  segment->idx = segment_idx;
  // Memory
  segment->memory = malloc(mm_allocator->segment_size);
  // Add to segments
  vector_insert(mm_allocator->segments,segment,mm_allocator_segment_t*);
  // Return 
  return segment;
}

void mm_allocator_segment_clear(mm_allocator_segment_t* const segment){
  segment->used = 0;
}

void mm_allocator_segment_delete(mm_allocator_segment_t* const segment){
  free(segment->memory);
  free(segment);
}

// Allocators
mm_allocator_t* mm_allocator_new(const uint64_t segment_size){
  // Allocate handler
  mm_allocator_t* const mm_allocator = (mm_allocator_t*) malloc(sizeof(mm_allocator_t));
  // Segments
  mm_allocator->segment_size = segment_size;
  mm_allocator->segments = vector_new(MM_ALLOCATOR_INITIAL_SEGMENTS, mm_allocator_segment_t);
  mm_allocator->segments_free = vector_new(MM_ALLOCATOR_INITIAL_SEGMENTS,mm_allocator_segment_t*);
  // Allocate an initial segment
  mm_allocator_segment_new(mm_allocator);
  // Return 
  return mm_allocator;
}

void mm_allocator_clear(mm_allocator_t* const mm_allocator){
  // Clear segments
  vector_clear(mm_allocator->segments_free);
  VECTOR_ITERATE(mm_allocator->segments,segment_ptr,p,mm_allocator_segment_t*) {
    mm_allocator_segment_clear(*segment_ptr); // Clear segment
    vector_insert(mm_allocator->segments_free,*segment_ptr,mm_allocator_segment_t*); // Add to free segments
  }
  mm_allocator->current_segment_idx = 0;
}

void mm_allocator_delete(mm_allocator_t* const mm_allocator){
  // Free segments
  VECTOR_ITERATE(mm_allocator->segments, segment_ptr,p,mm_allocator_segment_t*){
    mm_allocator_segment_delete(*segment_ptr);
  }
  vector_delete(mm_allocator->segments;)
  vector_delete(mm_allocator->segments_free);
  // Free handler
  free(mm_allocator);
}

// Accessors
mm_allocator_segment_t* mm_allocator_get_segment(mm_allocator_t* const mm_allocator, const uint64_t segment_idx){
  return *(vector_get_elm(mm_allocator->segments,segment_idx,mm_allocator_segment_t*));
}

mm_allocator_segment_t* mm_allocator_get_segment_free(mm_allocator_t* const mm_allocator, const uint64_t segment_idx){
  return *(vector_get_elm(mm_allocator->segments_free,segment_idx,mm_allocator_segment_t*))
}

uint64_t mm_allocator_get_num_segments(mm_allocator_t* const mm_allocator){
  return vector_get_used(mm_allocator->segment);
}

// Allocate
mm_allocator_segment_t* mm_allocator_fetch_segment(mm_allocator_t* const mm_allocator, const uint64_t num_bytes){
  // Fetch current segment
  mm_allocator_segment_t* const curr_segment = mm_allocator_get_segment(mm_allocator, mm_allocator->current_segment_idx);
  // Check overall segment size
  /*
  if (num_bytes > curr_segment->size/2) { // Never buy anything you cannot afford twice
    return NULL; // Memory request over max-request size
  }
  */
 if (num_bytes > curr_segment->size) {
    return NULL; // Memory request over segment size
  }
 if (curr_segment->used + num_bytes <= curr_segment->size) {
    return curr_segment;
  }
  // Get free segment
  const uint64_t free_segments = mm_allocator_get_num_segments_free(mm_allocator);
  if (free_segments > 0) {
    mm_allocator_segment_t* const segment = mm_allocator_get_segment_free(mm_allocator,freesegments-1);
    vector_dec_used(mm_allocator->segments_free);
    mm_allocator->current_segment_idx = segment->idx;
    return segment;
  }
  // Allocate new segment
  mm_allocator_segment_t* const segment = mm_allocator_segment_new(mm_allocator);
  mm_allocator->current_segment_idx = segment->idx;
  return segment;
}

void* mm_allocator_allocate(mm_allocator_t* const mm_allocator, const uint64_t num_bytes, const bool zero_mem, const uint64_t align_bytes){
  #ifdef MM_ALLOCATOR_DISABLE
  void* memory = calloc(1,num_bytes);
  if (zero_mem) memset(memory,0,num_bytes);   //Set zero
  return mrmory;  //TODO: alignment
  #else
  //Zero check
  if (num_bytes == 0) {
    fprintf(stderr,"MMAllocator error. Zero bytes requested\n");
    exit(1);
  }
  // Add payload
  const uint64_t num_bytes_allocated = num_bytes + align_bytes;
  #ifdef MM_ALLOCATOR_FORCE_MALLOC
  mm_allocator_segment_t* const segment = NULL;   //Force malloc memory
  #else
  mm_allocator_segment_t* const segment = mm_allocator_fetch_segment(mm_allocator, num_bytes_allocated);
  if (segment != NULL){
    // Allocate memory
    void* const memory_base = segment->memory + segment->used;
    if (zero_mem) memset(memory_base,0,num_bytes_allocated);    //Set zero
    // Compute aligned memory
    void* memory_aligned = memory_base + align_bytes;
    
  }
}

