#include "ArrayClass.h"

/*
	Funkcja tworzy oraz wypeĹ‚nia tabelÄ™ danymi z pliku
*/
int** ArrayClass::createArrayFromFile(int** array, int& N) {
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
			array = new int* [N];
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
/*
	Funkcja niszczy tabelÄ™ dynamicznÄ…
*/
int** ArrayClass::destroyArray(int** array, int N) {
	for (int i = 0; i < N; i++) {
		delete[] array[i];
	}
	delete[] array;
	return nullptr;
}
/*
	Funkcja wyĹ›wietla macierz sÄ…siedztwa
*/
void ArrayClass::showArray(int** array, int N) {
	std::cout << "\nN = " << N << "\n\n";
	for (int i = 0; i < N; i++) std::cout << "-------";
	std::cout << "\n";
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			std::cout << "|  " << std::left << std::setw(4) << array[i][j];
		}
		std::cout << "|\n";
		for (int j = 0; j < N; j++) std::cout << "-------";
		std::cout << "\n";
	}
}