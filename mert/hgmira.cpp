/***********************************************************************
Moses - factored phrase-based language decoder
Copyright (C) 2014- University of Edinburgh

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
***********************************************************************/

#include <iostream>
#include <sstream>
#include <vector>


#include <boost/program_options.hpp>

#include <util/file_piece.hh>


#include "ForestRescore.h"
#include "Hypergraph.h"

using namespace std;
using namespace MosesTuning;

namespace po = boost::program_options;

int main(int argc, char** argv) 
{
  bool help;
  string current_hypergraphs = "./test_data/hypergraph";
  string weights_file = "./test_data/weights";
  vector<string> references;
  float bleuWeight = 1;
//  string current_hypergraphs = "/home/bhaddow/experiments/sparse-reordering/hg/hypergraph";
  //Think this is just for vocabulary.
  //string lm_file = "/fs/magni0/bhaddow/experiments/sparse-reordering/lm/project-syndicate.binlm.1";

  po::options_description desc("Allowed options");
  desc.add_options()
  ("help,h", po::value(&help)->zero_tokens()->default_value(false), "Print this help message and exit")
  ("current-hypergraphs", po::value<string>(&current_hypergraphs), "Directory containing hypergraphs")
  ("reference,r", po::value<vector<string> >(&references), "Reference file(s)")
  ("bleu-weight,b", po::value<float>(&bleuWeight), "Bleu weight")
  ;

  po::options_description cmdline_options;
  cmdline_options.add(desc);
  po::variables_map vm;
  po::store(po::command_line_parser(argc,argv).
            options(cmdline_options).run(), vm);
  po::notify(vm);
  if (help) {
    cout << "Usage: " + string(argv[0]) +  " [options]" << endl;
    cout << desc << endl;
    exit(0);
  }

//  lm::ngram::ProbingModel lm(lm_file.c_str());
  
  stringstream name;
  name << current_hypergraphs << "/5.gz";
  references.push_back("./test_data/reference.tok");
  util::FilePiece file(name.str().c_str());
 // lm::ngram::SortedVocabulary vocab;

  
  Graph graph;
  ReadGraph(file, graph);

  ReferenceSet referenceSet;
  referenceSet.Load(references, graph.MutableVocab());

  
  WordVec ngram;
  ngram.push_back(&(graph.MutableVocab().FindOrAdd("the")));
  cerr << referenceSet.NgramMatches(9,ngram,true) << endl;
  ngram.push_back(&(graph.MutableVocab().FindOrAdd("1990")));
  cerr << referenceSet.NgramMatches(9,ngram,true) << endl;
  
  SparseVector weights;
  weights.load(weights_file);
  /*
  for (size_t i = 0; i < 10; ++i) {
    cerr << graph.GetEdge(i).GetScore(weights) << endl;
  }*/
  WordVec translation;
  Viterbi(graph,weights,bleuWeight,referenceSet,5,&translation);
  for (size_t i = 0; i < translation.size(); ++i) {
    cerr << translation[i]->first << " ";
  }
  cerr << endl;
}
