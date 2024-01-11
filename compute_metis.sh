#!/bin/sh

if [ $# -gt 0 ]
then
	echo "Type input graph filename: "
	read filename
	echo "Type number of partitions to compute: "
	read numpart
else
filename = $1
numpart = $2

command = `gpmetis $1 $2 -nooutput` 