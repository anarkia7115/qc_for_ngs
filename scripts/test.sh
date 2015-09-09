#!/bin/bash

for i in {0..37}
do
	./qc $i | wc -l
	#./qc $i | grep key
done
