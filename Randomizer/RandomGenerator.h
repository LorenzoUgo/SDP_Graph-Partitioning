//
// Created by Lorenzo Ugoccioni on 08/01/2024.
//

#ifndef SDP_GRAPH_PARTITIONING_RANDOMGENERATOR_H
#define SDP_GRAPH_PARTITIONING_RANDOMGENERATOR_H

#include <random>

using namespace std;

class randomNumber {

private:
    unsigned int seed;
    default_random_engine DRE;
public:
    randomNumber(){
        random_device rd;
        seed = rd();

        default_random_engine dre(rd());
        DRE = dre;
    }

    int random_int_num(int a, int b);

    float random_real_num(auto a, auto b);

};

#endif //SDP_GRAPH_PARTITIONING_RANDOMGENERATOR_H