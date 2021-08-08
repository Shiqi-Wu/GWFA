#include <iostream>
#include <string.h>


#define m 800
#define n 800

#define MAX_SCORE 2000

using namespace std;

int new_Matrix[m][n];
int s[m]; int t[n];

#include "wavefront_reduce.h"

int main()
{
	#include "string1.h"
	#include "string2.h"
	/* Wavefront on graph */
	Graph q;
	q.num = n;
	q.node[0].base = -1;
	for (int i = 1; i <= n; i++)
	{
		q.node[i].base = tran(str_s1[i - 1]);
	}
	for (int i = 0; i <= n-1; i++)
	{
		Node* p = new Node;
		p->base = i + 1;
		p->next = NULL;
		q.node[i].next = p;
	}
	q.node[n].next = NULL;
	penalty pen = { 2,3,1 };
	for (int i = 0; i < m; i++)
	{
		s[i] = tran(str_s2[i]);
	}
	int gap;
	cin >> gap;
	WFGraphAlign(q, s, pen, gap);
	return 0;
}