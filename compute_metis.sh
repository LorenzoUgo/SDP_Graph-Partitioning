#!/bin/sh

echo "This script uses METIS to compute best k-way partition of a graph and calculates fitness value according to the function defined in our project" 
echo "Make sure to have *gpmetis* installed if you want to use this script"
echo "You should be able to install it using your package manager"
echo ""

if [ $# -eq 0 ]
then
	echo "Usage: $0 <data/graph_#nodes_#edges/metis_text.txt> <#partitions>"
	return
else
	filename=$1
	numpart=$2
fi

cmd="gpmetis ./$filename $numpart"
$cmd

# output filename becomes filename.part.numpart
metisfile=$filename.part.$numpart
cmd="./graph_partitioner --compare $metisfile"
$cmd

cmd="rm $metisfile"
$cmd