#include "person.hpp"
#include <vector>
#include <iterator>
#include <random>
#include <algorithm>
#include <fstream>
#include <limits>
#include <iostream>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

person::person(int number, vector<int> likes, vector<double> weights, bool front): number(number), likes(likes), weights(weights), front(front) {
}

vector<person> person::randomPeople(int length) {
    random_device rnd;
    mt19937 engine(rnd());
    uniform_int_distribution<int> distr(0, length - 1);
    uniform_int_distribution<int> front(0, 5);
    vector<person> people;
    for (int i = 0;i < length;i++) {
        vector<int> likes;
        vector<double> weights;
        for (int j = 0;j < 2;j++) {
            likes.push_back(distr(engine));
        }
        weights.push_back(6);
        weights.push_back(6);
        people.push_back(person(i,likes,weights,(int)(front(engine)/5)));
    }
    return people;
}
string person::printPeople(vector<person> people) {
    json data;
    for (int i = 0; i < people.size(); i++)
    {
        json likes = people[i].likes;
        json weights = people[i].weights;
        json front = people[i].front;
        data["likes"].push_back(likes);
        
        data["weights"].push_back(weights);
        data["front"].push_back(front);
    }
    return data.dump();
}
vector<person> person::readPeople(string path,string type) {
    std::ifstream reading(path, std::ios::in);
    json data;
    reading >> data;
    vector<person> people;
    if (type == "a") {
        for (int i = 0;i < 49;i++) {
            vector<int> likes = data["likes"][i];
            vector<double> weights = data["weights"][i];
            bool front = data["front"][i];
            people.push_back(person(i,likes,weights,front));
        }
    } else {
        for (int i = 0;i < 49;i++) {
            vector<int> likes = data[i]["likes"];
            vector<double> weights = data[i]["weights"];
            bool front = data[i]["front"];
            people.push_back(person(i,likes,weights,front));
        }
    }
    return people;
}