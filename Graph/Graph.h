//
// Created by Lorenzo Ugoccioni on 23/12/2023.
//
#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <map>
#include <list>

using namespace std;

typedef struct {// Node
    int w; //node's weight
    int degree;
}Node;

typedef struct {// Edge
    int n1, n2; //nodes
    float w;    //edge's weight
}Edge;


class Graph {
private:
    vector<Edge> Edges;
    map<int, Node> Nodes;
    vector<vector<int>> mAdj;
    list<list<Node>> lAdj;
    int sizeN;
    int sizeE;
public:

};


#endif //SDP_GRAPH_PARTITIONING_GRAPH_H
