//
// Created by Gregorio Nicora on 27/12/23.
//

#include <filesystem>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <errno.h>
#include <string.h>

//#define DEBUG
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

    auto now = chrono::system_clock::now();
    auto now_ms = chrono::time_point_cast<chrono::milliseconds>(now);
    auto now_c = now_ms.time_since_epoch().count();

    // Generate nodes with random weights
    vector<pair<int, int>> nodes(num_nodes);
    for (int i = 0; i < num_nodes; i++) {
        nodes[i] = { i, rand() % 10 + 1 };
    }

    // Generate edges with random weights
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


    // default nodes format
    for (auto node : nodes) {
        outfile << node.first << " " << node.second << endl;
        outfile_bin.write((char*)(&node.first), sizeof(int));
        outfile_bin.write((char*)(&node.second), sizeof(int));
    }
    // default edges format + metis format (nodes go from 1 to num_nodes, instead of 0 to num_nodes-1)
    for(int i=0; i<num_nodes;i++) {
        outfile_metis << nodes.at(i).second;
        for (int j=0; j<num_nodes; j++) {
            if (j<=i) {// full line, prints same edge twice (undirected graph, matrix in simmetric), needed for metis format
                if (madj[i][j] != 0)
                    outfile_metis << " " << j+1 << " " << madj[i][j];
            }
            else// j > i, upper diagonal only, prints edges only once
                if (madj[i][j] != 0) {
                    outfile_metis << " " << j+1 << " " << madj[i][j];
                    outfile << i << " " << j << " " << madj[i][j] << endl;
                    outfile_bin.write((char*)(&i), sizeof(int));
                    outfile_bin.write((char*)(&j), sizeof(int));
                    outfile_bin.write((char*)(&madj[i][j]), sizeof(int));
                }
        }
        outfile_metis << endl;
    }
    outfile.close();
    outfile_bin.close();
    outfile_metis.close();

    auto now_now = chrono::system_clock::now();
    auto now_now_ms = chrono::time_point_cast<chrono::milliseconds>(now_now);
    auto now_now_c = now_now_ms.time_since_epoch().count();

    cout << "Graph files generated and saved to path: " << path << endl;
    cout << now_now_c - now_c << " ms" << endl;
    cout << (now_now_c - now_c)/1000 << setprecision(5) << " seconds" << endl;

    return 0;
}