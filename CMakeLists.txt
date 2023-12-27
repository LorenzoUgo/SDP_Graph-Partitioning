cmake_minimum_required(VERSION 3.20)
project(SDP_Graph_Partitioning)

set(CMAKE_CXX_STANDARD 14)

add_executable(SDP_Graph_Partitioning Graph/Graph.cpp Graph/Graph.h GraphGenerator/graph_generator.cpp)
