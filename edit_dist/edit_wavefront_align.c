// DESCRIPTION: Graph Wavefront Alignment on unit cost

#include "../edit_dist/edit_wavefront_align.h"

void edit_wavefronts_align(
    edit_wavefronts_t* const edit_wavefronts,
    const graph_t* const graph,
    const char* const text,
    const int text_length,
    int alignment_v){
        // Init padded strings
        strings_padded_t* const strings_padded = strings_padded_new_rhomb(graph, text, text_length, AFFINE_WAVEFRONT_PADDING, edit_wavefronts->mm_allocator);
        // Initialize wavefront
        EDIT_ADD_INDEX(edit_wavefronts->index_set
        , edit_wavefronts->index_set_num, 0, 0, 0, false);
        edit_wavefronts->visit[0][0] = true;
        // Compute wavefronts for increasing score
        int score = 0;
        int alignment_k = EDIT_WAVEFRONT_DIAGONAL(text_length, edit_wavefronts->graph->node[alignment_v].pattern_length);

        /*
        printf("%d\n",alignment_k);
        printf("%d\n",alignment_v);
        printf("%d\n",text_length);
        printf("%d\n",edit_wavefronts->graph->node[alignment_v].pattern_length);
        */
       
        while (true){
            // Extend wavefront
            edit_wavefront_extend(edit_wavefronts, strings_padded->text_padded, strings_padded->graph_padded, alignment_v, alignment_k);
            // Finalize
            if (edit_wavefronts->final_status) break;
            // Increase score
            ++score;
            for (int i = 0; i < edit_wavefronts->index_set_num; i++)
            {
                printf("%d\t%d\t%d\n",edit_wavefronts->index_set[i].segment_index,edit_wavefronts->index_set[i].diagonal_index,edit_wavefronts->index_set[i].offset);
            }
            printf("%d\n",score);
        }
    }