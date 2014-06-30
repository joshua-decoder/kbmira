#!/bin/sh

./kbmira  --dense-init test_data/dense  --hgdir test_data/hg_10 -o mert.out --no-shuffle --type hypergraph --reference test_data/reference.dev --iters 50
