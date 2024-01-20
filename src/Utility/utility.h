//
// Created by Lorenzo Ugoccioni on 14/01/2024.
//

#ifndef SDP_GRAPH_PARTITIONING_UTILITY_H
#define SDP_GRAPH_PARTITIONING_UTILITY_H

#include "../Individual/Individual.h"
//#include "GeneticAlgorithm/GA.h"
#include <iostream>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <sys/resource.h>

using namespace std;

struct UsageInfo {
    vector<int> partition;
    vector<int> executionTimes;
    vector<int> balanceIndexPartitions;
    float balanceIndex;
    float balanceFactor;
    float balanceIndexAverage;
    vector<vector<int>> cutSizeBetweenPartitions;
    float cutSize;
    float cutSizeAverage;
    int totalEdgesWeight;
    int totalNodesWeight;
    struct rusage usage;
    string fileName;
    double cpu_percentage;
};

float balanceFactor(int num_partitions, const vector<int>& partition, const Graph& G);
vector<int> calculatePartitionsWeight(int num_partitions, const vector<int>& partition, const Graph& G);
vector<vector<int>> calculateCutSizeBetweenPartitions(const Graph& G, const vector<int>& partition);
float avgCutSize(const vector<vector<int>>& cutSizes);
float avgBalanceFactor(const vector<int>& balanceFactors);


bool read_input(const std::string& filename, Graph& G, int& type_reading, int& num_t);
void saveInfoToFile(const UsageInfo& usageInfo);


void printPopulation(const vector<Individual>& population);
string time_conversion(int delta_t);
int time_now();
void compute_metis(string filename);



#endif //SDP_GRAPH_PARTITIONING_UTILITY_H
