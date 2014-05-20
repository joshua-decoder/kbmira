#!/bin/sh

#./kbmira  --dense-init test_data/init.opt  --ffile test_data/features.dat --scfile test_data/scores.dat -o mert.out --verbose --no-shuffle
./kbmira  --dense-init test_data/dense  --ffile test_data/features_10.dat --scfile test_data/scores_10.dat -o mert.out --verbose --no-shuffle --iters 10
