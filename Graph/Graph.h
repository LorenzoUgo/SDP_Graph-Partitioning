//
// Created by Lorenzo Ugoccioni on 23/12/2023.
//
#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <map>
#include <stack>
#include <random>
#include <unordered_set>
#include <string>
#include <mutex>

typedef struct {

    float weight;
    int degree;

} Node;

typedef struct {

    int n1;
    int n2;
    float weight;

} Edge;

class Graph {

private:
    // the id of the map is the id of the node while the node struct contains info about the node
    std::map<int, Node> Nodes;
    std::vector<Edge> Edges;
    std::vector<std::vector<std::vector<int>>> MatAdj; // inside the adjacency matrix we have a field for the link e one for the weight
    std::vector<std::vector<int>> MatDegree;
    int sizeN;
    int sizeE;
    
public:
    // Graph(int numNodes = 0, int numEdges = 0) : sizeN(numNodes), sizeE(numEdges) {} // non dovrebbe più servire
    mutex m;
    int num_of_nodes() { return sizeN; }
    int num_of_edges() { return sizeE; }
    void setSizeNodes(int value) { sizeN = value; }
    void setSizeEdges(int value) { sizeE = value; }
    int getNodeWeight(int nodeIndex) const {
        return Nodes.at(nodeIndex).weight;
    }
    double getNodeWeightAvg() const {
        int sum = 0;
        for (int i = 0; i < sizeN; i++) {
            sum += getNodeWeight(i);
        }
        return sum / sizeN;
    }
    bool readFileSequential(std::string filename);
    bool readFileParallel(std::string filename, int numthreads);

    std::map<int, Node> getNodes() { return Nodes; }
    std::vector<Edge> getEdges() { return Edges; }
    std::vector<std::vector<std::vector<int>>> getMatAdj() { return MatAdj; }
    std::vector<std::vector<int>> getMatDegree() { return MatDegree; }

    void setNode(int n, int weight);
    void setEdge(int n1, int n2, int weight);

    void computeAdjacencyMatrix();

    int getTotalEdgesWeight();
    int getTotalNodesWeight();

    void computeMatrixDegree();
    void incrementDegree(int idNode);

    void normalize();

    int returnLastID() { return Nodes.size(); }

    // Debug functions
    void printNodes();
    void printEdges();
    void printAdjacencyMatrix();
    void printDegreeMatrix();
    void printGraph() const;

    Graph& operator=(const Graph& other) {
        if (this == &other) {
            return *this; // Self-assignment check
        }

        // Copy primitive member variables
        sizeN = other.sizeN;
        sizeE = other.sizeE;

        // Copy Nodes using deep copy
        Nodes.clear(); // Clear current Nodes
        for (const auto& nodePair : other.Nodes) {
            Node newNode = nodePair.second;
            Nodes[nodePair.first] = newNode;
        }

        // Copy Edges
        Edges = other.Edges;

        // Copy MatAdj
        MatAdj = other.MatAdj;

        // Copy MatDegree
        MatDegree = other.MatDegree;

        return *this;
    }

    void readBinNodes(string filename, int start, int n);
    void readBinEdges(string filename, int start, int n);
};



#endif