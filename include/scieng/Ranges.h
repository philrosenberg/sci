#ifndef SCI_RANGES_H
#define SCI_RANGES_H

#include<ranges>
#include<vector>

namespace sci
{
	namespace ranges
	{

		template <class T>
		concept vector_range = std::ranges::contiguous_range<T> &&
			std::ranges::sized_range<T> &&
			requires(T & t)
			{
				{ *t.begin() } ->std::convertible_to<double>;
			};

		template <class T>
		concept vector_range_f = std::ranges::contiguous_range<T> &&
			std::ranges::sized_range<T> &&
			requires(T & t)
			{
				{ *t.begin() } ->std::convertible_to<float>;
			};

		template <class T>
		concept vector_range_resizable = sci::ranges::vector_range<T> &&
			requires(T& t)
			{
				{t.resize(0)};
			};

		template <class T>
		concept vector_range_resizable_f = sci::ranges::vector_range_f<T> &&
			requires(T & t)
			{
				{t.resize(0)};
			};
	}
}

#endif