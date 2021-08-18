#include <iostream>
#include <string.h>

#define m 5
#define n 5
#define MAX_EDGE 3
#define MAX_WAVEFRONT 25
#define HASH_SIZE 13

using namespace std;

#include "wavefront_forward.h"




int main()
{
	string t = "ACTGC";

	Graph q;
	q.num = m;
	q.node[0].base = -1;
	string s = "TACGA";
	for (int i = 1; i <= m; i++)
	{
		q.node[i].base = tran(s[i - 1]);
	}
	q.node[0].edgenum = 1;
	q.node[0].next[0] = 1;
	q.node[1].edgenum = 1;
	q.node[1].next[0] = 2;
	q.node[2].edgenum = 2;
	q.node[2].next[0] = 3;
	q.node[2].next[1] = 4;
	q.node[3].edgenum = 2;
	q.node[3].next[0] = 1;
	q.node[3].next[1] = 4;
	q.node[4].edgenum = 1;
	q.node[4].next[0] = 5;
	q.node[5].edgenum = 1;
	q.node[5].next[0] = 1;

	penalty p = { 4,6,2 };

	WF_Graph_Align(q, t, p);
}