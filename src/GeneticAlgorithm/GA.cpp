//
// Created by Lorenzo Ugoccioni on 14/01/2024.
//

#include "GA.h"

#define MUTATION_RATE 0.25

mutex printMutex;

/** Start Algorithm */

void GeneticAlgorithm::run(const Graph& G){
    for (int is = 0; is<NUM_ISLANDS; is++) {  
        cout << "Starting Population for Island n_" << is << endl;
        for (int in = 0; in < POPULATION_SIZE; in++) {
            Population[is].emplace_back(Individual(NUM_PARTITIONS, G.num_of_nodes(), G, balanced));
        }
    }
    if( parallel && dynamic ) {
        Galapagos_parallel_LR(G);
    }else if( parallel && not dynamic ){
        Galapagos_parallel(G);
    }else if( not parallel && dynamic ){
        Galapagos_LR(G);
    }else if( not parallel && not dynamic ){
        Galapagos(G);
    }else{
        cout <<"The program can't continue.\n Check if all the parameters are set correctly !!"<< endl;
    }

    //cout <<"\n *****   Our Algorithm ended correctly !!   *****\n"<< endl;
    bestOf = BestOfGalapagos();

}


/** Sequential implementation */

void GeneticAlgorithm::Eras(vector<Individual>& population, const Graph& G) {
    Individual parent1, parent2, offspring;
    srand(std::time(nullptr));

    for (int g = 0; g < NUM_GENERATIONS; g++) {
        cout << "Starting Generation n_" << g << endl;

        for (int i = 0; i < NUM_OFFSPRING; i++) {

            cout << "Offspring " << i << endl;

            if ( ( ( (float) rand() ) / ( (float) RAND_MAX+1.0 ) ) < MUTATION_RATE) {

                offspring = random_parent_selection(population);
                offspring.mutation();
                offspring.setFitness(G, balanced);

            } else {
                //DAI UN OCCHIO !!!!!
                parent1 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                parent2 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                offspring = one_cut_crossover(parent1, parent2);
                offspring.setFitness(G, balanced);

            }

            population.push_back(offspring);

        }

        sort(population.begin(), population.end(), [](const Individual& a, const Individual& b){return a.getFitness() < b.getFitness();});

        for (int i = 0; i < NUM_OFFSPRING; i++) {
            auto it = population.begin() + POPULATION_SIZE;
            population.erase(it);
        }
        /*population.resize(population_size)*/
    }
}

void GeneticAlgorithm::Galapagos( const Graph& G){

    for(int e = 0; e<NUM_ERAS; e++){
        cout << "Starting Era n_" << e << endl;// -->DEBUG

        for(int i = 0; i<NUM_ISLANDS; i++) {
            cout << "Starting Isola n_" << i << endl;// -->DEBUG

            Eras(Population.at(i), G);

        }
        //cout << "Migration phase now !! " << endl;// -->DEBUG
        if(NUM_ISLANDS > 1)
            Migration_bestOnes();
    }

}

void GeneticAlgorithm::Galapagos_LR( const Graph& G){
    Individual bestOfGalapagos = BestOfGalapagos();
    int e = 1;// -->DEBUG
    const int era_waited_for_improvement = ERAS_NO_UPGRADE;
    while(ERAS_NO_UPGRADE){
        cout << "Starting Era n_" << e << " - - - - - ";// -->DEBUG
        cout << "Current Champ: " << bestOfGalapagos.getFitness() << endl;// -->DEBUG

        for(int i = 0; i<NUM_ISLANDS; i++) {
            cout << "Starting Isola n_" << i << endl;
            Eras(Population.at(i), G);
        }
        cout << "Migration phase now !! " << endl;// -->DEBUG

        Migration_randomOnes();
        e++;// -->DEBUG
        if(check_early_end(bestOfGalapagos)){
            ERAS_NO_UPGRADE --;
        }else{
            bestOfGalapagos = BestOfGalapagos();
            ERAS_NO_UPGRADE = era_waited_for_improvement;
        }
    }

}


/** Parallel implementation */
void GeneticAlgorithm::Eras_parallel(int island_id, vector<Individual>& population, const Graph& G, barrier<>& b1, barrier<>& b2) {
    Individual parent1, parent2, offspring;
    srand(std::time(nullptr));

    for(int e = 0; e<NUM_ERAS; e++){

        for (int g = 0; g < NUM_GENERATIONS; g++) {
            cout << "Starting Generation n_" << g << endl;

            for (int i = 0; i < NUM_OFFSPRING; i++) {

                if ( ( ( (float) rand() ) / ( (float) RAND_MAX+1.0 ) ) < MUTATION_RATE) {

                    offspring = random_parent_selection(population);
                    offspring.mutation();
                    offspring.setFitness(G, balanced);

                } else {

                    parent1 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                    parent2 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                    offspring = one_cut_crossover(parent1, parent2);
                    offspring.setFitness(G, balanced);

                }

                population.push_back(offspring);

            }

            sort(population.begin(), population.end(), [](const Individual& a, const Individual& b){return a.getFitness() < b.getFitness();});

            for (int i = 0; i < NUM_OFFSPRING; i++) {
                auto it = population.begin() + POPULATION_SIZE;
                population.erase(it);
            }

            //population.resize(population_size)
        }

        printMutex.lock();
        cout << "ISLAND " << island_id << " WAITING FOR MIGRATION" << endl;
        printMutex.unlock();
        b1.arrive_and_wait();

        // WAIT ...
        b2.arrive_and_wait();

    }
}

void GeneticAlgorithm::Galapagos_parallel(const Graph& G){

    vector<thread> Islands;

    barrier<> barrier_1_cpp(NUM_ISLANDS + 1);
    barrier<> barrier_2_cpp(NUM_ISLANDS + 1);


    for(int i = 0; i<NUM_ISLANDS; i++) {
        Islands.emplace_back( [=, &G, &barrier_1_cpp, &barrier_2_cpp] {Eras_parallel(i, Population.at(i), G, barrier_1_cpp, barrier_2_cpp);});

    }

    for(int e = 0; e<NUM_ERAS; e++){
        cout << "Starting Era n_" << e << endl; // -->DEBUG


        barrier_1_cpp.arrive_and_wait();

        cout << "Migration phase now !! " << endl;// -->DEBUG
        Migration_bestOnes();

        barrier_2_cpp.arrive_and_wait();

    }

    for(auto& t: Islands) {
        t.join();
    }

}

void GeneticAlgorithm::Galapagos_parallel_LR(const Graph& G){

    vector<thread> Islands;
    barrier<> barrier_1_cpp(NUM_ISLANDS + 1);
    barrier<> barrier_2_cpp(NUM_ISLANDS + 1);

    Individual bestOfGalapagos = BestOfGalapagos();
    int e = 1;// -->DEBUG
    int era_waited_for_improvement = ERAS_NO_UPGRADE;

    for(int i = 0; i<NUM_ISLANDS; i++) {
        Islands.emplace_back( [=, &G, &barrier_1_cpp, &barrier_2_cpp] {Eras_parallel(i, Population.at(i), G, barrier_1_cpp, barrier_2_cpp);});

    }
    while(era_waited_for_improvement){
        cout << "Starting Era n_" << e << " - - - - - ";// -->DEBUG
        cout << "Current Champ: " << bestOfGalapagos.getFitness() << endl;// -->DEBUG

        barrier_1_cpp.arrive_and_wait();

        cout << "Migration phase now !! " << endl;// -->DEBUG
        e++;// -->DEBUG

        Migration_bestOnes();

        if(check_early_end(bestOfGalapagos)){
            era_waited_for_improvement --;
        }else{
            bestOfGalapagos = BestOfGalapagos();
            era_waited_for_improvement = ERAS_NO_UPGRADE;
        }

        barrier_2_cpp.arrive_and_wait();

    }

    for(auto& t: Islands) {
        t.join();
    }

}


/** Crossover function */

Individual GeneticAlgorithm::one_cut_crossover(Individual I1, Individual I2){

    srand(std::time(nullptr));
    int cut = rand() % (I1.getGenotypeSize() -1) +1;
    std::vector<int> newVector;

    newVector.insert(newVector.end(), I1.getGenotype().begin(), I1.getGenotype().begin() + cut);
    newVector.insert(newVector.end(), I2.getGenotype().begin() + cut, I2.getGenotype().end());

    Individual newIndividual(newVector, I1.getNumAlleles());

    return newIndividual;
}

Individual GeneticAlgorithm::two_cut_crossover(Individual I1, Individual I2){

    srand(std::time(nullptr));
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

    srand(std::time(nullptr));
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

    srand(std::time(nullptr));

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

    srand(std::time(nullptr));

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
    vector<Individual> partecipants;

    for(int i=0; i<num_partecipants; i++){
        int pos = rand() % (population.size()-1);
        auto it = find(partecipants.begin(), partecipants.end(), population[pos]);

        if(it == partecipants.end()) {
            partecipants.push_back(population[pos]);
        }
    }

    sort(partecipants.begin(), partecipants.end(), [](const Individual& a, const Individual& b){return a.getFitness() < b.getFitness();});
    return partecipants[0];
}

Individual GeneticAlgorithm::random_parent_selection(const vector<Individual>& population){
    srand(std::time(nullptr));

    return population[rand()%population.size()];
}


/** Migration function */

void GeneticAlgorithm::Migration_bestOnes(){
    vector<Individual> vett_bestOf;
    srand(std::time(nullptr));
    Individual I;
    int index = 0;
    cout << "migrazione  " <<endl;

    for(auto & i : Population){
        cout << "isola sceglie  !!" <<endl;

        for(int j = 0; j<NUM_MIGRANTS; j++){
            cout << "migrante  " << j <<endl;

            vett_bestOf.emplace_back(i.second.front());
            i.second.erase(i.second.begin());

        }
    }

    for(auto & i : Population){
        for(int j = 0; j<NUM_MIGRANTS; j++){
            cout << "migrante  " << j <<endl;

            (vett_bestOf.size()-1) ? index = rand() % (vett_bestOf.size() - 1) : index = 0;
            i.second.emplace_back(vett_bestOf[index]);
            vett_bestOf.erase(vett_bestOf.begin() + index);

        }
    }
}

void GeneticAlgorithm::Migration_randomOnes(){
    vector<Individual> vett_randOnes;
    srand(std::time(nullptr));
    Individual I;
    int index = 0;


    for(auto & i : Population){
        cout << "isola sceglie  !!" <<endl;

        for(int j = 0; j<NUM_MIGRANTS; j++){
            cout << "migrante  " << j <<endl;
            index = rand() % (i.second.size() - 1);
            vett_randOnes.emplace_back(i.second[index]);
            i.second.erase(i.second.begin()+index);

        }
    }

    for(auto & i : Population){
        for(int j = 0; j<NUM_MIGRANTS; j++){
            cout << "migrante  "<< j <<endl;

            (vett_randOnes.size()-1) ? index = rand() % (vett_randOnes.size() - 1) : index = 0;
            i.second.emplace_back(vett_randOnes[index]);
            vett_randOnes.erase(vett_randOnes.begin() + index);

        }
    }
}


/** Misc */

bool GeneticAlgorithm::check_early_end(const Individual& champ){
    bool no_upgrade = true;

    for(auto & population : Population){
        float percentage_gap = (champ.getFitness() - population.second.front().getFitness())  / champ.getFitness();
        if ( percentage_gap > LEARNING_RATE ){
            no_upgrade = false;
            break;
        }
    }

    return no_upgrade;

}

Individual GeneticAlgorithm::BestOfGalapagos(){
    vector<Individual> v = IslandsBests();

    sort(v.begin(), v.end(), [](const Individual& a, const Individual& b){return a.getFitness() < b.getFitness();});

    return v[0];
}

vector<Individual> GeneticAlgorithm::IslandsBests(){
    vector<Individual> v;

    for(auto vI : Population){
        v.emplace_back(vI.second[0]);
    }

    return v;
}

void GeneticAlgorithm::printGASettings(){
    cout << "BALANCED: " << balanced << endl;
    cout << "PARALLEL: " << parallel << endl;
    cout << "DYNAMIC: " << dynamic << endl;
    cout << "NUM_PARTITIONS: " << NUM_PARTITIONS << endl;
    cout << "POPULATION_SIZE: " << POPULATION_SIZE << endl;
    cout << "NUM_GENERATIONS: " << NUM_GENERATIONS << endl;
    cout << "NUM_OFFSPRING: " << NUM_OFFSPRING << endl;
    cout << "NUM_ISLANDS: " << NUM_ISLANDS << endl;
    cout << "NUM_ERAS: " << NUM_ERAS << endl;
    cout << "NUM_MIGRANTS: " << NUM_MIGRANTS << endl;
    cout << "ERAS_NO_UPGRADE: " << ERAS_NO_UPGRADE << endl;
    cout << "LEARNING_RATE: " << LEARNING_RATE << endl;
}