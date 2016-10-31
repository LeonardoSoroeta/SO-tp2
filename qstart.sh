#!/bin/bash

cd /Users/martin/Documents/TPSO/TP2

make all
if [ $? -ne 0 ]; then
	echo "make all failed"
	exit 2
fi

./run.sh
