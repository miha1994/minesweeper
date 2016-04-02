#pragma once
#include <vector>
#include <string>

/*
_______________________

 EXAMPLE OF USING:
_______________________

#include "hot_vars.h"

double f () {
    double d =  hot_vars.int_[0]    /    hot_vars.double_[2];
}

*/

struct ___hot_vars {
    std::vector <double> double_;
    std::vector <int> int_;
    std::vector <std::string> string_;
    ___hot_vars ();
};

extern ___hot_vars hot_vars;

#define I_    hot_vars.int_
#define D_ hot_vars.double_
#define S_ hot_vars.string_