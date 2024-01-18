//
// Created by Lorenzo Ugoccioni on 06/01/2024.
//

#ifndef ERA_H
#define ERA_H

#include "../Crossover/Crossover.h"
#include "../Individual/Individual.h"
#include "../Fitness/Fitness.h"
#include <iostream>
#include <set>
#include <random>
#include <algorithm>
#include <thread>
#include <barrier>
#include <mutex>

extern struct ga_parameters{
    int NUM_PARTITIONS = 3;
    int NUM_OFFSPRING = 25;
    int NUM_GENERATIONS = 50;
    int POPULATION_SIZE = 100;
    int NUM_ISLANDS = 6;
    int NUM_ERAS = 200;
    int NUM_MIGRANTS = 20;
    float MUTATION_RATE = 0.4;
} GA_parameters;

using namespace std;

bool descending_order(const Individual& I1, const Individual& I2);

bool ascending_order(const Individual& I1, const Individual& I2);

void Eras(vector<Individual>& population, const Graph& G, int num_generations, int num_offspring, int population_size, int num_partitions);

Individual Galapagos_fixed(map<int, vector<Individual>>& populations, const Graph& G, int num_eras, int num_generations, int num_offspring, int population_size, int num_partitions, int num_migrants=5);

Individual Galapagos(map<int, vector<Individual>>& populations, const Graph& G, int eras_no_upgrade, int num_generations, int num_offspring, int population_size, int num_partitions, int num_migrants);

Individual Galapagos_parallel(map<int, vector<Individual>>& populations, const Graph& G);

void Eras_parallel(int island_id, vector<Individual>& population, const Graph& G, barrier<>& b1, barrier<>& b2);

void Migration_bestOnes(map<int, vector<Individual>>& galapagosPopulation, int migrants);

void Migration_randomOnes(map<int, vector<Individual>>& galapagosPopulation, int migrants);

Individual BestOfGalapagos(map<int, vector<Individual>>& galapagosPopulation);

vector<Individual> BestOfIslands(map<int, vector<Individual>>& galapagosPopulation);

bool check_early_end(const Individual& champ, map<int, vector<Individual>>& populations, float& learning_rate, int& eras_no_upgrade);

#endif //ERA_H