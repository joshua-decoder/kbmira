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
#ifndef MERT_HOPEFEARDECODER_H
#define MERT_HOPEFEARDECODER_H

#include <vector>

#include <boost/scoped_ptr.hpp>

#include "HypPackEnumerator.h"
#include "MiraFeatureVector.h"
#include "MiraWeightVector.h"

//
// Used by batch mira to get the hope, fear and model hypothesis. This wraps
// the n-best list and lattice/hypergraph implementations
//

namespace MosesTuning {

//Abstract base class
class HopeFearDecoder {
public:
  //iterator methods
  virtual void reset() = 0;
  virtual void next() = 0;
  virtual bool finished() = 0;

  /**
    * Accepts equal length vectors of model and bleu weights, returns the hypothoses maximimising
    * modelScore*modelWeights[i] + bleu*bleuWeights[i] for each i
    **/
  virtual void HopeFear(
              const std::vector<ValType> backgroundBleu,
              const MiraWeightVector& wv,
              MiraFeatureVector* modelFeatures,
              MiraFeatureVector* hopeFeatures,
              MiraFeatureVector* fearFeatures,
              std::vector<float>* modelBleuStats,
              std::vector<float>* hopeBleuStats,
              ValType* hopeBleu,
              ValType* fearBleu,
              bool* hopeFearEqual
              ) = 0;


};


class NbestHopeFearDecoder : public virtual HopeFearDecoder {
public:
  NbestHopeFearDecoder(const std::vector<std::string>& featureFiles,
                         const std::vector<std::string>&  scoreFiles,
                         bool streaming,
                         bool  no_shuffle,
                         bool safe_hope
                         );

  virtual void reset();
  virtual void next();
  virtual bool finished();

  virtual void HopeFear(
              const std::vector<ValType> backgroundBleu,
              const MiraWeightVector& wv,
              MiraFeatureVector* modelFeatures,
              MiraFeatureVector* hopeFeatures,
              MiraFeatureVector* fearFeatures,
              std::vector<float>* modelBleuStats,
              std::vector<float>* hopeBleuStats,
              ValType* hopeBleu,
              ValType* fearBleu,
              bool* hopeFearEqual
              );

private:
  boost::scoped_ptr<HypPackEnumerator> train_;
  bool safe_hope_;

};

};

#endif

