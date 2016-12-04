// MatrixRankCpp.cpp : Defines the exported functions for the DLL application.
// Compile by using: cl /EHsc /DMATRIXRANK_EXPORTS /LD MatrixRankCpp.cpp

#include "stdafx.h"
#include "MatrixRankCpp.h"
#include <cmath>

int size;

/*
For matrix 3x3

deleteTemplateColumn - c2 c1 c0
deleteTemplateRow - r2 r1 r0

	c0 c1 c2
r0  a  b  c
r1  d  e  f
r2  g  h  i

i.e.: delTempCol = 6(110), delTempRow = 5(101), only avalible space is d

also

(delTempCol >> 0) & 1 = 0	(delTempRow >> 0) & 1 = 1
(delTempCol >> 1) & 1 = 1	(delTempRow >> 0) & 1 = 0
(delTempCol >> 2) & 1 = 1	(delTempRow >> 0) & 1 = 1
*/

int determinant(int * matrix, int deleteTemplateColumn, int deleteTemplateRow) //deleteTeplate is used as binary template indicating which columns and verses should be excluded, for example 8 (1000) indicates that column one should be excluded along with verse one, 12 (1010) - columns 1 and 3 should be excluded along with verses 1 and 2.
{
	int counterColumn = 0;
	int firstEmptyRow;
	int det = 0;

	for (int i = 0; i < size; i++) {
		counterColumn += (deleteTemplateColumn >> i) & 1; //finding how many columns are excluded
	}

	if ((size - counterColumn) == 1) { //checking for matrix 1x1
		int x, y;
		for (int i = 0; i < size; i++) {
			if (((deleteTemplateRow >> i) & 1) == 0) { //searching for 0 in binary representation of deleteTemplate
				x = i;
			}
			if (((deleteTemplateColumn >> i) & 1) == 0) { //searching for 0 in binary representation of deleteTemplate
				y = i;
			}
		}
		return matrix[(size*x) + y + 1];
	}
	else {
		int k = 0;
		int t = 0;

		for (int i = 0; i < size; i++) {
			if (!((deleteTemplateRow >> i) & 1)) {
				firstEmptyRow = i;
			}
		}

		for (int i = 0; i < size; i++) {
			t++;
			if (((deleteTemplateColumn >> i) & 1) == 0) {
				k++;
				det += pow(-1, (1 + k + firstEmptyRow)) * matrix[(size*firstEmptyRow) + i + 1] * determinant(matrix, deleteTemplateColumn + pow(2, i), deleteTemplateRow + pow(2, firstEmptyRow));
			}
		}
	}

	return det;
}

int rank(int * matrix, int deleteTemplateColumn, int deleteTemplateRow) {
	size = matrix[0];
	int counterColumn = 0;
	int counterRow = 0;

	for (int i = 0; i < size; i++) {
		counterColumn += (deleteTemplateColumn >> i) & 1; //finding how many columns are excluded
		counterRow += (deleteTemplateRow >> i) & 1; //finding how many rows are excluded
	}

	if (counterColumn != counterRow) {
		return -1;
	}
	else if (counterColumn == 0) {
		int det = determinant(matrix, deleteTemplateColumn, deleteTemplateRow);

		if (det == 0) {
			return -2;
		}
		else {
			return size;
		}
	}
	else {
		int det = determinant(matrix, deleteTemplateColumn, deleteTemplateRow);

		if (det == 0) {
			int rankNumber = 0;
			for (int i = 0; i < size; i++) {
				if (((deleteTemplateColumn >> i) & 1) == 0) {
					for (int j = 0; j < size; j++) {
						if (((deleteTemplateRow >> j) & 1) == 0) {
							int temp = rank(matrix, deleteTemplateColumn + pow(2, i), deleteTemplateRow + pow(2, j));
							if (temp > rankNumber) {
								rankNumber = temp;
								break;
							}
						}
					}
				}
			}
			return rankNumber;
		}
		else {
			return size - counterColumn;
		}
	}

	//return determinant(matrix, deleteTemplateColumn, deleteTemplateRow);
}