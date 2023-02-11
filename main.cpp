#include "deskdistr.hpp"
#include "genealgo.hpp"
#include "person.hpp"
#include "localsearch.hpp"
#include <iostream>
#include <iterator>
#include <regex>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "json.hpp"

#define SELECTION_TYPE_ROULETTE_WHEEL 1
#define SELECTION_TYPE_TOURNAMENT 2

using namespace std;

int worst(vector<deskDistr> population) {
    deskDistr worst_chromosome = population[0];
    int worst_index = 0;
    for (int i = 0;i < population.size();i++) {
        if (worst_chromosome.fitnessMin() > population[i].fitnessMin()) {
            worst_chromosome = population[i];
            worst_index = i;
        }
    }
    return worst_index;
}

vector<deskDistr> bests(vector<deskDistr> population) {
    vector<deskDistr> result = population;
    sort(result.begin(), result.end(), [](const deskDistr& x, const deskDistr& y) { return x.fitnessMin() > y.fitnessMin();});
    result.erase(std::unique(result.begin(), result.end()), result.end());
    result.erase(result.begin() + 10,result.end());
    return result;
}

int main(int argc, char* argv[]) {
    string arg = argv[1];
    if (arg == "run") {
        vector<deskDistr> result;
        vector<person> people = person::readPeople("../3-6.json","b");
        for (int i = 0;i < 30;i++) {
            deskDistr desk = deskDistr::makeRandomInstance(people);
            localSearch localEng = localSearch(desk,atoi(argv[2]));
            deskDistr mxi = localEng.maximin();
            cout << "maximin " << i << endl;
            cout << mxi.detail() << endl;
            result.push_back(mxi);
        }
        deskDistr::putJSONs(result,"first.json");
    } else if (arg == "-json") {
        vector<person> people;
        deskDistr desk;
        people = person::readPeople("../3-6.json","b");
        desk = deskDistr::readDesk(people,"../test.json");
        cout << desk.detail() << endl;
        cout << desk.analysis() << endl;

    } else if (arg == "-test") {
        vector<person> people = person::readPeople("../3-6.json","b");
        deskDistr desk = deskDistr::makeRandomInstance(people);
        localSearch localEng = localSearch(desk,atoi(argv[2]));
        deskDistr mxi = localEng.maximin();
        cout << mxi.detail() << endl;
        cout << mxi.analysis() << endl;
    } else if (arg == "-make") {
        vector<person> people = person::readPeople("../3-6.json","b");
        vector<deskDistr> desks = deskDistr::readDesks(people,"../desks.json");
        int hash = 0;
        for (int i = 0;i < 10;i++) {
            deskDistr desk = deskDistr::makeRandomInstance(people);
            localSearch localEng = localSearch(desk,500);
            deskDistr mxi = localEng.maximin();
            if (desks[worst(desks)].fitnessMin() < mxi.fitnessMin()) {
                cout << "update from" << endl << desks[worst(desks)].detail() << endl << desks[worst(desks)].analysis() << endl;
                cout << "update to" << endl << mxi.detail() << endl << mxi.analysis() << endl;
                desks[worst(desks)] = mxi;
                hash++;
            }
        }
        if (hash != 0) {
            deskDistr::saveDesks(desks,"../desks.json");
        }
    } else if (arg == "-setup") {
        vector<deskDistr> result;
        vector<person> people = person::readPeople("../3-6.json","b");
        for (int i = 0;i < 10;i++) {
            deskDistr desk = deskDistr::makeRandomInstance(people);
            localSearch localEng = localSearch(desk,atoi(argv[2]));
            deskDistr mxi = localEng.maximin();
            cout << "maximin " << i << endl;
            cout << mxi.detail() << endl;
            result.push_back(mxi);
        }
        deskDistr::putJSONs(result,"../desks.json");
    } else if (arg == "-see") {
        vector<person> people = person::readPeople("../3-6.json","b");
        vector<deskDistr> desks = deskDistr::readDesks(people,"../desks.json");
        for (int i = 0;i < 10;i++) {
            cout << "desk " << i << endl;
            cout << desks[i].detail() << endl;
            cout << desks[i].analysis() << endl;
        }
    } else if (arg == "-output") {
        vector<person> people = person::readPeople("../3-6.json","b");
        vector<deskDistr> desks = deskDistr::readDesks(people,"../desks.json");
        std::ofstream out("result.txt");
        for (int i = 0;i < desks.size();i++) {
            // cout << "desk " << i << endl;
            // cout << desks[i].output("name.json") << endl;
            out << "desk " << i << endl;
            out << desks[i].output("name.json") << endl;
        }
    } else if (arg == "-thread") {
        vector<deskDistr> result;
        vector<person> people = person::readPeople("../3-6.json","b");
        for (int i = 0;i < 20;i++) {
            deskDistr desk = deskDistr::makeRandomInstance(people);
            localSearch localEng = localSearch(desk,atoi(argv[2]));
            deskDistr mxi = localEng.maximin();
            cout << "maximin " << i << endl;
            cout << mxi.detail() << endl;
            result.push_back(mxi);
        }
        deskDistr::putJSONs(result,"../thread.json");
    } else if (arg == "-merge") {
        vector<person> people = person::readPeople("../3-6.json","b");
        vector<deskDistr> merge = deskDistr::readDesks(people,argv[2]);
        vector<deskDistr> desks = deskDistr::readDesks(people,argv[3]);
        desks.insert(desks.end(), merge.begin(), merge.end());
        vector<deskDistr> result = bests(desks);
        deskDistr::saveDesks(result,"../result.json");
    } else if (arg == "-evaluate") {
        vector<person> people = person::readPeople("../3-6.json","b");
        deskDistr desk = deskDistr::readDesk(people,"../test.json");
        std::ofstream out("result.txt");
        out << desk.output("name.json");
    }
    return 0;
}