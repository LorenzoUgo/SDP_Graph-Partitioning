//
// Created by Lorenzo Ugoccioni on 08/01/2024.
//

#include "RandomGenerator.h"

int randomNumber::random_int_num(int a, int b){
    uniform_int_distribution uid(a, b);
    return uid(DRE);
}

float randomNumber::random_real_num(auto a, auto b){
    uniform_real_distribution urd( (float) a, (float) b);
    return urd(DRE);
}