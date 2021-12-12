gcc -c -o bin/commons.o utils/commons.c
gcc -c -o bin/vector.o utils/vector.c
gcc -c -o bin/mm_allocator.o system/mm_allocator.c
g++ -c -o bin/test.o system/test.c