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
    Individual metisI({2,1,2,0,2,0,0,1,2,0,0,2,1,1,0,2,1,2,1,1});
    cout << metisI.fitness(G);
}
