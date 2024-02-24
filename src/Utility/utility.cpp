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

string time_conversion(int delta_t){
    int millisec, sec, min, h, day, year;
    string string_to_print;

    millisec = delta_t%1000;
    delta_t /= 1000;

    string_to_print.insert(0, to_string(millisec)+"ms ");
    if(delta_t > 0){
        sec = delta_t % 60;
        delta_t /= 60;
        string_to_print.insert(0, to_string(sec)+"s, ");

        if(delta_t > 0){
            min = delta_t%60;
            delta_t /= 60;
            string_to_print.insert(0, to_string(min)+"m, ");
        }

        if(delta_t > 0){
            h = delta_t%24;
            delta_t /= 24;
            string_to_print.insert(0, to_string(h)+"h, ");
        }

        if(delta_t > 0){
            day = delta_t%365;
            delta_t /= 365;
            string_to_print.insert(0, to_string(day)+"d, ");

            if(delta_t > 0){
                year = delta_t;
                string_to_print.insert(0, to_string(year)+"y, ");
            }
        }
    }


    return string_to_print;
}

// balance factor of 1.0 is perfectly balanced, 0.0 or 2.0 is completely unbalanced
float balanceFactor(int num_partitions, const vector<int>& partition, const Graph& G) {
    vector<float> partitions_weight(num_partitions);

    for(auto it = partition.begin();  it < partition.end();  it++){
        partitions_weight[*it] += G.getNodeWeight(distance(partition.begin(), it));
    }

    float max_partition_weight = *max_element(partitions_weight.begin(), partitions_weight.end());
    float min_partition_weight = *min_element(partitions_weight.begin(), partitions_weight.end());

    return max_partition_weight / min_partition_weight;
}

vector<int> calculatePartitionsWeight(int num_partitions, const vector<int>& partition, const Graph& G) {
    vector<int> partitions_weight(num_partitions);

    for(auto it = partition.begin();  it < partition.end();  it++){
        partitions_weight[*it] += G.getNodeWeight(distance(partition.begin(), it));
    }

    return partitions_weight;
}

bool read_input(const string& file, Graph& G, int type_reading, int num_t) {
    string text = file + "standard_text.txt";
    string bin = file + "standard_binary.bin";

    switch(type_reading) {
        case 0:
            if (!G.readFileSequentialTxt(text)) //"./data/graph_20_20/standard_text.txt"
                return false;
            break;
        case 1:
            if (!G.readFileSequentialBin(bin)) //"./data/graph_20_20/standard_text.txt"
                return false;
            break;
        case 2:
            if (!G.readFileParallel(bin, num_t)) //"./data/graph_20_20/standard_text.txt"
                return false;
            break;
        default:
            break;
    }

    return true;
}

void saveInfoToFile(const UsageInfo& usageInfo) {
    ofstream outputFile(usageInfo.fileName);
    if (outputFile.is_open()) {
        outputFile << "Execution times" << endl;
        outputFile << "Graph reading " << time_conversion(usageInfo.executionTimes[0]) << endl;
        outputFile << "Partitioning " << time_conversion(usageInfo.executionTimes[1]) << endl;
        outputFile << endl;
        outputFile << "Total nodes weight: " << usageInfo.totalNodesWeight << endl;
        outputFile << "Total edges weight: " << usageInfo.totalEdgesWeight << endl;
        for (int i =0 ;i < usageInfo.balanceIndexPartitions.size(); i++) {
            outputFile << "Partition "<< i << ". Internal weight: " << usageInfo.balanceIndexPartitions[i] << endl;
        }
        outputFile << "Balance Index: " << usageInfo.balanceIndex << endl;
        outputFile << "Global Cut Size: " << usageInfo.cutSize << endl;
        outputFile << endl;
        outputFile << usageInfo.parameters.str();
        outputFile << endl;
        outputFile << "CPU time used: " << usageInfo.usage.ru_utime.tv_sec << " seconds " << usageInfo.usage.ru_utime.tv_usec << " microseconds" << endl;
        outputFile << "Memory usage: " << usageInfo.usage.ru_maxrss << " KBs | " << usageInfo.usage.ru_maxrss / 1024.0 << " MBs | " << usageInfo.usage.ru_maxrss / (1024.0 * 1024.0) << " GBs" << endl;
        outputFile << "Fitness value: " << usageInfo.fitness << endl;
        outputFile << "Result " << endl;
        for (auto j : usageInfo.partition) {
            outputFile << j << " ";
        }
        outputFile.close();
    }
}


/*  
 * NOTE: utility function handled by compute_metis.sh script 
 */

void compute_metis(string filename) {
    ifstream fin(filename);
    istringstream name(filename);
    vector<int> ints;

    while(name >> std::ws) {
        if (isdigit(name.peek()))
            name >> ints.emplace_back();
        else
            name.ignore();
    }

    Graph G;
    string file = "./data/graph_" + std::to_string(ints[0]) +'_'+ std::to_string(ints[1]) + "/standard_binary.bin";
    G.readFileParallel(file, 5);

    G.normalize();

    if (!fin.is_open()) {
        cout << "Cannot open " << file << endl;
        return;
    }
    vector<int> result;
    int n;
    for (int i=0;i<G.num_of_nodes();i++) {
        fin >> n;
        result.emplace_back(n);
    }
    Individual I(result, ints[2],-1);
    I.setFitness(G, true);
    cout << "Fitness value is " << I.getFitness() << endl;
    return;
}