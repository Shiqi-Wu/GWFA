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
        index_set->in_dindex_ext = affine_wavefronts->full_dindex[ext_position];
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
    affine_wavefront_set* const wavefront_set,
    const affine_wavefront_index_set* index_set,
    int score){
        int** position_table = affine_wavefronts->position_table;
        awf_offset_t* const moffsets = wavefront_set->out_mwavefront->offsets;
        awf_offset_t* const doffsets = wavefront_set->out_dwavefront->offsets;
        awf_offset_t* const ioffsets = wavefront_set->out_iwavefront->offsets;
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
                    int w_k = AFFINE_WAVEFRONT_DIAGONAL(h+1,0);
                    int position = position_table[w][w_k];
                    moffsets[position]=0;
                    doffsets[position]=0;
                }
            }
        }
        if (!(index_set->in_dindex_ext==NULL))
        {
            for (int n = 0; n < index_set->in_dindex_ext_num; n++)
            {
                affine_wavefront_index_t index = index_set->in_dindex_ext[n];
                int v = index.segment_index;
                int k = index.diagonal_index;
                int i = affine_wavefronts->graph->node[v].pattern_length;
                int h = AFFINE_WAVEFRONT_H(k,i);
                for (int j = 0; j < affine_wavefronts->graph->node[v].next_num; j++)
                {
                    int w = affine_wavefronts->graph->node[v].next[j];
                    int w_k = AFFINE_WAVEFRONT_DIAGONAL(h,0);
                    int position = position_table[w][w_k];
                    doffsets[position]=0;
                }
            }
        }
        if (!(index_set->in_mindex_sub==NULL))
        {
            for (int n = 0; n < index_set->in_mindex_sub_num; n++)
            {
                affine_wavefront_index_t index = index_set->in_mindex_sub[n];
                int v = index.segment_index;
                int k = index.diagonal_index;
                int i = affine_wavefronts->graph->node[v].pattern_length;
                int h = AFFINE_WAVEFRONT_H(k,i);
                for (int j = 0; j < affine_wavefronts->graph->node[v].next_num; j++)
                {
                    int w = affine_wavefronts->graph->node[v].next[j];
                    int w_k = AFFINE_WAVEFRONT_DIAGONAL(h,0);
                    int position = position_table[w][w_k];
                    moffsets[position]=0;
                }
            }
        }
        for (int n = 0; n < affine_wavefronts->index_set_num; n++)
        {
            affine_wavefront_index_t index = affine_wavefronts->index_set[n];
            int v = index.segment_index;
            int k = index.diagonal_index;
            if (index.full)
                continue;
            if (n >= 1 && affine_wavefronts->index_set[n-1].segment_index == v && affine_wavefronts->index_set[n-1].diagonal_index == k - 1)
            {
                switch ( (wavefront_set->in_iwavefront_ext!=NULL && wavefront_set->in_iwavefront_ext->offset_num > affine_wavefronts->index_set[n-1].storage_position)<<1 |(wavefront_set->in_mwavefront_gap!=NULL && wavefront_set->in_mwavefront_gap->offset_num > affine_wavefronts->index_set[n-1].storage_position))
                {
                    case 0: break;
                    case 1:
                    {
                        ioffsets[index.storage_position] = wavefront_set->in_mwavefront_gap->offsets[affine_wavefronts->index_set[n-1].storage_position];
                        break;
                    }
                    case 2:
                    {
                        ioffsets[index.storage_position] = wavefront_set->in_iwavefront_ext->offsets[affine_wavefronts->index_set[n-1].storage_position];
                        break;
                    }
                    case 3:
                    {
                        ioffsets[index.storage_position] = MAX(wavefront_set->in_iwavefront_ext->offsets[affine_wavefronts->index_set[n-1].storage_position], wavefront_set->in_mwavefront_sub->offsets[affine_wavefronts->index_set[n-1].storage_position]);
                        break;
                    }
                }
            }
            if (n <  affine_wavefronts->index_set_num - 1 && affine_wavefronts->index_set[n + 1].segment_index == v && affine_wavefronts->index_set[n + 1].diagonal_index == k + 1)
            {
                switch ( (wavefront_set->in_dwavefront_ext != NULL && wavefront_set->in_dwavefront_ext->offset_num > affine_wavefronts->index_set[n+1].storage_position)<<1 |(wavefront_set->in_mwavefront_gap != NULL && wavefront_set->in_mwavefront_gap->offset_num > affine_wavefronts->index_set[n+1].storage_position))
                {
                    case 0: break;
                    case 1:
                    {
                        doffsets[index.storage_position] = MAX(wavefront_set->in_mwavefront_gap->offsets[affine_wavefronts->index_set[n+1].storage_position], doffsets[index.storage_position]);
                        break;
                    }
                    case 2:
                    {
                        doffsets[index.storage_position] = MAX(wavefront_set->in_dwavefront_ext->offsets[affine_wavefronts->index_set[n+1].storage_position], doffsets[index.storage_position]);
                        break;
                    }
                    case 3:
                    {
                        doffsets[index.storage_position] = MAX(MAX(wavefront_set->in_dwavefront_ext->offsets[affine_wavefronts->index_set[n+1].storage_position], wavefront_set->in_mwavefront_sub->offsets[affine_wavefronts->index_set[n+1].storage_position]),doffsets[index.storage_position]);
                        break;
                    }
                }
            }
            if (wavefront_set->in_mwavefront_sub!= NULL && index.storage_position < wavefront_set->in_mwavefront_sub->offset_num)
            {
                moffsets[index.storage_position] = MAX(MAX(MAX(wavefront_set->in_mwavefront_sub->offsets[index.storage_position], moffsets[index.storage_position]),doffsets[index.storage_position]),ioffsets[index.storage_position]);
            }
            else
            {
                moffsets[index.storage_position] = MAX(MAX(moffsets[index.storage_position],doffsets[index.storage_position]),ioffsets[index.storage_position]);
            }
        }
        affine_wavefronts->mwavefronts[score]->offsets=moffsets;
        affine_wavefronts->dwavefronts[score]->offsets=doffsets;
        affine_wavefronts->iwavefronts[score]->offsets=ioffsets;
        affine_wavefronts->mwavefronts[score]->offset_num=affine_wavefronts->index_set_num;
        affine_wavefronts->dwavefronts[score]->offset_num=affine_wavefronts->index_set_num;
        affine_wavefronts->iwavefronts[score]->offset_num=affine_wavefronts->index_set_num;
    } 

void affine_wavefronts_compute_offsets_dm(
    affine_wavefronts_t* const affine_wavefronts,
    affine_wavefront_set* const wavefront_set,
    const affine_wavefront_index_set* index_set,
    int score){
        int** position_table = affine_wavefronts->position_table;
        awf_offset_t* const moffsets = wavefront_set->out_mwavefront->offsets;
        awf_offset_t* const doffsets = wavefront_set->out_dwavefront->offsets;
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
                    int w_k = AFFINE_WAVEFRONT_DIAGONAL(h+1,0);
                    int position = position_table[w][w_k];
                    moffsets[position]=0;
                    doffsets[position]=0;
                }
            }
        }
        if (!(index_set->in_dindex_ext==NULL))
        {
            for (int n = 0; n < index_set->in_dindex_ext_num; n++)
            {
                affine_wavefront_index_t index = index_set->in_dindex_ext[n];
                int v = index.segment_index;
                int k = index.diagonal_index;
                int i = affine_wavefronts->graph->node[v].pattern_length;
                int h = AFFINE_WAVEFRONT_H(k,i);
                for (int j = 0; j < affine_wavefronts->graph->node[v].next_num; j++)
                {
                    int w = affine_wavefronts->graph->node[v].next[j];
                    int w_k = AFFINE_WAVEFRONT_DIAGONAL(h,0);
                    int position = position_table[w][w_k];
                    doffsets[position]=0;
                }
            }
        }
        if (!(index_set->in_mindex_sub==NULL))
        {
            for (int n = 0; n < index_set->in_mindex_sub_num; n++)
            {
                affine_wavefront_index_t index = index_set->in_mindex_sub[n];
                int v = index.segment_index;
                int k = index.diagonal_index;
                int i = affine_wavefronts->graph->node[v].pattern_length;
                int h = AFFINE_WAVEFRONT_H(k,i);
                for (int j = 0; j < affine_wavefronts->graph->node[v].next_num; j++)
                {
                    int w = affine_wavefronts->graph->node[v].next[j];
                    int w_k = AFFINE_WAVEFRONT_DIAGONAL(h,0);
                    int position = position_table[w][w_k];
                    moffsets[position]=0;
                }
            }
        }
        for (int n = 0; n < affine_wavefronts->index_set_num; n++)
        {
            affine_wavefront_index_t index = affine_wavefronts->index_set[n];
            int v = index.segment_index;
            int k = index.diagonal_index;
            if (index.full)
                continue;
            if (n <  affine_wavefronts->index_set_num - 1 && affine_wavefronts->index_set[n + 1].segment_index == v && affine_wavefronts->index_set[n + 1].diagonal_index == k + 1)
            {
                switch ( (wavefront_set->in_dwavefront_ext != NULL && wavefront_set->in_dwavefront_ext->offset_num > affine_wavefronts->index_set[n+1].storage_position)<<1 |(wavefront_set->in_mwavefront_gap != NULL && wavefront_set->in_mwavefront_gap->offset_num > affine_wavefronts->index_set[n+1].storage_position))
                {
                    case 0: break;
                    case 1:
                    {
                        doffsets[index.storage_position] = MAX(wavefront_set->in_mwavefront_gap->offsets[affine_wavefronts->index_set[n+1].storage_position], doffsets[index.storage_position]);
                        break;
                    }
                    case 2:
                    {
                        doffsets[index.storage_position] = MAX(wavefront_set->in_dwavefront_ext->offsets[affine_wavefronts->index_set[n+1].storage_position], doffsets[index.storage_position]);
                        break;
                    }
                    case 3:
                    {
                        doffsets[index.storage_position] = MAX(MAX(wavefront_set->in_dwavefront_ext->offsets[affine_wavefronts->index_set[n+1].storage_position], wavefront_set->in_mwavefront_sub->offsets[affine_wavefronts->index_set[n+1].storage_position]),doffsets[index.storage_position]);
                        break;
                    }
                }
            }
            if (wavefront_set->in_mwavefront_sub!= NULL && index.storage_position < wavefront_set->in_mwavefront_sub->offset_num)
            {
                moffsets[index.storage_position] = MAX(MAX(wavefront_set->in_mwavefront_sub->offsets[index.storage_position], moffsets[index.storage_position]),doffsets[index.storage_position]);
            }
            else
            {
                moffsets[index.storage_position] = MAX(moffsets[index.storage_position],doffsets[index.storage_position]);
            }
        }
        affine_wavefronts->mwavefronts[score]->offsets=moffsets;
        affine_wavefronts->dwavefronts[score]->offsets=doffsets;
        affine_wavefronts->mwavefronts[score]->offset_num=affine_wavefronts->index_set_num;
        affine_wavefronts->dwavefronts[score]->offset_num=affine_wavefronts->index_set_num;
    } 

void affine_wavefronts_compute_offsets_im(
    affine_wavefronts_t* const affine_wavefronts,
    affine_wavefront_set* const wavefront_set,
    const affine_wavefront_index_set* index_set,
    int score){
        int** position_table = affine_wavefronts->position_table;
        awf_offset_t* const moffsets = wavefront_set->out_mwavefront->offsets;
        awf_offset_t* const ioffsets = wavefront_set->out_iwavefront->offsets;
        if (!(index_set->in_mindex_sub==NULL))
        {
            for (int n = 0; n < index_set->in_mindex_sub_num; n++)
            {
                affine_wavefront_index_t index = index_set->in_mindex_sub[n];
                int v = index.segment_index;
                int k = index.diagonal_index;
                int i = affine_wavefronts->graph->node[v].pattern_length;
                int h = AFFINE_WAVEFRONT_H(k,i);
                for (int j = 0; j < affine_wavefronts->graph->node[v].next_num; j++)
                {
                    int w = affine_wavefronts->graph->node[v].next[j];
                    int w_k = AFFINE_WAVEFRONT_DIAGONAL(h,0);
                    int position = position_table[w][w_k];
                    moffsets[position]=0;
                }
            }
        }
        for (int n = 0; n < affine_wavefronts->index_set_num; n++)
        {
            affine_wavefront_index_t index = affine_wavefronts->index_set[n];
            int v = index.segment_index;
            int k = index.diagonal_index;
            if (index.full)
                continue;
            if (n >= 1 && affine_wavefronts->index_set[n-1].segment_index == v && affine_wavefronts->index_set[n-1].diagonal_index == k - 1)
            {
                switch ( (wavefront_set->in_iwavefront_ext!=NULL && wavefront_set->in_iwavefront_ext->offset_num > affine_wavefronts->index_set[n-1].storage_position)<<1 |(wavefront_set->in_mwavefront_gap!=NULL && wavefront_set->in_mwavefront_gap->offset_num > affine_wavefronts->index_set[n-1].storage_position))
                {
                    case 0: break;
                    case 1:
                    {
                        ioffsets[index.storage_position] = wavefront_set->in_mwavefront_gap->offsets[affine_wavefronts->index_set[n-1].storage_position];
                        break;
                    }
                    case 2:
                    {
                        ioffsets[index.storage_position] = wavefront_set->in_iwavefront_ext->offsets[affine_wavefronts->index_set[n-1].storage_position];
                        break;
                    }
                    case 3:
                    {
                        ioffsets[index.storage_position] = MAX(wavefront_set->in_iwavefront_ext->offsets[affine_wavefronts->index_set[n-1].storage_position], wavefront_set->in_mwavefront_sub->offsets[affine_wavefronts->index_set[n-1].storage_position]);
                        break;
                    }
                }
            }
            if (wavefront_set->in_mwavefront_sub!= NULL && index.storage_position < wavefront_set->in_mwavefront_sub->offset_num)
            {
                moffsets[index.storage_position] = MAX(MAX(wavefront_set->in_mwavefront_sub->offsets[index.storage_position], moffsets[index.storage_position]),ioffsets[index.storage_position]);
            }
            else
            {
                moffsets[index.storage_position] = MAX(moffsets[index.storage_position],ioffsets[index.storage_position]);
            }
        }
        affine_wavefronts->mwavefronts[score]->offsets=moffsets;
        affine_wavefronts->iwavefronts[score]->offsets=ioffsets;
        affine_wavefronts->mwavefronts[score]->offset_num=affine_wavefronts->index_set_num;
        affine_wavefronts->iwavefronts[score]->offset_num=affine_wavefronts->index_set_num;
    }

void affine_wavefronts_compute_offsets_m(
    affine_wavefronts_t* const affine_wavefronts,
    affine_wavefront_set* const wavefront_set,
    const affine_wavefront_index_set* index_set,
    int score){
        int** position_table = affine_wavefronts->position_table;
        awf_offset_t* const moffsets = wavefront_set->out_mwavefront->offsets;
        if (!(index_set->in_mindex_sub==NULL))
        {
            for (int n = 0; n < index_set->in_mindex_sub_num; n++)
            {
                affine_wavefront_index_t index = index_set->in_mindex_sub[n];
                int v = index.segment_index;
                int k = index.diagonal_index;
                int i = affine_wavefronts->graph->node[v].pattern_length;
                int h = AFFINE_WAVEFRONT_H(k,i);
                for (int j = 0; j < affine_wavefronts->graph->node[v].next_num; j++)
                {
                    int w = affine_wavefronts->graph->node[v].next[j];
                    int w_k = AFFINE_WAVEFRONT_DIAGONAL(h,0);
                    int position = position_table[w][w_k];
                    moffsets[position]=0;
                }
            }
        }
        for (int n = 0; n < affine_wavefronts->index_set_num; n++)
        {
            affine_wavefront_index_t index = affine_wavefronts->index_set[n];
            int v = index.segment_index;
            int k = index.diagonal_index;
            if (index.full)
                continue;
            if (wavefront_set->in_mwavefront_sub!= NULL && index.storage_position < wavefront_set->in_mwavefront_sub->offset_num)
            {
                moffsets[index.storage_position] = MAX(wavefront_set->in_mwavefront_sub->offsets[index.storage_position], moffsets[index.storage_position]);
            }
        }
        affine_wavefronts->mwavefronts[score]->offsets=moffsets;
        affine_wavefronts->mwavefronts[score]->offset_num=affine_wavefronts->index_set_num;
    } 

void affine_wavefronts_compute_offsets(
    affine_wavefronts_t* const affine_wavefronts,
    const affine_wavefront_set* const wavefront_set,
    const affine_wavefront_index_set* index_set,
    int score) {
        // Check null wavefronts
        if (wavefront_set->in_mwavefront_sub == NULL &&
            wavefront_set->in_mwavefront_gap == NULL &&
            wavefront_set->in_iwavefront_ext == NULL &&
            wavefront_set->in_dwavefront_ext == NULL){
                return;
            }
        // Allocate score-wavefronnts
        affine_wavefront_allocate_wavefront(affine_wavefronts, wavefront_set, score);
        // Compute WF
        const int kernel = ((wavefront_set->out_iwavefront!=NULL) << 1) | (wavefront_set->out_dwavefront!=NULL);
        switch (kernel) {
            case 3:
                affine_wavefronts_compute_offsets_idm(affine_wavefronts, &wavefront_set,&index_set,score);
                break;
            case 2:
                affine_wavefronts_compute_offsets_im(affine_wavefronts, &wavefront_set,&index_set,score);
                break;
            case 1:
                affine_wavefronts_compute_offsets_dm(affine_wavefronts, &wavefront_set,&index_set,score);
                break;
            case 0:
                affine_wavefronts_compute_offsets_m(affine_wavefronts, &wavefront_set,&index_set,score);
                break;
        }
    }

/*
Computation using Wavefronts
*/
void affine_wavefronts_align(
    affine_wavefronts_t* const affine_wavefronts,
    const graph_t* const graph,
    const char* const text,
    const int text_length, 
    int alignment_v){
    // Init padded strings
    // string_padded_t* const strings_padded = strings_padded_new_rhomb(graph,text,text_length,AFFINE_WAVEFRONT_PADDING,affine_wavefronts->mm_allocator);
    // Initialize wavefront
    affine_wavefront_initialize(affine_wavefronts);
    affine_wavefront_set* wavefront_set;
    affine_wavefront_set_initialize(wavefront_set);
    affine_wavefront_index_set* index_set;
    // Compute wavefronts for increasing score
    int score = 0;
    int alignment_k = AFFINE_WAVEFRONT_DIAGONAL(text_length, affine_wavefronts->graph[alignment_k].pattern_length);
   
    while (true) {
        // Exact extend s-wavefront
        affine_wavefronts_extend_mwavefront_compute_packed(affine_wavefronts,text,score, alignment_v, alignment_k);
        if (affine_wavefronts->final_status) break;
        // Update all wavefronts
        ++score; // Increase score
        affine_wavefronts_compute_wavefront(affine_wavefronts, wavefront_set, index_set, score);
    }
}