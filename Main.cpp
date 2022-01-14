#include <iostream>
#include "Menu.h"
#include "ArrayClass.h"

using namespace std;

int** ARRAY = nullptr;
int N;

int main() {

	int option, stop = 30, populationSize = 0, mutationMethod = 1, crossoverMethod = 1;
	double mutationVar = 0.1, crossoverVar = 0.8;

	while (true) {
		option = Menu::showMainMenu();
		switch (option) {
		case 1:
			if (ARRAY != nullptr) {
				ARRAY = ArrayClass::destroyArray(ARRAY, N);
			}
			ARRAY = ArrayClass::createArrayFromFile(ARRAY, N);
			cout << "\t";
			system("pause");
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
		case 7:
			crossoverMethod = Menu::showCrossoverMethodMenu(crossoverMethod);
			break;
		case 8:
			//TODO: Wykonaj algorytm
			break;
		case 9:
			if (ARRAY != nullptr) {
				ARRAY = ArrayClass::destroyArray(ARRAY, N);
			}
			cout << "\t";
			system("pause");
			return 0;
		default:
			break;
		}
	}
}
