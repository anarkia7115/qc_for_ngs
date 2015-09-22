#!/bin/bash

#f1="../data/WGC_20m_n1.fq.gz"
#f2="../data/WGC_20m_n2.fq.gz"
f1="../data/sample100t1.gz"
f2="../data/sample100t2.gz"
localOutput="../data/output/decompressed_files"

rm $localOutput/*

../bin/CopyAndDistribute \
  --gz1 $f1 --gz2 $f2 \
  --output $localOutput \
  --row 1000 

#./CopyAndDistribute \
#  --gz1 $f1 --gz2 $f2 \
#  --output $localOutput \
#  --row 1000000 
#


echo "./qc \
  --gz1 $f1 --gz2 $f2 \
  --output $localOutput \
  --row 1000000" 
