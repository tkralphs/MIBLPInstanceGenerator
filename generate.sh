
#!/bin/bash

currentdir=''.''
#datadir=''${currentdir}/data/MCDM/NO_CORR/GEN_FROM_60/19_ITEMS_1000_GEN''
#resultdir=''${currentdir}/results/MCDM/NO_CORR/GEN_FROM_60/STRONG_BRANCH''
outdir=''${currentdir}/data2

# argument variables
i=$1 # first instance
j=$2 # last instance
k=$3 # number of cols
l=$4 # number of rows
m=$5 # upbound of coeffs
n=$6 # sign of coeffs
o=$7 # sign of rhs
p=$8 # dominancy strength
q=$9 # type of problem
r=$10 # number of lower cols

echo $i
echo $j
echo $k
echo $l
echo $m
echo $n
echo $o
echo $p
echo $q

while [[ $i -le $j ]]
  do
  
  ./milpGen $k $l $m $n $o $p $q $r $i

  mv milp_${k}_${l}_${m}_${n}${o}${p}${q}_${i}.mps $outdir
  mv milp_${k}_${l}_${m}_${n}${o}${p}${q}_${i}.sol $outdir
  mv milp_${k}_${l}_${m}_${n}${o}${p}${q}_${i}.aux $outdir

  let i=$i+1
done

