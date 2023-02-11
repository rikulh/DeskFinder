#ifndef _PERSON_H_
#define _PERSON_H_

#include <vector>
#include <string>

using namespace std;

class person {
public:
    person(int number, vector<int> likes,vector<double> weights,bool front);
    // static vector<person> randomPeople(int length);
    static vector<person> readPeople(string path,string type);
    int number;
    vector<int> likes;
    vector<double> weights;
    bool front;
};

#endif
