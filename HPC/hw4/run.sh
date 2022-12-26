#!/bin/bash

rm -f result2 log
cmake .
make

echo -e "2000\t10000\t50000" > result2

for i in 2 4 8 16 32 64
do
  echo -e "\n\n========== RUN WITH $i NODES ========== \n\n"
  mpirun -c $i hw4 2>> result2
done

