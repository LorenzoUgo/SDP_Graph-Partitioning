//
// Created by Lorenzo Ugoccioni on 15/01/2024.
//

#include "Graph/Graph.h"
#include "GeneticAlgorithm/GA.h"
#include "Utility/utility.h"

#include <sstream>
#include <string>
#include <chrono>
#include <sys/resource.h>
#include <iostream>

using namespace std;

bool set_param(int num_param, char* params[], GeneticAlgorithm& GA, int& type_reading, int& num_thread, string& metisFile, bool& compare_metis) {

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
                cout << "-mod option set " << endl;
                GA.setDynamic(true);
                break;
            case 'b':
                cout << "-parallel option set " << endl;
                GA.setParallel(true);
                break;
            case 'c':
                cout << "-part option with arg: " << optarg << endl;
                GA.setNumPartitions(stoi(optarg));
                break;
            case 'd':
                cout << "-population option with arg: " << optarg << endl;
                GA.setPopulationSize(stoi(optarg));
                break;
            case 'e':
                cout << "-gen option with arg: " << optarg << endl;
                GA.setNumGenerations(stoi(optarg));
                break;
            case 'f':
                std::cout << "-era option with arg: " << optarg << endl;
                if (GA.isDynamic()) {
                    GA.setErasNoUpgrade(stoi(optarg));
                } else {
                    GA.setNumEras(stoi(optarg));
                }
                break;
            case 'g':
                cout << "-new option with arg: " << optarg << endl;
                GA.setNumOffspring(stoi(optarg));
                break;
            case 'h':
                cout << "-isl option with arg: " << optarg << endl;
                GA.setNumIslands(stoi(optarg));
                break;
            case 'i':
                cout << "-mig option with arg: " << optarg << endl;
                GA.setNumMigrants(stoi(optarg));
                if (GA.getNumMigrants() > GA.getPopulationSize()) {
                    cout << "Number of migrants (" << GA.getNumMigrants() << ") must be less or equal to population size (" << GA.getPopulationSize() << endl;
                    return false;
                }
                break;
            case 'j':
                cout << "-lr option with arg: " << optarg << endl;
                if (GA.isDynamic()) {
                    GA.setLearningRate(stof(optarg)/100);
                }
                break;
            case 'k':
                cout << "-bal option set" << endl;
                GA.setBalanced(true);
                break;
            case 'l':
                cout << "-binary option with arg: " << optarg << endl;
                num_thread = stoi(optarg);
                if(num_thread > 1)
                    type_reading = 2;
                else
                    type_reading = 1;
                break;
            case 'm':
                cout << "-compare option set " << endl;
                compare_metis = true;
                metisFile = string(optarg);
                break;
            default:
                std::cerr << "Option not valid" << std::endl;
                return false;
        }

    }
    return true;
}


int main(int argc, char** argv) {
    //unsigned int numThreads = std::thread::hardware_concurrency();
    if (argc < 2) {
        cout << "Missing parameters" << endl << "Usage: ./graph_partitioner <filename> [options]" << endl;
        cout << "README.md for usage info" << endl;
        return 1;
    }

    UsageInfo infos;
    struct rusage _use;

    int type_reading=0;
    int num_threads=0;
    bool compare_metis = false;
    string metisFile, filename = string(argv[1]);
    Graph G, G_norm;
    GeneticAlgorithm GA;

    /** SETTING ALGORITHM PARAMETERS  */
    //cout << "--> Setting graph parameters ..." << endl;

    if(!set_param(argc, argv, GA, type_reading, num_threads, metisFile, compare_metis))
        return 1;
    if(compare_metis){
        compute_metis(metisFile);
        return 0;
    }

    int t_start = time_now();
    if(! read_input(filename, G, type_reading, num_threads))
        return 1;
    int t_end = time_now();

    // TRY TO FIX
    if(!read_input(filename, G_norm, 2, 8))
        return 1;

    cout << "Graph read from file" << endl << time_conversion(t_end - t_start) << endl;
    infos.executionTimes.push_back(t_end - t_start);
    infos.totalEdgesWeight = G.getTotalEdgesWeight();
    infos.totalNodesWeight = G.getTotalNodesWeight();

    /**     STARTING GENETIC ALGORITHM  */
    //cout << "--> Starting algorithm ..." << endl;
    getrusage(RUSAGE_SELF, &_use);
    t_start = time_now();
    G_norm.normalize();
    GA.run(G);
    cout << "Fitness value: " << GA.getBestOf().getFitness() << endl;
    t_end = time_now();
    cout << endl;
    cout << GA.printParameters().str();
    infos.parameters = GA.printParameters();

    cout << endl << "Execution time: " << time_conversion(t_end - t_start) << endl;
    infos.executionTimes.push_back(t_end - t_start);

    infos.partition = GA.getBestOf().getGenotype();

    infos.cutSize = cut_size(GA.getBestOf().getGenotype(), G);
    infos.balanceIndex = balance_index(GA.getNumPartitions(), GA.getBestOf().getGenotype(), G);
    infos.balanceIndexPartitions = calculatePartitionsWeight(GA.getNumPartitions(), GA.getBestOf().getGenotype(), G);
    //infos.cutSizeBetweenPartitions = calculateCutSizeBetweenPartitions(G, GA.getBestOf().getGenotype());
    infos.fitness = GA.getBestOf().getFitness();

    /**     SAVE RESULTS TO FILE    */
    getrusage(RUSAGE_SELF, &(infos.usage));
    /*
    double duration = (t_start - t_end) / 1000.0;
    double cpu = (infos.usage.ru_utime.tv_sec - _use.ru_utime.tv_sec) + (infos.usage.ru_utime.tv_usec - _use.ru_utime.tv_usec) / 1000000.0;
    infos.cpu_percentage = 100 * cpu / duration;
    cout << "CPU usage: " << infos.cpu_percentage << "%" << endl;
    */
    cout << "Memory usage: " << infos.usage.ru_maxrss / (1024.0 * 1024.0) << " GBs" << endl;

    infos.fileName = filename + "gapart"
            + (GA.isBalanced() ?".balanced":"")
            + (GA.isParallel() ?".parallel":"")
            + "." + to_string(GA.getNumPartitions())
            + (GA.isParallel() ? "." + to_string(GA.getNumIslands()):"")
            + ".txt";
    saveInfoToFile(infos);

    return 0;
}