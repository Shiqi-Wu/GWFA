// DESCRIPTION: WFA extend exact-matches component

#ifndef AFFINE_WAVEFRONT_EXTEND_H_
#define AFFINE_WAVEFRONT_EXTEND_H_

#include "affine_wavefront.h"

/*
 * Constants
 */
#define AFFINE_WAVEFRONT_PADDING  10 // (-AFFINE_WAVEFRONT_OFFSET_NULL)

/*
 * Gap-Affine Wavefront exact extension
 */

void affine_wavefronts_extend_mwavefront_compute_packed(
    affine_wavefronts_t* const affine_wavefronts,
    const char* const text, const int score, 
    const int alignment_v, const int alignment_k);

#endif /* AFFINE_WAVEFRONT_EXTEND_H_ */