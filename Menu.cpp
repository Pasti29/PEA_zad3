//
// Created by karol on 15.12.2021.
//

#include "Menu.h"

int Menu::showMainMenu() {
	system("cls");
	int option;
	cout << "PEA - projekt nr 2" << endl;
	cout << "Autor: Karol Pastewski 252798" << endl << endl;
	cout << "Wpisz liczbe odpowiadajaca opcji z listy" << endl << endl;
	cout << "1.Wczytanie danych z pliku" << endl;
	cout << "2.Wprowadzenie kryterium stopu" << endl;
	cout << "3.Wybor sasiedztwa (dla TabuSearch)" << endl;
	cout << "4.Uruchomienie TabuSearch" << endl;
	cout << "5.Ustawienie wspolczynnika zmiany temperatury (dla SW)" << endl;
	cout << "6.Uruchomienie algorytmu SW" << endl;
	cout << "9.Wyjscie z programu" << endl;
	cin >> option;
	return option;
}