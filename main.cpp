#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <string>
#include <chrono>
#include "Graph/Graph.h"
#include "Crossover/Crossover.h"
#include "Individual/Individual.h"
#include "fitness/Fitness.h"
#include "Era/Era.h"

int num_partitions;

using namespace std;

void printVett(vector<int> vett){
    cout << "Partition --> ";
    for (int i = 0; i < vett.size(); ++i) {
        cout << vett[i] << " ";
    }
    cout << " / / / ";
}

void printIndividual(Individual ind){

    printVett(ind.getGenotype());
    cout << "Fitness --> " << ind.getFitnessValue();
    cout << endl;
}

void printPopulation(vector<Individual> population){
    for (auto& ind: population) {
        printIndividual(ind);
    }
    cout<<endl;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Too many arguments !!" << endl;
        return 1;
    }

    num_partitions = stoi(argv[2]);

    /** Apriamo il file in modalità di lettura  */
    string nomeFile = argv[1];
    ifstream fpInput("../data/" + nomeFile);

    if (!fpInput.is_open()) {
        cout << "File '" << nomeFile << "' has not been open correctly" << endl;
        return 1;
    }

    int nodes, edges, n1, n2, w;
    string line;

    getline(fpInput, line);
    nodes = std::stoi(line);
    getline(fpInput, line);
    edges = std::stoi(line);

    /** Generazione del Grafo  */

    Graph G = Graph(nodes, edges);

    std::cout << "Graph dimension: " << nodes << " - " << edges << std::endl;

    /** LETTURA DEL FILE graph_nodes_edges.TXT */

    for(int i=0; i<nodes; i++){
        fpInput >> n1;
        fpInput >> w;
        G.setNode(n1, w);
    }
    for(int i=0; i<edges; i++){
        fpInput >> n1;
        fpInput >> n2;
        fpInput >> w;
        G.setEdge(n1, n2, w);
        G.incrementDegree(n1);
        G.incrementDegree(n2);
    }
    fpInput.close();

    G.computeAdjacencyMatrix();
    G.computeMatrixDegree();

    Graph G_normalize = G;
    G_normalize.normalize();
    //G.printAdjacencyMatrix();
    //G.printDegreeMatrix();

    /** TEST PER FUNZIONE ERA, CROSSOVER, MUTATION, FITNESS E INDIVIDUAL */
    vector<Individual> startingPopulation;

    for(int i = 0;i<20; i++){
        startingPopulation.emplace_back(Individual(num_partitions, G.num_of_nodes(), G));
    }

    // Write nodes and edges to file
    auto now = chrono::system_clock::now();
    auto now_ms = chrono::time_point_cast<chrono::milliseconds>(now);
    auto now_c = now_ms.time_since_epoch().count();

    vector<Individual> end_era = Era(startingPopulation, G, 50, 20, 40, num_partitions);

    auto now_now = chrono::system_clock::now();
    auto now_now_ms = chrono::time_point_cast<chrono::milliseconds>(now_now);
    auto now_now_c = now_now_ms.time_since_epoch().count();

    cout << now_c << endl;
    cout << now_now_c << endl;
    cout << now_now_c - now_c << endl;

    Individual bestOf = end_era[0];

    printIndividual(bestOf);

    return 0;
}
