//
// Created by Gregorio Nicora on 13/01/24.
//

#include <vector>
#include <iostream>
#include <thread>
#include <barrier>

using namespace std;

struct GA_parameters{
    int NUM_PARTITIONS = 0;
    int NUM_OFFSPRING = 15;
    int NUM_GENERATIONS = 30;
    int POPULATION_SIZE = 20;
    int NUM_ISLANDS = 4;
    int NUM_ERAS = 5;
    int NUM_MIGRANTS = 2;
} GA_parameters;


void Eras_parallel(int thread_id, barrier<>& b1, barrier<>& b2) {

    for(int e = 0; e<GA_parameters.NUM_ERAS; e++){

        for (int g = 0; g < GA_parameters.NUM_GENERATIONS; g++) {
            cout << "Starting Generation n_" << g << endl;

            for (int i = 0; i < GA_parameters.NUM_OFFSPRING; i++) {

                cout << " THREAD " << thread_id  << " era " << e << " generation " << g << " offspring " << i << endl;

            }

        }

        b2.arrive_and_wait();
        // WAIT ...
        b1.arrive_and_wait();

    }
}



void Galapagos_parallel(){

    vector<thread> Islands;

    barrier barrier_1_cpp(GA_parameters.NUM_ISLANDS);
    barrier barrier_2_cpp(GA_parameters.NUM_ISLANDS);


    for(int i = 0; i<GA_parameters.NUM_ISLANDS; i++) {
        cout << "Starting Isola n_" << i << endl;
        Islands.emplace_back( Eras_parallel, i, barrier_1_cpp, barrier_2_cpp);

    }

    for(int e = 1; e<GA_parameters.NUM_ERAS; e++){

        barrier_1_cpp.arrive_and_wait();

        cout << "Sono il master!! \n" << endl;

        barrier_2_cpp.arrive_and_wait();

    }

    for(auto& t: Islands) {
        t.join();
    }


    return;
}






int main(int argc, char** argv) {


    Galapagos_parallel();



    return 0;
}

