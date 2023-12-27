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
    value.coarse = nullptr;
    Nodes.insert({ n, value });
}

void Graph::setNode(int n, int weight, Coarse* coarse) {
    Node value;
    value.weight = weight;
    value.degree = 0;
    value.coarse = new Coarse;
    value.coarse->n1 = coarse->n1;
    value.coarse->n2 = coarse->n2;
    value.coarse->weight1 = coarse->weight1;
    value.coarse->weight2 = coarse->weight2;
    value.coarse->adj = coarse->adj;
    Nodes.insert({ n, value });
}

void Graph::removeNode(int nodeId) {
    // Remove the node from the Nodes map
    Nodes.erase(nodeId);

    // Remove all edges connected to the node
    Edges.erase(std::remove_if(Edges.begin(), Edges.end(), [nodeId](const Edge& edge) {
        return edge.n1 == nodeId || edge.n2 == nodeId;
    }), Edges.end());

    // Remove the node from the adjacency list of all other nodes
    for (auto& nodePair : Nodes) {
        int id = nodePair.first;
        std::vector<std::vector<std::vector<int>>>& adjList = nodePair.second.coarse->adj;
        adjList.erase(std::remove_if(adjList.begin(), adjList.end(), [nodeId](const std::vector<std::vector<int>>& edge) {
            return edge[0][0] == nodeId || edge[1][0] == nodeId;
        }), adjList.end());
    }
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
        if (node.coarse != nullptr) {
            std::cout << ", Coarse n1: " << node.coarse->n1 << ", Coarse n2: " << node.coarse->n2;
        }
        std::cout << std::endl;
    }

    // Print the list of edges with their weights
    std::cout << "List of edges and their weights:" << std::endl;
    for (const auto& edge : Edges) {
        std::cout << "Edge between Node " << edge.n1 << " and Node " << edge.n2 << ", weight: " << edge.weight << std::endl;
    }
}

// Function to find the ID of a node given two IDs from the Coarse struct
int Graph::findNodeIdByCoarseIds(int n1, int n2) {
    for (const auto& [id, node] : Nodes) {
        if (node.coarse != nullptr && ((node.coarse->n1 == n1 && node.coarse->n2 == n2) ||
                                       (node.coarse->n1 == n2 && node.coarse->n2 == n1))) {
            return id;
        }
    }
    return -1; // Return -1 if the node with the given Coarse IDs is not found
}

// Function to find the ID of a node given one ID from the Coarse struct
int Graph::findNodeIdByCoarseSingleId(int n) {
    for (const auto& [id, node] : Nodes) {
        if (node.coarse != nullptr && (node.coarse->n1 == n || node.coarse->n2 == n)) {
            return id;
        }
    }
    return -1; // Return -1 if the node with the given Coarse IDs is not found
}

std::pair<int, int> Graph::getCoarseIdsById(int nodeId) {
    auto it = Nodes.find(nodeId);
    if (it != Nodes.end() && it->second.coarse != nullptr) {
        return { it->second.coarse->n1, it->second.coarse->n2 };
    }
    return { -1, -1 }; // Return {-1, -1} if the node doesn't have valid coarse IDs.
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