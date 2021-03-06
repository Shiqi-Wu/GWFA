// DESCRIPTION: Padded string module to avoid handling corner conditions


#ifndef STRING_PADDED_H
#define STRING_PADDED_H

/*
 * Includes
 */
#include "../utils/commons.h"
#include "../system/mm_allocator.h"
#include "../utils/graph_struct.h"

/*
 * Strings Padded
 */

typedef struct{
    char* pattern_padded_buffer;
    char* pattern_padded;
}segment_padded_t;

typedef struct{
    int node_num;
    segment_padded_t* node;
} graph_padded_t;

typedef struct {
  // Strings
  graph_padded_t* graph_padded;
  char* text_padded_buffer;
  char* text_padded;
  // MM
  mm_allocator_t* mm_allocator;
} strings_padded_t;

/*
 * Strings (text/pattern) padded
 */
void strings_padded_add_padding(
    const char* const buffer,
    const int buffer_length,
    const int begin_padding_length,
    const int end_padding_length,
    const char padding_value,
    char** const buffer_padded,
    char** const buffer_padded_begin,
    mm_allocator_t* const mm_allocator);

strings_padded_t* strings_padded_new_rhomb(
    const graph_t* const graph,
    const char* const text,
    const int text_length,
    const int padding_length,
    mm_allocator_t* const mm_allocator);

void strings_padded_delete(strings_padded_t* const strings_padded);

#endif /* STRING_PADDED_H */
