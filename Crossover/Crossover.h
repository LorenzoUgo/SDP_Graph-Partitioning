//
// Created by Lorenzo Ugoccioni on 06/01/2024.
//

#ifndef SDP_GRAPH_PARTITIONING_CROSSOVER_H
#define SDP_GRAPH_PARTITIONING_CROSSOVER_H

#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <set>
#include <random>

#include "../Individual/Individual.h"
#include "../fitness/Fitness.h"
#include "../Era/Era.h"

using namespace std;

Individual one_cut_crossover(Individual I1, Individual I2);
Individual two_cut_crossover(Individual I1, Individual I2);
Individual n_cut_crossover(Individual I1, Individual I2);
Individual uniform_crossover(Individual I1, Individual I2);
Individual unform_random_crossover(Individual I1, Individual I2);
vector<int> crossover_VETT(vector<int> v1, vector<int> v2);

Individual parent_selection_tournament(int num_partecipants, const vector<Individual>& population);
Individual random_parent_selection(const vector<Individual>& population);







#endif //SDP_GRAPH_PARTITIONING_CROSSOVER_H
