#include <iostream>
#include <string.h>

bool M_in[m + 1][n + 1];
bool I_in[m + 1][n + 1];
bool D_in[m + 1][n + 1];

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
	int next[MAX_EDGE];
	int edgenum;
}Node;

typedef struct Graph
{
	int num;
	Node node[n + 1];
}Graph;

typedef struct Index
{
	int h;
	int u;
};

typedef struct Wavefront
{
	int score;
	Index index[MAX_WAVEFRONT];
	int index_num;
};
