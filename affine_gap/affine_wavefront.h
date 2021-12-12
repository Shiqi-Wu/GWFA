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
    int score_stamp;
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
    vector_t* index_set;            //index sets
    affine_wavefront_t*** mwavefronts;           //M-wavefronts
    affine_wavefront_t** iwavefronts;           //I-wavefronts
    affine_wavefront_t** dwavefronts;           //D-wavefronts
    affine_wavefronts_penalties_t penalties;        //Penalties parameters
    mm_allocator_t* mm_allocator;
    int num_wavefronts;
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


#endif