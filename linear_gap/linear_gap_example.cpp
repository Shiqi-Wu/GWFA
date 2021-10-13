#include <string.h>
#include "../utils/struct.h"
#include "../utils/commons.h"
#include "../data/string 1.h"
#include "../data/string 2.h"
#include <ctime>
#include <iostream>

using namespace std;

clock_t start_time,end_time;

DEFINE_INDEX;


int main()
{
	const int MAX_EDGE = 1;
	const int MAX_NODE = 15000;
	
	DEFINE_LINEAR_PENALTY;
	DEFINE_NODE(MAX_EDGE);
	DEFINE_GRAPH(MAX_NODE);
	DEFINE_WAVEFRONT_SET(500000);
	const int pattern_size = 15000;
	const int sequence_size = 15000;

	#include"GWFA_linear_gap.cpp"
	
	//char sequence[6] = "ACTGC";
	char* t = sequence;

	Graph *q=MALLOC(1,Graph);
	q->num = pattern_size;
	q->node[0].base = -1;
	//string s = "TACGA";
	for (int i = 1; i <= pattern_size; i++)
	{
		q->node[i].base = tran(pattern[i - 1]);
		q->node[i-1].next[0]=i;
		//printf("%d\t%d\n", q->node[i-1].next[0],q->node[i].base);
		q->node[i-1].edgenum=1;
	}
	/*
	for(int i=0;i<sequence_size;i++)
	{
		printf("%d\t%d\n", i, tran(sequence[i]));
	}
	*/
	/*
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
	*/
	int Bool_size = (sequence_size + 1) * (sequence_size + 1);
	bool* D = MALLOC(Bool_size, bool);
	penalty p = { 13,17 };
	int hash_size = prime(MAX(p.mismatch, p.gap));
	Wavefront_set* H = MALLOC(hash_size, Wavefront_set);
	start_time=clock();	
	int score = GWF_EXTEND(t, q, p, sequence_size, pattern_size, D, H, hash_size);
	end_time=clock();		//程序结束用时
	double endtime=(double)(end_time-start_time)/CLOCKS_PER_SEC;
	cout<<"Total time:"<<endtime<<endl;		//s为单位
	cout<<"Total time:"<<endtime*1000<<"ms"<<endl;	//ms为单位
	free(q);
	free(H);
	free(D);
	printf("The best alignment score is: %d", score);
	
	return 0;
}
