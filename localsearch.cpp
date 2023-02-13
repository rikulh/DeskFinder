#include "localsearch.hpp"
#include "deskdistr.hpp"
#include <vector>
#include <iostream>
#include <string>
#include <cmath>

using namespace std;

localSearch::localSearch(deskDistr population, int max_generations): population(population), max_generations(max_generations) {
}
deskDistr localSearch::mountain(string evaltype) {
    deskDistr best_result = population;

    for (int i = 0;i < max_generations;i++) {
        cout << "generations: " << i << ", most_fitness: " << best_result.evaluation(evaltype) << endl;

        this->population.mutate();

        if (best_result.evaluation(evaltype) < this->population.evaluation(evaltype))
        {
            best_result = this->population;
        }
    }
    return best_result;
}

double prblty(double e,double d,double t) {
    if (e >= d) {
        return 1;
    } else {
        return exp((e-d)/t);
    }
}

deskDistr localSearch::annealing(string evaltype) {
    deskDistr best_result = population;
    double prob = 1;
    for (int i = 0;i < max_generations;i++) {
        double temp = pow(2,((max_generations - i) / max_generations));
        cout << "generations: " << i << ", most_fitness: " << best_result.evaluation(evaltype) << endl;
        cout << prob << "times" << endl;
        for (int i = 0;i < prob;i++) {
            this->population.mutate();
        }
        prob = prblty(this->population.fitnessMin(),best_result.fitnessMin(),temp);

        best_result = this->population;
    }
    return best_result;
}

deskDistr localSearch::maximin() {
    deskDistr best_result = population;
    int count = 0;
    while (count < max_generations) {
        int p = best_result.miner();
        double best_fitness = 0;
        int choice;
        for (int q = 0;q < population.length;q++) {
            if (q != p) {
                deskDistr swapped = best_result.swap(p,q);
                if ((best_result.fitness(p) < swapped.fitness(p)) && (best_fitness < swapped.fitnessSum())) {
                    choice = q;
                    best_fitness = swapped.fitnessSum();
                }
            }
        }
        if (best_fitness == 0) {
            // cout << "End fitness: " << best_fitness << endl;
            return population;
        } else {
            // cout << "fitness: " << best_fitness << endl;
            best_result = best_result.swap(p,choice);
        }
        count++;
    }
        cout << "count" << endl;
    return best_result;
}

deskDistr localSearch::maximinx(int times) {
    double best_fitness = 0.0;
    deskDistr best_result;
    for (int i = 0;i < times;i++) {
        deskDistr mxmn = maximin();
        if (best_fitness <= mxmn.fitnessMin()) {
            best_fitness = mxmn.fitnessMin();
            best_result = mxmn;
        }
    }
    return best_result;
}

deskDistr localSearch::maxstable() {
    deskDistr best_result = population;
    int count = 0;
    while (count < max_generations) {
        double best_fitness = -500;
        int choicep = 0;
        int choiceq = 0;
        for (int p = 0;p < population.length;p++) {
            for (int q = population.length-1;q > p;q--) {
                deskDistr swapped = best_result.swap(p,q);
                // cout << best_result.fitnessSum() << "&" << swapped.fitnessSum() << "%" << best_fitness << endl;
                if ((best_result.fitness(p) < swapped.fitness(p)) && (best_result.fitness(q) < swapped.fitness(q)) && (swapped.fitnessSum() > best_fitness)) {
                    choicep = p;
                    choiceq = q;
                    best_fitness = swapped.fitnessSum();
                }
            }
        }
        if (best_fitness == 0) {
            // cout << "End fitness: " << best_fitness << endl;
            return population;
        } else {
            // cout << "fitness: " << best_fitness << endl;
            best_result = best_result.swap(choicep,choiceq);
        }
        count++;
    }
    return best_result;
}