#include"../include/svector/gridview.h"
#include"../include/svector/gridtupleview.h"
#include"../include/svector/array.h"
#include<iostream>

#include<vector>
#include<deque>




static_assert((bool)std::ranges::random_access_range<sci::grid_view<std::deque<double>, 1>>, "sci::grid_view failed the test for being a random access range");
static_assert((bool)std::ranges::range<sci::grid_view<std::deque<double>, 1>>, "sci::grid_view failed the test for being a range");
static_assert((bool)std::random_access_iterator<std::ranges::iterator_t<sci::grid_view<std::deque<double>, 1>>>, "sci::grid_view failed the test for having a random access iterator");
static_assert((bool)std::bidirectional_iterator<std::ranges::iterator_t<sci::grid_view<std::deque<double>, 1>>>, "sci::grid_view failed the test for having a bidirectional iterator");
static_assert(std::input_iterator<std::ranges::iterator_t<sci::grid_view<std::deque<double>, 1>>>, "sci::grid_view failed the test for having a input iterator");
static_assert(std::forward_iterator<std::ranges::iterator_t<sci::grid_view<std::deque<double>, 1>>>, "sci::grid_view failed the test for having a forward iterator");
static_assert(std::output_iterator<std::ranges::iterator_t<sci::grid_view<std::deque<double>, 1>>, int>, "sci::grid_view failed the test for having a output iterator");
static_assert(std::indirectly_readable<std::ranges::iterator_t<sci::grid_view<std::deque<double>, 1>>>, "sci::grid_view failed the test for having a indirectly readable iterator");
static_assert((bool)!std::ranges::contiguous_range< sci::grid_view<std::deque<double>, 1>>, "grid_view<std::deque<>> should not be contiguous");

static_assert((bool)std::ranges::contiguous_range<sci::grid_view<std::vector<double>, 1>>, "sci::grid_view<std::vector<>> failed the test for being a contiguous range");

static_assert((bool)std::ranges::random_access_range<sci::gridpair_view<std::deque<double>, 1, std::deque<double>, 1>>, "sci::gridpair_view failed the test for being a random access range");
static_assert((bool)std::ranges::range<sci::gridpair_view<std::deque<double>, 1, std::deque<double>, 1>>, "sci::gridpair_view failed the test for being a range");
static_assert((bool)std::random_access_iterator<std::ranges::iterator_t<sci::gridpair_view<std::deque<double>, 1, std::deque<double>, 1>>>, "sci::gridpair_view failed the test for having a random access iterator");
static_assert((bool)std::bidirectional_iterator<std::ranges::iterator_t<sci::gridpair_view<std::deque<double>, 1, std::deque<double>, 1>>>, "sci::gridpair_view failed the test for having a bidirectional iterator");
static_assert(std::input_iterator<std::ranges::iterator_t<sci::gridpair_view<std::deque<double>, 1, std::deque<double>, 1>>>, "sci::grid_view gridpair_view the test for having a input iterator");
static_assert(std::forward_iterator<std::ranges::iterator_t<sci::gridpair_view<std::deque<double>, 1, std::deque<double>, 1>>>, "sci::gridpair_view failed the test for having a forward iterator");
static_assert(std::output_iterator<std::ranges::iterator_t<sci::gridpair_view<std::deque<double>, 1, std::deque<double>, 1>>, int>, "sci::gridpair_view failed the test for having a output iterator");
static_assert(std::indirectly_readable<std::ranges::iterator_t<sci::gridpair_view<std::deque<double>, 1, std::deque<double>, 1>>>, "sci::gridpair_view failed the test for having a indirectly readable iterator");
static_assert((bool)!std::ranges::contiguous_range< sci::gridpair_view<std::deque<double>, 1, std::deque<double>, 1>>, "gridpair_view<std::deque<>> should not be contiguous");

static_assert((bool)std::ranges::contiguous_range<sci::gridpair_view<std::vector<double>, 1, std::deque<double>, 1>>, "sci::gridpair_view<std::vector<>> failed the test for being a contiguous range");






template<class BASERANGE>
void output2d(sci::grid_view<BASERANGE, 2> grid)
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
void output1d(sci::grid_view<BASERANGE, 1> grid)
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
	//testing grid_view
	{
		std::array<size_t, 2>shapeArray{ 3, 2 };
		std::array<size_t, 1>stridesArray{ 2 };
		sci::GridPremultipliedStridesReference<2> strides2d(&stridesArray[0]);

		//sci::GridPremultipliedStridesReference<1> strides1d;

		//start with some data
		std::deque<double> que{ 1., 2., 3., 4., 5., 6. };
		auto subrange = std::ranges::subrange(que.begin() + 2, que.begin() + 5);

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
	}

	//testing GridData
	{
		std::array<size_t, 2> shape2d{4, 3};
		sci::GridPremultipliedStridesReference<2> strides2d(&shape2d[0]);
		std::vector<double> data;
		using view_type = std::remove_cv_t<decltype(data | sci::views::grid<2>(sci::GridPremultipliedStridesReference<2>()))>;
		view_type m_view = data | sci::views::grid<2>(strides2d);
		m_view = data | sci::views::grid<2>(strides2d);
		sci::GridData<double, 2> grid(shape2d);
		for (auto &val : grid)
			val = 1.0;
		output2d(grid.getView());
		grid[{0, 0}] = 2.0;
		grid[{1, 1}] = 4.0;
		grid[{2, 0}] = 8.0;
		output2d(grid.getView());
		grid.reshape({ 5, 3 }, 7.0);
		output2d(grid.getView());
		grid.reshape({ 6, 4 }, 8.0);
		output2d(grid.getView());

		std::cout << "Testing grid_view(GridData) constructor\n";
		sci::grid_view<sci::GridData<double, 2>, 2>view(grid);
		output2d(view);
	}

	//testing gridtuple_view
	{
		std::array<size_t, 2> shape2d{ 4, 3 };
		sci::GridData<double, 2>grid1(shape2d, 1.0);
		sci::GridData<double, 2>grid2(shape2d, 1.0);
		sci::gridpair_view<sci::GridData<double, 2>, 2, sci::GridData<double, 2>, 2> pair(grid1, grid2);
		auto grid3 = grid1 + grid2;
	}



	return 0;
}