#include <Eigen/Dense>
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "lsh_forest.cpp"

using namespace std;

const unsigned int D = 68;

typedef Eigen::Vector<float, D> Point;

vector<Point> readData() {
    ifstream f("../dataset/USCensus1990.data.txt");
    vector<Point> data;
    Point p;
    string line, token;

    if (f.is_open()) {
        while (getline(f, line)) {
            // skip the first row
            if (line[0] < '0' || line[0] > '9')
                continue;

            istringstream iss(line);
            float x;
            getline(iss, token, ','); // skip the first column
            for (int i = 0; i < D; ++i) {
                getline(iss, token, ',');
                x = stof(token);
                p(i) = x;
            }
            data.push_back(p);

            // TODO: Short cut for testing
            if (data.size() >= 10000)
                break;
        }
        f.close();
    } else {
        cout << "Can't open file" << endl;
    }

    // center the data at origin
    Point center = mean(data);
    for (Point &p: data)
        p -= center;

    return data;
}

int main() {
    Point *ans;

    cout << "Reading data..." << endl;
    vector<Point> data = readData();

    LSH_Forest<D> lshF(2, 2, 2);

    cout << "Building LSH forest..." << endl;
    lshF.build(data);

    cout << "Sample query" << endl;
    ans = lshF.query(data[0]);
    if (ans)
        cout << data[0] << endl << (*ans - data[0]) << endl;
    else
        cout << "Not found" << endl;

    return 0;
}
