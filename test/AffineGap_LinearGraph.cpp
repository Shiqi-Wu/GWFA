#include <string.h>
#include "../utils/struct.h"
#include "../utils/commons.h"
#include "../data/string 1.h"
#include "../data/string 2.h"
#include <ctime>
#include <iostream>

#pragma comment(linker, "/STACK:102400000,102400000")

using namespace std;

clock_t start_time,end_time;

DEFINE_INDEX;
DEFINE_AFFINE_STATUS;

Affine_status Status;

int main(int argc, char *argv[])
{
	const int MAX_EDGE = 1;
	const int MAX_NODE = 15000;
	
	DEFINE_AFFINE_PENALTY;
	DEFINE_NODE(MAX_EDGE);
	DEFINE_GRAPH(MAX_NODE);
	DEFINE_WAVEFRONT_SET(400000);
	DEFINE_AFFINE_WAVEFRONT_SET;
	const int pattern_size = 15000;
	const int sequence_size = 15000;
	
	#include"GWFA_affine_gap.cpp"
	
	int* t=MALLOC(sequence_size,int);
	for (int i=0; i<sequence_size; i++)
	{
		t[i]=tran(sequence[i]);
	}

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
	Status.M = MALLOC(Bool_size, bool);
	Status.D = MALLOC(Bool_size, bool);
	Status.I = MALLOC(Bool_size, bool);
	penalty p = { 2,3,1 };
	int hash_size = prime(MAX(p.mismatch, p.gap_open+p.gap_extention));
	Affine_wavefront_set* W = MALLOC(hash_size, Affine_wavefront_set);
	start_time=clock();	
	int score = GWF_AFFINE(t, q, p, sequence_size, pattern_size,pattern_size, Status, W, hash_size);
	end_time=clock();
	double endtime=(double)(end_time-start_time)/CLOCKS_PER_SEC;
	cout<<"Total time:"<<endtime<<endl;		//s为单位
	cout<<"Total time:"<<endtime*1000<<"ms"<<endl;	//ms为单位
	free(q);free(t);
	free(Status.M);free(Status.D);free(Status.I);
	free(W);
	printf("The best alignment score is: %d", score);
	
	return 0;
}
