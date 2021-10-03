#include <iostream>
#include <string>


using namespace std;


#include "../utils/struct.h"
#include "../utils/commons.h"



int main()
{
	const int MAX_EDGE = 3;
	const int MAX_NODE = 5;
	DEFINE_INDEX;
	DEFINE_LINEAR_PENALTY;
	DEFINE_NODE(MAX_EDGE);
	DEFINE_GRAPH(MAX_NODE);
	DEFINE_WAVEFRONT_SET(100);
	const int pattern_size = 5;
	const int sequence_size = 5;

	#include"GWFA_linear_gap.h"
	
	char sequence[6] = "ACTGC";
	char* t = sequence;

	Graph *q=MALLOC(1,Graph);
	q->num = 5;
	q->node[0].base = -1;
	string s = "TACGA";
	for (int i = 1; i <= 5; i++)
	{
		q->node[i].base = tran(s[i - 1]);
	}
	q->node[0].edgenum = 1;
	q->node[0].next[0] = 1;
	q->node[1].edgenum = 1;
	q->node[1].next[0] = 2;
	q->node[2].edgenum = 2;
	q->node[2].next[0] = 3;
	q->node[2].next[1] = 4;
	q->node[3].edgenum = 2;
	q->node[3].next[0] = 1;
	q->node[3].next[1] = 4;
	q->node[4].edgenum = 1;
	q->node[4].next[0] = 5;
	q->node[5].edgenum = 1;
	q->node[5].next[0] = 1;
	int Bool_size = (sequence_size + 1) * (sequence_size + 1);
	bool* D = MALLOC(Bool_size, bool);
	penalty p = { 4,3 };
	int hash_size = prime(MAX(p.mismatch, p.gap));
	Wavefront_set* H = MALLOC(hash_size, Wavefront_set);
	int score = GWF_EXTEND(t, q, p, 5, 5, D, H, hash_size);
	free(q);
	free(H);
	free(D);
	//cout << score << endl;
	return 0;
}