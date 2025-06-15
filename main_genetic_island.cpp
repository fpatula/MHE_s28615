#include <cstdio>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include <chrono>
#include <cmath>
#include <mutex>
#include <condition_variable>

using namespace std;


mutex populationMtx;
mutex migrationMtx;
list<mutex> gapMtxs;

condition_variable cv;

long lastColor;
long maxPopulationSize = 16;
long lastGeneration = 10;
int populatedIslands = 0;
int islandsNumber;

map<int, vector<vector<int>>> populations;

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
    auto populationFitness = vector<long>(population.size());
    const int populationSize = population.size();
    vector<future<long>> futures;
    for (int i = 0; i < populationSize; i++) {
        futures[i] = async(launch::async, [graphMatrix, population, i] {return fitness(graphMatrix, population[i]);});
    }
    for (int i = 0; i < populationSize; i++) {
        populationFitness[i] = futures[i].get();
    }
    return populationFitness;
}

vector<int> generateRandomSolution(const vector<int>& colors) {
    vector<int> newSolution = colors;
    uniform_int_distribution<> distribution(0, lastColor);
    for (int & color : newSolution) {
        color = distribution(generator);
    }
    return newSolution;
}

vector<vector<int>> initializePopulation(const vector<int>& initialGenotype){
    auto population = vector<vector<int>>(maxPopulationSize);
    for (int i = 0; i < maxPopulationSize; i++) {
      population[i] = generateRandomSolution(initialGenotype);
    }
    return population;
}

vector<vector<int>> elitePopulation(const vector<vector<bool>> &graphMatrix, const vector<int>& initialGenotype){
    auto population = vector<vector<int>>(maxPopulationSize*2);
    for (int i = 0; i < maxPopulationSize*2; i++) {
        population[i] = generateRandomSolution(initialGenotype);
    }
    auto elitePopulation = vector<vector<int>>(maxPopulationSize);
    sort(population.begin(), population.end(), [graphMatrix](const vector<int>& first, const vector<int>& second) {
        return fitness(graphMatrix, first)>fitness(graphMatrix, second);
    });
    for (int i = 0; i < maxPopulationSize; i++) {
        elitePopulation[i] = population[i];
    }
    return elitePopulation;
}

vector<vector<int>> tournamentSelection(const vector<vector<bool>> &graphMatrix, const vector<vector<int>>& population) {
    const int populationSize = population.size();
    auto selection = vector<vector<int>>(populationSize);
    uniform_int_distribution distribution(0, populationSize-1);
    int selectionIndex = 0;
    for (int i = 0; i < populationSize; i++) {
        vector<int> firstIndividual = population[distribution(generator)];
        vector<int> secondIndividual = population[distribution(generator)];
        selection[selectionIndex++] = fitness(graphMatrix, firstIndividual) > fitness(graphMatrix, secondIndividual) ? firstIndividual : secondIndividual;
    }
    return selection;
}

bool isLastGeneration(const long &generation){
     return generation == lastGeneration;
}

vector<vector<int>> onePointCrossover(const vector<vector<int>>& population){
    const long populationSize = population.size();
    const long genotypeSize = population[0].size();
    auto offsprings = vector<vector<int>>(populationSize);
    uniform_int_distribution<> distribution(0, genotypeSize-1);
    const int crossoverPoint = distribution(generator);
    for (int i = 0; i + 1 < populationSize; i+=2) {
        const vector<int>& firstParent = population[i];
        const vector<int>& secondParent = population[i+1];
        vector<int> firstOffspring = firstParent;
        vector<int> secondOffspring = secondParent;
        for (int j = 0; j < crossoverPoint; j++) {
          firstOffspring[j] = secondParent[j];
          secondOffspring[j] = firstParent[j];
        }
        offsprings[i] = firstOffspring;
        offsprings[i+1] = secondOffspring;
    }
    return offsprings;
}

vector<vector<int>> randomPointMutation(vector<vector<int>>& offsprings){
    const long genotypeSize = offsprings[0].size();
    const int divisor = lastColor + 1;
    uniform_real_distribution<> realDistribution(0, 1);
    uniform_int_distribution<> distribution(0, genotypeSize - 1);
    for (vector<int> & offspring : offsprings) {
        if(realDistribution(generator) <= 0.1){
            const int genotypeIndex = distribution(generator);
            if (realDistribution(generator) < 0.5) {
                offspring[genotypeIndex] = mod(offspring[genotypeIndex] + 1, divisor);
            }
            else {
                offspring[genotypeIndex] = mod(offspring[genotypeIndex] - 1, divisor);
            }
        }
    }
    return offsprings;
}

void increasePopulatedIslands(){
    lock_guard lock(migrationMtx);
    populatedIslands++;
}

void addPopulation(const vector<vector<int>> &population, const int &islandNumber){
    lock_guard lock(populationMtx);
    populations[islandNumber] = population;
}

void migratePopulation(const int &islandNumber, const int &migrationRate){
    const auto neighbourKey = mod(islandNumber+1, islandsNumber);
    lock_guard lock(migrationMtx);
    auto population = populations[islandNumber];
    auto neighbourPopulation = populations[neighbourKey];
    uniform_int_distribution<> distribution(0, maxPopulationSize-1);
    for(int i = 0; i < migrationRate; i++){
        const auto index = distribution(generator);
        const auto selectedPopulation = population[index];
        population[index] = neighbourPopulation[index];
        neighbourPopulation[index] = selectedPopulation;
        populations[islandNumber] = population;
        populations[neighbourKey] = neighbourPopulation;
    }
}

vector<vector<int>> geneticAlgorithm(const vector<vector<bool>> &graphMatrix, const vector<int> &initialGenotype, const int islandNumber, const int migrationRate, const int migrationGap) {
    vector<vector<int>> population = elitePopulation(graphMatrix, initialGenotype);
    addPopulation(population, islandNumber);
    int generation = 1;
    while(!isLastGeneration(generation)) {
        auto selection = tournamentSelection(graphMatrix, population);
        auto offsprings = onePointCrossover(selection);
        const auto mutatedOffsprings = randomPointMutation(offsprings);
        population = mutatedOffsprings;
        addPopulation(population, islandNumber);
        if(generation%migrationGap == 0){
            increasePopulatedIslands();
            const auto front = gapMtxs.begin();
            std::unique_lock lock(*next(front, islandNumber));
            if(populatedIslands%islandsNumber == 0) {
               cv.notify_all();
            }
            else {
                cv.wait(lock, [] {
                return populatedIslands%islandsNumber == 0;
                });
            }
            migratePopulation(islandNumber, migrationRate);
        }

        generation++;
    }
    return population;
}

int main(const int argc, char *argv[]) {
    if (argc < 7) {
        cerr << "You must provide algorithm data file name and 5 params";
        return -1;
    }
    const auto filePath = argv[1];
    islandsNumber = atoi(argv[2]);
    const auto migrationRate = atoi(argv[3]);
    const auto migrationGap = atoi(argv[4]);
    lastGeneration = atoi(argv[5]);
    maxPopulationSize = atoi(argv[6]);

    ifstream f(filePath);
    if (!f.is_open()) {
        cerr << "Failed opening algorithm data file";
        return -1;
    }
    const vector<vector<bool>> graphMatrix = readMatrix(f);
    const auto colors = vector(graphMatrix.size(), 0);

    const chrono::steady_clock::time_point start = chrono::steady_clock::now();
    std::vector<std::thread> ThreadVector;

    for(int i = 0; i < islandsNumber; i++)
    {
        gapMtxs.emplace_back();
        ThreadVector.emplace_back([&, i] {geneticAlgorithm(graphMatrix, colors, i, migrationRate, migrationGap);});
    }
    for(auto& t: ThreadVector)
    {
        t.join();
    }

    const chrono::steady_clock::time_point end = chrono::steady_clock::now();

    int solutionUniqueColors = INT32_MAX;

    cout<<"Found best population: "<<endl;
    vector<int> bestIndividual;

    for(const auto&[first, second]: populations){
        for(const auto& individual: second){
            printColors(individual);
            if (const int uniqueColors = getUniqueColorsNumber(individual); uniqueColors < solutionUniqueColors) {
                solutionUniqueColors = uniqueColors;
                bestIndividual = individual;
            }
        }

    }
    cout<<"Found best individual: "<<endl;
    printColors(bestIndividual);
    cout<<"Number of unique colors: "<<solutionUniqueColors<<endl;
    cout<<"Execution time (ms): " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;
    return 0;
}
