#include "edit_wavefront_align.h"

int main()
{
    // Patter & Text
    char* pattern = "TCTTTACTCGCGCGTTGGAGAAATACAATAGT";
    char* text    = "TCTATACTGCGCGTTTGGAGAAATAAAATAGT";
    // Allocate MM
    mm_allocator_t* const mm_allocator = mm_allocator_new(BUFFER_SIZE_8M);
    // Init graph
    graph_t* graph = (graph_t*)malloc(sizeof(graph_t));
    graph->node_num = 1;
    graph->node = (node_t*)malloc(sizeof(node_t));
    graph->node[0].node_idx = 0;
    graph->node[0].pattern_length = strlen(pattern);
    graph->node[0].next = NULL;
    graph->node[0].next_num = 0;
    graph->node[0].node_pattern = pattern;
    // Init Edit-WFA
    edit_wavefronts_t* edit_wavefronts = edit_wavefronts_new(strlen(pattern), strlen(text), mm_allocator, graph);
    // Align
    edit_wavefronts_align(edit_wavefronts, graph, text, strlen(text), 0);
    free(graph->node);
    free(graph);
    return 0;
}