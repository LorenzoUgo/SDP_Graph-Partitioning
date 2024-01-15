using namespace std;

#include "Graph/Graph.cpp"
#include <iostream>

bool areEqual(Graph &G1, Graph &G2) {
    if (G1.num_of_edges() != G2.num_of_edges()) {
        cout << "Different number of edges !" << endl;
        return false;
    }
    if (G1.num_of_nodes() != G2.num_of_nodes()) {
        cout << "Different number of nodes !" << endl;
        return false;
    }
    for (int i=0;i<G1.num_of_nodes();i++)
        if (G1.getNodes().at(i).weight != G2.getNodes().at(i).weight) {
            cout << "Nodes have different weight !" << endl;
                return false;
        }
    bool found = false;
    for (auto e1: G1.getEdges()) {
        found = false;
        for (auto e2: G2.getEdges()) {
            if (e1.n1==e2.n1 && e1.n2==e2.n2 && e1.weight==e2.weight)
                found = true;
            if (found) break;   
        }
        if (!found) {
            cout << "Edges are different !" << endl;
            return false;
        }
    }
    return true;
}

int main() {
	Graph Gseq1, Gseq2, Gpar;
    string graph = "./data/graph_10000000_30000000/";
    int numthreads = 3;

    /**     X FRANCSCO per trovare i file dove stanno i grafi da CLions   */
    /*
    if (Gseq1.readFileSequentialTxt(graph + "standard_text.txt")) {
        cout << "Graph sequential read from file successfully" << endl;
    } else {
        return 1;
    }
    */
    if (Gseq2.readFileSequentialBin(graph + "standard_binary.bin")) {
        cout << "Graph sequential read from file successfully" << endl;
    } else {
        return 1;
    }

	if (Gpar.readFileParallel(graph + "standard_binary.bin", numthreads)) {
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


    if (!areEqual(Gseq2,Gpar)) {
        cout << "Gseq edges" << endl;
        Gseq1.printEdges();
        cout << "Gpar edges" << endl;
        Gpar.printEdges();
        cout << "Gseq nodes" << endl;
        Gseq1.printNodes();
        cout << "Gpar nodes" << endl;
        Gpar.printNodes();
        cout << "Graphs are NOT EQUAL !!" << endl;
        return 1;
    }
    cout << "Graphs are EQUAL !!" << endl;

	return 0;
}