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
void affine_wavefront_fetch_index(affine_wavefronts_t* affine_wavefronts, int score, affine_wavefront_index_set* index_set)
{
    if (score >= affine_wavefronts->penalties.mismatch)
    {
        int sub_position = (score - affine_wavefronts->penalties.mismatch) % affine_wavefronts->full_index_size;
        index_set->in_mindex_sub = affine_wavefronts->full_mindex[sub_position];
        index_set->in_mindex_sub_num = affine_wavefronts->full_mindex_num[sub_position];
    }
    else
    {
        index_set->in_mindex_sub = NULL;
    }
    if (score >= affine_wavefronts->penalties.gap_opening + affine_wavefronts->penalties.gap_extension)
    {
        int gap_position = (score - affine_wavefronts->penalties.gap_extension - affine_wavefronts->penalties.gap_opening) % affine_wavefronts->full_index_size;
        index_set->in_mindex_gap = affine_wavefronts->full_mindex[gap_position];
        index_set->in_mindex_gap_num = affine_wavefronts->full_mindex_num[gap_position];
    }
    else
    {
        index_set->in_mindex_gap = NULL;
    }
    if (score >= affine_wavefronts->penalties.gap_extension)
    {
        int ext_position = (score - affine_wavefronts->penalties.gap_extension) % affine_wavefronts->full_index_size;
        index_set->in_dindex_ext = affine_wavefronts->full_iindex[ext_position];
    }
    else
    {
        index_set->in_dindex_ext = NULL;
    }
    int current_position = score % affine_wavefronts->full_index_size;
    index_set->out_mindex = affine_wavefronts->full_mindex[current_position];
    index_set->out_dindex = affine_wavefronts->full_dindex[current_position];
    index_set->out_mindex_num = &affine_wavefronts->full_mindex_num[current_position];
    index_set->out_dindex_num = &affine_wavefronts->full_dindex_num[current_position];
    *(index_set->out_mindex_num) = 0;
    *(index_set->out_dindex_num) = 0;
}


/*
Compute wavefront offsets
*/
void affine_wavefronts_compute_offsets_idm(
    affine_wavefronts_t* const affine_wavefronts,
    const affine_wavefront_set* const wavefront_set,
    const affine_wavefront_index_set* index_set){
        // update D in graph structure
        if (!(index_set->in_mindex_gap == NULL))
        {
            for (int n = 0; n < index_set->in_mindex_gap_num; n++)
            {
                affine_wavefront_index_t index = index_set->in_mindex_gap[n];
                int v = index.segment_index;
                int k = index.diagonal_index;
                int i = affine_wavefronts->graph->node[v].pattern_length;
                int h = AFFINE_WAVEFRONT_H(k,i);
                for (int j = 0; j < affine_wavefronts->graph->node[v].next_num; j++)
                {
                    int w = affine_wavefronts->graph->node[v].next[j];
                    
                }
            }
        }
    } 


void affine_wavefronts_compute_offsets(
    affine_wavefronts_t* const affine_wavefronts,
    const affine_wavefront_set* const wavefront_set,
    const affine_wavefront_index_set* index_set) {
        // Check null wavefronts
        if (wavefront_set->in_mwavefront_sub == NULL &&
            wavefront_set->in_mwavefront_gap == NULL &&
            wavefront_set->in_iwavefront_ext == NULL &&
            wavefront_set->in_dwavefront_ext == NULL){
                return;
            }
    }