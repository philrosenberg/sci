#ifndef SCI_ALGORITHM_H
#define SCI_ALGORITHM_H
#include<algorithm>
#include<iterator>
#include"math.h"
#include<ranges>
#include"multitransformview.h"
#include"statistics.h"
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

	template<class Value, std::forward_iterator FilterIt, std::forward_iterator OutputIt, Unary_Predicate<decltype(*FilterIt())> UnaryPred = decltype(sci::default_pred<decltype(*FilterIt())>)>
	void assign_if(const Value &v, FilterIt filter_first, OutputIt first, OutputIt last, UnaryPred pred = sci::default_pred<decltype(*filter_first)>)
	{
		for (; first != last; ++first, ++filter_first)
			if (pred(*filter_first))
				*first = v;
	}

	template<class Value, std::ranges::forward_range U, std::ranges::forward_range V, Unary_Predicate<typename U::value_type> UnaryPred = decltype(sci::default_pred<typename U::value_type>)>
	void assign_if(const Value& value, const U& filter, V& dest, UnaryPred pred = sci::default_pred<typename U::value_type>)
	{
		return assign_if(value, filter.begin(), dest.begin(), dest.end(), pred);
	}

	//differs from std::copy_if by having the if based on a potentially other container
	//also has defined behaviour if input and dest overlap providing dest <= input
	template<std::forward_iterator InputIt, std::forward_iterator FilterIt, std::forward_iterator OutputIt, Unary_Predicate<decltype(*FilterIt())> UnaryPred = decltype(sci::default_pred<decltype(*FilterIt())>)>
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

	template<class TUPLE>
	struct SubrangeBuilderTraits
	{
		using firstType = std::ranges::subrange<std::tuple_element_t<0, TUPLE>>;
		using remainder = SubrangeBuilderTraits<sci::TailTuple_t<1, TUPLE>>;
		static const size_t size = std::tuple_size_v<TUPLE>;
	};

	//a class that will build a tuple of subranges from a tuple of
	//begin iterators and a single length
	template<class TUPLE>
	struct SubrangeBuilder
	{
		//Constructor
		SubrangeBuilder(size_t length)
			:m_length(length)
		{
		}

		//public function to create the tuple
		using result_tuple_type = typename sci::TransformedTuple<SubrangeBuilderTraits<TUPLE>>::type;
		constexpr result_tuple_type values(const TUPLE& tuple)
		{
			if constexpr (std::tuple_size_v< TUPLE> > 0)
			{
				result_tuple_type result;
				valuesInternal<0>(tuple, result); //start the recursive calls
				return result;
			}
			else
				return std::tuple<>();
		}
	private:
		//private function called by the above
		template<size_t N>
		constexpr void valuesInternal(const TUPLE& tuple, result_tuple_type& result)
		{
			//assign the element
			auto first = std::get<N>(tuple);
			using iter_type = std::tuple_element_t<N, TUPLE>;
			std::get<N>(result) = std::ranges::subrange<iter_type>(first, first + m_length);

			//if this is not the final element continue recursion
			if constexpr (N < std::tuple_size_v<TUPLE> -1)
				valuesInternal<N + 1>(tuple, result);
		}
		const size_t m_length;
	};

	//As per sci::copy_if, but the predicate can take any number of arguments. Provide the filters as a parameter pack
	template<auto Pred, std::forward_iterator InputIt, std::forward_iterator OutputIt, std::forward_iterator... FilterIts>
	OutputIt copy_if_multi(InputIt first, InputIt last, OutputIt dest_first, FilterIts... iterators)
	{
		auto length = std::distance(first, last);
		auto itersTuple = std::make_tuple(iterators...);
		auto subranges = SubrangeBuilder<decltype(itersTuple)>(length).values(itersTuple);
		auto combinedFilterView = sci::make_multitransform_view<Pred>(subranges);

		using iterator = decltype(combinedFilterView.begin());
		static_assert(std::input_or_output_iterator<iterator>);
		static_assert(std::input_iterator<iterator>);
		static_assert(std::forward_iterator<iterator>);

		return sci::copy_if(first, last, combinedFilterView.begin(), dest_first);
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

	template<std::ranges::random_access_range T, std::ranges::forward_range U>
	void reorder(T& v, const U& newLocations)
	{
		if(newLocations.size() != v.size())
			throw(std::out_of_range("Called sci::reorder, but the size of the newLocations range does not match the size of the data range"));
		if (newLocations.size() == 0)
			return;
		if (sci::max(newLocations) >= newLocations.size())
			throw(std::out_of_range("Called sci::reorder, but the maximum index is too large too fit in the result"));
		T result = v;
		size_t i = 0;
		for (auto locIter = newLocations.begin(); locIter != newLocations.end(); ++locIter, ++i)
		{
			if constexpr (std::ranges::range<decltype(v[i])>)
				std::copy(v[i].begin(), v[i].end(), result[*locIter].begin());
			else
				result[*locIter] = v[i];
		}
		std::swap(result, v);
	}

	template<size_t N, class TUPLEOFARRAYPOINTERS, std::ranges::forward_range T>
	void reorder(TUPLEOFARRAYPOINTERS vs, const T& newlocations)
	{
		if constexpr (N > std::tuple_size_v< TUPLEOFARRAYPOINTERS>-1)
			return;
		else
		{
			reorder(*std::get<N>(vs), newlocations);
			reorder<N + 1>(vs, newlocations);
		}
	}

	template<std::ranges::random_access_range SORTBY, class TUPLEOFARRAYPOINTERS>
	void sortBy(SORTBY& toSortBy, TUPLEOFARRAYPOINTERS arrays)
	{
		//create a vector of indices which will represent the original locations of each element after it has been sorted
		std::vector<size_t> originalLocations(toSortBy.size());
		for (size_t i = 0; i < originalLocations.size(); ++i)
			originalLocations[i] = i;
		//sort the locations based on the data in v
		//the custom comparator function compares based on the element at the original index stored in the element being compared
		//by the end of the sort originalLocations[i] is the index of the ith lowest element of toSortBy
		std::sort(originalLocations.begin(), originalLocations.end(), [toSortBy](size_t originalIndex1, size_t originalIndex2) {return toSortBy[originalIndex1] < toSortBy[originalIndex2]; });

		//invert originalLocatons to set up our newLocations for each element of toSortBy
		std::vector<size_t> newLocations(originalLocations.size());
		for (size_t i = 0; i < originalLocations.size(); ++i)
		{
			newLocations[originalLocations[i]] = i;
		}

		//reorder the arrays
		reorder(toSortBy, newLocations);
		reorder<0>(arrays, newLocations);
	}

	//puta all values less than or equal to the last element at the beginning
	//of the container and all those bigger to the right of the container
	//values passed in must be iterator types
	//result is an iterator to the first element with a value bigger than the
	//pivot value.
	template <std::forward_iterator T>
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
	}

	template <std::bidirectional_iterator T, class U>
	size_t segregate_if(T containerBegin, T containerEnd, U predicate)
	{
		if (containerEnd - containerBegin == 0)
			return 0;
		size_t count = 0;
		auto writePoint = containerEnd;
		--writePoint;
		auto readPoint = containerBegin;
		while (true)
		{
			while (predicate(*writePoint) && readPoint != writePoint)
			{
				--writePoint;
				++count;
			}
			while (!predicate(*readPoint) && readPoint != writePoint)
			{
				++readPoint;
			}
			if (readPoint == writePoint)
				break;
			std::swap(*readPoint, *writePoint);
		}
		if (predicate(*readPoint))
			++count;
		return count;
	}

	template <std::ranges::bidirectional_range T, class U>
	size_t segregate_if(T& container, U predecate)
	{
		return segregate_if(std::ranges::begin(container), std::ranges::end(container), predecate);
	}

	//find the kth Biggest element. k=0 is the smallest element
	template<std::ranges::random_access_range RANGE>
	auto findKthBiggestValueInPlace(RANGE& v, size_t k)
	{
		if (v.size() <= k)
			throw(std::out_of_range("Attempt to find the kth biggest element of a vector, but k was larger than the max index"));

		auto partitionValue = *(v.end() - 1);
		auto begin = v.begin();
		auto end = v.end();
		std::swap(*(begin + k), *(end - 1)); //this makes the algorithm O(1)instead of O(N^2) if it is presorted
		auto partitionPoint = partitionOnLastElement(begin, end);
		size_t duplicates = segregate_if(begin, partitionPoint, [partitionPoint](const auto &val) {return val == *partitionPoint; });
		while (begin + k > partitionPoint  || begin + k < partitionPoint - duplicates)
		{
			if (begin + k < partitionPoint)
			{
				//move all the duplicates to the end of the first partition
				if (duplicates > 0)
					groupValuesToEnd(begin, partitionPoint, *partitionPoint);
				end = partitionPoint - duplicates;
				std::swap(*(begin + k), *(end - 1));
				partitionPoint = partitionOnLastElement(begin, end);
			}
			else
			{
				k -= partitionPoint - begin;
				begin = partitionPoint;
				std::swap(*(begin + k), *(end - 1));
				partitionPoint = partitionOnLastElement(begin, end);
			}
			//check how many values we have equal to k
			duplicates = segregate_if(begin, partitionPoint, [partitionPoint](const auto &val) {return val == *partitionPoint; });
		}
		return *partitionPoint;
	}

	template<std::ranges::forward_range RANGE>
	auto findKthBiggestValue(const RANGE& v, size_t k)
	{
		std::vector<typename RANGE::value_type> copy(v.begin(), v.end());
		return findKthBiggestValueInPlace(copy, k);
	}
}
#endif
