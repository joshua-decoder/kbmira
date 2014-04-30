CC=g++
CFLAGS=-I.. -I../util
OBJS = BleuDocScorer.o BleuScorer.o BleuScorerTest.o CderScorer.o Data.o DataTest.o evaluator.o extractor.o FeatureArray.o FeatureData.o FeatureDataIterator.o FeatureDataTest.o FeatureStats.o FileStream.o GzFileBuf.o hgmira.o Hypergraph.o HypPackEnumerator.o InterpolatedScorer.o kbmira.o mert.o MiraFeatureVector.o MiraWeightVector.o NgramTest.o Optimizer.o OptimizerFactory.o OptimizerFactoryTest.o Permutation.o PermutationScorer.o PerScorer.o Point.o PointTest.o PreProcessFilter.o pro.o ReferenceTest.o ScoreArray.o ScoreData.o ScoreDataIterator.o Scorer.o ScorerFactory.o ScoreStats.o SemposOverlapping.o SemposScorer.o sentence-bleu.o SentenceLevelScorer.o SingletonTest.o StatisticsBasedScorer.o TerScorer.o Timer.o TimerTest.o Util.o UtilTest.o Vocabulary.o VocabularyTest.o

all: $(OBJS)

%.o: %.cpp
	$(CC) -ftemplate-depth-128 -g -finline-functions -Wno-inline -Wall -pthread -DTRACE_ENABLE=1 -DWITH_THREADS -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES $(CFLAGS) -c -o $@ $<
#	$(CC) -ftemplate-depth-128 -O3 -finline-functions -Wno-inline -Wall -pthread -DNDEBUG -DTRACE_ENABLE=1 -DWITH_THREADS -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *~

