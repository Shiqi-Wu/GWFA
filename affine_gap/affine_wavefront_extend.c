
#include"affine_wavefront_extend.h"

void affine_wavefronts_extend_mwavefront_compute_packed(
    affine_wavefronts_t* const affine_wavefronts,
    const char* const text, const int score){
        // Fetch m-wavefront
        affine_wavefront_t* const mwavefront = affine_wavefronts->mwavefronts[score];
        if (mwavefront==NULL) return;
        // Extend diagonally each wavefront point
        awf_offset_t* const offsets = mwavefront->offsets;
        affine_wavefront_index_t* index_set = affine_wavefronts->index_set;
        affine_wavefront_index_t* index_set_null = affine_wavefronts->index_set_null;
        int n = 0; int nindex_num = 0;
        int index_num = affine_wavefronts->index_set_num;
        while (n < index_num){
            // Fetch offset & positions
            const affine_wavefront_index_t index = index_set[n++];
            const int k = index.diagonal_index;
            const int v = index.segment_index;
            const int position = index.storage_position;
            const awf_offset_t offset = offsets[position];
            if (offset < 0) continue;
            const uint32_t h = AFFINE_WAVEFRONT_H(k,offset);
            const int text_length = affine_wavefronts->text_length;
            if (h >= text_length) continue;
            const char* segment = affine_wavefronts->graph->node[v].node_pattern;
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
                    if(nindex_num > 0)
                    {
                        switch(((index_set_null[nindex_num-2].diagonal_index == k-1)&& index_set_null[nindex_num-2].segment_index == v)<<1+ ((index_set_null[nindex_num-1].diagonal_index == k)&& index_set_null[nindex_num-1].segment_index == v))
                        {
                            case 3:{
                                index_set_null[nindex_num-1].storage_position = position;
                                ADD_INDEX(index_set_null,nindex_num,k+1,v,INT_MAX);
                                break;
                            }
                            case 2:{
                                ADD_INDEX(index_set_null,nindex_num,k,v,position);
                                ADD_INDEX(index_set_null,nindex_num,k+1,v,INT_MAX);
                                break;
                            }
                            case 1:{
                                index_set_null[nindex_num-1].storage_position = position;
                                ADD_INDEX(index_set_null,nindex_num,k-1,v,INT_MAX);
                                ADD_INDEX(index_set_null,nindex_num,k+1,v,INT_MAX);
                                break;
                            }
                            case 0:{
                                ADD_INDEX(index_set_null,nindex_num,k-1,v,INT_MAX);
                                ADD_INDEX(index_set_null,nindex_num,k,v,position);
                                ADD_INDEX(index_set_null,nindex_num,k+1,v,INT_MAX);
                            }
                        }
                    }
                    else
                    {
                        ADD_INDEX(index_set_null,nindex_num,k-1,v,INT_MAX);
                        ADD_INDEX(index_set_null,nindex_num,k,v,position);
                        ADD_INDEX(index_set_null,nindex_num,k+1,v,INT_MAX);
                    }
                }
            }
            else{
                int count = 0;
                int next_num = affine_wavefronts->graph->node[v].next_num;
                int* next_node = affine_wavefronts->graph->node[v].next;
                for (int node_num = 0; node_num < next_num; node_num++)
                {
                    int w = next_node[node_num];
                    if (text[i+1]==affine_wavefronts->graph->node[w].node_pattern[0])
                    {
                        ADD_INDEX(index_set,index_num,i+1,w,INT_MAX);
                        ++count;
                    }
                    else{
                        ADD_INDEX(index_set_null,nindex_num,i,w,INT_MAX);
                        ADD_INDEX(index_set_null,nindex_num,i+1,w,INT_MAX);
                    }
                }
                if (next_num==0||count<next_num)
                {
                    ADD_INDEX(index_set_null,nindex_num,k+1,v,INT_MAX);
                }
            }            
        }
        sort(index_set_null,index_set_null+nindex_num,sort_index);
        nindex_num = unique(index_set_null,index_set_null+nindex_num,unique_index)-(int)index_set_null;
        for(n=0;n<nindex_num;n++)
        {
            if (index_set_null[n].storage_position >= affine_wavefronts->index_storage_num)
            {
                index_set_null[n].storage_position = affine_wavefronts->index_storage_num;
                affine_wavefronts->index_storage_num++;
            }
        }
        affine_wavefronts->index_set_num = nindex_num;
        affine_wavefronts->index_set = index_set_null;
        affine_wavefronts->index_set_null = index_set;
    }