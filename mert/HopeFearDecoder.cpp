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

#include "HopeFearDecoder.h"

using namespace std;

namespace MosesTuning {

NbestHopeFearDecoder::NbestHopeFearDecoder(
      const vector<string>& featureFiles,
      const vector<string>&  scoreFiles,
      bool streaming,
      bool  no_shuffle)  {
  if (streaming) {
    train_.reset(new StreamingHypPackEnumerator(featureFiles, scoreFiles));
  } else {
    train_.reset(new RandomAccessHypPackEnumerator(featureFiles, scoreFiles, no_shuffle));
  }
  train_->reset();
}


void NbestHopeFearDecoder::next() {
  train_->next();
}

bool NbestHopeFearDecoder::finished() {
  return train_->finished();
}

void NbestHopeFearDecoder::HopeFear(const std::vector<ValType> modelWeights,
              const std::vector<ValType> bleuWeights,
              const std::vector<ValType> backgroundBleu,
              std::vector<MiraFeatureVector>* featureVectors,
              std::vector<ValType>* bleus) {

}



};
