#!/bin/sh

nthread="2 4 6 8"
npart="2 4 6 8"

# AAAAAAA test del cazzo giusto vedere se funziona prima di bruciarti la cpu 
# parallel
#for nt in $nthread; do
#	for np in $npart; do
#		./graph_partitioner data/graph_750000_2250000/ --population 2 --new 1 --mig 1 --gen 1 --parallel --bal --isl "$nt" --part "$np"
#	done
#done

# sequential
#for np in $npart; do
#	./graph_partitioner data/graph_750000_2250000/ --bal --population 2 --new 1 --mig 1 --gen 1 --part "$np"
#done

# parallel
for nt in $nthread; do
	for np in $npart; do
		./graph_partitioner data/graph_750000_2250000/ --parallel --bal --isl "$nt" --part "$np"
	done
done

# sequential
for np in $npart; do
	./graph_partitioner data/graph_750000_2250000/ --bal --population 60 --new 30 --gen 1000 --part "$np"
done

mv data/graph_750000_2250000/gapart.balanced.parallel.2.* data/graph_750000_2250000/results/2part/
mv data/graph_750000_2250000/gapart.balanced.2.txt data/graph_750000_2250000/results/2part/

mv data/graph_750000_2250000/gapart.balanced.parallel.4.* data/graph_750000_2250000/results/4part/
mv data/graph_750000_2250000/gapart.balanced.4.txt data/graph_750000_2250000/results/4part/

mv data/graph_750000_2250000/gapart.balanced.parallel.6.* data/graph_750000_2250000/results/6part/
mv data/graph_750000_2250000/gapart.balanced.6.txt data/graph_750000_2250000/results/6part/

mv data/graph_750000_2250000/gapart.balanced.parallel.8.* data/graph_750000_2250000/results/8part/
mv data/graph_750000_2250000/gapart.balanced.8.txt data/graph_750000_2250000/results/8part/

echo "\nDone!\n"
