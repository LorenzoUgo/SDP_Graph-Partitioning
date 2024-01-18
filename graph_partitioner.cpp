//
// Created by Lorenzo Ugoccioni on 15/01/2024.
//

#include "Graph/Graph.h"
#include "GeneticAlgorithm/GA.h"
#include "Utility/utility.h"

#include <string>
#include <sstream>
#include <chrono>
#include <sys/resource.h>

using namespace std;

void set__param(int num_param, char* params[], GeneticAlgorithm& GA, int& type_reading, int& num_thread) {

    const char *const short_opts = "l:kabc:d:e:f:g:h:i:j:";    //""abc:d:e:f:g:h:i:j:
    const option long_opts[] = {
            {"binary",        required_argument, nullptr, 'l'},
            //{"metis",        required_argument, nullptr, 'm'},
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
                GA.setDynamic(true);
                break;
            case 'b':
                std::cout << "Opzione -parallel settata " << optarg << std::endl;
                GA.setParallel(true);
                break;
            case 'c':
                std::cout << "Opzione -part con argomento: " << optarg << std::endl;
                GA.setNumPartitions(stoi(params[6]));
                break;
            case 'd':
                std::cout << "Opzione -population con argomento: " << optarg << std::endl;
                GA.setPopulationSize(stoi(params[7]));
                break;
            case 'e':
                std::cout << "Opzione -gen con argomento: " << optarg << std::endl;
                GA.setNumGenerations(stoi(params[8]));
                break;
            case 'f':
                std::cout << "Opzione -era con argomento: " << optarg << std::endl;
                if (GA.isDynamic()) {
                    GA.setErasNoUpgrade(stoi(params[9]));
                } else {
                    GA.setNumEras(stoi(params[9]));
                }
                break;
            case 'g':
                std::cout << "Opzione -new con argomento: " << optarg << std::endl;
                GA.setNumOffspring(stoi(params[10]));
                break;
            case 'h':
                std::cout << "Opzione -isl con argomento: " << optarg << std::endl;
                GA.setNumIslands(stoi(params[11]));
                break;
            case 'i':
                std::cout << "Opzione -mig con argomento: " << optarg << std::endl;
                GA.setNumMigrants(stoi(params[12]));
                break;
            case 'j':
                std::cout << "Opzione -lr con argomento: " << optarg << std::endl;
                if (GA.isDynamic()) {
                    GA.setLearningRate(stof(params[13])/100);
                }
                break;
            case 'k':
                std::cout << "Opzione -bal settata " << optarg << std::endl;
                GA.setBalanced(true);
                break;
            case 'l':
                std::cout << "Opzione -binary settata " << optarg << std::endl;
                num_thread = stoi(params[2]);
                if(num_thread > 1)
                    type_reading = 2;
                else
                    type_reading = 1;
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
    int num_threads;
    Graph G;
    GeneticAlgorithm GA;

    /** SETTING ALGORITHM PARAMETERS  */
    cout << "--> Setting graph parameters ..." << endl;
    set__param(argc, argv);


    /** Apriamo il file in modalità di lettura  */
    int t_start = time_now();

    if(! read_input(string(argv[1]), G, type_reading, num_threads))
        return 1;

    int t_end = time_now();

    cout << "    Execution time -->" << time_conversion(t_end - t_start) << endl;
    infos.executionTimes.push_back(t_end - t_start);
    infos.totalEdgesWeight = G.getTotalEdgesWeight();
    infos.totalNodesWeight = G.getTotalNodesWeight();

    /**     STARTING GENETIC ALGORITHM  */
    cout << "--> Starting the search for the partition ..." << endl;
    getrusage(RUSAGE_SELF, &_use);
    t_start = time_now();

    GA.run(G);

    t_end = time_now();

    cout << "    Partition founded!" << endl;
    cout << "    Execution time -->" << time_conversion(t_end - t_start) << endl;
    infos.executionTimes.push_back(t_end - t_start);
    GA.getBestOf().printIndividual();

    infos.partition = GA.getBestOf().getGenotype();

    infos.cutSize = cut_size(GA.getBestOf().getGenotype(), G);
    infos.balanceIndex = balance_index(GA.getNumPartitions(), GA.getBestOf().getGenotype(), G);
    infos.balanceIndexPartitions = calculatePartitionsWeight(GA.getNumPartitions(), GA.getBestOf().getGenotype(), G);
    infos.cutSizeBetweenPartitions = calculateCutSizeBetweenPartitions(G, GA.getBestOf().getGenotype());

    /**     SAVE RESULTS TO FILE    */
    //     TO BE FINISHED......
    // Vorrei salvare più partizioni nello stesso file fatte in momenti diversi. Possibile?
    getrusage(RUSAGE_SELF, &infos.usage);
    double duration = (t_start - t_end) / 1000.0;
    double cpu = (infos.usage.ru_utime.tv_sec - _use.ru_utime.tv_sec) + (infos.usage.ru_utime.tv_usec - _use.ru_utime.tv_usec) / 1000000.0;
    infos.cpu_percentage = 100.0 * cpu / duration;
    cout << "CPU percentage used: " << infos.cpu_percentage << "%" << endl;
    cout << "Memory usage: " << infos.usage.ru_maxrss / (1024.0 * 1024.0) << " GBs" << endl;



    string outputFile = string(argv[1]) + "_GA_"
            + (GA.isBalanced() ?"Balanced_":"")
            + (GA.isParallel() ?"Parallel_":"")
            + to_string(GA.getNumPartitions()) + "_"
            + (GA.isParallel() ? to_string(GA.getNumIslands()):"");
    saveInfoToFile(infos);


    return 0;
}