// DESCRIPTION: WFA extend exact-matches component

#include"affine_wavefront_extend.h"

void affine_wavefronts_extend_mwavefront_compute_packed(
    affine_wavefronts_t* const affine_wavefronts,
    const char* const text,
    graph_padded_t* const graph,
    const int score, 
    const int alignment_v, const int alignment_k){
        // Fetch m-wavefront
        affine_wavefront_t* const mwavefront = affine_wavefronts->mwavefronts[score];
        if (mwavefront==NULL) return;
        // Extend diagonally each wavefront point
        awf_offset_t* const offsets = mwavefront->offsets;
        affine_wavefront_index_t* index_set = affine_wavefronts->mindex_set[score]->idx;
        affine_wavefront_index_t* index_set_null = affine_wavefronts->index_set_null->idx;
        int n = 0; int nindex_num = 0;
        int index_num = affine_wavefronts->mindex_set[score]->idx_num;
        while (n < index_num){
            // Fetch offset & positions
            const affine_wavefront_index_t index = index_set[n++];
            const int k = index.diagonal_index;
            const int v = index.segment_index;
            const int position = index.storage_position;
            if (index.full) {
                ADD_INDEX(index_set_null,nindex_num,k,v,position,true);
                continue;}
            const awf_offset_t offset = offsets[position];
            if (offset < 0) continue;
            const uint32_t h = AFFINE_WAVEFRONT_H(k,offset);
            const int text_length = affine_wavefronts->text_length;
            if (h >= text_length) {
                ADD_INDEX(index_set_null,nindex_num,k,v,position,true);
                continue;}
            const char* segment = graph->node[v].pattern_padded_buffer;
            const uint32_t i = AFFINE_WAVEFRONT_I(k,offset);
            const int segment_length = affine_wavefronts->graph->node[v].node_pattern;
            if (i < segment_length){
                // Fetch pattern/text blocks
                uint64_t* pattern_blocks = (uint64_t*)(segment+i);
                uint64_t* text_blocks = (uint64_t*)(text + h);
                uint64_t pattern_block = *pattern_blocks;
                uint64_t text_block = *text_blocks;
                // Compare 64-bits blocks
                uint64_t cmp = pattern_block ^ text_block;
                while(__builtin_expect(!cmp,0)){
                    // Increment offset (full block)
                    offsets[position] += 8;
                    // Next blocks
                    ++pattern_blocks;
                    ++text_blocks;
                    // Fetch
                    pattern_block = *pattern_blocks;
                    text_block = *text_blocks;
                    // Compare
                    cmp = pattern_block ^ text_block;
                }
                // Count equal characters
                const int equal_right_bits = __builtin_ctzl(cmp);
                const int equal_chars = DIV_FLOOR(equal_right_bits,8);
                // Increment offset
                offsets[position] += equal_chars;
                if (offsets[position]==segment_length){
                    index_set[--n]=index;
                }
                else{
                    if(nindex_num > 1)
                    {
                        switch(((index_set_null[nindex_num-2].diagonal_index == k - 1)&& index_set_null[nindex_num-2].segment_index == v)<<1 | ((index_set_null[nindex_num-1].diagonal_index == k)&& index_set_null[nindex_num-1].segment_index == v))
                        {
                            case 3:{
                                index_set_null[nindex_num-1].storage_position = position;
                                ADD_INDEX(index_set_null,nindex_num,k+1,v,INT_MAX,false);
                                break;
                            }
                            case 2:{
                                ADD_INDEX(index_set_null,nindex_num,k,v,position,false);
                                ADD_INDEX(index_set_null,nindex_num,k+1,v,INT_MAX,false);
                                break;
                            }
                            case 1:{
                                index_set_null[nindex_num-1].storage_position = position;
                                ADD_INDEX(index_set_null,nindex_num,k-1,v,INT_MAX,false);
                                ADD_INDEX(index_set_null,nindex_num,k+1,v,INT_MAX,false);
                                break;
                            }
                            case 0:{
                                ADD_INDEX(index_set_null,nindex_num,k-1,v,INT_MAX,false);
                                ADD_INDEX(index_set_null,nindex_num,k,v,position,false);
                                ADD_INDEX(index_set_null,nindex_num,k+1,v,INT_MAX,false);
                            }
                        }
                    }
                    else
                    {
                        ADD_INDEX(index_set_null,nindex_num,k-1,v,INT_MAX,false);
                        ADD_INDEX(index_set_null,nindex_num,k,v,position,false);
                        ADD_INDEX(index_set_null,nindex_num,k+1,v,INT_MAX,false);
                    }
                }
            }
            else{
                if(alignment_v==v && alignment_k==k)
                    {
                        affine_wavefronts->final_status = true;
                        return;
                    }
                ADD_INDEX(index_set_null,nindex_num,k,v,position,true);
                int count = 0;
                int next_num = affine_wavefronts->graph->node[v].next_num;
                int* next_node = affine_wavefronts->graph->node[v].next;
                for (int node_num = 0; node_num < next_num; node_num++)
                {
                    int w = next_node[node_num];
                    if (affine_wavefronts->visit[w][i+1] >= 0)
                        continue;
                    if (text[i+1]==affine_wavefronts->graph->node[w].node_pattern[0])
                    {
                        ADD_INDEX(index_set,index_num,i+1,w,affine_wavefronts->index_storage_num++,false);
                        ++count;
                    }
                    else{
                        ADD_INDEX(index_set_null,nindex_num,i,w,INT_MAX,false);
                        ADD_INDEX(index_set_null,nindex_num,i+1,w,INT_MAX,false);
                    }
                }
                if (next_num==0||count<next_num)
                {
                    ADD_INDEX(index_set_null,nindex_num,k+1,v,INT_MAX,false);
                }
            }            
        }
        sort(index_set_null,index_set_null+nindex_num,sort_index);
        nindex_num = unique(index_set_null,index_set_null+nindex_num,unique_index)-(int)index_set_null;
        for (n = 0;n < nindex_num;n++)
        {
            if (index_set_null[n].storage_position >= affine_wavefronts->index_storage_num)
            {
                index_set_null[n].storage_position = affine_wavefronts->index_storage_num;
                affine_wavefronts->index_storage_num++;
            }
        }
        int didx_num = 0; int iidx_num = 0;
        for (n = 1; n < nindex_num; n++)
        {
            if (index_set_null[n - 1].diagonal_index == index_set_null[n].diagonal_index - 1 && index_set_null[n - 1].segment_index == index_set_null[n].segment_index)
            {
                affine_wavefronts->dindex_set_gap[score]->idx[didx_num++] = index_set_null[n - 1];
                if (offsets[index_set_null[n].storage_position]>=0)
                {
                    affine_wavefronts->dindex_set_gap[score]->idx[iidx_num++] = index_set_null[n];
                }
            }
        }
        affine_wavefronts->mindex_set[score]->idx_num = nindex_num;
        affine_wavefronts->mindex_set[score]->idx = index_set_null;
    }