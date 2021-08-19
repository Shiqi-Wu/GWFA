void AddIndex(Wavefront& M, int h, int u, bool in[m + 1][n + 1]);
Wavefront WF_Extend(Wavefront& M, Graph& q, string& t);
Wavefront Extend(int h, int u, Wavefront& M, Graph& q, string& t);
void InitWavefront(Wavefront& M);

/* Match case */
Wavefront WF_Extend(Wavefront& M, Graph& q, string& t)
{
	int k = M.index_num;
	for (int i = 0; i < k; i++)
	{
		int u = M.index[i].u;
		int h = M.index[i].h;
		Extend(h, u, M, q, t);
	}
	return M;
}

Wavefront Extend(int h, int u, Wavefront& M, Graph& q, string& t)
{
	for (int j = 0; j < q.node[u].edgenum; j++)
	{
		int v = q.node[u].next[j];
		if (q.node[v].base == tran(t[h]))
		{
			AddIndex(M, h + 1, v, M_in);
			Extend(h + 1, v, M, q, t);
		}
	}
	return M;
}

/* Add Index to wavefront_index */
void AddIndex(Wavefront& M, int h, int u, bool in[m + 1][n + 1])
{
	if (!in[h][u])
	{
		in[h][u] = 1;
		M.index_num++;
		M.index[M.index_num - 1].h = h;
		M.index[M.index_num - 1].u = u;
	}
}

void InitWavefront(Wavefront& M)
{
	M.score = -1;
	M.index_num = 0;
}


