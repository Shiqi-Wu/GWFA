// DESCRIPTION: Graph Wavefront alignment algorithm for pairwise gap-affine alignment

#ifndef AFFINE_WAVEFRONT_H_
#define AFFINE_WAVEFRONT_H_

#include "utils/commons.h"
#include "utils/graph_struct.h"
#include "system/mm_allocator.h"
#include "affine_gap/affine_wavefront_penalties.h"

/*
Debug
*/
//#define AFFINE_WAVEFRONT_DEBUG
//#define AFFINE_WAVEFRONT_STATS

/*
Constants
*/
#define AFFINE_WAVEFRONT_K_NULL (INT_MAX/2)

/*
Translate k and offset to coordinates h,i
*/
#define AFFINE_WAVEFRONT_I(k,offset) (offset)
#define AFFINE_WAVEFRONT_H(k,offset) ((offset)+(k))

#define AFFINE_WAVEFRONT_DIAGONAL(h,i) ((h)-(i))
#define AFFINE_WAVEFRONT_OFFSET(h,i) (i)

/*
Add to index set
*/
#define ADD_INDEX(index_set, num, k, v, position, full_status) \
  index_set[num].diagonal_index=k;\
  index_set[num].segment_index=v;\
  index_set[num].storage_position=position;\
  index_set[num].full=full_status;\
  num++;

/*
Offset size
*/
//#define AFFINE_WAVEFRONT_W8
//#define AFFINE_WAVEFRONT_W16
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

/*
Diagonal_Index
*/
typedef struct{
    int diagonal_index;
    int segment_index;
    int storage_position;
    bool full;
} affine_wavefront_index_t;

typedef struct{
    bool null;
    int offset_num;
    awf_offset_t* offsets;
} affine_wavefront_t;

typedef struct{
    //Dimensions
    graph_t* graph;         //Graph pattern
    int text_length;        //Text length
    //Limits
    int max_penalty;        //MAX(mismatch_penalty, single_gap_penalty)
    affine_wavefront_index_t* index_set;            //index sets
    affine_wavefront_index_t* index_set_null;
    int index_set_num;
    int index_storage_num;
    affine_wavefront_t** mwavefronts;           //M-wavefronts
    affine_wavefront_t** iwavefronts;           //I-wavefronts
    affine_wavefront_t** dwavefronts;          //D-wavefronts
    affine_wavefront_t wavefront_null;
    affine_wavefront_t* wavefronts_mem;
    affine_wavefront_t* wavefronts_current;
    affine_wavefront_penalties_t penalties;      //Penalties parameters
    mm_allocator_t* mm_allocator;
    int num_wavefronts;
    // finalization
    bool final_status;
    // Hash_table
    int full_index_size;
    affine_wavefront_index_t** full_index;
    int* full_index_num;
} affine_wavefronts_t;

/*
SWF Wavefront Computation Set
*/
typedef struct{
    // In Wavefronts
    affine_wavefront_t* in_mwavefront_sub;
    affine_wavefront_t* in_mwavefront_gap;
    affine_wavefront_t* in_iwavefront_ext;
    affine_wavefront_t* in_dwavefront_ext;
    // Out Wavefronts
    affine_wavefront_t* out_mwavefront;
    affine_wavefront_t* out_iwavefront;
    affine_wavefront_t* out_dwavefront;
} affine_wavefront_set;

/*
Full index set
*/
typedef struct{
  affine_wavefront_index_t* in_index_gap;
  int in_index_gap_num;
  affine_wavefront_index_t* in_index_ext;
  int in_index_ext_num;
  affine_wavefront_index_t* in_index_sub;
  int in_index_sub_num;
}affine_wavefront_index_set;


void affine_wavefronts_allocate_wavefront_null(affine_wavefronts_t* const affine_wavefronts);

void affine_wavefront_allocate_wavefront_components(affine_wavefronts_t* const affine_wavefronts);

affine_wavefronts_t* affine_wavefronts_new(
    const int pattern_length,
    const int text_length,
    affine_wavefront_penalties_t* const penalties,
    mm_allocator_t* const mm_allocator);

void affine_wavefronts_clear(affine_wavefronts_t* const affine_wavefronts);

void affine_wavefronts_delete(affine_wavefronts_t* const affine_wavefronts);

affine_wavefront_t* affine_wavefronts_allocate_wavefront(affine_wavefronts_t* const affine_wavefronts,const int index_num);

/*
Sort index
*/
bool sort_index(affine_wavefront_index_t a, affine_wavefront_index_t b)
{
    if (a.segment_index<b.segment_index)
      return true;
    else if (a.segment_index==b.segment_index && a.diagonal_index<b.diagonal_index)
      return true;
    else if (a.segment_index==b.segment_index && a.diagonal_index==b.diagonal_index && a.storage_position<=b.storage_position)
      return true;
    else
      return false;
}

bool unique_index(affine_wavefront_index_t a, affine_wavefront_index_t b)
{
  return(a.segment_index==b.segment_index && a.diagonal_index==b.diagonal_index);
}

#endif