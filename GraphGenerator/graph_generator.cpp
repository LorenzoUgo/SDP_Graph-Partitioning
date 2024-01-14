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
#include <tuple>
#include <iomanip>
#include <sstream>
#include <errno.h>
#include <string.h>
#include <algorithm>

//#define DEBUG
#define MAX_NODE_WEIGHT 10
#define MAX_EDGE_WEIGHT 10

using namespace std;

int main(int argc, char** argv) {
    srand(time(nullptr));

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
    const int num_nodes = 10;
    const int num_edges = 15;
    #endif

    auto now = chrono::system_clock::now();
    auto now_ms = chrono::time_point_cast<chrono::milliseconds>(now);
    auto now_c = now_ms.time_since_epoch().count();

    // Generate nodes with random weights
    vector<pair<int, int>> nodes;
    for (int i = 0; i < num_nodes; i++) {
        nodes.push_back({ i, rand() % MAX_NODE_WEIGHT + 1 });
    }

    // Generate edges with random weights
    vector<tuple<int, int, int>> edges;
    for (int i = 0; i < num_edges; i++) {
        int u = rand() % num_nodes;
        int v = rand() % num_nodes;
        #ifdef DEBUG
        cout << "Edge n." << i << " generated" << endl;
        #endif
        while (u == v || std::find_if(edges.begin(), edges.end(), [&](auto e) { return get<0>(e) == u && get<1>(e) == v; }) != edges.end()) {
            u = rand() % num_nodes;
            v = rand() % num_nodes;
        }
        edges.push_back({ u, v, rand() % MAX_EDGE_WEIGHT + 1 });
    }

    // Write num nodes and edges to files
    ostringstream filename, filename_bin, filename_metis;
    string path = "../data/graph_" + to_string(num_nodes) + '_' + to_string(num_edges) + '/';
    std::filesystem::create_directory(path, std::filesystem::current_path());
    filename << path;
    filename_bin << path;
    filename_metis << path;

    filename << "standard_text.txt";
    ofstream outfile(filename.str());
    if (outfile.is_open())
        cout << filename.str() << " is open" << endl;
    else {
        cout << filename.str() << " : " << strerror(errno) << endl;
        return 1;
    }
    outfile << num_nodes << endl;
    outfile << num_edges << endl;

    filename_bin << "standard_binary.bin";
    ofstream outfile_bin(filename_bin.str(), ios::binary);
    if (outfile_bin.is_open())
        cout << filename_bin.str() << " is open" << endl;
    else {
        cout << filename_bin.str() << " : " << strerror(errno) << endl;
        return 1;
    }
    outfile_bin.write((char*)(&num_nodes),sizeof(int));
    outfile_bin.write((char*)(&num_edges),sizeof(int));

    filename_metis << "metis_text.txt";
    ofstream outfile_metis(filename_metis.str());
    if (outfile_metis.is_open())
        cout << filename_metis.str() << " is open" << endl;
    else {
        cout << filename_metis.str() << " : " << strerror(errno) << endl;
        return 1;
    }
    outfile_metis << num_nodes << " " << num_edges << " 011" << endl;


    vector<stringstream> metis_nodes(num_nodes);
    for (auto node : nodes) {
        outfile << node.first << " " << node.second << endl;
        outfile_bin.write((char*)(&node.first), sizeof(int));
        outfile_bin.write((char*)(&node.second), sizeof(int));
        metis_nodes.at(node.first) << node.second;
    }
    // NOTE in metis format nodes go from 1 to num_nodes, instead of 0 to num_nodes-1
    for (auto edge: edges) {
        int u = std::get<0>(edge), v = std::get<1>(edge), w = std::get<0>(edge);
        outfile << u << " " << v << " " << w << endl;
        outfile_bin.write((char*)(&u), sizeof(int));
        outfile_bin.write((char*)(&v), sizeof(int));
        outfile_bin.write((char*)(&w), sizeof(int));
        metis_nodes.at(u) << " " << (v+1) << " " << w;
        metis_nodes.at(v) << " " << (u+1) << " " << w;
    }
    for (int i=0;i<num_nodes;i++)
        outfile_metis << metis_nodes.at(i).str() << endl;

    outfile.close();
    outfile_bin.close();
    outfile_metis.close();

    auto now_now = chrono::system_clock::now();
    auto now_now_ms = chrono::time_point_cast<chrono::milliseconds>(now_now);
    auto now_now_c = now_now_ms.time_since_epoch().count();

    cout << "Graph files generated and saved to path: " << path << endl;
    cout << now_now_c - now_c << " ms" << endl;
    cout << (float)((float)(now_now_c - now_c)/1000) << setprecision(3) << " seconds" << endl;
    cout << (float)((float)(now_now_c - now_c)/(1000*60)) << setprecision(3) << " minutes" << endl;

    return 0;
}