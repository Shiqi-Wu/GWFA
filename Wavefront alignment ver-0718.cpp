#include <iostream>
#include <string.h>
#define m 5
#define n 5

#define MAX_VERTEX_NUM 10

#define MAX_SCORE 20


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

typedef struct NextNode
{
	int Index;
	NextNode* next;
}NextNode;

typedef struct
{
	char base[2];
	NextNode* next;
}Node;

typedef struct
{
	int num;
	Node node[MAX_VERTEX_NUM];
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

void InitWavefront(Wavefront& w);
void AddWavefront(Wavefront& w, WaveStruct& s, bool M_in[m + 1][n + 1]);
void WavefrontExtend(WaveStruct& M_s, Graph q, string& t, bool M_in[m + 1][n + 1]);
void GraphExtend(int h, int u, Wavefront& S, Graph& q, string& t);
void WavefrontNext(WaveStruct M[], WaveStruct I[], WaveStruct D[], Graph& q, string& t, int s, penalty p, bool M_in[m + 1][n + 1], bool I_in[m + 1][n + 1], bool D_in[m + 1][n + 1]);
void WFGraphAlign(Graph& q, string& t, penalty p);

int main()
{
	Graph q;
	q.num = 5;
	strcpy_s(q.node[1].base, "A");
	strcpy_s(q.node[2].base, "C");
	strcpy_s(q.node[3].base, "G");
	strcpy_s(q.node[4].base, "T");
	strcpy_s(q.node[5].base, "C");
	NextNode* next_node; NextNode* x;
	next_node = new NextNode;
	next_node->Index = 1; next_node->next = NULL; q.node[0].next = next_node;
	next_node = new NextNode;
	next_node->Index = 2; next_node->next = NULL; q.node[1].next = next_node;
	x = next_node;
	next_node = new NextNode;
	next_node->Index = 3; next_node->next = NULL; x->next = next_node;
	next_node = new NextNode;
	next_node->Index = 3; next_node->next = NULL; q.node[2].next = next_node;
	next_node = new NextNode;
	next_node->Index = 4; next_node->next = NULL; q.node[3].next = next_node;
	next_node = new NextNode;
	next_node->Index = 5; next_node->next = NULL; q.node[4].next = next_node;
	next_node = new NextNode;
	next_node->Index = 2; next_node->next = NULL; q.node[5].next = next_node;

	string t = "ATCCA";

	penalty p = { 4,6,2 };

	WFGraphAlign(q, t, p);

	return 0;
}

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
	}
}

void WavefrontExtend(WaveStruct& M_s, Graph q, string& t, bool M_in[m + 1][n + 1])
{
	Wavefront S;
	InitWavefront(S);
	Wavefront* p;
	p = M_s.next;
	while (p)
	{
		GraphExtend(p->h, p->u, S, q, t);
		//DelWavefront(*p, M_s);
		p = p->next;
	}

	Wavefront* pS;
	pS = &S;
	while (pS->next)
	{
		pS = pS->next;
		pS->back = p;
		AddWavefront(*pS, M_s, M_in);
	}
}

void GraphExtend(int h, int u, Wavefront& S, Graph& q, string& t)
{
	if (q.node[u].next)
	{
		NextNode* k;
		k = q.node[u].next;
		Wavefront* p;
		p = &S;
		while (p->next)
			p = p->next;
		//int m = size(t);
		if (!(k && h < m - 1))
		{
			Wavefront* pp = new Wavefront;
			pp->h = h;
			pp->u = u;
			pp->next = NULL;
			p->next = pp;
			p = p->next;
		}
		while (k && h < m - 1)
		{
			if (tran(q.node[k->Index].base[0]) != tran(t[h]))
			{
				Wavefront* pp = new Wavefront;
				pp->h = h;
				pp->u = u;
				pp->next = NULL;
				p->next = pp;
				p = p->next;
			}
			else
			{
				GraphExtend(h + 1, k->Index, S, q, t);
			}
			k = k->next;
		}
	}
}

void WavefrontNext(WaveStruct M[], WaveStruct I[], WaveStruct D[], Graph& q, string& t, int s, penalty p, bool M_in[m+1][n+1], bool I_in[m+1][n+1], bool D_in[m+1][n+1])
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
			NextNode* next_node;
			next_node = q.node[w->u].next;
			while (next_node)
			{
				new_w = new Wavefront;
				new_w->h = w->h;
				new_w->u = next_node->Index;
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
			NextNode* next_node;
			next_node = q.node[w->u].next;
			while (next_node)
			{
				new_w = new Wavefront;
				new_w->h = w->h;
				new_w->u = next_node->Index;
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
			NextNode* next_node = q.node[w->u].next;
			while (next_node)
			{
				new_w = new Wavefront;
				new_w->h = w->h + 1;
				new_w->u = next_node->Index;
				new_w->next = NULL;
				new_w->back = w;
				AddWavefront(*new_w, M[s], M_in);
				next_node = next_node->next;
			}
			w = w->next;
		}
	}
}

void WFGraphAlign(Graph& q, string& t, penalty p)
{
	//int m; int n; 
	int s;
	//m = size(t)+1;
	//n = q.num + 1;
	s = 0;
	WaveStruct M[MAX_SCORE + 1];
	WaveStruct I[MAX_SCORE + 1];
	WaveStruct D[MAX_SCORE + 1];
	bool M_in[m + 1][n + 1];
	bool I_in[m + 1][n + 1];
	bool D_in[m + 1][n + 1];
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
	w = new Wavefront;
	w->h = 0; w->u = 0; w->next = NULL;
	AddWavefront(*w, M[0], M_in);
	while (1)
	{
		WavefrontExtend(M[s], q, t, M_in);
		w = new Wavefront;
		w->h = m;
		w->u = n;
		w->next = NULL;
		w->back = NULL;
		if (M_in[m][n])
			break;
		temp = M[s].next;
		while (temp)
		{
			cout << s << '\t' << temp->h << '\t' << temp->u << endl;
			temp = temp->next;
		}
		s++;
		WavefrontNext(M, I, D, q, t, s, p, M_in, I_in, D_in);
	}
	while (w)		//Backtrace
	{
		cout << w->h << "\t" << w->u << endl;
		w = w->back;
	}
}