//
// Created by Lorenzo Ugoccioni on 11/01/2024.
//

#include "Era.h"

mutex printMutex;

Individual Galapagos_parallel(map<int, vector<Individual>>& populations, const Graph& G){

    vector<thread> Islands;

    barrier<> barrier_1_cpp(GA_parameters.NUM_ISLANDS + 1);
    barrier<> barrier_2_cpp(GA_parameters.NUM_ISLANDS + 1);


    for(int i = 0; i<GA_parameters.NUM_ISLANDS; i++) {
        cout << "Starting Isola n_" << i << endl;
        Islands.emplace_back( [=, &populations, &G, &barrier_1_cpp, &barrier_2_cpp] {Eras_parallel(i, populations.at(i), G, barrier_1_cpp, barrier_2_cpp);});

    }

    for(int e = 1; e<GA_parameters.NUM_ERAS; e++){


        barrier_1_cpp.arrive_and_wait();
        printMutex.lock();
        cout << "Migration phase now !! " << endl;
        printMutex.unlock();
        Migration_bestOnes(populations, GA_parameters.NUM_MIGRANTS);

        barrier_2_cpp.arrive_and_wait();


    }

    for(auto& t: Islands) {
        t.join();
    }


    return BestOfGalapagos(populations);
}

void Eras_parallel(int island_id, vector<Individual>& population, const Graph& G, barrier<>& b1, barrier<>& b2) {
    Individual parent1, parent2, offspring;
    srand(std::time(0));

    random_device rd;
    uniform_int_distribution uid(0, 1);
    default_random_engine dre(rd());

    printMutex.lock();
    cout << "ISLAND " << island_id << " STARTING" << endl;

    for(int e = 0; e<GA_parameters.NUM_ERAS; e++){

        for (int g = 0; g < GA_parameters.NUM_GENERATIONS; g++) {

            for (int i = 0; i < GA_parameters.NUM_OFFSPRING; i++) {

                if ((float) uid(dre) < GA_parameters.MUTATION_RATE) {

                    offspring = random_parent_selection(population);
                    offspring.mutation();
                    offspring.setFitnessValue(fitness(GA_parameters.NUM_PARTITIONS, offspring.getGenotype(), G));

                } else {

                    parent1 = parent_selection_tournament(rand() % (GA_parameters.POPULATION_SIZE / 5 - 1) + 1, population);
                    parent2 = parent_selection_tournament(rand() % (GA_parameters.POPULATION_SIZE / 5 - 1) + 1, population);
                    offspring = uniform_random_crossover(parent1, parent2);
                    offspring.setFitnessValue(fitness(GA_parameters.NUM_PARTITIONS, offspring.getGenotype(), G));

                }

                population.push_back(offspring);

            }

            sort(population.begin(), population.end(), ascending_order);

            for (int i = 0; i < GA_parameters.NUM_OFFSPRING; i++) {
                auto it = population.begin() + GA_parameters.POPULATION_SIZE;
                population.erase(it);
            }

        }


        b1.arrive_and_wait();
        printMutex.lock();
        cout << "ISLAND " << island_id << " WAITING FOR MIGRATION" << endl;
        printMutex.unlock();
        // WAIT ...
        b2.arrive_and_wait();



    }
}