#include <vector>
#include <string>
#include "Graph/Graph.h"
#include <chrono>
#include "Crossover/Crossover.h"
#include "Individual/Individual.h"
#include "fitness/Fitness.h"
#include "Era/Era.h"
#include <thread>

using namespace std;

struct GA_parameters{
    int NUM_PARTITIONS = 0;
    int NUM_OFFSPRING = 15;
    int NUM_GENERATIONS = 30;
    int POPULATION_SIZE = 20;
    int NUM_ISLANDS = 3;
    int NUM_ERAS = 5;
    int NUM_MIGRANTS = 2;
} GA_parameters;

void printVett(const vector<int>& vett){
    cout << "Partition --> ";
    for (int i : vett) {
        cout << i << " ";
    }
    cout << " / / / ";
}

void printIndividual(const Individual& ind){

    printVett(ind.getGenotype());
    cout << "Fitness --> " << ind.getFitnessValue();
    cout << endl;
}

void printPopulation(const vector<Individual>& population){
    for (auto& ind: population) {
        printIndividual(ind);
    }
    cout<<endl;
}

void time_conversion(auto delta_t){
    int millisec, sec, min, h, day, year;
    string string_to_print;

    millisec = delta_t%1000;
    delta_t /= 1000;

    string_to_print.insert(0, to_string(millisec)+" Milliseconds. ");
    if(delta_t > 0){
        sec = delta_t % 60;
        delta_t /= 60;
        string_to_print.insert(0, to_string(sec)+" Seconds, ");

        if(delta_t > 0){
            min = delta_t%60;
            delta_t /= 60;
            string_to_print.insert(0, to_string(min)+" Minutes, ");
        }

        if(delta_t > 0){
            h = delta_t%24;
            delta_t /= 24;
            string_to_print.insert(0, to_string(h)+" Hours, ");
        }

        if(delta_t > 0){
            day = delta_t%365;
            delta_t /= 365;
            string_to_print.insert(0, to_string(day)+" Day, ");

            if(delta_t > 0){
                year = delta_t;
                string_to_print.insert(0, to_string(year)+" Years, ");
            }
        }
    }

    cout << string_to_print <<endl;
}


int main(int argc, char** argv) {

    //unsigned int numThreads = std::thread::hardware_concurrency();

    if (argc != 3) {
        cout << "Too many arguments !!" << endl;
        return 1;
    }

    // get graph read file return value bool
    Graph G;
    if (G.readFileSequential("./data/graph_20_20/standard_text.txt"))
        cout << "Graph read from file successfully" << endl;
    else 
        return 1;

    /*cout<< "Generazione delle strutture interne del grafo: Matrice di Adiacenza + Grado dei nodi. Tempo impiegato -->";

    G.computeAdjacencyMatrix();
    G.computeMatrixDegree();

    Graph G_normalize = G;
    G_normalize.normalize();
    G.printAdjacencyMatrix();
    G.printDegreeMatrix();

    now_now = chrono::system_clock::now();
    now_now_ms = chrono::time_point_cast<chrono::milliseconds>(now_now);
    now_now_c = now_now_ms.time_since_epoch().count();

    time_conversion(now_now_c - now_c);
    now_c = now_now_c;*/

    /**     TEST PER FUNZIONE ERA, CROSSOVER, MUTATION, FITNESS E INDIVIDUAL    */
    GA_parameters.NUM_PARTITIONS = stoi(argv[2]);


    //map<int, vector<Individual>> GalapagosPopulation;
    vector<Individual> startingPopulation;
    cout<< "Creazione delle Isole ("<< GA_parameters.NUM_ISLANDS <<" isole) e della loro popolazione("<< GA_parameters.POPULATION_SIZE << " individui). Tempo impiegato -->";

    /*for (int is = 0; is<GA_parameters.NUM_ISLANDS; is++) {    /** Si potrà parallelizzare anche la creazione delle popolazioni iniziali delle varie isole?
        cout << "Starting Population for Island n_" << is << endl;
        for (int in = 0; in < GA_parameters.POPULATION_SIZE; in++) {
            GalapagosPopulation[is].emplace_back(Individual(NUM_PARTITIONS, G.num_of_nodes(), G));
        }
    }*/
    for (int i = 0; i < GA_parameters.POPULATION_SIZE; i++) {
        startingPopulation.emplace_back(Individual(GA_parameters.NUM_PARTITIONS, G.num_of_nodes(), G));
    }
    /**     STARTING GALAPAGOS    */

    cout<< "Inizio computazione della partizione dl grafo... \n GA con parametri:\n-> " << GA_parameters.NUM_OFFSPRING << " nuovi individui ad ogni generazione.\n->" << GA_parameters.NUM_GENERATIONS <<" generazioni per ogni era.\n->"<< GA_parameters.NUM_MIGRANTS <<" individui da un scambiare tra le isole alla fine di ogni era.\n->"<< GA_parameters.NUM_ERAS <<" ere definite prima di terminare.\n\n" << endl;

    /*cout<< "Creazione delle Isole ("<< NUM_ISLANDS <<" isole) e della loro popolazione("<< POPULATION_SIZE << individui"). Tempo impiegato -->";
    Individual bestOfGalapagos = Galapagos_fixed(GalapagosPopulation, G, NUM_ERAS, NUM_GENERATIONS, NUM_OFFSPRING, POPULATION_SIZE, NUM_PARTITIONS, NUM_MIGRANTS);

     Eras(startingPopulation, G, GA_parameters.NUM_GENERATIONS, GA_parameters.NUM_OFFSPRING, GA_parameters.POPULATION_SIZE, GA_parameters.NUM_PARTITIONS);
    */

    cout << fitness(GA_parameters.NUM_PARTITIONS, {2,1,2,0,2,0,0,1,2,0,0,2,1,1,0,2,1,2,1,1}, G);

    /*now_now = chrono::system_clock::now();
    now_now_ms = chrono::time_point_cast<chrono::milliseconds>(now_now);
    now_now_c = now_now_ms.time_since_epoch().count();
    cout<< "Tempo impiegato -->";
    time_conversion(now_now_c - now_c);

    Individual bestOf = startingPopulation[0];
    printIndividual(bestOf);

    printIndividual(bestOfGalapagos);*/

    return 0;
}

