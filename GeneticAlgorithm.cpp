#include "GeneticAlgorithm.h"

/*
	Konstruktor inicjalizujący wszystkie potrzebne zmienne.

	Wejścia:
		array - tablica sąsiedztwa
		N - wielkość tablicy array
		stop - liczba oznaczająca kryterium stopu w sekundach
		populationSize - wielkość populacji początkowej
		mutationVar - prawdopodobieństwo mutacji
		crossoverVar - prawdopodobieństwo krzyżowania
		mutationMethod - wartość oznaczająca wybraną metodę mutacji
		crossoverMethod - wartość oznaczająca wybraną metodę mutacji
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
	Funkcja obliczająca koszt ścieżki.

	Wejścia:
		path - ścieżka do obliczenia kosztu
	Wyjście:
		cost - koszt ścieżki
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
	Funkcja tworząca pierwszą populację.
	Początkowa populacji jest wielkości populationSize, a osobniki są losowe.
*/
void GeneticAlgorithm::setStartPopulation(std::vector<pathInfo>& population) {
	auto rng = std::default_random_engine{ rd() };

	std::vector<int> base;
	base.reserve(N);

	for (int i = 0; i <= N - 1; i++) {
		base.push_back(i);
	}

	for (int i = 0; i < populationSize; i++) {
		pathInfo newIndividual;
		newIndividual.path = base;
		std::shuffle(std::begin(newIndividual.path), std::end(newIndividual.path), rng);
		newIndividual.cost = calcPathCost(newIndividual.path);
		population.push_back(newIndividual);
	}
}
/*
	Funkcja pomocnicza dla funkcji tournament, znajdującą
	osobnika o najniższym koszcie.

	Wejścia:
		sample - pula osobników
	Wyjście:
		index - indeks osobnika o najkrótszym koszcie
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

	Wejścia:
		population - populacja zgłoszona do turnieju
	Ważne zmienne:
		tournamentPool - pula osobników wybrania do turnieju
		tournamentSize - wielkość puli tournamentPool, wyznaczona na 20% wielkości populacji
	Wyjścia:
		matingPool - końcowa populacja wybrania do krzyżowania

	Funkcja losowo wybiera osobników do turnieju, a następnie wywołuje funkcję minPathId, która zwraca indeks osobnika o najniższym koszcie. Liczba wykonanych turniejów jest taka sama
	jak wielkość populacji.
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
	Funkcja wykonująca metodę krzyżowania PMX - Partially Mapped Crossover

	Wejścia:
		parent1, parent2 - osobniki wybrane do krzyżowania
	Ważne zmienne:
		random - zwraca losową całkowitą liczbę z przedziału <0, N - 1>,
			gdzie N - wielkość tablicy sąsiedztwa
		k1, k2 - losowe miasta
		map1, map2 - struktura mapy działająca jak tabela odwzorowań (map2 jest odwrotnością map1, czyli
			klucze w map1 są wartościami w map2 itp.)
		child1Count, child2Count - struktura mapy oznaczająca ile razy miasto znajduje się na ścieżce
			osobnika potomnego
	Wyjścia:
		child1, child2 - osobniki potomne utworzone z krzyżowania

	Funkcja wykonuje się według tego schematu:
		1. Wylosowanie dwóch miast
		2. Wykonanie transpozycji w podciągu od k1 do k2, dodanie elementów do tablicy odwzorowań i odnotowanie zmian liczności w child1Count i child2Count
		3. Przepisanie z rodzica do potomka tych miast, które nie zostały wpisane w kroku 2
		4. Dla miejsca w ścieżce, które nie maja jeszcze przypisanego miasta, wstawiamy miasto zgodnie z tabelą odwzorowań
		5. Jeżeli dodanie miasta oznaczałoby powstanie duplikatu w ścieżce, to ponownie wstawiane jest miasto zgodnie z tabelą odwzorowań
		6. Wracaj do kroku 5, dopóki dodanie miasta nie oznacza utworzenia duplikatu
		7. Wracaj do kroku 4, dopóki wszystkie miejsca w ścieżce nie będą miały przydzielonego miasta
		8. Oblicz koszt utworzonych osobników potomnych
		9. Zwróć osobniki potomne
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
	Funkcja wykonująca metodę mutacji - inversion.

	Wejścia:
		individual - osobnik, który ma zostać poddany mutacji
	Ważne zmienne:
		random - zwraca losową całkowitą liczbę z przedziału <0, N-1>,
			gdzie N - wielkość tablicy sąsiedztwa
		k1, k2 - losowe miasta

	Funkcja wybiera losowy podciąg (od k1 do k2 ) miast i zamienia ich kolejność.
*/
void GeneticAlgorithm::mutationInversion(pathInfo& individual) {
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
		int tmp = individual.path[k1 + i];
		individual.path[k1 + i] = individual.path[k2 - i];
		individual.path[k2 - i] = tmp;
	}

	individual.cost = calcPathCost(individual.path);
}
/*
	Funkcja wykonująca metodę mutacji - transposition.

	Wejścia:
		individual - osobnik, który ma zostać poddany mutacji
	Ważne zmienne:
		random - zwraca losową całkowitą liczbę z przedziału <0, N-1>,
			gdzie N - wielkość tablicy sąsiedztwa
		k1, k2 - losowe miasta

	Funkcja zamienia dwa losowo wybrane miasta miejscami.
*/
void GeneticAlgorithm::mutationTransposition(pathInfo& individual) {
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

	int tmp = individual.path[k1];
	individual.path[k1] = individual.path[k2];
	individual.path[k2] = tmp;

	individual.cost = calcPathCost(individual.path);
}
/*
	Funkcja zwraca najniższy koszt wśród osobników z populacji.

	Wejścia:
		population - populacja osobników
	Wyjścia:
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
	Główna funkcja programu.

	Ważne zmienne:
		population - populacja osobników
		newGeneration - nowa generacja osobników
		parent1, parent2 - rodzice wybrani do krzyżowania
		child1, child2 - potomkowie otrzymani z krzyżowania
		randomReal - zwraca losową rzeczywistą liczbę z zakresu <0, 1>
		bestInGeneration - lista przechowująca wartości najmniejszych kosztów dla każdej generacji
	Wyjścia:
		bestInGeneration.back() - znaleziony przez algorytm najniższy koszt

	Funkcja wykonuje się według tego schematu:
		1. Utworzenie populacji początkowych osobników
		2. Sprawdzenie warunku zatrzymania
			2.1. Selekcja turniejowa osobników - wybranie populacji macierzystej
				2.2.1. Krzyżowanie losowych osobników (zależy od prawdopodobieństwa w crossoverVal)
				2.2.2. Mutacja otrzymanych osobników (zależy od prawdopodobieństwa w mutationVal)
				2.2.3. Wróć do kroku 2.2.1 w populacji znajdują się jeszcze osobniki do krzyżowania
			2.2 Utworzenie nowej populacji
			2.3 Powrót do kroku 2
		3. Zwrócenie znalezionego najniższego kosztu
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