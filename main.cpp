#include <cstdio>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>

using namespace std;

long lastColor;

enum ALGORITHMS {
    FULL_SEARCH,
    HILL_CLIMBING,
    HILL_CLIMBING_STOCHASTIC,
    TABU
};

unordered_map<string,ALGORITHMS> const algorithmsMap = { {"full_search",FULL_SEARCH}, {"hill_climbing", HILL_CLIMBING}, {"hill_climbing_stochastic", HILL_CLIMBING_STOCHASTIC}, {"tabu", TABU}};

int mod(const int number, const int divisor) {
    const int reminder = number%divisor;
    return reminder<0 ? reminder+divisor : reminder;
}

long getLastColor(const vector<vector<bool>>& graphMatrix) {
    int globalLastColor = 0;
    for (auto & adjacencyRow : graphMatrix) {
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
    vector<vector<bool>> graphMatrix;
    string line;
    string token;

    while (getline(f, line)) {
        constexpr char delimiter = ' ';
        auto matrix_row = vector<bool>();
        stringstream lineStream(line);
        while (getline(lineStream, token, delimiter)) {
            matrix_row.push_back(atoi(token.c_str()));
        }
        graphMatrix.push_back(matrix_row);
    }
    lastColor = getLastColor(graphMatrix);
    return graphMatrix;
}

long getUniqueColorsNumber(const vector<int> &colors) {
    const auto uniqueColors = set(colors.begin(), colors.end());
    return uniqueColors.size();
}

void printGraphMatrix(const vector<vector<bool>>& graphMatrix) {
    for (auto & i : graphMatrix) {
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

long loss(const vector<vector<bool>> &graphMatrix, const vector<int>& colors) {
    long loss = 0;
    const long numberOfUniqueColors = getUniqueColorsNumber(colors);
    long errorWeight = numberOfUniqueColors;
    loss += numberOfUniqueColors;
    for (int i = 0; i < colors.size(); i++) {
        const auto& adjacencyRow = graphMatrix[i];
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

vector<int> findSolution(const vector<vector<bool>> &graphMatrix, const vector<int> &initialColors) {
    auto currentSolution = initialColors;
    auto globalBestSolution = currentSolution;
    long globalBestLoss = loss(graphMatrix, globalBestSolution);
    const long size = initialColors.size();
    const long maxHop = size - 1;
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
        if (const long currentLoss = loss(graphMatrix, currentSolution); globalBestLoss > currentLoss) {
            globalBestSolution = currentSolution;
            globalBestLoss = currentLoss;
        }
    }
    return globalBestSolution;
}

vector<int> fullSearchAlgorithm(const vector<vector<bool>> &graphMatrix) {
    const vector colors(graphMatrix.size(), 0);
    return findSolution(graphMatrix, colors);
}

vector<int> hillClimbingAlgorithm(const vector<vector<bool>> &graphMatrix, const vector<int> &initialColors, const int maxIterations) {
    vector<int> globalBestSolution = initialColors;
    long globalBestLoss = loss(graphMatrix, initialColors);
    bool foundBestSolution = false;
    for (int i = 0; i < maxIterations || foundBestSolution; i++) {
        vector<vector<int>> neighbours = findNeighbours(globalBestSolution);
        vector<int> bestNeighbour = neighbours[0];
        long bestNeighbourLoss = loss(graphMatrix, bestNeighbour);
        const long numberOfNeighbours = neighbours.size();
        for (int j = 1; j < numberOfNeighbours; j++) {
            const auto& neighbour = neighbours[j];
            if (const long neighbourLoss = loss(graphMatrix, neighbour); bestNeighbourLoss > neighbourLoss) {
                bestNeighbour = neighbour;
                bestNeighbourLoss = neighbourLoss;
            }
        }
        if (bestNeighbourLoss < globalBestLoss) {
            globalBestSolution = bestNeighbour;
            globalBestLoss = bestNeighbourLoss;
        }
        else {
            foundBestSolution = true;
        }
    }
    return globalBestSolution;
}

vector<int> hillClimbingStochasticAlgorithm(const vector<vector<bool>> &graphMatrix, const vector<int> &initialColors, const int maxIterations) {
    vector<int> globalBestSolution = generateRandomSolution(initialColors);
    long globalBestLoss = loss(graphMatrix, initialColors);
    random_device generator;
    for (int i = 0; i < maxIterations; i++) {
        vector<vector<int>> neighbours = findNeighbours(globalBestSolution);
        uniform_int_distribution<> distribution(0, neighbours.size()-1);
        const vector<int>& selectedNeighbour = neighbours[distribution(generator)];
        if (const long selectedNeighbourLoss = loss(graphMatrix, selectedNeighbour); selectedNeighbourLoss < globalBestLoss) {
            globalBestSolution = selectedNeighbour;
            globalBestLoss = selectedNeighbourLoss;
        }
    }
    return globalBestSolution;
}

int main(const int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "You must provide algorithm data file name and algorithm name";
        return -1;
    }
    const auto filePath = argv[1];
    const auto algorithmName = argv[2];
    const auto maxIterations = atoi(argv[3]);
    cout<<"File path: "<<filePath<<endl<<"Algorithm name: "<<algorithmName<<endl;
    ifstream f(filePath);
    if (!f.is_open()) {
        cerr << "Failed opening algorithm data file";
        return -1;
    }

    const vector<vector<bool>> graphMatrix = readMatrix(f);
    const auto colors = vector(graphMatrix.size(), 0);

    vector<int> solution;

    switch (algorithmsMap.at(algorithmName)) {
        case FULL_SEARCH:
            solution = fullSearchAlgorithm(graphMatrix);
            break;

        case HILL_CLIMBING:
            solution = hillClimbingAlgorithm(graphMatrix, colors, maxIterations);
            break;

        case HILL_CLIMBING_STOCHASTIC:
            solution = hillClimbingStochasticAlgorithm(graphMatrix, colors, maxIterations);
            break;

        case TABU:
            solution = fullSearchAlgorithm(graphMatrix);
            break;

        default:
            cerr<<"Unknown algorithm name"<<endl;
    }
    cout<<"Found best solution: "<<endl;
    printColors(solution);
    cout<<"Number of unique colors: "<<getUniqueColorsNumber(solution)<<endl;
    return 0;
}
