#ifndef LINEAR_WAVEFRONT_H_
#define LINEAR_WAVEFRONT_H_

#include "../utils/commons.h"
#include "../utils/graph_struct.h"
#include "../system/mm_allocator.h"
#include "../linear_gap/linear_wavefront_penalty.h"

/*
Constants
*/
#define LINEAR_WAVEFRONT_K_NULL (INT_MAX/2)

/*
Translate k and offset to coordinates h,i
*/
#define LINEAR_WAVEFRONT_I(k,offset) (offset)
#define LINEAR_WAVEFRONT_H(k,offset) ((offset)+(k))

#define LINEAR_WAVEFRONT_DIAGONAL(h,i) ((h)-(i))
#define LINEAR_WAVEFRONT_OFFSET(h,i) (i)

/*
Offset size
*/
//#define LINEAR_WAVEFRONT_W8
//#define LINEAR_WAVEFRONT_W16
#define LINEAR_WAVEFRONT_W32

#ifdef LINEAR_WAVEFRONT_W8
  typedef int8_t awf_offset_t;
  #define LINEAR_WAVEFRONT_OFFSET_NULL (INT8_MIN/2)
#else
  #ifdef LINEAR_WAVEFRONT_W16
    typedef int16_t awf_offset_t;
    #define LINEAR_WAVEFRONT_OFFSET_NULL (INT16_MIN/2)
  #else // AFFINE_WAVEFRONT_W32
    typedef int32_t awf_offset_t;
    #define LINEAR_WAVEFRONT_OFFSET_NULL (INT32_MIN/2)
  #endif
#endif

/*
Diagonal_Index
*/

typedef struct{
  int diagonal_index;
  int segment_index;
  awf_offset_t offset;
} linear_wavefront_index_t;

typedef struct{
  int idx_num;
  linear_wavefront_index_t* idx;
} linear_wavefront_indexs_t;

/*
typedef struct{
  linear_wavefront_index_t* idx_buffer;
  linear_wavefront_indexs_t* index;
} linear_wavefront_node_indexs_t;
*/

typedef struct{
  int hi,lo;
  awf_offset_t* offsets;
} linear_wavefront_t;

typedef struct{
  // Dimensions
  graph_t* graph;   // Graph pattern
  int text_length;    // Text length
  // Limits
  int max_penalty;    // MAX(mismatch_penalty,single_gap_penalty)
  // Wavefront
  linear_wavefront_t** wavefronts;
  // Penalties
  linear_wavefront_penalty_t penalties;
  // Index set for extend
  linear_wavefront_indexs_t idx_set;
  // Nodes in computation
  int* node;
  int node_num;
  // Finalization
  bool final_status;
  // Position table
  int** visit;
} linear_wavefronts_t;

#endif