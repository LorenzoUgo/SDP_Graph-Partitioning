//
// Created by Lorenzo Ugoccioni on 08/01/2024.
//

#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <iostream>
#include <vector>
#include <random>
#include <functional>
#include <chrono>
#include <algorithm>
#include "../Graph/Graph.h"
#include <thread>
#include <future>

using namespace std;

class Individual {

private:
    vector<int> genotype;
    int num_alleles;
    float fitness_value;

public:
    // constructor used for crossover computation
    Individual(vector<int> genotype = {}, int num_alleles = -1, float fitness = -1) : genotype(move(genotype)), num_alleles(num_alleles),fitness_value(fitness){}

    // constructor used for building population, randomizes genotype with given size and given max value for each gene
    Individual(int n_alleles, int genotypeSize, const Graph& G, const bool& balance) : num_alleles(n_alleles) {
        genotype = {};
        std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> gene_value(0,num_alleles-1);
        auto value = std::bind(gene_value, generator);

        for (int i=0; i<genotypeSize; i++)
            genotype.emplace_back(value());

        setFitnessParallel(G, balance);
    }

    // NOTA perchè usare setgenotype in crossover se esiste il constructor?
    // void setGenotype(vector<int> &gtype) { genotype = gtype; }
    void setFitness(const Graph& G, const bool& balance, float cut_size_weight=0.5 , float balance_index_weight=0.5);
    void setFitnessParallel(const Graph& G, const bool& balance, float cut_size_weight=0.5 , float balance_index_weight=0.5);
    const vector<int> &getGenotype() const { return genotype; }
    float getFitness() const { return fitness_value; }
    int getNumAlleles() { return num_alleles; }
    int getGenotypeSize() const { return genotype.size(); }
    
    void mutation();
    void printIndividual();

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

float get_mutation_fraction();

float cut_size(vector<int> genotype, const Graph& G);
void cut_size_parallel(vector<int>& genotype, const Graph& G, float& cut_size);
void cut_evaluator(int index, vector<int> genotype, vector<float> values, vector<Edge> edges);
void balance_evaluator(int index, vector<int> genotype, const Graph& G, vector<vector<float>> partitions_weight_values);

float balance_index(int num_partitions, const vector<int>& genotype, const Graph& G);
void balance_index_parallel(int num_partitions, const vector<int>& genotype, const Graph& G, float& balance_index);

#endif //INDIVIDUAL_H
