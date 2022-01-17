
#include "affine_wavefront_utils.h"

/*
Accessors
*/
affine_wavefront_t* affine_wavefronts_get_source_mwavefront(
    affine_wavefronts_t* const affine_wavefronts, const int score){
        return (score < 0 || affine_wavefronts->mwavefronts[score] == NULL)? &affine_wavefronts->wavefront_null : affine_wavefronts->mwavefronts[score];
    }

affine_wavefront_t* affine_wavefronts_get_source_iwavefront(
    affine_wavefronts_t* const affine_wavefronts, const int score){
        return (score < 0 || affine_wavefronts->iwavefronts[score] == NULL)? &affine_wavefronts->wavefront_null : affine_wavefronts->iwavefronts[score];
    }

affine_wavefront_t* affine_wavefronts_get_source_dwavefront(
    affine_wavefronts_t* const affine_wavefronts, const int score){
        return (score < 0 || affine_wavefronts->dwavefronts[score] == NULL)? &affine_wavefronts->wavefront_null : affine_wavefronts->dwavefronts[score];
    }

affine_wavefront_indexs_t* affine_wavefronts_get_source_mindex(
    affine_wavefronts_t* const affine_wavefronts, const int score){
        return (score < 0 || affine_wavefronts->mindex_set[score] == NULL)? NULL : affine_wavefronts->mindex_set[score];
    }

affine_wavefront_indexs_t* affine_wavefronts_get_source_iindex(
    affine_wavefronts_t* const affine_wavefronts, const int score){
        return (score < 0 || affine_wavefronts->iindex_set[score] == NULL)? NULL : affine_wavefronts->iindex_set[score];
    }

affine_wavefront_indexs_t* affine_wavefronts_get_source_dindex(
    affine_wavefronts_t* const affine_wavefronts, const int score){
        return (score < 0 || affine_wavefronts->dindex_set[score] == NULL)? NULL : affine_wavefronts->dindex_set[score];
    }

affine_wavefront_indexs_t* affine_wavefronts_get_source_iindex_gap(
    affine_wavefronts_t* const affine_wavefronts, const int score){
        return (score < 0 || affine_wavefronts->iindex_set_gap[score] == NULL)? NULL : affine_wavefronts->iindex_set_gap[score];
    }

affine_wavefront_indexs_t* affine_wavefronts_get_source_dindex_gap(
    affine_wavefronts_t* const affine_wavefronts, const int score){
        return (score < 0 || affine_wavefronts->dindex_set_gap[score] == NULL)? NULL : affine_wavefronts->dindex_set_gap[score];
    }

/*
Initial Conditions and finalization
*/
void affine_wavefront_initialize(
    affine_wavefronts_t* const affine_wavefronts){
        affine_wavefronts->mwavefronts[0] = affine_wavefronts_allocate_wavefront(affine_wavefronts,1);
        affine_wavefronts->mwavefronts[0]->offset_num = 1;
        affine_wavefronts->mwavefronts[0]->offsets[0] = 0;
    }

bool affine_wavefront_end_reached(
    affine_wavefronts_t* const affine_wavefronts){
        return affine_wavefronts->final_status;
    }