#include "deskdistr.hpp"
#include "person.hpp"
#include "genealgo.hpp"
#include "json.hpp"
#include "localsearch.hpp"
#include <algorithm>
#include <random>
#include <sstream>
#include <string>
#include <regex>
#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>

#define SELECTION_TYPE_ROULETTE_WHEEL 1
#define SELECTION_TYPE_TOURNAMENT 2

using namespace std;

string totwo(int from) {
    string num = regex_replace("0d",regex("d"),to_string(from));
    return num.substr(num.length() - 2);
}
string to7(double from) {
    string num = regex_replace("d0000000",regex("d"),to_string(from));
    return num.substr(0,7);
}
string to2(double from) {
    string num = regex_replace("d00",regex("d"),to_string(from));
    return num.substr(0,2);
}

deskDistr::deskDistr(vector<person> people, vector<int> desks) {
    this->people = people;
    this->desks = desks;
}

deskDistr::deskDistr() {
}

deskDistr::deskDistr(const deskDistr& rhs): desks(rhs.desks), people(rhs.people) {
}

deskDistr deskDistr::readDesk(vector<person> people, string filename) {
    deskDistr instance;
    instance.people = people;
    std::ifstream reading(filename, std::ios::in);
    nlohmann::json data;
    reading >> data;
    // vector<int> desks = data;

    vector<int> deskre = data["desks"];
    vector<int> desks;
    for (int i = 0;i < 39;i++) {
        vector<int>::iterator indexIter = find(deskre.begin(),deskre.end(),i);
        int dist = distance(deskre.begin(), indexIter);
        cout << deskre[dist] << "," << i << endl;
        desks.push_back(dist);
    }

    instance.desks = desks;
    return instance;
}
vector<deskDistr> deskDistr::readDesks(vector<person> people, string filename) {
    vector<deskDistr> instance;
    std::ifstream reading(filename, std::ios::in);
    nlohmann::json data;
    reading >> data;
    for (int i = 0;i < data.size();i++) {
        vector<int> desks = data[i]["desks"];
        deskDistr new_elem = deskDistr(people,desks);
        instance.push_back(new_elem);
    }
    return instance;
}
void deskDistr::saveDesks(vector<deskDistr> desks,string filename) {
    nlohmann::json results = nlohmann::json::array();
    for(int j = 0;j < desks.size();j++) {
        nlohmann::json result;
        result["desks"] = desks[j].desks;
        results.push_back(result);
    }
    std::ofstream out(filename);
    out << results;
}
void deskDistr::saveDesk(deskDistr desk,string filename) {
    nlohmann::json result;
    result["desks"] = desk.desks;
    std::ofstream out(filename);
    out << result;
}

deskDistr deskDistr::makeRandomInstance(vector<person> people) {
    vector<int> desks;
    for (int i = 0;i < 39;i++) {
        desks.push_back(i);
    }
    random_device rd;
    default_random_engine rng(rd());
    shuffle(desks.begin(), desks.end(), rng);
    return deskDistr(people,desks);
}

vector<deskDistr> deskDistr::makeRandomPopulation(vector<person> people, int number) {
    vector<deskDistr> initial_population;

    for (int i = 0;i < number;i++) {
        deskDistr new_elem = deskDistr::makeRandomInstance(people);
        initial_population.push_back(new_elem);
    }

    return initial_population;
}

double deskDistr::satisfaction(double distance) const {
    return -tanh(distance - 2) + 1;
}

double deskDistr::distanceBetween(int indexA,int indexB) const {
    int fromx = (int)((indexA + 1) / 8);
    int fromy = (indexA + 1) % 8;
    int tox = (int)((indexB + 1) / 8);
    int toy = (indexB + 1) % 8;
    return sqrt((fromx - tox) * (fromx - tox) + (fromy - toy) * (fromy - toy));
}

double deskDistr::fitness(int of) const {
    double fit = 0;
    for (int i = 0;i < people[of].likes.size(); i++) {
        fit += satisfaction(distanceBetween(desks[people[of].likes[i]],desks[of])) * (people[of].weights[i]/6);
    }
    if (people[of].front && (((desks[of]+1) % 8) > 1)) {
        fit -= ((desks[of]+1) % 8) * 100;
    }
    return fit;
}

double deskDistr::fitnessSum() const  {
    double sum = 0;
    for (int i = 0;i < 39;i++) {
        sum += this->fitness(i);
    }
    return sum;
}

double deskDistr::miner() const {
    int index = 0;
    double min = fitness(index);
    for (int i = 1;i < 39;i++) {
        if (min > fitness(i)) {
            min = fitness(i);
            index = i;
        }
    }
    return index;
}

double deskDistr::maxer() const {
    int index = 0;
    double min = fitness(index);
    for (int i = 1;i < 39;i++) {
        if (min < fitness(i)) {
            min = fitness(i);
            index = i;
        }
    }
    return index;
}

double deskDistr::fitnessMin() const {
    return fitness(miner());
}

double deskDistr::fitnessMax() const {
    return fitness(maxer());
}

vector<double> deskDistr::evaluations() const {
    vector<double> result;
    result.push_back(fitnessSum());
    result.push_back(0);
    return result;
    // return fitnessMin();
}

double deskDistr::evaluation(string evaltype) const {
    if (evaltype == "sum") {
        return fitnessSum();
    } else if (evaltype == "min") {
        return fitnessMin();
    } else {
        return fitnessMax();
    }
}

void deskDistr::mutate() {
    random_device rnd;
    mt19937 engine(rnd());
    uniform_int_distribution<int> distr(0, 38);
    for (int i = 0;i < 5;i++) {
    int one = distr(engine);
    int two;
    do {
        two = distr(engine);
    } while (one == two);
    int oneval = this->desks[one];
    int twoval = this->desks[two];
    this->desks[one] = twoval;
    this->desks[two] = oneval;
    }
}

deskDistr deskDistr::swap(int a,int b) const {
    vector<int> result = this->desks;
    int aval = this->desks[a];
    int bval = this->desks[b];
    result[a] = bval;
    result[b] = aval;
    return deskDistr(people,result);
}

vector<deskDistr> deskDistr::crossover(deskDistr another) {
    vector<person> son;
    vector<person> daughter;
    vector<deskDistr> result;
    for (int i = 0;i < 39;i++) {
        random_device rnd;
        mt19937 engine(rnd());
        uniform_int_distribution<int> distr(0, 1);
        vector<double> anoweights(another.around(i).size(),1);
        vector<double> weights(around(i).size(),1);
        if (distr(engine) == 0) {
            son.push_back(person(i,another.around(i),anoweights,this->people[i].front));
            daughter.push_back(person(i,around(i),weights,this->people[i].front));
        } else {
            son.push_back(person(i,around(i),weights,this->people[i].front));
            daughter.push_back(person(i,another.around(i),anoweights,this->people[i].front));
        }
    }
    deskDistr son_dist = deskDistr::makeRandomInstance(son);
    localSearch sonEngine = localSearch(son_dist,100);
    deskDistr daughter_dist = deskDistr::makeRandomInstance(daughter);
    localSearch daughterEngine = localSearch(daughter_dist,100);
    deskDistr son_result = sonEngine.maximin();
    deskDistr daughter_result = daughterEngine.maximin();
    result.push_back(deskDistr(people,son_result.desks));
    result.push_back(deskDistr(people,daughter_result.desks));
    return result;
}

vector<int> deskDistr::around(int index) const {
    vector<int> result;
    vector<int> desks = this->desks;
    for (int j = -8;j < 17;j += 8) {
        if (desks[index] % 8 == 7) {
            for (int i = 0;i < 2;i++) {
                if ((desks[index] + i + j >= 0) && (i + j != 0)) {
                    vector<int>::iterator indexIter = find(desks.begin(),desks.end(),desks[index] + i + j);
                    int dist = distance(desks.begin(), indexIter);
                    result.push_back(dist);
                }
            }
        } else if (desks[index]% 8 == 6) {
            for (int i = -1;i < 1;i++) {
                if ((desks[index] + i + j >= 0) && (i + j != 0)) {
                    vector<int>::iterator indexIter = find(desks.begin(),desks.end(),desks[index] + i + j);
                    int dist = distance(desks.begin(), indexIter);
                    result.push_back(dist);
                }
            }
        } else {
            for (int i = -1;i < 2;i++) {
                if ((desks[index] + i + j >= 0) && (i + j != 0)) {
                    vector<int>::iterator indexIter = find(desks.begin(),desks.end(),desks[index] + i + j);
                    int dist = distance(desks.begin(), indexIter);
                    result.push_back(dist);
                }
            }
        }
    }
    return result;
}
// ╭────┬────┬────┬────┬────╮
// │ 31 │ 23 │ 15 │ 07 │    │
// ├────┼────┼────┼────┼────┤
// │ 32 │ 24 │ 16 │ 08 │ 00 │
// ├────┼────┼────┼────┼────┤
// │ 33 │ 25 │ 17 │ 09 │ 01 │
// ├────┼────┼────┼────┼────┤
// │ 34 │ 26 │ 18 │ 10 │ 02 │
// ├────┼────┼────┼────┼────┤
// │ 35 │ 27 │ 19 │ 11 │ 03 │
// ├────┼────┼────┼────┼────┤
// │ 36 │ 28 │ 20 │ 12 │ 04 │
// ├────┼────┼────┼────┼────┤
// │ 37 │ 29 │ 21 │ 13 │ 05 │
// ├────┼────┼────┼────┼────┤
// │ 38 │ 30 │ 22 │ 14 │ 06 │
// ╰────┴────┴────┴────┴────╯

string deskDistr::detail() const {
    string str = "\n╭────┬────┬────┬────┬────╮\n"
                 "│ d42 │ d34 │ d26 │ d18 │    │\n"
                 "├────┼────┼────┼────┼────┤\n"
                 "│ d43 │ d35 │ d27 │ d19 │ d11 │\n"
                 "├────┼────┼────┼────┼────┤\n"
                 "│ d44 │ d36 │ d28 │ d20 │ d12 │\n"
                 "├────┼────┼────┼────┼────┤\n"
                 "│ d45 │ d37 │ d29 │ d21 │ d13 │\n"
                 "├────┼────┼────┼────┼────┤\n"
                 "│ d46 │ d38 │ d30 │ d22 │ d14 │\n"
                 "├────┼────┼────┼────┼────┤\n"
                 "│ d47 │ d39 │ d31 │ d23 │ d15 │\n"
                 "├────┼────┼────┼────┼────┤\n"
                 "│ d48 │ d40 │ d32 │ d24 │ d16 │\n"
                 "├────┼────┼────┼────┼────┤\n"
                 "│ d49 │ d41 │ d33 │ d25 │ d17 │\n"
                 "╰────┴────┴────┴────┴────╯\n";
    for (int i = 0;i<39;i++) {
        string num = regex_replace("0d",regex("d"),to_string(i+1));
        str = regex_replace(str, regex("d"+to_string(desks[i]+11)), num.substr(num.length() - 2));
    }
    return str;
}

string deskDistr::output(string path) const {
    std::ifstream reading(path, std::ios::in);
    nlohmann::json data;
    reading >> data;
    string str = "\n╭───────┬───────┬───────┬───────┬───────╮\n"
                 "│ d42 │ d34 │ d26 │ d18 │       │\n"
                 "├───────┼───────┼───────┼───────┼───────┤\n"
                 "│ d43 │ d35 │ d27 │ d19 │ d11 │\n"
                 "├───────┼───────┼───────┼───────┼───────┤\n"
                 "│ d44 │ d36 │ d28 │ d20 │ d12 │\n"
                 "├───────┼───────┼───────┼───────┼───────┤\n"
                 "│ d45 │ d37 │ d29 │ d21 │ d13 │\n"
                 "├───────┼───────┼───────┼───────┼───────┤\n"
                 "│ d46 │ d38 │ d30 │ d22 │ d14 │\n"
                 "├───────┼───────┼───────┼───────┼───────┤\n"
                 "│ d47 │ d39 │ d31 │ d23 │ d15 │\n"
                 "├───────┼───────┼───────┼───────┼───────┤\n"
                 "│ d48 │ d40 │ d32 │ d24 │ d16 │\n"
                 "├───────┼───────┼───────┼───────┼───────┤\n"
                 "│ d49 │ d41 │ d33 │ d25 │ d17 │\n"
                 "╰───────┴───────┴───────┴───────┴───────╯\n";
    for (int i = 0;i<39;i++) {
        string name = data[i];
        string namereg = regex_replace("d　　",regex("d"),name);
        str = regex_replace(str, regex("d"+to_string(desks[i]+11)), namereg.substr(0,9));
    }
    return str;
}

string deskDistr::analysis() const {
    string title = "╭───────────────────────────────────────────────────────────────────────────────────────────────────────╮\n"
                   "│        sum: sfsfs, max: maxam, min: minim                                                             │\n"
                   "├───────┬────────┬────────────┬────────────┬────────────┬────────────┬────────────┬─────┬───────────────┤\n";
    title = regex_replace(title,regex("sfsfs"),to_string(fitnessSum()).substr(0,5));
    title = regex_replace(title,regex("maxam"),to_string(fitnessMax()).substr(0,5));
    title = regex_replace(title,regex("minim"),to_string(fitnessMin()).substr(0,5));
    stringstream result;
    result << title;
    for (int i = 0;i < 39;i++) {
        // result << "[" << people[i].weights[0] << "," << people[i].weights[1] << "],";
        result << "│num: " << totwo(i+1) << "│desk: " << totwo(desks[i]);
        for (int j = 0;j < 5;j++) {
            if (people[i].likes.size() > j) {
                result << "│like" << j+1 << ": " << totwo(people[i].likes[j]+1) << "," << to2(people[i].weights[j]);
            } else {
                result << "│            ";
            }
        }
        if (people[i].front) {
            result << "│front";
        } else {
            result << "│     ";
        }
        result << "│fitness:" << to7(fitness(i)) << "│" << endl;
    }
         result << "╰───────┴────────┴────────────┴────────────┴────────────┴────────────┴────────────┴─────┴───────────────╯" << endl;
    
    return result.str();
}

void deskDistr::putJSON(string filename) const {
    nlohmann::json result;
    result["desks"] = this->desks;
    std::ofstream out(filename);
    out << result;
    // cout << result.dump() << endl;
}
void deskDistr::putJSONs(vector<deskDistr> desks,string filename) {
    nlohmann::json results = nlohmann::json::array();
    for(int j = 0;j < desks.size();j++) {
        nlohmann::json result;
        result["desks"] = desks[j].desks;
        results.push_back(result);
    }
    std::ofstream out(filename);
    out << results;
}

int deskDistr::indexOf(int num) const {
    vector<int> desks = this->desks;
    vector<int>::iterator indexIter = find(desks.begin(),desks.end(),num);
    int index = distance(desks.begin(), indexIter);
    return index;
}