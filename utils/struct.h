#include <string.h>

#define DEFINE_AFFINE_PENALTY typedef struct penalty	\
{	\
	int mismatch;	\
	int gap_open;	\
	int gap_extention;	\
}penalty;

#define DEFINE_LINEAR_PENALTY typedef struct penalty	\
{	\
	int mismatch;	\
	int gap;	\
}penalty;

#define DEFINE_NODE(MAX_EDGE) typedef struct Node	\
{	\
	int base;	\
	int next[MAX_EDGE];	\
	int edgenum;	\
}Node;

#define DEFINE_GRAPH(MAX_NODE) typedef struct Graph	\
{	\
	int num;	\
	Node node[20000];	\
}Graph;

#define DEFINE_INDEX typedef struct Index	\
{	\
	int h;	\
	int u;	\
};

#define DEFINE_WAVEFRONT_SET(MAX_INDEX_SET) typedef struct Wavefront_set	\
{	\
	int score;	\
	Index index[MAX_INDEX_SET];	\
	int index_num;	\
};

/*int tran(char a) {
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
}*/
