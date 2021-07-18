#include <iostream>
#include <string.h>

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

typedef struct
{
	bool in;
	Wavefront* position;
}Inposition;

void InitWavefront(Wavefront& w);
Inposition in(Wavefront& w, WaveStruct& s);
void AddWavefront(Wavefront& w, WaveStruct& s);
void DelWavefront(Wavefront& w, WaveStruct& s);
void WavefrontExtend(WaveStruct& M_s, Graph q, string& t);
void GraphExtend(int h, int u, Wavefront& S, Graph& q, string& t);
void WavefrontNext(WaveStruct M[], WaveStruct I[], WaveStruct D[], Graph& q, string& t, int s, penalty p);
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
}

void InitWavefront(Wavefront& w)
{
	w.h = -1;
	w.u = -1;
	w.next = NULL;
	w.back = NULL;
}

Inposition in(Wavefront& w, WaveStruct& s)		//return whether w is in s and its position or if we need add it to s, where it should be. Some exception have to be dealt with.
{
	Wavefront* q;
	Inposition x;
	Wavefront* p = new Wavefront;
	q = s.next;
	if (!q)
	{
		x.in = 0; x.position = NULL;
		return x;
	}
	p->next = q;
	while (q && ((q->h < w.h) || (q->h == w.h && q->u < w.u)))
	{
		p = q;
		q = q->next;
	}
	if (q && q->h == w.h && q->u == w.u)
	{
		q = p;
		x.in = 1; x.position = q;
		return x;
	}
	else
	{
		q = p;
		x.in = 0; x.position = q;
		return x;
	}
}

void AddWavefront(Wavefront& w, WaveStruct& s)		//add the wavefront Index w to s
{
	Wavefront* q;
	q = s.next;
	Inposition x; x = in(w, s);
	if (!q)
		s.next = &w;
	else if (!x.in)
	{
		q = x.position;
		if ((q->h < w.h) || (q->h == w.h && q->u < w.u))
		{
			if (q->next == s.next)
			{
				w.next = q->next;
				s.next = &w;
			}
			else {
				w.next = q->next;
				q->next = &w;
			}

			/*if (q->h < s.next->h || (q->h == w.h && q->u < w.u))
				s.next == &w;
				*/
		}
		else
		{
			w.next = s.next;
			s.next = &w;
		}
	}
	q = s.next;
	/*
	while (q)
	{
		cout << q->h << q->u << endl;
		q = q->next;
	}
	*/
}

void DelWavefront(Wavefront& w, WaveStruct& s)		//delete a wavefront which equals to w from s
{
	Wavefront* q;
	q = s.next;
	Inposition x; x = in(w, s);
	if (q && q->h == w.h && q->u == w.u)
	{
		s.next = s.next->next;
	}
	else if (x.in)
	{
		q = x.position;
		q->next = q->next->next;
	}
}

void WavefrontExtend(WaveStruct& M_s, Graph q, string& t)
{
	Wavefront S;
	InitWavefront(S);
	if (M_s.next)
	{
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
			AddWavefront(*pS, M_s);
		}
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
		int m = size(t);
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
			if (tran(q.node[k->Index].base[0]) != tran(t[h + 1]))
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

void WavefrontNext(WaveStruct M[], WaveStruct I[], WaveStruct D[], Graph& q, string& t, int s, penalty p)
{
	Wavefront* w;
	Wavefront* new_w;
	int k;
	Wavefront* temp = new Wavefront;
	if (s - p.o - p.e >= 0)
	{
		w = M[s - p.o - p.e].next;
		while (w)
		{
			new_w = new Wavefront;
			new_w->h = w->h + 1;
			new_w->u = w->u;
			new_w->next = NULL;
			for (k = 0; k < s; k++)
			{
				if (in(*new_w, I[k]).in)
					break;
			}
			if (k == s)
			{
				new_w->back = w;
				AddWavefront(*new_w, I[s]);
				for (k = 0; k < s; k++)
				{
					if (in(*new_w, M[k]).in)
						break;
				}
				if (k == s)
				{
					AddWavefront(*new_w, M[s]);
				}
			}
			NextNode* next_node;
			next_node = q.node[w->u].next;
			while (next_node)
			{
				new_w = new Wavefront;
				new_w->h = w->h;
				new_w->u = next_node->Index;
				new_w->next = NULL;
				new_w->back = w;
				for (k = 0; k < s; k++)
				{
					if (in(*new_w, D[k]).in)
						break;
				}
				if (k == s)
				{
					AddWavefront(*new_w, D[s]);
					for (k = 0; k < s; k++)
					{
						if (in(*new_w, M[k]).in)
							break;
					}
					if (k == s)
						AddWavefront(*new_w, M[s]);
				}
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
			for (k = 0; k < s; k++)
			{
				Wavefront* p = new Wavefront;
				if (in(*new_w, I[k]).in)
					break;
			}
			if (k == s)
			{
				AddWavefront(*new_w, I[s]);
				for (k = 0; k < s; k++)
				{
					if (in(*new_w, M[k]).in)
						break;
				}
				if (k == s)
					AddWavefront(*new_w, M[s]);
			}
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
				for (k = 0; k < s; k++)
				{
					if (in(*new_w, D[k]).in)
						break;
				}
				if (k == s)
				{
					AddWavefront(*new_w, D[s]);
					for (k = 0; k < s; k++)
					{
						if (in(*new_w, M[k]).in)
							break;
					}
					if (k == s)
						AddWavefront(*new_w, M[s]);
				}
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
				for (k = 0; k < s; k++)
				{
					if (in(*new_w, M[k]).in)
						break;

				}
				if (k == s)
					AddWavefront(*new_w, M[s]);
				next_node = next_node->next;
			}
			w = w->next;
		}
	}
}

void WFGraphAlign(Graph& q, string& t, penalty p)
{
	int m; int n; int s;
	m = size(t);
	n = q.num + 1;
	s = 0;
	WaveStruct M[MAX_SCORE + 1];
	WaveStruct I[MAX_SCORE + 1];
	WaveStruct D[MAX_SCORE + 1];
	for (int k = 0; k <= MAX_SCORE; k++)
	{
		M[k].score = k; M[k].next = NULL;
		I[k].score = k; I[k].next = NULL;
		D[k].score = k; D[k].next = NULL;
	}
	Wavefront* w; Wavefront* temp = new Wavefront;
	Inposition x;
	w = new Wavefront;
	w->h = -1; w->u = 0; w->next = NULL;
	AddWavefront(*w, M[0]);
	while (1)
	{
		WavefrontExtend(M[s], q, t);
		w = new Wavefront;
		w->h = m - 1;
		w->u = n - 1;
		w->next = NULL;
		w->back = NULL;
		x = in(*w, M[s]);
		if (x.in)
			break;
		temp = M[s].next;
		while (temp)
		{
			cout << s << '\t' << temp->h << '\t' << temp->u << endl;
			temp = temp->next;
		}
		s++;
		WavefrontNext(M, I, D, q, t, s, p);
	}
	w = x.position->next;
	while (w)		//Backtrace
	{
		cout << w->h << "\t" << w->u << endl;
		w = w->back;
	}
}