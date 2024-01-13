cmake_minimum_required(VERSION 3.20)
project(SDP_Graph_Partitioning)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -pthread")

add_executable(SDP_Graph_Partitioning main_test_parallelization.cpp )

# main_test_readgraph.cpp
# main.cpp Graph/Graph.cpp Graph/Graph.h fitness/Fitness.cpp fitness/Fitness.h Individual/Individual.h Individual/Individual.cpp Crossover/Crossover.cpp Crossover/Crossover.h Era/Era.cpp Era/Era.h Randomizer/RandomGenerator.cpp Randomizer/RandomGenerator.h
# Era/Era_Parallel.cpp
# GraphGenerator/graph_generator.cpp
