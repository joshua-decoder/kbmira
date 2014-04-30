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

#include <boost/lexical_cast.hpp>

#include "util/double-conversion/double-conversion.h"
#include "util/string_piece.hh"
#include "util/tokenize_piece.hh"

#include "Hypergraph.h"

using namespace std;

namespace MosesTuning {

StringPiece NextLine(util::FilePiece& from) {
  StringPiece line;
  while ((line = from.ReadLine()).starts_with("#"));
  return line;
}

const Vocab::Entry &Vocab::FindOrAdd(const StringPiece &str) {
#if BOOST_VERSION >= 104200
  Map::const_iterator i= map_.find(str, Hash(), Equals());
#else
  std::string copied_str(str.data(), str.size());
  Map::const_iterator i = map_.find(copied_str.c_str());
#endif
  if (i != map_.end()) return *i;
  char *copied = static_cast<char*>(piece_backing_.Allocate(str.size() + 1));
  memcpy(copied, str.data(), str.size());
  copied[str.size()] = 0;
  return *map_.insert(Entry(copied, map_.size())).first;
}

double_conversion::StringToDoubleConverter converter(double_conversion::StringToDoubleConverter::NO_FLAGS, NAN, NAN, "inf", "nan");


/**
 * Reads an incoming edge.
**/
Edge* ReadEdge(util::FilePiece &from, Graph &graph) {
  Edge* edge = graph.NewEdge();
  StringPiece line = NextLine(from);
  util::TokenIter<util::MultiCharacter> pipes(line, util::MultiCharacter(" ||| "));
  for (util::TokenIter<util::SingleCharacter, false> i(*pipes, util::SingleCharacter(' ')); i; ++i) {
    StringPiece got = *i;
    if ('[' == *got.data() && ']' == got.data()[got.size() - 1]) {
      // non-terminal
      char *end_ptr;
      unsigned long int child = std::strtoul(got.data() + 1, &end_ptr, 10);
      UTIL_THROW_IF(end_ptr != got.data() + got.size() - 1, FormatException, "Bad non-terminal" << got);
      UTIL_THROW_IF(child >= graph.VertexSize(), FormatException, "Reference to vertex " << child << " but we only have " << graph.VertexSize() << " vertices.  Is the file in bottom-up format?");
      edge->AddWord(NULL);
      edge->AddChild(&graph.GetVertex(child));
    } else {
      const Vocab::Entry &found = graph.MutableVocab().FindOrAdd(got);
      edge->AddWord(&found);
    }
  }
 
  ++pipes;
  for (util::TokenIter<util::SingleCharacter, false> i(*pipes, util::SingleCharacter(' ')); i; ++i) {
    StringPiece fv = *i;
    if (!fv.size()) break;
    size_t equals = fv.find_last_of("=");
    UTIL_THROW_IF(equals == fv.npos, FormatException, "Failed to parse feature '" << fv << "'");
    StringPiece name = fv.substr(0,equals);
    StringPiece value = fv.substr(equals+1);
    int processed;
    float score = converter.StringToFloat(value.data(), value.length(), &processed);
    UTIL_THROW_IF(isnan(score), FormatException, "Failed to parse weight '" << value << "'");
    edge->AddFeature(name,score);
  }
  return edge; 
}


/**
  * Read from "Kenneth's hypergraph" aka cdec target_graph format (with comments)
**/
void ReadGraph(util::FilePiece &from, Graph &graph) {

  //First line should contain field names
  StringPiece line = from.ReadLine();
  UTIL_THROW_IF(line.compare("# target ||| features") != 0, FormatException, "Incorrect format spec on first line: '" << line << "'");
  line = NextLine(from);
  
  //Then expect numbers of vertices
  util::TokenIter<util::SingleCharacter, false> i(line, util::SingleCharacter(' '));
  unsigned long int vertices = boost::lexical_cast<unsigned long int>(*i);
  ++i;
  unsigned long int edges = boost::lexical_cast<unsigned long int>(*i);
  graph.SetCounts(vertices, edges);
  cerr << "vertices: " << vertices << "; edges: " << edges << endl;
  for (size_t i = 0; i < vertices; ++i) {
    line = NextLine(from);
    unsigned long int edge_count = boost::lexical_cast<unsigned long int>(line);
    Vertex* vertex = graph.NewVertex();
    for (unsigned long int e = 0; e < edge_count; ++e) {
      vertex->AddEdge(ReadEdge(from, graph));
    }
  }
}

};
