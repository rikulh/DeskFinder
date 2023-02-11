#include "genealgo.hpp"
#include "deskdistr.hpp"
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <limits>
#include <string>
#include <iostream>

#define SELECTION_TYPE_ROULETTE_WHEEL 1
#define SELECTION_TYPE_TOURNAMENT 2

using namespace std;

vector<deskDistr> nlargest(int n, vector<deskDistr> iterable);
vector<deskDistr> choices(vector<deskDistr> vec, vector<double> weights, int k);
vector<deskDistr> choices(vector<deskDistr> vec, int k);

geneAlgo::geneAlgo(vector<deskDistr> population, int max_generations, double mutation_probability, double crossover_probability, int selection_type, string evaltype) {
    this->population = population;
    this->max_generations = max_generations;
    this->mutation_probability = mutation_probability;
    this->crossover_probability = crossover_probability;
    this->selection_type = selection_type;
    this->evaltype = evaltype;
}

deskDistr geneAlgo::run() {
    deskDistr best_result = best_population_chromosome();
    for (int i = 0;i < max_generations;i++) {
        cout << " generations: " << i << ", most_fitness: " << best_result.evaluation(evaltype)  << endl;

        this->increase_generation();

        deskDistr current_generation_best_chromosome = this->best_population_chromosome();

        if (best_result.evaluation(evaltype) < current_generation_best_chromosome.evaluation(evaltype))
        {
            best_result = current_generation_best_chromosome;
        }
    }
    return best_result;
}

void geneAlgo::increase_generation() {
    vector<deskDistr> new_population;
    while (new_population.size() < this->population.size()) {
        vector<deskDistr> parents = this->selectionParents();
        vector<deskDistr> next_generations = this->next_generation_chromosomes(parents);
        copy(next_generations.begin(),next_generations.end(),back_inserter(new_population));
    }

    if (new_population.size() > this->population.size()) {
        new_population.erase(new_population.end());
    }

    this->population = new_population;
}

vector <deskDistr> geneAlgo::roulette_wheel_selection() {
    vector<deskDistr> original = this->population;
    vector<double> weights;
    vector<deskDistr> selected_chromosomes;
    for (size_t i = 0;i < original.size();i++) {
        weights.push_back(original[i].fitnessSum());
    }
    selected_chromosomes = choices(population,weights,2);
    return selected_chromosomes;
}

vector<deskDistr> geneAlgo::tournament_selection() {
    int participants_num = population.size();
    vector<deskDistr> participants = choices(this->population,participants_num);
    vector<deskDistr> selected_chromosomes = nlargest(2,participants);
    return selected_chromosomes;
}

vector<deskDistr> geneAlgo::next_generation_chromosomes(vector<deskDistr> parents) {
    random_device rd;
    mt19937 engine(rd());
    uniform_int_distribution<int> distr(numeric_limits<int>::min(),numeric_limits<int>::max());

    srand(distr(engine));
    double random_val = (double)rand() / RAND_MAX;
    vector<deskDistr> next_generation_chromosomes = parents;
    if (random_val < this->crossover_probability) {
        next_generation_chromosomes = parents[0].crossover(parents[1]);
    }
    
    random_val = (double)rand() / RAND_MAX;
    if (random_val < this->mutation_probability) {
        for (int i = 0;i < next_generation_chromosomes.size();i++) {
            next_generation_chromosomes[i].mutate();
        }
    }
    return next_generation_chromosomes;
}

vector<deskDistr> geneAlgo::selectionParents() {
    vector<deskDistr> parents;
    if (selection_type == SELECTION_TYPE_ROULETTE_WHEEL) {
        parents = this->roulette_wheel_selection();
    } else if (this->selection_type == SELECTION_TYPE_TOURNAMENT)
    {
        parents = this->tournament_selection();
    }
    return parents;
}

deskDistr geneAlgo::best_population_chromosome() {
    deskDistr best_chromosome = population[0];
    for (int i = 0;i < population.size();i++) {
        if (best_chromosome.fitnessSum() < this->population[i].fitnessSum()) {
            best_chromosome = population[i];
        }
    }
    return best_chromosome;
}


vector<deskDistr> choices(vector<deskDistr> vec, int k) {
    vector<deskDistr> result;
    random_device rnd;
    default_random_engine engine(rnd());
    uniform_int_distribution<int> distr(0, vec.size()-1);
    for(int i = 0;i<k;i++) {
        result.push_back(vec[distr(engine)]);
    }
    return result;
}

vector<deskDistr> choices(vector<deskDistr> vec, vector<double> weights, int k) {
    vector<deskDistr> result;

    random_device rd;
    mt19937 eng(rd());
    int max = accumulate(weights.begin(),weights.end(),0);
    uniform_int_distribution<int> distr(1, max);

    for(int i = 0;i < k;i++) {
        int rand = distr(eng);
        int j = 0;
        while (weights[j] < rand) {
            rand -= weights[j];
            j++;
        }
        result.push_back(vec[j]);
    }
    return result;
}

vector<deskDistr> nlargest(int n, vector<deskDistr> iterable) {
    vector<deskDistr> orig = iterable;
    vector<deskDistr> result;
    for(size_t i = 0;i<n;i++) {
        vector<deskDistr>::iterator max;
        max = max_element(orig.begin(),orig.end());
        result.push_back(*max);
        orig.erase(max);
    }
    return result;
}