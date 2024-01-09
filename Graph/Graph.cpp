//
// Created by Lorenzo Ugoccioni on 23/12/2023.
//

#include "Graph.h"
#include <iostream>
#include <algorithm>

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

    for(int i=0; i < this->Edges.size(); i++){
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

    for(int i=0; i < this->Edges.size(); i++) {
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
