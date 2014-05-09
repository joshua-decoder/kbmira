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

#include <cmath>

#include "BleuScorer.h"
#include "HopeFearDecoder.h"

using namespace std;

namespace MosesTuning {

static const ValType BLEU_RATIO = 5;

NbestHopeFearDecoder::NbestHopeFearDecoder(
      const vector<string>& featureFiles,
      const vector<string>&  scoreFiles,
      bool streaming,
      bool  no_shuffle,
      bool safe_hope
      ) : safe_hope_(safe_hope) {
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

void NbestHopeFearDecoder::HopeFear(
              const std::vector<ValType> backgroundBleu,
              const MiraWeightVector& wv,
              const MiraFeatureVector* modelFeatures,
              const MiraFeatureVector* hopeFeatures,
              const MiraFeatureVector* fearFeatures,
              vector<float>* modelBleuStats,
              ValType* hopeBleu,
              ValType* fearBleu
              ) {

  
  // Hope / fear decode
  ValType hope_scale = 1.0;
  size_t hope_index=0, fear_index=0, model_index=0;
  ValType hope_score=0, fear_score=0, model_score=0;
  for(size_t safe_loop=0; safe_loop<2; safe_loop++) {
    ValType hope_bleu, hope_model;
    for(size_t i=0; i< train_->cur_size(); i++) {
      const MiraFeatureVector& vec=train_->featuresAt(i);
      ValType score = wv.score(vec);
      ValType bleu = sentenceLevelBackgroundBleu(train_->scoresAt(i),backgroundBleu);
      // Hope
      if(i==0 || (hope_scale*score + bleu) > hope_score) {
        hope_score = hope_scale*score + bleu;
        hope_index = i;
        hope_bleu = bleu;
        hope_model = score;
      }
      // Fear
      if(i==0 || (score - bleu) > fear_score) {
        fear_score = score - bleu;
        fear_index = i;
      }
      // Model
      if(i==0 || score > model_score) {
        model_score = score;
        model_index = i;
      }
    }
    // Outer loop rescales the contribution of model score to 'hope' in antagonistic cases
    // where model score is having far more influence than BLEU
    hope_bleu *= BLEU_RATIO; // We only care about cases where model has MUCH more influence than BLEU
    if(safe_hope_ && safe_loop==0 && abs(hope_model)>1e-8 && abs(hope_bleu)/abs(hope_model)<hope_scale)
      hope_scale = abs(hope_bleu) / abs(hope_model);
    else break;
  }
  modelFeatures = &(train_->featuresAt(model_index));
  hopeFeatures = &(train_->featuresAt(hope_index));
  fearFeatures = &(train_->featuresAt(fear_index));

  const vector<float>& hope_stats = train_->scoresAt(hope_index);
  *hopeBleu = sentenceLevelBackgroundBleu(hope_stats, backgroundBleu);
  const vector<float>& fear_stats = train_->scoresAt(fear_index);
  *fearBleu = sentenceLevelBackgroundBleu(fear_stats, backgroundBleu);

  *modelBleuStats = train_->scoresAt(model_index);
}



};
