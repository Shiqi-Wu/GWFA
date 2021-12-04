#include"vector.h"

/*
constants
*/
#define VECTOR_EXPAND_FACTOR (3.0/2.0)

/*
Setup
*/
vector_t* vector_new_(const uint64_t num_initial_elements, const uint64_t element_size){
	vector_t* const vector_buffer = malloc(sizeof(vector_t));
	vector_buffer->element_size = element_size;