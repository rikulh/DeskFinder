#ifndef _GENE_ALGO_HPP_
#define _GENE_ALGO_HPP_

#include "deskdistr.hpp"
#include <vector>
#include <string>

using namespace std;

class geneAlgo {
public:
    geneAlgo(vector<deskDistr> population, int max_generations, double mutation_probability, double crossover_probability, int selection_type, string evaltype);
    deskDistr run();
    deskDistr best_population_chromosome();
private:
    vector<deskDistr> population;
    int max_generations;
    double mutation_probability;
    double crossover_probability;
    int selection_type;
    string evaltype;

    void increase_generation();

    vector<deskDistr> roulette_wheel_selection();
    vector<deskDistr> tournament_selection();
    
    vector<deskDistr> next_generation_chromosomes(vector<deskDistr> parents);
    vector<deskDistr> selectionParents();
};

#endif