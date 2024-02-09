#!/bin/sh

if [ $# -eq 0 ]
then
	echo "Usage: $0 <data/graph_#nodes_#edges/>"
	return
else
	filename=$1
	numpart=$2
fi

gfolder=$1
nthread="2 4 6 8"
npart="2 4 6 8"

# parallel
for nt in $nthread; do
	for np in $npart; do
		./graph_partitioner "$gfolder" --parallel --bal --isl "$nt" --part "$np"
		# Test ./graph_partitioner "$gfolder" --parallel --bal --isl "$nt" --part "$np" --population 1 --new 1 --mig 1 --gen 1 --era 1
	done
done

# sequential
for np in $npart; do
	./graph_partitioner "$gfolder" --bal --population 60 --new 30 --gen 600 --part "$np"
	# Test./graph_partitioner "$gfolder" --bal --isl "$nt" --part "$np" --population 1 --new 1 --mig 1 --gen 1 --era 1
done

# create resulsts folder and move files
for np in $npart; do
	mkdir -p "$gfolder"results/"$np"part/
done
for np in $npart; do
	mv "$gfolder"gapart.balanced.parallel."$np".* "$gfolder"results/"$np"part/
	mv "$gfolder"gapart.balanced."$np".txt "$gfolder"results/"$np"part/
done

echo "\nDone!\n"
