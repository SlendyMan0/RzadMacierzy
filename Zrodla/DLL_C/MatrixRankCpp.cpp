// MatrixRankCpp.cpp : Defines the exported functions for the DLL application.
// Compile by using: cl /EHsc /DMATRIXRANK_EXPORTS /LD MatrixRankCpp.cpp

#include "stdafx.h"
#include "MatrixRankCpp.h"
#include <cmath>

int adder(int * matrix, int deleteTemplate) //deleteTeplate is used as binary template indicating which columns and verses should be excluded, for example 8 (1000) indicates that column one should be excluded along with verse one, 12 (1010) - columns and verses 1 and 3 should be excluded.
{
	int size = sqrt(matrix[0]);
	int counter = 0;
	int sum = 0;

	for (int i = 0; i < size; i++) {
		counter += (deleteTemplate >> i) & 1; //finding how many verses and columns are excluded
	}

	if ((size - counter) == 1) { //checking for matrix 1x1
		for (int i = 0; i < size; i++) {
			if (((deleteTemplate >> i) & 1) == 0) { //searching for 0 in binary representation of deleteTemplate
				return matrix[(size*(size - 1)) + i +1];
			}
		}
	}
	else {
		for (int i = 0; i < size; i++) {
			if (((deleteTemplate >> i) & 1) == 0) {
				int pow1 = 1;
				int pow2 = 1;

				for (int j = 0; j < i; j++) {
					pow1 *= -1;
					pow2 *= 2;
				}
				sum += pow1 * matrix[(size*counter) + i + 1] * adder(matrix, deleteTemplate + pow2);
			}
		}
	}

	return sum;
}