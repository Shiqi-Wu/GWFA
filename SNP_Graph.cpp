#include <iostream>
#include <string.h>
#include <time.h>


#define m 80
#define n 88
#define n2 80

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
	for (int i = 1; i <= n2; i++)
	{
		q.node[i].base = tran(str_s1[i - 1]);
	}
	for (int i = 0; i <= n2 - 1; i++)
	{
		Node* p = new Node;
		p->base = i + 1;
		p->next = NULL;
		q.node[i].next = p;
	}
	q.node[n2].next = NULL;
	for (int i = n2 + 1; i <= n - 1; i++)
	{
		srand((int)time(NULL));
		//cout << i << endl;
		q.node[i].base = rand() % 4;
		//cout << q.node[i].base << endl;
		srand((int)time(NULL));
		Node* p = new Node;
		int temp = (i - n2 - 1) * 10 + (rand() % 10) + 1;
		//cout << temp << endl;
		p->base = i;
		p->next = q.node[temp].next;
		q.node[temp].next = p;
		p = new Node;
		p->base = temp + 1;
		p->next = NULL;
		q.node[i].next = p;
	}
	srand((int)time(NULL));
	//cout << i << endl;
	q.node[n].base = rand() % 4;
	//cout << q.node[i].base << endl;
	srand((int)time(NULL));
	Node* p = new Node;
	int temp = (n - n2 - 1) * 10 + (rand() % 10) + 1;
	//cout << temp << endl;
	if (temp == n2)
		temp = temp - 1;
	p->base = n;
	p->next = q.node[temp].next;
	q.node[temp].next = p;
	p = new Node;
	p->base = temp + 1;
	p->next = NULL;
	q.node[n].next = p;
	penalty pen = { 2,3,1 };
	for (int i = 0; i < m; i++)
	{
		s[i] = tran(str_s2[i]);
	}
	int gap=800;
	//cin >> gap;
	WFGraphAlign(q, s, pen, gap);
	return 0;
}