cmake_minimum_required(VERSION 3.20)
project(SDP_Graph_Partitioning)

set(CMAKE_CXX_STANDARD 20)

add_executable(SDP_Graph_Partitioning main.cpp Graph/Graph.cpp Graph/Graph.h GraphGenerator/graph_generator.cpp fitness/fitness.cpp)  #GraphGenerator/graph_generator.cpp
