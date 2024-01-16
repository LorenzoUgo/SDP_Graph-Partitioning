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


// balance factor of 1.0 is perfectly balanced, 0.0 or 2.0 is completely unbalanced
double calculateBalanceFactor(Graph& graph, const std::vector<bool>& partitionA) {
    double weightA = 0.0;
    double weightB = 0.0;
    for (int i = 0; i < graph.num_of_nodes(); i++) {
        if (partitionA[i]) {
            weightA += graph.getNodeWeight(i);
        }
        else {
            weightB += graph.getNodeWeight(i);
        }
    }
    return std::min(weightA, weightB) / std::max(weightA, weightB);
}

int calculatePartitionsWeight(Graph& graph, const std::vector<bool>& partitionA) {
    int weight = 0;
    for (int i = 0; i < graph.num_of_nodes(); i++) {
        if (partitionA[i]) {
            weight += graph.getNodeWeight(i);
        }
    }
    return weight;
}

double calculateBalanceFactorPartitions(Graph& G, const std::vector<std::vector<bool>>& partitions) {
    int partitionSize = partitions.size();
    std::vector<int> partitionsWeight(partitionSize);
    int maxPartitionWeight = 0;
    int minPartitionWeight = std::numeric_limits<int>::max();

    for (int i = 0; i < partitionSize; ++i) {
        partitionsWeight[i] = calculatePartitionsWeight(G, partitions[i]);
        maxPartitionWeight = std::max(maxPartitionWeight, partitionsWeight[i]);
        minPartitionWeight = std::min(minPartitionWeight, partitionsWeight[i]);
    }

    return (double)minPartitionWeight / (double)maxPartitionWeight;
}

int calculateCutSize(Graph& graph, const std::vector<bool>& partitionA) {
    int cutSize = 0;

    for (const Edge& edge : graph.getEdges()) {
        if (partitionA[edge.n1] != partitionA[edge.n2]) {
            cutSize += edge.weight;
            // cutSize++;
        }
    }

    return cutSize;
}

int calculateCutSizeBetweenPartitions(Graph& G, const std::vector<bool>& partitionA, const std::vector<bool>& partitionB) {
    int cutSize = 0;

    const std::vector<std::vector<std::vector<int>>>& matAdj = G.getMatAdj();

    for (int i = 0; i < G.num_of_nodes(); ++i) {
        for (int j = 0; j < G.num_of_nodes(); ++j) {
            if (partitionA[i] && partitionB[j] && matAdj[i][j][0] != -1) {
                cutSize += matAdj[i][j][1]; // Use the weight stored in the adjacency matrix
            }
        }
    }

    return cutSize;
}

int calculateCutSizePartitions(Graph& G, const std::vector<std::vector<bool>>& partitions) {
    int totalCutSize = 0;
    for (int i = 0; i < partitions.size(); ++i) {
        for (int j = i + 1; j < partitions.size(); ++j) {
            totalCutSize += calculateCutSizeBetweenPartitions(G, partitions[i], partitions[j]);
        }
    }
    return totalCutSize;
}

void read_input(const string& file, Graph* G) {

}

void savePartitionDataToFile(const PartitionData& partitionData) {
    ofstream outputFile(partitionData.fileName);
    if (outputFile.is_open()) {
        outputFile << "Total nodes weight: " << partitionData.totalNodesWeight << std::endl;
        outputFile << "Total edges weight: " << partitionData.totalEdgesWeight << std::endl;
        outputFile << "Graph reading execution time --> " << time_conversion((int) partitionData.executionTimes[0]) << endl
        << std::endl;

        outputFile << "Partition " << ": ";
        for (auto j : partitionData.partitions) {
            outputFile << j << " ";
        }
        for (int i =0 ;i < partitionData.partitionsWeights.size(); i++) {
            outputFile << "Partition "<< i << "internal weight: " << partitionData.partitionsWeights[i] << " | ";
        }
        outputFile << endl << " | | Cut Size: " << partitionData.cutSize << endl;
        outputFile << endl;

        outputFile << "Execution time partitioning --> " << time_conversion((int) partitionData.executionTimes[1]) << endl;
        outputFile << "CPU time used: " << partitionData.usage.ru_utime.tv_sec << " seconds " << partitionData.usage.ru_utime.tv_usec << " microseconds" << endl;
        outputFile << "Memory usage: " << partitionData.usage.ru_maxrss << " KBs | " << partitionData.usage.ru_maxrss / 1024.0 << " MBs | " << partitionData.usage.ru_maxrss / (1024.0 * 1024.0) << " GBs" << endl;
        outputFile << "CPU percentage: " << partitionData.cpu_percentage << "%" << endl;
        outputFile.close();
    }
}

double calculateAverageBalanceFactor(const std::vector<double>& balanceFactors) {
    double sum = 0.0;
    for (double balanceFactor : balanceFactors) {
        sum += balanceFactor;
    }
    return sum / balanceFactors.size();
}

double calculateAverageCutSize(const std::vector<int>& cutSizes) {
    double sum = 0.0;
    for (int cutSize : cutSizes) {
        sum += cutSize;
    }
    return sum / cutSizes.size();
}