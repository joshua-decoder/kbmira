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

#include <limits>
#include <list>

#include <boost/unordered_set.hpp>

#include "util/file_piece.hh"
#include "util/tokenize_piece.hh"

#include "ForestRescore.h"

using namespace std;

namespace MosesTuning {

static const size_t kNgramOrder = 4;

std::ostream& operator<<(std::ostream& out, const WordVec& wordVec) {
  out << "[";
  for (size_t i = 0; i < wordVec.size(); ++i) {
    out << wordVec[i]->first;
    if (i+1< wordVec.size()) out << " ";
  }
  out << "]";
  return out;
}

void ReferenceSet::Load(vector<string>& files, Vocab& vocab) {
  for (size_t i = 0; i < files.size(); ++i) {
    util::FilePiece fh(files[i].c_str());
    size_t sentenceId = 0;
    while(true) {
      StringPiece line;
      try {
        line = fh.ReadLine();
      } catch (util::EndOfFileException &e) {
        break;
      }
      //cerr << line << endl;
      boost::unordered_multiset<WordVec, NgramHash, NgramEquals> ngramCounts;
      list<WordVec> openNgrams;
      //tokenize & count
      for (util::TokenIter<util::SingleCharacter, true> j(line, util::SingleCharacter(' ')); j; ++j) {
        const Vocab::Entry* nextTok = &(vocab.FindOrAdd(*j));
        openNgrams.push_front(WordVec());
        for (list<WordVec>::iterator k = openNgrams.begin(); k != openNgrams.end();  ++k) {
          k->push_back(nextTok);
          ngramCounts.insert(*k);
        }
        if (openNgrams.size() >=  kNgramOrder) openNgrams.pop_back();
      }

      //merge into overall ngram map
      for (boost::unordered_multiset<WordVec>::const_iterator ni = ngramCounts.begin();
        ni != ngramCounts.end(); ++ni) {
        size_t count = ngramCounts.count(*ni);
        //cerr << *ni << " " << count <<  endl;
        if (ngramCounts_.size() <= sentenceId) ngramCounts_.resize(sentenceId+1);
        NgramMap::iterator totalsIter = ngramCounts_[sentenceId].find(*ni);
        if (totalsIter == ngramCounts_[sentenceId].end()) {
          ngramCounts_[sentenceId][*ni] = pair<size_t,size_t>(count,count);
        } else {
          ngramCounts_[sentenceId][*ni].first = max(count, ngramCounts_[sentenceId][*ni].first); //clip
          ngramCounts_[sentenceId][*ni].second += count; //no clip
        }
      }
      //cerr << endl;
      ++sentenceId;
    }
  }

}
  
size_t ReferenceSet::NgramMatches(size_t sentenceId, const WordVec& ngram, bool clip) const  {
  return 0;
}

static const FeatureStatsType kMinScore = -numeric_limits<FeatureStatsType>::max();
typedef pair<const Edge*,FeatureStatsType> BackPointer;


/**
 * Recurse through back pointers
 **/
static void GetBestTranslate(size_t vertexId, const Graph& graph, const vector<BackPointer>& bps, WordVec* text) {
  if (!bps[vertexId].first) return;
  const Edge* prevEdge = bps[vertexId].first;
  size_t childId = 0;
  for (size_t i = 0; i < prevEdge->Words().size(); ++i) {
    if (prevEdge->Words()[i] != NULL) {
      text->push_back(prevEdge->Words()[i]);
    } else {
      size_t childVertexId = prevEdge->Children()[childId++];
      WordVec childText;
      GetBestTranslate(childVertexId,graph,bps,&childText);
      text->insert(text->end(), childText.begin(), childText.end());
    }
  }
}

void Viterbi(const Graph& graph, const SparseVector& weights, float , WordVec* text) 
{
  BackPointer init(NULL,kMinScore);
  vector<BackPointer> backPointers(graph.VertexSize(),init);
  for (size_t vi = 0; vi < graph.VertexSize(); ++vi) {
    const Vertex& vertex = graph.GetVertex(vi);
    const vector<const Edge*>& incoming = vertex.GetIncoming();
    if (!incoming.size()) {
      backPointers[vi].second = 0;  
    } else {
      for (size_t ei = 0; ei < incoming.size(); ++ei) {
        FeatureStatsType incomingScore = incoming[ei]->GetScore(weights);
        for (size_t i = 0; i < incoming[ei]->Children().size(); ++i) {
          size_t childId = incoming[ei]->Children()[i];
          UTIL_THROW_IF(backPointers[childId].second == kMinScore,
            FormatException, "Graph was not topologically sorted. curr=" << vi << " prev=" << childId);
          incomingScore += backPointers[childId].second;
        }
        if (incomingScore >= backPointers[vi].second) {
          backPointers[vi].first = incoming[ei];
          backPointers[vi].second = incomingScore;
        }
      }
    }
  }

  //expand back pointers
  GetBestTranslate(graph.VertexSize()-1, graph, backPointers, text);
}


};
