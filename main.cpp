#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

vector<vector<int> > readMatrix(ifstream &f) {
    vector<vector<int> > graph_matrix;
    string line;
    string token;

    while (getline(f, line)) {
        char delimiter = ' ';
        auto matrix_row = vector<int>();
        stringstream lineStream(line);
        while (getline(lineStream, token, delimiter)) {
            matrix_row.push_back(atoi(token.c_str()));
        }
        graph_matrix.push_back(matrix_row);
    }
    return graph_matrix;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "You must provide algorithm data file name and algorithm name";
        return -1;
    }

    ifstream f(argv[1]);
    if (!f.is_open()) {
        cerr << "Failed opening algorithm data file";
        return -1;
    }

    vector<int> colors;
    vector<vector<int> > graph_matrix = readMatrix(f);

    return 0;
}
