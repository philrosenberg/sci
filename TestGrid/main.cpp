#include"../include/svector/array.h"
#include<iostream>

template<class BASERANGE>
void output2d(sci::grid_view<2, BASERANGE> grid)
{
	std::array<size_t, 2> shape = grid.shape();

	for (size_t i = 0; i < shape[0]; ++i)
	{
		for (size_t j = 0; j < shape[1]; ++j)
			std::cout << grid[i][j] << ",";
		std::cout << "\n";
	}
	std::cout << "\n\n";
}

template<class BASERANGE>
void output1d(sci::grid_view<1, BASERANGE> grid)
{
	std::array<size_t, 1> shape = grid.shape();

	for (size_t i = 0; i < shape[0]; ++i)
	{
			std::cout << grid[i] << ",";
	}
	std::cout << "\n\n";
}


int main()
{
	std::array<size_t, 2>shapeArray{ 3, 2 };
	std::array<size_t, 1>stridesArray{2};
	sci::GridPremultipliedStridesReference<2> strides2d(&stridesArray[0]);

	//sci::GridPremultipliedStridesReference<1> strides1d;

	//start with some data
	std::deque<double> que{ 1., 2., 3., 4., 5., 6. };
	auto subrange = std::ranges::subrange(que.begin() + 2, que.begin()+5);

	//create a 1D grid from que and output it
	auto gridRange1d1 = que | sci::views::grid<1>;
	for (size_t i = 0; i < gridRange1d1.size(); ++i)
		std::cout << gridRange1d1[i] << ",";
	std::cout << "\n\n";

	//create a 1D grid from subrange and output it
	auto gridRange1d2 = subrange | sci::views::grid<1>;
	for (size_t i = 0; i < gridRange1d2.size(); ++i)
		std::cout << gridRange1d2[i] << ",";
	std::cout << "\n\n";

	//create a 2D grid from que and output it
	auto gridRange = que | sci::views::grid<2>(strides2d);
	//auto gridRange = que | sci::grid_fn<2>(shape1);
	//for (size_t i = 0; i < gridRange.size(); ++i)
	//	std::cout << gridRange[i] << ",";
	for (size_t i = 0; i < shapeArray[0]; ++i)
	{
		for (size_t j = 0; j < shapeArray[1]; ++j)
			std::cout << gridRange[i][j] << ",";
		std::cout << "\n";
	}
	std::cout << "\n\n";

	//pass the grid into a function expecting a 2d grid
	output2d(gridRange);
	output1d(gridRange1d1);

	return 0;
}