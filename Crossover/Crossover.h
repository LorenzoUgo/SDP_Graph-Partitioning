//
// Created by Lorenzo Ugoccioni on 06/01/2024.
//

#ifndef SDP_GRAPH_PARTITIONING_CROSSOVER_H
#define SDP_GRAPH_PARTITIONING_CROSSOVER_H

#include <vector>


using namespace std;

class Individual{

private:
    vector<int> Genotype;
    float fitness_value;

public:
    Individual(vector<int> genotype = {}, float fitness = NULL) : Genotype(move(genotype)), fitness_value(fitness){}

    const vector<int> &getGenotype() const { return Genotype; }
    float getFitnessValue() const { return fitness_value; }
    void setGenotype(const vector<int> &genotype) { Genotype = genotype; }
    void setFitnessValue() { /** Computed starting from its genotype !*/; }

    int getGenotypeSize() { return Genotype.size(); }

};

Individual one_cut_crossover(Individual I1, Individual I2);
Individual two_cut_crossover(Individual I1, Individual I2);
Individual n_cut_crossover(Individual I1, Individual I2);
Individual uniform_crossover(Individual I1, Individual I2);
Individual unform_random_crossover(Individual I1, Individual I2);
vector<int> crossover_VETT(vector<int> v1, vector<int> v2);







#endif //SDP_GRAPH_PARTITIONING_CROSSOVER_H
