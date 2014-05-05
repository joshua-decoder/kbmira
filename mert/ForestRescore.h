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
#ifndef MERT_FOREST_RESCORE_H
#define MERT_FOREST_RESCORE_H

#include <vector>

#include "Hypergraph.h"

namespace MosesTuning {

std::ostream& operator<<(std::ostream& out, const WordVec& wordVec);

class ReferenceSet {


public:

  void Load(std::vector<std::string>& files, Vocab& vocab);

  size_t NgramMatches(size_t sentenceId, const WordVec&, bool clip) const;

private:

  struct NgramHash : public std::unary_function<const WordVec&, std::size_t> {
    std::size_t operator()(const WordVec& ngram) const {
      size_t seed = 0;
      for (size_t i = 0; i < ngram.size(); ++i) {
        seed = util::MurmurHashNative(&(ngram[i]->second), sizeof(ngram[i]->second), seed);
      }
      return seed;
    }
  };

  struct NgramEquals : public std::binary_function<const WordVec&, const WordVec&, bool> {
    bool operator()(const WordVec& first, const WordVec& second) const {
      if (first.size() != second.size()) return false;
      for (size_t i = 0; i < first.size(); ++i) {
        if (first[i]->second != second[i]->second) return false;
      }
      return true;

    }
  };


  //ngrams to (clipped,unclipped) counts
  typedef boost::unordered_map<WordVec, std::pair<std::size_t,std::size_t>, NgramHash,NgramEquals> NgramMap;
  std::vector<NgramMap> ngramCounts_;


};


void Viterbi(const Graph& graph, const SparseVector& weights, float bleuWeight, WordVec* text);

};

#endif
