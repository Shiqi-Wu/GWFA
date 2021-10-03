void WF_NEXT(Wavefront M[], Wavefront D[], Wavefront I[], int s, Graph q, string t, penalty p);
void WF_Graph_Align(Graph& q, string& t, penalty p);

void WF_NEXT(Wavefront M[], Wavefront D[], Wavefront I[], int s, Graph q, string t, penalty p)
{
	int o_hash = (s - p.o - p.e + HASH_SIZE) % HASH_SIZE;
	int e_hash = (s - p.e + HASH_SIZE) % HASH_SIZE;
	int x_hash = (s - p.x + HASH_SIZE) % HASH_SIZE;
	int s_hash = s % HASH_SIZE;
	for (int i = 0; i < M[o_hash].index_num; i++)
	{
		int h = M[o_hash].index[i].h;
		int u = M[o_hash].index[i].u;
		AddIndex(I[s_hash], h + 1, u, I_in);
		AddIndex(M[s_hash], h + 1, u, M_in);
		for (int j = 0; j < q.node[u].edgenum; j++)
		{
			AddIndex(D[s_hash], h, q.node[u].next[j], D_in);
			AddIndex(M[s_hash], h, q.node[u].next[j], M_in);
		}
	}
	for (int i = 0; i < I[e_hash].index_num; i++)
	{
		int h = I[e_hash].index[i].h;
		int u = I[e_hash].index[i].u;
		AddIndex(I[s_hash], h + 1, u, I_in);
		AddIndex(M[s_hash], h + 1, u, M_in);
	}
	for (int i = 0; i < D[e_hash].index_num; i++)
	{
		int h = D[e_hash].index[i].h;
		int u = D[e_hash].index[i].u;
		for (int j = 0; j < q.node[u].edgenum; j++)
		{
			AddIndex(D[s_hash], h, q.node[u].next[j], D_in);
			AddIndex(M[s_hash], h, q.node[u].next[j], M_in);
		}
	}
	for (int i = 0; i < M[x_hash].index_num; i++)
	{
		int h = M[x_hash].index[i].h;
		int u = M[x_hash].index[i].u;
		for (int j = 0; j < q.node[u].edgenum; j++)
		{
			AddIndex(M[s_hash], h + 1, q.node[u].next[j], M_in);
		}
	}
}

void WF_Graph_Align(Graph& q, string& t, penalty p)
{
	Wavefront M[HASH_SIZE], D[HASH_SIZE], I[HASH_SIZE];

	/* Initial setting */
	for (int i = 0; i <= m; i++)
	{
		for (int j = 0; j <= n; j++)
		{
			M_in[i][j] = 0;
			D_in[i][j] = 0;
			I_in[i][j] = 0;
		}
	}
	for (int i = 0; i < HASH_SIZE; i++)
	{
		InitWavefront(M[i]);
		InitWavefront(D[i]);
		InitWavefront(I[i]);
	}
	AddIndex(M[0], 0, 0, M_in);
	int s = 0;
	while (1)
	{
		int s_hash = s % HASH_SIZE;
		M[s_hash] = WF_Extend(M[s_hash], q, t);
		if (M_in[m][n])
			break;
		for (int i = 0; i < M[s_hash].index_num; i++)
		{
			cout << s << '\t' << M[s_hash].index[i].h << '\t' << M[s_hash].index[i].u << endl;
		}
		s = s + 1;
		s_hash = s % HASH_SIZE;
		InitWavefront(M[s_hash]);
		InitWavefront(D[s_hash]);
		InitWavefront(I[s_hash]);
		WF_NEXT(M, D, I, s, q, t, p);
	}
	cout << s << endl;
}