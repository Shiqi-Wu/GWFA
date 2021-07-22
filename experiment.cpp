#include <iostream>
#include <string.h>


#define m 20
#define n 20

#define MAX_VERTEX_NUM 10

#define MAX_SCORE 20

using namespace std;

int new_Matrix[m][n];

bool M_in[m + 1][n + 1];
bool I_in[m + 1][n + 1];
bool D_in[m + 1][n + 1];

#include "wavefront.h"

int main()
{
	/* Create record matrix */
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			new_Matrix[i][j] = INT_MAX;
	/* Create random bool matrix */
	srand((int)time(0));
	bool V[n][n];

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (j == i + 1)
				V[i][j] = 1;
			else
				V[i][j] = rand() % 2;
			cout << V[i][j];
		}
		cout << endl;
	}

	/* Create random string */
	int s[m]; int t[n];
	srand((int)time(0));
	for (int k = 0; k < m; k++)
	{
		s[k] = rand() % 4;
		cout << s[k];
	}
	cout << endl;
	for (int k = 0; k < n; k++)
	{
		t[k] = rand() % 4;
		cout << t[k];
	}
	cout << endl;

	/* Navarro */
	int DP_Matrix[m][n];
	int temp;
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
			DP_Matrix[i][j] = 0;
	}
	for (int j = 0; j < n; j++)
		if (s[0] != t[j])
			DP_Matrix[0][j] = 1;		//Æô¶¯
	for (int i = 1; i < m; i++)
	{
		for (int j = 0; j < n; j++)		//Calculate P_ij(almost correct scores)
		{
			DP_Matrix[i][j] = DP_Matrix[i - 1][j] + 1;	//InitNextRow
		}
		for (int j = 0; j < n; j++)
		{
			int delta = int(s[i] != t[j]);
			temp = DP_Matrix[i][j];
			for (int k = 0; k < n; k++)
			{
				if (V[k][j] == 1)	//node k->node j
				{
					DP_Matrix[i - 1][k] + delta - temp > 0 ? temp = temp : temp = DP_Matrix[i - 1][k] + delta;
				}
			}
			DP_Matrix[i][j] = temp;
			//cout << DP_Matrix[i][j];
		}
		int max = 0;
		for (int j = 0; j < n; j++)
			max = DP_Matrix[i][j] > max ? DP_Matrix[i][j] : max;
		int Index[n];
		int rank_record = 0;
		for (int k = 0; k <= max; k++)
		{
			for (int j = 0; j < n; j++)
			{
				if (DP_Matrix[i][j] == k)
				{
					Index[rank_record] = j;
					rank_record++;
				}
			}
		}

		int new_line[1][n];
		for (int j = 0; j < n; j++)		//Calculate C_ij
		{
			new_line[0][j] = DP_Matrix[i][j];
		}
		int cal_rank;
		for (int j = 0; j < n; j++)
		{
			cal_rank = Index[j];
			for (int k = 0; k < n; k++)
			{
				if (V[k][cal_rank] == 1)
				{
					DP_Matrix[i][k] + 1 - new_line[0][cal_rank] > 0 ? new_line[0][cal_rank] = new_line[0][cal_rank] : new_line[0][cal_rank] = DP_Matrix[i][k] + 1;
				}
			}

		}
		for (int j = 0; j < n; j++)
		{
			DP_Matrix[i][j] = new_line[0][j];
		}

	}
	
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cout << DP_Matrix[i][j] << "\t";
		}
		cout << endl;
	}

	/* Wavefront on graph */
	Graph q;
	q.num = n;
	q.node[0].base = -1;
	for (int i = 1; i <= n; i++)
	{
		q.node[i].base = t[i - 1];
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
	
	
	penalty pen = { 1,0,1 };

	WFGraphAlign(q, s, pen);

	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cout << DP_Matrix[i][j] - new_Matrix[i][j] << '\t';
		}
		cout << endl;
	}

	return 0;
}