// DESCRIPTION: Wavefront extend on unit cost

#include "edit_wavefront_extend.h"

void edit_wavefront_extend(
    edit_wavefronts_t* const edit_wavefronts,
    const char* const text,
    const graph_padded_t* const graph,
    const int alignment_v, 
    const int alignment_k){
        edit_wavefront_index_t* pre_idx = edit_wavefronts->index_set;
        edit_wavefront_index_t* aft_idx = edit_wavefronts->index_set_null;
        int n = edit_wavefronts->index_set_num;
        int aft_num = 0;
        int t = 0;
        while (t < n)
        {
            edit_wavefront_index_t index = pre_idx[t++];
            if (index.full)
            {
                aft_idx[aft_num++] = index;
                continue;
            }
            int k = index.diagonal_index;
            int v = index.segment_index;
            int offset = index.offset;
            int i =EDIT_WAVEFRONT_I(k,offset);
            int h = EDIT_WAVEFRONT_H(k,offset);
            if (h > edit_wavefronts->text_length)
            {
                aft_idx[aft_num++] = index;
                continue;
            }
            const char* segment = graph->node[v].pattern_padded;
            if (i < edit_wavefronts->graph->node[v].pattern_length - 1)
            {
                // Fetch pattern/text blocks
                uint64_t* pattern_blocks = (uint64_t*)(segment+i);
                uint64_t* text_blocks = (uint64_t*)(text + h);
                uint64_t pattern_block = *pattern_blocks;
                uint64_t text_block = *text_blocks;
                // Compare 64-bits blocks
                uint64_t cmp = pattern_block ^ text_block;
                while(__builtin_expect(!cmp,0)){
                    // Increment offset (full block)
                    index.offset;
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
                index.offset += equal_chars;
                offset = index.offset;
                if (index.offset == edit_wavefronts->graph->node[v].pattern_length - 1)
                {
                    pre_idx[n++] = index;
                }
                else 
                {
                    if (aft_num > 1)
                    {
                        int kernel = ((aft_idx[aft_num - 2].diagonal_index == k - 1) && aft_idx[aft_num - 2].segment_index == v)<<1 | ((aft_idx[aft_num - 1].diagonal_index == k) && (aft_idx[aft_num - 1].segment_index == v));
                        switch (kernel)
                        {
                            case 3: {
                                aft_idx[aft_num - 2].offset = MAX(aft_idx[aft_num - 2].offset, index.offset);
                                aft_idx[aft_num - 1].offset = MAX(aft_idx[aft_num - 1].offset + 1, index.offset);
                                aft_idx[aft_num - 1].full = aft_idx[aft_num - 1].full | index.full;
                                EDIT_ADD_INDEX(aft_idx, aft_num, k + 1, v, offset + 1, false);
                                break;
                            }
                            case 2: {
                                aft_idx[aft_num - 2].offset = MAX(aft_idx[aft_num - 2].offset, index.offset);
                                index.offset ++;
                                aft_idx[aft_num++] = index;
                                EDIT_ADD_INDEX(aft_idx, aft_num, k + 1, v, offset + 1, false);
                                break;
                            }
                            case 1: {
                                aft_idx[aft_num - 1].offset = MAX(aft_idx[aft_num - 1].offset, index.offset + 1);
                                aft_idx[aft_num - 1].full = aft_idx[aft_num - 1].full | index.full;
                                EDIT_ADD_INDEX(aft_idx, aft_num, k - 1, v, offset, false);
                                EDIT_ADD_INDEX(aft_idx, aft_num, k + 1, v, offset + 1, false);
                            }
                            case 0:{
                                EDIT_ADD_INDEX(aft_idx, aft_num, k - 1, v, offset, false);
                                index.offset++;
                                aft_idx[aft_num++] = index;
                                EDIT_ADD_INDEX(aft_idx, aft_num, k + 1, v, offset + 1, false);
                            }
                        }
                    }
                    else
                    {
                        EDIT_ADD_INDEX(aft_idx, aft_num, k - 1, v, offset, false);
                        index.offset ++;
                        aft_idx[aft_num++] = index;
                        EDIT_ADD_INDEX(aft_idx, aft_num, k + 1, v, offset + 1, false);
                    }
                }
            }
            else
            {
                if (v == alignment_v && k == alignment_k)
                {
                    edit_wavefronts->final_status = true;
                    return;
                }
                for (int j = 0; j < edit_wavefronts->graph->node[v].next_num; j++)
                {
                    int w = edit_wavefronts->graph->node[v].next[j];
                    if (text[h + 1] == edit_wavefronts->graph->node[w].node_pattern[0])
                    {
                        int k_w = EDIT_WAVEFRONT_DIAGONAL(h+1, 0);
                        EDIT_ADD_INDEX(pre_idx,n,k_w,w,0,false);
                    }
                    else
                    {
                        int k_w = EDIT_WAVEFRONT_DIAGONAL(h+1, 0);
                        EDIT_ADD_INDEX(aft_idx,aft_num,k_w,w,0,false);
                        EDIT_ADD_INDEX(aft_idx,aft_num,k_w+1,w,0,false);
                    }
                }
                EDIT_ADD_INDEX(aft_idx,aft_num,k-1,v,offset,true);
                EDIT_ADD_INDEX(aft_idx,aft_num,k,v,offset,true);
            }
        }
        // Sort
        edit_sort(aft_idx, aft_idx + aft_num);
        aft_num = edit_unique(aft_idx, aft_idx + aft_num) - aft_idx;
        // printf('%d\n', aft_num);
        // update visited
        for (int j = 0; j < aft_num; j++)
        {
            int v = aft_idx[j].segment_index;
            int k = aft_idx[j].diagonal_index;
            edit_wavefronts->visit[v][k + edit_wavefronts->graph->node[v].pattern_length - 1] = true;
        }
        // Exchange
        edit_wavefronts->index_set = aft_idx;
        edit_wavefronts->index_set_null = pre_idx;
        edit_wavefronts->index_set_num = aft_num;
    }