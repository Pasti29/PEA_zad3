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
/*
 * Funkcja obliczaj�ca koszt �cie�ki podanej jako parametr.
 */
int GeneticAlgorithm::calcPathCost(std::vector<int> path) {
	int cost = 0;
	for (int i = 0; i < path.size() - 1; ++i) {
		cost += array[path[i]][path[i + 1]];
	}
	cost += array[path[N - 1]][path[0]];
	return cost;
}
/*
	Funckcja tworz�ca pierwsz� populacj�.
	Pocz�tkowa populacji jest wielko�ci populationSize, a osobniki s� losowe
*/
void GeneticAlgorithm::setStartPopulation(std::vector<pathInfo>& population) {
	auto rng = std::default_random_engine{ rd() };

	std::vector<int> base;
	base.reserve(N);

	for (int i = 0; i <= N - 1; i++) {
		base.push_back(i);
	}

	for (int i = 0; i < populationSize; i++) {
		pathInfo newChromosome;
		newChromosome.path = base;
		std::shuffle(std::begin(newChromosome.path), std::end(newChromosome.path), rng);
		newChromosome.cost = calcPathCost(newChromosome.path);
		population.push_back(newChromosome);
	}
}

int GeneticAlgorithm::minPathId(std::vector<pathInfo> pool) {
	int min = INT_MAX;
	int i = 0;
	int index = 0;
	for (pathInfo route : pool) {
		if (route.cost < min) {
			index = i;
			min = route.cost;
		}
		i++;
	}
	return index;
}

std::vector<GeneticAlgorithm::pathInfo> GeneticAlgorithm::tournament(std::vector<pathInfo> population) {
	auto rng = std::default_random_engine{ rd() };

	std::vector<pathInfo> matingPool;
	matingPool.reserve(population.size());

	std::vector<pathInfo> tournamentPool;
	int tournamentSize = 5 * populationSize / 100;
	tournamentPool.reserve(tournamentSize);

	for (int i = 0; i < population.size(); i++) {
		tournamentPool.clear();
		std::sample(population.begin(), population.end(), std::back_inserter(tournamentPool), tournamentSize, rng);
		int minId = minPathId(tournamentPool);
		matingPool.push_back(tournamentPool[minId]);
	}

	return matingPool;
}

std::tuple<GeneticAlgorithm::pathInfo, GeneticAlgorithm::pathInfo> GeneticAlgorithm::crossoverPMX(pathInfo parent1, pathInfo parent2) {
	auto rng = std::default_random_engine{ rd() };
	std::uniform_int_distribution<int> distrubution(0, N - 1);
	auto random = bind(distrubution, rng);

	int k1, k2;
	k1 = random();
	do {
		k2 = random();
	} while (k1 == k2);
	if (k2 < k1) {
		int tmp = k1;
		k1 = k2;
		k2 = tmp;
	}

	pathInfo child1, child2;
	child1.path.resize(N, INT_MAX);
	child2.path.resize(N, INT_MAX);

	std::map<int, int> map1{}, map2{};
	std::map<int, int> child1Count{}, child2Count{};
	for (int i = k1; i <= k2; i++) {
		child1.path[i] = parent2.path[i];
		child2.path[i] = parent1.path[i];
		map1.insert({ parent1.path[i], parent2.path[i] });
		map2.insert({ parent2.path[i], parent1.path[i] });
		child1Count.insert({ child1.path[i], 1 });
		child2Count.insert({ child2.path[i], 1 });
	}

	for (int i = 0; i < N; i++) {
		if (i >= k1 && i <= k2) continue;

		auto p1 = std::find(child1.path.begin(), child1.path.end(), parent1.path[i]);
		if (p1 == child1.path.end()) {
			child1.path[i] = parent1.path[i];
			auto result = child1Count.insert({ child1.path[i], 1 });
			if (result.second == false) {
				result.first->second++;
			}
		}

		auto p2 = std::find(child2.path.begin(), child2.path.end(), parent2.path[i]);
		if (p2 == child2.path.end()) {
			child2.path[i] = parent2.path[i];
			auto result = child2Count.insert({ child2.path[i], 1 });
			if (result.second == false) {
				result.first->second++;
			}
		}
	}

	for (int i = 0; i < N; i++) {
		if (child1.path[i] == INT_MAX) {
			auto search = map1.find(parent1.path[i]);
			if (search != map1.end()) {
				child1.path[i] = search->second;
				auto result = child1Count.insert({ child1.path[i], 1 });
				while (result.second == false) {
					search = map1.find(child1.path[i]);
					child1.path[i] = search->second;
					result = child1Count.insert({ child1.path[i], 1 });
				}
			} else {
				search = map2.find(parent1.path[i]);
				child1.path[i] = search->second;
				auto result = child1Count.insert({ child1.path[i], 1 });
				while (result.second == false) {
					search = map2.find(child1.path[i]);
					child1.path[i] = search->second;
					result = child1Count.insert({ child1.path[i], 1 });
				}
			}
		}
		if (child2.path[i] == INT_MAX) {
			auto search = map1.find(parent2.path[i]);
			if (search != map1.end()) {
				child2.path[i] = search->second;
				auto result = child2Count.insert({ child2.path[i], 1 });
				while (result.second == false) {
					search = map1.find(child2.path[i]);
					child2.path[i] = search->second;
					result = child2Count.insert({ child2.path[i], 1 });
				}
			} else {
				search = map2.find(parent2.path[i]);
				child2.path[i] = search->second;
				auto result = child2Count.insert({ child2.path[i], 1 });
				while (result.second == false) {
					search = map2.find(child2.path[i]);
					child2.path[i] = search->second;
					result = child2Count.insert({ child2.path[i], 1 });
				}
			}
		}
	}
	child1.cost = calcPathCost(child1.path);
	child2.cost = calcPathCost(child2.path);

	return { child1, child2 };
}

void GeneticAlgorithm::mutationInversion(pathInfo& chromosome) {
	auto rng = std::default_random_engine{ rd() };
	std::uniform_int_distribution<int> distrubution(0, N - 1);
	auto random = bind(distrubution, rng);

	int k1, k2;
	k1 = random();
	do {
		k2 = random();
	} while (k1 == k2);
	if (k2 < k1) {
		int tmp = k1;
		k1 = k2;
		k2 = tmp;
	}

	for (int i = 0; i <= (k2 - k1 + 1) / 2; i++) {
		int tmp = chromosome.path[k1 + i];
		chromosome.path[k1 + i] = chromosome.path[k2 - i];
		chromosome.path[k2 - i] = tmp;
	}

	chromosome.cost = calcPathCost(chromosome.path);
}

int GeneticAlgorithm::findBest(std::vector<pathInfo> population) {
	int min = INT_MAX;
	for (pathInfo route : population) {
		if (min > route.cost) {
			min = route.cost;
		}
	}
	return min;
}

int GeneticAlgorithm::findPath() {
	long long int frequency, start;
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);

	std::vector<int> bestInGeneration;

	std::vector<pathInfo> population;
	population.reserve(populationSize);
	int oldPopulationSize = populationSize;
	setStartPopulation(population);

	std::vector<pathInfo> newGeneration;
	newGeneration.reserve(population.size());
	pathInfo parent1, parent2, child1, child2;

	auto rng = std::default_random_engine{ rd() };
	std::uniform_real_distribution<> distribution(0, 1);
	auto randomReal = bind(distribution, rng);

	

	start = read_QPC();
	while ((read_QPC() - start) / frequency < stop && population.size() >= 2) {
		bestInGeneration.push_back(findBest(population));
		//std::cout << bestInGeneration.back() << " " << population.size() << "\n";

		population = tournament(population);

		do {
			int index = rng() % population.size();
			parent1 = population[index];
			population.erase(population.begin() + index);
			index = rng() % population.size();
			parent2 = population[index];
			population.erase(population.begin() + index);

			if (randomReal() <= crossoverVar) {
				std::tie(child1, child2) = crossoverPMX(parent1, parent2);
				if (randomReal() <= mutationVar) {
					mutationInversion(child1);
				}
				if (randomReal() <= mutationVar) {
					mutationInversion(child2);
				}
				newGeneration.push_back(child1);
				newGeneration.push_back(child2);
			}
		} while (population.size() >= 2);
		population = newGeneration;
		oldPopulationSize = population.size();
		newGeneration.clear();

	}
	return bestInGeneration.back();
	//system("pause");
}
