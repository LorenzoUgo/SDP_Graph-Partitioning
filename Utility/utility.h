//
// Created by Lorenzo Ugoccioni on 14/01/2024.
//

#ifndef SDP_GRAPH_PARTITIONING_UTILITY_H
#define SDP_GRAPH_PARTITIONING_UTILITY_H

#include "../Individual/Individual.h"
#include "../GeneticAlgorithm/GA.h"
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace std;

bool descending_order(const Individual& I1, const Individual& I2);

bool ascending_order(const Individual& I1, const Individual& I2);

void printPopulation(const vector<Individual>& population);

void time_conversion(int delta_t);

int readGraph(const string& file, Graph& G);

int time_now();

void metis_comparison(string file_metis, const Graph& G);


#endif //SDP_GRAPH_PARTITIONING_UTILITY_H
