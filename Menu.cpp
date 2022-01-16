//
// Created by karol on 15.12.2021.
//

#include "Menu.h"

int Menu::showMainMenu() {
	system("cls");
	int option;

	std::cout << "PEA - projekt nr 3\n";
	std::cout << "Autor: Karol Pastewski 252798\n\n";

	std::cout << "Wpisz liczbe odpowiadajaca opcji z listy\n\n";

	std::cout << "1.Wczytanie danych z pliku i wyswietlenie ich\n";
	std::cout << "2.Wprowadzenie kryterium stopu\n";
	std::cout << "3.Ustawienie wielkosci populacji poczatkowej\n";
	std::cout << "4.Ustawienie wspolczynnika mutacji\n";
	std::cout << "5.Ustawienie wspolczynnika krzyzowania\n";
	std::cout << "6.Wybor metody mutacji\n";
	std::cout << "7.Wybor metody krzyzowania\n";
	std::cout << "8.Uruchomienie algorytmu\n";
	std::cout << "9.Wyjscie z programu\n";

	std::cin >> option;
	return option;
}

int Menu::showStopMenu(int var) {
	std::cout << "\tStop [s] (obecnie " << var << " [s]): ";
	std::cin >> var;
	std::cout << "\t";
	system("pause");
	return var;
}

int Menu::showPopulationMenu(int var) {
	std::cout << "Wielkosc populacji (obecnie " << var << "): ";
	std::cin >> var;
	std::cout << "\t";
	//system("pause");
	return var;
}

double Menu::showMutationVarMenu(double var) {
	std::cout << "Wspolczynnik mutacji (obecnie " << var << "): ";
	std::cin >> var;
	std::cout << "\t";
	system("pause");
	return var;
}

double Menu::showCrossoverVarMenu(double var) {
	std::cout << "Wspolczynnik krzyzowania (obecnie " << var << "): ";
	std::cin >> var;
	std::cout << "\t";
	system("pause");
	return var;
}

int Menu::showMutationMethodMenu(int var) {
	std::cout << "Metoda mutacji (obecnie " << var << "):\n"
		 << "\t1.\n"
		 << "\t2.\n\t";
	std::cin >> var;
	std::cout << "\t";
	system("pause");
	return var;
}

int Menu::showCrossoverMethodMenu(int var) {
	std::cout << "Metoda krzyzowania (obecnie " << var << "):\n"
		<< "\t1.\n"
		<< "\t2.\n\t";
	std::cin >> var;
	std::cout << "\t";
	system("pause");
	return var;
}