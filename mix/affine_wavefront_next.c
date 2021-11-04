#include "affine_wavefront_next.h"

void fench_wavefront(
graph_affine_wavefronts_set* const graph_wavefront_set, int score, penalty affine_penalty)
{
	
	int mismatch_score=score-affine_penalty.mismatch;
	int ext_score=score-affine_penalty.gap_extention;
	int ope_score=score-affine_penalty.gap_open-affine_penalty.gap_extention;
	if(graph_wavefront_set[mismatch_score]->out_mindex_num>0)
	{
		graph_wavefront_set[score]->in_mindex_num_sub=graph_wavefront_set[mismatch_score]->out_mindex_num;
		graph_wavefront_set[score]->in_mcompute_set_sub=graph_wavefront_set[mismatch_score]->out_mcompute_diagonal_set;
		graph_wavefront_set[score]->in_mwavefront_sub=graph_wavefront_set[mismatch_score]->out_mwavefronts;
	}
	if(graph_wavefront_set[ope_score]->out_iindex_num>0)
	{
		graph_wavefront_set[score]->in_iindex_num_gap=graph_wavefront_set[ope_score]->out_mindex_num;
		graph_wavefront_set[score]->in_icompute_set_gap=graph_wavefront_set[ope_score]->out_mcompute_diagonal_set;
		graph_wavefront_set[score]->in_iwavefront_gap=graph_wavefront_set[ope_score]->out_mwavefronts;
	}
	if(graph_wavefront_set[ext_score]->ext_iindex_num>0)
	{
		graph_wavefront_set[score]->in_iindex_num_ext=graph_wavefront_set[ext_score]->out_iindex_num;
		graph_wavefront_set[score]->in_icompute_set_ext=graph_wavefront_set[ext_score]->out_icompute_diagonal_set;
		graph_wavefront_set[score]->in_iwavefront_ext=graph_wavefront_set[ext_score]->out_iwavefronts;
	}
	if(graph_wavefront_set[ope_score]->out_dindex_num>0)
	{
		graph_wavefront_set[score]->in_dindex_num_gap=graph_wavefront_set[ope_score]->out_mindex_num;
		graph_wavefront_set[score]->in_dcompute_set_gap=graph_wavefront_set[ope_score]->out_mcompute_diagonal_set;
		graph_wavefront_set[score]->in_dwavefront_gap=graph_wavefront_set[ope_score]->out_mwavefronts;
	}
	if(graph_wavefront_set[ext_score]->ext_dindex_num>0)
	{
		graph_wavefront_set[score]->in_dindex_num_gap=graph_wavefront_set[ext_score]->out_dindex_num;
		graph_wavefront_set[score]->in_dcompute_set_ext=graph_wavefront_set[ext_score]->out_dcompute_diagonal_set;
		graph_wavefront_set[score]->in_dwavefront_ext=graph_wavefront_set[ext_score]->out_dwavefronts;
	}
}

void affine_wavefront_next(
graph* const pattern, char* const text, int text_length,
graph_affine_wavefront_set* const mwavefront, bool*** status_set)
{
	int mwavefront->out_mindex_num=0;
	int mwavefront->out_iindex_num=0;
	int mwavefront->out_dindex_num=0;
	int fragment_rank;int diagnal_rank;int offset;int offset_position;
	//int next_fragment_rank; int next_diagnoal_rank;
	for(int i = 0; i < mwavefront->in_iindex_num_gap;i++)
	{
		fragment_rank=mwavefront->in_icompute_set_gap[i]->fragment_rank;
		diagnal_rank=mwavefront->in_icompute_set_gap[i]->diagnal_rank;
		offset_position=diagnal_rank+pattern->fragment[fragment_rank]->fragment_length;
		offset=mwavefront->in_iwavefront_gap[fragment_rank]->offset[offset_position];
		if(offset<text_length-1)
		{
			status_set[0][fragment_rank][diagnal_rank+1]=1;
			mwavefront->out_iwavefronts[fragment_rank]->null[offset_position+1]=1;
			mwavefront->out_icompute_diagonal_set[out_mindex_num]={fragment_rank,diagnal_rank+1};
			mwavefront->out_iindex_num++;
			mwavefront->out_iwavefronts[fragment_rank]->offset[offset_position+1]=offset+1;
		}
	}
	for(int i = 0; i < mwavefront->in_iindex_num_ext;i++)
	{
		fragment_rank=mwavefront->in_icompute_set_ext[i]->fragment_rank;
		diagnal_rank=mwavefront->in_icompute_set_ext[i]->diagnal_rank;
		offset_position=diagnal_rank+pattern->fragment[fragment_rank]->fragment_length;
		offset=mwavefront->in_iwavefront_ext[fragment_rank]->offset[offset_position];
		if(offset<text_length-1)
		{
			if(!mwavefront->out_iwavefronts[fragment_rank]->null[offset_position+1])
			{
				status_set[0][fragment_rank][diagnal_rank+1]=1;
				mwavefront->out_iwavefronts[fragment_rank]->null[offset_position+1]=1;
				mwavefront->out_icompute_diagonal_set[mwavefront->out_iindex_num]={fragment_rank,diagnal_rank+1};
				mwavefront->out_iindex_num++;
				mwavefront->out_iwavefronts[fragment_rank]->offset[offset_position+1]=offset+1;
			}
			else
			{
				mwavefront->out_iwavefronts[fragment_rank]->offset[offset_position+1]=MAX(mwavefront->out_iwavefronts[fragment_rank]->offset[offset_position+1],offset+1);
			}
		}
	}
	for(int i = 0; i < mwavefront-> in_dindex_num_gap; i++)
	{
		fragment_rank=mwavefront->in_dcompute_set_gap->fragment_rank;
		diagnal_rank=mwavefront->in_dcompute_set_gap->diagnal_rank;
		offset_position=diagnal_rank+pattern->fragment[fragment_rank]->fragment_length;
		offset=dwavefront->in_dwavefront_gap[fragment_rank]->offset[offset_position];
		if(offset+diagnal_rank>=pattern->fragment[fragment_rank]->fragment_length)
		{
			int next_fragment_rank;int next_diagnol_rank;int next_offset_position;
			for (int j=0;j<pattern->fragment[fragment_rank]->edge_num;j++)
			{
				next_fragment_rank=pattern->fragment[fragment_rank]->next_fragment[j];
				next_diagnol_rank=offset;
				next_offset_position=offset+pattern->fragment[next_fragment_rank]->fragment_length;
				if(!status_set[1][next_fragment_rank][next_diagnal_rank])
				{
					status_set[1][next_fragment_rank][next_diagnal_rank]=1;
					mwavefront->out_dwavefronts[next_fragment_rank]->null[next_offset_position]=offset;
					mwavefront->out_dcompute_diagonal_set[mwavefront->out_dindex_num]={next_fragment_rank,offset};
					mwavefront->out_dindex_num++;
					mwavefront->out_dwavefronts[next_fragment_rank]->offset[next_offset_position]=offset;
				}
			}
		}
		else
		{
			status_set[1][fragment_rank][diagnal_rank-1]=1;
			mwavefront->out_iwavefronts[fragment_rank]->null[offset_position-1]=1;
			mwavefront->out_icompute_diagonal_set[mwavefront->out_iindex_num]={fragment_rank,diagnal_rank-1};
			mwavefront->out_iindex_num++;
			mwavefront->out_iwavefronts[fragment_rank]->offset[offset_position-1]=offset;
		}
	}
	for(int i = 0; i < mwavefront-> in_dindex_num_ext; i++)
	{
		fragment_rank=mwavefront->in_dcompute_set_ext->fragment_rank;
		diagnal_rank=mwavefront->in_dcompute_set_ext->diagnal_rank;
		offset_position=diagnal_rank+pattern->fragment[fragment_rank]->fragment_length;
		offset=dwavefront->in_dwavefront_ext[fragment_rank]->offset[offset_position];
		if(offset+diagnal_rank>=pattern->fragment[fragment_rank]->fragment_length)
		{
			int next_fragment_rank;int next_diagnol_rank;int next_offset_position;
			for (int j=0;j<pattern->fragment[fragment_rank]->edge_num;j++)
			{
				next_fragment_rank=pattern->fragment[fragment_rank]->next_fragment[j];
				next_diagnol_rank=offset;
				next_offset_position=offset+pattern->fragment[next_fragment_rank]->fragment_length;
				if(!status_set[1][next_fragment_rank][next_diagnal_rank])
				{
					status_set[1][next_fragment_rank][next_diagnal_rank]=1;
					mwavefront->out_dwavefronts[next_fragment_rank]->null[next_offset_position]=offset;
					mwavefront->out_dcompute_diagonal_set[mwavefront->out_dindex_num]={next_fragment_rank,offset};
					mwavefront->out_dindex_num++;
					mwavefront->out_dwavefronts[next_fragment_rank]->offset[next_offset_position]=offset;
				}
			}
		}
		else
		{
			if(!mwavefront->out_iwavefronts[fragment_rank]->null[offset_position-1])
			{
				status_set[1][fragment_rank][diagnal_rank-1]=1;
				mwavefront->out_iwavefronts[fragment_rank]->null[offset_position-1]=1;
				mwavefront->out_icompute_diagonal_set[mwavefront->out_iindex_num]={fragment_rank,diagnal_rank-1};
				mwavefront->out_iindex_num++;
				mwavefront->out_iwavefronts[fragment_rank]->offset[offset_position-1]=offset;
			}
			else
			{
				mwavefront->out_iwavefronts[fragment_rank]->offset[offset_position+1]=MAX(mwavefront->out_iwavefronts[fragment_rank]->offset[offset_position+1],offset);
			}
		}
	}
	for(int i = 0; i < mwavefront->in_mindex_num_sub; i++)
	{
		fragment_rank=mwavefront->in_mcompute_set_sub->fragment_rank;
		diagnal_rank=mwavefront->in_mcompute_set_sub->diagnal_rank;
		offset_position=diagnal_rank+pattern->fragment[fragment_rank]->fragment_length;
		offset=dwavefront->in_mwavefront_sub[fragment_rank]->offset[offset_position];
		if(offset+diagnal_rank>=pattern->fragment[fragment_rank]->fragment_length&&offset<text_length-1)
		{
			int next_fragment_rank;int next_diagnol_rank;int next_offset_position;
			for (int j=0;j<pattern->fragment[fragment_rank]->edge_num;j++)
			{
				next_fragment_rank=pattern->fragment[fragment_rank]->next_fragment[j];
				next_diagnol_rank=offset+1;
				next_offset_position=offset+1+pattern->fragment[next_fragment_rank]->fragment_length;
				if(!status_set[2][next_fragment_rank][next_diagnal_rank])
				{
					status_set[1][next_fragment_rank][next_diagnal_rank]=1;
					mwavefront->out_mwavefronts[next_fragment_rank]->null[next_offset_position]=offset;
					mwavefront->out_mcompute_diagonal_set[mwavefront->out_mindex_num]={next_fragment_rank,offset};
					mwavefront->out_mindex_num++;
					mwavefront->out_mwavefronts[next_fragment_rank]->offset[next_offset_position]=offset;
				}
			}
		}
		else if (offset<text_length-1)
		{
			mwavefront->out_mwavefronts[fragment_rank]->null[offset_position]=1;
			mwavefront->out_mcompute_diagonal_set[out_mindex_num]={fragment_rank,diagnal_rank+1};
			mwavefront->out_mindex_num++;
			mwavefront->out_mwavefronts[fragment_rank]->offset[offset_position]=max(max(offset+1,(mwavefront->out_iwavefronts[fragment_rank]->offset[diagnal_rank])*mwavefront->out_iwavefronts[fragment_rank]->bool[diagnal_rank]),(mwavefront->out_dwavefronts[fragment_rank]->offset[diagnal_rank])*mwavefront->out_dwavefronts[fragment_rank]->bool[diagnal_rank]);
		}
	}
}