#include "linear_wavefront.h"

linear_wavefronts_t* linear_wavefronts_new(
    const int pattern_length,
    const int text_length,
    linear_wavefront_penalty_t* const penalties,
    mm_allocator_t* const mm_allocator,
    graph_t* graph){
        // Allocate
        linear_wavefronts_t* const linear_wavefronts = mm_allocator_alloc(mm_allocator,linear_wavefront_t);
        // Dimensions
        const int max_score_misms = MIN(pattern_length, text_length) * penalties->mismatch;
        const int max_score_indel = ABS(pattern_length - text_length) * penalties->gap;
        const int num_wavefronts = max_score_indel + max_score_misms;
        linear_wavefronts->num_wavefronts = num_wavefronts;
        // MM
        linear_wavefronts->mm_allocator = mm_allocator;
        // Limits
        const int max_penalty = MAX(penalties->mismatch, penalties->gap);
        // Graph
        linear_wavefronts->graph = graph;
        
    }