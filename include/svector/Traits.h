#pragma once
#include<vector>
#include<cmath>
namespace sci
{
	template<class T>
	struct VectorTraits
	{
		typedef T baseType;
		static const size_t nDimensions = 0;
		template<size_t N>
		struct OtherDimensionVersion
		{
			typedef std::vector<VectorTraits<T>::OtherDimensionVersion<N - 1>> type;
		};
		template<>
		struct OtherDimensionVersion<0>
		{
			typedef T type;
		};
		template<class OTHER_BASE_TYPE>
		struct Other
		{
			typedef OTHER_BASE_TYPE type;
			static const bool sameBaseType = std::is_same<VectorTraits<T>::baseType, OTHER_BASE_TYPE>::value;
		};
	};
	template<class T>
	struct VectorTraits <std::vector<T>>
	{
		typedef typename VectorTraits<T>::baseType baseType;
		static const size_t nDimensions = VectorTraits<T>::nDimensions + 1;
		template<class OTHER_BASE_TYPE>
		struct Other
		{
			typedef std::vector<typename VectorTraits<T>::Other<OTHER_BASE_TYPE>::type> type;
			static const bool sameBaseType = std::is_same<VectorTraits<T>::baseType, OTHER_BASE_TYPE>::value;
		};
	};

	template<class T>
	struct TypeTraits
	{
		typedef T unitlessType;
		static unitlessType unitless(size_t v) { return unitlessType(v); }
		static unitlessType unitless(double v) { return unitlessType(v); }
		static auto sqrt(const T &v) ->decltype(std::sqrt(v)) { return std::sqrt(v); }
	};

	template<class T, class U>
	struct Promoted
	{
		typename typedef decltype(T() + U()) type;
	};
}