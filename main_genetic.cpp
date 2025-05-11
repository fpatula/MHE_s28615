#include <cstdio>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include <chrono>
#include <cmath>

using namespace std;

long lastColor;
long maxPopulationSize = 8;
random_device generator;

int mod(const int number, const int divisor) {
    const int reminder = number % divisor;
    return reminder < 0 ? reminder + divisor : reminder;
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

long fitness(const vector<vector<bool>> &graphMatrix, const vector<int>& colors) {
    const long numberOfUniqueColors = getUniqueColorsNumber(colors);
    long errorWeight = numberOfUniqueColors;
    const long numberOfVertices = graphMatrix.size();
    const long numberOfPoints = numberOfVertices * numberOfVertices;
    const long numberOfColors = lastColor + 1;
    long fitness = numberOfColors + numberOfPoints + numberOfColors * numberOfPoints;
    fitness -= numberOfUniqueColors;
    for (int i = 0; i < colors.size(); i++) {
        const auto& adjacencyRow = graphMatrix[i];
        for (int j = 0; j < adjacencyRow.size(); j++) {
            if (adjacencyRow[j] && colors[i] == colors[j]) {
                fitness -= ++errorWeight;
            }
        }
    }
    return fitness;
}

vector<long> calculatePopulationFitness(const vector<vector<bool>> &graphMatrix, const vector<vector<int>>& population) {
    auto populationFitness = vector<long>(graphMatrix.size());
    const int populationSize = population.size();
    for (int i = 0; i < populationSize; i++) {
        populationFitness[i] = fitness(graphMatrix, population[i]);
    }
    return populationFitness;
}

vector<int> generateRandomSolution(vector<int> colors) {
    for (int & color : colors) {
        uniform_int_distribution<> distribution(0, lastColor);
        color = distribution(generator);
    }
    return colors;
}

vector<vector<int>> initializePopulation(const vector<int>& initialGenome){
    auto population = vector<vector<int>>(maxPopulationSize);
    for (int i = 0; i <= maxPopulationSize; i++) {
      population[i] = generateRandomSolution(initialGenome);
    }
    return population;
}

vector<vector<int>> tournamentSelection(const vector<vector<bool>> &graphMatrix, const vector<vector<int>>& population) {
    const long populationSize = population.size();
    auto selection = vector<vector<int>>(populationSize);
    uniform_int_distribution<> distribution(0, populationSize);
    int selectionIndex = 0;
    for (int i = 0; i < populationSize; i++) {
        vector<int> firstIndividual = population[distribution(generator)];
        vector<int> secondIndividual = population[distribution(generator)];
        selection[selectionIndex++] = fitness(graphMatrix, firstIndividual) > fitness(graphMatrix, secondIndividual) ? firstIndividual : secondIndividual;
    }
    return selection;
}

bool isLastGeneration(const vector<vector<bool>> &graphMatrix, const vector<vector<int>>& population){
     static long generation = 1;
     return generation++ ==  population.size() == 1 || generation == 10;
}

bool isPopulationFitnessSatysfying(const vector<vector<bool>> &graphMatrix, const vector<vector<int>>& population){
    const long numberOfVertices = graphMatrix.size();
    const long numberOfPoints = numberOfVertices * numberOfVertices;
    const long numberOfColors = lastColor + 1;

    const long baseFitness = numberOfPoints + numberOfColors * numberOfPoints;
    long bestPopulationFitness = 0;
    const vector<long> populationFitness = calculatePopulationFitness(graphMatrix, population);
    for (const auto fitness : populationFitness) {
        if (bestPopulationFitness < fitness) {
           bestPopulationFitness = fitness;
        }
    }
    return bestPopulationFitness >= baseFitness;
}

vector<vector<int>> midCrossover(vector<vector<int>>& population){
    const long populationSize = population.size();
    const long genomeSize = population[0].size();
    vector<vector<int>> offsprings = vector<vector<int>>(populationSize);
    for (int i = 0; i < populationSize; i+=2) {
        const vector<int>& firstParent = population[i];
        const vector<int>& secondParent = population[i+1];
        vector<int> firstOffspring = firstParent;
        vector<int> secondOffspring = secondParent;
        for (int j = 0; j < genomeSize/2; j++) {
          firstOffspring[j] = secondParent[j];
          secondOffspring[j] = firstParent[j];
        }
        offsprings[i] = firstOffspring;
        offsprings[i+1] = secondOffspring;
    }
    return offsprings;
}

vector<vector<int>> threePointsCrossover(vector<vector<int>>& population){
    const long populationSize = population.size();
    const long genomeSize = population[0].size();
    vector<vector<int>> offsprings = vector<vector<int>>(populationSize);
    for (int i = 0; i < populationSize; i+=2) {
        const vector<int>& firstParent = population[i];
        const vector<int>& secondParent = population[i+1];
        vector<int> firstOffspring = firstParent;
        vector<int> secondOffspring = secondParent;
        int crossPoint = genomeSize/3;
        for (int j = 0; j < crossPoint; j++) {
            firstOffspring[j] = secondParent[j];
            secondOffspring[j] = firstParent[j];
        }
        for (int j = crossPoint * 2; j < genomeSize; j++) {
            firstOffspring[j] = secondParent[j];
            secondOffspring[j] = firstParent[j];
        }
        offsprings[i] = firstOffspring;
        offsprings[i+1] = secondOffspring;
    }
    return offsprings;
}

vector<vector<int>> randomPointMutation(vector<vector<int>>& offsprings){
    const long genomeSize = offsprings[0].size();
    const int divisor = lastColor + 1;
    random_device generator;
    uniform_real_distribution<> realDistribution(0, 1);
    uniform_int_distribution<> distribution(0, genomeSize - 1);
    for (vector<int> & offspring : offsprings) {
        if(realDistribution(generator) <= 0.02){
            int genomeIndex = distribution(generator);
            offspring[genomeIndex] = mod(offspring[genomeIndex] + 1, divisor);
        }
    }
    return offsprings;
}

vector<vector<int>> allPointsMutation(vector<vector<int>>& offsprings){
    const long genomeSize = offsprings[0].size();
    const int divisor = lastColor + 1;
    random_device generator;
    uniform_real_distribution<> realDistribution(0, 1);
    for (vector<int> & offspring : offsprings) {
      for (int j = 0; j < genomeSize; j++) {
          if(realDistribution(generator) <= 0.01){
              offspring[j] = mod(offspring[j] + 1, divisor);
          }
      }
    }
    return offsprings;
}

vector<vector<int>> geneticAlgorithm(const vector<vector<bool>> &graphMatrix, const vector<int> &initialGenome, vector<vector<int>> (*crossover)(vector<vector<int>>& population), vector<vector<int>> (*mutation)(vector<vector<int>>& population), bool (*termCondition)(const vector<vector<bool>> &graphMatrix, const vector<vector<int>>& population)) {
    int generation = 0;
    vector<vector<int>> population = initializePopulation(initialGenome);
    while(!termCondition(graphMatrix, population)) {
      auto selection = tournamentSelection(graphMatrix, population);
      auto offsprings = crossover(selection);
      const auto mutatedOffsprings = mutation(offsprings);
      cout<<"Generation: "<<generation<<endl;
      population = mutatedOffsprings;
      generation++;
    }
    return population;
}

int main(const int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "You must provide algorithm data file name and algorithm name";
        return -1;
    }
    const auto filePath = argv[1];
    ifstream f(filePath);
    if (!f.is_open()) {
        cerr << "Failed opening algorithm data file";
        return -1;
    }

    const vector<vector<bool>> graphMatrix = readMatrix(f);
    const auto colors = vector(graphMatrix.size(), 0);

    vector<vector<int>> solution = geneticAlgorithm(graphMatrix, colors, midCrossover, allPointsMutation, isLastGeneration);
    int solutionUniqueColors = 0;
    const chrono::steady_clock::time_point start = chrono::steady_clock::now();

    const chrono::steady_clock::time_point end = chrono::steady_clock::now();
    cout<<"Found best population: "<<endl;
    for(const auto& individual: solution){
        printColors(individual);
        int uniqueColors = getUniqueColorsNumber(individual);
        solutionUniqueColors = uniqueColors < solutionUniqueColors ? uniqueColors : solutionUniqueColors;
    }
    cout<<"Number of unique colors: "<<solutionUniqueColors<<endl;
    cout<<"Execution time (ms): " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;
    return 0;
}
