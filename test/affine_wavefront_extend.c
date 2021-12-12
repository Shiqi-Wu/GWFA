#include "affine_wavefront_extend.h"

index* affine_wavefront_extend(
graph* const pattern, char* const text, int text_length,
graph_affine_wavefront_set* const gmwavefront, index* index_set_extend, bool** status_set)
{
	int fragment_rank;int diagonal_rank;int n;int h;int v;int offset_position;
	int index_set_extend_num = 0;
	int index_set_pre_num = gmwavefront->out_mindex_num;
	index* index_set_pre = gmwavefront->out_mcompute_diagonal_set;
	affine_wavefronts_t* mwavefront = gmwavefront->out_mwavefronts;
	while(fragment_set_pre_num > 0)
	{
		fragment_set_pre_num--;
		fragment_rank = fragment_set_pre[fragment_set_pre_num]->fragment_rank;
		diagonal_rank = fragment_set_pre[fragment_set_pre_num]->diagnal_rank;
		offset_position = mwavefront[fragment_rank]->fragment_length + diagnal_rank;
		h = mwavefront[fragment_rank]-> offset[offset_position];
		int v = h - diagnal_rank;
		while(v < pattern->pattern_fragment[fragment_rank]->fragment_length
		&& h < text_length 
		&& pattern->pattern_fragment[fragment_rank]->base[v]==text[h])
		{
			v++;
			h++;
		}
		mwavefront[fragment_rank]->offset[offset_position]=h;
		n=0;
		if(v==pattern->pattern_fragment[fragment_rank])
		{
			for(int i=0; i < pattern->pattern_fragment[fragment_rank]->edge_num; i++)
			{
				int next_pattern_rank = pattern->pattern_fragment[fragment_rank]->next_fragment[i];
				if (pattern->pattern_fragment[next_pattern_rank]->base[0]==text[h])
				{
					n++;
					offset_position=h + pattern->pattern_fragment[next_pattern_rank]->pattern_length;
					if(!status_set[fragment_rank][offset_position])
					{
						mwavefront[next_pattern_rank]->null[offset_position]=0;
						mwavefront[next_pattern_rank]->offset[offset_position]=0;
						fragment_set_pre[fragment_set_pre_num]->fragment_rank=next_pattern_rank;
						fragment_set_pre[fragment_set_pre_num]->diagnal_rank=h;
						fragment_set_pre_num++;
					}
				}
			}
		}
		if(n==0 || n<pattern->pattern_fragment[fragment_rank]->edge_num)
		{
			index_set_extend[index_set_extend_num]->fragment_rank=fragment_rank;
			index_set_extend[index_set_extend_num]->fragment_rank=diagnal_rank;
			index_set_extend_num++;
		}
	}
	mwavefront->index_num=index_set_extend_num;
	return index_set_extend;
}