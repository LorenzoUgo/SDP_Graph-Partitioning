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
#include <fstream>
#include <sys/resource.h>

using namespace std;

struct PartitionData {
    //vector<std::vector<bool>> partitions;
    vector<bool> partitions;
    vector<double> executionTimes;
    vector<double> partitionsWeights;
    int cutSize;
    int totalEdgesWeight;
    int totalNodesWeight;
    struct rusage usage;
    string fileName;
    double cpu_percentage;
};


void printPopulation(const vector<Individual>& population);
string time_conversion(int delta_t);
int time_now();
void metis_comparison(const string& file_metis, const Graph& G);

double calculateBalanceFactor(Graph& graph, const std::vector<bool>& partitionA);
double calculateBalanceFactorPartitions(Graph& G, const std::vector<std::vector<bool>>& partitions);
double calculateAverageBalanceFactor(const std::vector<double>& balanceFactors);
int calculateCutSize(Graph& graph, const std::vector<bool>& partitionA);
int calculateCutSizePartitions(Graph& G, const std::vector<std::vector<bool>>& partitions);
double calculateAverageCutSize(const std::vector<int>& cutSizes);
void read_input(const std::string& filename, Graph* G);
void savePartitionDataToFile(const PartitionData& partitionData);

#endif //SDP_GRAPH_PARTITIONING_UTILITY_H
