//
// Created by Lorenzo Ugoccioni on 14/01/2024.
//

#ifndef SDP_GRAPH_PARTITIONING_UTILITY_H
#define SDP_GRAPH_PARTITIONING_UTILITY_H

#include "../Individual/Individual.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <sys/resource.h>

using namespace std;

struct UsageInfo {
    stringstream parameters;
    vector<int> partition;
    vector<int> executionTimes;
    vector<int> balanceIndexPartitions;
    float balanceIndex;
    float balanceFactor;
    float balanceIndexAverage;
    float cutSize;
    float cutSizeAverage;
    int totalEdgesWeight;
    int totalNodesWeight;
    struct rusage usage;
    string fileName;
    double cpu_percentage;
    float fitness;
};

float balanceFactor(int num_partitions, const vector<int>& partition, const Graph& G);
vector<int> calculatePartitionsWeight(int num_partitions, const vector<int>& partition, const Graph& G);
float avgBalanceFactor(const vector<int>& balanceFactors);

bool read_input(const std::string& filename, Graph& G, int type_reading, int num_t);
void saveInfoToFile(const UsageInfo& usageInfo);

string time_conversion(int delta_t);
int time_now();
void compute_metis(string filename);

#endif //SDP_GRAPH_PARTITIONING_UTILITY_H
