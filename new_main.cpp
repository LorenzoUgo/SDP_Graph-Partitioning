//
// Created by Lorenzo Ugoccioni on 15/01/2024.
//

#include <string>
#include <sstream>

#include "Graph/Graph.h"
#include "GeneticAlgorithm/GA.h"
#include "Utility/utility.h"

using namespace std;

int main(int argc, char** argv) {
    //unsigned int numThreads = std::thread::hardware_concurrency();
    if (argc < 5) {
        cout << "Too few arguments !!" << endl;
        return 1;
    }

    /** Apriamo il file in modalità di lettura  */
    Graph G;
    string path = "../data/" + string(argv[1]);

    int t_start = time_now();
    // get graph read file return value bool
    if (G.readFileSequentialTxt(path)) //"./data/graph_20_20/standard_text.txt"
        cout << "Graph read from file successfully" << endl;
    else
        return 1;
    /*cout<< "Generazione delle strutture interne del grafo: Matrice di Adiacenza + Grado dei nodi. Tempo impiegato -->";
    Graph G_normalize = G;
    G_normalize.normalize();
    G.printAdjacencyMatrix();
    G.printDegreeMatrix();*/
    int t_end = time_now();

    time_conversion(t_end - t_start);


    /** SETTING ALGORITHM PARAMETERS  */

    GeneticAlgorithm GA;

    GA.set__param(argc, argv);

    /**     STARTING GENETIC ALGORITHM  */

    t_start = time_now();

    GA.run(G);

    t_end = time_now();

    GA.BestOfGalapagos().printIndividual();

    time_conversion(t_end - t_start);


    /**     METIS COMPARISON    */

    //metis_comparison(argv[0], G);

    /**     SAVE RESULTS TO FILE    */
    //     TO BE FINISHED
    // Vorrei salvare più partizioni nello stesso file fatte in momenti diversi. Possibile?

    struct PartitionData partData;
    size_t ultimoslash = path.find_last_of('/');
    path.erase(ultimoslash);

    string outputFile = path.substr(path.find_last_of('/')+1, path.length() - path.find_last_of('/')-1) + "_GA_"
            + (GA.isBalanced() ?"Balanced_":"")
            + (GA.isParallel() ?"Parallel_":"")
            + to_string(GA.getNumPartitions()) + "_"
            + (GA.isParallel() ? to_string(GA.getNumPartitions()) + "_":"");
    savePartitionDataToFile(partData);

    // ......

    return 0;
}