#pragma once

#include <iostream>

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
