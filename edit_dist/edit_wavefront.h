// DESCRIPTION: Graph Wavefront alignment algorithm for unit cost

#include "../utils/commons.h"
#include "../utils/graph_struct.h"
#include "../system/mm_allocator.h"

#ifndef EDIT_WAVEFRONT_
#define EDIT_WAVEFRONT_

// Translate k and offset to coordinates h,i
#define EDIT_WAVEFRONT_I(k,offset) (offset)
#define EDIT_WAVEFRONT_H(k,offset) ((offset)+(k))

#define EDIT_WAVEFRONT_DIAGONAL(h,i) ((h)-(i))
#define EDIT_WAVEFRONT_OFFSET(h,i) (i)

// Offset size
#define AFFINE_WAVEFRONT_W32

#ifdef AFFINE_WAVEFRONT_W8
  typedef int8_t awf_offset_t;
  #define AFFINE_WAVEFRONT_OFFSET_NULL (INT8_MIN/2)
#else
  #ifdef AFFINE_WAVEFRONT_W16
    typedef int16_t awf_offset_t;
    #define AFFINE_WAVEFRONT_OFFSET_NULL (INT16_MIN/2)
  #else // AFFINE_WAVEFRONT_W32
    typedef int32_t awf_offset_t;
    #define AFFINE_WAVEFRONT_OFFSET_NULL (INT32_MIN/2)
  #endif
#endif

// Diagonal idx
typedef struct{
    int diagonal_index;
    int segment_index;
    awf_offset_t offset;
    bool full;
} edit_wavefront_index_t;

// Wavefronts
typedef struct{
    // Dimentions
    graph_t* graph;     //Graph pattern
    int text_length;        //Text length
    // Index set
    edit_wavefront_index_t* index_set;
    edit_wavefront_index_t* index_set_null;
    int index_set_num;
    // Finalization
    bool final_status;
    // Visit
    bool** visit;
    // Allocate
    mm_allocator_t* mm_allocator;
} edit_wavefronts_t;

edit_wavefronts_t* edit_wavefronts_new(
    const int pattern_length,
    const int text_length,
    mm_allocator_t* const mm_allocator,
    graph_t* graph);

void edit_wavefronts_clear(edit_wavefronts_t* const edit_wavefronts);

void edit_wavefronts_delete(edit_wavefronts_t* const edit_wavefronts);

bool edit_sort_index(edit_wavefront_index_t a, edit_wavefront_index_t b);

bool edit_unique_index(edit_wavefront_index_t a, edit_wavefront_index_t b);

void edit_sort(edit_wavefront_index_t* begin, edit_wavefront_index_t* end);

edit_wavefront_index_t* edit_unique(edit_wavefront_index_t* first, edit_wavefront_index_t* last);

// Add Index
#define EDIT_ADD_INDEX(index_set, index_num, k_num, v_num, offset_num, full_num) index_set[index_num].diagonal_index = k_num; index_set[index_num].segment_index = v_num; index_set[index_num].offset = offset_num; index_set[index_num].full = full_num;index_num++;

#endif