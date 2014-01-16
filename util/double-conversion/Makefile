CC=g++
CFLAGS="-I../.."

OBJS = bignum.o bignum-dtoa.o cached-powers.o diy-fp.o double-conversion.o fast-dtoa.o fixed-dtoa.o strtod.o

all: $(OBJS)

%.o: %.cc
	$(CC) -ftemplate-depth-128 -O3 -finline-functions -Wno-inline -Wall -pthread  -DNDEBUG -DTRACE_ENABLE=1 -DWITH_THREADS -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *~ 
