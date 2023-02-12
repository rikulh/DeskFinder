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
#include <map>

#include "json.hpp"

#define SELECTION_TYPE_ROULETTE_WHEEL 1
#define SELECTION_TYPE_TOURNAMENT 2

using namespace std;

int worst(vector<deskDistr> population)
{
    deskDistr worst_chromosome = population[0];
    int worst_index = 0;
    for (int i = 0; i < population.size(); i++)
    {
        if (worst_chromosome.fitnessMin() > population[i].fitnessMin())
        {
            worst_chromosome = population[i];
            worst_index = i;
        }
    }
    return worst_index;
}

vector<deskDistr> bests(vector<deskDistr> population)
{
    vector<deskDistr> result = population;
    sort(result.begin(), result.end(), [](const deskDistr &x, const deskDistr &y)
         { return x.fitnessMin() > y.fitnessMin(); });
    result.erase(unique(result.begin(), result.end()), result.end());
    result.erase(result.begin() + 10, result.end());
    return result;
}

int main(int argc, char *argv[])
{
    map<string, string> params;
    for (int i = 2; i + 1 < argc; i += 2)
    {
        if (argv[i][0] == '-')
        {
            params[argv[i]] = argv[i + 1];
        }
        else
        {
            params["-i2"] = argv[i];
            i--;
        }
    }
    string command = argv[1];
    if (command == "genpeople")
    {
        if (params.count("-o") * params.count("-l") != 0)
        {
            std::ofstream out(params["-o"]);
            out << person::printPeople(person::randomPeople(atoi(params["-l"].c_str())));
        }
        else
        {
            cout << "Error: missing parameter(s)" << endl;
        }
    }
    else if (command == "mine")
    {
        if (params.count("-p") * params.count("-d") != 0)
        {
            vector<person> people = person::readPeople(params["-p"], "a");
            vector<deskDistr> desks = deskDistr::readDesks(people, params["-d"]);
            bool changed = false;
            for (int i = 0; i < 10; i++)
            {
                deskDistr desk = deskDistr::makeRandomInstance(people);
                localSearch localEng = localSearch(desk, 500);
                deskDistr mxi = localEng.maximin();
                if (desks[worst(desks)].fitnessMin() < mxi.fitnessMin())
                {
                    cout << "update from" << endl
                         << desks[worst(desks)].detail() << endl
                         << desks[worst(desks)].analysis() << endl;
                    cout << "update to" << endl
                         << mxi.detail() << endl
                         << mxi.analysis() << endl;
                    desks[worst(desks)] = mxi;
                    changed = true;
                }
            }
            if (changed)
            {
                deskDistr::saveDesks(desks, params["-d"]);
            }
        }
        else
        {
            cout << "Error: missing parameter(s)" << endl;
        }
    }
    else if (command == "init")
    {
        if (params.count("-p") * params.count("-o") != 0)
        {
            vector<deskDistr> result;
            vector<person> people = person::readPeople(params["-p"], "a");
            for (int i = 0; i < 10; i++)
            {
                deskDistr desk = deskDistr::makeRandomInstance(people);
                result.push_back(desk);
            }
            deskDistr::putJSONs(result, params["-o"]);
        }
        else
        {
            cout << "Error: missing parameter(s)" << endl;
        }
    }
    else if (command == "evaluate")
    {
        if (params.count("-p") * params.count("-d") != 0)
        {
            vector<person> people = person::readPeople(params["-p"], "a");
            vector<deskDistr> desks = deskDistr::readDesks(people, params["-d"]);
            for (int i = 0; i < 10; i++)
            {
                cout << "desk " << i << endl;
                cout << desks[i].detail() << endl;
                cout << desks[i].analysis() << endl;
            }
        }
        else
        {
            cout << "Error: missing parameter(s)" << endl;
        }
    }
    else if (command == "print")
    {
        if (params.count("-p") * params.count("-d") * params.count("-n") != 0)
        {
            vector<person> people = person::readPeople(params["-p"], "a");
            vector<deskDistr> desks = deskDistr::readDesks(people, params["-d"]);
            for (int i = 0; i < desks.size(); i++)
            {
                cout << "desk " << i << endl;
                cout << desks[i].print(params["-n"]) << endl; // namelist.json
            }
        }
        else
        {
            cout << "Error: missing parameter(s)" << endl;
        }
    }
    return 0;
}