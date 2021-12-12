//DESCRIPTION: 
#include "affine_gap/affine_wavefront.h"

affine_wavefronts_allocate_wavefront_componets(affine_wavefronts_t* const affine_wavefronts){
    // Parameters
    mm_allocator_t* const mm_allocator = affine_wavefront->mm_allocator;
    // Initialize wavefronts
    affine_wavefronts->mwavefronts = mm_allocator_calloc(mm_allocator,affine_wavefronts->num_wavefronts,affine_wavefront_t*,true);
    // Allocate bulk-memory (for all wavefronts)
    const int num_wavefront = affine_wavefronts->num_wavefronts;
    affine_wavefront_t* const wavefronts_mem = mm_allocator_

}

