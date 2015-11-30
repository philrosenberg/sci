#ifndef SCI_ALGORITHM_H
#define SCI_ALGORITHM_H
#include<algorithm>
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
		std::transform( first1, last1, destFirst, unaryOp );
	}
#endif
}
#endif