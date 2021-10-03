#include <iostream>
#include <string.h>
#include "wavefront.h"

#define m 4
#define n 5

#define MAX_VERTEX_NUM 10

#define MAX_SCORE 20

using namespace std;

int main()
{
	Graph q;
	q.num = n;
	string s = "CATTA";

	bool V[n][n] = {
	{0,1,0,0,0},
	{0,0,1,0,0},
	{1,0,0,1,0},
	{0,0,0,0,1},
	{0,0,1,0,0}
	};

	q = GraphInit(V, s);
	string t = "TATT";
	int tran_string[m];
	for (int i = 0; i < m; i++)
	{
		tran_string[i] = tran(t[i]);
	}
	penalty p = { 1,0,1 };

	WFGraphAlign(q, tran_string, p);

	return 0;
}