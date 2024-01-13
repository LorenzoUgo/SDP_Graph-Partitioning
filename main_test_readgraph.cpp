using namespace std;

#include "Graph/Graph.cpp"
#include <iostream>

int main() {
	Graph Gseq, Gpar;

    /**     X FRANCSCO per trovare i file dove stanno i grafi da CLions   */
    if (Gseq.readFileSequential("./data/graph_20_20/standard_text.txt")) {
        cout << "Graph sequential read from file successfully" << endl;
    } else {
        return 1;
    }

	if (Gseq.readFileParallel("./data/graph_20_20/standard_binary.bin", 2)) {
        cout << "Graph parallel read from file successfully" << endl;
    } else {
        return 1;
    }

    /** X GREGORIO e LORENZO per trovare i file dove stanno i grafi da CLions
    if (Gseq.readFileSequential("../data/graph_20_20/standard_text.txt")){
        cout << "Graph sequential read from file successfully" << endl;
    } else {
        return 1;

    if (Gseq.readFileParallel("../data/graph_20_20/standard_binary.bin", 2)){
        cout << "Graph parallel read from file successfully" << endl;
    } else {
        return 1;
    }
     */

    cout << "Gseq edges" << endl;
    Gseq.printEdges();
    cout << "Gpar edges" << endl;
    Gpar.printEdges();
    cout << "Gseq nodes" << endl;
    Gseq.printNodes();
    cout << "Gseq nodes" << endl;
    Gpar.printNodes();
	return 0;
}