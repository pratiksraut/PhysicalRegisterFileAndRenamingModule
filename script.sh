#!/bin/bash

make clean
make
rm -rf *.log

#astar 1	real 	speculative
./721sim --disambig=0,1,0 --perf=0,0,0,0 --fq=64 --cp=32 --al=256 --lsq=128 --iq=64 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c473.astar_test.76.0.22.gz pk
diff -iw stats*.log output/astar1.txt > output/test1.txt
rm -rf *.log

#astar 2	real 	oracle
./721sim --disambig=1,0,0 --perf=0,0,0,0 --fq=64 --cp=32 --al=256 --lsq=128 --iq=64 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c473.astar_test.76.0.22.gz pk
diff -iw stats*.log output/astar2.txt > output/test2.txt
rm -rf *.log

#astar 3	perfect 	speculative	 	 
./721sim --disambig=0,1,0 --perf=1,0,0,0 --fq=64 --cp=32 --al=256 --lsq=128 --iq=64 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c473.astar_test.76.0.22.gz pk
diff -iw stats*.log output/astar3.txt > output/test3.txt
rm -rf *.log

#astar 4	perfect 	oracle	 	 
./721sim --disambig=1,0,0 --perf=1,0,0,0 --fq=64 --cp=32 --al=256 --lsq=128 --iq=64 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c473.astar_test.76.0.22.gz pk
diff -iw stats*.log output/astar4.txt > output/test4.txt
rm -rf *.log

#aster 5 real 	speculative	 smaller structures, 8 chkpts
./721sim --disambig=0,1,0 --perf=0,0,0,0 --fq=64 --cp=8 --al=128 --lsq=64 --iq=32 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c473.astar_test.76.0.22.gz pk
diff -iw stats*.log output/astar5.txt > output/test5.txt
rm -rf *.log

#astar 6	perfect 	oracle	 smaller structures, 4 chkpts
./721sim --disambig=1,0,0 --perf=1,0,0,0 --fq=64 --cp=4 --al=128 --lsq=64 --iq=32 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c473.astar_test.76.0.22.gz pk
diff -iw stats*.log output/astar6.txt > output/test6.txt
rm -rf *.log

#astar 7 real 	speculative	 smaller structures,4 chkpts
./721sim --disambig=0,1,0 --perf=0,0,0,0 --fq=64 --cp=4 --al=128 --lsq=64 --iq=32 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c473.astar_test.76.0.22.gz pk
diff -iw stats*.log output/astar7.txt > output/test7.txt
rm -rf *.log

#hmmer 1	real 	speculative	 	 
./721sim --disambig=0,1,0 --perf=0,0,0,0 --fq=64 --cp=32 --al=256 --lsq=128 --iq=64 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c456.hmmer_test.74.0.22.gz pk
diff -iw stats*.log output/hmmer1.txt > output/test8.txt
rm -rf *.log

#hmmer 2	real 	oracle	 	 
./721sim --disambig=1,0,0 --perf=0,0,0,0 --fq=64 --cp=32 --al=256 --lsq=128 --iq=64 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c456.hmmer_test.74.0.22.gz pk
diff -iw stats*.log output/hmmer2.txt > output/test9.txt
rm -rf *.log 

#hmmer 3	perfect speculative	 	 
./721sim --disambig=0,1,0 --perf=1,0,0,0 --fq=64 --cp=32 --al=256 --lsq=128 --iq=64 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c456.hmmer_test.74.0.22.gz pk
diff -iw stats*.log output/hmmer3.txt > output/test10.txt
rm -rf *.log 

#hmmer 4	perfect 	oracle	 	 
./721sim --disambig=1,0,0 --perf=1,0,0,0 --fq=64 --cp=32 --al=256 --lsq=128 --iq=64 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c456.hmmer_test.74.0.22.gz pk
diff -iw stats*.log output/hmmer4.txt > output/test11.txt
rm -rf *.log 


#hmmer	5 perfect 	oracle	 perfect fetch
./721sim --disambig=1,0,0 --perf=1,0,0,1 --fq=64 --cp=32 --al=256 --lsq=128 --iq=64 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c456.hmmer_test.74.0.22.gz pk
diff -iw stats*.log output/hmmer5.txt > output/test12.txt
rm -rf *.log 

#hmmer 6	real 	speculative	 smaller structures, 8 chkpts
./721sim --disambig=0,1,0 --perf=0,0,0,0 --fq=64 --cp=8 --al=128 --lsq=64 --iq=32 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c456.hmmer_test.74.0.22.gz pk
diff -iw stats*.log output/hmmer6.txt > output/test13.txt
rm -rf *.log

#hmmer 7	perfect 	oracle	 4 chkpts
./721sim --disambig=1,0,0 --perf=1,0,0,0 --fq=64 --cp=4 --al=256 --lsq=128 --iq=64 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c456.hmmer_test.74.0.22.gz pk
diff -iw stats*.log output/hmmer7.txt > output/test14.txt
rm -rf *.log

#hmmer 8	perfect 	oracle	 perfect fetch, 4 chkpts
./721sim --disambig=1,0,0 --perf=1,0,0,1 --fq=64 --cp=4 --al=256 --lsq=128 --iq=64 --iqnp=4 --fw=4 --dw=4 --iw=8 --rw=4 -m2048 -e100000000 -c456.hmmer_test.74.0.22.gz pk
diff -iw stats*.log output/hmmer8.txt > output/test15.txt
rm -rf *.log

