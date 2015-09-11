#!/bin/bash

f1="../data/WGC_100t_n1.gz"
f2="../data/WGC_100t_n2.gz"
localOutput="../data/output"

./qc \
  --gz1 $f1 --gz2 $f2 \
  --output $localOutput \
  --row 1000000 


echo "./qc \
  --gz1 $f1 --gz2 $f2 \
  --output $localOutput \
  --row 1000000" 
