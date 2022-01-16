#pragma once

#include <windows.h>
#include <random>
#include <functional>
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <map>

class GeneticAlgorithm {
private:
	struct pathInfo {
		std::vector<int> path;
		int cost = INT_MAX;
	};

	int** array;
	int N, stop, populationSize, mutationMethod, crossoverMethod;
	double mutationVar, crossoverVar;
	std::random_device rd;

public:
	GeneticAlgorithm(int **array, int N, int stop, int populationSize, double mutationVar, double crossoverVar, int mutationMethod, int crossoverMethod);
	~GeneticAlgorithm();

private:
	static long long int read_QPC();
	int calcPathCost(std::vector<int> path);
	void setStartPopulation(std::vector<pathInfo>& population);
	int minPathId(std::vector<pathInfo> pool);
	std::vector<pathInfo> tournament(std::vector<pathInfo> population);
	std::tuple<pathInfo, pathInfo> crossoverPMX(pathInfo parent1, pathInfo parent2);
	void mutationInversion(pathInfo& chromosome);
	int findBest(std::vector<pathInfo> population);

public:
	int findPath();

};

