#include "Individual.h"

#define MUTATION_FRACTION 0.05 // maximum number of mutated genes expressed as fraction of genotype size
#define NUM_THREADS 4

void Individual::mutation() {
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> mutation_incidence(0,genotype.size()*MUTATION_FRACTION);
    std::uniform_int_distribution<int> mutation_value(1,num_alleles-1);

    auto incidence = std::bind(mutation_incidence, generator);
    auto value = std::bind(mutation_value, generator);

    int mutations = incidence();
    for(int i=0;i<mutations;i++){
        genotype[rand()%genotype.size()] = (genotype[rand()%genotype.size()] + value())%num_alleles;
    }
}

float get_mutation_fraction() {
    return MUTATION_FRACTION;
}

float cut_size(vector<int> genotype, const Graph& G){
    float cut_size = 0;
    for (auto edge : G.getEdges()){
        if(genotype[edge.n1] != genotype[edge.n2])
            cut_size += edge.weight;
    }
    return cut_size;
}

void cut_evaluator(int index, vector<int> genotype, vector<float>& values, vector<Edge> edges){
    for (int i=index; i<edges.size(); i+=NUM_THREADS){
        if(genotype[edges[i].n1] != genotype[edges[i].n2])
            values[index] += edges[i].weight;
    }
}

void cut_size_parallel(vector<int>& genotype, const Graph& G, float& cut_size, int internal){
    auto edges = G.getEdges();

    if(internal){
        vector<thread> cut_size_evaluators;
        vector<float> values(NUM_THREADS);
        for(int i = 0; i<NUM_THREADS; i++) {
            cut_size_evaluators.emplace_back( [=, &genotype, &values, &edges] {cut_evaluator(i, genotype, values, edges);});
        }

        for(int i=0; i<NUM_THREADS; i++){
            cut_size_evaluators[i].join();
        }

        cut_size = reduce(values.begin(), values.end());
    }else{
        for (int i=0; i<edges.size(); i++){
            if(genotype[edges[i].n1] != genotype[edges[i].n2])
                cut_size += edges[i].weight;
        }
    }
}

void balance_evaluator(int index, vector<int> genotype, const Graph& G, vector<vector<float>>& partitions_weight_values){
    for(int i=index; i<genotype.size(); i+=NUM_THREADS){
        partitions_weight_values[index][genotype[i]] += G.getNodeWeight(i);
    }
}

void balance_index_parallel(int num_partitions, const vector<int>& genotype, const Graph& G, float& balance_index, int internal){
    vector<float> partitions_weight(num_partitions);

    if(internal){
        vector<thread> partitions_weight_evaluators;
        vector<vector<float>> partitions_weight_values(NUM_THREADS, vector<float>(num_partitions));

        for(int i = 0; i<NUM_THREADS; i++) {
            partitions_weight_evaluators.emplace_back( [=, &genotype, &G, &partitions_weight_values] {balance_evaluator(i, genotype, G, partitions_weight_values);});
        }

        for(int i=0; i<NUM_THREADS; i++){
            partitions_weight_evaluators[i].join();
        }

        for(int i=0; i<NUM_THREADS; i++){
            for(int j=0; j<num_partitions;j++){
                partitions_weight[j] += partitions_weight_values[i][j];
            }
        }
    }else{
        for(int i=0; i<genotype.size(); i++){
            partitions_weight[genotype[i]] += G.getNodeWeight(i);
        }
    }

    float max_partition_weight = *max_element(partitions_weight.begin(), partitions_weight.end());
    float min_partition_weight = *min_element(partitions_weight.begin(), partitions_weight.end());

    balance_index = max_partition_weight - min_partition_weight;
}

float balance_index(int num_partitions, const vector<int>& genotype, const Graph& G){
    vector<float> partitions_weight(num_partitions);

    for(auto it = genotype.begin();  it < genotype.end();  it++){
        partitions_weight[*it] += G.getNodeWeight(distance(genotype.begin(), it));
    }

    float max_partition_weight = *max_element(partitions_weight.begin(), partitions_weight.end());
    float min_partition_weight = *min_element(partitions_weight.begin(), partitions_weight.end());

    return max_partition_weight - min_partition_weight;
}

void Individual::setFitness(const Graph& G, const bool& balance, float cut_size_weight , float balance_index_weight){

    fitness_value= (
            balance ?
            (cut_size_weight*cut_size(genotype, G)) + (balance_index_weight*balance_index(num_alleles, genotype, G))
                    :
            (cut_size(genotype, G))
    );
}

void Individual::setFitnessParallel(const Graph& G, const bool& balance, int internal, float cut_size_weight , float balance_index_weight){

    float cut_size_value = 0.0;
    thread t1([&] { cut_size_parallel(genotype, G, cut_size_value, internal);});

    float balance_index_value = 0.0;
    thread t2([&] { balance_index_parallel(num_alleles, genotype, G, balance_index_value, internal);});

    t1.join();
    t2.join();

    fitness_value= (
            balance ?
            (cut_size_weight*cut_size_value) + (balance_index_weight*balance_index_value)
                    :
            (cut_size_value)
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

