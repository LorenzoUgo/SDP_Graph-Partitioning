//
// Created by Lorenzo Ugoccioni on 08/01/2024.
//

#ifndef FITNESS_H
#define FITNESS_H

#include "../Graph/Graph.h"
#include <vector>
#include <algorithm>
using namespace std;


// calcola il fitness value come combinazione lineare di dimensione taglio e bilanciamento peso partizioni
float fitness(int num_partitions, const vector<int>& genotype, const Graph& G, float cut_size_weight = 0.5, float balance_index_weight = 0.5);

#endif //FITNESS_H