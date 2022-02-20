// DESCRIPTION: Graph Wavefront Alignment on unit cost

#ifndef EDIT_WAVEFRONT_ALIGN_H_
#define EDIT_WAVEFRONT_ALIGN_H_

#include "edit_wavefront.h"
#include "../utils/commons.h"
#include "edit_wavefront_extend.h"
#include "../utils/string_padded.h"

void edit_wavefronts_align(
    edit_wavefronts_t* const edit_wavefronts,
    const graph_t* const graph,
    const char* const text,
    const int text_length,
    int alignment_v);
    
#endif