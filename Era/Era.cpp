//
// Created by Lorenzo Ugoccioni on 06/01/2024.
//

#include "Era.h"

bool descending_order(Individual I1, Individual I2){
    return I1.getFitnessValue() > I2.getFitnessValue();
}
bool ascending_order(Individual I1, Individual I2){
    return I1.getFitnessValue() < I2.getFitnessValue();
}

vector<Individual> Era(vector<Individual> population, const Graph& G, int num_generations, int num_offspring, int population_size, int num_partitions){
    float mutation_rate = .25;
    Individual parent1, parent2, offspring;
    srand(std::time(0));

    random_device rd;
    uniform_int_distribution uid(0, 1);
    default_random_engine dre(rd());

    for(int j = 0; j<num_generations; j++) {

        for (int i = 0; i < num_offspring; i++) {

            if ((float) uid(dre) < mutation_rate) {

                offspring = random_parent_selection(population);
                offspring.mutation();
                offspring.setFitnessValue(fitness(num_partitions, offspring.getGenotype(), G));

            }else{

                parent1 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                parent2 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                offspring = unform_random_crossover(parent1, parent2);
                offspring.setFitnessValue(fitness(2, offspring.getGenotype(), G));

            }

            population.push_back(offspring);

        }

        sort(population.begin(), population.end(), ascending_order);

        population.resize(population_size);
    }

    return population;
}