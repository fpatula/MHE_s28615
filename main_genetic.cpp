#include <cstdio>
#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include <chrono>
#include <cmath>

using namespace std;

enum MUTATION {
    RANDOM_POINT,
    ALL_POINTS
};

unordered_map<string,MUTATION> const mutationMap = { {"random_point",RANDOM_POINT}, {"all_points", ALL_POINTS}};

enum CROSSOVER {
    ONE_POINT,
    RANDOM_POINTS
};

unordered_map<string,CROSSOVER> const crossoverMap = { {"one_point",ONE_POINT}, {"random_points", RANDOM_POINTS}};

enum END_CONDITION {
    LAST_GENERATION,
    WELL_FIT
};

unordered_map<string,END_CONDITION> const conditionMap = { {"last_generation",LAST_GENERATION}, {"well_fit", WELL_FIT}};


long lastColor;
long maxPopulationSize = 16;
long lastGeneration = 10;

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
    for (int i = 0; i < populationSize; i++) {
        populationFitness[i] = fitness(graphMatrix, population[i]);
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

vector<vector<int>> tournamentSingleSelection(const vector<vector<bool>> &graphMatrix, const vector<vector<int>>& population) {
    const int populationSize = population.size();
    const int firstGroupSize = populationSize/2;
    const int secondGroupSize = populationSize%2 == 0 ? firstGroupSize : firstGroupSize + 1;
    auto selection = vector<vector<int>>(populationSize);
    auto firstSelectionGroup = vector<vector<int>>(firstGroupSize);
    auto secondSelectionGroup = vector<vector<int>>(secondGroupSize);
    uniform_int_distribution firstGroupDistribution(0, firstGroupSize-1);
    uniform_int_distribution secondGroupDistribution(0, secondGroupSize-1);
    int selectionIndex = 0;
    for (int i = 0; i < populationSize; i++) {
        vector<int> firstIndividual = population[firstGroupDistribution(generator)];
        vector<int> secondIndividual = population[secondGroupDistribution(generator)];
        selection[selectionIndex++] = fitness(graphMatrix, firstIndividual) > fitness(graphMatrix, secondIndividual) ? firstIndividual : secondIndividual;
    }
    return selection;
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

bool isLastGeneration(const vector<vector<bool>> &graphMatrix, const vector<vector<int>>& population){
     static long generation = 1;
     return generation++ == lastGeneration;
}

bool isPopulationFitnessSatisfying(const vector<vector<bool>> &graphMatrix, const vector<vector<int>>& population){
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

vector<vector<int>> randomPointCrossover(const vector<vector<int>>& population){
    const long populationSize = population.size();
    const long genotypeSize = population[0].size();
    auto offsprings = vector<vector<int>>(populationSize);
    uniform_real_distribution<> realDistribution(0, 1);
    const double crossoverChance = realDistribution(generator);
    for (int i = 0; i < populationSize; i+=2) {
        const vector<int>& firstParent = population[i];
        const vector<int>& secondParent = population[i+1];
        vector<int> firstOffspring = firstParent;
        vector<int> secondOffspring = secondParent;
        for (int j = 0; j < genotypeSize; j++) {
            if (crossoverChance > 0.5) {
                firstOffspring[j] = secondParent[j];
                secondOffspring[j] = firstParent[j];
            }
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

vector<vector<int>> allPointsMutation(vector<vector<int>>& offsprings){
    const long genotypeSize = offsprings[0].size();
    const int divisor = lastColor + 1;
    uniform_real_distribution<> realDistribution(0, 1);
    for (vector<int> & offspring : offsprings) {
      for (int j = 0; j < genotypeSize; j++) {
          if(realDistribution(generator) <= 0.05){
              if (realDistribution(generator) < 0.5) {
                offspring[j] = mod(offspring[j] + 1, divisor);
              }
              else {
                 offspring[j] = mod(offspring[j] - 1, divisor);
              }
          }
      }
    }
    return offsprings;
}

vector<vector<int>> geneticAlgorithm(const vector<vector<bool>> &graphMatrix, const vector<int> &initialGenotype, vector<vector<int>> (*crossover)(const vector<vector<int>>& population), vector<vector<int>> (*mutation)(vector<vector<int>>& population), bool (*termCondition)(const vector<vector<bool>> &graphMatrix, const vector<vector<int>>& population)) {
    vector<vector<int>> population = elitePopulation(graphMatrix, initialGenotype);
    int generation = 1;
    while(!termCondition(graphMatrix, population)) {
        /*cout<<"Generation: "<<generation<<endl;
        for(const auto& individual: population){
            printColors(individual);
        }*/
        auto selection = tournamentSelection(graphMatrix, population);
        auto offsprings = crossover(selection);
        const auto mutatedOffsprings = mutation(offsprings);
        population = mutatedOffsprings;
        generation++;
    }
    return population;
}

int main(const int argc, char *argv[]) {
    if (argc < 6) {
        cerr << "You must provide algorithm data file name and 4 params";
        return -1;
    }
    const auto filePath = argv[1];
    const auto conditionName = argv[2];
    const auto crossoverName = argv[3];
    const auto mutationName = argv[4];

    maxPopulationSize = atoi(argv[5]);

    ifstream f(filePath);
    if (!f.is_open()) {
        cerr << "Failed opening algorithm data file";
        return -1;
    }
    const vector<vector<bool>> graphMatrix = readMatrix(f);
    const auto colors = vector(graphMatrix.size(), 0);
    bool (*endCondition)(const vector<vector<bool>> &, const vector<vector<int>>&) = &isLastGeneration;
    switch (conditionMap.at(conditionName)) {
        case LAST_GENERATION:
            endCondition = &isLastGeneration;
        break;
        case WELL_FIT:
            endCondition = &isPopulationFitnessSatisfying;
        break;

        default:
            cerr<<"Unknown end condition name"<<endl;
    }

    vector<vector<int>>(*crossover)(const vector<vector<int>>&) = &randomPointCrossover;
    switch (crossoverMap.at(crossoverName)) {
        case ONE_POINT:
            crossover = &onePointCrossover;
        break;
        case RANDOM_POINTS:
            crossover = &randomPointCrossover;
        break;

        default:
            cerr<<"Unknown crossover name"<<endl;
    }

    vector<vector<int>>(*mutation)(vector<vector<int>>& ) = &randomPointMutation;
    switch (mutationMap.at(mutationName)) {
        case RANDOM_POINT:
            mutation = &randomPointMutation;
        break;
        case ALL_POINTS:
            mutation = &allPointsMutation;
        break;

        default:
            cerr<<"Unknown mutation name"<<endl;
    }

    const chrono::steady_clock::time_point start = chrono::steady_clock::now();

    vector<vector<int>> solution = geneticAlgorithm(graphMatrix, colors, crossover, mutation, endCondition);
    int solutionUniqueColors = INT32_MAX;

    const chrono::steady_clock::time_point end = chrono::steady_clock::now();
    cout<<"Found best population: "<<endl;
    vector<int> bestIndividual;

    for(const auto& individual: solution){
        printColors(individual);
        if (const int uniqueColors = getUniqueColorsNumber(individual); uniqueColors < solutionUniqueColors) {
            solutionUniqueColors = uniqueColors;
            bestIndividual = individual;
        }
    }
    cout<<"Found best individual: "<<endl;
    printColors(bestIndividual);
    cout<<"Number of unique colors: "<<solutionUniqueColors<<endl;
    cout<<"Execution time (ms): " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;
    return 0;
}
