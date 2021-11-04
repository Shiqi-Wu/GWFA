/*
 * Pattern Graph
 */

typedef struct{
	int fragment_length;
	int* base;
	int edge_num;
	int* next_fragment;
}pattern_fragment;

typedef struct{
	int fragment_num;
	pattern_fragment* fragment;
}graph;

/*
 * Penalty
 */
typedef struct{
	int mismatch;
	int gap_extention;
	int gap_open;
}penalty;

/*
 * Position index
 */
typedef struct{
	int fragment_rank;
	int diagnal_rank;
}index;

/*
 *Wavefront set
 */
 
/* typedef struct{
	bool null;
	//bool diagonal_null;
	int offset;
}affine_wavefront_t; */

typedef struct{
	//Dimensions
	//int fragment_rank;
	bool* null;
	//int fragment_length;		//Pattern length
	int num_wavefronts;		//Total number of allocatable wavefronts
	//int diagnal_range;
	int* offset;
}affine_wavefronts_t;

typedef struct{
	//Dimensions
	affine_wavefronts_t* out_mwavefronts;
	affine_wavefronts_t* out_iwavefronts;
	affine_wavefronts_t* out_dwavefronts;
	affine_wavefronts_t* in_mwavefront_sub;
	affine_wavefronts_t* in_iwavefront_gap;
	affine_wavefronts_t* in_iwavefront_ext;
	affinr_wavefronts_t* in_dwavefront_gap;
	affinr_wavefronts_t* in_dwavefront_ext;
	index* out_mcompute_diagonal_set;
	index* out_icompute_diagonal_set;
	index* out_dcompute_diagonal_set;
	index* in_mcompute_set_sub;
	index* in_icompute_set_gap;
	index* in_icompute_set_ext;
	index* in_dcompute_set_gap;
	index* in_dcompute_set_ext;
	int out_mindex_num;
	int out_iindex_num;
	int out_dindex_num;
	int in_mindex_num_sub;
	int in_iindex_num_gap;
	int in_iindex_num_ext;
	int in_dindex_num_gap;
	int in_dindex_num_ext;
	int allocate_num_index;		//index to be allocated
}graph_affine_wavefront_set

typedef struct{
	//Dimensions
	int text_length;		//Text length
	int allocate_wavefront_set;
	graph_affine_wavefront_set* graph_wavefronts;
}graph_affine_wavefronts_set