//
// Created by Lorenzo Ugoccioni on 14/01/2024.
//

#include "GA.h"


/** Start Algorithm */

void GeneticAlgorithm::run(const Graph& G){
    for (int is = 0; is<NUM_ISLANDS; is++) {    /** Si potrà parallelizzare anche la creazione delle popolazioni iniziali delle varie isole? */
        cout << "Starting Population for Island n_" << is << endl;
        for (int in = 0; in < POPULATION_SIZE; in++) {
            Population[is].emplace_back(Individual(NUM_PARTITIONS, G.num_of_nodes(), G));
        }
    }

    if( parallel && dynamic ) {
        Galapagos_parallel(Population, G);
    }if( parallel && not dynamic ){
        Galapagos_parallel_fixed(Population, G);
    }else if( not parallel && dynamic ){
        Galapagos(Population, G);
    }else if( not parallel && not dynamic ){
        Galapagos_fixed(Population, G);
    }else{
        cout <<"Our algorithm can't continue.\n Check if all the parameters are set correctly !!"<< endl;
    }

    bestPartition = BestOfGalapagos(Population);

}


/** Sequential implementation */

void GeneticAlgorithm::Eras(vector<Individual>& population, const Graph& G) {
    float mutation_rate = .25;
    Individual parent1, parent2, offspring;
    srand(std::time(0));

    random_device rd;
    uniform_int_distribution uid(0, 1);
    default_random_engine dre(rd());

    for (int g = 0; g < NUM_GENERATIONS; g++) {
        //cout << "Starting Generation n_" << g << endl;

        for (int i = 0; i < NUM_OFFSPRING; i++) {

            if ((float) uid(dre) < mutation_rate) {

                offspring = random_parent_selection(population);
                offspring.mutation();
                offspring.setFitnessValue(fitness(NUM_PARTITIONS, offspring.getGenotype(), G));

            } else {
                //DAI UN OCCHIO !!!!!
                parent1 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                parent2 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                offspring = uniform_random_crossover(parent1, parent2);
                offspring.setFitnessValue(fitness(NUM_PARTITIONS, offspring.getGenotype(), G));

            }

            population.push_back(offspring);

        }

        sort(population.begin(), population.end(), ascending_order);

        for (int i = 0; i < NUM_OFFSPRING; i++) {
            auto it = population.begin() + POPULATION_SIZE;
            //int d = (int) distance(genotype.begin(), it);
            population.erase(it);
        }
        /*population.resize(population_size)*/
    }
}

Individual GeneticAlgorithm::Galapagos_fixed(map<int, vector<Individual>>& populations, const Graph& G){

    for(int e = 0; e<NUM_ERAS; e++){
        cout << "Starting Era n_" << e << endl;

        for(int i = 0; i<NUM_ISLANDS; i++) {
            cout << "Starting Isola n_" << i << endl;

            Eras(populations.at(i), G);
        }
        cout << "Migration phase now !! " << endl;

        Migration_bestOnes(populations);
    }

    return BestOfGalapagos(populations);
}

Individual GeneticAlgorithm::Galapagos(map<int, vector<Individual>>& populations, const Graph& G){
    Individual bestOfGalapagos = BestOfGalapagos(populations);
    float lerning_rate = 0.03;
    int e = 1;
    const int era_waited_for_improvement = ERAS_NO_UPGRADE;
    while(ERAS_NO_UPGRADE){
        cout << "Starting Era n_" << e << " - - - - - ";
        cout << "Current Champ: " << bestOfGalapagos.getFitnessValue() << endl;

        for(int i = 0; i<populations.size(); i++) {
            //cout << "Starting Isola n_" << i << endl;
            Eras(populations.at(i), G);
        }
        cout << "Migration phase now !! " << endl;

        Migration_randomOnes(populations);
        e++;
        if(check_early_end(bestOfGalapagos, populations)){
            ERAS_NO_UPGRADE --;
        }else{
            bestOfGalapagos = BestOfGalapagos(populations);
            ERAS_NO_UPGRADE = era_waited_for_improvement;
        }
    }

    return BestOfGalapagos(populations);
}


/** Parallel implementation */

void GeneticAlgorithm::Eras_parallel(vector<Individual>& population, const Graph& G, barrier<>& b1, barrier<>& b2) {
    float mutation_rate = .25;
    Individual parent1, parent2, offspring;
    srand(std::time(0));

    random_device rd;
    uniform_int_distribution uid(0, 1);
    default_random_engine dre(rd());

    for(int e = 0; e<NUM_ERAS; e++){

        for (int g = 0; g < NUM_GENERATIONS; g++) {
            cout << "Starting Generation n_" << g << endl;

            for (int i = 0; i < NUM_OFFSPRING; i++) {

                if ((float) uid(dre) < mutation_rate) {

                    offspring = random_parent_selection(population);
                    offspring.mutation();
                    offspring.setFitnessValue(fitness(NUM_PARTITIONS, offspring.getGenotype(), G));

                } else {

                    parent1 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                    parent2 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                    offspring = uniform_random_crossover(parent1, parent2);
                    offspring.setFitnessValue(fitness(NUM_PARTITIONS, offspring.getGenotype(), G));

                }

                population.push_back(offspring);

            }

            sort(population.begin(), population.end(), ascending_order);

            for (int i = 0; i < NUM_OFFSPRING; i++) {
                auto it = population.begin() + POPULATION_SIZE;
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

Individual GeneticAlgorithm::Galapagos_parallel_fixed(map<int, vector<Individual>>& populations, const Graph& G){

    vector<thread> Islands;

    /*pthread_barrier_init(&barrier_1, nullptr, populations.size() + 1);
    pthread_barrier_init(&barrier_2, nullptr, populations.size() + 1);*/

    barrier barrier_1_cpp(populations.size() + 1);
    barrier barrier_2_cpp(populations.size() + 1);


    for(int i = 0; i<populations.size(); i++) {
        cout << "Starting Isola n_" << i << endl;
        Islands.emplace_back( &GA::Eras_parallel, populations.at(i), G, barrier_1_cpp, barrier_2_cpp);

    }

    for(int e = 1; e<NUM_ERAS; e++){
        cout << "Starting Era n_" << e << endl;

        //pthread_barrier_wait(&barrier_1);
        barrier_1_cpp.arrive_and_wait();

        cout << "Migration phase now !! " << endl;
        Migration_bestOnes(populations);

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

Individual GeneticAlgorithm::Galapagos_parallel(map<int, vector<Individual>>& populations, const Graph& G){

    vector<thread> Islands;
    barrier barrier_1_cpp(populations.size() + 1);
    barrier barrier_2_cpp(populations.size() + 1);

    Individual bestOfGalapagos = BestOfGalapagos(populations);
    float lerning_rate = 0.03;
    int e = 1;
    const int era_waited_for_improvement = ERAS_NO_UPGRADE;


    for(int i = 0; i<populations.size(); i++) {
        cout << "Starting Isola n_" << i << endl;
        Islands.emplace_back( &GA::Eras_parallel, populations.at(i), G, barrier_1_cpp, barrier_2_cpp);

    }
    while(ERAS_NO_UPGRADE){
        cout << "Starting Era n_" << e << " - - - - - ";
        cout << "Current Champ: " << bestOfGalapagos.getFitnessValue() << endl;

        barrier_1_cpp.arrive_and_wait();

        cout << "Migration phase now !! " << endl;
        e++;

        Migration_bestOnes(populations);

        if(check_early_end(bestOfGalapagos, populations)){
            ERAS_NO_UPGRADE --;
        }else{
            bestOfGalapagos = BestOfGalapagos(populations);
            ERAS_NO_UPGRADE = era_waited_for_improvement;
        }

        barrier_2_cpp.arrive_and_wait();

    }

    for(auto& t: Islands) {
        t.join();
    }

    /*pthread_barrier_destroy(&barrier_1);
    pthread_barrier_destroy(&barrier_2);*/

    return BestOfGalapagos(populations);
}


/** Crossover function */

Individual GeneticAlgorithm::one_cut_crossover(Individual I1, Individual I2){

    srand(std::time(0));
    int cut = rand() % (I1.getGenotypeSize() -1) +1;
    std::vector<int> newVector;

    newVector.insert(newVector.end(), I1.getGenotype().begin(), I1.getGenotype().begin() + cut);
    newVector.insert(newVector.end(), I2.getGenotype().begin() + cut, I2.getGenotype().end());

    Individual newIndividual(newVector, I1.getNumAlleles());

    return newIndividual;
}

Individual GeneticAlgorithm::two_cut_crossover(Individual I1, Individual I2){

    srand(std::time(0));
    int cut1 = rand() % (I1.getGenotypeSize() -1) +1;
    int cut2 = rand() % (I1.getGenotypeSize() -1) +1;

    std::vector<int> newVector;

    if(rand()%2){
        newVector.insert(newVector.end(), I1.getGenotype().begin(), I1.getGenotype().begin() + min(cut1, cut2));
        newVector.insert(newVector.end(), I2.getGenotype().begin() + min(cut1, cut2), I2.getGenotype().end()+ max(cut1, cut2));
        newVector.insert(newVector.end(), I1.getGenotype().begin() + max(cut1, cut2), I1.getGenotype().end());

    }else{
        newVector.insert(newVector.end(), I2.getGenotype().begin(), I2.getGenotype().begin() + min(cut1, cut2));
        newVector.insert(newVector.end(), I1.getGenotype().begin() + min(cut1, cut2), I1.getGenotype().end() + max(cut1, cut2));
        newVector.insert(newVector.end(), I2.getGenotype().begin() + max(cut1, cut2), I2.getGenotype().end());
    }

    Individual newIndividual(newVector, I1.getNumAlleles());

    return newIndividual;
}

Individual GeneticAlgorithm::n_cut_crossover(Individual I1, Individual I2){

    srand(std::time(0));
    int cuts = rand() % (I1.getGenotypeSize() -1) +1;
    set<int> set_cut;

    for (int i = 0; i < cuts; ++i) {
        set_cut.insert(rand() % (I1.getGenotypeSize() -1) +1);
    }

    vector<int> vett_cut(set_cut.begin(), set_cut.end());

    std::vector<int> newVector;

    if(rand()%2){
        for (int i = 0; i <= vett_cut.size(); ++i) {
            if(i==0) {
                newVector.insert(newVector.end(), I1.getGenotype().begin(), I1.getGenotype().begin() + vett_cut.front());
            }else if(i==vett_cut.size()) {
                if(i%2) {
                    newVector.insert(newVector.end(), I2.getGenotype().begin() + vett_cut.back(), I2.getGenotype().end());
                }else{
                    newVector.insert(newVector.end(), I1.getGenotype().begin() + vett_cut.back(), I1.getGenotype().end());
                }
            }else if(i%2) {
                newVector.insert(newVector.end(), I2.getGenotype().begin() + vett_cut[i-1], I2.getGenotype().begin() + vett_cut[i]);
            }else{
                newVector.insert(newVector.end(), I1.getGenotype().begin() + vett_cut[i-1], I1.getGenotype().begin() + vett_cut[i]);
            }
        }
    }else{
        for (int i = 0; i <= vett_cut.size(); ++i) {
            if(i==0){
                newVector.insert(newVector.end(), I2.getGenotype().begin(), I2.getGenotype().begin() + vett_cut.front());
            }else if(i==vett_cut.size()){
                if(i%2) {
                    newVector.insert(newVector.end(), I1.getGenotype().begin() + vett_cut.back(), I1.getGenotype().end());
                }else {
                    newVector.insert(newVector.end(), I2.getGenotype().begin() + vett_cut.back(), I2.getGenotype().end());
                }
            }else if(i%2) {
                newVector.insert(newVector.end(), I2.getGenotype().begin() + vett_cut[i-1], I2.getGenotype().begin() + vett_cut[i]);
            }else{
                newVector.insert(newVector.end(), I1.getGenotype().begin() + vett_cut[i-1], I1.getGenotype().begin() + vett_cut[i]);
            }
        }
    }

    Individual newIndividual(newVector, I1.getNumAlleles());

    return newIndividual;
}

Individual GeneticAlgorithm::uniform_crossover(Individual I1, Individual I2){

    srand(std::time(0));

    std::vector<int> newVector1, newVector2;

    for (int i = 0; i < I1.getGenotypeSize(); ++i) {
        if(i%2){
            newVector1.push_back(I1.getGenotype()[i]);
            newVector2.push_back(I2.getGenotype()[i]);
        }else{
            newVector1.push_back(I2.getGenotype()[i]);
            newVector2.push_back(I1.getGenotype()[i]);
        }
    }
    if (rand() % 2) {
        return {newVector1, I1.getNumAlleles()};
    } else {
        return {newVector2, I1.getNumAlleles()};
    }
}

Individual GeneticAlgorithm::uniform_random_crossover(Individual I1, Individual I2){

    srand(std::time(0));

    std::vector<int> newVector;

    for (int i = 0; i < I1.getGenotypeSize(); ++i) {
        if(rand() % 2){
            newVector.push_back(I1.getGenotype()[i]);
        }else{
            newVector.push_back(I2.getGenotype()[i]);
        }
    }

    Individual newIndividual(newVector, I1.getNumAlleles());

    return newIndividual;
}


/** Parent selection function */

Individual GeneticAlgorithm::parent_selection_tournament(int num_partecipants, const vector<Individual>& population){
    random_device rd;
    uniform_int_distribution uid(0, (int) population.size()-1);
    default_random_engine dre(rd());

    vector<Individual> partecipants;

    for(int i=0; i<num_partecipants; i++){
        int pos = uid(dre);
        auto it = find(partecipants.begin(), partecipants.end(), population[pos]);

        if(it == partecipants.end()) {
            partecipants.push_back(population[pos]);
        }
    }

    sort(partecipants.begin(), partecipants.end(), ascending_order);
    return partecipants[0];
}

Individual GeneticAlgorithm::random_parent_selection(const vector<Individual>& population){
    srand(std::time(0));

    return population[rand()%population.size()];
}


/** Migration function */

void GeneticAlgorithm::Migration_bestOnes(map<int, vector<Individual>>& galapagosPopulation){
    vector<Individual> bestOf;
    srand(std::time(0));
    Individual I;
    int index = 0;

    random_device rd;
    uniform_int_distribution uid(0, 1);
    default_random_engine dre(rd());


    for(auto & i : galapagosPopulation){
        for(int j = 0; j<NUM_MIGRANTS; j++){

            bestOf.emplace_back(i.second.front());
            i.second.erase(i.second.begin());

        }
    }

    for(auto & i : galapagosPopulation){
        for(int j = 0; j<NUM_MIGRANTS; j++){

            (bestOf.size()-1) ? index = rand() % (bestOf.size() - 1) : index = 0;
            i.second.emplace_back(bestOf[index]);
            bestOf.erase(bestOf.begin() + index);

        }
    }
}

void GeneticAlgorithm::Migration_randomOnes(map<int, vector<Individual>>& galapagosPopulation){
    vector<Individual> bestOf;
    srand(std::time(0));
    Individual I;
    int index = 0;

    random_device rd;
    uniform_int_distribution uid(0, 1);
    default_random_engine dre(rd());


    for(auto & i : galapagosPopulation){
        for(int j = 0; j<NUM_MIGRANTS; j++){

            index = rand() % (i.second.size() - 1);
            bestOf.emplace_back(i.second[index]);
            i.second.erase(i.second.begin()+index);

        }
    }

    for(auto & i : galapagosPopulation){
        for(int j = 0; j<NUM_MIGRANTS; j++){

            (bestOf.size()-1) ? index = rand() % (bestOf.size() - 1) : index = 0;
            i.second.emplace_back(bestOf[index]);
            bestOf.erase(bestOf.begin() + index);

        }
    }
}


/** funzioni varie */

bool GeneticAlgorithm::check_early_end(const Individual& champ, map<int, vector<Individual>>& populations){
    bool no_upgrade = true;

    for(auto & population : populations){
        float percentage_gap = (champ.getFitnessValue() - population.second.front().getFitnessValue())  / champ.getFitnessValue();
        if ( percentage_gap > LEARNING_RATE ){
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

Individual GeneticAlgorithm::BestOfGalapagos(map<int, vector<Individual>>& galapagosPopulation){
    vector<Individual> v;

    for(auto vI : galapagosPopulation){
        v.emplace_back(vI.second[0]);
    }
    sort(v.begin(), v.end(), ascending_order);

    return v[0];
}

vector<Individual> GeneticAlgorithm::BestOfIslands(map<int, vector<Individual>>& galapagosPopulation){
    vector<Individual> v;

    for(auto vI : galapagosPopulation){
        v.emplace_back(vI.second[0]);
    }

    return v;
}

