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

int main(int argc, char** argv) {
    //unsigned int numThreads = std::thread::hardware_concurrency();
    if (argc < 5) {
        cout << "Too few arguments !!" << endl;
        return 1;
    }
    PartitionData partData;
    struct rusage usage1;

    /** Apriamo il file in modalità di lettura  */
    Graph G;
    string fileTxt = "../data/" + string(argv[1]) + "/standard_text.txt";
    string fileBinary = "../data/" + string(argv[1]) + "/standard_binary.bin";

    cout << "--> File Reading and Graph generation ..." << endl;
    int t_start = time_now();
    // get graph read file return value bool
    if (G.readFileSequentialTxt(fileTxt)) //"./data/graph_20_20/standard_text.txt"
        cout << "    Graph read from text file successfully !!" << endl;
    else
        return 1;

    /* get graph read binary file return value bool
    if (G.readFileSequentialBin(fileBinary)) //"./data/graph_20_20/standard_text.txt"
        cout << "Graph read from binary file successfully" << endl;
    else
        return 1;

     get graph read binary file in parallel, return value bool
    if (G.readFileParallel(fileBinary, 8)) //"./data/graph_20_20/standard_text.txt"
        cout << "Graph read from binary file successfully" << endl;
    else
        return 1;*/

    /*cout<< "Generazione delle strutture interne del grafo: Matrice di Adiacenza + Grado dei nodi. Tempo impiegato -->";
    Graph G_normalize = G;
    G_normalize.normalize();
    G.printAdjacencyMatrix();
    G.printDegreeMatrix();*/

    int t_end = time_now();
    cout << "    Execution time -->" << time_conversion(t_end - t_start) << endl;
    partData.executionTimes.push_back(t_end - t_start);
    partData.totalEdgesWeight = G.getTotalEdgesWeight();    // --> ??
    partData.totalNodesWeight = G.getTotalNodesWeight();    // --> ??

    /** SETTING ALGORITHM PARAMETERS  */
    cout << "--> Setting graph parameters ..." << endl;
    GeneticAlgorithm GA;
    GA.set__param(argc, argv);

    /**     STARTING GENETIC ALGORITHM  */
    cout << "--> Starting the search for the partition ..." << endl;
    getrusage(RUSAGE_SELF, &usage1);
    t_start = time_now();

    GA.run(G);

    t_end = time_now();
    cout << "    Partition founded!" << endl;
    cout << "    Execution time -->" << time_conversion(t_end - t_start) << endl;
    partData.executionTimes.push_back(t_end - t_start);
    GA.getBestOf().printIndividual();

    partData.partition = GA.getBestOf().getGenotype();

    partData.cutSize = cut_size(GA.getBestOf().getGenotype(), G);
    partData.balanceIndex = balance_index(GA.getNumPartitions(), GA.getBestOf().getGenotype(), G);
    partData.balanceFactor = balanceFactor(GA.getNumPartitions(), GA.getBestOf().getGenotype(), G);
    partData.balanceIndexPartitions = calculatePartitionsWeight(GA.getNumPartitions(), GA.getBestOf().getGenotype(), G);
    partData.cutSizeBetweenPartitions = calculateCutSizeBetweenPartitions(G, GA.getBestOf().getGenotype());
    partData.cutSizeAverage = avgCutSize(partData.cutSizeBetweenPartitions);
    partData.balanceIndexAverage = avgBalanceFactor(partData.balanceIndexPartitions);

    /**     METIS COMPARISON    */

    //metis_comparison(argv[0], G);

    /**     SAVE RESULTS TO FILE    */
    //     TO BE FINISHED......
    // Vorrei salvare più partizioni nello stesso file fatte in momenti diversi. Possibile?
    getrusage(RUSAGE_SELF, &partData.usage);
    double duration = (t_start - t_end) / 1000.0;
    double cpu = (partData.usage.ru_utime.tv_sec - usage1.ru_utime.tv_sec) + (partData.usage.ru_utime.tv_usec - usage1.ru_utime.tv_usec) / 1000000.0;
    partData.cpu_percentage = 100.0 * cpu / duration;
    std::cout << "CPU percentage used: " << partData.cpu_percentage << "%" << std::endl;
    std::cout << "Memory usage: " << partData.usage.ru_maxrss / (1024.0 * 1024.0) << " GBs" << std::endl;



    string outputFile = string(argv[1]) + "_GA_"
            + (GA.isBalanced() ?"Balanced_":"")
            + (GA.isParallel() ?"Parallel_":"")
            + to_string(GA.getNumPartitions()) + "_"
            + (GA.isParallel() ? to_string(GA.getNumPartitions()) + "_":"");
    savePartitionDataToFile(partData);


    return 0;
}