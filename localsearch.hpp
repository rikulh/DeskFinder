#ifndef _LOCAL_SEARCH_HPP_
#define _LOCAL_SEARCH_HPP_

#include "deskdistr.hpp"
#include <vector>
#include <string>

using namespace std;

class localSearch {
public:
    localSearch(deskDistr population, int max_generations);
    deskDistr mountain(string evaltype);
    deskDistr annealing(string evaltype);
    deskDistr maximin();
    deskDistr maximinx(int times);
    deskDistr maxstable();
private:
    deskDistr population;
    int max_generations;
};

#endif