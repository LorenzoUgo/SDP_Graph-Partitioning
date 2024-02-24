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

bool Graph::readFileSequentialBin(string filename) {
    int nodes, edges;
    string line;
    ifstream fpInput(filename);

    if (!fpInput.is_open()) {
        cout << filename << " : SEQUENTIAL BIN " << strerror(errno) << endl;
        return false;
    }

    fpInput.read((char*)(&nodes), 4);
    fpInput.read((char*)(&edges), 4);
    setSizeNodes(nodes);
    setSizeEdges(edges);

    int n1, n2, weight, node;
    for (int i=0;i<nodes;i++) {
        fpInput.read((char*)(&node), 4);
        fpInput.read((char*)(&weight), 4);;
        setNode(node,weight);
    }
    for (int i=0;i<edges;i++) {
        fpInput.read((char*)(&n1), 4);
        fpInput.read((char*)(&n2), 4);
        fpInput.read((char*)(&weight), 4);
        setEdge(n1,n2,weight);
    }
    return true;
}

bool Graph::readFileSequentialTxt(string filename) {
    int nodes, edges, n1, n2, w;
    string line;
    ifstream fpInput(filename);

    if (!fpInput.is_open()) {
        cout << filename << " : SEQUENTIAL TXT " << strerror(errno) << endl;
        return false;
    }

    fpInput >> nodes;
    fpInput >> edges;
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
    }
    fpInput.close();
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
    vector<thread> threads;
    int numedges, numnodes;
    ifstream fpInput(filename, ios::binary);
    if (!fpInput.is_open()) {
        cout << filename << " PARALLEL: " << strerror(errno) << endl;
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
