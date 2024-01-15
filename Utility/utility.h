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
#include <getopt.h>

using namespace std;

bool descending_order(const Individual& I1, const Individual& I2);

bool ascending_order(const Individual& I1, const Individual& I2);

float cut_size(vector<int> genotype, const Graph& G);

float balance_index(int num_partitions, const vector<int>& genotype, const Graph& G);

float fitness(int num_partitions, const vector<int>& genotype, const Graph& G, float cut_size_weight=0.5 , float balance_index_weight=0.5);

void printVett(const vector<int>& vett);

void printIndividual(const Individual& ind);

void printPopulation(const vector<Individual>& population);

void time_conversion(int delta_t);

int readGraph(const string& file, Graph& G);

int time_now();

void metis_comparison(string file_metis, const Graph& G);

void set_GA_parameters(GeneticAlgorithm GA, int argc, char* argv[]);


#endif //SDP_GRAPH_PARTITIONING_UTILITY_H
