//
// Created by Lorenzo Ugoccioni on 14/01/2024.
//

#include "utility.h"

bool descending_order(const Individual& I1, const Individual& I2){
    return I1.getFitnessValue() > I2.getFitnessValue();
}

bool ascending_order(const Individual& I1, const Individual& I2){
    return I1.getFitnessValue() < I2.getFitnessValue();
}

// calcola la dimensione del taglio sommando i pesi degli archi che vanno da una partizione ad un'altra
float cut_size(vector<int> genotype, const Graph& G){
    float cut_size = 0;
    for (auto edge : G.getEdges()){
        if(genotype[edge.n1] != genotype[edge.n2])
            cut_size += edge.weight;
    }
    return cut_size;
}

// calcola la differenza tra la partizione con il maggior peso e quella con il minor peso dei nodi al suo interno
float balance_index(int num_partitions, const vector<int>& genotype, const Graph& G){
    vector<float> partitions_weight(num_partitions);

    for(auto it = genotype.begin();  it < genotype.end();  it++){
        partitions_weight[*it] += G.getNodeWeight(distance(genotype.begin(), it));
    }

    float max_partition_weight = *max_element(partitions_weight.begin(), partitions_weight.end());
    float min_partition_weight = *min_element(partitions_weight.begin(), partitions_weight.end());

    return max_partition_weight - min_partition_weight;
}

// calcola il fitness value come combinazione lineare di dimensione taglio e bilanciamento peso partizioni
float fitness(int num_partitions, const vector<int>& genotype, const Graph& G, float cut_size_weight , float balance_index_weight){
    return (cut_size_weight*cut_size(genotype, G)) + (balance_index_weight*balance_index(num_partitions, genotype, G));
}

int readGraph(const string& file, Graph& G){
    string path = "../data/" + file;
    int t_start = time_now();

    // get graph read file return value bool
    if (G.readFileSequentialTxt(path)) //"./data/graph_20_20/standard_text.txt"
        cout << "Graph read from file successfully" << endl;
    else
        return 1;

    /*cout<< "Generazione delle strutture interne del grafo: Matrice di Adiacenza + Grado dei nodi. Tempo impiegato -->";

    G.computeAdjacencyMatrix();
    G.computeMatrixDegree();

    Graph G_normalize = G;
    G_normalize.normalize();
    G.printAdjacencyMatrix();
    G.printDegreeMatrix();*/

    int t_end = time_now();
    time_conversion(t_end - t_start);

    return 1;
}

int time_now(){
    auto t = chrono::system_clock::now();
    auto t_ms = chrono::time_point_cast<chrono::milliseconds>(t);
    auto t_c = t_ms.time_since_epoch().count();
    return (int) t_c;
}

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

void time_conversion(int delta_t){
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

    cout<< "Employed Time --> " << string_to_print <<endl;
}

void metis_comparison(const string& file_metis, const Graph& G, GeneticAlgorithm& GA){
    // apertura file metis, lettura risultato
    cout << fitness(GA.getNumPartitions(), {2,1,2,0,2,0,0,1,2,0,0,2,1,1,0,2,1,2,1,1}, G);
}

void set_GA_parameters(GeneticAlgorithm GA, int argc, char* argv[]) {

    const char *const short_opts = "abc:d:e:f:g:h:i:j:";
    const option long_opts[] = {
            {"mod",        required_argument, nullptr, 'a'},
            {"parallel",   required_argument, nullptr, 'b'},
            {"part",       required_argument, nullptr, 'c'},
            {"population", required_argument, nullptr, 'd'},
            {"gen",        required_argument, nullptr, 'e'},
            {"era",        required_argument, nullptr, 'f'},
            {"new",        required_argument, nullptr, 'g'},
            {"isl",        required_argument, nullptr, 'h'},
            {"mig",        required_argument, nullptr, 'i'},
            {"lr",         required_argument, nullptr, 'j'},
            {nullptr,      0,                 nullptr, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, short_opts, long_opts, nullptr)) != -1) {
        switch (opt) {
            case 'a':
                std::cout << "Opzione -mod settata " << optarg << std::endl;
                GA.setDynamic(true);
                break;
            case 'b':
                std::cout << "Opzione -parallel settata " << optarg << std::endl;
                GA.setParallel(true);
                break;
            case 'c':
                std::cout << "Opzione -part con argomento: " << optarg << std::endl;
                GA.setNumPartitions(stoi(argv[4]));
                break;
            case 'd':
                std::cout << "Opzione -population con argomento: " << optarg << std::endl;
                GA.setPopulationSize(stoi(argv[5]));
                break;
            case 'e':
                std::cout << "Opzione -gen con argomento: " << optarg << std::endl;
                GA.setNumGenerations(stoi(argv[6]));
                break;
            case 'f':
                std::cout << "Opzione -era con argomento: " << optarg << std::endl;
                if (GA.isDynamic()) {
                    GA.setErasNoUpgrade(stoi(argv[7]));
                } else {
                    GA.setNumEras(stoi(argv[7]));
                }
                break;
            case 'g':
                std::cout << "Opzione -new con argomento: " << optarg << std::endl;
                GA.setNumOffspring(stoi(argv[8]));
                break;
            case 'h':
                std::cout << "Opzione -isl con argomento: " << optarg << std::endl;
                GA.setNumIslands(stoi(argv[9]));
                break;
            case 'i':
                std::cout << "Opzione -mig con argomento: " << optarg << std::endl;
                GA.setNumMigrants(stoi(argv[10]));
                break;
            case 'j':
                std::cout << "Opzione -lr con argomento: " << optarg << std::endl;
                if (GA.isDynamic()) {
                    GA.setLearningRate(stof(argv[11]));
                }
                break;
            default:
                std::cerr << "Opzione non valida." << std::endl;
        }

    }
}
