//
// Created by Gregorio Nicora on 27/12/23.
//

#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <tuple>
#include <map>

#define DEBUG
#define MAX_EDGE_WEIGHT 10

using namespace std;

int main(int argc, char** argv) {
    srand(time(NULL));

    #ifndef DEBUG
    if (argc != 3) {
        cout << "Usage: ./graph_generator <num_nodes> <num_edges>" << endl;
        return 1;
    }
    int num_nodes = std::stoi(argv[1]);
    int num_edges = std::stoi(argv[2]);
    #endif
    // DEBUG
    #ifdef DEBUG
    int num_nodes = 5;
    int num_edges = 6;
    #endif

    // Generate nodes with random weights
    vector<pair<int, int>> nodes(num_nodes);

    for (int i = 0; i < num_nodes; i++) {
        nodes[i] = { i, rand() % 10 + 1 };
    }

    // Generate edges with random weights
    map<pair<int, int>, int> edges;
    int madj[num_nodes][num_nodes] = {};

    for (int i = 0; i < num_edges; i++) {
        int u = rand() % num_nodes;
        int v = rand() % num_nodes;
        while (u == v || madj[u][v] != 0) {
            u = rand() % num_nodes;
            v = rand() % num_nodes;
        }
        int w = rand() % MAX_EDGE_WEIGHT + 1;
        madj[u][v] = w;
        madj[v][u] = w;
    }

    while(edges.size() < num_edges) {
        int u = (rand()*rand()) % num_nodes;
        int v = (rand()*rand()) % num_nodes;

        edges[{u, v}] = rand() % 10 + 1 ;
        cout <<"Arco: " << edges.size() << endl;
    }

    // Write nodes and edges to file
    auto now = chrono::system_clock::now();
    auto now_ms = chrono::time_point_cast<chrono::milliseconds>(now);
    auto now_c = now_ms.time_since_epoch().count();

    ostringstream filename, filename_metis;
    filename << "graph_" << num_nodes << "_" << num_edges << ".txt";
    filename_metis << "graph_" << num_nodes << "_" << num_edges << "_metis.txt";
    ofstream outfile("../data/" + filename.str());
    ofstream outfile_metis("../data/METIS/" + filename_metis.str());
    outfile << num_nodes << endl;
    outfile << num_edges << endl;
    outfile_metis << num_nodes << " " << num_edges << " 011" << endl;
    /* from LSB(right) to MSB(left)
     * 0: nodes have no 'size' (1 node has 1 weight value, no 'multi-constrained partition')
     * 1: nodes have specified weight
     * 1: edges have specified weight
     *
     * source: METIS manual
     */

    // default nodes format
    for (auto node : nodes) {
        outfile << node.first << " " << node.second << endl;
    }


    // default edges format + metis format (nodes go from 1 to num_nodes, instead of 0 to num_nodes-1)
    for(int i=0; i<num_nodes;i++) {
        outfile_metis << nodes.at(i).second;
        for (int j = 0; j < num_nodes; j++) {
            if (j <=
                i) {// full line, prints same edge twice (undirected graph, matrix in simmetric), needed for metis format
                if (madj[i][j] != 0)
                    outfile_metis << " " << j + 1 << " " << madj[i][j];
            } else// j > i, upper diagonal only, prints edges only once
            if (madj[i][j] != 0) {
                outfile_metis << " " << j + 1 << " " << madj[i][j];
                outfile << i << " " << j << " " << madj[i][j] << endl;
            }
        }
        outfile_metis << endl;
    }

    for (auto edge : edges) {
        outfile << edge.first.first << " " << edge.first.second << " " << edge.second << endl;
    }

    outfile_metis.close();
    outfile.close();

    auto now_now = chrono::system_clock::now();
    auto now_now_ms = chrono::time_point_cast<chrono::milliseconds>(now_now);
    auto now_now_c = now_now_ms.time_since_epoch().count();

    std::cout << "Graph file generated and saved to file: ./data/" << filename.str() << std::endl;
    std::cout << "Graph file(metis format) generated and saved to file: ./data/METIS/" << filename_metis.str() << std::endl;

    std::cout << now_c << std::endl;
    std::cout << now_now_c << std::endl;
    std::cout << now_now_c - now_c << std::endl;

    return 0;
}