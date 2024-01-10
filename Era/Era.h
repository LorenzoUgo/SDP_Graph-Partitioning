//
// Created by Lorenzo Ugoccioni on 06/01/2024.
//

#ifndef SDP_GRAPH_PARTITIONING_GENERATION_H
#define SDP_GRAPH_PARTITIONING_GENERATION_H

#include "../Crossover/Crossover.h"
#include "../Individual/Individual.h"
#include "../fitness/Fitness.h"
#include "../Randomizer/RandomGenerator.h"
#include <set>
#include <random>
#include <algorithm>

using namespace std;

bool descending_order(const Individual& I1, const Individual& I2);

bool ascending_order(const Individual& I1, const Individual& I2);

vector<Individual> Era(vector<Individual> population, const Graph& G, int num_generations, int num_offspring, int population_size, int num_partitions);

#endif //SDP_GRAPH_PARTITIONING_GENERATION_H