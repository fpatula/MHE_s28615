#include <cstdio>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

vector<vector<int>> readMatrix(ifstream &f) {
    vector<vector<int> > graph_matrix;
    string line;
    string token;

    while (getline(f, line)) {
        constexpr char delimiter = ' ';
        auto matrix_row = vector<int>();
        stringstream lineStream(line);
        while (getline(lineStream, token, delimiter)) {
            matrix_row.push_back(atoi(token.c_str()));
        }
        graph_matrix.push_back(matrix_row);
    }
    return graph_matrix;
}

int gain() {
    return 0;
}



vector<vector<int>> findNeighbours() {
    return {};
}

vector<int> fullSearchAlgorithm(vector<vector<int>> &graph_matrix, vector<int> colors) {

    return {};
}

int main(const int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "You must provide algorithm data file name and algorithm name";
        return -1;
    }

    ifstream f(argv[1]);
    if (!f.is_open()) {
        cerr << "Failed opening algorithm data file";
        return -1;
    }

    const vector<vector<int>> graph_matrix = readMatrix(f);
    auto colors = vector(graph_matrix.size(), 0);

    return 0;
}
