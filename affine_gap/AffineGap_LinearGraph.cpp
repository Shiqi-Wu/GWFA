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


int main(int argc, char *argv[])
{
	const int MAX_EDGE = 1;
	const int MAX_NODE = 15000;
	
	DEFINE_AFFINE_PENALTY;
	DEFINE_NODE(MAX_EDGE);
	DEFINE_GRAPH(MAX_NODE);
	DEFINE_WAVEFRONT_SET(500000);
	const int pattern_size = 15000;
	const int sequence_size = 15000;
	
	#include"GWFA_affine_gap.cpp"
	
	char* t = sequence;

	Graph *q=MALLOC(1,Graph);
	q->num = pattern_size;
	q->node[0].base = -1;
	
	for (int i = 1; i <= pattern_size; i++)
	{
		q->node[i].base = tran(pattern[i - 1]);
		q->node[i-1].next[0]=i;
		//printf("%d\t%d\n", q->node[i-1].next[0],q->node[i].base);
		q->node[i-1].edgenum=1;
	}

	int Bool_size = (sequence_size + 1) * (pattern_size + 1);
	bool* M = MALLOC(Bool_size, bool);
	bool* D = MALLOC(Bool_size, bool);
	bool* I = MALLOC(Bool_size, bool);
	penalty p = { 4,6,2 };
	int hash_size = prime(MAX(p.mismatch, p.gap_open+p.gap_extention));
	Wavefront_set* Index_set_M = MALLOC(hash_size, Wavefront_set);
	Wavefront_set* Index_set_D = MALLOC(hash_size, Wavefront_set);
	Wavefront_set* Index_set_I = MALLOC(hash_size, Wavefront_set);
	start_time=clock();	
	int score = GWF_EXTEND(t, q, p, sequence_size, pattern_size, M, D, I, Index_set_M, Index_set_D, Index_set_I, hash_size);
	end_time=clock();
	double endtime=(double)(end_time-start_time)/CLOCKS_PER_SEC;
	cout<<"Total time:"<<endtime<<endl;		//s为单位
	cout<<"Total time:"<<endtime*1000<<"ms"<<endl;	//ms为单位
	free(q);
	free(M);free(D);free(I);
	free(Index_set_M);free(Index_set_D);free(Index_set_I);
	printf("The best alignment score is: %d", score);
	
	return 0;
}