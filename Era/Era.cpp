//
// Created by Lorenzo Ugoccioni on 06/01/2024.
//

#include "Era.h"

bool descending_order(Individual I1, Individual I2){
    return I1.getFitnessValue() > I2.getFitnessValue();
}

vector<Individual> Era(vector<Individual> population, Graph G, int num_generations, int num_offspring, int population_size){
    float mutation_rate = .25;
    Individual parent1, parent2, offspring;

    /**
      * Implementazione di numeri random mega figa con libreria <random> !!
      * Potrebbe però essere necessaria una classe apposita
      * per non ripetere la creazione della random machine e della distribuzione ogni volta...
      * */
    random_device rd;
    uniform_int_distribution uid(0, 1 );
    default_random_engine dre(rd());

    for(int j = 0; j<num_generations; j++) {

        for (int i = 0; i < num_offspring; i++) {

            if ((float) uid(dre) < mutation_rate) {

                parent1 = random_parent_selection(population);
                offspring.mutation();

            }else{

                parent1 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                parent2 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                offspring = unform_random_crossover(parent1, parent2);
                offspring.setFitnessValue(fitness(3, offspring.getGenotype(), G));     //DA settare parametro globale, il num_partition

            }

            population.push_back(offspring);
        }

        sort(population.begin(), population.end(), descending_order);
        population(population.begin(), population.begin() + population_size);
    }

    return population;
}