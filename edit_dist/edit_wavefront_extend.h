// DESCRIPTION: Wavefront extend on unit cost

#include "edit_dist/edit_wavefront.h"

#ifndef EDIT_WAVEFRONT_EXTEND_H_
#define EDIT_WAVEFRONT_EXTEND_H_
/*
 * Constants
 */
#define AFFINE_WAVEFRONT_PADDING  10 // (-AFFINE_WAVEFRONT_OFFSET_NULL)

// Edit distance wavefront extension
void edit_wavefront_extend(
    edit_wavefronts_t* const edit_wavefronts,
    const char* const text,
    const int alignment_v, 
    const int alignment_k);

#endif