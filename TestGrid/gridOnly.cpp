//this is a test to ensure things compile if just grid.h is included
//it also checks that a bool grid actually uses uint8_t rather than the
//bool omptimisation
#include"../include/scieng/grid.h"

template<typename Container, typename expectedType>
concept unoptimisedBoolContainer = requires(Container c, expectedType * ptr)
{
	ptr = &(*c.begin());
};

template<typename Container, typename expectedType>
constexpr bool isOptimisedBoolContainer() requires (unoptimisedBoolContainer<Container, expectedType>)
{
	return false;
}

template<typename Container, typename expectedType>
constexpr bool isOptimisedBoolContainer() requires (!unoptimisedBoolContainer<Container, expectedType>)
{
	return true;
}

void testJustGrid()
{
	//check I can create a grid when I only include grid.h
	sci::GridData<double, 2> grid;

	//check the bool type works, as it has code to avoid std::vector bool optimisation
	sci::GridData<bool, 2> gridBool({ 16,16 });

	static_assert(!isOptimisedBoolContainer<sci::GridData<bool, 2>, uint8_t>(), "sci::GridData<bool, 2> failed the test that it is not optimised");
	static_assert(!isOptimisedBoolContainer<sci::GridData<int, 2>, int>(), "my optimised bool test is wrong and incorrectly thought sci::GridData<uint_8, 2> was optimised");
	static_assert(isOptimisedBoolContainer<std::vector<bool>, bool>(), "my optimised bool test is wrong and incorrectly thought sci::vector<bool> was unoptimised");
	
}