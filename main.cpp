#include <cstdio>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

long lastColor;

vector<vector<bool>> readMatrix(ifstream &f) {
    vector<vector<bool>> graph_matrix;
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

long getUniqueColorsNumber(const vector<int> &colors) {
    const auto uniqueColors = set(colors.begin(), colors.end());
    return uniqueColors.size();
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

void printColors(const vector<int>& colors) {
    cout<<"|";
    for (const auto color : colors) {
        cout<<color<<"|";
    }
    cout<<endl;
}

long getLastColor(const vector<vector<bool>>& graph_matrix) {
    int globalLastColor = 0;
    for (auto & adjacencyRow : graph_matrix) {
        int maxColors = 0;
        for (const bool isAdjacent : adjacencyRow) {
            if (isAdjacent) {
                maxColors++;
            }
        }
        if (maxColors > globalLastColor) {
            globalLastColor = maxColors;
        }
    }
    return globalLastColor;
}

long loss(const vector<vector<bool>> &graph_matrix, const vector<int>& colors) {
    long loss = 0;
    const long numberOfUniqueColors = getUniqueColorsNumber(colors);
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
        uniform_int_distribution<> distribution(0, lastColor);
        color = distribution(generator);
    }
    return colors;
}

vector<vector<int>> findNeighbours(const vector<int>& colors) {
    auto neighbours = vector(colors.size(), vector(colors));
    const long initialSize = neighbours.size();
    for (int i = 0; i < initialSize; i++) {
        vector<int>& vector = neighbours[i];
        if (vector[i] != lastColor && vector[i]!=0) {
            auto neighbour = vector;
            vector[i]++;
            neighbour[i]--;
            neighbours.push_back(neighbour);
        }
        else if (vector[i] == 0) {
            vector[i]++;
        }
        else {
            vector[i]--;
        }
    }
    return neighbours;
}

vector<int> findSolutionRecursive(const vector<vector<bool>> &graph_matrix, const vector<int> &colors, vector<int> &solution, bool &stop, const int index) {
    if (stop || index == colors.size()) {
        return solution;
    }
    for (int i = 1; i <= lastColor; i++) {
        vector<int> newColors = colors;
        newColors[index]=i;
        long uniqueColorsNumber = getUniqueColorsNumber(newColors);
        if (loss(graph_matrix, newColors) == uniqueColorsNumber) {
            stop = true;
            solution = newColors;
            return solution;
        }
        findSolutionRecursive(graph_matrix, newColors, solution, stop, index+1);
    }
    return solution;
}

vector<int> fullSearchAlgorithm(const vector<vector<bool>> &graph_matrix) {
    vector colors(graph_matrix.size(), 0);
    vector solution(graph_matrix.size(), 0);
    bool stop = false;
    return findSolutionRecursive(graph_matrix, colors, solution, stop, 0);
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
    lastColor = getLastColor(graph_matrix);

    auto neighbours = findNeighbours(colors);
    for (auto neighbour : neighbours) {
        printColors(neighbour);
    }

    return 0;
}
