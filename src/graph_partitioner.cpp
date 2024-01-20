//
// Created by Lorenzo Ugoccioni on 15/01/2024.
//

#include "Graph/Graph.h"
#include "GeneticAlgorithm/GA.h"
#include "Utility/utility.h"

#include <string>
#include <chrono>
#include <sys/resource.h>
#include <iostream>

using namespace std;

void set_param(int num_param, char* params[], GeneticAlgorithm& GA, int& type_reading, int& num_thread, string& metisFile, bool& compare_metis) {

    const char *const short_opts = "m:l:kabc:d:e:f:g:h:i:j:";    //""abc:d:e:f:g:h:i:j:
    const option long_opts[] = {
            {"mod",        no_argument, nullptr, 'a'},
            {"parallel",   no_argument, nullptr, 'b'},
            {"part",       required_argument, nullptr, 'c'},
            {"population", required_argument, nullptr, 'd'},
            {"gen",        required_argument, nullptr, 'e'},
            {"era",        required_argument, nullptr, 'f'},
            {"new",        required_argument, nullptr, 'g'},
            {"isl",        required_argument, nullptr, 'h'},
            {"mig",        required_argument, nullptr, 'i'},
            {"lr",         required_argument, nullptr, 'j'},
            {"bal",        no_argument, nullptr, 'k'},
            {"binary",        required_argument, nullptr, 'l'},
            {"compare",        required_argument, nullptr, 'm'},
            {nullptr,      0,                 nullptr, 0}
    };

    int opt;
    while ((opt = getopt_long(num_param, params, short_opts, long_opts, nullptr)) != -1) {
        switch (opt) {
            case 'a':
                cout << "Opzione -mod settata " << endl;
                GA.setDynamic(true);
                break;
            case 'b':
                cout << "Opzione -parallel settata " << endl;
                GA.setParallel(true);
                break;
            case 'c':
                cout << "Opzione -part con argomento: " << optarg << endl;
                GA.setNumPartitions(stoi(optarg));
                break;
            case 'd':
                cout << "Opzione -population con argomento: " << optarg << endl;
                GA.setPopulationSize(stoi(optarg));
                break;
            case 'e':
                cout << "Opzione -gen con argomento: " << optarg << endl;
                GA.setNumGenerations(stoi(optarg));
                break;
            case 'f':
                std::cout << "Opzione -era con argomento: " << optarg << endl;
                if (GA.isDynamic()) {
                    GA.setErasNoUpgrade(stoi(optarg));
                } else {
                    GA.setNumEras(stoi(optarg));
                }
                break;
            case 'g':
                cout << "Opzione -new con argomento: " << optarg << endl;
                GA.setNumOffspring(stoi(optarg));
                break;
            case 'h':
                cout << "Opzione -isl con argomento: " << optarg << endl;
                GA.setNumIslands(stoi(optarg));
                break;
            case 'i':
                cout << "Opzione -mig con argomento: " << optarg << endl;
                GA.setNumMigrants(stoi(optarg));
                break;
            case 'j':
                cout << "Opzione -lr con argomento: " << optarg << endl;
                if (GA.isDynamic()) {
                    GA.setLearningRate(stof(optarg)/100);
                }
                break;
            case 'k':
                cout << "Opzione -bal settata " << endl;
                GA.setBalanced(true);
                break;
            case 'l':
                cout << "Opzione -binary settata: " << optarg << endl;
                num_thread = stoi(optarg);
                if(num_thread > 1)
                    type_reading = 2;
                else
                    type_reading = 1;
                break;
            case 'm':
                cout << "Opzione -compare settata " << endl;
                compare_metis = true;
                metisFile = string(optarg);
                break;
            default:
                std::cerr << "Opzione non valida." << std::endl;
        }

    }
}


int main(int argc, char** argv) {
    //unsigned int numThreads = std::thread::hardware_concurrency();
    if (argc < 2) {
        cout << "Too few arguments !!" << endl;
        return 1;
    }

    UsageInfo infos;
    struct rusage _use;

    int type_reading=0;
    int num_threads=0;
    bool compare_metis = false;
    string metisFile;
    Graph G;
    GeneticAlgorithm GA;

    /** SETTING ALGORITHM PARAMETERS  */
    cout << "--> Setting graph parameters ..." << endl;
    set_param(argc, argv, GA, type_reading, num_threads, metisFile, compare_metis);
    if(compare_metis){
        compute_metis(metisFile);
        return 0;
    }

    int t_start = time_now();

    if(! read_input(string(argv[1]), G, type_reading, num_threads))
        return 1;

    int t_end = time_now();

    cout << "Execution time -->" << time_conversion(t_end - t_start) << endl;
    infos.executionTimes.push_back(t_end - t_start);
    infos.totalEdgesWeight = G.getTotalEdgesWeight();
    infos.totalNodesWeight = G.getTotalNodesWeight();

    /**     STARTING GENETIC ALGORITHM  */
    cout << "--> Starting algorithm ..." << endl;
    getrusage(RUSAGE_SELF, &_use);
    t_start = time_now();
    GA.run(G);
    t_end = time_now();

    cout << "Execution time -->" << time_conversion(t_end - t_start) << endl;
    infos.executionTimes.push_back(t_end - t_start);
    GA.getBestOf().printIndividual();

    infos.partition = GA.getBestOf().getGenotype();

    infos.cutSize = cut_size(GA.getBestOf().getGenotype(), G);
    infos.balanceIndex = balance_index(GA.getNumPartitions(), GA.getBestOf().getGenotype(), G);
    infos.balanceIndexPartitions = calculatePartitionsWeight(GA.getNumPartitions(), GA.getBestOf().getGenotype(), G);
    //infos.cutSizeBetweenPartitions = calculateCutSizeBetweenPartitions(G, GA.getBestOf().getGenotype());

    /**     SAVE RESULTS TO FILE    */

    cout << "Test usage" << endl;
    getrusage(RUSAGE_SELF, &(infos.usage));
    double duration = (t_start - t_end) / 1000.0;
    double cpu = (infos.usage.ru_utime.tv_sec - _use.ru_utime.tv_sec) + (infos.usage.ru_utime.tv_usec - _use.ru_utime.tv_usec) / 1000000.0;
    infos.cpu_percentage = 100 * cpu / duration;
    cout << "CPU percentage used: " << infos.cpu_percentage << "%" << endl;
    cout << "Memory usage: " << infos.usage.ru_maxrss / (1024.0 * 1024.0) << " GBs" << endl;

    infos.fileName = string(argv[1]) + "_GA_"
            + (GA.isBalanced() ?"Balanced_":"")
            + (GA.isParallel() ?"Parallel_":"")
            + to_string(GA.getNumPartitions()) + "_"
            + (GA.isParallel() ? to_string(GA.getNumIslands()):"");
    saveInfoToFile(infos);

    return 0;
}