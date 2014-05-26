CC=g++
CFLAGS=-I.

all: kbmira extractor evaluator hgmira forest_rescore_test hypergraph_test tests

tests:
	./forest_rescore_test
	./hypergraph_test

extractor: mertlib
	$(CC) -o extractor -Wl,--start-group mert/extractor.o libmert_lib.a -Wl,-Bstatic -lm -lbz2 -lboost_thread-mt -lboost_system-mt  -lboost_filesystem-mt -Wl,-Bdynamic -ldl -lSegFault -lz -lrt -Wl,--end-group -pthread

evaluator: mertlib
	$(CC) -o evaluator  -Wl,--start-group mert/evaluator.o libmert_lib.a -Wl,-Bstatic -lm -lbz2 -lboost_thread-mt -lboost_system-mt  -lboost_filesystem-mt -Wl,-Bdynamic -ldl -lSegFault -lz -lrt -Wl,--end-group -pthread

kbmira: mertlib
#	$(CC) -ftemplate-depth-128 -O3 -finline-functions -Wno-inline -Wall -pthread  -DNDEBUG -DTRACE_ENABLE=1 -DWITH_THREADS -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES $(CFLAGS) -c -o mert/kbmira.o mert/kbmira.cpp
	$(CC) -o $@ -Wl,--start-group mert/kbmira.o libmert_lib.a -Wl,-Bstatic -lboost_program_options-mt -lm -lbz2 -lboost_thread-mt -lboost_system-mt -lboost_filesystem-mt -Wl,-Bdynamic -ldl -lSegFault -lz -lrt -Wl,--end-group -pthread

hgmira: mertlib
	$(CC) -o $@ -Wl,--start-group mert/hgmira.o libmert_lib.a -Wl,-Bstatic -lboost_program_options-mt -lm -lbz2 -lboost_thread-mt -lboost_system-mt -lboost_filesystem-mt -Wl,-Bdynamic -ldl -lSegFault -lz -lrt -Wl,--end-group -pthread

forest_rescore_test: mertlib
	$(CC) -o $@ -Wl,--start-group mert/ForestRescoreTest.o libmert_lib.a  -Wl,-Bstatic -lboost_unit_test_framework-mt -lm  -lbz2  -lboost_thread-mt -lboost_system-mt -lz -Wl,-Bdynamic -ldl -lSegFault -lrt -Wl,--end-group -g -pthread

hypergraph_test: mertlib
	$(CC) -o $@ -Wl,--start-group mert/HypergraphTest.o libmert_lib.a  -Wl,-Bstatic -lboost_unit_test_framework-mt -lm  -lbz2  -lboost_thread-mt -lboost_system-mt -lz -Wl,-Bdynamic -ldl -lSegFault -lrt -Wl,--end-group -g -pthread


OBJS=mert/Util.o mert/GzFileBuf.o mert/FileStream.o mert/Timer.o mert/ScoreStats.o mert/ScoreArray.o mert/ScoreData.o mert/ScoreDataIterator.o mert/FeatureStats.o mert/FeatureArray.o mert/FeatureData.o mert/FeatureDataIterator.o mert/ForestRescore.o mert/ForestRescoreTest.o mert/MiraFeatureVector.o mert/MiraWeightVector.o mert/HypPackEnumerator.o mert/Data.o mert/BleuScorer.o mert/BleuDocScorer.o mert/SemposScorer.o mert/SemposOverlapping.o mert/InterpolatedScorer.o mert/Point.o mert/PerScorer.o mert/Scorer.o mert/ScorerFactory.o mert/Optimizer.o mert/OptimizerFactory.o mert/TER/alignmentStruct.o mert/TER/hashMap.o mert/TER/hashMapStringInfos.o mert/TER/stringHasher.o mert/TER/terAlignment.o mert/TER/terShift.o mert/TER/hashMapInfos.o mert/TER/infosHasher.o mert/TER/stringInfosHasher.o mert/TER/tercalc.o mert/TER/tools.o mert/TerScorer.o mert/CderScorer.o mert/Vocabulary.o mert/PreProcessFilter.o mert/SentenceLevelScorer.o mert/Permutation.o mert/PermutationScorer.o mert/StatisticsBasedScorer.o util/read_compressed.o util/double-conversion/cached-powers.o util/double-conversion/double-conversion.o util/double-conversion/diy-fp.o util/double-conversion/fast-dtoa.o util/double-conversion/bignum.o util/double-conversion/bignum-dtoa.o util/double-conversion/strtod.o util/double-conversion/fixed-dtoa.o util/bit_packing.o util/ersatz_progress.o util/exception.o util/file.o util/file_piece.o util/mmap.o util/murmur_hash.o util/pool.o util/scoped.o util/string_piece.o util/usage.o mert/Hypergraph.o mert/HypergraphTest.o mert/HopeFearDecoder.o



#libmert_lib.a:
mertlib:
	$(MAKE) -C util
	$(MAKE) -C util/double-conversion
	$(MAKE) -C mert
	$(MAKE) -C mert/TER

	/usr/bin/ar rc libmert_lib.a $(OBJS)

clean:
	$(MAKE) -C util clean
	$(MAKE) -C util/double-conversion clean
	$(MAKE) -C mert clean
	$(MAKE) -C mert/TER clean
	rm -f kbmira evaluator extractor libmert_lib.a
