ď»ż#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <string>

#include "Menu.h"
#include "ArrayClass.h"
#include "GeneticAlgorithm.h"

int** ARRAY = nullptr;
int N;

int main() {
	int option, stop = 30, populationSize = 100, mutationMethod = 1, crossoverMethod = 1;
	double mutationVar = 0.01, crossoverVar = 0.8;

	time_t now = std::time(0);
	char* dt = std::ctime(&now);

	std::string str(dt);
	std::replace(str.begin(), str.end(), ' ', '_');
	std::replace(str.begin(), str.end(), ':', '-');
	str.erase(0, 4);
	str.pop_back();

	str = "Wyniki\\result_" + str + ".csv";

	std::ofstream fileResult;
	fileResult.open(str);
	fileResult << "Nr,Rozmiar tablicy,Rozmiar populacji,Wartosc stopu [s],Praw. mutacji,Praw. krzyzowania,Metoda mutacji,Metoda krzyzowania,Najlepsza wartosc" << std::endl;

	while (true) {
		option = Menu::showMainMenu();

		switch (option) {
		case 1:
			if (ARRAY != nullptr) {
				ARRAY = ArrayClass::destroyArray(ARRAY, N);
			}
			ARRAY = ArrayClass::createArrayFromFile(ARRAY, N);
			//ArrayClass::showArray(ARRAY, N);
			//std::cout << "\t";
			//system("pause");
			break;
		case 2:
			stop = Menu::showStopMenu(stop);
			break;
		case 3:
			populationSize = Menu::showPopulationMenu(populationSize);
			break;
		case 4:
			mutationVar = Menu::showMutationVarMenu(mutationVar);
			break;
		case 5:
			crossoverVar = Menu::showCrossoverVarMenu(crossoverVar);
			break;
		case 6:
			mutationMethod = Menu::showMutationMethodMenu(mutationMethod);
			break;
			//case 7:
			//	crossoverMethod = Menu::showCrossoverMethodMenu(crossoverMethod);
			//	break;
		case 8:
			if (ARRAY == nullptr) {
				std::cout << "\tNiezainicjalozowano tabeli.\n";
				system("pause");
			} else {
				int best;
				for (int i = 0; i < 10; i++) {
					fileResult << i + 1 << "," << N << "," << populationSize << "," << stop << "," << mutationVar << "," << crossoverVar << "," << mutationMethod << "," << crossoverMethod;
					GeneticAlgorithm ga(ARRAY, N, stop, populationSize, mutationVar, crossoverVar, mutationMethod, crossoverMethod);
					best = ga.findPath();
					fileResult << "," << best << std::endl;
				}
				system("pause");
			}
			break;
		case 9:
			if (ARRAY != nullptr) {
				ARRAY = ArrayClass::destroyArray(ARRAY, N);
			}
			fileResult.close();
			return 0;
		default:
			break;
		}
	}
}