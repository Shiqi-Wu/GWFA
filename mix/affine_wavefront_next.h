#include "../utils/commons.h"
#include "affine_wavefront.h"

void fench_wavefront(
graph_affine_wavefronts_set* const graph_wavefront_set, int score, penalty affine_penalty);

void affine_wavefront_next(
graph* const pattern, char* const text, int text_length,
graph_affine_wavefront_set* const mwavefront, bool*** status_set);