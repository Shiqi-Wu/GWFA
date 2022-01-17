// DESCRIPTION: Graph Wavefront alignment algorithm for pairwise gap-affine alignment

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
        wavefront_set->out_mwavefront = affine_wavefronts_allocate_wavefront(affine_wavefronts,2*affine_wavefronts->index_storage_num);
        affine_wavefronts->mwavefronts[score] = wavefront_set->out_mwavefront;
        // Allocate I-Wavefront
        if (!wavefront_set->in_mwavefront_gap->null || !wavefront_set->in_iwavefront_ext->null) {
            affine_wavefront_allocate_wavefront(affine_wavefronts,affine_wavefronts->index_storage_num);
            affine_wavefronts->iwavefronts[score] = wavefront_set->out_iwavefront;
        } else {
            wavefront_set->out_iwavefront = NULL;
        }
        // Allocate D-Wavefront
        if (!wavefront_set->in_mwavefront_gap->null || !wavefront_set->in_dwavefront_ext->null) {
            wavefront_set->out_dwavefront = affine_wavefronts_allocate_wavefront(affine_wavefronts,affine_wavefronts->index_storage_num);
            affine_wavefronts->dwavefronts[score]=wavefront_set->out_dwavefront;
        } else {
            wavefront_set->out_dwavefront = NULL;
         }
    }


void affine_wavefront_fetch_index(affine_wavefronts_t* affine_wavefronts, int score, affine_wavefront_index_set* index_set)
{
    // Compute score
    const affine_wavefront_penalties_t* const wavefront_penalties = & (affine_wavefronts->penalties);
    const int mismatch_score = score - wavefront_penalties->mismatch;
    const int gap_open_score = score - wavefront_penalties->gap_opening - wavefront_penalties->gap_extension;
    const int gap_extend_score = score - wavefront_penalties->gap_extension;
    // Fetch index sets
    index_set->in_mindex_sub = affine_wavefronts_get_source_mindex(affine_wavefronts,mismatch_score);
    index_set->in_dindex_gap = affine_wavefronts_get_source_dindex_gap(affine_wavefronts,gap_open_score);
    index_set->in_dindex_gap = affine_wavefronts_get_source_dindex_gap(affine_wavefronts,gap_open_score);
    index_set->in_iindex_ext = affine_wavefronts_get_source_iindex(affine_wavefronts,gap_extend_score);
    index_set->in_dindex_ext = affine_wavefronts_get_source_dindex(affine_wavefronts,gap_extend_score);
}

// Build new index set
void affine_wavefront_new_index_set_idm(affine_wavefront_index_set* index_set, affine_wavefronts_t* affine_wavefronts)
{
    index_set->out_dindex = mm_allocator_alloc(affine_wavefronts->mm_allocator, affine_wavefront_indexs_t*);
    index_set->out_iindex = mm_allocator_alloc(affine_wavefronts->mm_allocator, affine_wavefront_indexs_t*);
    index_set->out_mindex = mm_allocator_alloc(affine_wavefronts->mm_allocator, affine_wavefront_indexs_t*);

    // I-index set
    affine_wavefront_index_t* temp = index_set->index_null;
    if (index_set->in_iindex_gap!=NULL){
        memcpy(temp, index_set->in_iindex_gap->idx, index_set->in_iindex_gap->idx_num * sizeof (affine_wavefront_index_t));
        temp = temp + index_set->in_iindex_gap->idx_num;
    }
    if (index_set->in_iindex_ext!=NULL){
        memcpy(temp, index_set->in_iindex_ext->idx, index_set->in_iindex_ext->idx_num * sizeof (affine_wavefront_index_t));
        temp = temp + index_set->in_iindex_ext->idx_num;
    }
    sort(index_set->index_null,temp,sort_index);
    int index_num = unique(index_set->index_null,temp,unique_index)-(int)index_set->index_null;
    index_set->out_iindex->idx = mm_allocator_calloc(affine_wavefronts->mm_allocator, index_num, affine_wavefront_index_t, false);
    memcpy(index_set->out_iindex->idx,index_set->index_null,(index_num)*sizeof(affine_wavefront_index_t));
    index_set->out_iindex->idx_num = index_num;

    // D-index set
    temp = index_set->index_null;
    if (index_set->in_dindex_gap!=NULL){
        memcpy(temp, index_set->in_dindex_gap->idx, index_set->in_dindex_gap->idx_num * sizeof (affine_wavefront_index_t));
        temp = temp + index_set->in_dindex_gap->idx_num;
    }
    if (index_set->in_dindex_ext!=NULL){
        memcpy(temp, index_set->in_dindex_ext->idx, index_set->in_dindex_ext->idx_num * sizeof (affine_wavefront_index_t));
        temp = temp + index_set->in_dindex_ext->idx_num;
    }
    sort(index_set->index_null,temp,sort_index);
    int index_num = unique(index_set->index_null,index_set->index_null + index_set->in_dindex_gap->idx_num + index_set->in_dindex_ext->idx_num,unique_index)-(int)index_set->index_null;
    index_set->out_dindex->idx = mm_allocator_calloc(affine_wavefronts->mm_allocator, 2*index_num, affine_wavefront_index_t, false);
    memcpy(index_set->out_dindex->idx,index_set->index_null,(index_num)*sizeof(affine_wavefront_index_t));
    index_set->out_dindex->idx_num = index_num;
    
    // M-index set 
    temp = index_set->index_null;
    memcry(temp, index_set->out_dindex->idx, index_set->out_dindex->idx_num * sizeof(affine_wavefront_index_t));
    temp = temp + index_set->out_dindex->idx_num;
    memcry(temp, index_set->out_iindex->idx, index_set->out_iindex->idx_num * sizeof(affine_wavefront_index_t));
    temp = temp + index_set->out_iindex->idx_num;
    if (index_set->in_mindex_sub!=NULL)
    {
        memcry(temp, index_set->in_mindex_sub->idx, index_set->in_mindex_sub->idx_num * sizeof(affine_wavefront_index_t));
        temp = temp + index_set->in_mindex_sub->idx_num;
    }
    sort(index_set->index_null,temp, sort_index);
    int index_num = unique(index_set->index_null,temp,unique_index)-(int)index_set->index_null;
    index_set->out_mindex->idx = mm_allocator_calloc(affine_wavefronts->mm_allocator, 2*index_num, affine_wavefront_index_t, false);
    memcry(index_set->out_mindex->idx,index_set->index_null,(index_num)*sizeof(affine_wavefront_index_t));
    index_set->out_mindex->idx_num = index_num;
}

void affine_wavefront_new_index_set_dm(affine_wavefront_index_set* index_set, affine_wavefronts_t* affine_wavefronts)
{
    index_set->out_dindex = mm_allocator_alloc(affine_wavefronts->mm_allocator, affine_wavefront_indexs_t*);
    index_set->out_iindex = NULL;
    index_set->out_mindex = mm_allocator_alloc(affine_wavefronts->mm_allocator, affine_wavefront_indexs_t*);

    // D-index set
    affine_wavefront_index_t* temp = index_set->index_null;
    if (index_set->in_dindex_gap!=NULL){
        memcpy(temp, index_set->in_dindex_gap->idx, index_set->in_dindex_gap->idx_num * sizeof (affine_wavefront_index_t));
        temp = temp + index_set->in_dindex_gap->idx_num;
    }
    if (index_set->in_dindex_ext!=NULL){
        memcpy(temp, index_set->in_dindex_ext->idx, index_set->in_dindex_ext->idx_num * sizeof (affine_wavefront_index_t));
        temp = temp + index_set->in_dindex_ext->idx_num;
    }
    sort(index_set->index_null,temp,sort_index);
    int index_num = unique(index_set->index_null,index_set->index_null + index_set->in_dindex_gap->idx_num + index_set->in_dindex_ext->idx_num,unique_index)-(int)index_set->index_null;
    index_set->out_dindex->idx = mm_allocator_calloc(affine_wavefronts->mm_allocator, 2*index_num, affine_wavefront_index_t, false);
    memcpy(index_set->out_dindex->idx,index_set->index_null,(index_num)*sizeof(affine_wavefront_index_t));
    index_set->out_dindex->idx_num = index_num;
    
    // M-index set 
    temp = index_set->index_null;
    memcry(temp, index_set->out_dindex->idx, index_set->out_dindex->idx_num * sizeof(affine_wavefront_index_t));
    temp = temp + index_set->out_dindex->idx_num;
    if (index_set->in_mindex_sub!=NULL)
    {
        memcry(temp, index_set->in_mindex_sub->idx, index_set->in_mindex_sub->idx_num * sizeof(affine_wavefront_index_t));
        temp = temp + index_set->in_mindex_sub->idx_num;
    }
    sort(index_set->index_null,temp, sort_index);
    int index_num = unique(index_set->index_null,temp,unique_index)-(int)index_set->index_null;
    index_set->out_mindex->idx = mm_allocator_calloc(affine_wavefronts->mm_allocator, 2*index_num, affine_wavefront_index_t, false);
    memcry(index_set->out_mindex->idx,index_set->index_null,(index_num)*sizeof(affine_wavefront_index_t));
    index_set->out_mindex->idx_num = index_num;
}

void affine_wavefront_new_index_set_im(affine_wavefront_index_set* index_set, affine_wavefronts_t* affine_wavefronts)
{
    index_set->out_dindex = NULL;
    index_set->out_iindex = mm_allocator_alloc(affine_wavefronts->mm_allocator, affine_wavefront_indexs_t*);
    index_set->out_mindex = mm_allocator_alloc(affine_wavefronts->mm_allocator, affine_wavefront_indexs_t*);

    // I-index set
    affine_wavefront_index_t* temp = index_set->index_null;
    if (index_set->in_iindex_gap!=NULL){
        memcpy(temp, index_set->in_iindex_gap->idx, index_set->in_iindex_gap->idx_num * sizeof (affine_wavefront_index_t));
        temp = temp + index_set->in_iindex_gap->idx_num;
    }
    if (index_set->in_iindex_ext!=NULL){
        memcpy(temp, index_set->in_iindex_ext->idx, index_set->in_iindex_ext->idx_num * sizeof (affine_wavefront_index_t));
        temp = temp + index_set->in_iindex_ext->idx_num;
    }
    sort(index_set->index_null,temp,sort_index);
    int index_num = unique(index_set->index_null,temp,unique_index)-(int)index_set->index_null;
    index_set->out_iindex->idx = mm_allocator_calloc(affine_wavefronts->mm_allocator, index_num, affine_wavefront_index_t, false);
    memcpy(index_set->out_iindex->idx,index_set->index_null,(index_num)*sizeof(affine_wavefront_index_t));
    index_set->out_iindex->idx_num = index_num;
    
    // M-index set 
    temp = index_set->index_null;
    memcry(temp, index_set->out_iindex->idx, index_set->out_iindex->idx_num * sizeof(affine_wavefront_index_t));
    temp = temp + index_set->out_iindex->idx_num;
    if (index_set->in_mindex_sub!=NULL)
    {
        memcry(temp, index_set->in_mindex_sub->idx, index_set->in_mindex_sub->idx_num * sizeof(affine_wavefront_index_t));
        temp = temp + index_set->in_mindex_sub->idx_num;
    }
    sort(index_set->index_null,temp, sort_index);
    int index_num = unique(index_set->index_null,temp,unique_index)-(int)index_set->index_null;
    index_set->out_mindex->idx = mm_allocator_calloc(affine_wavefronts->mm_allocator, 2*index_num, affine_wavefront_index_t, false);
    memcry(index_set->out_mindex->idx,index_set->index_null,(index_num)*sizeof(affine_wavefront_index_t));
    index_set->out_mindex->idx_num = index_num;
}

void affine_wavefront_new_index_set_m(affine_wavefront_index_set* index_set, affine_wavefronts_t* affine_wavefronts)
{
    index_set->out_dindex = NULL;
    index_set->out_iindex = NULL;
    index_set->out_mindex = mm_allocator_alloc(affine_wavefronts->mm_allocator, affine_wavefront_indexs_t*);
    
    // M-index set 
    affine_wavefront_index_t* temp = index_set->index_null;
    memcry(temp, index_set->in_mindex_sub->idx, index_set->in_mindex_sub->idx_num * sizeof(affine_wavefront_index_t));
    temp = temp + index_set->in_mindex_sub->idx_num;
    sort(index_set->index_null,temp, sort_index);
    int index_num = unique(index_set->index_null,temp,unique_index)-(int)index_set->index_null;
    index_set->out_mindex->idx = mm_allocator_calloc(affine_wavefronts->mm_allocator, 2*index_num, affine_wavefront_index_t, false);
    memcry(index_set->out_mindex->idx,index_set->index_null,(index_num)*sizeof(affine_wavefront_index_t));
    index_set->out_mindex->idx_num = index_num;
}

/*
Compute wavefront offsets
*/
void affine_wavefronts_compute_offsets_idm(
    affine_wavefronts_t* const affine_wavefronts,
    affine_wavefront_set* const wavefront_set,
    const affine_wavefront_index_set* index_set,
    int score){
        int** position_table = affine_wavefronts->visit;
        awf_offset_t* const moffsets = wavefront_set->out_mwavefront->offsets;
        awf_offset_t* const doffsets = wavefront_set->out_dwavefront->offsets;
        awf_offset_t* const ioffsets = wavefront_set->out_iwavefront->offsets;
        
        // Create new index set
        affine_wavefront_new_index_set_idm(index_set, affine_wavefronts);

        // Update D-wavefronts
        affine_wavefront_index_t* dindex = index_set->out_dindex->idx;
        affine_wavefront_index_t index;
        int idx_num = index_set->out_dindex->idx_num; int v; int k; int storage_position; int kernel;
        for (int n = 0; n < idx_num; n++)
        {
            index = dindex[n];
            v = index.segment_index;
            k = index.diagonal_index;
            storage_position = index.storage_position;
            if (index.full)
                continue;
            if (n < idx_num - 1 && dindex[n + 1].segment_index == v && dindex[n + 1].diagonal_index == k + 1)
            {
                storage_position = dindex[n + 1].storage_position;
            }
            else
            {
                storage_position = affine_wavefronts->visit[v][k + 1];
            }
            if (storage_position >= 0)
            {
                kernel = (wavefront_set->in_dwavefront_ext!=NULL && wavefront_set->in_dwavefront_ext->offset_num > storage_position)<<1 | (wavefront_set->in_mwavefront_gap!= NULL && wavefront_set->in_mwavefront_gap->offset_num > storage_position);
                switch(kernel)
                {
                    case 0: break;
                    case 1:{
                        doffsets[index.storage_position] = MAX(0, wavefront_set->in_mwavefront_gap->offsets[storage_position] + 1);
                        break;
                    }
                    case 2:{
                        doffsets[index.storage_position] = MAX(0, wavefront_set->in_dwavefront_ext->offsets[storage_position] + 1);
                        break;
                    }
                    case 3:{
                        doffsets[index.storage_position] = MAX(0, MAX(wavefront_set->in_dwavefront_ext->offsets[storage_position] + 1, wavefront_set->in_mwavefront_gap->offsets[storage_position] + 1));
                        break;
                    }
                }
                if (doffsets[index.storage_position] >= affine_wavefronts->graph[v].pattern_length - 1)
                {
                    dindex[n].full = true;
                    int h = AFFINE_WAVEFRONT_H(k, doffsets[index.storage_position]);
                    for (int i = 0; i < affine_wavefronts->graph->node[v].next_num; i++)
                    {
                        int w = affine_wavefronts->graph->node[v].next[i];
                        int w_position;
                        if(affine_wavefronts->visit[w][h]>=0)
                        {
                            w_position = affine_wavefronts->visit[w][h];
                        }
                        else
                        {
                            w_position = affine_wavefronts->index_storage_num;
                            affine_wavefronts->visit[w][h] = affine_wavefronts->index_storage_num;
                            affine_wavefronts->index_storage_num++;
                        }
                        ADD_INDEX(index_set->out_dindex->idx, index_set->out_dindex->idx_num, h, w, w_position,false);
                    }
                }
                else
                {
                    index.diagonal_index = k - 1;
                    if (n > 0 && dindex[n - 1].segment_index == v && dindex[n - 1].diagonal_index == k - 1)
                    {
                        index.storage_position = dindex[n - 1].storage_position;
                    }
                    else
                    {
                        index.storage_position = affine_wavefronts->visit[v][k - 1] > 0? affine_wavefronts->visit[v][k - 1]:affine_wavefronts->index_storage_num ++;
                        affine_wavefronts->visit[v][k - 1] = index.storage_position;
                    }
                    dindex[n] = index;
                }
            }
            else
            {
                doffsets[n]=0;
                index.storage_position = affine_wavefronts->visit[v][k - 1] > 0? affine_wavefronts->visit[v][k - 1]:affine_wavefronts->index_storage_num ++;
                affine_wavefronts->visit[v][k - 1] = index.storage_position;
                index.segment_index = k - 1;
                dindex[n] = index;
            }
        }
        sort(dindex, dindex + index_set->out_dindex->idx_num, sort_index);
        index_set->out_dindex->idx_num = unique(dindex, dindex + index_set->out_dindex->idx_num, unique_index) - (int)dindex;

        // update I-wavefronts
        affine_wavefront_index_t* iindex = index_set->out_iindex->idx;
        affine_wavefront_index_t pre_index;
        int idx_num = 0;
        for (int n = 0; n < index_set->out_dindex->idx_num; n++)
        {
            index = iindex[n];
            v = index.segment_index; k = index.diagonal_index;
            if (pre_index.segment_index == v && pre_index.diagonal_index == k - 1)
            {
                storage_position = pre_index.storage_position;
            }
            else
            {
                storage_position = affine_wavefronts->visit[v][k - 1];
            }
            kernel = (wavefront_set->in_iwavefront_ext!=NULL && wavefront_set->in_iwavefront_ext->offset_num > storage_position)<<1 | (wavefront_set->in_mwavefront_gap!= NULL && wavefront_set->in_mwavefront_gap->offset_num > storage_position);
            switch (kernel)
            {
                case 1:
                {
                    ioffsets[index.storage_position] = MAX(wavefront_set->in_mwavefront_gap->offsets[storage_position],0);
                    break;
                }
                case 2:
                {
                    ioffsets[index.storage_position] = MAX(wavefront_set->in_iwavefront_ext->offsets[storage_position],0);
                    break;
                }
                case 3:
                {
                    ioffsets[index.storage_position] = MAX(wavefront_set->in_mwavefront_gap->offsets[storage_position],MAX(wavefront_set->in_iwavefront_ext->offsets[storage_position],0));
                    break;
                }
            }
            int h = AFFINE_WAVEFRONT_H(k, ioffsets[index.storage_position]);
            pre_index = index;
            if (h >= affine_wavefronts->text_length - 1)
                continue;
            if (n < index_set->out_dindex->idx_num - 1 && iindex[n + 1].segment_index == v && iindex [n + 1].diagonal_index == k + 1)
            {
                storage_position = iindex[n + 1].storage_position;
            }
            else
            {
                storage_position = affine_wavefronts->visit[v][k + 1]>=0? affine_wavefronts->visit[v][k + 1]: affine_wavefronts->index_storage_num ++;
                affine_wavefronts->visit[v][k + 1] = storage_position;
            }
            index.storage_position = storage_position;
            index.diagonal_index = k + 1;
            dindex[idx_num++] = index;
        }
        index_set->out_dindex->idx_num = idx_num;
        
        // update M wavefronts
        affine_wavefront_index_t* mindex = index_set->out_mindex->idx;
        for (int n = 0; n < index_set->out_mindex->idx_num; n++)
        {
            storage_position = mindex[n].storage_position;
            if (mindex[n].storage_position < wavefront_set->in_mwavefront_gap->offset_num)
            {
                moffsets[storage_position] = MAX(0,MAX(ioffsets[storage_position],MAX(doffsets[storage_position], wavefront_set->in_mwavefront_gap->offsets[storage_position] + 1)));
            }
            else
            {
                moffsets[storage_position] = MAX(0,MAX(ioffsets[storage_position],doffsets[storage_position]));
            }
        }

        affine_wavefronts->mwavefronts[score] = wavefront_set->out_mwavefront;
        affine_wavefronts->iwavefronts[score] = wavefront_set->out_iwavefront;
        affine_wavefronts->dwavefronts[score] = wavefront_set->out_dwavefront;

        affine_wavefronts->mindex_set[score] = index_set->out_mindex;
        affine_wavefronts->dindex_set[score] = index_set->out_dindex;
        affine_wavefronts->iindex_set[score] = index_set->out_iindex;
    }

void affine_wavefronts_compute_offsets_dm(
    affine_wavefronts_t* const affine_wavefronts,
    affine_wavefront_set* const wavefront_set,
    const affine_wavefront_index_set* index_set,
    int score){
        int** position_table = affine_wavefronts->visit;
        awf_offset_t* const moffsets = wavefront_set->out_mwavefront->offsets;
        awf_offset_t* const doffsets = wavefront_set->out_dwavefront->offsets;

        // Create new index set
        affine_wavefront_new_index_set_dm(index_set, affine_wavefronts);
        
         // Update D-wavefronts
        affine_wavefront_index_t* dindex = index_set->out_dindex->idx;
        affine_wavefront_index_t index;
        int idx_num = index_set->out_dindex->idx_num; int v; int k; int storage_position; int kernel;
        for (int n = 0; n < idx_num; n++)
        {
            index = dindex[n];
            v = index.segment_index;
            k = index.diagonal_index;
            storage_position = index.storage_position;
            if (index.full)
                continue;
            if (n < idx_num - 1 && dindex[n + 1].segment_index == v && dindex[n + 1].diagonal_index == k + 1)
            {
                storage_position = dindex[n + 1].storage_position;
            }
            else
            {
                storage_position = affine_wavefronts->visit[v][k + 1];
            }
            if (storage_position >= 0)
            {
                kernel = (wavefront_set->in_dwavefront_ext!=NULL && wavefront_set->in_dwavefront_ext->offset_num > storage_position)<<1 | (wavefront_set->in_mwavefront_gap!= NULL && wavefront_set->in_mwavefront_gap->offset_num > storage_position);
                switch(kernel)
                {
                    case 0: break;
                    case 1:{
                        doffsets[index.storage_position] = MAX(0, wavefront_set->in_mwavefront_gap->offsets[storage_position] + 1);
                        break;
                    }
                    case 2:{
                        doffsets[index.storage_position] = MAX(0, wavefront_set->in_dwavefront_ext->offsets[storage_position] + 1);
                        break;
                    }
                    case 3:{
                        doffsets[index.storage_position] = MAX(0, MAX(wavefront_set->in_dwavefront_ext->offsets[storage_position] + 1, wavefront_set->in_mwavefront_gap->offsets[storage_position] + 1));
                        break;
                    }
                }
                if (doffsets[index.storage_position] >= affine_wavefronts->graph[v].pattern_length - 1)
                {
                    dindex[n].full = true;
                    int h = AFFINE_WAVEFRONT_H(k, doffsets[index.storage_position]);
                    for (int i = 0; i < affine_wavefronts->graph->node[v].next_num; i++)
                    {
                        int w = affine_wavefronts->graph->node[v].next[i];
                        int w_position;
                        if(affine_wavefronts->visit[w][h]>=0)
                        {
                            w_position = affine_wavefronts->visit[w][h];
                        }
                        else
                        {
                            w_position = affine_wavefronts->index_storage_num;
                            affine_wavefronts->visit[w][h] = affine_wavefronts->index_storage_num;
                            affine_wavefronts->index_storage_num++;
                        }
                        ADD_INDEX(index_set->out_dindex->idx, index_set->out_dindex->idx_num, h, w, w_position,false);
                    }
                }
                else
                {
                    index.diagonal_index = k - 1;
                    if (n > 0 && dindex[n - 1].segment_index == v && dindex[n - 1].diagonal_index == k - 1)
                    {
                        index.storage_position = dindex[n - 1].storage_position;
                    }
                    else
                    {
                        index.storage_position = affine_wavefronts->visit[v][k - 1] > 0? affine_wavefronts->visit[v][k - 1]:affine_wavefronts->index_storage_num ++;
                        affine_wavefronts->visit[v][k - 1] = index.storage_position;
                    }
                    dindex[n] = index;
                }
            }
            else
            {
                doffsets[n]=0;
                index.storage_position = affine_wavefronts->visit[v][k - 1] > 0? affine_wavefronts->visit[v][k - 1]:affine_wavefronts->index_storage_num ++;
                affine_wavefronts->visit[v][k - 1] = index.storage_position;
                index.segment_index = k - 1;
                dindex[n] = index;
            }
        }
        sort(dindex, dindex + index_set->out_dindex->idx_num, sort_index);
        index_set->out_dindex->idx_num = unique(dindex, dindex + index_set->out_dindex->idx_num, unique_index) - (int)dindex;

        // update M wavefronts
        affine_wavefront_index_t* mindex = index_set->out_mindex->idx;
        for (int n = 0; n < index_set->out_mindex->idx_num; n++)
        {
            storage_position = mindex[n].storage_position;
            if (mindex[n].storage_position < wavefront_set->in_mwavefront_gap->offset_num)
            {
                moffsets[storage_position] = MAX(0,MAX(doffsets[storage_position], wavefront_set->in_mwavefront_gap->offsets[storage_position] + 1));
            }
            else
            {
                moffsets[storage_position] = MAX(0,doffsets[storage_position]);
            }
        }

        affine_wavefronts->mwavefronts[score] = wavefront_set->out_mwavefront;
        affine_wavefronts->iwavefronts[score] = wavefront_set->out_iwavefront;
        affine_wavefronts->dwavefronts[score] = wavefront_set->out_dwavefront;

        affine_wavefronts->mindex_set[score] = index_set->out_mindex;
        affine_wavefronts->dindex_set[score] = index_set->out_dindex;
        affine_wavefronts->iindex_set[score] = index_set->out_iindex;
    }

void affine_wavefronts_compute_offsets_im(
    affine_wavefronts_t* const affine_wavefronts,
    affine_wavefront_set* const wavefront_set,
    const affine_wavefront_index_set* index_set,
    int score){
        int** position_table = affine_wavefronts->visit;
        awf_offset_t* const moffsets = wavefront_set->out_mwavefront->offsets;
        awf_offset_t* const ioffsets = wavefront_set->out_iwavefront->offsets;
        
        // Create new index set
        affine_wavefront_new_index_set_im(index_set, affine_wavefronts);

        affine_wavefront_index_t index;
        int v; int k; int storage_position; int kernel;

        // update I-wavefronts
        affine_wavefront_index_t* iindex = index_set->out_iindex->idx;
        affine_wavefront_index_t pre_index;
        int idx_num = 0;
        for (int n = 0; n < index_set->out_dindex->idx_num; n++)
        {
            index = iindex[n];
            v = index.segment_index; k = index.diagonal_index;
            if (pre_index.segment_index == v && pre_index.diagonal_index == k - 1)
            {
                storage_position = pre_index.storage_position;
            }
            else
            {
                storage_position = affine_wavefronts->visit[v][k - 1];
            }
            kernel = (wavefront_set->in_iwavefront_ext!=NULL && wavefront_set->in_iwavefront_ext->offset_num > storage_position)<<1 | (wavefront_set->in_mwavefront_gap!= NULL && wavefront_set->in_mwavefront_gap->offset_num > storage_position);
            switch (kernel)
            {
                case 1:
                {
                    ioffsets[index.storage_position] = MAX(wavefront_set->in_mwavefront_gap->offsets[storage_position],0);
                    break;
                }
                case 2:
                {
                    ioffsets[index.storage_position] = MAX(wavefront_set->in_iwavefront_ext->offsets[storage_position],0);
                    break;
                }
                case 3:
                {
                    ioffsets[index.storage_position] = MAX(wavefront_set->in_mwavefront_gap->offsets[storage_position],MAX(wavefront_set->in_iwavefront_ext->offsets[storage_position],0));
                    break;
                }
            }
            int h = AFFINE_WAVEFRONT_H(k, ioffsets[index.storage_position]);
            pre_index = index;
            if (h >= affine_wavefronts->text_length - 1)
                continue;
            if (n < index_set->out_dindex->idx_num - 1 && iindex[n + 1].segment_index == v && iindex [n + 1].diagonal_index == k + 1)
            {
                storage_position = iindex[n + 1].storage_position;
            }
            else
            {
                storage_position = affine_wavefronts->visit[v][k + 1]>=0? affine_wavefronts->visit[v][k + 1]: affine_wavefronts->index_storage_num ++;
                affine_wavefronts->visit[v][k + 1] = storage_position;
            }
            index.storage_position = storage_position;
            index.diagonal_index = k + 1;
            iindex[idx_num++] = index;
        }
        index_set->out_dindex->idx_num = idx_num;

        // update M wavefronts
        affine_wavefront_index_t* mindex = index_set->out_mindex->idx;
        for (int n = 0; n < index_set->out_mindex->idx_num; n++)
        {
            storage_position = mindex[n].storage_position;
            if (mindex[n].storage_position < wavefront_set->in_mwavefront_gap->offset_num)
            {
                moffsets[storage_position] = MAX(0,MAX(ioffsets[storage_position], wavefront_set->in_mwavefront_gap->offsets[storage_position] + 1));
            }
            else
            {
                moffsets[storage_position] = MAX(0,ioffsets[storage_position]);
            }
        }

        affine_wavefronts->mwavefronts[score] = wavefront_set->out_mwavefront;
        affine_wavefronts->iwavefronts[score] = wavefront_set->out_iwavefront;
        affine_wavefronts->dwavefronts[score] = wavefront_set->out_dwavefront;

        affine_wavefronts->mindex_set[score] = index_set->out_mindex;
        affine_wavefronts->dindex_set[score] = index_set->out_dindex;
        affine_wavefronts->iindex_set[score] = index_set->out_iindex;
    }

void affine_wavefronts_compute_offsets_m(
    affine_wavefronts_t* const affine_wavefronts,
    affine_wavefront_set* const wavefront_set,
    const affine_wavefront_index_set* index_set,
    int score){
        int** position_table = affine_wavefronts->visit;
        awf_offset_t* const moffsets = wavefront_set->out_mwavefront->offsets;
        
        // Create new index set
        affine_wavefront_new_index_set_m(index_set, affine_wavefronts);

        affine_wavefront_index_t index;
        int v; int k; int storage_position; int kernel;

        // update M wavefronts
        affine_wavefront_index_t* mindex = index_set->out_mindex->idx;
        for (int n = 0; n < index_set->out_mindex->idx_num; n++)
        {
            storage_position = mindex[n].storage_position;
            if (mindex[n].storage_position < wavefront_set->in_mwavefront_gap->offset_num)
            {
                moffsets[storage_position] = MAX(0, wavefront_set->in_mwavefront_gap->offsets[storage_position] + 1);
            }
            else
            {
                moffsets[storage_position] = 0;
            }
        }

        affine_wavefronts->mwavefronts[score] = wavefront_set->out_mwavefront;
        affine_wavefronts->iwavefronts[score] = wavefront_set->out_iwavefront;
        affine_wavefronts->dwavefronts[score] = wavefront_set->out_dwavefront;

        affine_wavefronts->mindex_set[score] = index_set->out_mindex;
        affine_wavefronts->dindex_set[score] = index_set->out_dindex;
        affine_wavefronts->iindex_set[score] = index_set->out_iindex;
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
    strings_padded_t* const strings_padded = strings_padded_new_rhomb(graph,text,text_length,AFFINE_WAVEFRONT_PADDING,affine_wavefronts->mm_allocator);
    // Initialize wavefront
    affine_wavefront_initialize(affine_wavefronts);
    affine_wavefront_set* wavefront_set;
    affine_wavefront_set_initialize(wavefront_set);
    affine_wavefront_index_set* index_set;
    affine_wavefront_index_set_initialize(index_set);
    // Compute wavefronts for increasing score
    int score = 0;
    int alignment_k = AFFINE_WAVEFRONT_DIAGONAL(text_length, affine_wavefronts->graph[alignment_k].pattern_length);
   
    while (true) {
        // Exact extend s-wavefront
        affine_wavefronts_extend_mwavefront_compute_packed(affine_wavefronts,strings_padded->text_padded, strings_padded->graph_padded, score, alignment_v, alignment_k);
        if (affine_wavefronts->final_status) break;
        // Update all wavefronts
        ++score; // Increase score
        affine_wavefronts_compute_wavefront(affine_wavefronts, wavefront_set, index_set, score);
    }
}