//
// Created by karol on 15.12.2021.
//

#ifndef PEA_ZAD2_ARRAYCLASS_H
#define PEA_ZAD2_ARRAYCLASS_H

#include <fstream>
#include <iostream>

using namespace std;

class ArrayClass {
public:

	static int **destroyArray(int **array, int N);

	static int **createArrayFromFile(int **array, int &N);


};

#endif //PEA_ZAD2_ARRAYCLASS_H
