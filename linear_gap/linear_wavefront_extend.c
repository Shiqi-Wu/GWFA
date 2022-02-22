#include "linear_wavefront_extend.h"

int linear_wavefronts_extend_inner(
    linear_wavefronts_t* const linear_wavefronts,
    const char* const text,
    graph_padded_t* const graph,
    const int node_idx,
    const int h,
    int offset){
        const char* segment = graph->node[node_idx].pattern_padded_buffer;
        // Fetch pattern/text blocks
        uint64_t* pattern_blocks = (uint64_t*)(segment + offset);
        uint64_t* text_blocks = (uint64_t*)(text + h);
        uint64_t pattern_block = *pattern_blocks;
        uint64_t text_block = *text_blocks;
        // Compare 64-bits blocks
        uint64_t cmp = pattern_block ^ text_block;
        while(__builtin_expect(!cmp,0)){
            // Increment offset (full block)
            offset += 8;
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
        offset += equal_chars;
        return offset;
    }

void linear_wavefronts_extend_graph(
    linear_wavefronts_t* const linear_wavefronts,
    linear_wavefront_t* const wavefront,
    const char* const text,
    graph_padded_t* const graph,
    const int node_idx,
    const int h){
        int next_num = linear_wavefronts->graph->node[node_idx].next_num;
        int* next_node = linear_wavefronts->graph->node[node_idx].next;
        linear_wavefront_indexs_t* idx_set = linear_wavefronts->idx_set;
        for (int n = 0; n < next_num; n++)
        {
            int w = next_node[n];
            if (wavefront[w].offsets!=NULL && wavefront[w].hi >= h+1 && wavefront[w].lo <= h && wavefront[w].offset[2*h-1] > 0)
                continue;
            if (text[h+1] == linear_wavefronts->graph->node[w].node_pattern[0])
            {
                idx_set->idx[idx_set->idx_num].diagonal_index = h;
                idx_set->idx[idx_set->idx_num].segment_index = w;
                idx_set->idx[idx_set->idx_num].offset = 1;
                idx_set->idx_num++;
            }
            else
            {
                idx_set->idx[idx_set->idx_num].diagonal_index = h;
                idx_set->idx[idx_set->idx_num].segment_index = w;
                idx_set->idx[idx_set->idx_num].offset = 0;
                idx_set->idx_num++;
            }
        }
    }

void linear_wavefronts_extend_wavefront_compute_packed(
    linear_wavefronts_t* const linear_wavefronts,
    const char* const text,
    graph_padded_t* const graph,
    const int score,
    const int alignment_v,
    const int alignment_k){
        // Fetch wavefront
        linear_wavefront_t* const wavefront = linear_wavefronts->wavefronts[score];
        if (wavefront == NULL) return;
        // Extend diagonally each wavefront point
        linear_wavefront_indexs_t* idx_set = linear_wavefronts->idx_set;
        idx_set->idx_num = 0;
        int offset, h, k, hi, lo, node_idx;
        for (int n = 0; n < linear_wavefronts->node; n++)
        {
            node_idx = linear_wavefronts->node[n]; 
            int kernel = (wavefront[node_idx].hi > 0)<<1 | (wavefront[node_idx].lo > 0));
            hi = wavefront[node_idx].hi; lo = wavefront[node_idx].lo;
            const int segment_length = affine_wavefronts->graph->node[node_idx].node_pattern;
            switch (kernel)
            {
                case 0: 
                {
                    for (k = lo; k <= hi; k++)
                    {
                        offset = wavefront[node_idx].offsets[-2*j];
                        if (offset < 0)
                            continue;
                        h = LINEAR_WAVEFRONT_H(k,offset);
                        if (h >= linear_wavefronts->text_length)
                            continue;
                        if (offset < segment_length){
                            offset = linear_wavefronts_extend_inne, text, graph, node_idx, h, offset);
                            wavefront[node_idx].offsets[-2*j] = offset;
                            if (offset == segment_length)
                            {
                                idx_set->idx[idx_set->idx_num].diagonal_index = k;
                                idx_set->idx[idx_set->idx_num].segment_index = node_idx;
                                idx_set->idx[idx_set->idx_num].offset = offset;
                                idx_set->idx_num++;
                            }
                        }
                        else
                        {
                            if (node_idx == alignment_v && k == alignment_k)
                            {
                                linear_wavefronts->final_status == true;
                                return;
                            }
                            linear_wavefronts_extend_graph(linear_wavefronts,wavefront,text,graph,node_idx,h);
                        }
                    }
                }
                case 1:
                {
                    continue;
                }
                case 2:
                {
                    for (k = lo; k <= 0; k++)
                    {
                        offset = wavefront[node_idx].offsets[-2*j];
                        if (offset < 0)
                            continue;
                        h = LINEAR_WAVEFRONT_H(k,offset);
                        if (h >= linear_wavefronts->text_length)
                            continue;
                        if (offset < segment_length){
                            offset = linear_wavefronts_extend_inne, text, graph, node_idx, h, offset);
                            wavefront[node_idx].offsets[-2*j] = offset;
                            if (offset == segment_length)
                            {
                                idx_set->idx[idx_set->idx_num].diagonal_index = k;
                                idx_set->idx[idx_set->idx_num].segment_index = node_idx;
                                idx_set->idx[idx_set->idx_num].offset = offset;
                                idx_set->idx_num++;
                            }
                        }
                        else
                        {
                            if (node_idx == alignment_v && k == alignment_k)
                            {
                                linear_wavefronts->final_status == true;
                                return;
                            }
                            linear_wavefronts_extend_graph(linear_wavefronts,wavefront,text,graph,node_idx,h);
                        }
                    }
                    for (k = 1; k <= hi; k++)
                    {
                        offset = wavefront[node_idx].offsets[2*j-1];
                        if (offset < 0)
                            continue;
                        h = LINEAR_WAVEFRONT_H(k,offset);
                        if (h >= linear_wavefronts->text_length)
                            continue;
                        if (offset < segment_length){
                            offset = linear_wavefronts_extend_inne, text, graph, node_idx, h, offset);
                            wavefront[node_idx].offsets[2*j-1] = offset;
                            if (offset == segment_length)
                            {
                                idx_set->idx[idx_set->idx_num].diagonal_index = k;
                                idx_set->idx[idx_set->idx_num].segment_index = node_idx;
                                idx_set->idx[idx_set->idx_num].offset = offset;
                                idx_set->idx_num++;
                            }
                        }
                        else
                        {
                            if (node_idx == alignment_v && k == alignment_k)
                            {
                                linear_wavefronts->final_status == true;
                                return;
                            }
                            linear_wavefronts_extend_graph(linear_wavefronts,wavefront,text,graph,node_idx,h);
                        }
                    }
                }
                case 3:
                {
                    for (k = lo; k <= hi; k++)
                    {
                        offset = wavefront[node_idx].offsets[2*j-1];
                        if (offset < 0)
                            continue;
                        h = LINEAR_WAVEFRONT_H(k,offset);
                        if (h >= linear_wavefronts->text_length)
                            continue;
                        if (offset < segment_length){
                            offset = linear_wavefronts_extend_inne, text, graph, node_idx, h, offset);
                            wavefront[node_idx].offsets[2*j-1] = offset;
                            if (offset == segment_length)
                            {
                                idx_set->idx[idx_set->idx_num].diagonal_index = k;
                                idx_set->idx[idx_set->idx_num].segment_index = node_idx;
                                idx_set->idx[idx_set->idx_num].offset = offset;
                                idx_set->idx_num++;
                            }
                        }
                        else
                        {
                            if (node_idx == alignment_v && k == alignment_k)
                            {
                                linear_wavefronts->final_status == true;
                                return;
                            }
                            linear_wavefronts_extend_graph(linear_wavefronts,wavefront,text,graph,node_idx,h);
                        }
                    } 
                }
            }
        }
        for (int n = 0; n < linear_wavefronts->idx_set.idx_num; n++)
        {
            node_idx = idx_set->idx[n].segment_index;
            k = idx_set->idx[n].diagonal_index;
            offset = idx_set->idx[n].offset;
            const int segment_length = affine_wavefronts->graph->node[node_idx].node_pattern;
            if (offset < 0)
                continue;
            h = LINEAR_WAVEFRONT_H(k,offset);
            if (h >= linear_wavefronts->text_length)
                continue;
            if (offset < segment_length)
            {
                idx_set->idx[n].offset = linear_wavefronts_extend_inner(linear_wavefronts, text, graph, node_idx, h, offset);
                if (idx_set->idx[n].offset == segment_length)
                {
                    if (node_idx == alignment_v && k == alignment_k)
                    {
                        linear_wavefronts->final_status == true;
                        return;
                    }
                    linear_wavefronts_extend_graph(linear_wavefronts,wavefront,text,graph,node_idx,h);
                }
            }
            else
            {
                if (node_idx == alignment_v && k == alignment_k)
                {
                    linear_wavefronts->final_status == true;
                    return;
                }
                linear_wavefronts_extend_graph(linear_wavefronts,wavefront,text,graph,node_idx,h);
            }
        }
        for (int n = 0; n < linear_wavefronts->idx_set.idx_num; n++)
        {
            node_idx = idx_set->idx[n].segment_index;
            k = idx_set->idx[n].diagonal_index;
            offset = idx_set->idx[n].offset;
            if (wavefront[node_idx].hi<wavefront[node_idx].lo && !linear_wavefronts->node_status[node_idx])
            {
                linear_wavefronts->node[node_num++] = node_idx;
                linear_wavefronts->node_status[node_idx] = true;
            }
            k > 0 ? wavefront[node_idx].offsets[2*k-1] = MAX(offset,wavefront[node_idx].offsets[2*k-1]):wavefront[node_idx].offsets[-2*k] = MAX(offset,wavefront[node_idx].offsets[-2*k]);
            wavefront[node_idx].hi = MAX(k, wavefront[node_idx].hi);
            wavefront[node_idx].lo = MIN(k, wavefront[node_idx].lo);
        }
    }