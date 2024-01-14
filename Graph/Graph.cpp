//
// Created by Lorenzo Ugoccioni on 23/12/2023.
//

#include "Graph.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <thread>
#include <mutex>
#include <errno.h>
#include <string.h>

using namespace std;

bool Graph::readFileSequential(string filename) {
    auto now = chrono::system_clock::now();
    auto now_ms = chrono::time_point_cast<chrono::milliseconds>(now);
    auto now_c = now_ms.time_since_epoch().count();
    int nodes, edges, n1, n2, w;
    string line;
    ifstream fpInput(filename);

    if (!fpInput.is_open()) {
        cout << filename << " : " << strerror(errno) << endl;
        return false;
    }

    getline(fpInput, line);
    cout << cin.gcount();
    nodes = std::stoi(line);
    getline(fpInput, line);
    edges = std::stoi(line);

    setSizeNodes(nodes);
    setSizeEdges(edges);

    for(int i=0; i<nodes; i++){
        fpInput >> n1;
        fpInput >> w;
        setNode(n1, w);
    }

    for(int i=0; i<edges; i++){
        fpInput >> n1;
        fpInput >> n2;
        fpInput >> w;
        setEdge(n1, n2, w);
        incrementDegree(n1);
        incrementDegree(n2);
    }
    fpInput.close();

    auto now_now = chrono::system_clock::now();
    auto now_now_ms = chrono::time_point_cast<chrono::milliseconds>(now_now);
    auto now_now_c = now_now_ms.time_since_epoch().count();

    cout << "Graph read sequential from file '" << filename << "'" << endl;
    cout << now_now_c - now_c << " ms" << endl;
    cout << (float)((float)(now_now_c - now_c)/1000) << setprecision(3) << " seconds" << endl;

    return true;
}

void Graph::readBinNodes(string filename, int start, int n) {
    int node,weight;
    ifstream fin(filename, ios::binary);
    fin.seekg(start+2*4);
    
    std::unique_lock nlock{mnodes, std::defer_lock};
    for (int i=0;i<n;i++) {
        fin.read((char*)(&node), 4);
        fin.read((char*)(&weight), 4);;
        nlock.lock();
        setNode(node,weight);
        nlock.unlock();
    }
    fin.close();
}

void Graph::readBinEdges(string filename, const int start, const int n) {
    int n1,n2,weight;
    ifstream fin(filename, ios::binary);
    fin.seekg(start+2*4); // 2 initial ints num nodes and num edges 

    std::unique_lock elock{medges, std::defer_lock};
    for (int i=0;i<n;i++) {
        fin.read((char*)(&n1), 4);
        fin.read((char*)(&n2), 4);
        fin.read((char*)(&weight), 4);
        elock.lock();
        setEdge(n1,n2,weight);
        elock.unlock();
    }
    fin.close();
}

bool Graph::readFileParallel(string filename, int numthreads){
    auto now = chrono::system_clock::now();
    auto now_ms = chrono::time_point_cast<chrono::milliseconds>(now);
    auto now_c = now_ms.time_since_epoch().count();

    vector<thread> threads;
    int numedges, numnodes;
    ifstream fpInput(filename, ios::binary);
    if (!fpInput.is_open()) {
        cout << filename << " : " << strerror(errno) << endl;
        return false;
    }

    fpInput.read((char*)(&numnodes), 4);
    fpInput.read((char*)(&numedges), 4);
    setSizeNodes(numnodes);
    setSizeEdges(numedges);

    int numtnodes = numthreads/2;
    int numtedges = numthreads - numtnodes;

    int numnodesread = numnodes/numtnodes; 
    int numnodesread_diff = numnodes%numtnodes;
    int numedgesread = numedges/numtedges;
    int numedgesread_diff = numedges%numtedges;

    int i, nprev=0, eprev = numnodes*2*4, ntoread, etoread;
    for(i=0;i<numthreads/2;i++) {
        ntoread = numnodesread;
        etoread = numedgesread;
        if (numnodesread_diff > 0) { // i.e if numNodes or numThreadNodes is is odd, i.e. if numnodes/numtnodes is not integer 
            ntoread++;
            numnodesread_diff--;
        }
        if (numedgesread_diff > 0) {
            etoread++;
            numedgesread_diff--;
        }
        threads.emplace_back(&Graph::readBinNodes, this, filename, nprev, ntoread);
        threads.emplace_back(&Graph::readBinEdges, this, filename, eprev, etoread);
        nprev += ntoread*2*4;
        eprev += etoread*3*4;
    }
    if (numtedges > numtnodes) { //i.e. numthreads is odd
        etoread = numedges - (eprev-(numnodes*2*4))/(3*4);
        threads.emplace_back(&Graph::readBinEdges, this, filename, eprev, etoread);
    }
    for (auto& t: threads)
        t.join();

    auto now_now = chrono::system_clock::now();
    auto now_now_ms = chrono::time_point_cast<chrono::milliseconds>(now_now);
    auto now_now_c = now_now_ms.time_since_epoch().count();

    cout << "Graph read parallel from file '" << filename << "'" << endl;
    cout << now_now_c - now_c << " ms" << endl;
    cout << (float)((float)(now_now_c - now_c)/1000) << setprecision(3) << " seconds" << endl;

    return true;
}

void Graph::setNode(int n, int weight) {
    Node value;
    value.weight = weight;
    value.degree = 0;
    Nodes.insert({ n, value });
}

void Graph::setEdge(int n1, int n2, int weight) {
    Edge value;
    value.n1 = n1;
    value.n2 = n2;
    value.weight = weight;
    Edges.push_back(value);
}

void Graph::computeAdjacencyMatrix() {
    // Clear the existing adjacency matrix
    MatAdj.clear();

    // Resize the adjacency matrix to match the number of nodes
    MatAdj.resize(sizeN, std::vector<std::vector<int>>(sizeN, std::vector<int>(2, 0)));

    // Iterate over the edges and update the adjacency matrix
    for (const Edge& edge : Edges) {
        // std::cout << "Edge: " << edge.n1 << " " << edge.n2 << " " << edge.weight << std::endl;
        int n1 = edge.n1;
        int n2 = edge.n2;
        int weight = edge.weight;

        // Set the link in the adjacency matrix
        MatAdj[n1][n2][0] = 1;
        MatAdj[n2][n1][0] = 1;

        // Set the weight in the adjacency matrix
        MatAdj[n1][n2][1] = weight;
        MatAdj[n2][n1][1] = weight;
    }
}

void Graph::computeMatrixDegree() {
    MatDegree.resize(sizeN, std::vector<int>(sizeN, (0)));
    for (int i = 0; i < sizeN; i++) {
        MatDegree[i][i] = Nodes.find(i)->second.degree;
    }
}

void Graph::incrementDegree(int idNode) {
    auto it = Nodes.find(idNode);
    it->second.degree++;
}

void Graph::printNodes() {
    std::cout << "Nodes: " << std::endl;
    for (auto& n : Nodes) {
        std::cout << n.first << " " << n.second.weight << std::endl;
    }
}

void Graph::printEdges() {
    std::cout << "Edges:" << std::endl;
    for (auto& edge : Edges) {
        std::cout << edge.n1 << " " << edge.n2 << " " << edge.weight << std::endl;
    }
}

void Graph::printAdjacencyMatrix() {
    std::cout << "Adjacency matrix:" << std::endl;
    for (auto& row : MatAdj) {
        for (auto& col : row) {
            std::cout << col[0] << "-" << col[1] << " ";
        }
        std::cout << std::endl;
    }
}

void Graph::printDegreeMatrix() {
    std::cout << "Degree matrix:" << std::endl;
    for (auto& row : MatDegree) {
        for (auto& col : row) {
            std::cout << col << " ";
        }
        std::cout << std::endl;
    }
}

void Graph::printGraph() const {
    std::cout << "Number of nodes: " << sizeN << std::endl;
    std::cout << "Number of edges: " << sizeE << std::endl;

    // Print the list of nodes with their weights
    std::cout << "List of nodes and their weights:" << std::endl;
    for (const auto& [id, node] : Nodes) {
        std::cout << "Node " << id << ", weight: " << node.weight;
        std::cout << std::endl;
    }

    // Print the list of edges with their weights
    std::cout << "List of edges and their weights:" << std::endl;
    for (const auto& edge : Edges) {
        std::cout << "Edge between Node " << edge.n1 << " and Node " << edge.n2 << ", weight: " << edge.weight << std::endl;
    }
}

int Graph::getTotalEdgesWeight() {
    int totalWeight = 0;
    for (const Edge& edge : Edges) {
        totalWeight += edge.weight;
    }
    return totalWeight;
}

int Graph::getTotalNodesWeight() {
    int totalWeight = 0;
    for (const auto& [id, node] : Nodes) {
        totalWeight += node.weight;
    }
    return totalWeight;
}

void Graph::normalize(){
    int min, max;

    for(int i=0; i < (int)(this->Edges.size()); i++){
        if (i == 0){
            min = this->Edges[i].weight;
            max = this->Edges[i].weight;
        }else{
            if(min > this->Edges[i].weight){
                min = this->Edges[i].weight;
            }
            if(max < this->Edges[i].weight){
                max = this->Edges[i].weight;
            }
        }
    }

    for(int i=0; i < (int)(this->Edges.size()); i++) {
        this->Edges[i].weight = (this->Edges[i].weight - min)/(max-min);
    }

    for (const auto& node : this->Nodes) {
        if (node.first == 0){
            min = node.second.weight;
            max = node.second.weight;
        }else{
            if(node.second.weight < min){
                min = node.second.weight;
            }
            if(node.second.weight > max){
                max = node.second.weight;
            }
        }
    }

    for (auto& node : this->Nodes) {
        node.second.weight = (node.second.weight - min)/(max-min);
    }



    }
