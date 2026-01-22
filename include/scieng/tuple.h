#ifndef SCIENG_TUPLE
#define SCIENG_TUPLE
#include<tuple>

namespace sci
{
	template<size_t N, class TUPLE>
	struct TailTuple
	{
	private:
		using thisType = std::tuple_element_t<N, TUPLE>;
		using thisSingleTupleType = std::tuple<thisType>;
		using remainderType = typename TailTuple<N + 1, TUPLE>::type;
	public:
		using type = decltype(std::tuple_cat(std::declval<thisSingleTupleType>(), std::declval<remainderType>()));
	};

	template<size_t N, class TUPLE> requires(std::tuple_size<TUPLE>() == N + 1)
		struct TailTuple<N, TUPLE>
	{
	private:
		using thisType = std::tuple_element_t<N, TUPLE>;
		using thisSingleTupleType = std::tuple<thisType>;
	public:
		using type = thisSingleTupleType;
	};

	template<size_t N, class TUPLE> requires(std::tuple_size<TUPLE>() == N)
		struct TailTuple<N, TUPLE>
	{
		using type = std::tuple<>;
	};

	template<size_t N, class TUPLE>
	using TailTuple_t = typename TailTuple<N, TUPLE>::type;


	//A trait class to get a Tuple class which is a transformed version
	//of an existing tuple.
	//
	//TUPLETRANSFORMER must be a struct/class
	//TUPLETRANSFORMER::firstType must be the type you wish to transform the first element of the tuple into;
	//TUPLETRANSFORMER::remainder must be a TUPLETRANSFORMER applied to a tuple with the first element omitted.
	//It must be callable recursively to get the nth transformed type
	//TUPLETRANSFORMER::size must be the number of elements in the tuple to transform
	//Recursive calls will be made up to and including when TUPLETRANSFORMER::size == 1.
	//if TUPLETRANSFORMER::size == 0 and empty tuple will be created
	// 
	//This example would get pointers to types
	//template<class TUPLETOTRANSFORM>
	//struct PointerTransform : public TupleTransform<TUPLETOTRANSFORM, PointerTransform>
	//{
	//	using 
	//	using firstType = std::tuple_element_t<0, TUPLETOTRANSFORM>*;
	//}
	//using MyTuple = std::tuple<double, int, float>
	//using pointersTuple = TransformedTuple_t<PointerTransform<MyTuple>>; //should be std::tuple<double*, int*, float*>
	template<class TUPLETRANSFORMER>
	struct TransformedTuple
	{
	private:
		using transformedType = typename TUPLETRANSFORMER::firstType;
		using transformedSingleTupleType = std::tuple<transformedType>;
		using transformTraitRemainderType = typename TUPLETRANSFORMER::remainder;
		using transformedRemainderType = typename TransformedTuple<transformTraitRemainderType>::type;
	public:
		using type = decltype(std::tuple_cat(std::declval<transformedSingleTupleType>(), std::declval<transformedRemainderType>()));
	};

	template<class TUPLETRANSFORMER> requires(TUPLETRANSFORMER::size == 1)
		struct TransformedTuple<TUPLETRANSFORMER>
	{
	private:
		using transformedType = typename TUPLETRANSFORMER::firstType;
		using transformedSingleTupleType = std::tuple<transformedType>;
	public:
		using type = transformedSingleTupleType;
	};

	template<class TUPLETRANSFORMER> requires(TUPLETRANSFORMER::size == 0)
		struct TransformedTuple<TUPLETRANSFORMER>
	{
		using type = std::tuple<>;
	};

	template<class TUPLETRANSFORMER>
	using TransformedTuple_t = typename TransformedTuple<TUPLETRANSFORMER>::type;

	template<size_t N, class TUPLE>
	constexpr auto tail(const TUPLE& tuple)
	{
		if constexpr (N == std::tuple_size_v<TUPLE>)
			return std::make_tuple();
		else if constexpr (N == std::tuple_size_v<TUPLE> - 1)
			return std::make_tuple(std::get<N>(tuple));
		else return std::tuple_cat(std::make_tuple(std::get<N>(tuple)), tail<N + 1>(tuple));
	}

	template<class TUPLE, class TRANSFORM>
	constexpr auto transform(const TUPLE& tuple, TRANSFORM op)
	{
		if constexpr (std::tuple_size_v<TUPLE> == 0)
			return std::tuple<>();
		else if constexpr (std::tuple_size_v<TUPLE> == 1)
			return std::make_tuple(op(std::get<0>(tuple)));
		else
		{
			return std::tuple_cat(std::make_tuple(op(std::get<0>(tuple))),
				transform(tail(tuple), op));
		}
	}
}

#endif