//
// Created by Lorenzo Ugoccioni on 06/01/2024.
//

#ifndef SDP_GRAPH_PARTITIONING_GENERATION_H
#define SDP_GRAPH_PARTITIONING_GENERATION_H

#include "../Crossover/Crossover.h"
#include "../Individual/Individual.h"
#include "../fitness/Fitness.h"
#include <set>
#include <random>
#include <algorithm>

bool descending_order(Individual I1, Individual I2);

vector<Individual> Era(vector<Individual> population, Graph G, int num_generations, int num_offspring, int population_size);
#endif //SDP_GRAPH_PARTITIONING_GENERATION_H
