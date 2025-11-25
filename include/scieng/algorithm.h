#ifndef SCI_ALGORITHM_H
#define SCI_ALGORITHM_H
#include<algorithm>
#include<iterator>
#include"math.h"
namespace sci
{
	//This sci vesion of std::transform avoids the MSVC security warnings which won't switch off
#ifdef _MSC_VER
	template<class InputIt, class OutputIt, class UnaryOperation>
	OutputIt transform( InputIt first1, InputIt last1, OutputIt destFirst, UnaryOperation unaryOp )
	{
		while (first1 != last1)
		{
			*destFirst++ = unaryOp(*first1++);
		}
		return destFirst;
	}
#else
	template<class InputIt, class OutputIt, class UnaryOperation>
	OutputIt transform( InputIt first1, InputIt last1, OutputIt destFirst, UnaryOperation unaryOp )
	{
		return std::transform( first1, last1, destFirst, unaryOp );
	}
#endif

	template< class T>
	bool default_pred(const T& t)
	{
		return bool(t);
	}

	template<class T, class U>
	concept Unary_Predicate = requires(T & t, U & u)
	{
		{ t(u) }-> std::convertible_to<bool>;
	};

	//differs from std::copy_if by having the if based on a potentially other container
	//also has defined behaviour if input and dest overlap providing dest <= input
	template<class InputIt, class FilterIt, class OutputIt, Unary_Predicate<decltype(*FilterIt())> UnaryPred = decltype(sci::default_pred<decltype(*FilterIt())>)>
	OutputIt copy_if(InputIt first, InputIt last, FilterIt filter_first, OutputIt dest_first, UnaryPred pred = sci::default_pred<decltype(*filter_first)>)
	{
		for (; first != last; ++first, ++filter_first)
		{
			if (pred(*filter_first))
			{
				*dest_first = *first;
				++dest_first;
			}
		}
		return dest_first;
	}

	//differs from std::copy_if by having the if based on a potentially other container
	//also has defined behaviour if input and dest overlap providing input.begin() <= dest.begin()
	template<std::ranges::forward_range T, std::ranges::forward_range U, std::ranges::forward_range V, Unary_Predicate<typename U::value_type> UnaryPred = decltype(sci::default_pred<typename U::value_type>)>
	auto copy_if(const T &input, const U &filter, V &dest, UnaryPred pred = sci::default_pred<typename U::value_type>)
	{
		return copy_if(input.begin(), input.end(), filter.begin(), dest.begin(), pred);
	}

	//differs from std::remove_if by having the if based on a potentially other container
	template<class InputIt, class FilterIt, Unary_Predicate<decltype(*FilterIt())> UnaryPred = decltype(sci::default_pred<decltype(*FilterIt())>)>
	InputIt remove_if(InputIt first, InputIt last, FilterIt filter_first, UnaryPred pred = sci::default_pred<decltype(*filter_first)>)
	{
		InputIt dest = first;
		for (; first != last; ++first, ++filter_first)
		{
			if (!pred(*filter_first))
			{
				*dest = std::move(*first);
				++dest;
			}
		}
		return dest;
	}

	//differs from std::remove_if by having the if based on a potentially other container
	template<std::ranges::forward_range T, std::ranges::forward_range U, Unary_Predicate<typename U::value_type> UnaryPred = decltype(sci::default_pred<typename U::value_type>)>
	auto remove_if(const T& input, const U& filter, UnaryPred pred = sci::default_pred<typename U::value_type>)
	{
		return remove_if(input.begin(), input.end(), filter.begin(), pred);
	}

	template<class InputIt, class FilterIt, Unary_Predicate<decltype(*FilterIt())> UnaryPred = decltype(sci::default_pred<decltype(*FilterIt())>)>
	InputIt keep_if(InputIt first, InputIt last, FilterIt filter_first, UnaryPred pred = sci::default_pred<decltype(*filter_first)>)
	{
		InputIt dest = first;
		for (; first != last; ++first, ++filter_first)
		{
			if (pred(*filter_first))
			{
				*dest = std::move(*first);
				++dest;
			}
		}
		return dest;
	}

	template<std::ranges::forward_range T, std::ranges::forward_range U, Unary_Predicate<typename U::value_type> UnaryPred = decltype(sci::default_pred<typename U::value_type>)>
	auto keep_if(const T& input, const U& filter, UnaryPred pred = sci::default_pred<typename U::value_type>)
	{
		return keep_if(input.begin(), input.end(), filter.begin(), pred);
	}

	template<class InputIt, Unary_Predicate<decltype(*InputIt())> UnaryPred = decltype(sci::default_pred<decltype(*InputIt())>)>
	InputIt keep_if(InputIt first, InputIt last, UnaryPred pred = sci::default_pred<decltype(*first)>)
	{
		InputIt dest = first;
		for (; first != last; ++first)
		{
			if (pred(*first))
			{
				*dest = std::move(*first);
				++dest;
			}
		}
		return dest;
	}

	template<std::ranges::forward_range T, std::ranges::forward_range V, class UnaryPred = decltype(sci::default_pred<typename T::value_type>)>
	auto keep_if(const T& input, UnaryPred pred = sci::default_pred<typename T::value_type>)
	{
		return keep_if(input.begin(), input.end(), pred);
	}

	template<class InputIt, class IndexIt, class OutputIt>
	void copy_locations(InputIt input, IndexIt locations_first, IndexIt locations_last, OutputIt dest)
	{
		for (; locations_first != locations_last; ++locations_first, ++dest)
			*dest = *(input + (*locations_first));
	}

	template<std::ranges::random_access_range T, std::ranges::forward_range U, std::ranges::forward_range V>
	void copy_locations(const T &input, const U &locations, V &dest)
	{
		copy_locations(input.begin(), locations.begin(), locations.end(), dest.begin());
	}

	template<class It, class PositionOperator>
	void fill(It begin, It end, PositionOperator op)
	{
		for (It current = begin; current != end; ++current)
			*current = op(begin, current);
	}

	template<std::ranges::forward_range T, class PositionOperator>
	void fill(T &t, PositionOperator op)
	{
		sci::fill(t.begin(), t.end(), op);
	}

	class distance_filler
	{
	public:
		template<class It>
		auto operator()(It begin, It current)
		{
			return std::distance(begin, current);
		}
	};

	template<class T, class U=T>
	class arithmetic_filler
	{
	public:
		arithmetic_filler(T start, U spacing)
			:m_start(start), m_spacing(spacing)
		{}
		template<class It>
		constexpr auto operator()(It begin, It current) const
		{
			return m_start + std::distance(begin, current) * m_spacing;
		}
	private:
		T m_start;
		U m_spacing;
	};

	template<class T, class U = T>
	class geometric_filler
	{
	public:
		geometric_filler(T start, U multiplier)
			:m_start(start), m_multiplier(multiplier)
		{}
		template<class It>
		auto operator()(It begin, It current) const
		{
			return m_start * sci::pow(m_multiplier, std::distance(begin, current));
		}
	private:
		T m_start;
		U m_multiplier;
	};

	//interpolates a sequence of ys from a sorted sequence of xs, sorted sequence of ys and sorted sequence of requested xs
	template<std::forward_iterator T, std::forward_iterator U, std::input_iterator V, std::output_iterator<decltype(*U()+(*V()-*T())*(*U()-*U())/(*T()-*T()))> W>
	void interpolate(T xsBegin, T xsEnd, U ys, V givenXsBegin, V givenXsEnd, W requestedYs)
	{
		T lowerX = xsBegin;
		T upperX = xsBegin;
		++upperX;
		T nextUpperX = upperX;
		++nextUpperX;
		U lowerY = ys;
		U upperY = ys;
		++upperY;
		while (givenXsBegin != givenXsEnd)
		{

			while (*givenXsBegin > *upperX && nextUpperX != xsEnd)
			{
				++lowerX;
				++upperX;
				++nextUpperX;
				++lowerY;
				++upperY;
			}
			*requestedYs = *lowerY + (*givenXsBegin - *lowerX) * (*upperY - *lowerY) / (*upperX - *lowerX);
			++givenXsBegin;
			++requestedYs;
		}

	}

	//as interpolate, but instead of interpolating betwen two points either side of the given x, return the lower of them
	template<std::forward_iterator T, std::forward_iterator U, std::input_iterator V, std::output_iterator<decltype(*U())> W, class X>
	void lower(T xsBegin, T xsEnd, U ys, V givenXsBegin, V givenXsEnd, W requestedYs, X fill)
	{
		while (givenXsBegin != givenXsEnd && *givenXsBegin < *xsBegin)
		{
			*requestedYs = fill;
			++givenXsBegin;
			++requestedYs;
		}

		T lowerX = xsBegin;
		T upperX = xsBegin;
		++upperX;
		U lowerY = ys;

		while (givenXsBegin != givenXsEnd)
		{
			while (upperX != xsEnd && *givenXsBegin >= *upperX)
			{
				++lowerX;
				++upperX;
				++lowerY;
			}
			*requestedYs = *lowerY;
			++givenXsBegin;
			++requestedYs;
		}

	}
}
#endif
