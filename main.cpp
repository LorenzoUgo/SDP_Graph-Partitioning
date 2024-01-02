#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <string>
#include "Graph/Graph.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "Too many arguments !!" << endl;
        return 1;
    }
    string nomeFile = argv[1];
    // Apriamo il file in modalità di lettura
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

    Graph G = Graph(nodes, edges);

    std::cout << "Graph dimension: " << nodes << " - " << edges << std::endl;

    //LETTURA DEL FILE graph_nodes_edges.TXT
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
    G.printAdjacencyMatrix();
    G.printDegreeMatrix();

    return 0;
}

float fitness(){}
