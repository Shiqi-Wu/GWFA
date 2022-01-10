//DESCRIPTION: Graph Wavefront alignment algorithm for pairwise gap-affine alignment
#include "affine_gap/affine_wavefront.h"

void affine_wavefronts_allocate_wavefront_null(affine_wavefronts_t* const affine_wavefronts) {
    // Allocate null wavefront
    const int wavefront_length = affine_wavefronts->graph->pattern_length + affine_wavefronts->graph->node_num * (affine_wavefronts->text_length - 1);
    awf_offset_t* const offsets_null = mm_allocator_calloc(affine_wavefronts->mm_allocator,wavefront_length,awf_offset_t,false);
    // Initialize
    affine_wavefronts->wavefront_null.null = true;
    affine_wavefronts->wavefront_null.offsets = offsets_null;
    int i;
    for (i=0;i<wavefront_length;++i){
        offsets_null[i] = AFFINE_WAVEFRONT_OFFSET_NULL;
    }
    // Allocate index set
    affine_wavefronts->index_set_num = 0;
    affine_wavefronts->index_set = mm_allocator_calloc(affine_wavefronts->mm_allocator,affine_wavefronts->num_wavefronts,affine_wavefront_index_t,false);
}

void affine_wavefront_allocate_wavefront_components(affine_wavefronts_t* const affine_wavefronts){
    // Parameters
    mm_allocator_t* const mm_allocator = affine_wavefronts->mm_allocator;
    // Initialize wavefronts
    affine_wavefronts->mwavefronts = mm_allocator_calloc(mm_allocator,affine_wavefronts->num_wavefronts,affine_wavefronts_t*,true);
    affine_wavefronts->iwavefronts = mm_allocator_calloc(mm_allocator, affine_wavefronts->num_wavefronts, affine_wavefront_t*, true);
    affine_wavefronts->dwavefronts = mm_allocator_calloc(mm_allocator,affine_wavefronts->num_wavefronts, affine_wavefront_t*, true);
    // Allocate bulk-memory (for all wavefronts)
    const int num_wavefronts = affine_wavefronts->num_wavefronts;
    affine_wavefront_t* const wavefronts_mem = mm_allocator_calloc(mm_allocator, 3*num_wavefronts,affine_wavefront_t,false);
    affine_wavefronts->wavefronts_mem = wavefronts_mem;
    affine_wavefronts->wavefronts_current = wavefronts_mem;
    // Initialize full index sets
    affine_wavefronts->full_dindex = mm_allocator_calloc(mm_allocator, affine_wavefronts->num_wavefronts, affine_wavefront_index_t*, true);
    affine_wavefronts->full_mindex = mm_allocator_calloc(mm_allocator, affine_wavefronts->num_wavefronts, affine_wavefront_index_t*, true);
    // Initialize position matrix
    affine_wavefronts->position_table = mm_allocator_calloc(mm_allocator, affine_wavefronts->graph->node_num, int*, true);
    int i;
    for (i=1; i<affine_wavefronts->graph->node_num; i++)
    {
        affine_wavefronts->position_table[i] = NULL;
    }
}

void affine_wavefront_allocate_columns(affine_wavefronts_t* const affine_wavefronts, int node_idx){
    affine_wavefronts->position_table[node_idx] = mm_allocator_calloc(affine_wavefronts->mm_allocator, affine_wavefronts->graph->node[node_idx].pattern_length + affine_wavefronts->text_length + 1, int, true);
    int i;
    for (i=0; i < affine_wavefronts->graph->node[node_idx].pattern_length + affine_wavefronts->text_length + 1; i++)
    {
        affine_wavefronts->position_table[node_idx][i] = -1;
    }
}

affine_wavefronts_t* affine_wavefronts_new(
    const int pattern_length,
    const int text_length,
    affine_wavefront_penalties_t* const penalties,
    mm_allocator_t* const mm_allocator,
    graph_t* graph){
        // Allocate
        affine_wavefronts_t* const affine_wavefronts = mm_allocator_alloc(mm_allocator,affine_wavefronts_t);
        // Dimensions
        const int max_score_misms = MIN(pattern_length, text_length) * penalties->mismatch;
        const int max_score_indel = penalties->gap_opening + ABS(pattern_length-text_length) * penalties->gap_extension;
        const int num_wavefronts = max_score_misms + max_score_indel;
        affine_wavefronts->num_wavefronts = num_wavefronts;
        // MM
        affine_wavefronts->mm_allocator = mm_allocator;
        // Graph
        affine_wavefronts->graph = graph;
        // Limits
        const int single_gap_penalty = penalties->gap_opening + penalties->gap_extension;
        const int max_penalty = MAX(penalties->mismatch,single_gap_penalty);
        affine_wavefronts_allocate_wavefront_componets(affine_wavefronts);
        affine_wavefronts_allocate_wavefront_null(affine_wavefronts);
        affine_wavefront_allocate_columns(affine_wavefronts,0);
        #ifdef AFFINE_WAVEFRONT_DEBUG
        affine_table_allocate(&affine_wavefronts->gap_affine_table,pattern_length,text_length,mm_allocator);
        int h, v;
        for (h=0;h<=text_length;++h) {
            for (v=0;v<=pattern_length;++v) {
            affine_wavefronts->gap_affine_table.columns[h][v].M = -1;
            affine_wavefronts->gap_affine_table.columns[h][v].D = -1;
            affine_wavefronts->gap_affine_table.columns[h][v].I = -1;
            }
        }
        #endif
        // Return
        return affine_wavefronts;
    }

 void affine_wavefronts_clear(affine_wavefronts_t* const affine_wavefronts){
    // Clear wavefronts
    mm_allocator_t* const mm_allocator = affine_wavefronts->mm_allocator;
    affine_wavefront_t** const mwavefronts = affine_wavefronts->mwavefronts;
    affine_wavefront_t** const iwavefronts = affine_wavefronts->iwavefronts;
    affine_wavefront_t** const dwavefronts = affine_wavefronts->dwavefronts;
    int i;
    for (i=0;i<affine_wavefronts->num_wavefronts;++i){
        if (mwavefronts[i]!=NULL){
            mm_allocator_free(mm_allocator,mwavefronts[i]->offsets);
            mwavefronts[i] = NULL;
        }
        if (dwavefronts[i]!=NULL){
            mm_allocator_free(mm_allocator,dwavefronts[i]->offsets);
            dwavefronts[i] = NULL;
        }
        if (iwavefronts[i]!=NULL){
            mm_allocator_free(mm_allocator,iwavefronts[i]->offsets);
            iwavefronts[i] = NULL;
        }
    }
}

void affine_wavefronts_delete(affine_wavefronts_t* const affine_wavefronts){
    // Parameters
    mm_allocator_t* const mm_allocator = affine_wavefronts->mm_allocator;
    // Clear wavefronts
    affine_wavefronts_clear(affine_wavefronts);
    // Free MID-Wavefronts
    mm_allocator_free(mm_allocator,affine_wavefronts->mwavefronts);
    mm_allocator_free(mm_allocator,affine_wavefronts->iwavefronts);
    mm_allocator_free(mm_allocator,affine_wavefronts->dwavefronts);
    mm_allocator_free(mm_allocator,affine_wavefronts->wavefront_null.offsets );
    mm_allocator_free(mm_allocator,affine_wavefronts->index_set);
    // Free bulk memory
    mm_allocator_free(mm_allocator,affine_wavefronts->wavefronts_mem);
    // DEBUG
    #ifdef AFFINE_WAVEFRONT_DEBUG
        affine_table_free(&affine_wavefronts->gap_affine_table,mm_allocator);
    #endif
    // Free handler
    mm_allocator_free(mm_allocator,affine_wavefronts);
}

affine_wavefront_t* affine_wavefronts_allocate_wavefront(affine_wavefronts_t* const affine_wavefronts,const int index_num){
    affine_wavefront_t* const wavefront = affine_wavefronts->wavefronts_current;
    ++(affine_wavefronts->wavefronts_current);
    // Configure offsets
    wavefront->null = false;
    wavefront->offset_num = index_num;
    awf_offset_t* const offsets_mem = mm_allocator_calloc(affine_wavefronts->mm_allocator, index_num, awf_offset_t, false);
    wavefront->offsets = offsets_mem;
    // Return
    return wavefront;
}

