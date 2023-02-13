#ifndef _DESKDISTR_H_
#define _DESKDISTR_H_

#include "person.hpp"
#include <vector>
#include <string>

using namespace std;

class deskDistr {
public:
    deskDistr();
    static deskDistr makeRandomInstance(vector<person> people, int length);
    static deskDistr readDesk(vector<person> people, string filename);
    static vector<deskDistr> readDesks(vector<person> people, string filename);
    static void saveDesks(vector<deskDistr> desks,string filename);
    static void saveDesk(deskDistr desks,string filename);
    deskDistr(vector<person> people, vector<int> desks, int length);
    deskDistr(const deskDistr& rhs);

    vector<person> people;
    vector<int> desks;//indexが出席番号、内容が席
    int length;

    double maxer() const;
    double miner() const;
    
    double fitnessSum() const;
    double fitnessMin() const;
    double fitnessMax() const;
    vector<double> evaluations() const;
    double evaluation(string evaltype) const;

    void mutate();

    string detail() const;
    string print(string path) const;
    string analysis() const;
    void putJSON(string filename) const;
    static void putJSONs(vector<deskDistr> desks,string filename);

    int indexOf(int num) const;
    deskDistr swap(int a,int b) const;

    bool operator < (deskDistr left) const {
	    return this->fitnessMin() < left.fitnessMin();
    }
    bool operator > (deskDistr left) const {
		return this->fitnessMin() > left.fitnessMin();
	}
    bool operator == (deskDistr left) const {
		return this->desks == left.desks;
	}

    double fitness(int index) const;
    double satisfaction(double distance) const;
    double distanceBetween(int indexA, int indexB) const;
};

#endif