#include "GeneticAlgorithm.h"

/*
	Konstruktor inicjalizuj�cy wszystkie potrzebne zmienne.

	Wej�cia:
		array - tablica s�siedztwa
		N - wielko�� tablicy array
		stop - liczba oznaczaj�ca kryterium stopu w sekundach
		populationSize - wielko�� populacji pocz�tkowej
		mutationVar - prawdopodobie�stwo mutacji
		crossoverVar - prawdopodobie�stwo krzy�owania
		mutationMethod - warto�� oznaczaj�ca wybran� metod� mutacji
		crossoverMethod - warto�� oznaczaj�ca wybran� metod� mutacji
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
	Funkcja obliczaj�ca koszt �cie�ki.

	Wej�cia:
		path - �cie�ka do obliczenia kosztu
	Wyj�cie:
		cost - koszt �cie�ki
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
	Funkcja tworz�ca pierwsz� populacj�.
	Pocz�tkowa populacji jest wielko�ci populationSize, a chromosomy s� losowe.
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
	Funkcja pomocnicza dla funkcji tournament, znajduj�c� chromosom o najni�szym koszcie.

	Wej�cia:
		sample - pula chromosom�w
	Wyj�cie:
		index - indeks chromosomu o najkr�tszym koszcie
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

	Wej�cia:
		population - populacja zg�oszona do turnieju
	Wa�ne zmienne:
		tournamentPool - pula chromosom�w wybrania do turnieju
		tournamentSize - wielko�� puli tournamentPool, wyznaczona na 20% wielko�ci populacji
	Wyj�cia:
		matingPool - ko�cowa populacja wybrania do krzy�owania

	Funkcja losowo wybiera chromosomy do turnieju, a nast�pnie wywo�uje funkcj� minPathId, kt�ra
	zwraca indeks chromosomu o najni�szym koszcie. Liczba wykonanych turniej�w jest taka sama
	jak wielko�� populacji.
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
	Funkcja wykonuj�ca metod� krzy�owania PMX - Partially Mapped Crossover

	Wej�cia:
		parent1, parent2 - chromosomy wybrane do krzy�owania
	Wa�ne zmienne:
		random - zwraca losow� ca�kowit� liczb� z przedzia�u <0, N - 1>,
			gdzie N - wielko�� tablicy s�siedztwa
		k1, k2 - losowe miasta
		map1, map2 - struktura mapy dzia�aj�ca jak tabela odwzorowa� (map2 jest odwrotno�ci� map1, czyli
			klucze w map1 s� warto�ciami w map2 itp.)
		child1Count, child2Count - struktura mapy oznaczaj�ca ile razy miasto znajduje si� na �cie�ce
			chromosomu potomnego
	Wyj�cia:
		child1, child2 - chromosomy potomne utworzone z krzy�owania

	Funkcja wykonuje si� wed�ug tego schematu:
		1. Wylosowanie dw�ch miast
		2. Wykonanie transpozycji w podci�gu od k1 do k2, dodanie element�w do tablicy odwzorowa� i odnotowanie
			zmian liczno�ci w child1Count i child2Count
		3. Przepisanie z rodzica do potomka tych miast, kt�re nie zosta�y wpisane w kroku 2
		4. Dla miejsca w �cie�ce, kt�re nie maja jeszcze przypisanego miasta, wstawiamy miasto zgodnie z tabel� odwzorowa�
		5. Je�eli dodanie miasta oznacza�oby powstanie duplikatu w �cie�ce, to ponownie wstawiane jest miasto zgodnie
			z tabel� odwzorowa�
		6. Wracaj do kroku 5, dop�ki dodanie miasta nie oznacza utworzenia duplikatu
		7. Wracaj do kroku 4, dop�ki wszystkie miejsca w �cie�ce nie b�d� mia�y przydzielonego miasta
		8. Oblicz koszt utworzonych chromosom�w potomnych
		9. Zwr�� chromosomy potomne
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
	Funkcja wykonuj�ca metod� mutacji - inversion.

	Wej�cia:
		chromosome - chromosom, kt�ry ma zosta� poddany mutacji
	Wa�ne zmienne:
		random - zwraca losow� ca�kowit� liczb� z przedzia�u <0, N - 1>,
			gdzie N - wielko�� tablicy s�siedztwa
		k1, k2 - losowe miasta

	Funkcja wybiera losowy podci�g (od k1 do k2 ) miast i zamienia ich kolejno��.
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
	Funkcja wykonuj�ca metod� mutacji - transposition.

	Wej�cia:
		chromosome - chromosom, kt�ry ma zosta� poddany mutacji
	Wa�ne zmienne:
		random - zwraca losow� ca�kowit� liczb� z przedzia�u <0, N - 1>,
			gdzie N - wielko�� tablicy s�siedztwa
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
	Funkcja zwraca najni�szy koszt w�r�d chromosom�w z populacji.

	Wej�cia:
		population - populacja chromosom�w
	Wyj�cia:
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
	G��wna funkcja programu.

	Wa�ne zmienne:
		population - populacja chromosom�w
		newGeneration - nowa generacja chromosom�w
		parent1, parent2 - rodzice wybrani do krzy�owania
		child1, child2 - potomkowie otrzymani z krzy�owania
		randomReal - zwraca losow� rzeczywist� liczb� z zakresu <0, 1>
		bestInGeneration - lista przechowuj�ca warto�ci najmniejszych koszt�w dla ka�dej generacji
	Wyj�cia:
		bestInGeneration.back() - znaleziony przez algorytm najni�szy koszt

	Funkcja wykonuje si� wed�ug tego schematu:
		1. Utworzenie populacji pocz�tkowych chromosom�w
		2. Sprawdzenie warunku zatrzymania
			2.1. Selekcja turniejowa chromosom�w - wybranie populacji macierzystej
				2.2.1. Krzy�owanie losowych chromosom�w (zale�y od prawdopodobie�stwa w crossoverVal)
				2.2.2. Mutacja otrzymanych chromosom�w (zale�y od prawdopodobie�stwa w mutationVal)
				2.2.3. Wr�� do kroku 2.2.1 w populacji znajduj� si� jeszcze chromosomy do krzy�owania
			2.2 Utworzenie nowej populacji
			2.3 Powr�t do kroku 2
		3. Zwr�cenie znalezionego najni�szego kosztu
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