cmake_minimum_required(VERSION 3.20)
project(SDP_Graph_Partitioning)

set(CMAKE_CXX_STANDARD 20)

add_executable(SDP_Graph_Partitioning main.cpp
        Graph/Graph.cpp Graph/Graph.h
        fitness/Fitness.cpp fitness/Fitness.h
        Individual/Individual.h Individual/Individual.cpp
        Crossover/Crossover.cpp Crossover/Crossover.h
        Era/Era.cpp Era/Era.h
        Randomizer/RandomGenerator.cpp Randomizer/RandomGenerator.h)  #GraphGenerator/graph_generator.cpp

