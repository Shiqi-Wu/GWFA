// DESCRIPTION: Graph Wavefront alignment algorithm for unit cost
#include "edit_dist/edit_wavefront.h"

edit_wavefronts_t* edit_wavefronts_new(
    const int pattern_length,
    const int text_length,
    mm_allocator_t* const mm_allocator,
    graph_t* graph){
        // Allocate
        edit_wavefronts_t* const edit_wavefronts = mm_allocator_alloc(mm_allocator, edit_wavefronts_t);
        int wavefront_num = (text_length + 1) * graph->node_num + graph->pattern_length;
        edit_wavefronts->index_set = mm_allocator_calloc(mm_allocator, wavefront_num, edit_wavefront_index_t, true);
        edit_wavefronts->index_set_null =mm_allocator_calloc(mm_allocator, wavefront_num, edit_wavefront_index_t, false);
        edit_wavefronts->visit = mm_allocator_calloc(mm_allocator, graph->node_num, bool*, false);
        for(int i = 0; i < graph->node_num; i++)
        {
            edit_wavefronts->visit[i] = mm_allocator_calloc(mm_allocator, text_length + 1 + graph->node[i].pattern_length
            , bool, true);
        }
        // Initialize
        edit_wavefronts->index_set_num = 0;
        edit_wavefronts->final_status = false;
        // MM
        edit_wavefronts->mm_allocator = mm_allocator;
        // Graph and text
        edit_wavefronts->graph = graph;
        edit_wavefronts->text_length = text_length;
        // Return
        return edit_wavefronts;
    }

void edit_wavefronts_clear(edit_wavefronts_t* const edit_wavefronts){
    // Clear wavefronts
    mm_allocator_t* const mm_allocator = edit_wavefronts->mm_allocator; 
    if (edit_wavefronts->index_set!=NULL)
    {
        mm_allocator_free(mm_allocator, edit_wavefronts->index_set);
    }
    if (edit_wavefronts->index_set_null!=NULL)
    {
        mm_allocator_free(mm_allocator, edit_wavefronts->index_set_null);
    }
    // Clear visit table
    for (int i = 0; i < edit_wavefronts->graph->node_num; i++)
    {
        mm_allocator_free(mm_allocator, edit_wavefronts->visit[i]);
    }
}

void edit_wavefronts_delete(edit_wavefronts_t* const edit_wavefronts)
{
    // Parameters
    mm_allocator_t* const mm_allocator = edit_wavefronts->mm_allocator;
    // Clear wavefronts and visited
    edit_wavefronts_clear(edit_wavefronts);
    // Clear Graph
    edit_wavefronts->graph = NULL;
    // Free visit table
    mm_allocator_free(mm_allocator, edit_wavefronts->visit);
    // Free hanlder
    mm_allocator_free(mm_allocator, edit_wavefronts);
}
