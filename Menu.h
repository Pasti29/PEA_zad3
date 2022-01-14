//
// Created by karol on 15.12.2021.
//

#ifndef PEA_ZAD2_MENU_H
#define PEA_ZAD2_MENU_H

#include <iostream>

using namespace std;

class Menu {
public:
	static int showMainMenu();
	static int showStopMenu(int var);
	static int showPopulationMenu(int var);
	static double showMutationVarMenu(double var);
	static double showCrossoverVarMenu(double var);
	static int showMutationMethodMenu(int var);
	static int showCrossoverMethodMenu(int var);

};

#endif //PEA_ZAD2_MENU_H
