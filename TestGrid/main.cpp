#include"../include/svector/gridview.h"
#include"../include/svector/gridtransformview.h"
#include"../include/svector/grid.h"
#include<iostream>

#include<vector>
#include<deque>

void testJustGrid();


//These are concepts to help debugging should there be a problem with the requires statements
template <class _It>
concept _part__Indirectly_readable_impl = requires(const _It __i) {
	typename std::iter_value_t<_It>;
	typename std::iter_reference_t<_It>;
	typename std::iter_rvalue_reference_t<_It>;
	{ *__i } -> std::same_as<std::iter_reference_t<_It>>;
	{ _RANGES iter_move(__i) } -> std::same_as<std::iter_rvalue_reference_t<_It>>;
}&& std::common_reference_with<std::iter_reference_t<_It>&&, std::iter_value_t<_It>&>
&& std::common_reference_with<std::iter_reference_t<_It>&&, std::iter_rvalue_reference_t<_It>&&>
&& std::common_reference_with<std::iter_rvalue_reference_t<_It>&&, const std::iter_value_t<_It>&>;

template <class _It, class _Ty>
concept output_iterator = std::input_or_output_iterator<_It> && std::indirectly_writable<_It, _Ty>
&& requires(_It __i, _Ty && __t) {
	*__i++ = static_cast<_Ty&&>(__t);
};

template <class _It, class _Ty>
concept part_indirectly_writable = requires(_It && __i, _Ty && __t) {
	*__i = static_cast<_Ty&&>(__t);
	*static_cast<_It&&>(__i) = static_cast<_Ty&&>(__t);
	const_cast<const std::iter_reference_t<_It>&&>(*__i) = static_cast<_Ty&&>(__t);
	//const_cast<const std::iter_reference_t<_It>&&>(*static_cast<_It&&>(__i)) = static_cast<_Ty&&>(__t);
};


using gd = sci::GridData<double, 2>;
static_assert((bool)std::ranges::random_access_range<gd>, "sci::grid_view failed the test for being a random access range");
static_assert((bool)std::ranges::range<gd>, "sci::grid_view failed the test for being a range");
static_assert((bool)std::random_access_iterator<std::ranges::iterator_t<gd>>, "sci::grid_view failed the test for having a random access iterator");
static_assert((bool)std::bidirectional_iterator<std::ranges::iterator_t<gd>>, "sci::grid_view failed the test for having a bidirectional iterator");
static_assert(std::input_iterator<std::ranges::iterator_t<gd>>, "sci::grid_view failed the test for having a input iterator");
static_assert(std::forward_iterator<std::ranges::iterator_t<gd>>, "sci::grid_view failed the test for having a forward iterator");
static_assert(std::output_iterator<std::ranges::iterator_t<gd>, double>, "sci::grid_view failed the test for having a output iterator");
static_assert(std::indirectly_readable<std::ranges::iterator_t<gd>>, "sci::grid_view failed the test for having a indirectly readable iterator");


static_assert((bool)std::ranges::random_access_range<sci::grid_view<std::deque<double>, 1>>, "sci::grid_view failed the test for being a random access range");
static_assert((bool)std::ranges::range<sci::grid_view<std::deque<double>, 1>>, "sci::grid_view failed the test for being a range");
static_assert((bool)std::random_access_iterator<std::ranges::iterator_t<sci::grid_view<std::deque<double>, 1>>>, "sci::grid_view failed the test for having a random access iterator");
static_assert((bool)std::bidirectional_iterator<std::ranges::iterator_t<sci::grid_view<std::deque<double>, 1>>>, "sci::grid_view failed the test for having a bidirectional iterator");
static_assert(std::input_iterator<std::ranges::iterator_t<sci::grid_view<std::deque<double>, 1>>>, "sci::grid_view failed the test for having a input iterator");
static_assert(std::forward_iterator<std::ranges::iterator_t<sci::grid_view<std::deque<double>, 1>>>, "sci::grid_view failed the test for having a forward iterator");
static_assert(std::output_iterator<std::ranges::iterator_t<sci::grid_view<std::deque<double>, 1>>, double>, "sci::grid_view failed the test for having a output iterator");
static_assert(std::indirectly_readable<std::ranges::iterator_t<sci::grid_view<std::deque<double>, 1>>>, "sci::grid_view failed the test for having a indirectly readable iterator");
static_assert((bool)!std::ranges::contiguous_range< sci::grid_view<std::deque<double>, 1>>, "grid_view<std::deque<>> should not be contiguous");

//static_assert((bool)std::ranges::contiguous_range<sci::grid_view<std::vector<double>, 1>>, "sci::grid_view<std::vector<>> failed the test for being a contiguous range");

using gv = sci::grid_view<std::deque<double>, 1>;
using gpv = sci::gridpairtransform_view<sci::plus<double, double>, gv, gv>;
static_assert((bool)std::ranges::random_access_range<gpv>, "sci::gridpair_view failed the test for being a random access range");
static_assert((bool)std::ranges::range<gpv>, "sci::gridpair_view failed the test for being a range");
static_assert((bool)std::random_access_iterator<std::ranges::iterator_t<gpv>>, "sci::gridpair_view failed the test for having a random access iterator");
static_assert((bool)std::bidirectional_iterator<std::ranges::iterator_t<gpv>>, "sci::gridpair_view failed the test for having a bidirectional iterator");
static_assert(std::input_iterator<std::ranges::iterator_t<gpv>>, "sci::grid_view gridpair_view the test for having a input iterator");
static_assert(std::forward_iterator<std::ranges::iterator_t<gpv>>, "sci::gridpair_view failed the test for having a forward iterator");
//static_assert(std::output_iterator<std::ranges::iterator_t<sci::gridpair_view<std::deque<double>, 1, std::deque<double>, 1>>, std::pair<double, double>>, "sci::gridpair_view failed the test for having a output iterator");
//static_assert(std::indirectly_writable<std::ranges::iterator_t<sci::gridpair_view<std::deque<double>, 1, std::deque<double>, 1>>, std::pair<double, double>>, "sci::gridpair_view failed the test for indirectly writable");
//static_assert(part_indirectly_writable<std::ranges::iterator_t<sci::gridpair_view<std::deque<double>, 1, std::deque<double>, 1>>, std::pair<double,double>>, "sci::gridpair_view failed the test for part indirectly writable");
static_assert(std::input_or_output_iterator<std::ranges::iterator_t<gpv>>, "sci::gridpair_view failed the test for having a input or output iterator");
static_assert(std::indirectly_readable<std::ranges::iterator_t<gpv>>, "sci::gridpair_view failed the test for having a indirectly readable iterator");
//static_assert(part_indirectly_readable<std::ranges::iterator_t<sci::gridpair_view<std::deque<double>, 1, std::deque<double>, 1>>>, "sci::gridpair_view failed the test for having a part indirectly readable iterator");
//static_assert((bool)!std::ranges::contiguous_range< sci::gridpair_view<std::deque<double>, 1, std::deque<double>, 1>>, "gridpair_view<std::deque<>> should not be contiguous");

//static_assert((bool)std::ranges::contiguous_range<sci::gridpair_view<std::vector<double>, 1, std::deque<double>, 1>>, "sci::gridpair_view<std::vector<>> failed the test for being a contiguous range");


static_assert(sci::IsGrid<sci::GridData<double, 0>>, "zero dimensional grid failed the IsGrid test");



template<sci::IsGridDims<2> GRID>
void output2d(GRID grid)
{
	std::array<size_t, 2> shape = grid.shape();

	for (size_t i = 0; i < shape[0]; ++i)
	{
		for (size_t j = 0; j < shape[1]; ++j)
			//std::cout << grid[i][j] << ",";
			std::cout << grid[{ i, j }] << ",";
		std::cout << "\n";
	}
	std::cout << "\n\n";
}

template<class T>
void output1d(T grid) requires (T::ndims == 1)
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
	//test the gridOnly cpp file
	testJustGrid();

	//testing GridData
	{
		//std::vector<double> dest;
		//std::vector<double> source(20);
		//std::span<double> viewOfSource(source.begin(), source.end());
		//dest.assign(viewOfSource.begin(), viewOfSource.end());

		sci::GridData < double, 2> d2;
		sci::GridData<double, 1> d1{ 1., 2., 3., 4., 5. };
		sci::GridData<double, 1> d1_2{ 11., 12., 13., 14., 15. };
		auto d1_3 = d1 + d1 + 2;
		output1d(d1_3);
		d2.push_back(d1_3);
	}
	//testing grid_view
	{
		std::array<size_t, 2>shapeArray{ 3, 2 };
		std::array<size_t, 1>stridesArray{ 2 };
		sci::GridPremultipliedStridesReference<2> strides2d(&stridesArray[0]);

		//sci::GridPremultipliedStridesReference<1> strides1d;

		//start with some data
		std::deque<double> que{ 1., 2., 3., 4., 5., 6. };
		auto subrange = std::ranges::subrange(que.begin() + 2, que.begin() + 5);
		double scalar = 100.0;

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

		//test slicing
		auto gridRange1d3 = gridRange[0];
		output1d(gridRange1d1);

		//create a zero d grid
		auto gridRange0d = scalar | sci::views::grid<0>;
		std::cout << gridRange0d[{}] << "\n\n";
		sci::getGridView(scalar);
		//auto gridRange0d2 = sci::grid_fn<0>(sci::GridPremultipliedStridesReference<0>())(scalar);
	}

	//testing GridData
	{
		std::array<size_t, 2> shape2d{4, 3};
		sci::GridPremultipliedStridesReference<2> strides2d(&shape2d[0]);
		std::vector<double> data;
		using view_type = std::remove_cv_t<decltype(data | sci::views::grid<2>(sci::GridPremultipliedStridesReference<2>()))>;
		view_type m_view = data | sci::views::grid<2>(strides2d);
		m_view.retarget( data | sci::views::grid<2>(strides2d));
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

		//std::cout << "Testing grid_view(GridData) constructor\n";
		//sci::grid_view<std::vector<double>, 2>view(grid);
		//output2d(view);
	}

	//testing gridtuple_view
	{
		//transforming 1d arrays
		sci::GridData<double, 1> grid1(3, 1);
		sci::gridpairtransform_view<sci::plus<double, double>, sci::GridData<double, 1>, sci::GridData<double, 1>> pair1d(grid1, grid1);
		std::cout << "testing gridtuple_view operator[](size_t) accessor for a 1D case\n";
		output1d(pair1d);

		//transforming 2d arrays
		std::array<size_t, 2> shape2d{ 4, 3 };
		sci::GridData<double, 2>grid2(shape2d, 1.0);
		sci::GridData<double, 2>grid3(shape2d, 3.0);
		sci::gridpairtransform_view<sci::plus<double, double>, sci::GridData<double, 2>, sci::GridData<double, 2>> pair2d(grid2, grid3);
		std::cout << "Testing gridtuple_view operator[](std::array) accessor for a 2D case\n";
		auto val2 = pair2d[{0, 0}];
		std::cout << val2 << "\n\n";
		std::cout << "testing gridtuple_view operator[](size_t) slice accessor for a 2D case\n";
		auto val = pair2d[0];
		for (size_t j = 0; j < shape2d[1]; ++j)
			std::cout << val[j] << ",";
		std::cout << "\n\n";

		//transforming 1d and 2d mixed
		//sci::gridpairtransform_view<sci::plus<double, double>, sci::GridData<double, 2>, sci::GridData<double, 1>> pairmixed(grid2, grid1); //this shouldn't compile 


		//std::cout << "testing transforming a gridtuple_view, then turning it back to a grid\n";
		//for(size_t i=0; i<shape2d[0]; ++i)
		//	for(size_t j = 0; j<shape2d[1])
		//		std::cout <<
		//auto grid3 = grid1 + grid2;
		//using pair_type = sci::gridpair_view<sci::GridData<double, 2>, sci::GridData<double, 2>>;
		//auto pair2 = sci::make_gridpair_view(grid1, grid2);
		//auto operated = pair | std::ranges::views::transform([](pair_type::const_reference_type p) { return (p.first + p.second); });
		//return operated | views::grid<std::max(T::ndims, U::ndims)>(a.getStrides());
		//output2d(operated | sci::views::grid<2>(grid1.getStrides()));

		const sci::GridData<double, 2>& gridref = grid2;
		auto gridbad = sci::getGridView(gridref);
		output2d(gridbad);
		std::cout << "testing operator +\n";
		auto grid4 = grid2 + grid3;
		output2d(grid3);

		auto grid5 = grid2 + 5.0 - grid3;
		output2d(grid5);

		auto grid6 = grid2 + 2.0;
		std::cout << "ndims = " << grid6.ndims << "\n\n";
		sci::GridData<double, 2>grid8(grid6);
		sci::GridData<double, 2> grid7 = grid6;
		typename decltype(grid6)::value_type value=3.0;
		static_assert(sci::IsGridDims<decltype(grid6),2>, "transformed grid is not a grid");

		std::cout << "testing operator ==\n";
		output2d(grid2 == 3.0);

		std::cout << "testing operator +=";
		output2d(grid2 += 1.0);
	}



	return 0;
}