#ifndef AFFINE_WAVEFRONT_ALIGN_H_
#define AFFINE_WAVEFRONT_ALIGN_H_

#include "affine_wavefront.h"
#include "../utils/commons.h"
#include "affine_wavefront_utils.h"
#include "affine_wavefront_extend.h"
#include "utils/string_padded.h"

void affine_wavefronts_fetch_wavefronts(
    affine_wavefronts_t* const affine_wavefronts,
    affine_wavefront_set* const wavefront_set,
    const int score);

void affine_wavefronts_allocate_wavefronts(
    affine_wavefronts_t* const affine_wavefronts,
    affine_wavefront_set* const wavefront_set,
    const int score);

void affine_wavefront_fetch_index(
    affine_wavefronts_t* affine_wavefronts, 
    int score, affine_wavefront_index_set* index_set);

void affine_wavefront_new_index_set_idm(
    affine_wavefront_index_set* index_set, 
    affine_wavefronts_t* affine_wavefronts);

void affine_wavefront_new_index_set_dm(
    affine_wavefront_index_set* index_set, 
    affine_wavefronts_t* affine_wavefronts);

void affine_wavefront_new_index_set_im(
    affine_wavefront_index_set* index_set, 
    affine_wavefronts_t* affine_wavefronts);

void affine_wavefront_new_index_set_m(
    affine_wavefront_index_set* index_set, 
    affine_wavefronts_t* affine_wavefronts);

void affine_wavefronts_compute_offsets_idm(
    affine_wavefronts_t* const affine_wavefronts,
    affine_wavefront_set* const wavefront_set,
    const affine_wavefront_index_set* index_set,
    int score);

void affine_wavefronts_compute_offsets_dm(
    affine_wavefronts_t* const affine_wavefronts,
    affine_wavefront_set* const wavefront_set,
    const affine_wavefront_index_set* index_set,
    int score);

void affine_wavefronts_compute_offsets_im(
    affine_wavefronts_t* const affine_wavefronts,
    affine_wavefront_set* const wavefront_set,
    const affine_wavefront_index_set* index_set,
    int score);

void affine_wavefronts_compute_offsets_m(
    affine_wavefronts_t* const affine_wavefronts,
    affine_wavefront_set* const wavefront_set,
    const affine_wavefront_index_set* index_set,
    int score);

void affine_wavefronts_compute_offsets(
    affine_wavefronts_t* const affine_wavefronts,
    const affine_wavefront_set* const wavefront_set,
    const affine_wavefront_index_set* index_set,
    int score);

void affine_wavefronts_align(
    affine_wavefronts_t* const affine_wavefronts,
    const graph_t* const graph,
    const char* const text,
    const int text_length, 
    int alignment_v);
#endif /* AFFINE_WAVEFRONT_ALIGN_H_ */
