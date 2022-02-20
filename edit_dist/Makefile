objects = main.o edit_wavefront_align.o edit_wavefront_extend.o edit_wavefront.o\
		commons.o string_padded.o mm_allocator.o vector.o

edit: $(objects)
	gcc -o edit $(objects) -g

main.o: main.c
	gcc -c main.c -g

edit_wavefront_align.o: edit_wavefront_align.c 
	cc -c edit_wavefront_align.c -g

edit_wavefront_extend.o: edit_wavefront_extend.c 
	cc -c edit_wavefront_extend.c -g

edit_wavefront.o: edit_wavefront.c
	cc -c edit_wavefront.c -g

commons.o: ../utils/commons.c
	gcc -c ../utils/commons.c -g

string_padded.o: ../utils/string_padded.c
	gcc -c ../utils/string_padded.c -g

mm_allocator.o: ../system/mm_allocator.c
	gcc -c ../system/mm_allocator.c -g

vector.o: ../utils/vector.c
	gcc -c ../utils/vector.c -g

.PHONY : clean
clean:
	rm edit $(objects)