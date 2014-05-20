#include <iostream>

#include "ForestRescore.h"

#define BOOST_TEST_MODULE MertPoint
#include <boost/test/unit_test.hpp>



using namespace std;
using namespace MosesTuning;

BOOST_AUTO_TEST_CASE(viterbi_simple_lattice)
{
  Vocab vocab;
  WordVec words;
  string wordStrings[] =
    {"<s>", "</s>", "a", "b", "c", "d", "e", "f", "g"};
  for (size_t i = 0; i < 9; ++i) {
    words.push_back(&(vocab.FindOrAdd((wordStrings[i]))));
  }

  Graph graph(vocab);
  graph.SetCounts(5,5);

  Edge* e0 = graph.NewEdge();
  e0->AddWord(words[0]);

  Vertex* v0 = graph.NewVertex();
  v0->AddEdge(e0);

  Edge* e1 = graph.NewEdge();
  e0->AddWord(NULL);
  e0->AddChild(0);
  e0->AddWord(words[2]);
  e0->AddWord(words[3]);

  Vertex* v1 = graph.NewVertex();
  v1->AddEdge(e1);

  Edge* e2 = graph.NewEdge();
  e2->AddWord(NULL);
  e2->AddChild(1);
  e2->AddWord(words[4]);
  e2->AddWord(words[5]);

  Vertex* v2 = graph.NewVertex();
  v2->AddEdge(e2);

  Edge* e3 = graph.NewEdge();
  e3->AddWord(NULL);
  e3->AddChild(2);
  e3->AddWord(words[6]);
  e3->AddWord(words[7]);
  e3->AddWord(words[8]);

  Vertex* v3 = graph.NewVertex();
  v3->AddEdge(e3);

  Edge* e4 = graph.NewEdge();
  e4->AddWord(NULL);
  e4->AddChild(3);
  e4->AddWord(words[1]);

  Vertex* v4 = graph.NewVertex();
  v4->AddEdge(e4);



}
