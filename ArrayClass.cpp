//
// Created by karol on 15.12.2021.
//

#include "ArrayClass.h"

// Funkcja tworzy oraz wypełnia tabelę danymi z pliku
int **ArrayClass::createArrayFromFile(int **array, int &N) {
	std::string fileName;
	std::string ignoredLines;
	std::cout << "\tPodaj nazwe pliku (z rozszerzeniem): ";
	std::cin >> fileName;
	std::ifstream file(fileName);
	int var;
	if (file.is_open()) {

		for (int i = 0; i < 3; ++i) {
			file.ignore(INT_MAX, '\n');
		}
		file >> ignoredLines;
		file >> N;
		for (int i = 0; i < 4; ++i) {
			file.ignore(INT_MAX, '\n');
		}
		if (file.fail()) {
			std::cout << "\tBlad pliku - ODCZYT DLUGOSCI\n";
		} else {
			array = new int *[N];
			for (int i = 0; i < N; i++) {
				array[i] = new int[N];
				for (int j = 0; j < N; j++) {
					file >> var;
					array[i][j] = var;
					if (file.fail()) {
						std::cout << "\tBlad pliku - ODCZYT TABLICY\n";
						break;
					}
				}
			}
		}
	}
	return array;
}

// Funkcja niszczy tabelę dynamiczną
int **ArrayClass::destroyArray(int **array, int N) {
	for (int i = 0; i < N; i++) {
		delete[] array[i];
	}
	delete[] array;
	return nullptr;
}
