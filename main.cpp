#include <iostream>
#include <vector>

using namespace std;

//Data Structures
//Nodo di una lista
typedef struct node *link;
struct node{
    int v;
    int wt;
    link *next;
};

//Grafo come lista di adiacenza
struct graph_List_Adj{
    int V;
    int E;
    vector<link> lAdj;
    link z;
};

//Grafo come lista di adiacenza
struct graph_Matrix_Adj{
    int V;
    int E;
    vector<vector<int>> mAdj;
};


int main() {
    int V, E;
    graph_List_Adj GLA;
    graph_Matrix_Adj GMA;

    std::cout << "Define the graph dimension!" << std::endl;
    cin>> V ;
    cin>> E;


    return 0;
}
