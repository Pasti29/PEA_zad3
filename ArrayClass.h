#pragma once

#include <fstream>
#include <iostream>
#include <iomanip>

class ArrayClass {
public:

	static int **destroyArray(int **array, int N);
	static void showArray(int** array, int N);
	static int **createArrayFromFile(int **array, int &N);


};
