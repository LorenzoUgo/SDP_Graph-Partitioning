//
// Created by Lorenzo Ugoccioni on 14/01/2024.
//

#ifndef SDP_GRAPH_PARTITIONING_GA_H
#define SDP_GRAPH_PARTITIONING_GA_H

#include <iostream>
#include <set>
#include <random>
#include <algorithm>
#include <thread>
#include <barrier>
#include <mutex>
#include <getopt.h>
#include <sstream>

#include "../Individual/Individual.h"

using namespace std;

class GeneticAlgorithm {

private:
    int NUM_PARTITIONS;
    int POPULATION_SIZE;
    int NUM_GENERATIONS;
    int NUM_OFFSPRING;
    int NUM_ISLANDS;
    int NUM_ERAS;
    int NUM_MIGRANTS;
    int ERAS_NO_UPGRADE;
    float LEARNING_RATE;
    bool parallel;
    bool dynamic;
    bool balanced;

    map<int, vector<Individual>> Population;
    Individual bestOf;

public:
    /*      Defines default values for sequential version     */
    explicit GeneticAlgorithm(
        int numPartitions=2, 
        int populationSize=20, 
        int numGenerations=80, 
        int numOffspring=12, 
        int numIslands=1,
        int numEras=1, 
        int numMigrants=7, 
        int erasNoUpgrade=5, 
        float learningRate=0.001, 
        bool Parallel=false, 
        bool Dynamic=false, 
        bool Balanced=false) 
        : NUM_PARTITIONS(numPartitions), POPULATION_SIZE(populationSize), NUM_GENERATIONS(numGenerations), 
            NUM_OFFSPRING(numOffspring), NUM_ISLANDS(numIslands), NUM_ERAS(numEras), NUM_MIGRANTS(numMigrants), 
            ERAS_NO_UPGRADE(erasNoUpgrade), LEARNING_RATE(learningRate), parallel(Parallel), dynamic(Dynamic), 
            balanced(Balanced) {};

    /*      Defines default values for parallel version     */
    void setParallel(bool Parallel) {
        parallel = Parallel;
        NUM_ERAS = 15;
        if (NUM_ISLANDS == 1) { // num islands not yet set -> set default parallel values
            NUM_ISLANDS = 4;
            NUM_GENERATIONS = NUM_GENERATIONS / NUM_ISLANDS;
        }
    } 
    void setNumPartitions(int numPartitions) {NUM_PARTITIONS = numPartitions;}
    void setNumOffspring(int numOffspring) {NUM_OFFSPRING = numOffspring;}
    void setNumGenerations(int numGenerations) {NUM_GENERATIONS = numGenerations;}
    void setPopulationSize(int populationSize) {POPULATION_SIZE = populationSize;}
    void setNumIslands(int numIslands) {
        if (parallel && (NUM_ISLANDS == 4)) { // set parallel has been called before
            NUM_ISLANDS = numIslands;
            NUM_GENERATIONS = NUM_GENERATIONS*4 / NUM_ISLANDS;
        }
    }
    void setNumEras(int numEras) {NUM_ERAS = numEras;}
    void setNumMigrants(int numMigrants) {NUM_MIGRANTS = numMigrants;}
    void setDynamic(bool Dynamic) {dynamic = Dynamic;}
    void setPopulation(const map<int, vector<Individual>> &population) {Population = population;}

    void setErasNoUpgrade(int erasNoUpgrade) {ERAS_NO_UPGRADE = erasNoUpgrade;}
    void setLearningRate(float learningRate) {LEARNING_RATE = learningRate;}
    void setBalanced(bool Balanced) {balanced = Balanced;}



    int getNumPartitions() const {return NUM_PARTITIONS;}
    int getNumOffspring() const {return NUM_OFFSPRING;}
    int getNumGenerations() const {return NUM_GENERATIONS;}
    int getPopulationSize() const {return POPULATION_SIZE;}
    int getNumIslands() const {return NUM_ISLANDS;}
    int getNumEras() const {return NUM_ERAS;}
    int getNumMigrants() const {return NUM_MIGRANTS;}
    int getErasNoUpgrade() const {return ERAS_NO_UPGRADE;}
    float getLearningRate() const {return LEARNING_RATE;}
    bool isParallel() const {return parallel;}
    bool isDynamic() const {return dynamic;}
    bool isBalanced() const {return balanced;}

    const map<int, vector<Individual>> &getPopulation() const {return Population;}
    Individual getBestOf() const {return bestOf;}

    stringstream printParameters();

    void run(const Graph& G);


    /** misc functions */
    bool check_early_end(const Individual& champ);
    Individual BestIndividual();
    Individual BestOfGalapagos();
    vector<Individual> IslandsBests();

    /** Sequential implementation */
    void Eras(vector<Individual>& population, const Graph& G);
    void Galapagos( const Graph& G );
    void Galapagos_LR( const Graph& G );

    /** Parallel implementation */
    void Galapagos_parallel(  const Graph& G );
    void Galapagos_parallel_LR( const Graph& G );
    void Eras_parallel(int island_id, vector<Individual>& population, const Graph& G, barrier<>& b1, barrier<> &b2);
    void Eras_parallel_LR(int island_id, vector<Individual>& population, const Graph& G, barrier<>& b1, barrier<>& b2, bool &earlyend);

    /** Crossover function */
    Individual one_cut_crossover(Individual I1, Individual I2);
    Individual two_cut_crossover(Individual I1, Individual I2);
    Individual n_cut_crossover(Individual I1, Individual I2);
    Individual uniform_crossover(Individual I1, Individual I2);
    Individual uniform_random_crossover(Individual I1, Individual I2);

    /** Parent selection function */
    pair<Individual, Individual> parents_selection_tournament(int num_partecipants, vector<Individual> population);
    pair<Individual, Individual> random_parents_selection(vector<Individual> population);
    Individual parent_selection_tournament(int num_partecipants, const vector<Individual>& population);
    Individual random_parent_selection(const vector<Individual>& population);

    /** Migration function */
    void Migration_randomOnes();
    void Migration_bestOnes();

};

#endif //SDP_GRAPH_PARTITIONING_GA_H
