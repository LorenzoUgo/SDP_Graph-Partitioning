//
// Created by Lorenzo Ugoccioni on 11/01/2024.
//

#include "Era.h"

//pthread_barrier_t barrier_1, barrier_2;

Individual Galapagos_parallel(map<int, vector<Individual>>& populations, const Graph& G, int num_eras, int num_generations, int num_offspring, int population_size, int num_partitions, int num_migrants){

    vector<thread> Islands;

    /*pthread_barrier_init(&barrier_1, nullptr, populations.size() + 1);
    pthread_barrier_init(&barrier_2, nullptr, populations.size() + 1);*/

    barrier barrier_1_cpp(populations.size() + 1);
    barrier barrier_2_cpp(populations.size() + 1);


    for(int i = 0; i<populations.size(); i++) {
        cout << "Starting Isola n_" << i << endl;
        Islands.emplace_back( Eras_parallel, populations.at(i), G, barrier_1_cpp, barrier_2_cpp, num_eras, num_generations, num_offspring, population_size, num_partitions );

    }

    for(int e = 1; e<num_eras; e++){
        cout << "Starting Era n_" << e << endl;

        //pthread_barrier_wait(&barrier_1);
        barrier_1_cpp.arrive_and_wait();

        cout << "Migration phase now !! " << endl;
        Migration_bestOnes(populations, num_migrants);

        barrier_2_cpp.arrive_and_wait();
        //pthread_barrier_wait(&barrier_2);

    }

    for(auto& t: Islands) {
        t.join();
    }

    /*pthread_barrier_destroy(&barrier_1);
    pthread_barrier_destroy(&barrier_2);*/

    return BestOfGalapagos(populations);
}

void Eras_parallel(vector<Individual>& population, const Graph& G, barrier<>& b1, barrier<>& b2, int num_eras, int num_generations, int num_offspring, int population_size, int num_partitions) {
    float mutation_rate = .25;
    Individual parent1, parent2, offspring;
    srand(std::time(0));

    random_device rd;
    uniform_int_distribution uid(0, 1);
    default_random_engine dre(rd());

    for(int e = 0; e<num_eras; e++){

        for (int g = 0; g < num_generations; g++) {
            cout << "Starting Generation n_" << g << endl;

            for (int i = 0; i < num_offspring; i++) {

                if ((float) uid(dre) < mutation_rate) {

                    offspring = random_parent_selection(population);
                    offspring.mutation();
                    offspring.setFitnessValue(fitness(num_partitions, offspring.getGenotype(), G));

                } else {

                    parent1 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                    parent2 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                    offspring = uniform_random_crossover(parent1, parent2);
                    offspring.setFitnessValue(fitness(num_partitions, offspring.getGenotype(), G));

                }

                population.push_back(offspring);

            }

            sort(population.begin(), population.end(), ascending_order);

            for (int i = 0; i < num_offspring; i++) {
                auto it = population.begin() + population_size;
                population.erase(it);
            }

            /*population.resize(population_size)*/
        }

        /*pthread_barrier_wait(&b1);
        // WAIT ...
        pthread_barrier_wait(&b2);*/

        b2.arrive_and_wait();
        // WAIT ...
        b1.arrive_and_wait();

    }
}