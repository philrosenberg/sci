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
	};

	template<class T>
	struct VectorTraits <std::vector<T>>
	{
		typedef typename VectorTraits<T>::baseType baseType;
		static const size_t nDimensions = VectorTraits<T>::nDimensions + 1;
	};

	template<class T, class OTHER_BASE_TYPE>
	struct OtherBase
	{
		typedef OTHER_BASE_TYPE type;
		static const bool sameBaseType = std::is_same<typename VectorTraits<T>::baseType, typename VectorTraits<OTHER_BASE_TYPE>::baseType>::value;
	};

	template<class T, class OTHER_BASE_TYPE>
	struct OtherBase<std::vector<T>, OTHER_BASE_TYPE>
	{
		typedef typename std::vector<OtherBase<T, OTHER_BASE_TYPE>> type;
		static const bool sameBaseType = std::is_same<typename VectorTraits<T>::baseType, typename VectorTraits<OTHER_BASE_TYPE>::baseType>::value;
	};

	template<class T, size_t N>
	struct OtherDimensionVersion
	{
		typedef std::vector<OtherDimensionVersion<T, N - 1>> type;
	};
	template<class T>
	struct OtherDimensionVersion<T, 0>
	{
		typedef typename VectorTraits<T>::baseType type;
	};


	template<class T>
	struct TypeTraits
	{
		typedef T unitlessType;
		static constexpr unitlessType unitless(size_t v) { return unitlessType(v); }
		static constexpr unitlessType unitless(double v) { return unitlessType(v); }
		static constexpr auto sqrt(const T &v) ->decltype(std::sqrt(v)) { return std::sqrt(v); }
		static const T unity(1);
		static const T zero(0);
	};

	template<class T, class U>
	struct Promoted
	{
		typedef decltype(T() + U()) type;
	};
}
