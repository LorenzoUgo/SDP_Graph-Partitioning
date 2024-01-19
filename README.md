# SDP_Graph-Partitioning
**Graph Partitioning** project for *System And Device Programming* @ PoliTO

## About The Project --> To be modified.
In computer science, it is common to work with large graphs that need to be divided into non-overlapping sections, known as partitions. This process is called p-way partitioning. Our aim is to minimize the sum of the weights of edges that cross between partitions and balance the sum of weights of nodes in each partition.

## Getting Started

The commands to run the entire project are in `Makefile` file.

### Installation

There are three fundamental commands to use properly our program:

1. Delete previous object files
```sh
make clean
```
[*OPTIONAL*] Delete data folder

    make clean_data

2. Compile code and extract data

```sh
make
```

3. [*OPTIONAL*] Create graph file

```sh
./graph_generator #nodes #edges
```

This command will create a folder at path *data/* with name *graph_#nodes_#edges/*. 
Inside of this folder, you'll find 3 files:

- 2 versions (text and binary) in standard format, which is the one used in our algorithm;
- 1 text version compatible with *METIS* program, in order to compute some reference values. 

The weights of nodes and edges are set as random numbers between 1 and 10.

4. Run program (details below)

```sh
./graph_partitioner data/graph_#nodes_#edges
```

#### Benchmark Input directory

- [graph with 1 M nodes & 2,5 M edges]( ./data/graph_1000000_2500000 )
- [graph with 3 M nodes & 7,5 M edges]( ./data/graph_3000000_7500000 )
- [graph with 5 M nodes & 12,5 M edges]( ./data/graph_5000000_12500000 )
- [graph with 10 M nodes & 25 M edges]( ./data/graph_10000000_25000000 )

> NOTE
> 
> If you want to see the contents of a graph, it is recommended to use files with the extension .txt
> 
> The file with the extension .bin is used to parallel read the graph 

#### Algorithm name

- GA: Genetic Algorithm sequential (one or more islands executed in sequence);
- GA: Genetic Algorithm parallel (one thread dedicated for each island);

> The GA is provided with DEFAULT SETTINGS, tuned by us in the train phase:
> 
> BALANCED partitions = false; PARALLEL computation = false; dynamic = false;
> 
> number of PARTIITONS = ...; populations size = ...; number of generations = ...;
> number of OFFSPRING = ...; number of ISLANDS = ...; number of ERAS = ...;
> number of MIGRANTS = ...; number of consecutive ERAS WITHOUT IMPROVEMENT = ...; Learning Rate = ...;

## Usage

In order to use our program correctly it's important to understand what to pass to run it. The only 
**mandatory** argument is the : _Input graph folder name_

All the following argument can be used by the user to change the default setting ( they're **Optional** ).

| Argument | Usage |
| ----------- | ------ |
| -bal | if activated defines to make a balanced partition.  |
| -mod | if activated defines that the algorithm stops if there has been no significant improvement for some era.  |
| -parallel | if enabled defines parallel computation.  |
| -part | if enabled defines how many partitions to partition the graph into.  |
| -population | if activated, needs a value defining the population size.  |
| -gen | if activated, needs a value defining the number of generations.  |
| -era | `-mod NOT Active` if activated, needs a value to defines the number of eras.  |
| -era | `-mod Active` if activated, needs a value defining the number of consecutive eras to wait without improvement.  |
| -new | if activated, needs a value defining the number of offsprings.  |
| -isl | if activated, needs a value defining the number of islands.  |
| -mig | if activated, needs a value defining the number of migrants.  |
| -lr | `-mod Active` if activated, needs a value defining the learning rate.  |

```sh
 ./graph_partitioner  ./data/graph_1000000_2500000 -bal -parallel -era 50 -isl 10 # Parallel execution to compute balanced partition, using 50 era and 10 island (other parameters stay  default)

./graph_partitioner  ./data/graph_1000000_2500000 -mod -era 7 -new 15 -lr 20 # Sequential execution (default), generating 15 new offspring, that stop if there's not at least one improvement of 20% for 7 consecutive eras

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


## Files' Content   --> ???

#### MLRSB.cpp

It contains the main function `MLRSB` that implements the MLRSB algorithm. It takes as input the graph and it returns the best partitioning found. It also contains the function `pMLRSB` that implements the pMLRSB algorithm. It takes as input the graph and the number of partitions and it returns the best partitioning found.

#### Parallel_MLRSB.cpp

It contains the main function `Parallel_MLRSB` that implements the Parallel MLRSB algorithm. It takes as input the graph and the number of threads and it returns the best partitioning found. It also contains the function `Parallel_pMLRSB` that implements the Parallel pMLRSB algorithm. It takes as input the graph, the number of partitions and the number of threads and it returns the best partitioning found.

#### RSB.cpp

It contains the main function `RSB` that implements the RSB algorithm. It takes as input the graph and it returns the best partitioning found. It also contains the function `pRSB` that implements the pRSB algorithm. It takes as input the graph and the number of partitions and it returns the best partitioning found.

#### Parallel_RSB.cpp

It contains the main function `Parallel_RSB` that implements the Parallel RSB algorithm. It takes as input the graph and the number of threads and it returns the best partitioning found. It also contains the function `Parallel_pRSB` that implements the Parallel pRSB algorithm. It takes as input the graph, the number of partitions and the number of threads and it returns the best partitioning found.

#### utils.cpp

It contains a set of utility functions. The most important are: `read_graph` that reads the graph from a text file, `uncoarsening` that performs the uncoarsening phase of the algorithm and other functions to calculate the quality of the partitioning.

#### Graph.cpp

It contains the class `Graph` that represents the graph. It contins functions to calculate the adjacency matrix, degree matrix and other utility functions to add/remove nodes and edges. Finally, it contains some functions to help with the coarsening phase.