class GWF_AFFINE_ALIGN {
public:
	Affine_wavefront_set EXTEND(Affine_wavefront_set& original_wavefront_set, int pattern_size, int sequence_size, Affine_status& Status, int* t, Graph& q)
	{
		Index temp_index;
		Affine_wavefront_set final_wavefront_set;
		final_wavefront_set.score=original_wavefront_set.score;
		final_wavefront_set.M_set.index_num=0;
		final_wavefront_set.D_set.index_num=0;
		final_wavefront_set.I_set.index_num=0;
		for (int i=0;i<original_wavefront_set.D_set.index_num;i++)
		{
			temp_index = original_wavefront_set.D_set.index[i];
			if(Status.D[(pattern_size + 1) * temp_index.h + temp_index.u])
				continue;
			INDEX_ADD(final_wavefront_set.D_set,temp_index.h,temp_index.u);
			Status.D[(pattern_size + 1) * temp_index.h + temp_index.u]=1;
			if(!Status.M[(pattern_size + 1) * temp_index.h + temp_index.u])
				INDEX_ADD(original_wavefront_set.M_set,temp_index.h,temp_index.u);
		}
		for (int i=0;i<original_wavefront_set.I_set.index_num;i++)
		{
			temp_index = original_wavefront_set.I_set.index[i];
			if(Status.I[(pattern_size + 1) * temp_index.h + temp_index.u])
				continue;
			INDEX_ADD(final_wavefront_set.I_set,temp_index.h,temp_index.u);
			Status.I[(pattern_size + 1) * temp_index.h + temp_index.u]=1;
			if(!Status.M[(pattern_size + 1) * temp_index.h + temp_index.u])
				INDEX_ADD(original_wavefront_set.M_set,temp_index.h,temp_index.u);
		}
		while (original_wavefront_set.M_set.index_num > 0)
		{
			temp_index = original_wavefront_set.M_set.index[original_wavefront_set.M_set.index_num - 1];
			original_wavefront_set.M_set.index_num--;
			if (Status.M[(pattern_size + 1) * temp_index.h + temp_index.u])
			{
				//printf("%d\t%d\n",(pattern_size + 1) * temp_index.h + temp_index.u,Status.M[(pattern_size + 1) * temp_index.h + temp_index.u]);
				continue;
			}
			Status.M[(pattern_size + 1) * temp_index.h + temp_index.u] = 1;
			int n = 0;
			for (int i = 0; i < q.node[temp_index.u].edgenum; i++)
			{
				int w = q.node[temp_index.u].next[i];
				if (temp_index.h <= sequence_size - 1 && t[temp_index.h] == q.node[w].base)
				{
					INDEX_ADD(original_wavefront_set.M_set,temp_index.h + 1,w);
					n++;
				}
				//printf("%d\t%d\n",t[temp_index.h],q.node[w].base);
			}
			if (q.node[temp_index.u].edgenum == 0 || q.node[temp_index.u].edgenum > n)
			{
				INDEX_ADD(final_wavefront_set.M_set,temp_index.h,temp_index.u);
			}
		}
		return final_wavefront_set;
	}
	void AFFINE_NEXT(Affine_wavefront_set* temp_wavefront_set, int sequence_size, int pattern_size, Affine_status& Status, penalty penalty, int score, int Hash_size, int* t, Graph& q)
	{
		int H_position = score % Hash_size;
		int hash_gap_open = (score + penalty.gap_open + penalty.gap_extention) % Hash_size;
		int hash_gap_extention = (score + penalty.gap_extention) % Hash_size;
		int hash_mismatch = (score + penalty.mismatch) % Hash_size;
		for (int i = 0; i < temp_wavefront_set[H_position].I_set.index_num; i++)
		{
			Index temp_index = temp_wavefront_set[H_position].I_set.index[i];
			if (temp_index.h <= sequence_size - 1 && !Status.I[(temp_index.h + 1) * pattern_size + temp_index.u])
			{
				INDEX_ADD(temp_wavefront_set[hash_gap_extention].I_set,temp_index.h+1,temp_index.u);
			}
		}
		for (int i = 0; i < temp_wavefront_set[H_position].D_set.index_num; i++)
		{
			Index temp_index = temp_wavefront_set[H_position].D_set.index[i];
			for (int j = 0; j < q.node[temp_index.u].edgenum; j++)
			{
				if (Status.D[temp_index.h * (pattern_size + 1) + q.node[temp_index.u].next[j]])
					continue;
				INDEX_ADD(temp_wavefront_set[hash_gap_extention].D_set, temp_index.h, q.node[temp_index.u].next[j]);
			}
		}
		for (int i = 0; i < temp_wavefront_set[H_position].M_set.index_num; i++)
		{
			Index temp_index = temp_wavefront_set[H_position].M_set.index[i];
			if (temp_index.h <= sequence_size - 1 && !Status.M[(temp_index.h + 1) * (pattern_size + 1) + temp_index.u] && !Status.I[(pattern_size + 1) * temp_index.h + temp_index.u])
			{
				INDEX_ADD(temp_wavefront_set[hash_gap_open].I_set,temp_index.h+1,temp_index.u);
			}
			for (int j = 0; j < q.node[temp_index.u].edgenum; j++)
			{
				if (!Status.M[temp_index.h * (pattern_size + 1) + q.node[temp_index.u].next[j]]&& t[temp_index.h]!=q.node[q.node[temp_index.u].next[j]].base && !Status.D[(pattern_size + 1) * temp_index.h + temp_index.u])
					INDEX_ADD(temp_wavefront_set[hash_gap_open].D_set, temp_index.h, q.node[temp_index.u].next[j]);
				if (temp_index.h <= sequence_size - 1 && !Status.M[(temp_index.h + 1) * (pattern_size + 1) + q.node[temp_index.u].next[j]])
				{
					INDEX_ADD(temp_wavefront_set[hash_mismatch].M_set, temp_index.h + 1, q.node[temp_index.u].next[j]);
				}
			}
		}
		temp_wavefront_set[hash_gap_open].score = score + penalty.gap_open + penalty.gap_extention;
		temp_wavefront_set[hash_gap_extention].score =  score + penalty.gap_extention;
		temp_wavefront_set[hash_mismatch].score = score + penalty.mismatch;
	}
	int AFFINE_ALIGN(int* t, Graph& q, penalty p, int sequence_size, int pattern_size,int final_node, Affine_status& Status, Affine_wavefront_set*& W, int hash_size)
	{
		/* Initialize Wavefront */
		static const int Hash_size = prime(MAX(p.gap_open+p.gap_extention, p.mismatch));
		for (int i = 0; i < Hash_size; i++)
		{
			W[i].M_set.index_num = 0;
		        W[i].D_set.index_num = 0;
			W[i].I_set.index_num = 0;
			W[i].score=-1;
		}

		int score = 0;
		for (int i = 0; i < (pattern_size + 1) * (sequence_size + 1); i++)
		{
			Status.M[i] = 0;
			Status.D[i] = 0;
			Status.I[i] = 0;
		}

		/* Add the first Wavefront Index*/
		INDEX_ADD(W[0].M_set,0,0);
		W[0].score = 0;

		while (true)
		{
			int s_position = score % Hash_size;

			/* Extend */
			W[s_position] = EXTEND(W[s_position], pattern_size, sequence_size, Status, t, q);

			/* Output */
			
		//	for (int i = 0; i < W[s_position].M_set.index_num; i++)
		//		printf("%d\t%d\t%d\n", W[s_position].score, W[s_position].M_set.index[i].h, W[s_position].M_set.index[i].u);
			
			/* Termination */
			if (Status.M[(pattern_size + 1) * sequence_size + final_node] == 1)
				break;

			/* Next */
			AFFINE_NEXT(W, sequence_size, pattern_size, Status, p, score, Hash_size, t, q);

			/* Find the next score*/
			W[s_position].M_set.index_num = 0;
			W[s_position].D_set.index_num = 0; 
			W[s_position].I_set.index_num = 0; 
			W[s_position].score=-1;
			int next_score = INT_MAX;
			
			for (int i = 0; i < Hash_size; i++)
			{
				//printf("%d\t%d\n", H[i].index_num, H[i].score);
				if (W[i].M_set.index_num > 0||W[i].D_set.index_num > 0||W[i].I_set.index_num > 0)
					next_score = MIN(next_score,W[i].score);
			}
			score = next_score;
		}
		return score;
	}
};

GWF_AFFINE_ALIGN Wavefront;

#define GWF_AFFINE(t, q, p, sequence_size, pattern_size,final_node, Status, W, hash_size) Wavefront.AFFINE_ALIGN(t, *q, p, sequence_size, pattern_size,final_node, Status,W, hash_size)
