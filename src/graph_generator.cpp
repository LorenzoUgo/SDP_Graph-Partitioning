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
#include <map>

#define MAX_NODE_WEIGHT 10
#define MAX_EDGE_WEIGHT 10

using namespace std;

int main(int argc, char** argv) {
    srand(time(nullptr));

    if (argc != 3) {
        cout << "Usage: ./graph_generator <num_nodes> <num_edges>" << endl;
        return 1;
    }
    int num_nodes = std::stoi(argv[1]);
    unsigned int num_edges = std::stoi(argv[2]);

    auto now = chrono::system_clock::now();
    auto now_ms = chrono::time_point_cast<chrono::milliseconds>(now);
    auto now_c = now_ms.time_since_epoch().count();

    cout << "Generating nodes" << endl;
    vector<pair<int, int>> nodes;
    for (int i = 0; i < num_nodes; i++) {
        nodes.push_back({ i, rand() % MAX_NODE_WEIGHT + 1 });
        //cout <<"Node n." << i << " generated" << endl; // DEBUG
    }

    cout << "Generating edes" << endl;
    map<pair<int, int>, int> edges;
    while(edges.size() < num_edges) {
        int u = rand() % num_nodes;
        int v = rand() % num_nodes;
        if (edges.find({u, v}) == edges.end())
            edges[{u, v}] = (rand() % MAX_EDGE_WEIGHT) + 1 ;
        //cout <<"Edge n." << edges.size() << " generated" << endl; //DEBUG
    }

    cout << "Writing files, this may take a while" << endl;
    ostringstream filename, filename_bin, filename_metis;
    filesystem::create_directories("./data/");
    string path = "./data/graph_" + to_string(num_nodes) + '_' + to_string(num_edges) + '/';
    filesystem::create_directory(path, filesystem::current_path());
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
    //int i=0;
    for (auto node : nodes) {
        outfile << node.first << " " << node.second << endl;
        outfile_bin.write((char*)(&node.first), sizeof(int));
        outfile_bin.write((char*)(&node.second), sizeof(int));
        metis_nodes.at(node.first) << node.second;
        //cout <<"Node n." << i << " written" << endl;
        //i++;
    }
    // NOTE in metis format nodes go from 1 to num_nodes, instead of 0 to num_nodes-1
    //i=0;
    for (auto edge: edges) {
        int u = edge.first.first, v = edge.first.second, w = edge.second;
        outfile << u << " " << v << " " << w << endl;
        outfile_bin.write((char*)(&u), sizeof(int));
        outfile_bin.write((char*)(&v), sizeof(int));
        outfile_bin.write((char*)(&w), sizeof(int));
        metis_nodes.at(u) << " " << (v+1) << " " << w;
        metis_nodes.at(v) << " " << (u+1) << " " << w;
        //i++;
        //cout <<"Edge n." << i << " written" << endl;
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
    cout << (float)((float)(now_now_c - now_c)/(1000*60*60)) << setprecision(3) << " hours" << endl;

    return 0;
}