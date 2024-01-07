#include <vector>
#include <random>
#include <functional>
#include <chrono>

#ifdef MUTATION
#define MUTATION_RATE 0.01 // 1% probability
#endif // MUTATION

using namespace std;

// NOTA classe individuo ha senso che stia qua
class Individual {

    private:
        vector<int> genotype;
        int num_alleles;
        float fitness_value;

    public:
        // constructor used for crossover computation
        Individual(vector<int> genotype = {}, float fitness = -1) : genotype(move(genotype)), fitness_value(fitness){}

        // constructor used for building population, randomizes genotype with given size and given max value for each gene 
        Individual(int n_alleles, int genotypeSize) : num_alleles(n_alleles) {
            genotype = {};
            std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
            std::uniform_int_distribution<int> gene_value(0,3);
            auto value = std::bind(gene_value, generator);

            for (int i=0; i<genotypeSize; i++)
                genotype.emplace_back(value());
        }

        int getNumAlleles() { return num_alleles; }

        // NOTA perchè usare setgenotype in crossover se esiste il constructor? 
        void setGenotype(vector<int> &gtype) { genotype = gtype; }
        const vector<int> &getGenotype() const { return genotype; }
        int getGenotypeSize() const { return genotype.size(); }
        
        float getFitnessValue() const { return fitness_value; }
        void setFitnessValue(float fitness_val) { 
                // NOTA avrebbe senso che fosse la fitness a chiamare dopo aver calcolato il valore. Fitness prende l'individuo e il grafo, fa i calcoli e poi setta il valore
                fitness_value = fitness_val;
            }

        #ifdef MUTATION
        void mutation() {
            std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
            std::uniform_real_distribution<float> mutation_incidence(0,1);
            std::uniform_int_distribution<int> mutation_value(1,num_alleles-1);

            auto incidence = std::bind(mutation_incidence, generator);
            auto value = std::bind(mutation_value, generator);

            for (auto& g: genotype)
                if (incidence() > MUTATION_RATE)
                    g += value()%num_alleles;
        }
        #endif  // MUTATION      
};

