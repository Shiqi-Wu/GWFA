class GWF {
public:
	Wavefront_set EXTEND(Wavefront_set original_wavefront_set, bool D[], int pattern_size, int sequence_size, char* t, Graph q)
	{
		Index temp_index;
		Wavefront_set final_wavefront_set;
		final_wavefront_set.index_num = 0;
		final_wavefront_set.score = original_wavefront_set.score;
		while (original_wavefront_set.index_num > 0)
		{
			temp_index = original_wavefront_set.index[original_wavefront_set.index_num - 1];
			original_wavefront_set.index_num--;
			if (D[(pattern_size + 1) * temp_index.h + temp_index.u])
				continue;
			D[(pattern_size + 1) * temp_index.h + temp_index.u] = 1;
			int n = 0;
			for (int i = 0; i < q.node[temp_index.u].edgenum; i++)
			{
				int w = q.node[temp_index.u].next[i];
				if (temp_index.h <= sequence_size - 1 && tran(t[temp_index.h]) == q.node[w].base)
				{
					original_wavefront_set.index[original_wavefront_set.index_num].h = temp_index.h + 1;
					original_wavefront_set.index[original_wavefront_set.index_num].u = w;
					original_wavefront_set.index_num++;
					n++;
				}
			}
			if (q.node[temp_index.u].edgenum == 0 || q.node[temp_index.u].edgenum > n)
			{
				final_wavefront_set.index[final_wavefront_set.index_num] = temp_index;
				final_wavefront_set.index_num++;
			}
		}
		return final_wavefront_set;
	}
	void LINEAR_NEXT(Wavefront_set temp_wavefront_set[], bool D[], char* t, int sequence_size, int pattern_size, penalty penalty, int score, int Hash_size, Graph q)
	{
		int H_position = score % Hash_size;
		int hash_gap = (score + penalty.gap) % Hash_size;
		int hash_mismatch = (score + penalty.mismatch) % Hash_size;
		for (int i = 0; i < temp_wavefront_set[H_position].index_num; i++)
		{
			Index temp_index = temp_wavefront_set[H_position].index[i];
			if (temp_index.h <= sequence_size - 1 && !D[(temp_index.h + 1) * pattern_size + temp_index.u])
			{
				temp_wavefront_set[hash_gap].index[temp_wavefront_set[hash_gap].index_num] = { temp_index.h + 1,temp_index.u };
				temp_wavefront_set[hash_gap].index_num++;
			}
			for (int j = 0; j < q.node[temp_index.u].edgenum; j++)
			{
				if (!D[temp_index.h * pattern_size + q.node[temp_index.u].next[j]])
				{
					temp_wavefront_set[hash_gap].index[temp_wavefront_set[hash_gap].index_num] = { temp_index.h,q.node[temp_index.u].next[j] };
					temp_wavefront_set[hash_gap].index_num++;
				}
				if (temp_index.h <= sequence_size - 1 && !D[(temp_index.h + 1) * pattern_size + q.node[temp_index.u].next[j]])
				{
					temp_wavefront_set[hash_mismatch].index[temp_wavefront_set[hash_mismatch].index_num] = { temp_index.h + 1,q.node[temp_index.u].next[j] };
					temp_wavefront_set[hash_mismatch].index_num++;
				}
			}
		}
		temp_wavefront_set[hash_gap].score = score + penalty.gap;
		temp_wavefront_set[hash_mismatch].score = score + penalty.mismatch;
	}
	int LINEAR_ALIGN(char* t, Graph& q, penalty p, int sequence_size, int pattern_size, bool* D, Wavefront_set* H, int hash_size)
	{
		/* Initialize Wavefront */
		static const int Hash_size = prime(MAX(p.gap, p.mismatch));
		for (int i = 0; i < Hash_size; i++)
		{
			H[i].index_num = 0;
			H[i].score = -1;
		}

		int score = 0;
		for (int i = 0; i < (pattern_size + 1) * (sequence_size + 1); i++)
			D[i] = 0;

		/* Add the first Wavefront Index*/
		H[0].index[0] = { 0,0 };
		H[0].index_num++;
		H[0].score = 0;

		while (true)
		{
			int s_position = score % Hash_size;

			/* Extend */
			H[s_position] = EXTEND(H[s_position], D, pattern_size, sequence_size, t, q);

			/* Output */
			for (int i = 0; i < H[s_position].index_num; i++)
				printf("%d\t%d\t%d\n", H[s_position].score, H[s_position].index[i].h, H[s_position].index[i].u);

			/* Termination */
			if (D[(pattern_size + 1) * (sequence_size + 1) - 1] == 1)
				break;

			/* Next */
			LINEAR_NEXT(H, D, t, sequence_size, pattern_size, p, score, Hash_size, q);

			/* Find the next score*/
			H[s_position].index_num = 0;
			H[s_position].score = -1;
			int next_score = INT_MAX;
			for (int i = 0; i < Hash_size; i++)
			{
				printf("%d\t%d\n", H[i].index_num, H[i].score);
				if (H[i].index_num > 0)
					next_score = MIN(next_score, H[i].score);
			}
			score = next_score;
		}
		return score;
	}
};

GWF Wavefront;

#define GWF_EXTEND(t, q, p, pattern_size, sequence_size, D, H, hash_size) Wavefront.LINEAR_ALIGN(t, *q, p, pattern_size, sequence_size, D, H, hash_size)