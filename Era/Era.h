//
// Created by Lorenzo Ugoccioni on 06/01/2024.
//

#ifndef SDP_GRAPH_PARTITIONING_GENERATION_H
#define SDP_GRAPH_PARTITIONING_GENERATION_H

#include "../Crossover/Crossover.h"
#include "../Individual/Individual.h"
#include "../fitness/Fitness.h"
#include "../Randomizer/RandomGenerator.h"
#include <iostream>
#include <set>
#include <random>
#include <algorithm>
#include <thread>
#include <barrier>
#include <pthread.h>

using namespace std;

bool descending_order(const Individual& I1, const Individual& I2);

bool ascending_order(const Individual& I1, const Individual& I2);

void Eras(vector<Individual>& population, const Graph& G, int num_generations, int num_offspring, int population_size, int num_partitions);

Individual Galapagos_fixed(map<int, vector<Individual>>& populations, const Graph& G, int num_eras, int num_generations, int num_offspring, int population_size, int num_partitions, int num_migrants=5);

Individual Galapagos(map<int, vector<Individual>>& populations, const Graph& G, int eras_no_upgrade, int num_generations, int num_offspring, int population_size, int num_partitions, int num_migrants);

Individual Galapagos_parallel(map<int, vector<Individual>>& populations, const Graph& G, int num_eras, int num_generations, int num_offspring, int population_size, int num_partitions, int num_migrants);

void Eras_parallel(vector<Individual>& population, const Graph& G, barrier<>& b1, barrier<>& b2, int num_eras, int num_generations, int num_offspring, int population_size, int num_partitions);

void Migration_bestOnes(map<int, vector<Individual>>& galapagosPopulation, int migrants);

void Migration_randomOnes(map<int, vector<Individual>>& galapagosPopulation, int migrants);

Individual BestOfGalapagos(map<int, vector<Individual>>& galapagosPopulation);

vector<Individual> BestOfIslands(map<int, vector<Individual>>& galapagosPopulation);

bool check_early_end(const Individual& champ, map<int, vector<Individual>>& populations, float& learning_rate, int& eras_no_upgrade);

#endif //SDP_GRAPH_PARTITIONING_GENERATION_H