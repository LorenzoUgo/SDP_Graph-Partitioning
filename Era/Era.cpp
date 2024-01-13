//
// Created by Lorenzo Ugoccioni on 06/01/2024.
//

#include "Era.h"

bool descending_order(const Individual& I1, const Individual& I2){
    return I1.getFitnessValue() > I2.getFitnessValue();
}
bool ascending_order(const Individual& I1, const Individual& I2){
    return I1.getFitnessValue() < I2.getFitnessValue();
}


/**     Evoluzione Isola     */
void Eras(vector<Individual>& population, const Graph& G, int num_generations, int num_offspring, int population_size, int num_partitions) {
    float mutation_rate = .25;
    Individual parent1, parent2, offspring;
    srand(std::time(0));

    random_device rd;
    uniform_int_distribution uid(0, 1);
    default_random_engine dre(rd());

    for (int g = 0; g < num_generations; g++) {
        //cout << "Starting Generation n_" << g << endl;

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
            //int d = (int) distance(genotype.begin(), it);
            population.erase(it);
        }
        /*population.resize(population_size)*/
    }
}


/** Gestione Isole */
Individual Galapagos_fixed(map<int, vector<Individual>>& populations, const Graph& G, int num_eras, int num_generations, int num_offspring, int population_size, int num_partitions, int num_migrants){

    for(int e = 0; e<num_eras; e++){
        cout << "Starting Era n_" << e << endl;

        for(int i = 0; i<populations.size(); i++) {
            cout << "Starting Isola n_" << i << endl;

            Eras(populations.at(i), G, num_generations, num_offspring, population_size, num_partitions);
        }
        cout << "Migration phase now !! " << endl;

        Migration_bestOnes(populations, num_migrants);
    }

    return BestOfGalapagos(populations);
}

/** Gestione Isole */
Individual Galapagos(map<int, vector<Individual>>& populations, const Graph& G, int eras_no_upgrade, int num_generations, int num_offspring, int population_size, int num_partitions, int num_migrants){

   Individual bestOfGalapagos = BestOfGalapagos(populations);
    float lerning_rate = 0.03;
    int e = 1;
    const int era_waited_for_improvement = eras_no_upgrade;
    while(eras_no_upgrade){
        cout << "Starting Era n_" << e << " - - - - - ";
        cout << "Current Champ: " << bestOfGalapagos.getFitnessValue() << endl;

        for(int i = 0; i<populations.size(); i++) {
            //cout << "Starting Isola n_" << i << endl;
            Eras(populations.at(i), G, num_generations, num_offspring, population_size, num_partitions);
        }
        cout << "Migration phase now !! " << endl;

        Migration_randomOnes(populations, num_migrants);
        e++;
        if(check_early_end(bestOfGalapagos, populations, lerning_rate, eras_no_upgrade)){
            eras_no_upgrade --;
        }else{
            bestOfGalapagos = BestOfGalapagos(populations);
            eras_no_upgrade = era_waited_for_improvement;
        }
    }

    return BestOfGalapagos(populations);
}

void Migration_bestOnes(map<int, vector<Individual>>& galapagosPopulation, int migrants){
    vector<Individual> bestOf;
    srand(std::time(0));
    Individual I;
    int index = 0;

    random_device rd;
    uniform_int_distribution uid(0, 1);
    default_random_engine dre(rd());


    for(auto & i : galapagosPopulation){
        for(int j = 0; j<migrants; j++){

            bestOf.emplace_back(i.second.front());
            i.second.erase(i.second.begin());

        }
    }

    for(auto & i : galapagosPopulation){
        for(int j = 0; j<migrants; j++){

            (bestOf.size()-1) ? index = rand() % (bestOf.size() - 1) : index = 0;
            i.second.emplace_back(bestOf[index]);
            bestOf.erase(bestOf.begin() + index);

        }
    }
}

void Migration_randomOnes(map<int, vector<Individual>>& galapagosPopulation, int migrants){
    vector<Individual> bestOf;
    srand(std::time(0));
    Individual I;
    int index = 0;

    random_device rd;
    uniform_int_distribution uid(0, 1);
    default_random_engine dre(rd());


    for(auto & i : galapagosPopulation){
        for(int j = 0; j<migrants; j++){

            index = rand() % (i.second.size() - 1);
            bestOf.emplace_back(i.second[index]);
            i.second.erase(i.second.begin()+index);

        }
    }

    for(auto & i : galapagosPopulation){
        for(int j = 0; j<migrants; j++){

            (bestOf.size()-1) ? index = rand() % (bestOf.size() - 1) : index = 0;
            i.second.emplace_back(bestOf[index]);
            bestOf.erase(bestOf.begin() + index);

        }
    }
}

Individual BestOfGalapagos(map<int, vector<Individual>>& galapagosPopulation){
    vector<Individual> v;

    for(auto vI : galapagosPopulation){
        v.emplace_back(vI.second[0]);
    }
    sort(v.begin(), v.end(), ascending_order);

    return v[0];
}

vector<Individual> BestOfIslands(map<int, vector<Individual>>& galapagosPopulation){
    vector<Individual> v;

    for(auto vI : galapagosPopulation){
        v.emplace_back(vI.second[0]);
    }

    return v;
}


bool check_early_end(const Individual& champ, map<int, vector<Individual>>& populations, float& learning_rate, int& eras_no_upgrade){
    bool no_upgrade = true;

    for(auto & population : populations){
        float percentage_gap = (champ.getFitnessValue() - population.second.front().getFitnessValue())  / champ.getFitnessValue();
        if ( percentage_gap > learning_rate ){
            no_upgrade = false;
            break;
        }
    }

    if(no_upgrade){
        return true;
    }else{
        return false;
    }

}
