//DESCRIPTION: Graph struct in GWFA Algorithm

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