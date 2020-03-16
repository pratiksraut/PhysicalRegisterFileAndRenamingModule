#!/bin/bash

make clean
make
rm -rf *.log

//astar 	real 	speculative
./721sim --disambig=0,1,0 --perf=0,0,0,0 --fq=64 --cp=32 --al=256 --lsq=128 --iq=64 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c473.astar_test.76.0.22.gz pk
diff -iw stats*.log astar1.txt > test1.txt
rm -rf *.log