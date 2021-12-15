#include "affine_wavefront_align.h"

/*
Fetch & allocate wavefronts
*/
void affine_wavefronts_fetch_wavefronts(
    affine_wavefronts_t* const affine_wavefronts,
    affine_wavefront_set* const wavefront_set,
    const int score) {
        // Compute scores
        const affine_wavefront_penalties_t* const wavefront_penalties = & (affine_wavefronts->penalties);
        const int mismatch_score = score - wavefront_penalties->mismatch;
        const int gap_open_score = score - wavefront_penalties->gap_opening - wavefront_penalties->gap_extension;
        const int gap_extend_score = score - wavefront_penalties->gap_extension;
        // Fetch wavefronts
        wavefront_set->in_mwavefront_sub = affine_wavefronts_get_source_mwavefront(affine_wavefronts,mismatch_score);
        wavefront_set->in_mwavefront_gap = affine_wavefronts_get_source_mwavefront(affine_wavefronts,gap_open_score);
        wavefront_set->in_iwavefront_ext = affine_wavefronts_get_source_iwavefront(affine_wavefronts,gap_extend_score);
        wavefront_set->in_dwavefront_ext = affine_wavefronts_get_source_dwavefront(affine_wavefronts,gap_extend_score);
    }

void affine_wavefronts_allocate_wavefronts(
    affine_wavefronts_t* const affine_wavefronts,
    affine_wavefront_set* const wavefront_set,
    const int score)
    {
        // Allocate M-Wavefront
        wavefront_set->out_mwavefront = affine_wavefronts_allocate_wavefront(affine_wavefronts,affine_wavefronts->index_set_num);
        affine_wavefronts->mwavefronts[score] = wavefront_set->out_mwavefront;
        // Allocate I-Wavefront
        if (!wavefront_set->in_mwavefront_gap->null || !wavefront_set->in_iwavefront_ext->null) {
            affine_wavefront_allocate_wavefront(affine_wavefronts,affine_wavefronts->index_set_num);
            affine_wavefronts->iwavefronts[score] = wavefront_set->out_iwavefront;
        } else {
            wavefront_set->out_iwavefront = NULL;
        }
        // Allocate D-Wavefront
        if (!wavefront_set->in_mwavefront_gap->null || !wavefront_set->in_dwavefront_ext->null) {
            wavefront_set->out_dwavefront = affine_wavefronts_allocate_wavefront(affine_wavefronts,affine_wavefronts->index_set_num);
            affine_wavefronts->dwavefronts[score]=wavefront_set->out_dwavefront;
        } else {
            wavefront_set->out_dwavefront = NULL;
         }
    }

/* 
Fetch Full indexes
*/
void affine_wavefront_fetch_index(affine_wavefronts_t* affine_wavefronts, int score, affine_wavefront_index_set* indexes_set)
{
    if (score >= affine_wavefronts->penalties.mismatch)
    {
        
    }
}


/*
Compute wavefront offsets
*/
