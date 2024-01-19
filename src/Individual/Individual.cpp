#include "Individual.h"

#define MUTATION 0.1 // 10% probability

void Individual::mutation() {
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> mutation_incidence(0,1);
    std::uniform_int_distribution<int> mutation_value(1,num_alleles-1);

    auto incidence = std::bind(mutation_incidence, generator);
    auto value = std::bind(mutation_value, generator);

    for (auto& g: genotype)
        if (incidence() < MUTATION)
            g = (g+value())%num_alleles;
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
void Individual::setFitness(const Graph& G, const bool& balance, float cut_size_weight , float balance_index_weight){

    fitness_value= (
            balance ?
                (cut_size_weight*cut_size(genotype, G)) + (balance_index_weight*balance_index(num_alleles, genotype, G))
            :
                (cut_size_weight*cut_size(genotype, G))
    );
}

void Individual::printIndividual(){

    cout << "Partition --> ";
    for (int i : genotype) {
        cout << i << " ";
    }
    cout << " / / / ";
    cout << "Fitness --> " << fitness_value << endl;
}
