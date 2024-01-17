//
// Created by Lorenzo Ugoccioni on 14/01/2024.
//

#include "GA.h"

mutex printMutex;


/** Start Algorithm */

void GeneticAlgorithm::run(const Graph& G){
    for (int is = 0; is<NUM_ISLANDS; is++) {    /** Si potrà parallelizzare anche la creazione delle popolazioni iniziali delle varie isole? */
        cout << "Starting Population for Island n_" << is << endl;
        for (int in = 0; in < POPULATION_SIZE; in++) {
            Population[is].emplace_back(Individual(NUM_PARTITIONS, G.num_of_nodes(), G));
        }
    }

    if( parallel && dynamic ) {
        bestOf = Galapagos_parallel(G);
    }if( parallel && not dynamic ){
        bestOf = Galapagos_parallel_fixed(G);
    }else if( not parallel && dynamic ){
        bestOf = Galapagos(G);
    }else if( not parallel && not dynamic ){
        bestOf = Galapagos_fixed(G);
    }else{
        cout <<"Our algorithm can't continue.\n Check if all the parameters are set correctly !!"<< endl;
    }

    cout <<"\n *****   Our Algorithm ended correctly !!   *****\n"<< endl;

}


/** Sequential implementation */

void GeneticAlgorithm::Eras(vector<Individual>& population, const Graph& G) {
    float mutation_rate = 0.25;
    Individual parent1, parent2, offspring;
    srand(std::time(nullptr));

    for (int g = 0; g < NUM_GENERATIONS; g++) {
        //cout << "Starting Generation n_" << g << endl;

        for (int i = 0; i < NUM_OFFSPRING; i++) {

            if ( ( ( (float) rand() ) / ( (float) RAND_MAX+1.0 ) ) < mutation_rate) {

                offspring = random_parent_selection(population);
                offspring.mutation();
                offspring.setFitnessValue(G);

            } else {
                //DAI UN OCCHIO !!!!!
                parent1 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                parent2 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                offspring = uniform_random_crossover(parent1, parent2);
                offspring.setFitnessValue(G);

            }

            population.push_back(offspring);

        }

        sort(population.begin(), population.end(), [](const Individual& a, const Individual& b){return a.getFitnessValue() < b.getFitnessValue();});

        for (int i = 0; i < NUM_OFFSPRING; i++) {
            auto it = population.begin() + POPULATION_SIZE;
            population.erase(it);
        }
        /*population.resize(population_size)*/
    }
}

Individual GeneticAlgorithm::Galapagos_fixed( const Graph& G){

    for(int e = 0; e<NUM_ERAS; e++){
        cout << "Starting Era n_" << e << endl;

        for(int i = 0; i<NUM_ISLANDS; i++) {
            cout << "Starting Isola n_" << i << endl;

            Eras(Population.at(i), G);
        }
        cout << "Migration phase now !! " << endl;

        Migration_bestOnes();
    }

    return BestOfGalapagos();
}

Individual GeneticAlgorithm::Galapagos( const Graph& G){
    Individual bestOfGalapagos = BestOfGalapagos();
    float lerning_rate = 0.03;
    int e = 1;
    const int era_waited_for_improvement = ERAS_NO_UPGRADE;
    while(ERAS_NO_UPGRADE){
        cout << "Starting Era n_" << e << " - - - - - ";
        cout << "Current Champ: " << bestOfGalapagos.getFitnessValue() << endl;

        for(int i = 0; i<NUM_ISLANDS; i++) {
            //cout << "Starting Isola n_" << i << endl;
            Eras(Population.at(i), G);
        }
        cout << "Migration phase now !! " << endl;

        Migration_randomOnes();
        e++;
        if(check_early_end(bestOfGalapagos)){
            ERAS_NO_UPGRADE --;
        }else{
            bestOfGalapagos = BestOfGalapagos();
            ERAS_NO_UPGRADE = era_waited_for_improvement;
        }
    }

    return BestOfGalapagos();
}


/** Parallel implementation */
void GeneticAlgorithm::Eras_parallel(int island_id, vector<Individual>& population, const Graph& G, barrier<>& b1, barrier<>& b2) {
    float mutation_rate = .25;
    Individual parent1, parent2, offspring;
    srand(std::time(nullptr));

    for(int e = 0; e<NUM_ERAS; e++){

        for (int g = 0; g < NUM_GENERATIONS; g++) {
            cout << "Starting Generation n_" << g << endl;

            for (int i = 0; i < NUM_OFFSPRING; i++) {

                if ( ( ( (float) rand() ) / ( (float) RAND_MAX+1.0 ) ) < mutation_rate) {

                    offspring = random_parent_selection(population);
                    offspring.mutation();
                    offspring.setFitnessValue(G);

                } else {

                    parent1 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                    parent2 = parent_selection_tournament(rand() % (population.size() / 5 - 1) + 1, population);
                    offspring = uniform_random_crossover(parent1, parent2);
                    offspring.setFitnessValue(G);

                }

                population.push_back(offspring);

            }

            sort(population.begin(), population.end(), [](const Individual& a, const Individual& b){return a.getFitnessValue() < b.getFitnessValue();});

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

Individual GeneticAlgorithm::Galapagos_parallel_fixed(const Graph& G){

    vector<thread> Islands;

    barrier<> barrier_1_cpp(NUM_ISLANDS + 1);
    barrier<> barrier_2_cpp(NUM_ISLANDS + 1);


    for(int i = 0; i<NUM_ISLANDS; i++) {
        Islands.emplace_back( [=, &G, &barrier_1_cpp, &barrier_2_cpp] {Eras_parallel(i, Population.at(i), G, barrier_1_cpp, barrier_2_cpp);});

    }

    for(int e = 1; e<NUM_ERAS; e++){
        cout << "Starting Era n_" << e << endl;

        barrier_1_cpp.arrive_and_wait();

        cout << "Migration phase now !! " << endl;
        Migration_bestOnes();

        barrier_2_cpp.arrive_and_wait();

    }

    for(auto& t: Islands) {
        t.join();
    }

    return BestOfGalapagos();
}

Individual GeneticAlgorithm::Galapagos_parallel(const Graph& G){

    vector<thread> Islands;
    barrier<> barrier_1_cpp(NUM_ISLANDS + 1);
    barrier<> barrier_2_cpp(NUM_ISLANDS + 1);

    Individual bestOfGalapagos = BestOfGalapagos();
    float lerning_rate = 0.03;
    int e = 1;
    int era_waited_for_improvement = ERAS_NO_UPGRADE;


    for(int i = 0; i<NUM_ISLANDS; i++) {
        Islands.emplace_back( [=, &G, &barrier_1_cpp, &barrier_2_cpp] {Eras_parallel(i, Population.at(i), G, barrier_1_cpp, barrier_2_cpp);});

    }
    while(era_waited_for_improvement){
        cout << "Starting Era n_" << e << " - - - - - ";
        cout << "Current Champ: " << bestOfGalapagos.getFitnessValue() << endl;

        barrier_1_cpp.arrive_and_wait();

        cout << "Migration phase now !! " << endl;
        e++;

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

    return BestOfGalapagos();
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

    sort(partecipants.begin(), partecipants.end(), [](const Individual& a, const Individual& b){return a.getFitnessValue() < b.getFitnessValue();});
    return partecipants[0];
}

Individual GeneticAlgorithm::random_parent_selection(const vector<Individual>& population){
    srand(std::time(nullptr));

    return population[rand()%population.size()];
}


/** Migration function */

void GeneticAlgorithm::Migration_bestOnes(){
    vector<Individual> bestOf;
    srand(std::time(nullptr));
    Individual I;
    int index = 0;

    for(auto & i : Population){
        for(int j = 0; j<NUM_MIGRANTS; j++){

            bestOf.emplace_back(i.second.front());
            i.second.erase(i.second.begin());

        }
    }

    for(auto & i : Population){
        for(int j = 0; j<NUM_MIGRANTS; j++){

            (bestOf.size()-1) ? index = rand() % (bestOf.size() - 1) : index = 0;
            i.second.emplace_back(bestOf[index]);
            bestOf.erase(bestOf.begin() + index);

        }
    }
}

void GeneticAlgorithm::Migration_randomOnes(){
    vector<Individual> bestOf;
    srand(std::time(nullptr));
    Individual I;
    int index = 0;


    for(auto & i : Population){
        for(int j = 0; j<NUM_MIGRANTS; j++){

            index = rand() % (i.second.size() - 1);
            bestOf.emplace_back(i.second[index]);
            i.second.erase(i.second.begin()+index);

        }
    }

    for(auto & i : Population){
        for(int j = 0; j<NUM_MIGRANTS; j++){

            (bestOf.size()-1) ? index = rand() % (bestOf.size() - 1) : index = 0;
            i.second.emplace_back(bestOf[index]);
            bestOf.erase(bestOf.begin() + index);

        }
    }
}


/** funzioni varie */

bool GeneticAlgorithm::check_early_end(const Individual& champ){
    bool no_upgrade = true;

    for(auto & population : Population){
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

Individual GeneticAlgorithm::BestOfGalapagos(){
    vector<Individual> v = IslandsBests();

    sort(v.begin(), v.end(), [](const Individual& a, const Individual& b){return a.getFitnessValue() < b.getFitnessValue();});

    return v[0];
}

vector<Individual> GeneticAlgorithm::IslandsBests(){
    vector<Individual> v;

    for(auto vI : Population){
        v.emplace_back(vI.second[0]);
    }

    return v;
}

void GeneticAlgorithm::set__param(int num_param, char* params[]) {

    const char *const short_opts = "kabc:d:e:f:g:h:i:j:";    //""abc:d:e:f:g:h:i:j:
    const option long_opts[] = {
            {"bal",        required_argument, nullptr, 'k'},
            {"mod",        required_argument, nullptr, 'a'},
            {"parallel",   required_argument, nullptr, 'b'},
            {"part",       required_argument, nullptr, 'c'},
            {"population", required_argument, nullptr, 'd'},
            {"gen",        required_argument, nullptr, 'e'},
            {"era",        required_argument, nullptr, 'f'},
            {"new",        required_argument, nullptr, 'g'},
            {"isl",        required_argument, nullptr, 'h'},
            {"mig",        required_argument, nullptr, 'i'},
            {"lr",         required_argument, nullptr, 'j'},
            {nullptr,      0,                 nullptr, 0}
    };

    int opt;
    while ((opt = getopt_long(num_param, params, short_opts, long_opts, nullptr)) != -1) {
        switch (opt) {
            case 'a':
                std::cout << "Opzione -mod settata " << optarg << std::endl;
                dynamic = true;
                break;
            case 'b':
                std::cout << "Opzione -parallel settata " << optarg << std::endl;
                parallel = true;
                break;
            case 'c':
                std::cout << "Opzione -part con argomento: " << optarg << std::endl;
                NUM_PARTITIONS = stoi(params[4]);
                break;
            case 'd':
                std::cout << "Opzione -population con argomento: " << optarg << std::endl;
                POPULATION_SIZE = stoi(params[5]);
                break;
            case 'e':
                std::cout << "Opzione -gen con argomento: " << optarg << std::endl;
                NUM_GENERATIONS = stoi(params[6]);
                break;
            case 'f':
                std::cout << "Opzione -era con argomento: " << optarg << std::endl;
                if (dynamic) {
                    ERAS_NO_UPGRADE = stoi(params[7]);
                } else {
                    NUM_ERAS = stoi(params[7]);
                }
                break;
            case 'g':
                std::cout << "Opzione -new con argomento: " << optarg << std::endl;
                NUM_OFFSPRING = stoi(params[8]);
                break;
            case 'h':
                std::cout << "Opzione -isl con argomento: " << optarg << std::endl;
                NUM_ISLANDS = stoi(params[9]);
                break;
            case 'i':
                std::cout << "Opzione -mig con argomento: " << optarg << std::endl;
                NUM_MIGRANTS = stoi(params[10]);
                break;
            case 'j':
                std::cout << "Opzione -lr con argomento: " << optarg << std::endl;
                if (dynamic) {
                    LEARNING_RATE = stof(params[11])/100;
                }
                break;
            case 'k':
                std::cout << "Opzione -bal settata " << optarg << std::endl;
                balanced = true;
                break;
            default:
                std::cerr << "Opzione non valida." << std::endl;
        }

    }
}
