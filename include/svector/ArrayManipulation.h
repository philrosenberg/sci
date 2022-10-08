#ifndef SCI_ARRAYMANIPULATION
#define SCI_ARRAYMANIPULATION

#include"gridtupleview.h"
#include<limits>
#include"serr.h"
#include<algorithm>
//#include"svector.h"

namespace sci
{
	template<std::ranges::range RANGE>
	bool allTrue(RANGE && range)
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
	auto operator | (GRID& grid, VIEW &view)
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
	auto abs(const GRID &grid)
	{
		using valueType = decltype(sci::abs(GRID::value_type()));
		GridData<valueType, GRID::ndims> result(grid.shape());
		auto resultIter = result.begin();
		auto inputIter = grid.begin();
		for (; inputIter != grid.end(); ++inputIter, ++resultIter)
			*resultIter = sci::abs(*inputIter);
		return result;
	}

	template<int POW, IsGrid GRID>
	auto pow(const GRID& grid)
	{
		using valueType = decltype(sci::pow<POW>(GRID::value_type()));
		GridData<valueType, GRID::ndims> result(grid.shape());
		auto resultIter = result.begin();
		auto inputIter = grid.begin();
		for (; inputIter != grid.end(); ++inputIter, ++resultIter)
			*resultIter = sci::pow<POW>(*inputIter);
		return result;
	}

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

	template<IsGrid GRID>
	auto sqrt(const GRID& grid)
	{
		return root<2> (grid);
	}

	template<std::ranges::range RANGE>
	auto min(RANGE && range)
	{
		using result_type = std::remove_reference_t<decltype(*range.begin())>;
		if (range.size() == 0)
			return std::numeric_limits<result_type>::quiet_NaN();
		result_type result = *range.begin();
		for (auto iter = range.begin() + 1; iter != range.end(); ++iter)
			result = std::min(result, *iter);
		return result;
	}

	template<std::ranges::range RANGE>
	auto max(RANGE&& range)
	{
		using result_type = std::remove_reference_t<decltype(*range.begin())>;
		if (range.size() == 0)
			return std::numeric_limits<result_type>::quiet_NaN();
		result_type result = *range.begin();
		for (auto iter = range.begin() + 1; iter != range.end(); ++iter)
			result = std::max(result, *iter);
		return result;
	}

	template<IsGrid GRID>
	auto min(const GRID& grid)
	{
		if (grid.size() == 0)
			return std::numeric_limits<GRID::value_type>::quiet_NaN();
		typename GRID::value_type result = std::numeric_limits<GRID::value_type>::infinity();
		for (auto& g : grid)
			result =std::min(g, result);
		return result;
	}

	template<IsGrid GRID>
	auto max(const GRID& grid)
	{
		if (grid.size() == 0)
			return std::numeric_limits<GRID::value_type>::quiet_NaN();
		typename GRID::value_type result = -std::numeric_limits<GRID::value_type>::infinity();
		for (auto& g : grid)
			result = std::max(g, result);
		return result;
	}

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

	//find the kth Biggest element. k=0 is the smallest element
	template<class RANGE>
	auto findKthBiggestValueInPlace(RANGE &&v, size_t k)
	{
		sci::assertThrow(v.size() > k, sci::err(sci::SERR_VECTOR, 0, sU("Attempt to find the kth biggest element of a vector, but k was too large for the vector")));

		auto partitionValue = *(v.end() - 1);
		auto begin = v.begin();
		auto end = v.end();
		auto partitionPoint = partitionOnLastElement(begin, end);
		while (begin + k != partitionPoint)
		{
			if (begin + k < partitionPoint)
			{
				end = partitionPoint;
				partitionPoint = partitionOnLastElement(begin, end);
			}
			else
			{
				k -= partitionPoint - begin;
				begin = partitionPoint;
				partitionPoint = partitionOnLastElement(begin, end);
			}
		}
		return *partitionPoint;
	}

	//find the median of a vector
	template<class RANGE>
	auto medianInPlace(RANGE&& v)
	{
		using value_type = std::remove_cvref_t<decltype(*v.begin())>;
		if (v.size() % 2 == 1)
			return findKthBiggestValue(v, v.size() / 2);

		if (v.size() == 0)
			return std::numeric_limits<value_type>::quiet_NaN();

		//find the larger of the two values needed to caculate the median

		size_t k = v.size() / 2;
		value_type value1 = findKthBiggestValueInPlace(v, k);

		//now find the smaller - because we have already partitioned the array around the larger
		//value, this is just the max of the values before the partition point
		value_type value2 = v.front();
		for (auto iter = v.begin(); iter != v.end() && (*iter) <= value1 ; ++iter)
			if (*iter > value2)
				value2 = *iter;

		return value_type((value1 + value2) / sci::TypeTraits<decltype(value1)>::unitlessType(2.0));
	}
	template<class RANGE>
	auto findKthBiggestValue(RANGE&& v, size_t k)
	{
		std::vector copy(v.begin(), v.end());
		return findKthBiggestInPlace(copy);
	}
	template<class RANGE>
	auto median(RANGE&& v)
	{
		std::vector copy(v.begin(), v.end());
		return medianInPlace(copy);
	}

	template <IsGridDims<1> T, IsGridDims<1> U>
	auto integrate(T x, U y)
	{
		typedef decltype((x[0] - x[0])* y[0]) returnType;
		if (x.size() != y.size())
			return std::numeric_limits<returnType>::quiet_NaN();
		if (x.size() == 0)
			return returnType(0);
		returnType result(0);
		auto yi = y.begin() + 1;
		for (auto xi = x.begin() + 1; xi != x.end(); ++xi)
		{
			result += (*yi + *(yi - 1)) * (*xi - *(xi - 1));
			++yi;
		}
		result *= TypeTraits<returnType>::unitlessType(0.5);
		return result;
	}

	template <IsGridDims<1> T, IsGridDims<1> U>
	auto integrate(T x, U y, typename T::value_type minx, typename T::value_type maxx)
	{
		typedef decltype((x[0] - x[0])* y[0]) returnType;
		if (x.size() != y.size())
			return std::numeric_limits<returnType>::quiet_NaN();
		returnType result(0);
		auto yi = y.begin() + 1;
		bool swappedLimits = false;
		if (maxx < minx)
		{
			std::swap(minx, maxx);
			swappedLimits = true;
		}
		sci::assertThrow(minx >= x[0], sci::err(sci::SERR_VECTOR, 0, sU("sci::integrate called with a min bound outside the data range.")));
		sci::assertThrow(maxx <= x.back(), sci::err(sci::SERR_VECTOR, 0, sU("sci::integrate called with a max bound outside the data range.")));
		for (auto xi = x.begin() + 1; xi != x.end(); ++xi, ++yi)
		{
			typename T::value_type x0 = *(xi - 1);
			typename T::value_type x1 = *xi;
			typename U::value_type y0 = *(yi - 1);
			typename U::value_type y1 = *yi;
			if (x1 < minx)
				continue;
			if (x0 > maxx)
				continue;
			if (x0 < minx)
			{
				y0 = typename U::value_type((y1 - y0) / (x1 - x0) * (minx - x0) + y0);
				x0 = minx;
			}
			if (x1 > maxx)
			{
				y1 = typename U::value_type((y1 - y0) / (x1 - x0) * (maxx - x0) + y0);
				x1 = maxx;
			}

			result += returnType((y0 + y1) * (x1 - x0));
		}
		result *= TypeTraits<returnType>::unitless(0.5);
		if (swappedLimits)
			return -result;
		return result;
	}

	template<std::ranges::range T, std::ranges::range U, class V>
	void assign(T& destination, const U& filter, const V& value)
	{
		auto iterD = destination.begin();
		auto iterF = filter.begin();
		for (; iterD != destination.end(); ++iterD, ++iterF)
			if (*iterF != 0)
				*iterD = value;
	}

	template<IsGrid T>
	void reorder(T& v, const sci::GridData<size_t, 1>& newlocations)
	{
		if constexpr (T::ndims == 0)
			return;

		T result(v.shape());
		if constexpr (T::ndims == 1)
			for (size_t i = 0; i < result.size(); ++i)
				result[newlocations[i]] = v[i];
		else
			for (size_t i = 0; i < result.shape()[0]; ++i)
				result[newlocations[i]] = sci::GridData<T::value_type, T::ndims - 1>(v[i]);
		std::swap(result, v);;
	}

	template<size_t N, class TUPLEOFARRAYPOINTERS>
	void reorder(TUPLEOFARRAYPOINTERS vs, const sci::GridData<size_t, 1>& newlocations)
	{
		if constexpr (N > std::tuple_size_v< TUPLEOFARRAYPOINTERS>-1)
			return;
		else
		{
			reorder(*std::get<N>(vs), newlocations);
			reorder<N + 1>(vs, newlocations);
		}
	}

	template<IsGridDims<1> SORTBY, class TUPLEOFARRAYPOINTERS>
	void sortBy(SORTBY &toSortBy, TUPLEOFARRAYPOINTERS arrays)
	{
		//create a vector of indices which will represent the original locations o each element after it has been sorted
		sci::GridData<size_t, 1> originalLocations(toSortBy.size());
		for (size_t i = 0; i < originalLocations.size(); ++i)
			originalLocations[i] = i;
		//sort the locations based on the data in v
		//the custom comparator function compares based on the element at the original index stored in the element being compared
		//by the end of the sort originalLocations[i] is the index of the ith lowest element of toSortBy
		std::sort(originalLocations.begin(), originalLocations.end(), [toSortBy](size_t originalIndex1, size_t originalIndex2) {return toSortBy[originalIndex1] < toSortBy[originalIndex2]; });

		//invert originalLocatons to set up our newLocations for each element of toSortBy
		sci::GridData<size_t, 1>newLocations(originalLocations.size());
		for (size_t i = 0; i < originalLocations.size(); ++i)
		{
			newLocations[originalLocations[i]] = i;
		}

		//reorder the arrays
		reorder(toSortBy, newLocations);
		reorder<0>(arrays, newLocations);
	}
}

#endif