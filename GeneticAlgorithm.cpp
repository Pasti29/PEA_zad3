#include "GeneticAlgorithm.h"

GeneticAlgorithm::GeneticAlgorithm(int** array, int N, int stop, int populationSize, double mutationVar, double crossoverVar, int mutationMethod, int crossoverMethod) {
	this->array = array;
	this->N = N;
	this->stop = stop;
	this->populationSize = populationSize;
	this->mutationVar = mutationVar;
	this->crossoverVar = crossoverVar;
	this->mutationMethod = mutationMethod;
	this->crossoverMethod = crossoverMethod;
}

GeneticAlgorithm::~GeneticAlgorithm() {

}
/*
 * Funkcja potrzebna do obliczania czasu.
 */
long long int GeneticAlgorithm::read_QPC() {
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	return ((long long int) count.QuadPart);
}

int GeneticAlgorithm::calcPathCost(std::vector<int> path) {
	int cost = 0;
	for (int i = 0; i < path.size() - 1; ++i) {
		cost += array[path[i]][path[i + 1]];
	}
	cost += array[path[N - 1]][path[0]];
	return cost;
}

void GeneticAlgorithm::setStartPopulation(std::vector<pathInfo>& population) {
	std::random_device r;
	//auto rd = std::random_device{};
	auto rng = std::default_random_engine{ r() };

	std::vector<int> base;
	for (int i = 0; i <= N - 1; i++) {
		base.push_back(i);
	}

	for (int i = 0; i < populationSize; i++) {
		pathInfo newMan;
		newMan.path = base;
		std::shuffle(std::begin(newMan.path), std::end(newMan.path), rng);
		newMan.cost = calcPathCost(newMan.path);
		population.push_back(newMan);
	}
}

void GeneticAlgorithm::findPath() {
	std::vector<pathInfo> population;
	setStartPopulation(population);
}
