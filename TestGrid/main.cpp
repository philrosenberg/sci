#include"../include/svector/array.h"
#include<iostream>

int main()
{
	std::views::transform;

	std::deque<double> que{ 1., 2., 3., 4., 5., 6. };
	auto gridRange = que | sci::views::grid;
	for (size_t i = 0; i < gridRange.size(); ++i)
		std::cout << gridRange[i] << ",";
	std::cout << "\n\n";
}