#!/bin/bash

localInput="../data"
gz1fn="WGC_20m_n1.fq.gz"
gz2fn="WGC_20m_n2.fq.gz"
localOutput="../data/output"

./qc \
  --gz1 $localInput/$gz1fn --gz2 $localInput/$gz2fn \
  --output $localOutput \
  --row 1000000 

