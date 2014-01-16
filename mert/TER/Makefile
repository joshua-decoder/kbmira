CC=g++
CFLAGS=
OBJS = alignmentStruct.o hashMap.o hashMapInfos.o hashMapStringInfos.o infosHasher.o stringHasher.o stringInfosHasher.o terAlignment.o tercalc.o terShift.o tools.o

all: $(OBJS)

%.o: %.cpp
	$(CC) -ftemplate-depth-128 -O3 -finline-functions -Wno-inline -Wall -pthread  -DNDEBUG -DTRACE_ENABLE=1 -DWITH_THREADS -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *~
