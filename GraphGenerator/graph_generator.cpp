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

using namespace std;

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Usage: ./graph_generator <num_nodes> <num_edges>" << endl;
        return 1;
    }
    int num_nodes = std::stoi(argv[1]);
    int num_edges = std::stoi(argv[2]);

    // Generate nodes with random weights
    vector<pair<int, int>> nodes(num_nodes);

    for (int i = 0; i < num_nodes; i++) {
        nodes[i] = { i, rand() % 10 + 1 };
    }

    // Generate edges with random weights
    vector<tuple<int, int, int>> edges;

    for (int i = 0; i < num_edges; i++) {
        int u = rand() % num_nodes;
        int v = rand() % num_nodes;
        while (u == v || find_if(edges.begin(), edges.end(), [&](auto e) { return get<0>(e) == u && get<1>(e) == v; }) != edges.end()) {
            u = rand() % num_nodes;
            v = rand() % num_nodes;
        }
        edges.push_back({ u, v, rand() % 10 + 1 });
    }

    // Write nodes and edges to file
    auto now = chrono::system_clock::now();
    auto now_ms = chrono::time_point_cast<chrono::milliseconds>(now);
    auto now_c = now_ms.time_since_epoch().count();

    ostringstream filename;
    filename << "graph_" << num_nodes << "_" << num_edges << ".txt";
    ofstream outfile("../data/" + filename.str());
    outfile << num_nodes << endl;
    outfile << num_edges << endl;

    for (auto node : nodes) {
        outfile << node.first << " " << node.second << endl;
    }
    for (auto edge : edges) {
        outfile << get<0>(edge) << " " << get<1>(edge) << " " << get<2>(edge) << endl;
    }
    outfile.close();

    auto now_now = chrono::system_clock::now();
    auto now_now_ms = chrono::time_point_cast<chrono::milliseconds>(now_now);
    auto now_now_c = now_now_ms.time_since_epoch().count();

    std::cout << "Graph generated and saved to file: ./data/" << filename.str() << std::endl;

    std::cout << now_c << std::endl;
    std::cout << now_now_c << std::endl;
    std::cout << now_now_c - now_c << std::endl;

    return 0;
}