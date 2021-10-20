class GWF_AFFINE_ALIGN {
public:
	Wavefront_set EXTEND(Wavefront_set original_wavefront_set_M, Wavefront_set original_wavefront_set_D, Wavefront_set original_wavefront_set_I, int pattern_size, int sequence_size, bool* M, bool* D, bool* I, char* t, Graph q)
	{
		Index temp_index;
		Wavefront_set final_wavefront_set[3];
		for (int i=0;i<3;i++)
		{
			final_wavefront_set[i].index_num = 0;
			final_wavefront_set[i].score = original_wavefront_set_M.score;
		}
		for (int i=0;i<original_wavefront_set_D.index_num;i++)
		{
			temp_index = original_wavefront_set_D.index[i];
			if(D[(pattern_size + 1) * temp_index.h + temp_index.u])
				continue;
			INDEX_ADD(final_wavefront_set[1],temp_index.h,temp_index.u);
			D[(pattern_size + 1) * temp_index.h + temp_index.u]=1;
			if(!M[(pattern_size + 1) * temp_index.h + temp_index.u])
				INDEX_ADD(original_wavefront_set_M,temp_index.h,temp_index.u);
		}
		for (int i=0;i<original_wavefront_set_I.index_num;i++)
		{
			temp_index = original_wavefront_set_I.index[i];
			if(I[(pattern_size + 1) * temp_index.h + temp_index.u])
				continue;
			INDEX_ADD(final_wavefront_set[2],temp_index.h,temp_index.u);
			I[(pattern_size + 1) * temp_index.h + temp_index.u]=1;
			if(!M[(pattern_size + 1) * temp_index.h + temp_index.u])
				INDEX_ADD(orginal_wavefront_set_M,temp_index.h,temp_index.u);
		}
		original_wavefront_set_D = final_wavefront_set[1];
		original_wavefront_set_I = final_wavefront_set[2];
		while (original_wavefront_set_M.index_num > 0)
		{
			temp_index = original_wavefront_set.index_M[original_wavefront_set_M.index_num - 1];
			original_wavefront_set_M.index_num--;
			if (M[(pattern_size + 1) * temp_index.h + temp_index.u])
				continue;
			M[(pattern_size + 1) * temp_index.h + temp_index.u] = 1;
			int n = 0;
			for (int i = 0; i < q.node[temp_index.u].edgenum; i++)
			{
				int w = q.node[temp_index.u].next[i];
				if (temp_index.h <= sequence_size - 1 && tran(t[temp_index.h]) == q.node[w].base)
				{
					INDEX_ADD(original_wavefront_set_M,temp_index.h + 1,w);
					n++;
				}
			}
			if (q.node[temp_index.u].edgenum == 0 || q.node[temp_index.u].edgenum > n)
			{
				INDEX_ADD(final_wavefront_set[0],temp_index.h,temp_index.u);
			}
		}
		return final_wavefront_set[0];
	}
	void AFFINE_NEXT(Wavefront_set temp_wavefront_set_M[], Wavefront_set temp_wavefront_set D[], Wavefront_set temp_wavefront_set I[], int sequence_size, int pattern_size, bool* M, bool* D, bool* I, penalty penalty, int score, int Hash_size, char* t, Graph q)
	{
		int H_position = score % Hash_size;
		int hash_gap_open = (score + penalty.gap_open + penalty.gap_extention) % Hash_size;
		int hash_gap_extention = (score + penalty.gap_extention) % Hash_size;
		for (int i = 0; i < temp_wavefront_set_I[H_position].index_num; i++)
		{
			Index temp_index = temp_wavefront_set_I[H_position].index[i];
			if (temp_index.h <= sequence_size - 1 && !I[(temp_index.h + 1) * pattern_size + temp_index.u])
			{
				INDEX_ADD(I[hash_gap_extention],temp_index.h+1,temp_index.u);
			}
		}
		for (int i = 0; i < temp_wavefront_set_D[H_position].index_num; i++)
		{
			Index temp_index = temp_wavefront_set_D[H_position].index[i];
			for (int j = 0; j < q.node[temp_index.u].edgenum; j++)
			{
				if (D[temp_index.h * (pattern_size + 1) + q.node[temp_index.u].next[j]])
					continue;
				INDEX_ADD(temp_wavefront_set_D[hash_gap_extention], temp_index.h, q.node[temp_index.u].next[j]);
			}
		}
		for (int i = 0; i < temp_wavefront_set_M[H_position].index_num; i++)
		{
			Index temp_index = temp_wavefront_set_M[H_position].index[i];
			if (temp_index.h <= sequence_size - 1 && !M[(temp_index.h + 1) * (pattern_size + 1) + temp_index.u] && !I[(pattern_size + 1) * temp_index.h + temp_index.u])
			{
				INDEX_ADD(I[hash_gap_open],temp_index.h+1,temp_index.u);
			}
			for (int j = 0; j < q.node[temp_index.u].edgenum; j++)
			{
				if (!M[temp_index.h * (pattern_size + 1) + q.node[temp_index.u].next[j]]&& tran(t[temp_index.h])!=q.node[q.node[temp_index.u].next[j]].base && !D[(pattern_size + 1) * temp_index.h + temp_index.u])
					INDEX_ADD(D[hash_gap_open], temp_index.h, q.node[temp_index.u].next[j]);
				if (temp_index.h <= sequence_size - 1 && !M[(temp_index.h + 1) * (pattern_size + 1) + q.node[temp_index.u].next[j]])
				{
					INDEX_ADD(M[hash_mismatch], temp_index.h + 1, q.node[temp_index.u].next[j]);
				}
			}
		}
		temp_wavefront_set[hash_gap_open].score = score + penalty.gap_open + penalty.gap_extention;
		temp_wavefront_set[hash_gap_extention].score =  score + penalty.gap_extention;
		temp_wavefront_set[hash_mismatch].score = score + penalty.mismatch;
	}
	int AFFINE_ALIGN(char* t, Graph& q, penalty p, int sequence_size, int pattern_size,int final_node, bool* M, bool* D, bool* I, Wavefront_set* Indexset_M, Wavefront_set* Indexset_D, Wavefront_set* Indexset_I, int hash_size)
	{
		/* Initialize Wavefront */
		static const int Hash_size = prime(MAX(p.gap_open+p.gap_extention, p.mismatch));
		for (int i = 0; i < Hash_size; i++)
		{
			Indexset_M[i].index_num = 0; H[i].score = -1;
			Indexset_D[i].index_num = 0; H[i].score = -1;
			Indexset_I[i].index_num = 0; H[i].score = -1;
		}

		int score = 0;
		for (int i = 0; i < (pattern_size + 1) * (sequence_size + 1); i++)
		{
			M[i] = 0;
			D[i] = 0;
			I[i] = 0;
		}

		/* Add the first Wavefront Index*/
		INDEX_ADD(Indexset_M[0],0,0);
		Indexset_M[0].score = 0;

		while (true)
		{
			int s_position = score % Hash_size;

			/* Extend */
			Indexset_M[s_position] = EXTEND(Indexset_M[s_position], Indexset_D[s_position], Indexset_I[s_position], pattern_size, sequence_size, M, t, q);

			/* Output */
			
			/*for (int i = 0; i < H[s_position].index_num; i++)
				printf("%d\t%d\t%d\n", H[s_position].score, H[s_position].index[i].h, H[s_position].index[i].u);*/
			
			/* Termination */
			if (M[(pattern_size + 1) * sequence_size + final_node] == 1)
				break;

			/* Next */
			LINEAR_NEXT(Indexset_M, Indexset_D, Indexset_I, sequence_size, pattern_size, M, D, I, p, score, Hash_size, t, q);

			/* Find the next score*/
			Indexset_M[s_position].index_num = 0; Indexset_M[s_position].score = -1;
			Indexset_D[s_position].index_num = 0; Indexset_D[s_position].score = -1;
			Indexset_I[s_position].index_num = 0; Indexset_I[s_position].score = -1;
			int next_score = INT_MAX;
			
			for (int i = 0; i < Hash_size; i++)
			{
				//printf("%d\t%d\n", H[i].index_num, H[i].score);
				if (Indexset_M[i].index_num > 0)
					next_score = MIN(next_score, Indexset_M[i].score);
			}
			score = next_score;
		}
		return score;
	}
};

GWF_AFFINE_ALIGN Wavefront;

#define GWF_AFFINE(t, q, p, sequence_size, pattern_size,final_node, M, D, I, Indexset_M, Indexset_D, Indexset_I, hash_size) Wavefront.LINEAR_ALIGN(t, *q, p, sequence_size, pattern_size,final_node, M, D, I, Indexset_M, Indexset_D, Indexset_I, hash_size)