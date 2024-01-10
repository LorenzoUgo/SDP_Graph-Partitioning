//
// Created by Lorenzo Ugoccioni on 08/01/2024.
//

#ifndef SDP_GRAPH_PARTITIONING_INDIVIDUAL_H
#define SDP_GRAPH_PARTITIONING_INDIVIDUAL_H

#include <vector>
#include <random>
#include <functional>
#include <chrono>
#include "../fitness/Fitness.h"
#include "../Graph/Graph.h"

using namespace std;

class Individual {

private:
    vector<int> genotype;
    int num_alleles;
    float fitness_value;

public:
    // constructor used for crossover computation
    Individual(vector<int> genotype = {}, int num_alleles = -1, float fitness = -1) : genotype(move(genotype)), fitness_value(fitness), num_alleles(num_alleles){}

    // constructor used for building population, randomizes genotype with given size and given max value for each gene
    Individual(int n_alleles, int genotypeSize, const Graph& G) : num_alleles(n_alleles) {
        genotype = {};
        std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> gene_value(0,num_alleles-1);
        auto value = std::bind(gene_value, generator);

        for (int i=0; i<genotypeSize; i++)
            genotype.emplace_back(value());

        fitness_value = fitness(num_alleles, genotype, G);
    }

    // NOTA perchè usare setgenotype in crossover se esiste il constructor?
    void setGenotype(vector<int> &gtype) { genotype = gtype; }
    void setFitnessValue(float fitness_val) {
        // NOTA avrebbe senso che fosse la fitness a chiamare dopo aver calcolato il valore. Fitness prende l'individuo e il grafo, fa i calcoli e poi setta il valore
        fitness_value = fitness_val;
    }

    const vector<int> &getGenotype() const { return genotype; }
    float getFitnessValue() const { return fitness_value; }
    int getNumAlleles() { return num_alleles; }

    int getGenotypeSize() const { return genotype.size(); }
    void mutation();

    //Copy Constructor
    Individual(const Individual& altro) {
        // Copia dei campi
        genotype = altro.genotype;
        fitness_value = altro.fitness_value;
        num_alleles = altro.num_alleles;
    }
    // operation override
    Individual& operator=(const Individual& altro) {
        if (this != &altro) {
            genotype = altro.genotype;
            fitness_value = altro.fitness_value;
            num_alleles = altro.num_alleles;
        }
        return *this;
    }

    bool operator==(const Individual& other) const {
        return genotype == other.genotype && fitness_value == other.fitness_value;
    }

    bool operator<(const Individual& other) const {
        return fitness_value < other.fitness_value;
    }
};

#endif //SDP_GRAPH_PARTITIONING_INDIVIDUAL_H
