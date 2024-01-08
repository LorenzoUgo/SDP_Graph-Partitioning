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