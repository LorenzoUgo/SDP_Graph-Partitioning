//
// Created by Gregorio Nicora on 13/01/24.
//

#include <vector>
#include <iostream>
#include <thread>
#include <barrier>
#include <mutex>

using namespace std;

struct GA_parameters{
    int NUM_PARTITIONS = 0;
    int NUM_OFFSPRING = 4;
    int NUM_GENERATIONS = 4;
    int POPULATION_SIZE = 4;
    int NUM_ISLANDS = 4;
    int NUM_ERAS = 5;
    int NUM_MIGRANTS = 2;
} GA_parameters;

mutex printMutex;


void Eras_parallel(int thread_id, barrier<>& b1, barrier<>& b2) {
    printMutex.lock();
    cout << " THREAD " << thread_id  << endl;
    printMutex.unlock();
    for(int e = 0; e<GA_parameters.NUM_ERAS; e++){

        for (int g = 0; g < GA_parameters.NUM_GENERATIONS; g++) {
            //cout << " THREAD " << thread_id  << " Starting Generation n_" << g << endl;

        }

        b1.arrive_and_wait();
        printMutex.lock();
        cout << " THREAD " << thread_id  << " arrivato a barrier b2 in era " << e << endl;
        printMutex.unlock();
        // WAIT ...

        b2.arrive_and_wait();
    }
}



void Galapagos_parallel(){

    vector<std::thread> Islands;

    barrier<> barrier_1_cpp(GA_parameters.NUM_ISLANDS + 1 );
    barrier<> barrier_2_cpp(GA_parameters.NUM_ISLANDS + 1 );


    for(int i = 0; i<GA_parameters.NUM_ISLANDS; ++i) {
        //cout << "Starting Isola n_" << i << endl;
        Islands.emplace_back( [=, &barrier_1_cpp, &barrier_2_cpp] {Eras_parallel(i, barrier_1_cpp, barrier_2_cpp);});
    }

    for(int e = 0; e<GA_parameters.NUM_ERAS; e++){

        barrier_1_cpp.arrive_and_wait();
        printMutex.lock();
        cout << "Sono il master!! GIRO: " << e << endl;
        printMutex.unlock();
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

