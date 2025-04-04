#include <cstdio>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

long lastColor;

int mod(const int number, const int divisor) {
    const int reminder = number%divisor;
    return reminder<0 ? reminder+divisor : reminder;
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
    lastColor = getLastColor(graph_matrix);
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
    const long size = colors.size();
    const int divisor = lastColor + 1;
    auto neighbours = vector(size*2 , vector(colors));
    for (int i = 0; i < size; i++) {
        const int secondIndex = i+size;
        neighbours[i][i]=mod(neighbours[i][i]+1, divisor);
        neighbours[secondIndex][i]=mod(neighbours[secondIndex][i]-1, divisor);
    }
    return neighbours;
}

vector<int> findSolution(const vector<vector<bool>> &graph_matrix, const vector<int> &initialColors) {
    auto currentSolution = initialColors;
    auto globalBestSolution = currentSolution;
    long globalBestLoss = loss(graph_matrix, globalBestSolution);
    const int size = initialColors.size();
    const int maxHop = size - 1;
    int hop = 0;
    int currentIndex = 0;
    while (hop != maxHop) {
        if (currentSolution[currentIndex] == lastColor) {
            if (const int nextIndex = currentIndex+1; nextIndex != size && currentSolution[nextIndex] == lastColor) {
                currentSolution[currentIndex] = 0;
                currentIndex = nextIndex;
                hop++;
            }
            else if (nextIndex != size) {
                currentSolution[currentIndex] = 0;
                currentSolution[nextIndex]++;
                currentIndex = 0;
                hop=0;
            }
        } else {
            currentSolution[currentIndex]++;
        }
        if (const long currentLoss = loss(graph_matrix, currentSolution); globalBestLoss > currentLoss) {
            globalBestSolution = currentSolution;
            globalBestLoss = currentLoss;
        }
    }
    return globalBestSolution;
}

vector<int> fullSearchAlgorithm(const vector<vector<bool>> &graph_matrix) {
    const vector colors(graph_matrix.size(), 0);
    return findSolution(graph_matrix, colors);
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
    const auto colors = vector(graph_matrix.size(), 0);
    const auto solution = fullSearchAlgorithm(graph_matrix);
    printColors(solution);
    cout<<"Number of unique colors: "<<getUniqueColorsNumber(solution)<<endl;
    return 0;
}
