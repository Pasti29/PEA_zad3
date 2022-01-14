#pragma once

#include <windows.h>
#include <random>
#include <functional>
#include <iostream>
#include <algorithm>
#include <vector>

class GeneticAlgorithm {
private:
	struct pathInfo {
		std::vector<int> path;
		int cost;
	};

	int** array;
	int N, stop, populationSize, mutationMethod, crossoverMethod;
	double mutationVar, crossoverVar;

public:
	GeneticAlgorithm(int **array, int N, int stop, int populationSize, double mutationVar, double crossoverVar, int mutationMethod, int crossoverMethod);
	~GeneticAlgorithm();

private:
	static long long int read_QPC();
	int calcPathCost(std::vector<int> path);
	void setStartPopulation(std::vector<pathInfo>& population);

public:
	void findPath();

};

