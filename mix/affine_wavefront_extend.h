#include "../utils/commons.h"
#include "affine_wavefront.h"

index* affine_wavefront_extend(
graph* const pattern, char* const text, int text_length,
graph_affine_wavefront_set* const gmwavefront, index* index_set_extend, bool** status_set);