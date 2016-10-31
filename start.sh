#!/bin/bash

cd /Users/martin/Documents/TPSO/TP2

make clean
if [ $? -ne 0 ]; then
	echo "make clean failed"
	exit 1
fi

make all
if [ $? -ne 0 ]; then
	echo "make all failed"
	exit 2
fi

./run.sh
