#include<iostream>
#include<string>

#include "../utils/struct.h"
#include "../utils/commons.h"

static int index_num;

DEFINE_LINEAR_PENALTY;
DEFINE_INDEX;

/*DEFINE_EXTEND class GWFAE{	
		Wavefront_set EXTEND(Wavefront_set original_wavefront_set, int index_num, bool D[], int pattern_size, int sequence_size, char* t, Node q[]);	\
};	
Wavefront_set GWFAE::EXTEND(Wavefront_set original_wavefront_set, int index_num, bool D[], int pattern_size, int sequence_size, char* t, Node q[])	\
{	
	int final_index_num = -1;	
	Index temp_index;	
	Index* final_wavefront_set;	
	final_wavefront_set = MALLOC(1,Wavefront_set);	
	while (index_num >= 0)	
	{	
		temp_index = original_wavefront_set[index_num];	
		index_num--;	
		if (D[pattern_size * temp_index.h + temp_index.u])	
		{	
			continue;	
		}	
		D[pattern_size * temp_index.h + temp_index.u] = 1;	
		int n = 0;	
		for (int i = 0; i < q[temp_index.u].edgenum; i++)	
		{	
			int w = q[temp_index.u].next[i].u;	
			if (temp_index.h < sequence_size && t[temp_index.h + 1] == q[w].base)	
			{	
				index_num++;	
				original_wavefront_set[index_num].h = temp_index.h + 1;	
				original_wavefront_set[index_num].u = w;	
				n++;	
			}	
			if (q[temp_index.u].edgenum == 0 || q[temp_index.u].edgenum > n)	
			{	
				final_index_num++;	
				final_wavefront_set[final_index_num] = temp_index;	
			}	
		}	
	}	
	index_num = final_index_num;	
	return final_wavefront_set;	
}*/

#define GWFA_EXTEND(original_wavefront_set, index_num, D, pattern_size, sequence_size, t, q) GWFAE.EXTEND(original_wavefront_set, index_num, D, pattern_size, sequence_size, t, q);
