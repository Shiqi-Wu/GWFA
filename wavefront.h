#include <iostream>
#include <string.h>


using namespace std;

int tran(char a) {
	int c;
	if (a == 'A') {
		c = 0;
	}
	if (a == 'G') {
		c = 1;
	}
	if (a == 'C') {
		c = 2;
	}
	if (a == 'T')
	{
		c = 3;
	}
	return c;
}

typedef struct
{
	int x;
	int o;
	int e;
}penalty;

typedef struct Node
{
	int base;
	Node* next;
}Node;

typedef struct
{
	int num;
	Node node[n+1];
}Graph;

using namespace std;

typedef struct Wavefront
{
	int h;		//INDEX on sequence
	int u;		//INDEX on graph
	Wavefront* next;		//next INDEX
	Wavefront* back;		//backtrace
}Wavefront;

typedef struct
{
	int score;
	Wavefront* next;
}WaveStruct;

Graph GraphInit(bool V[n][n], string& s)
{
	Graph q;
	q.num = n;
	q.node[0].base = -1;
	for (int i = 1; i <= n; i++)
	{
		q.node[i].base = tran(s[i - 1]);
	}
	Node* p = new Node;
	p->base = 1;
	p->next = NULL;
	q.node[0].next = p;


	for (int i = 1; i <= n; i++)
	{
		q.node[i].next = NULL;
		for (int j = 1; j <= n; j++)
		{
			if (V[i - 1][j - 1] == 1)
			{
				p = new Node;
				p->base = j;
				p->next = q.node[i].next;
				q.node[i].next = p;
			}
		}
	}
	return q;
}

void InitWavefront(Wavefront& w);
void AddWavefront(Wavefront& w, WaveStruct& s, bool M_in[m + 1][n + 1]);
void WavefrontExtend(WaveStruct& M_s, Graph q, int tran_string[], bool M_in[m + 1][n + 1]);
void GraphExtend(int h, int u, Wavefront& S, Graph& q, int tran_string[]);
void WavefrontNext(WaveStruct M[], WaveStruct I[], WaveStruct D[], Graph& q, int tran_string[], int s, penalty p, bool M_in[m + 1][n + 1], bool I_in[m + 1][n + 1], bool D_in[m + 1][n + 1]);
void WFGraphAlign(Graph& q, int tran_string[], penalty p);


void InitWavefront(Wavefront& w)
{
	w.h = -1;
	w.u = -1;
	w.next = NULL;
	w.back = NULL;
}

void AddWavefront(Wavefront& w, WaveStruct& s, bool M_in[m + 1][n + 1])	//add the wavefront Index w to s
{
	Wavefront* q;
	q = s.next;
	if (!M_in[w.h][w.u])
	{
		M_in[w.h][w.u] = 1;
		w.next = q;
		s.next = &w;
		new_Matrix[w.h - 1][w.u - 1] = s.score> new_Matrix[w.h - 1][w.u - 1]? new_Matrix[w.h - 1][w.u - 1]:s.score;
	}
}

void WavefrontExtend(WaveStruct& M_s, Graph q, int tran_string[], bool M_in[m + 1][n + 1])
{
	Wavefront S;
	InitWavefront(S);
	Wavefront* p;
	p = M_s.next;
	while (p)
	{
		GraphExtend(p->h, p->u, S, q, tran_string);
		//DelWavefront(*p, M_s);
		p = p->next;
	}

	Wavefront* pS; Wavefront* d;
	pS = &S; d = pS->next;
	while (pS->next)
	{
		pS = d; d = d->next;
		//cout << pS->h << pS->u << endl;
		AddWavefront(*pS, M_s, M_in);
	}
	pS = &S;
}

void GraphExtend(int h, int u, Wavefront& S, Graph& q, int tran_string[])
{

	if (q.node[u].next)
	{
		Node* k;
		k = q.node[u].next;
		Wavefront* p;
		p = &S;

		Wavefront* pp = new Wavefront;
		pp->h = h;
		pp->u = u;
		pp->next = p->next;
		p->next = pp;
		while (k && h <= m - 1)
		{
			if (q.node[k->base].base == tran_string[h])
				GraphExtend(h + 1, k->base, S, q, tran_string);
			k = k->next;
		}
	}
}

void WavefrontNext(WaveStruct M[], WaveStruct I[], WaveStruct D[], Graph& q, int tran_string[], int s, penalty p, bool M_in[m + 1][n + 1], bool I_in[m + 1][n + 1], bool D_in[m + 1][n + 1])
{
	Wavefront* w;
	Wavefront* new_w;
	if (s - p.o - p.e >= 0)
	{
		w = M[s - p.o - p.e].next;
		while (w)
		{
			new_w = new Wavefront;
			new_w->h = w->h + 1;
			new_w->u = w->u;
			new_w->next = NULL;
			new_w->back = w;
			AddWavefront(*new_w, I[s], I_in);
			AddWavefront(*new_w, M[s], M_in);
			Node* next_node;
			next_node = q.node[w->u].next;
			while (next_node)
			{
				new_w = new Wavefront;
				new_w->h = w->h;
				new_w->u = next_node->base;
				new_w->next = NULL;
				new_w->back = w;
				AddWavefront(*new_w, D[s], D_in);
				AddWavefront(*new_w, M[s], M_in);
				next_node = next_node->next;
			}
			w = w->next;
		}
	}
	if (s - p.e >= 0)
	{
		w = I[s - p.e].next;

		while (w)
		{
			new_w = new Wavefront;
			new_w->h = w->h + 1;
			new_w->u = w->u;
			new_w->next = NULL;
			new_w->back = w;
			AddWavefront(*new_w, I[s], I_in);
			AddWavefront(*new_w, M[s], M_in);
			w = w->next;
		}
		w = D[s - p.e].next;
		while (w)
		{
			Node* next_node;
			next_node = q.node[w->u].next;
			while (next_node)
			{
				new_w = new Wavefront;
				new_w->h = w->h;
				new_w->u = next_node->base;
				new_w->next = NULL;
				new_w->back = w;
				AddWavefront(*new_w, D[s], D_in);
				AddWavefront(*new_w, M[s], M_in);
				next_node = next_node->next;
			}
			w = w->next;
		}
	}
	if (s - p.x >= 0)
	{
		w = M[s - p.x].next;
		while (w)
		{
			Node* next_node = q.node[w->u].next;
			while (next_node)
			{
				new_w = new Wavefront;
				new_w->h = w->h + 1;
				new_w->u = next_node->base;
				new_w->next = NULL;
				new_w->back = w;
				AddWavefront(*new_w, M[s], M_in);
				next_node = next_node->next;
			}
			w = w->next;
		}
	}
}

void WFGraphAlign(Graph& q, int tran_string[], penalty p)
{
	//int m; int n; 
	int s;
	//m = size(t)+1;
	//n = q.num + 1;
	s = 0;
	WaveStruct M[MAX_SCORE + 1];
	WaveStruct I[MAX_SCORE + 1];
	WaveStruct D[MAX_SCORE + 1];
	/*bool M_in[m + 1][n + 1];
	bool I_in[m + 1][n + 1];
	bool D_in[m + 1][n + 1];*/
	for (int i = 0; i <= m; i++)
	{
		for (int j = 0; j <= n; j++)
		{
			M_in[i][j] = 0;
			I_in[i][j] = 0;
			D_in[i][j] = 0;
		}
	}
	for (int k = 0; k <= MAX_SCORE; k++)
	{
		M[k].score = k; M[k].next = NULL;
		I[k].score = k; I[k].next = NULL;
		D[k].score = k; D[k].next = NULL;
	}
	Wavefront* w; Wavefront* temp = new Wavefront;
	for (int k = 0; k <= n; k++)
	{
		w = new Wavefront;
		w->h = 0; w->u = k; w->next = NULL;
		AddWavefront(*w, M[0], M_in);
	}
	while (1)
	{
		WavefrontExtend(M[s], q, tran_string, M_in);
		w = new Wavefront;
		w->h = m;
		w->u = n;
		w->next = NULL;
		w->back = NULL;
		temp = M[s].next;
		while (temp)
		{
			cout << s << '\t' << temp->h << '\t' << temp->u << endl;
			temp = temp->next;
		}
		int k = 0;
		for (; k < n; k++)
			if (!M_in[m][k])
				break;
		if (k == n)
			break;
		/*
		if (M_in[m][n])
			break;
			*/
		s++;
		WavefrontNext(M, I, D, q, tran_string, s, p, M_in, I_in, D_in);
	}
	/*while (w)		//Backtrace
	{
		cout << w->h << "\t" << w->u << endl;
		w = w->back;
	}*/
}
