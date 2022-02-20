
#include "affine_wavefront.h"

affine_wavefront_t* affine_wavefronts_get_source_mwavefront(
    affine_wavefronts_t* const affine_wavefronts, const int score);

affine_wavefront_t* affine_wavefronts_get_source_iwavefront(
    affine_wavefronts_t* const affine_wavefronts, const int score);

affine_wavefront_t* affine_wavefronts_get_source_dwavefront(
    affine_wavefronts_t* const affine_wavefronts, const int score);

affine_wavefront_indexs_t* affine_wavefronts_get_source_mindex(
    affine_wavefronts_t* const affine_wavefronts, const int score);

affine_wavefront_indexs_t* affine_wavefronts_get_source_iindex(
    affine_wavefronts_t* const affine_wavefronts, const int score);

affine_wavefront_indexs_t* affine_wavefronts_get_source_dindex(
    affine_wavefronts_t* const affine_wavefronts, const int score);

affine_wavefront_indexs_t* affine_wavefronts_get_source_iindex_gap(
    affine_wavefronts_t* const affine_wavefronts, const int score);

affine_wavefront_indexs_t* affine_wavefronts_get_source_dindex_gap(
    affine_wavefronts_t* const affine_wavefronts, const int score);