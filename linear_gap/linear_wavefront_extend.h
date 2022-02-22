#ifndef LINEAR_WAVEFRONT_EXTEND_H_
#define LINEAR_WAVEFRONT_EXTEND_H_

#include "linear_wavefront.h"
#include "../utils/strings_padded.h"

// Linear Gap Wavefront exact extension

int linear_wavefronts_extend_inner(
    linear_wavefronts_t* const linear_wavefronts,
    const char* const text,
    graph_padded_t* const graph,
    const int node_idx,
    const int h,
    int offset);

void linear_wavefronts_extend_graph(
    linear_wavefronts_t* const linear_wavefronts,
    linear_wavefront_t* const wavefront,
    const char* const text,
    graph_padded_t* const graph,
    const int node_idx,
    const int h);

void linear_wavefronts_extend_graph(
    linear_wavefronts_t* const linear_wavefronts,
    linear_wavefront_t* const wavefront,
    const char* const text,
    graph_padded_t* const graph,
    const int node_idx,
    const int h);

#endif