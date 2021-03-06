#include <string.h>
#include "../utils/struct.h"
#include "../utils/commons.h"
#include "../data/string 1.h"
#include "../data/string 2.h"
#include <ctime>
#include <iostream>
//#include <stdlib.h>

using namespace std;

clock_t start_time,end_time;
bool *D;

DEFINE_INDEX;

int main()
{
	const int MAX_EDGE=2;
	const int MAX_NODE=16500;

	DEFINE_LINEAR_PENALTY;
	DEFINE_NODE(MAX_EDGE);
	DEFINE_GRAPH(MAX_NODE);
	DEFINE_WAVEFRONT_SET(500000);
	const int pattern_size = 16500;
	const int sequence_size = 15000;

	#include"GWFA_linear_gap.cpp"

	char* t = sequence;
	Graph* q=MALLOC(1,Graph);
	q->num = pattern_size;
	q->node[0].base = -1;
	int i=1;
	int final_node=pattern_size * 10 / 11;
	for (; i <= final_node; i++)
	{
		q->node[i].base=tran(pattern[i-1]);
		q->node[i-1].next[0]=i;
		q->node[i-1].edgenum=1;
	}
	
	srand((int)time(0));
	for (i=final_node + 1;i<=pattern_size;i++)
	{
		q->node[i].base=rand()%4;
		int pre_node=rand()%pattern_size;
		while(q->node[pre_node].edgenum>=2 && pre_node<final_node)
			pre_node++;
		if(pre_node<final_node)
		{
			q->node[pre_node].edgenum++;
			q->node[pre_node].next[q->node[pre_node].edgenum-1]=i;
			q->node[i].next[0]=pre_node+1;
			q->node[i].edgenum=1;
		}
	}
	
	int Bool_size = (sequence_size + 1) * (pattern_size + 1);
	D = MALLOC(Bool_size, bool);
	penalty p = { 4,3 };
	int hash_size = prime(MAX(p.mismatch, p.gap));
	Wavefront_set* H = MALLOC(hash_size, Wavefront_set);
	start_time=clock();
	int score = GWF_LINEAR(t, q, p, sequence_size, pattern_size, final_node, D, H, hash_size);
	end_time=clock();
	double endtime=(double)(end_time-start_time)/CLOCKS_PER_SEC;
	cout<<"Total time:"<<endtime<<endl;
	cout<<"Total time:"<<endtime*1000<<"ms"<<endl;
	free(q);
	free(H);
	free(D);
	printf("The best alignment score is: %d", score);
	return 0;
}
		



