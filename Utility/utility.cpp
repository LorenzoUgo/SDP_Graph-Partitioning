//
// Created by Lorenzo Ugoccioni on 14/01/2024.
//

#include "utility.h"

int time_now(){
    auto t = chrono::system_clock::now();
    auto t_ms = chrono::time_point_cast<chrono::milliseconds>(t);
    auto t_c = t_ms.time_since_epoch().count();
    return (int) t_c;
}

void printPopulation(const vector<Individual>& population){
    for (Individual ind: population) {
        ind.printIndividual();
    }
    cout<<endl;
}

string time_conversion(int delta_t){
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


    return string_to_print;
}

void metis_comparison(const string& file_metis, const Graph& G){
    // apertura file metis, lettura risultato
    Individual metisI({2,1,2,0,2,0,0,1,2,0,0,2,1,1,0,2,1,2,1,1});
    cout << metisI.fitness(G);
}

/** - - - - - - - - - - - - - - - - - */


// balance factor of 1.0 is perfectly balanced, 0.0 or 2.0 is completely unbalanced -> max/min
float balanceFactor(int num_partitions, const vector<int>& partition, const Graph& G) {
    vector<float> partitions_weight(num_partitions);

    for(auto it = partition.begin();  it < partition.end();  it++){
        partitions_weight[*it] += G.getNodeWeight(distance(partition.begin(), it));
    }

    float max_partition_weight = *max_element(partitions_weight.begin(), partitions_weight.end());
    float min_partition_weight = *min_element(partitions_weight.begin(), partitions_weight.end());

    return max_partition_weight / min_partition_weight;
}
// Total weights of nodes inside a partition
vector<int> calculatePartitionsWeight(int num_partitions, const vector<int>& partition, const Graph& G) {
    vector<int> partitions_weight(num_partitions);

    for(auto it = partition.begin();  it < partition.end();  it++){
        partitions_weight[*it] += G.getNodeWeight(distance(partition.begin(), it));
    }

    return partitions_weight;
}
// balance factor of 1.0 is perfectly balanced, 0.0 or 2.0 is completely unbalanced -> each partitions against each other
vector<vector<int>> balanceFactorBetweenPartitions(int num_partitions, const vector<int>& partition, const Graph& G) {
    vector<vector<float>> balanceFactorPartitions(num_partitions);
    vector<float> balanceFactors(num_partitions);

    for(auto it = partition.begin();  it < partition.end();  it++){
        balanceFactors[*it] += G.getNodeWeight(distance(partition.begin(), it));
    }
    for (int i = 0; i < num_partitions; ++i) {
        for (int j = i+1; i < num_partitions; ++i) {
            balanceFactorPartitions[i][j] = balanceFactors[i] / balanceFactors[j];
            balanceFactorPartitions[j][i] = balanceFactors[j] / balanceFactors[i];

        }
    }

    return balanceFactorPartitions;
}

// Cut Size value -> Between each partitions
vector<vector<int>> calculateCutSizeBetweenPartitions(const Graph& G, const vector<int>& partition) {
    vector<vector<int>> cutSizeBetweenPartitions;

    for (auto edge : G.getEdges()){
        if(partition[edge.n1] != partition[edge.n2]) {
            cutSizeBetweenPartitions[edge.n1][edge.n2] += edge.weight;
            cutSizeBetweenPartitions[edge.n2][edge.n1] += edge.weight;
        }
    }

    return cutSizeBetweenPartitions;
}
// balance factor of 1.0 is perfectly balanced, 0.0 or 2.0 is completely unbalanced -> each partitions against each other
float avgCutSize(const vector<vector<int>>& cutSizes) {
    float sum = 0.0;
    for (int i = 0; i < cutSizes.size(); ++i) {
        for (int j = i+1; i < cutSizes.size(); ++i) {
            sum += cutSizes[i][j];
        }
    }
    return sum / cutSizes.size();
}


// Read input file and generate the graph
void read_input(const string& file, Graph* G) {
    string text = "../data/" + file + "/standard_text.txt";
    string bin = "../data/" + file + "/standard_binary.bin";

    /* get graph read file return value bool*/
    if (G->readFileSequentialTxt(text)) //"./data/graph_20_20/standard_text.txt"
        cout << "Graph read from file successfully" << endl;
    else
        return;

    // get graph read file return value bool
    if (G->readFileSequentialBin(bin)) //"./data/graph_20_20/standard_text.txt"
        cout << "Graph read from file successfully" << endl;
    else
        return;

    /*cout<< "Generazione delle strutture interne del grafo: Matrice di Adiacenza + Grado dei nodi. Tempo impiegato -->";
    Graph G_normalize = G;
    G_normalize.normalize();
    G.printAdjacencyMatrix();
    G.printDegreeMatrix();*/


}

// Save partition informations in output file
void savePartitionDataToFile(const PartitionData& partitionData) {
    ofstream outputFile(partitionData.fileName);
    if (outputFile.is_open()) {
        outputFile << "Graph reading. Execution time --> " << time_conversion(partitionData.executionTimes[0]) << endl;
        outputFile << "Total nodes weight: " << partitionData.totalNodesWeight << "\t\t";
        outputFile << "Total edges weight: " << partitionData.totalEdgesWeight << endl
        << endl;

        outputFile << "Execution time partitioning --> " << time_conversion((int) partitionData.executionTimes[1]) << endl
        << endl;
        outputFile << "Partition " << ": ";
        for (auto j : partitionData.partition) {
            outputFile << j << " ";
        }
        for (int i =0 ;i < partitionData.balanceIndexPartitions.size(); i++) {
            outputFile << "Partition "<< i << ". Internal weight: " << partitionData.balanceIndexPartitions[i] << " | ";
        }
        outputFile << endl;
        for (int i = 0 ; i < partitionData.balanceIndexPartitions.size(); i++) {
            for (int j = i+1 ; j < partitionData.balanceIndexPartitions.size(); j++) {
                outputFile << "Cut Size between " << i << " and " << i << ": "<< partitionData.cutSizeBetweenPartitions[i]
                           << " | ";
            }
        }

        outputFile << endl << endl << " Balance Index: " << partitionData.balanceIndex;
        outputFile << " | | Global Cut Size: " << partitionData.cutSize << endl;
        outputFile << endl;

        outputFile << "CPU time used: " << partitionData.usage.ru_utime.tv_sec << " seconds " << partitionData.usage.ru_utime.tv_usec << " microseconds" << endl;
        outputFile << "Memory usage: " << partitionData.usage.ru_maxrss << " KBs | " << partitionData.usage.ru_maxrss / 1024.0 << " MBs | " << partitionData.usage.ru_maxrss / (1024.0 * 1024.0) << " GBs" << endl;
        outputFile << "CPU percentage: " << partitionData.cpu_percentage << "%" << endl;
        outputFile.close();
    }
}

// balance factor of 1.0 is perfectly balanced, 0.0 or 2.0 is completely unbalanced -> each partitions against each other
float avgBalanceFactor(const vector<int>& balanceFactors) {
    float sum = 0.0;
    for (double balanceFactor : balanceFactors) {
        sum += balanceFactor;
    }
    return sum / balanceFactors.size();
}

