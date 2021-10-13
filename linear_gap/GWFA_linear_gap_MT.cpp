#include <string.h>
#include <iostream>
#include "../utils/struct.h"
#include "../utils/commons.h"
#include "../utils/GWFA_fasta_read.h"

DEFINE_INDEX;


int main(int argc, char *argv[])
{
	char * sequence=GWFA_fasta_read(argv[1], se_size);
	char * pattern=GWFA_fasta_read(argv[2], pa_size);
	const int MAX_EDGE = 1;
	const int MAX_NODE = pa_size;
	const int pattern_size = pa_size;
	const int sequence_size = se_size;
	
	DEFINE_LINEAR_PENALTY;
	DEFINE_NODE(MAX_EDGE);
	DEFINE_GRAPH(20000);
	DEFINE_WAVEFRONT_SET(1000000);


	#include"GWFA_linear_gap.cpp"

	Graph *q=MALLOC(1,Graph);
	q->num = MAX_NODE;
	q->node[0].base = -1;
	for (int i = 1; i <= pattern_size; i++)
	{
		q->node[i].base = tran(pattern[i - 1]);
	}

	int Bool_size = (sequence_size + 1) * (sequence_size + 1);
	bool* D = MALLOC(Bool_size, bool);
	penalty p = { 4,3 };
	int hash_size = prime(MAX(p.mismatch, p.gap));
	Wavefront_set* H = MALLOC(hash_size, Wavefront_set);
	int score = GWF_EXTEND(sequence, q, p, sequence_size, pattern_size, D, H, hash_size);
	free(sequence);
	free(pattern);
	free(q);
	free(H);
	free(D);
	cout << score << endl;
	return 0;
}
