#!/bin/bash
ulimit -s unlimited
for((i=1;i<=5;i++))
do
for((j=1;j<=1;j++))
do
echo "round  $i $j"
#./Gen 1
./Main $j
done
done
echo finish
