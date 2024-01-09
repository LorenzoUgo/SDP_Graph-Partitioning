//
// Created by Gregorio Nicora on 07/01/24.
//

#include "Fitness.h"

// calcola la dimensione del taglio sommando i pesi degli archi che vanno da una partizione ad un'altra
float cut_size(vector<int> genotype, Graph G){
    float cut_size = 0;
    for (auto edge : G.getEdges()){
        if(genotype[edge.n1] != genotype[edge.n2])
            cut_size += edge.weight;
    }
    return cut_size;
}


// calcola la differenza tra la partizione con il maggior peso e quella con il minor peso dei nodi al suo interno
float balance_index(int num_partitions, vector<int> genotype, const Graph& G){
    vector<float> partitions_weight (num_partitions);

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