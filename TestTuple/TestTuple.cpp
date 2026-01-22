// TestTuple.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "../include/scieng/tuple.h"
#include<assert.h>


//A struct that helps create the tuple type for a tuple of const pointers
//that match a tuple of some other set of types.
//I chose to make the pointers const only so I could use constexpr to do
//compile time testing
template<class TUPLE>
struct ConstPointerTransform
{
	using firstType = const std::tuple_element_t<0, TUPLE>*;
	using remainder = ConstPointerTransform<sci::TailTuple_t<1, TUPLE>>;
	static const size_t size = std::tuple_size_v<TUPLE>;
};

//This class will actually build the tuple of pointers
//It could just be a templated function based on the 
//valuesInternal function below, but this way provides and
//interface that can't be used wrong as the Internal
//function needs to be called with N=0 to start and
//it avoids polluting the namespace
template<class TUPLE>
struct Address1
{
public:
	using pointer_tuple_type = sci::TransformedTuple<ConstPointerTransform<TUPLE>>::type;
	constexpr static pointer_tuple_type values(const TUPLE& tuple)
	{
		pointer_tuple_type result;
		valuesInternal<0>(tuple, result); //start the recursive calls
		return result;
	}
private:
	template<size_t N>
	constexpr static void valuesInternal(const TUPLE& tuple, pointer_tuple_type& result)
	{
		//assigne the element
		std::get<N>(result) = getTransformed(std::get<N>(tuple));

		//if this is the final element, return it as a unit size tuple and end recursion
		if constexpr (N < std::tuple_size_v<TUPLE> -1)
			valuesInternal<N+1>(tuple, result);
	}
	constexpr static auto getTransformed(auto& input)
	{
		return &input; //get the address
	}
};


//this version works when the transformed types do not have default constructors
//or if they are references and must be assigned at initialisation
//It is more complicated than above, but provided here as a useful example
template<class TUPLE>
struct Address2
{
public:
	using pointer_tuple_type = sci::TransformedTuple<ConstPointerTransform<TUPLE>>::type;
	constexpr static pointer_tuple_type values(const TUPLE& tuple)
	{
		return valuesInternal<0>(tuple);
	}
private:
	template<size_t N>
	constexpr static auto valuesInternal(const TUPLE& tuple)
	{
		//get the type we want to create for this element to ensure
		//our auto return type doesn't create a mismatch
		using element_type = std::tuple_element_t<N, pointer_tuple_type>;

		//if this is the final element, return it as a unit size tuple and end recursion
		if constexpr (N == std::tuple_size_v<TUPLE> -1)
			return std::tuple<element_type>(getTransformed(std::get<N>(tuple)));

		//if this is not the final element create a unit size tuple of this element
		//an concatenate it with the transformed version of the remainder
		else
		{
			return std::tuple_cat(std::tuple<element_type>(getTransformed(std::get<N>(tuple))),
				valuesInternal<N + 1>(tuple));
		}
	}
	constexpr static auto getTransformed(auto& input)
	{
		return &input; //get the address
	}
};

int main()
{
    using myTupleType = std::tuple<int, float, char>;
    using myTuplePointerType1 = sci::TransformedTuple<ConstPointerTransform<myTupleType>>::type;
    static_assert(std::is_same_v<std::tuple_element_t<0, myTuplePointerType1>, const std::tuple_element_t<0, myTupleType>*>);
    static_assert(std::is_same_v<std::tuple_element_t<1, myTuplePointerType1>, const std::tuple_element_t<1, myTupleType>*>);
    static_assert(std::is_same_v<std::tuple_element_t<2, myTuplePointerType1>, const std::tuple_element_t<2, myTupleType>*>);

    constexpr myTupleType tuple{ 3, 4.2f, 0x42 };
    static_assert(sci::tail<0>(tuple) == std::tuple<int, float, char>{ 3, 4.2f, 0x42 });
    static_assert(sci::tail<1>(tuple) == std::tuple<float, char>(4.2f, 0x42));
    static_assert(sci::tail<2>(tuple) == std::tuple<char>(0x42));

	auto addresses1 = Address1<myTupleType>::values(tuple);
	auto addresses2 = Address2<myTupleType>::values(tuple);

	assert(&std::get<0>(tuple) == std::get<0>(addresses1));
	assert(&std::get<1>(tuple) == std::get<1>(addresses1));
	assert(&std::get<2>(tuple) == std::get<2>(addresses1));

	assert(&std::get<0>(tuple) == std::get<0>(addresses2));
	assert(&std::get<1>(tuple) == std::get<1>(addresses2));
	assert(&std::get<2>(tuple) == std::get<2>(addresses2));
}
