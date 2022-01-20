#include "GeneticAlgorithm.h"

/*
	Konstruktor inicjalizuj¹cy wszystkie potrzebne zmienne.

	Wejœcia:
		array - tablica s¹siedztwa
		N - wielkoœæ tablicy array
		stop - liczba oznaczaj¹ca kryterium stopu w sekundach
		populationSize - wielkoœæ populacji pocz¹tkowej
		mutationVar - prawdopodobieñstwo mutacji
		crossoverVar - prawdopodobieñstwo krzy¿owania
		mutationMethod - wartoœæ oznaczaj¹ca wybran¹ metodê mutacji
		crossoverMethod - wartoœæ oznaczaj¹ca wybran¹ metodê mutacji
*/
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
/*
	Destruktor
*/
GeneticAlgorithm::~GeneticAlgorithm() {
}
/*
	Funkcja potrzebna do obliczania czasu.
 */
long long int GeneticAlgorithm::read_QPC() {
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	return ((long long int) count.QuadPart);
}
/*
	Funkcja obliczaj¹ca koszt œcie¿ki.

	Wejœcia:
		path - œcie¿ka do obliczenia kosztu
	Wyjœcie:
		cost - koszt œcie¿ki
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
	Funkcja tworz¹ca pierwsz¹ populacjê.
	Pocz¹tkowa populacji jest wielkoœci populationSize, a chromosomy s¹ losowe.
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
/*
	Funkcja pomocnicza dla funkcji tournament, znajduj¹c¹ chromosom o najni¿szym koszcie.

	Wejœcia:
		sample - pula chromosomów
	Wyjœcie:
		index - indeks chromosomu o najkrótszym koszcie
*/
int GeneticAlgorithm::minPathId(std::vector<pathInfo> sample) {
	int min = INT_MAX;
	int i = 0;
	int index = 0;
	for (pathInfo route : sample) {
		if (route.cost < min) {
			index = i;
			min = route.cost;
		}
		i++;
	}
	return index;
}
/*
	Funkcja selekcji turniejowej.

	Wejœcia:
		population - populacja zg³oszona do turnieju
	Wa¿ne zmienne:
		tournamentPool - pula chromosomów wybrania do turnieju
		tournamentSize - wielkoœæ puli tournamentPool, wyznaczona na 20% wielkoœci populacji
	Wyjœcia:
		matingPool - koñcowa populacja wybrania do krzy¿owania

	Funkcja losowo wybiera chromosomy do turnieju, a nastêpnie wywo³uje funkcjê minPathId, która
	zwraca indeks chromosomu o najni¿szym koszcie. Liczba wykonanych turniejów jest taka sama
	jak wielkoœæ populacji.
*/
std::vector<GeneticAlgorithm::pathInfo> GeneticAlgorithm::tournament(std::vector<pathInfo> population) {
	auto rng = std::default_random_engine{ rd() };

	std::vector<pathInfo> matingPool;
	matingPool.reserve(population.size());

	std::vector<pathInfo> tournamentPool;
	int tournamentSize = 20 * population.size() / 100;
	if (tournamentSize <= 1)
		return population;
	tournamentPool.reserve(tournamentSize);

	for (int i = 0; i < population.size(); i++) {
		tournamentPool.clear();
		std::sample(population.begin(), population.end(), std::back_inserter(tournamentPool), tournamentSize, rng);
		int minId = minPathId(tournamentPool);
		matingPool.push_back(tournamentPool[minId]);
	}

	return matingPool;
}
/*
	Funkcja wykonuj¹ca metodê krzy¿owania PMX - Partially Mapped Crossover

	Wejœcia:
		parent1, parent2 - chromosomy wybrane do krzy¿owania
	Wa¿ne zmienne:
		random - zwraca losow¹ ca³kowit¹ liczbê z przedzia³u <0, N - 1>,
			gdzie N - wielkoœæ tablicy s¹siedztwa
		k1, k2 - losowe miasta
		map1, map2 - struktura mapy dzia³aj¹ca jak tabela odwzorowañ (map2 jest odwrotnoœci¹ map1, czyli
			klucze w map1 s¹ wartoœciami w map2 itp.)
		child1Count, child2Count - struktura mapy oznaczaj¹ca ile razy miasto znajduje siê na œcie¿ce
			chromosomu potomnego
	Wyjœcia:
		child1, child2 - chromosomy potomne utworzone z krzy¿owania

	Funkcja wykonuje siê wed³ug tego schematu:
		1. Wylosowanie dwóch miast
		2. Wykonanie transpozycji w podci¹gu od k1 do k2, dodanie elementów do tablicy odwzorowañ i odnotowanie
			zmian licznoœci w child1Count i child2Count
		3. Przepisanie z rodzica do potomka tych miast, które nie zosta³y wpisane w kroku 2
		4. Dla miejsca w œcie¿ce, które nie maja jeszcze przypisanego miasta, wstawiamy miasto zgodnie z tabel¹ odwzorowañ
		5. Je¿eli dodanie miasta oznacza³oby powstanie duplikatu w œcie¿ce, to ponownie wstawiane jest miasto zgodnie
			z tabel¹ odwzorowañ
		6. Wracaj do kroku 5, dopóki dodanie miasta nie oznacza utworzenia duplikatu
		7. Wracaj do kroku 4, dopóki wszystkie miejsca w œcie¿ce nie bêd¹ mia³y przydzielonego miasta
		8. Oblicz koszt utworzonych chromosomów potomnych
		9. Zwróæ chromosomy potomne
*/
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
/*
	Funkcja wykonuj¹ca metodê mutacji - inversion.

	Wejœcia:
		chromosome - chromosom, który ma zostaæ poddany mutacji
	Wa¿ne zmienne:
		random - zwraca losow¹ ca³kowit¹ liczbê z przedzia³u <0, N - 1>,
			gdzie N - wielkoœæ tablicy s¹siedztwa
		k1, k2 - losowe miasta

	Funkcja wybiera losowy podci¹g (od k1 do k2 ) miast i zamienia ich kolejnoœæ.
*/
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
/*
	Funkcja wykonuj¹ca metodê mutacji - transposition.

	Wejœcia:
		chromosome - chromosom, który ma zostaæ poddany mutacji
	Wa¿ne zmienne:
		random - zwraca losow¹ ca³kowit¹ liczbê z przedzia³u <0, N - 1>,
			gdzie N - wielkoœæ tablicy s¹siedztwa
		k1, k2 - losowe miasta

	Funkcja zamienia dwa losowo wybrane miasta miejscami.
*/
void GeneticAlgorithm::mutationTransposition(pathInfo& chromosome) {
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

	int tmp = chromosome.path[k1];
	chromosome.path[k1] = chromosome.path[k2];
	chromosome.path[k2] = tmp;

	chromosome.cost = calcPathCost(chromosome.path);
}
/*
	Funkcja zwraca najni¿szy koszt wœród chromosomów z populacji.

	Wejœcia:
		population - populacja chromosomów
	Wyjœcia:
		min - minimalny koszt
*/
int GeneticAlgorithm::findBest(std::vector<pathInfo> population) {
	int min = INT_MAX;
	for (pathInfo route : population) {
		if (min > route.cost) {
			min = route.cost;
		}
	}
	return min;
}
/*
	G³ówna funkcja programu.

	Wa¿ne zmienne:
		population - populacja chromosomów
		newGeneration - nowa generacja chromosomów
		parent1, parent2 - rodzice wybrani do krzy¿owania
		child1, child2 - potomkowie otrzymani z krzy¿owania
		randomReal - zwraca losow¹ rzeczywist¹ liczbê z zakresu <0, 1>
		bestInGeneration - lista przechowuj¹ca wartoœci najmniejszych kosztów dla ka¿dej generacji
	Wyjœcia:
		bestInGeneration.back() - znaleziony przez algorytm najni¿szy koszt

	Funkcja wykonuje siê wed³ug tego schematu:
		1. Utworzenie populacji pocz¹tkowych chromosomów
		2. Sprawdzenie warunku zatrzymania
			2.1. Selekcja turniejowa chromosomów - wybranie populacji macierzystej
				2.2.1. Krzy¿owanie losowych chromosomów (zale¿y od prawdopodobieñstwa w crossoverVal)
				2.2.2. Mutacja otrzymanych chromosomów (zale¿y od prawdopodobieñstwa w mutationVal)
				2.2.3. Wróæ do kroku 2.2.1 w populacji znajduj¹ siê jeszcze chromosomy do krzy¿owania
			2.2 Utworzenie nowej populacji
			2.3 Powrót do kroku 2
		3. Zwrócenie znalezionego najni¿szego kosztu
*/
int GeneticAlgorithm::findPath() {
	long long int frequency, start;
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);

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

	std::list<int> bestInGeneration;

	start = read_QPC();
	while ((read_QPC() - start) / frequency < stop && population.size() >= 2) {
		bestInGeneration.push_back(findBest(population));

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
					mutationMethod == 1 ? mutationInversion(child1) : mutationTransposition(child1);
				}
				if (randomReal() <= mutationVar) {
					mutationMethod == 1 ? mutationInversion(child2) : mutationTransposition(child2);
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
}