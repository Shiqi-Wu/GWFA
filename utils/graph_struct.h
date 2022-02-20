//DESCRIPTION: Graph struct in GWFA Algorithm

#ifndef GRAPH_STRUCT_H
#define GRAPH_STRUCT_H

typedef struct{
    int node_idx;
    int pattern_length;
    char* node_pattern;
    int next_num;
    int* next;
}node_t;

typedef struct{
    int pattern_length;
    int node_num;
    node_t* node;
}graph_t;

#endif