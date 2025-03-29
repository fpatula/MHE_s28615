#include <cstdio>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

long colorsLimit;

vector<vector<bool>> readMatrix(ifstream &f) {
    vector<vector<bool> > graph_matrix;
    string line;
    string token;

    while (getline(f, line)) {
        constexpr char delimiter = ' ';
        auto matrix_row = vector<bool>();
        stringstream lineStream(line);
        while (getline(lineStream, token, delimiter)) {
            matrix_row.push_back(atoi(token.c_str()));
        }
        graph_matrix.push_back(matrix_row);
    }
    return graph_matrix;
}

long loss(const vector<vector<bool>> &graph_matrix, const vector<int>& colors) {
    long loss = 0;
    const auto uniqueColors = set(colors.begin(), colors.end());
    const long numberOfUniqueColors = uniqueColors.size();
    long errorWeight = numberOfUniqueColors;
    loss += numberOfUniqueColors;
    for (int i = 0; i < colors.size(); i++) {
        const auto& adjacencyRow = graph_matrix[i];
        for (int j = 0; j < adjacencyRow.size(); j++) {
            if (adjacencyRow[j] && colors[i] == colors[j]) {
                loss += ++errorWeight;
            }
        }
    }
    return loss;
}

vector<int> generateRandomSolution(vector<int> colors) {
    for (int & color : colors) {
        random_device generator;
        uniform_int_distribution<> distribution(0, colorsLimit);
        color = distribution(generator);
    }
    return colors;
}

vector<vector<int>> findNeighbours(const vector<int>& colors) {
    auto neighbours = vector(colors.size(), vector(colors));
    for (int i = 0; i < neighbours.size(); i++) {
        neighbours[i][i]++;
    }
    return neighbours;
}

vector<int> fullSearchAlgorithm(vector<vector<bool>> &graph_matrix, vector<int> colors) {
    for (int i = 0; i < graph_matrix.size(); i++) {
        for (int j = 0; j < graph_matrix[i].size(); j++) {

        }
    }
    return {};
}

void printGraphMatrix(const vector<vector<bool>>& graph_matrix) {
    for (auto & i : graph_matrix) {
        cout<<"|";
        for (const bool j : i) {
            cout<<j<<"|";
        }
        cout<<"\n";
    }
}

long getColorsLimit(const vector<vector<bool>>& graph_matrix) {
    int globalMaxColors = 1;
    for (auto & adjacencyRow : graph_matrix) {
        int maxColors = 1;
        for (const bool isAdjacent : adjacencyRow) {
            if (isAdjacent) {
                maxColors++;
            }
        }
        if (maxColors > globalMaxColors) {
            globalMaxColors = maxColors;
        }
    }
    return globalMaxColors+1;
}

int main(const int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "You must provide algorithm data file name and algorithm name";
        return -1;
    }
    cout<<"File path: "<<argv[1]<<endl<<"Algorithm name: "<<argv[2]<<endl;
    ifstream f(argv[1]);
    if (!f.is_open()) {
        cerr << "Failed opening algorithm data file";
        return -1;
    }

    const vector<vector<bool>> graph_matrix = readMatrix(f);
    auto colors = vector(graph_matrix.size(), 0);
    colorsLimit = getColorsLimit(graph_matrix);
    printGraphMatrix(graph_matrix);

    return 0;
}
