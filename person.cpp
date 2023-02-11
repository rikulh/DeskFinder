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

// vector<person> person::randomPeople(int length) {
//     random_device rnd;
//     mt19937 engine(rnd());
//     uniform_int_distribution<int> distr(0, length - 1);
//     uniform_int_distribution<int> likedistr(0, 5);
//     int likecount = likedistr(engine);
//     vector<person> people;
//     for (int i = 0;i < length;i++) {
//         vector<int> likes;
//         uniform_int_distribution<double> distra(0, 1);
//         vector<double> weights;
//         for (int j = 0;j < likecount;j++) {
//             // int new_like = distr(engine);
//             // while (*find(likes.begin(),likes.end(),new_like) == new_like) {
//             //     new_like = distr(engine);
//             // }
//             likes.push_back(distr(engine));
//         }
//         weights.push_back(distra(engine));
//         weights.push_back(12 - weights[0]);
//         people.push_back(person(i,likes,weights));
//     }
//     return people;
// }
vector<person> person::readPeople(string path,string type) {
    std::ifstream reading(path, std::ios::in);
    json data;
    reading >> data;
    vector<person> people;
    if (type == "a") {
        for (int i = 0;i < 39;i++) {
            vector<int> likes = data["likes"][i];
            vector<double> weights = data["weights"][i];
            bool front = data["front"][i];
            people.push_back(person(i,likes,weights,front));
        }
    } else {
        for (int i = 0;i < 39;i++) {
            vector<int> likes = data[i]["likes"];
            vector<double> weights = data[i]["weights"];
            bool front = data[i]["front"];
            people.push_back(person(i,likes,weights,front));
        }
    }
    return people;
}