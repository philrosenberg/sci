#ifndef SCI_ARRAYMANIPULATION
#define SCI_ARRAYMANIPULATION

#include"gridtupleview.h"
#include<limits>
#include"serr.h"
#include<algorithm>
#include"math.h"

namespace sci
{
	template<std::ranges::range RANGE>
	bool allTrue(RANGE&& range)
	{
		for (auto iter = range.begin(); iter != range.end(); ++iter)
			if (!(*iter))
				return false;
		return true;
	}
	template<std::ranges::range RANGE>
	bool anyTrue(RANGE&& range)
	{
		for (auto iter = range.begin(); iter != range.end(); ++iter)
			if (*iter)
				return true;
		return false;
	}

	template< IsGrid GRID, class VIEW>
	auto operator | (GRID& grid, VIEW& view)
	{
		return grid_view< decltype(grid | view), grid.ndims>(grid | view, grid.getStrides);
	}


	/*int sum(std::span<int const>s)
	{
		int result =0;
		for (int val : s)
			result += val;
		return result;
	}
	void increment(std::span<int>s)
	{
		for (int &val : s)
			++val;
	}*/

	template<IsGrid GRID>
	auto abs(const GRID& grid)
	{
		using valueType = decltype(sci::abs(GRID::value_type()));
		GridData<valueType, GRID::ndims> result(grid.shape());
		auto resultIter = result.begin();
		auto inputIter = grid.begin();
		for (; inputIter != grid.end(); ++inputIter, ++resultIter)
			*resultIter = sci::abs(*inputIter);
		return result;
	}

	/*template<int POW, IsGrid GRID>
	auto pow(const GRID& grid)
	{
		using valueType = decltype(sci::pow<POW>(GRID::value_type()));
		GridData<valueType, GRID::ndims> result(grid.shape());
		auto resultIter = result.begin();
		auto inputIter = grid.begin();
		for (; inputIter != grid.end(); ++inputIter, ++resultIter)
			*resultIter = sci::pow<POW>(*inputIter);
		return result;
	}*/

	template<int ROOT, IsGrid GRID>
	auto root(const GRID& grid)
	{
		using valueType = decltype(sci::root<ROOT>(GRID::value_type()));
		GridData<valueType, GRID::ndims> result(grid.shape());
		auto resultIter = result.begin();
		auto inputIter = grid.begin();
		for (; inputIter != grid.end(); ++inputIter, ++resultIter)
			*resultIter = sci::root<ROOT>(*inputIter);
		return result;
	}

	/*template<IsGrid GRID>
	auto sqrt(const GRID& grid)
	{
		return root<2> (grid);
	}*/

	//puta all values less than or equal to the last element at the beginning
	//of the container and all those bigger to the right of the container
	//values passed in must be iterator types
	//result is an iterator to the first element with a value bigger than the
	//pivot value.
	//commented out as it is currently already defines in svector.h, but I might move it later
	/*template <class T>
	T partitionOnLastElement(T containerBegin, T containerEnd)
	{
		T result = containerBegin;
		auto pivotValue = *(containerEnd - 1);
		for (T iter = containerBegin; iter != containerEnd - 1; ++iter)
		{
			if (*iter <= pivotValue)
			{
				std::swap(*iter, *result);
				++result;
			}
		}
		std::swap(*result, *(containerEnd - 1));
		return result;
	}*/

	template<std::ranges::forward_range T>
	size_t count(const T& range)
	{
		size_t result = 0;
		for (const auto& r : range)
			if (r)
				++result;
		return result;
	}

	template< IsGridDims<1> T, IsGridDims<1> U>
	GridData<typename T::value_type, 1> filter(const T& source, const U& flag)
	{
		size_t count = sci::count(flag);
		GridData<typename T::value_type, 1> result(count);
		auto dest = result.begin();
		for (size_t i = 0; i < source.size(); ++i)
		{
			if (flag[i])
			{
				*dest = source[i];
				++dest;
			}
		}
		return result;
	}

	template< IsGridDims<1> T, class U>
	void replaceNans(T& grid, const U& value)
	{
		for (auto& g : grid)
			if (g != g)
				g = value;
	}


	template<IsGridDims<2> GRID>
	GridData<typename GRID::value_type, 2> transpose(const GRID& grid)
	{
		std::array<size_t, 2> oldShape = grid.shape();
		std::array<size_t, 2> newShape = { oldShape[1], oldShape[0] };
		GridData<typename GRID::value_type, 2> result(newShape);
		for (size_t i = 0; i < oldShape[0]; ++i)
			for (size_t j = 0; j < oldShape[1]; ++j)
				result[j][i] = grid[i][j];
		return result;
	}
}

#endif