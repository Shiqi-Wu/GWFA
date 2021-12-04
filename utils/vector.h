/*
 * Data Structures
 */
typedef struct {
  void* memory;
  uint64_t used;
  uint64_t element_size;
  uint64_t elements_allocated;
} vector_t;
