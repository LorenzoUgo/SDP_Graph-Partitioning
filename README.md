# Genetic Algorithm applied to Graph Partitioning
Developed a GA for **Graph Partitioning**. 

Project for *System And Device Programming* course @ PoliTO

## Development environment

We tested our program with WSL (Windows Subsystem for Linux) with an Intel Core i7-8700K CPU @ 3.70GHz.
The RAM used was 8 GB and we were allowed to run 12 threads in parallel.


## About The Project

The goal of this project was to develop two versions (sequential and parallel) of a _p-way partitioning algorithm_.
This means that our algorithm's goal is to split the initial graph G into _p_ non-overlapping subgraphs while satisfying two specific properties:
1. the sum of the weights of the nodes in each subgraph is balanced 
2. the sum of the weights of the edges crossing between subgraphs is minimized.

In order to do so, we decided to exploit a Genetic Algorithm, using the islands paradigm in its parallel version.

## Getting Started

The commands to run the entire project are in `Makefile` file.

### _How to compile ?_
These are fundamental commands to compile our program:

1. Delete previous object files and eventual data folder
```sh
make clean
make clean_data
```
2. Compile code and extract data

```sh
make
```

###_How to run ?_
These are fundamental commands to run our program:

1. `OPTIONAL` It creates a new graph to be partitioned
```sh
./graph_generator #nodes #edges
```

2. Run program ([details below](./README.md#Usage)) with a set of command line arguments, using

```sh
./graph_partitioner <data/graph_#nodes_#edges/> --binary [valore] --bal --mod --parallel --part [valore] --population [valore] --gen [valore] --era [valore] --new [valore] --isl [valore] --mig [valore] --lr [valore] 
``` 

Extra. Run metis application to obtain a reference value

```sh
sh compute_metis.sh
```

### Autorun
In order to run the same tests we did, use:

```sh
sh autorun.sh <data/graph_#nodes_#edges>
```

## Benchmark Input directory
- [graph with 250 k nodes & 750 k edges]( ./data/graph_250000_750000 )
- [graph with 500 k nodes & 1,5 M edges]( ./data/graph_500000_1500000 )
- [graph with 750 k nodes & 2,25 M edges]( ./data/graph_750000_2250000 )
- [graph with 1 M nodes & 3 M edges]( ./data/graph_1000000_3000000 )


> NOTE
> 
> If you want to see the contents of a graph, it is recommended to use files with the extension .txt
> 
> The file with the extension .bin is used to parallel read the graph 

## Algorithm name

- GA: Genetic Algorithm sequential (one thread for one island);
- GA: Genetic Algorithm parallel (one thread dedicated to each island);

> The GA is provided with DEFAULT SETTINGS, tuned by us in the train phase. They are used if you run the algorithm without
> specifying any optional parameters.
> 
> Please refer to the output file if you are interested to see their values.

## Usage

In order to use our program correctly it's important to understand what to pass to run it. The only 
**mandatory** argument is the : _Input graph folder name_

All the following argument can be used by the user to change the default setting ( they're **Optional** ).

| Argument | Usage |
| ----------- | ------ |
| --binary | if activated defines how many threads used to read the file in input. |
| --bal | if activated defines to make a balanced partition.  |
| --mod | if activated defines that the algorithm stops if there has been no significant improvement for some era.  |
| --parallel | if enabled defines parallel computation.  |
| --part | if enabled defines how many partitions to partition the graph into.  |
| --population | if activated, needs a value defining the population size.  |
| --gen | if activated, needs a value defining the number of generations.  |
| --era | `-mod NOT Active` if activated, needs a value to defines the number of eras.  |
| --era | `-mod Active` if activated, needs a value defining the number of consecutive eras to wait without improvement.  |
| --new | if activated, needs a value defining the number of offsprings.  |
| --isl | if activated, needs a value defining the number of islands.  |
| --mig | if activated, needs a value defining the number of migrants.  |
| --lr | `-mod Active` if activated, needs a value defining the learning rate.  |

```sh
 ./graph_partitioner  ./data/graph_1000000_2500000 --bal --parallel --era 50 --isl 10 # Parallel execution to compute balanced partition, using 50 era and 10 island (other parameters stay  default)

./graph_partitioner  ./data/graph_1000000_2500000 --mod --era 7 --new 15 --lr 20 # Sequential execution (default), generating 15 new offspring, that stop if there's not at least one improvement of 20% for 7 consecutive eras

./graph_partitioner  ./data/graph_1000000_2500000 # All parameters are kept by default (Sequential computation, not balanced partitions) 

./graph_generator 100000 250000 # Create a graph file with 100000 nodes and 250000 edges
```

## Folders' Content

There are 5 main folders.

### GeneticAlgorithm

It contains a ***Genetic Algorithm*** class, which contains all the functions and parameters needed:

- GA.cpp
- GA.h

### Utility

It contains all utility functions.

- utility.cpp
- utility.h

### Graph

It contains a ***Graph*** class and related functions:

- Graph.cpp
- Graph.h


### Individual

It contains a ***Individual*** class and related functions to encode it:

- Individual.cpp
- Individual.h