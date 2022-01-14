//
// Created by karol on 15.12.2021.
//

#include "Menu.h"

int Menu::showMainMenu() {
	system("cls");
	int option;

	cout << "PEA - projekt nr 3" << endl;
	cout << "Autor: Karol Pastewski 252798" << endl << endl;

	cout << "Wpisz liczbe odpowiadajaca opcji z listy" << endl << endl;

	cout << "1.Wczytanie danych z pliku i wyswietlenie ich" << endl;
	cout << "2.Wprowadzenie kryterium stopu" << endl;
	cout << "3.Ustawienie wielkosci populacji poczatkowej" << endl;
	cout << "4.Ustawienie wspolczynnika mutacji" << endl;
	cout << "5.Ustawienie wspolczynnika krzyzowania" << endl;
	cout << "6.Wybor metody mutacji" << endl;
	cout << "7.Wybor metody krzyzowania" << endl;
	cout << "8.Uruchomienie algorytmu" << endl;
	cout << "9.Wyjscie z programu" << endl;

	cin >> option;
	return option;
}

int Menu::showStopMenu(int var) {
	cout << "\tStop [s] (obecnie " << var << " [s]): ";
	cin >> var;
	cout << "\t";
	system("pause");
	return var;
}

int Menu::showPopulationMenu(int var) {
	cout << "Wielkosc populacji (obecnie " << var << "): ";
	cin >> var;
	cout << "\t";
	system("pause");
	return var;
}

double Menu::showMutationVarMenu(double var) {
	cout << "Wspolczynnik mutacji (obecnie " << var << "): ";
	cin >> var;
	cout << "\t";
	system("pause");
	return var;
}

double Menu::showCrossoverVarMenu(double var) {
	cout << "Wspolczynnik krzyzowania (obecnie " << var << "): ";
	cin >> var;
	cout << "\t";
	system("pause");
	return var;
}

int Menu::showMutationMethodMenu(int var) {
	cout << "Metoda mutacji (obecnie " << var << "):\n"
		 << "\t1.\n"
		 << "\t2.\n\t";
	cin >> var;
	cout << "\t";
	system("pause");
	return var;
}

int Menu::showCrossoverMethodMenu(int var) {
	cout << "Metoda krzyzowania (obecnie " << var << "):\n"
		<< "\t1.\n"
		<< "\t2.\n\t";
	cin >> var;
	cout << "\t";
	system("pause");
	return var;
}