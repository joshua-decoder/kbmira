CC=g++
CFLAGS="-I../"
OBJS = bit_packing.o bit_packing_test.o ersatz_progress.o exception.o file.o file_piece.o file_piece_test.o joint_sort_test.o mmap.o multi_intersection_test.o murmur_hash.o pool.o probing_hash_table_test.o read_compressed.o read_compressed_test.o scoped.o sorted_uniform_test.o string_piece.o tokenize_piece_test.o usage.o

all: $(OBJS)

%.o: %.cc
	$(CC) -ftemplate-depth-128 -O3 -finline-functions -Wno-inline -Wall -pthread -DHAVE_BZLIB -DHAVE_ZLIB -DNDEBUG -DTRACE_ENABLE=1 -DWITH_THREADS -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *~
