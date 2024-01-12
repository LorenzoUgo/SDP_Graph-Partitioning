using namespace std;

#include "Graph/Graph.cpp"
#include <iostream>

int main() {
	Graph Gseq, Gpar;
    if (Gseq.readFileSequential("./data/graph_20_20/standard_text.txt"))
        cout << "Graph sequential read from file successfully" << endl;
    else {
        return 1;
	}
	if (Gseq.readFileSequential("./data/graph_20_20/standard_binary.bin"))
        cout << "Graph parallel read from file successfully" << endl;
    else {
        return 1;
	}
	return 0;
}