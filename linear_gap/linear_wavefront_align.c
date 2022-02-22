#include "linear_wavefront_align.h"

// Fetch wavefronts

linear_wavefront_t* linear_wavefront_get_source_wavefront(
    linear_wavefronts_t* linear_wavefronts, int score)
    {
        if (score >= 0)
            return linear_wavefronts->wavefronts[score];
        else
            return NULL;
    }

void linear_wavefronts_fetch_wavefronts(
    linear_wavefronts_t* const linear_wavefronts,
    linear_wavefront_set* const wavefront_set,
    const int score) {
        // Compute score
        const linear_wavefront_penalty_t* const wavefront_penalties = &(linear_wavefronts->penalties);
        const int mismatch_score = score - wavefront_penalties->mismatch;
        const int gap_score = score - wavefront_penalties->gap;
        // Fetch wavefronts
        wavefront_set->in_gwavefront = linear_wavefront_get_source_wavefront(linear_wavefronts, gap_score);
        wavefront_set->in_mwavefront = linear_wavefront_get_source_wavefront(linear_wavefronts, mismatch_score);
    }

void linear_wavefronts_initialize_wavefronts(
    linear_wavefronts_t* const linear_wavefronts,
    linear_wavefront_set* const wavefront_set,
    const int score){
        if (wavefront_set->in_mwavefront==NULL && wavefront_set->in_gwavefront==NULL){
            wavefront_set->out_wavefront = NULL;
            return;
        }
        wavefront_set->out_wavefront = linear_wavefront_get_source_wavefront(linear_wavefronts, score);
        int count = 0; int node_idx, kernel;
        awf_offset_t lo_offset, hi_offset;
        for (int n = 0; n < linear_wavefronts->node_num; n++)
        {
            node_idx = linear_wavefronts->node[n];
            kernel = (wavefront_set->in_mwavefront[node_idx].null<<1)|(wavefront_set->in_gwavefront[node_idx].null);
            switch (kernel)
            {
                case 0:{
                    lo_offset = wavefront_set->in_gwavefront[node_idx].offsets[transp(wavefront_set->in_gwavefront[node_idx].lo)];
                    hi_offset = wavefront_set->in_gwavefront[node_idx].offsets[transp(wavefront_set->in_gwavefront[node_idx].hi)];
                    lo_offset > 0? wavefront_set->out_wavefront[node_idx].lo = MIN(wavefront_set->in_mwavefront[node_idx].lo, wavefront_set->in_gwavefront[node_idx].lo - 1):wavefront_set->out_wavefront[node_idx].lo = MIN(wavefront_set->in_mwavefront[node_idx].lo, wavefront_set->in_gwavefront[node_idx].lo);
                    hi_offset > 0? wavefront_set->out_wavefront[node_idx].hi = MAX(wavefront_set->in_mwavefront[node_idx].hi, wavefront_set->in_gwavefront[node_idx].hi + 1):wavefront_set->out_wavefront[node_idx].hi = MAX(wavefront_set->in_mwavefront[node_idx].hi, wavefront_set->in_gwavefront[node_idx].hi - 1);
                    wavefront_set->out_wavefront[node_idx].null = false;
                }
                case 1:{
                    wavefront_set->out_wavefront[node_idx].lo = wavefront_set->in_mwavefront[node_idx].lo;
                    wavefront_set->out_wavefront[node_idx].hi = wavefront_set->in_mwavefront[node_idx].hi;
                    wavefront_set->out_wavefront[node_idx].null = false;
                }
                case 2:{
                    lo_offset = wavefront_set->in_gwavefront[node_idx].offsets[transp(wavefront_set->in_gwavefront[node_idx].lo)];
                    hi_offset = wavefront_set->in_gwavefront[node_idx].offsets[transp(wavefront_set->in_gwavefront[node_idx].hi)];
                    lo_offset>=0? wavefront_set->out_wavefront[node_idx].lo = wavefront_set->in_gwavefront[node_idx].lo - 1:wavefront_set->out_wavefront[node_idx].lo = wavefront_set->in_gwavefront[node_idx].lo;
                    hi_offset>=0? wavefront_set->out_wavefront[node_idx].hi = wavefront_set->in_gwavefront[node_idx].hi + 1:wavefront_set->out_wavefront[node_idx].hi = wavefront_set->in_gwavefront[node_idx].hi - 1;
                    wavefront_set->out_wavefront[node_idx].null = false;
                }
                case 3:{
                    count++;
                }
            }
        }
        if (count == linear_wavefronts->node_num)
            wavefront_set->out_wavefront = NULL;
    }

void linear_wavefronts_compute_offsets(
    linear_wavefronts_t* const linear_wavefronts,
    linear_wavefront_set* const wavefront_set,
    int score){
        if (wavefront_set->out_wavefront == NULL)
            return;
        int node_idx, kernel, k, p;
        for (int n = 0; n < linear_wavefronts->node_num; n++){
            node_idx = linear_wavefronts->node[n];
            if (wavefront_set->out_wavefront[node_idx].null)
                continue;
            kernel = (wavefront_set->in_mwavefront[node_idx].null<<1)|(wavefront_set->in_gwavefront[node_idx].null);
            switch (kernel)
            {
                case 0:{
                    for (k = wavefront_set->in_mwavefront[node_idx].lo; k <= wavefront_set->in_mwavefront[node_idx].hi; k++){
                        p = transp(k);
                        if (wavefront_set->in_mwavefront[node_idx].offset[p]>=0)
                            wavefront_set->out_wavefront[node_idx].offset[p] = wavefront_set->in_mwavefront[node_idx].offset[p] + 1;
                    }
                    for (k = wavefront_set->in_gwavefront[node_idx].lo; k <= wavefront_set->in_gwavefront[node_idx].hi; k++1)
                    {
                        p = transp(k);
                        if (wavefront_set->in_gwavefront[node_idx].offset[p] > 0)
                        {
                            wavefront_set->out_wavefront[node_idx].offset[transp(k - 1)] = MAX(wavefront_set->out_wavefront[node_idx].offset[p] + 1,wavefront_set->out_wavefront[node_idx].offset[transp(k - 1)]);
                            wavefront_set->out_wavefront[node_idx].offset[transp(k + 1)] = MAX(wavefront_set->out_wavefront[node_idx].offset[p],wavefront_set->out_wavefront[node_idx].offset[transp(k + 1)]);
                        }
                        else if (wavefront_set->in_gwavefront[node_idx].offset[p] == 0)
                        {
                            wavefront_set->out_wavefront[node_idx].offset[transp(k - 1)] = MAX(wavefront_set->out_wavefront[node_idx].offset[p] + 1,wavefront_set->out_wavefront[node_idx].offset[transp(k - 1)]);
                        }
                    }
                }
                case 1:{
                    for (k = wavefront_set->out_wavefront[node_idx].lo; k <= wavefront_set->out_wavefront[node_idx].hi; k++){
                        p = transp(k);
                        if (wavefront_set->in_mwavefront[node_idx].offset[p]>=0)
                            wavefront_set->out_wavefront[node_idx].offset[p] = wavefront_set->in_mwavefront[node_idx].offset[p] + 1;
                    }
                }
                case 2:{
                    for (k = wavefront_set->in_gwavefront[node_idx].lo; k <= wavefront_set->in_gwavefront[node_idx].hi; k++1)
                    {
                        p = transp(k);
                        if (wavefront_set->in_gwavefront[node_idx].offset[p] > 0)
                        {
                            wavefront_set->out_wavefront[node_idx].offset[transp(k - 1)] = MAX(wavefront_set->out_wavefront[node_idx].offset[p] + 1,wavefront_set->out_wavefront[node_idx].offset[transp(k - 1)]);
                            wavefront_set->out_wavefront[node_idx].offset[transp(k + 1)] = MAX(wavefront_set->out_wavefront[node_idx].offset[p],wavefront_set->out_wavefront[node_idx].offset[transp(k + 1)]);
                        }
                        else if (wavefront_set->in_gwavefront[node_idx].offset[p] == 0)
                        {
                            wavefront_set->out_wavefront[node_idx].offset[transp(k - 1)] = MAX(wavefront_set->out_wavefront[node_idx].offset[p] + 1,wavefront_set->out_wavefront[node_idx].offset[transp(k - 1)]);
                        }
                    }
                }
            }
        }
    }