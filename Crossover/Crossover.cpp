//
// Created by Lorenzo Ugoccioni on 06/01/2024.
//

#include "Crossover.h"

Individual one_cut_crossover(Individual I1, Individual I2){

    srand(std::time(0));
    int cut = rand() % (I1.getGenotypeSize() -1) +1;
    std::vector<int> newVector;

    newVector.insert(newVector.end(), I1.getGenotype().begin(), I1.getGenotype().begin() + cut);
    newVector.insert(newVector.end(), I2.getGenotype().begin() + cut, I2.getGenotype().end());

    Individual newIndividual(newVector);

    return newIndividual;
}

Individual two_cut_crossover(Individual I1, Individual I2){

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

    Individual newIndividual(newVector);

    return newIndividual;
}

Individual n_cut_crossover(Individual I1, Individual I2){

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

    Individual newIndividual(newVector);

    return newIndividual;
}

Individual uniform_crossover(Individual I1, Individual I2){

    srand(std::time(0));

    std::vector<int> newVector1, newVector2;
    Individual newIndividual;

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
        newIndividual.setGenotype(newVector1);
    } else {
        newIndividual.setGenotype(newVector2);
    }

    return newIndividual;
}

Individual unform_random_crossover(Individual I1, Individual I2){

    srand(std::time(0));

    std::vector<int> newVector;

    for (int i = 0; i < I1.getGenotypeSize(); ++i) {
        if(rand() % 2){
            newVector.push_back(I1.getGenotype()[i]);
        }else{
            newVector.push_back(I2.getGenotype()[i]);
        }
    }

    Individual newIndividual(newVector);

    return newIndividual;
}

Individual parent_selection_tournament(int num_partecipants, const vector<Individual>& population){
    random_device rd;
    uniform_int_distribution uid(1, (int) population.size());
    default_random_engine dre(rd());

    vector<Individual> partecipants;

    for(int i=0; i<num_partecipants; i++){
        int pos = uid(dre);
        auto it = find(partecipants.begin(), partecipants.end(), population[pos]);

        if(it == partecipants.end()) {
            partecipants.push_back(population[pos]);
        }
    }

    sort(partecipants.begin(), partecipants.end(), descending_order);
    return partecipants[0];
}

Individual random_parent_selection(const vector<Individual>& population){
    srand(std::time(0));

    return population[rand()%population.size()];
}

vector<int> crossover_VETT(vector<int> v1, vector<int> v2){
    /**
     * Implementazione di numeri random mega figa con libreria <random> !!
     * Potrebbe però essere necessaria una classe apposita
     * per non ripetere la creazione della random machine e della distribuzione ogni volta...
     * */
    random_device rd;
    uniform_int_distribution uid(1, 19);
    default_random_engine dre(rd());
    int cuts = uid(dre);
    set<int> set_cut;

    for (int i = 0; i < cuts; ++i) {
        set_cut.insert(uid(dre));
    }

    vector<int> vett_cut(set_cut.begin(), set_cut.end());

    std::vector<int> newVector;

    if(uid(dre) % 2){
        for (int i = 0; i <= vett_cut.size(); ++i) {
            if(i==0) {
                newVector.insert(newVector.end(), v1.begin(), v1.begin() + vett_cut.front());
            }else if(i==vett_cut.size()) {
                if(i%2) {
                    newVector.insert(newVector.end(), v2.begin() + vett_cut.back(), v2.end());
                }else{
                    newVector.insert(newVector.end(), v1.begin() + vett_cut.back(), v1.end());
                }
            }else if(i%2) {
                newVector.insert(newVector.end(), v2.begin() + vett_cut[i-1], v2.begin() + vett_cut[i]);
            }else{
                newVector.insert(newVector.end(), v1.begin() + vett_cut[i-1], v1.begin() + vett_cut[i]);
            }
        }
    }else{

        for (int i = 0; i <= vett_cut.size(); ++i) {
            if(i==0){
                newVector.insert(newVector.end(), v2.begin(), v2.begin() + vett_cut.front());
            }else if(i==vett_cut.size()){
                if(i%2) {
                    newVector.insert(newVector.end(), v1.begin() + vett_cut.back(), v1.end());
                }else {
                    newVector.insert(newVector.end(), v2.begin() + vett_cut.back(), v2.end());
                }
            }else if(i%2) {
                newVector.insert(newVector.end(), v2.begin() + vett_cut[i-1], v2.begin() + vett_cut[i]);
            }else{
                newVector.insert(newVector.end(), v1.begin() + vett_cut[i-1], v1.begin() + vett_cut[i]);
            }
        }
    }

    return newVector;
}
