// TestArray.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "../include/svector/array.h"

size_t getFirst(const sci::Array<size_t, 1> &array)
{
	return array[0];
}
double getFirst(const sci::Array<double, 2> &array)
{
	return array[0][0];
}
double getFirst(const sci::Array<double, 3> &array)
{
	return array[0][0][0];
}

int main()
{
	sci::Array<size_t, 1> array1d_1;
	sci::Array<double, 2> array2d_1;
	sci::Array<double, 3> array3d_1;

	array1d_1.setSize(3);
	array1d_1[0] = 5;
	array1d_1[1] = 3;
	array1d_1[2] = 7;

	sci::Array<size_t, 1> array1d_2(2);
	array1d_2[0] = 6;
	array1d_2[1] = 2;
	sci::Array<double, 2> array2d_2(array1d_2);
	sci::Array<double, 3> array3d_2(array1d_1);

	array2d_2[0][0] = 2.0;
	array3d_2[0][0][0] = 3.0;
	
	size_t val1 = array1d_1[0];
	double val2 = array2d_2[0][0];
	double val3 = array3d_2[0][0][0];

	val1 = getFirst(array1d_2);
	val2 = getFirst(array2d_2);
	val3 = getFirst(array3d_2);

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
