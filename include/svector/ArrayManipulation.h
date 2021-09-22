#ifndef SCI_ARRAYMANIPULATION
#define SCI_ARRAYMANIPULATION

#include"array.h"
#include<limits>

namespace sci
{
	/*template<size_t NDIMS>
	bool allTrue(GridView<bool const, NDIMS> gridView)
	{
		bool result = true;
		for (auto iter = gridView.begin(); iter != gridView.end(); ++iter)
			result &= *iter;
		return result;
	}
	template<class T>
	requires(bool(isGrid<T>()))
		bool allTrue(T gridView)
	{
		return allTrue(GridView<bool const, T::nDimensions()>(gridView));
	}
	template<size_t NDIMS>
	bool anyTrue(GridView<bool const, NDIMS> gridView)
	{
		for (auto iter = gridView.begin(); iter != gridView.end(); ++iter)
			result &= *iter;
		return false;
	}
	template<class T>
	requires(bool(isGrid<T>()))
		bool anyTrue(T gridView)
	{
		return anyTrue(GridView<bool const, T::nDimensions()>(gridView));
	}*/
	bool anyTrue(std::span<bool const> s)
	{
		for (bool val : s)
			if (val)
				return true;
		return false;
	}
	bool allTrue(std::span<bool> s)
	{
		for (bool val : s)
			if (!val)
				return false;
		return true;
	}
	int sum(std::span<int const>s)
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
	}

	template<class T, size_t NDIMS>
	GridData<T, NDIMS> abs(GridView<T, NDIMS> gridView)
	{
		GridData<T, NDIMS> result(gridView.getShape());
		auto resultIter = result.begin();
		auto inputIter = gridView.begin();
		for (; inputIter != gridView.end(); ++inputIter, ++resultIter)
			*resultIter = sci::abs(inputIter);
		return result;
	}

	template<int POW, class T, size_t NDIMS>
	auto pow(GridView<T, NDIMS> gridView)
	{
		using valueType = decltype(sci::pow<POW>(T()));
		GridData<valueType, NDIMS> result(gridView.getShape());
		auto resultIter = result.begin();
		auto inputIter = gridView.begin();
		for (; inputIter != gridView.end(); ++inputIter, ++resultIter)
			*resultIter = sci::pow<POW>(*inputIter);
		return result;
	}

	template<int POW, class T, size_t NDIMS>
	auto pow(const GridData<T, NDIMS> &gridView)
	{
		using valueType = decltype(sci::pow<POW>(T()));
		GridData<valueType, NDIMS> result(gridView.getShape());
		auto resultIter = result.begin();
		auto inputIter = gridView.begin();
		for (; inputIter != gridView.end(); ++inputIter, ++resultIter)
			*resultIter = sci::pow<POW>(*inputIter);
		return result;
	}

	template<int ROOT, class T, size_t NDIMS>
	auto root(GridView<T, NDIMS> gridView)
	{
		using valueType = decltype(sci::root<ROOT>(T()));
		GridData<valueType, NDIMS> result(gridView.getShape());
		auto resultIter = result.begin();
		auto inputIter = gridView.begin();
		for (; inputIter != gridView.end(); ++inputIter, ++resultIter)
			*resultIter = sci::root<ROOT>(*inputIter);
		return result;
	}

	template<int ROOT, class T, size_t NDIMS>
	auto root(const GridData<T, NDIMS>& gridView)
	{
		using valueType = decltype(sci::root<ROOT>(T()));
		GridData<valueType, NDIMS> result(gridView.getShape());
		auto resultIter = result.begin();
		auto inputIter = gridView.begin();
		for (; inputIter != gridView.end(); ++inputIter, ++resultIter)
			*resultIter = sci::root<ROOT>(*inputIter);
		return result;
	}

	template<class T, size_t NDIMS>
	T min(GridView<T, NDIMS> gridView)
	{
		if (gridView.size() == 0)
			return std::numeric_limits<T>::quiet_NaN();
		T result = *gridView.begin();
		for (auto iter = gridView.begin() + 1; iter != gridView.end(); ++iter)
			result = std::min(result, *iter);
		return result;
	}

	template<class T, size_t NDIMS>
	T max(GridView<T, NDIMS> gridView)
	{
		if (gridView.size() == 0)
			return std::numeric_limits<T>::quiet_NaN();
		T result = *gridView.begin();
		for (auto iter = gridView.begin() + 1; iter != gridView.end(); ++iter)
			result = std::max(result, *iter);
		return result;
	}
}

#endif