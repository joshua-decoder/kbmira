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

#include <string>

#include <boost/noncopyable.hpp>
#include <boost/scoped_array.hpp>
#include <boost/functional/hash/hash.hpp>
#include <boost/unordered_map.hpp>


#include "util/exception.hh"
#include "util/file_piece.hh"
#include "util/murmur_hash.hh"
#include "util/pool.hh"
#include "util/string_piece.hh"

#include "FeatureStats.h"

namespace MosesTuning {

typedef unsigned int WordIndex;
const WordIndex kMaxWordIndex = UINT_MAX;

template <class T> class FixedAllocator : boost::noncopyable {
  public:
    FixedAllocator() : current_(NULL), end_(NULL) {}

    void Init(std::size_t count) {
      assert(!current_);
      array_.reset(new T[count]);
      current_ = array_.get();
      end_ = current_ + count;
    }

    T &operator[](std::size_t idx) {
      return array_.get()[idx];
    }
    const T &operator[](std::size_t idx) const {
      return array_.get()[idx];
    }

    T *New() {
      T *ret = current_++;
      UTIL_THROW_IF(ret >= end_, util::Exception, "Allocating past end");
      return ret;
    }

    std::size_t Capacity() const {
      return end_ - array_.get();
    }

    std::size_t Size() const {
      return current_ - array_.get();
    }

  private:
    boost::scoped_array<T> array_;
    T *current_, *end_;
};


class Vocab {
  public:
    typedef std::pair<const char *const, WordIndex> Entry;

    const Entry &FindOrAdd(const StringPiece &str);

  private:
    util::Pool piece_backing_;

    struct Hash : public std::unary_function<const char *, std::size_t> {
      std::size_t operator()(StringPiece str) const {
        return util::MurmurHashNative(str.data(), str.size());
      }
    };

    struct Equals : public std::binary_function<const char *, const char *, bool> {
      bool operator()(StringPiece first, StringPiece second) const {
        return first == second;
      }
    };

    typedef boost::unordered_map<const char *, WordIndex, Hash, Equals> Map;
    Map map_;

};

class Vertex;

/**
 * An edge has 1 head vertex, 0..n child (tail) vertices, a list of words and a feature vector.
**/
class Edge {
  public:
    void AddWord(const Vocab::Entry *word) {
      words_.push_back(word);
    }

    void AddChild(const Vertex* child) {
      children_.push_back(child);
    }

    void AddFeature(const StringPiece& name, FeatureStatsType value) {
      //TODO StringPiece interface
      features_.set(name.as_string(),value);
    }

    typedef std::vector<const Vocab::Entry*> WordVec;

    const WordVec &Words() const {
      return words_;
    }
    
    const SparseVector& Features() const {
      return features_;
    }

    FeatureStatsType GetScore(const SparseVector& weights) {
      return inner_product(features_, weights);
    }

  private:
    // NULL for non-terminals.  
    std::vector<const Vocab::Entry*> words_;
    std::vector<const Vertex*> children_;
    SparseVector features_;
};

/*
 * A vertex has 0..n incoming edges
 **/
class Vertex {
  public:
    void AddEdge(const Edge* edge) {incoming_.push_back(edge);}

  private:
    std::vector<const Edge*> incoming_;
};


class Graph : boost::noncopyable {
  public:
    void SetCounts(std::size_t vertices, std::size_t edges) {
      vertices_.Init(vertices);
      edges_.Init(edges);
    }

    Vocab &MutableVocab() { return vocab_; }

    Edge *NewEdge() {      
      return edges_.New();
    }

    Vertex *NewVertex() {
      return vertices_.New();
    }

    Vertex &GetVertex(std::size_t index) {
      return vertices_[index];
    }

    Edge &GetEdge(std::size_t index) {
      return edges_[index];
    }

    std::size_t VertexSize() const { return vertices_.Size(); }

  private:
    FixedAllocator<Edge> edges_;    
    FixedAllocator<Vertex> vertices_;
    Vocab vocab_;
};

class FormatException : public util::Exception {
  public:
    FormatException() {}
    ~FormatException() throw() {}
};


void ReadGraph(util::FilePiece &from, Graph &graph);


};
