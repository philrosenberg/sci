#pragma once

#include"Ranges.h"
#include"Traits.h"
#include"math.h"
#include<vector>
#include<algorithm>

namespace sci
{

	template<class T, class U>
	T& assign(T& destination, const U& source)
	{
		return destination = source;
	}

	template<std::ranges::forward_range T>
	auto sum(const T& v)
	{
		std::remove_cvref_t<decltype(*std::ranges::cbegin(v))> sum(0);
		for (auto vi : v)
			sum += vi;
		return sum;
	}

	template<std::ranges::forward_range T, std::ranges::forward_range U>
	auto sum(const T& v, const U& weights)
	{
		std::remove_cvref_t<decltype(*std::ranges::cbegin(v) * *std::ranges::cbegin(weights))> sum(0);
		std::remove_cvref_t<decltype(*std::ranges::cbegin(weights))> weight(0);
		auto vi = std::ranges::cbegin(v);
		auto weighti = std::ranges::cbegin(weights);
		for (; vi < std::ranges::cend(v); ++vi, ++weighti)
		{
			sum += *vi * *weighti;
			weight += *weighti;
		}
		return sum / weight;
	}

	template<std::ranges::forward_range T>
	auto product(const T& v)
	{
		std::remove_cvref_t<decltype(*std::ranges::cbegin(v))> product(1);
		for (auto vi : v)
			product *= vi;
		return product;
	}

	template<std::ranges::forward_range T>
	auto mean(const T& v)
	{
		return sum(v) / typename TypeTraits<std::ranges::range_value_t<T>>::unitlessType(v.size());
	}

	template<std::ranges::forward_range T, std::ranges::forward_range U>
	auto mean(const T& v, const U& weights)
	{
		decltype(*std::ranges::cbegin(v) * *std::ranges::cbegin(weights)) sum(0);
		decltype(*std::ranges::cbegin(weights)) weight(0);
		auto vi = std::ranges::cbegin(v);
		auto weighti = std::ranges::cbegin(weights);
		for (; vi < std::ranges::cend(v); ++vi, ++weighti)
		{
			sum += *vi * *weighti;
			weight += *weighti;
		}
		return sum / weight;
	}

	template<std::ranges::forward_range T, class U>
	auto centralMoment(int moment, const T& v, const U& mean)
	{
		using resultType = decltype (sci::pow(*std::ranges::cbegin(v) - mean, moment));
		using unitlessType = typename TypeTraits<resultType>::unitlessType;

		resultType result = TypeTraits<resultType>::zero;
		for(auto vi : v)
			result += sci::pow((vi - mean), moment);
		return result / unitlessType(v.size() - 1);
	}

	template<std::ranges::forward_range T >
	auto centralMoment(int moment, const T& v)
	{
		auto mean = sci::mean(v);
		return centralMoment(moment, v, mean);
	}

	template<std::ranges::forward_range T, class U>
	auto centralMomentNoBessel(int moment, const T& v, const U& mean)
	{
		using resultType = decltype (sci::pow(*std::ranges::cbegin(v) - mean, moment));
		using unitlessType = typename TypeTraits<resultType>::unitlessType;

		resultType result = TypeTraits<resultType>::zero;
		const T* vEnd = &v[0] + v.size;
		for (const T* vi = &v[0]; vi != v.end(); ++vi)
			result += sci::pow((*vi - mean), moment);
		return result / unitlessType(v.size());
	}

	template<std::ranges::forward_range T >
	auto centralMomentNoBessel(int moment, const T& v)
	{
		auto mean = sci::mean(v);
		return centralMomentNoBessel(moment, v, mean);
	}

	template<int MOMENT, std::ranges::forward_range T, class U>
	auto centralMoment(const T& v, const U& mean)
	{
		using baseType = decltype(*std::ranges::cbegin(v) - mean);
		using resultType = decltype (sci::pow<MOMENT, baseType>(*std::ranges::cbegin(v) - mean));
		using unitlessType = typename TypeTraits<resultType>::unitlessType;

		resultType result = TypeTraits<resultType>::zero;
		for (auto vi : v)
			result += sci::pow<MOMENT, baseType>((vi - mean));
		return result / unitlessType(v.size() - 1);
	}

	template<int MOMENT, std::ranges::forward_range T>
	auto centralMoment(const T& v)
	{
		auto mean = sci::mean(v);
		return centralMoment<MOMENT>(v, mean);
	}

	template<int MOMENT, std::ranges::forward_range T, class U>
	auto centralMomentNoBessel(const T& v, const U& mean)
	{
		using baseType = decltype(*std::ranges::cbegin(v) - mean);
		using resultType = decltype (sci::pow<MOMENT, baseType>(baseType()));
		using unitlessType = typename TypeTraits<resultType>::unitlessType;

		resultType result = TypeTraits<resultType>::zero;
		for (auto vi : v)
			result += sci::pow<MOMENT, baseType>((vi - mean));
		return result / unitlessType(v.size());
	}

	template<int MOMENT, std::ranges::forward_range T>
	auto centralMomentNoBessel(const T& v)
	{
		auto mean = sci::mean(v);
		return centralMomentNoBessel<MOMENT>(v, mean);
	}
	
	template<std::ranges::forward_range T, class U, class V>
	void meanAndVariance(const std::vector<T>& v, U& mean, V& variance)
	{
		mean = sci::mean(v);
		variance = centralMoment<2>(v, mean);
	}

	template<std::ranges::forward_range T, class U, class V>
	void meanAndVarianceNoBessel(const T& v, U& mean, V& variance)
	{
		mean = sci::mean(v);
		variance = centralMomentNoBessel<2>(v, mean);
	}

	template<std::ranges::forward_range T>
	auto variance(const T& v)
	{
		auto mean = sci::mean(v);
		return centralMoment<2>(v, mean);
	}

	template<std::ranges::forward_range T>
	auto varianceNoBessel(const T& v)
	{
		auto mean = sci::mean(v);
		return centralMomentNoBessel<2>(v, mean);
	}

	template<std::ranges::forward_range T, class U>
	auto variance(const T& v, const U& mean)
	{
		return centralMoment<2>(v, mean);
	}

	template<std::ranges::forward_range T, class U>
	auto varianceNoBessel(const T& v, const U& mean)
	{
		return centralMomentNoBessel<2>(v, mean);
	}

	template<std::ranges::forward_range T, std::ranges::forward_range U, class V, class W>
	auto covariance(const T& v1, const V& mean1, const U& v2, const W& mean2)
	{
		using resultType = decltype ((*std::ranges::cbegin(v1) - mean1)* (*std::ranges::cbegin(v2) - mean2));
		using unitlessType = typename TypeTraits<resultType>::unitlessType;

		resultType result = TypeTraits<resultType>::zero;
		auto it1 = std::ranges::cbegin(v1);
		auto it2 = std::ranges::cbegin(v2);
		for (; it1 != std::ranges::cend(v1); ++it1, ++it2)
			result += (*it1 - mean1) * (*it2 - mean2);

		return result / unitlessType(std::ranges::cend(v1) - std::ranges::cbegin(v1) - 1);
	}

	template<std::ranges::forward_range T, std::ranges::forward_range U>
	auto covariance(const T& v1, const U& v2)
	{
		return covariance(v1, mean(v1), v2, mean(v2));
	}
	
	template<std::ranges::forward_range T, std::ranges::forward_range U, class V, class W>
		auto covarianceNoBessel(const T& v1, const V& mean1, const U& v2, const W &mean2)
	{
		using resultType = decltype ((*std::ranges::cbegin(v1) - mean1)* (*std::ranges::cbegin(v2) - mean2));
		using unitlessType = typename TypeTraits<resultType>::unitlessType;

		resultType result = TypeTraits<resultType>::zero;
		auto it1 = std::ranges::cbegin(v1);
		auto it2 = std::ranges::cbegin(v2);
		for (; it1 != std::ranges::cend(v1); ++it1, ++it2)
			result += (*it1 - mean1) * (*it2 - mean2);

		return result / unitlessType(std::ranges::cend(v1) - std::ranges::cbegin(v1));
	}

	template<std::ranges::forward_range T, std::ranges::forward_range U>
	auto covarianceNoBessel(const T& v1, const U& v2)
	{
		return covarianceNoBessel(v1, mean(v1), v2, mean(v2));
	}

	template<std::ranges::forward_range T, class U, class V>
	void meanAndStdev(const std::vector<T>& v, U& mean, V& stDev)
	{
		mean = sci::mean(v);
		stDev = sci::sqrt(centralMoment<2>(v, mean));
	}

	template<std::ranges::forward_range T, class U, class V>
	void meanAndStdevNoBessel(const T& v, U& mean, V& stDev)
	{
		mean = sci::mean(v);
		stDev = sci::sqrt(centralMomentNoBessel<2>(v, mean));
	}

	template<std::ranges::forward_range T>
	auto stdev(const T& v)
	{
		auto mean = sci::mean(v);
		return sci::sqrt(centralMoment<2>(v, mean));
	}

	template<std::ranges::forward_range T>
	auto stdevNoBessel(const T& v)
	{
		auto mean = sci::mean(v);
		return sci::sqrt(centralMomentNoBessel<2>(v, mean));
	}

	template<std::ranges::forward_range T, class U>
	auto stdev(const T& v, const U& mean)
	{
		return sci::sqrt(centralMoment<2>(v, mean));
	}

	template<std::ranges::forward_range T, class U>
	auto stdevNoBessel(const T& v, const U& mean)
	{
		return sci::sqrt(centralMomentNoBessel<2>(v, mean));
	}

	template<class T, class U, class V, class W>
	auto varianceOfTheVariance(const T& fourthMoment, const U& variance, const V& mean, W n)
	{
		return (fourthMoment - (n - 3.0) / (n - 1.0) * variance * variance) / n;
	}

	template<std::ranges::forward_range T>
	auto varianceOfTheVariance(const T &v)
	{
		decltype(variance(v)) var;
		decltype(sci::variance(std::vector<decltype(var)>(0))) varOfVar;
		sci::variance(v, var, varOfVar);
		return varOfVar;
	}

	template<std::ranges::forward_range T>
	auto varianceOfTheVarianceNoBessel(const T& v)
	{
		decltype(variance(v)) var;
		decltype(varianceOfTheVariance(v)) varOfVar;
		sci::varianceNoBessel(v, var, varOfVar);
		return varOfVar;
	}

	template<std::ranges::forward_range T>
	void variance(const T& v, decltype(sci::variance(v))& variance, decltype(varianceOfTheVariance(v))& varianceofthevariance)
	{
		auto meanval = sci::mean(v);
		variance = sci::variance(v, meanval);
		auto fourthMoment = sci::centralMoment<4>(v, meanval);
		varianceofthevariance = sci::varianceOfTheVariance(fourthMoment, variance, meanval, v.size());
	}

	template<std::ranges::forward_range T>
	void varianceNoBessel(const T& v, decltype(sci::variance(v))& variance, decltype(varianceOfTheVariance(v))& varianceofthevariance)
	{
		auto mean = sci::mean(v);
		variance = sci::varianceNoBessel(v, mean);
		auto fourthMoment = centralMomentNoBessel<4>(v, mean);
		varianceofthevariance = sci::varianceOfTheVariance(fourthMoment, variance, mean, v.size());
	}

	template<std::ranges::forward_range T>
	auto min(const T& v)
	{
		using retType = std::remove_cvref_t<decltype(*std::ranges::cbegin(v))>;
		if (std::ranges::size(v) == 0)
			return std::numeric_limits<retType>::quiet_NaN();
		retType result = std::numeric_limits<retType>::has_infinity ? std::numeric_limits<retType>::infinity() : std::numeric_limits<retType>::max();
		for (auto vi : v)
			if (vi<result)
				result = vi;
		return result;
	}

	template<std::ranges::forward_range T, class U>
	auto minGtLimit(const T& v, const U &limit)
	{
		using retType = std::remove_cvref_t<decltype(*std::ranges::cbegin(v))>;
		bool found = false;
		retType result = std::numeric_limits<retType>::has_infinity ? std::numeric_limits<retType>::infinity() : std::numeric_limits<retType>::max();
		for (auto vi : v)
		{
			if (vi < result && vi > limit)
			{
				result = vi;
				found = true;
			}
		}
		return found ? result : std::numeric_limits<retType>::quiet_NaN();
	}

	template<std::ranges::forward_range T, class U>
	auto minGteLimit(const T& v, const U& limit)
	{
		using retType = std::remove_cvref_t<decltype(*std::ranges::cbegin(v))>;
		bool found = false;
		retType result = std::numeric_limits<retType>::has_infinity ? std::numeric_limits<retType>::infinity() : std::numeric_limits<retType>::max();
		for (auto vi : v)
		{
			if (vi < result && vi >= limit)
			{
				result = vi;
				found = true;
			}
		}
		return found ? result : std::numeric_limits<retType>::quiet_NaN();
	}

	template<std::ranges::forward_range T>
	auto max(const T& v)
	{
		using retType = std::remove_cvref_t<decltype(*std::ranges::cbegin(v))>;
		if (std::ranges::size(v) == 0)
			return std::numeric_limits<retType>::quiet_NaN();
		retType result = std::numeric_limits<retType>::has_infinity ? -std::numeric_limits<retType>::infinity() : std::numeric_limits<retType>::lowest();
		for (auto vi : v)
			if (vi > result)
				result = vi;
		return result;
	}

	template<std::ranges::forward_range T, class U>
	auto maxLtLimit(const T& v, const U& limit)
	{
		using retType = std::remove_cvref_t<decltype(*std::ranges::cbegin(v))>;
		bool found = false;
		retType result = std::numeric_limits<retType>::has_infinity ? -std::numeric_limits<retType>::infinity() : std::numeric_limits<retType>::lowest();
		for (auto vi : v)
		{
			if (vi > result && vi < limit)
			{
				result = vi;
				found = true;
			}
		}
		return found ? result : std::numeric_limits<retType>::quiet_NaN();
	}

	template<std::ranges::forward_range T, class U>
	auto maxLteLimit(const T& v, const U& limit)
	{
		using retType = std::remove_cvref_t<decltype(*std::ranges::cbegin(v))>;
		bool found = false;
		retType result = std::numeric_limits<retType>::has_infinity ? -std::numeric_limits<retType>::infinity() : std::numeric_limits<retType>::lowest();
		for (auto vi : v)
		{
			if (vi > result && vi <= limit)
			{
				result = vi;
				found = true;
			}
		}
		return found ? result : std::numeric_limits<retType>::quiet_NaN();
	}

	template<class T, class U>
	auto linearinterpolate(T x, T x1, T x2, U y1, U y2)
	{
		auto m = (y1 - y2) / (x1 - x2);
		return m * (x - x1) + y1;
	}

	/*
	template<class T, class U>
	auto variance(const std::vector<T>& v, const std::vector<U>& weights)
	{
		typedef decltype(variance(v)) returnType;
		typedef decltype(variance(v)* weights[0]) sumType;
		if (v.size() == 0)
			return std::numeric_limits<returnType>::quiet_NaN();
		sci::assertThrow(v.size() == weights.size(), sci::err());
		auto meanval = sci::mean(v, weights);
		sumType sum = 0.0;
		U weight = 0.0;
		auto vi = &v[0];
		auto vEnd = vi + v.size();
		auto weighti = &weights[0];
		for (; vi != vEnd; ++vi, ++weighti)
		{
			sum += (*vi - meanval) * (*vi - meanval) * *weighti;
			weight += *weighti;
		}
		return sum / weight;
	}

	template<class T, class U, class V>
	auto variance(const std::vector<T>& v, const std::vector<U>& weights, const V& mean)
	{
		typedef decltype(variance(v)) returnType;
		typedef decltype(variance(v)* weights[0]) sumType;
		if (v.size() == 0)
			return std::numeric_limits<returnType>::quiet_NaN();
		sci::assertThrow(v.size() == weights.size(), sci::err());
		sumType sum = 0.0;
		U weight = 0.0;
		auto vi = &v[0];
		auto vEnd = vi + v.size();
		auto weighti = &weights[0];
		for (; vi != vEnd; ++vi, ++weighti)
		{
			sum += (*vi - mean) * (*vi - mean) * *weighti;
			weight += *weighti;
		}
		return sum / weight;
	}

	template<class T, class U>
	auto stdev(const std::vector<T>& v, const U& mean)
	{
		auto var = variance(v, mean);
		return TypeTraits<decltype(var)>::sqrt(var);
	}

	template<class T>
	auto  stdev(const std::vector<T>& v)
	{
		auto meanval = sci::mean(v);
		return sci::stdev(v, meanval);
	}

	template<class T, class U>
	auto stdev(const std::vector<T>& v, const std::vector<U>& weights)
	{
		auto meanval = sci::mean(v, weights);
		return sci::stdev(v, weights, meanval);
	}

	template<class T, class U, class V>
	auto stdev(const std::vector<T>& v, const std::vector<U>& weights, const V& mean) -> decltype(stdev(v))
	{
		return TypeTraits<T>::sqrt(variance(v, weights, mean));
	}

	template<class T, class U>
	auto stdevnobessel(const std::vector<T>& v, const U& mean) -> decltype(stdev(v))
	{
		return TypeTraits<T>::sqrt(variancenobessel(v, mean));
	}

	template<class T, class U, class V>
	T varianceOfTheVariance(const T& fourthMoment, const U& variance, const V& mean, T n)
	{
		return (fourthMoment - T(n - 3.0) / T(n - 1.0) * variance * variance) / n;
	}

	template<class T>
	auto varianceOfTheVariance(const std::vector<T>& v) -> decltype(sci::variance(std::vector<decltype(variance(v))>(0)))
	{
		decltype(variance(v)) var;
		decltype(sci::variance(std::vector<decltype(var)>(0))) varOfVar;
		sci::variance(v, var, varOfVar);
		return varOfVar;
	}

	template<class T>
	auto varianceOfTheVarianceNoBessel(const std::vector<T>& v) -> decltype(varianceOfTheVariance(v))
	{
		decltype(variance(v)) var;
		decltype(varianceOfTheVariance(v)) varOfVar;
		sci::varianceNoBessel(v, var, varOfVar);
		return varOfVar;
	}

	template<class T>
	void variance(const std::vector<T>& v, decltype(sci::variance(v))& variance, decltype(varianceOfTheVariance(v))& varianceofthevariance)
	{
		T meanval = sci::mean(v);
		variance = sci::variance(v, meanval);
		auto fourthMoment = sci::centralMoment<4>(v, meanval);
		varianceofthevariance = sci::varianceOfTheVariance(fourthMoment, variance, meanval, (T)v.size());
	}

	template<class T>
	void varianceNoBessel(const std::vector<T>& v, decltype(sci::variance(v))& variance, decltype(varianceOfTheVariance(v))& varianceofthevariance)
	{
		T mean = sci::mean(v);
		variance = sci::varianceNoBessel(v, mean);
		auto fourthMoment = centralMomentNoBessel<4>(v, mean);
		varianceofthevariance = sci::varianceOfTheVariance(fourthMoment, variance, mean, (T)v.size());
	}

	template<class T>
	T geometricMean(const std::vector<T>& v)
	{
		if (v.size() == 0) return std::numeric_limits<T>::quiet_NaN();
		T result = v[0];
		const T* vi = &v[0] + 1;
		const T* vilimit = &v[0] + v.size();
		for (; vi < vilimit; ++vi)
			result += *vi * *vi;
		auto count = decltype(anyBaseVal(v))(v.size());
		return std::sqrt(result) / count;
	}

	template<class T>
	std::vector<T> geometricMean(const std::vector<std::vector<T>>& v)
	{
		if (v.size() == 0) return std::vector<T>(0);
		std::vector<T> result(v[0].size(), 0.0);
		for (typename std::vector<std::vector<T>>::const_iterator vi = v.begin(); vi != v.end(); ++vi)
			result += *vi * *vi;
		return sci::sqrt(result) / decltype(anyBaseVal(v))(v.size());
	}


	template< class U, class T>
	inline U min(const std::vector<T>& v)
	{
		if (v.size() == 0) return std::numeric_limits<U>::quiet_NaN();
		if constexpr (VectorTraits<std::vector<T>>::nDimensions == 1)
		{
			typename std::vector<T>::const_iterator vi = v.begin();
			U result = *vi;
			++vi;
			while (result != result && vi != v.end())
			{
				result = *vi;
				++vi;
			}
			for (; vi != v.end(); ++vi)
				result = *vi < result ? *vi : result;
			return result;

		}
		else
		{
			typename std::vector<T>::const_iterator vi = v.begin();
			U result = min<U>(*vi);
			++vi;
			while (result != result && vi != v.end())
			{
				result = min<U>(*vi);
				++vi;
			}
			for (; vi != v.end(); ++vi)
			{
				U newmin = min<U>(*vi);
				result = newmin < result ? newmin : result;
			}
			return result;
		}
	}

	//return the minimum value from a vector greater than a given limit
	template< class U, class T>
	U mingtlimit(const std::vector<T>& v, U limit)
	{
		//this will only be called by 1d vectors as we have a 2d specialisation
		U result = std::numeric_limits<U>::quiet_NaN();
		if (v.size() == 0) return std::numeric_limits<U>::quiet_NaN();
		typename std::vector<T>::const_iterator vi = v.begin();
		while ((*vi != *vi || !(*vi > limit)) && vi != v.end())
		{
			++vi;
		}
		if (vi == v.end()) return result;
		result = *vi;
		//search the remaining numbers
		for (; vi != v.end(); ++vi) result = (*vi) < result && (*vi) > limit ? (*vi) : result;
		return result;
	}

	//return the minimum value from a vector greater than or equal to a given limit
	template< class U, class T>
	U mingtelimit(const std::vector<T>& v, U limit)
	{
		//this will only be called by 1d vectors as we have a 2d specialisation
		if (v.size() == 0) return std::numeric_limits<U>::quiet_NaN();
		U result = std::numeric_limits<U>::quiet_NaN();
		typename std::vector<T>::const_iterator vi = v.begin();
		while ((*vi != *vi || !(*vi >= limit)) && vi != v.end())
		{
			++vi;
		}
		if (vi == v.end()) return result;
		result = *vi;
		//search the remaining numbers
		for (; vi != v.end(); ++vi) result = (*vi) < result && (*vi) >= limit ? (*vi) : result;
		return result;
	}


	//return the maximum value from a vector less than a given limit
	template< class U, class T>
	U maxltlimit(const std::vector<T>& v, U limit)
	{
		//this will only be called by 1d vectors as we have a 2d specialisation
		if (v.size() == 0) return std::numeric_limits<U>::quiet_NaN();
		U result = std::numeric_limits<U>::quiet_NaN();
		typename std::vector<T>::const_iterator vi = v.begin();
		while ((*vi != *vi || !(*vi < limit)) && vi != v.end())
		{
			++vi;
		}
		if (vi == v.end()) return result;
		result = *vi;
		//search the remaining numbers
		for (; vi != v.end(); ++vi) result = (*vi) > result && (*vi) < limit ? (*vi) : result;
		return result;
	}

	//return the maximum value from a vector less than or equal to a given limit
	template< class U, class T>
	U maxltelimit(const std::vector<T>& v, U limit)
	{
		//this will only be called by 1d vectors as we have a 2d specialisation
		if (v.size() == 0) return std::numeric_limits<U>::quiet_NaN();
		U result = std::numeric_limits<U>::quiet_NaN();
		typename std::vector<T>::const_iterator vi = v.begin();
		while ((*vi != *vi || !(*vi <= limit)) && vi != v.end())
		{
			++vi;
		}
		if (vi == v.end()) return result;
		result = *vi;
		//search the remaining numbers
		for (; vi != v.end(); ++vi) result = (*vi) > result && (*vi) <= limit ? result : (*vi);
		return result;
	}

	template<class U, class T >
	U max(const std::vector<T>& v)
	{
		if (v.size() == 0) return std::numeric_limits<U>::quiet_NaN();
		typename std::vector<T>::const_iterator vi = v.begin();
		U result = max<U>(*vi);
		++vi;
		while (result != result && vi != v.end())
		{
			result = max<U>(*vi);
			++vi;
		}
		for (; vi != v.end(); ++vi)
		{
			U newmax = max<U>(*vi);
			result = newmax > result ? newmax : result;
		}
		return result;
	}

	template<class T>
	size_t indexofmax(std::vector<T> v)
	{
		if (v.size() == 0) return std::numeric_limits<size_t>::quiet_NaN();
		size_t result = 0;
		typename std::vector<T>::const_iterator vi = v.begin();
		T currentmax = *vi;
		++vi;
		while (currentmax != currentmax && vi != v.end())
		{
			currentmax = *vi;
			++vi;
			++result;
		}
		for (; vi != v.end(); ++vi)
		{
			if (*vi > currentmax)
			{
				result = vi - v.begin();
				currentmax = *vi;
			}
		}
		return result;
	}

	template<class T>
	size_t indexofmin(std::vector<T> v)
	{
		if (v.size() == 0) return std::numeric_limits<size_t>::quiet_NaN();
		size_t result = 0;
		typename std::vector<T>::const_iterator vi = v.begin();
		T currentmin = *vi;
		++vi;
		while (currentmin != currentmin && vi != v.end())
		{
			currentmin = *vi;
			++vi;
			++result;
		}
		for (; vi != v.end(); ++vi)
		{
			if (*vi < currentmin)
			{
				result = vi - v.begin();
				currentmin = *vi;
			}
		}
		return result;
	}

	template<class T>
	std::vector<T> max(const std::vector<T>& v1, const std::vector<T>& v2)
	{
		std::vector<T> result(std::min(v1.size(), v2.size()));
		if (result.size() == 0) return result;
		typename std::vector<T>::const_iterator v1i = v1.begin();
		typename std::vector<T>::const_iterator v2i = v2.begin();
		for (typename std::vector<T>::iterator resulti = result.begin(); resulti != result.end(); ++resulti)
		{
			*resulti = std::max(*v1i, *v2i);
			++v1i;
			++v2i;
		}
		return result;
	}

	template<class T>
	std::vector<T> min(const std::vector<T>& v1, const std::vector<T>& v2)
	{
		std::vector<T> result(std::min(v1.size(), v2.size()));
		if (result.size() == 0) return result;
		typename std::vector<T>::const_iterator v1i = v1.begin();
		typename std::vector<T>::const_iterator v2i = v2.begin();
		for (typename std::vector<T>::iterator resulti = result.begin(); resulti != result.end(); ++resulti)
		{
			*resulti = std::min(*v1i, *v2i);
			++v1i;
			++v2i;
		}
		return result;
	}
	*/

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

	//find the kth Biggest element. k=0 is the smallest element
	template<std::ranges::forward_range RANGE>
	auto findKthBiggestValueInPlace(RANGE&& v, size_t k)
	{
		sci::assertThrow(v.size() > k, sci::err(sci::SERR_VECTOR, 0, sU("Attempt to find the kth biggest element of a vector, but k was too large for the vector")));

		auto partitionValue = *(v.end() - 1);
		auto begin = v.begin();
		auto end = v.end();
		auto partitionPoint = partitionOnLastElement(begin, end);
		while (begin + k != partitionPoint)
		{
			if (begin + k < partitionPoint)
			{
				end = partitionPoint;
				partitionPoint = partitionOnLastElement(begin, end);
			}
			else
			{
				k -= partitionPoint - begin;
				begin = partitionPoint;
				partitionPoint = partitionOnLastElement(begin, end);
			}
		}
		return *partitionPoint;
	}

	//find the median of a vector
	template<std::ranges::forward_range RANGE>
	auto medianInPlace(RANGE&& v)
	{
		using value_type = std::remove_cvref_t<decltype(*v.begin())>;
		if (v.size() % 2 == 1)
			return findKthBiggestValue(v, v.size() / 2);

		if (v.size() == 0)
			return std::numeric_limits<value_type>::quiet_NaN();

		//find the larger of the two values needed to caculate the median

		size_t k = v.size() / 2;
		value_type value1 = findKthBiggestValueInPlace(v, k);

		//now find the smaller - because we have already partitioned the array around the larger
		//value, this is just the max of the values before the partition point
		value_type value2 = v.front();
		for (auto iter = v.begin(); iter != v.end() && (*iter) <= value1; ++iter)
			if (*iter > value2)
				value2 = *iter;

		return value_type((value1 + value2) / sci::TypeTraits<decltype(value1)>::unitlessType(2.0));
	}
	template<std::ranges::forward_range RANGE>
	auto findKthBiggestValue(RANGE&& v, size_t k)
	{
		std::vector copy(v.begin(), v.end());
		return findKthBiggestValueInPlace(copy, k);
	}
	template<std::ranges::random_access_range RANGE>
	auto median(RANGE&& v)
	{
		std::vector copy(v.begin(), v.end());
		return medianInPlace(copy);
	}

	template <std::ranges::forward_range T, std::ranges::forward_range U>
	auto integrate(T x, U y)
	{
		using returnType = decltype((*(std::ranges::begin(x)) - *(std::ranges::begin(x))) * (* (std::ranges::begin(y))));
		returnType result(0);
		auto yi = y.begin() + 1;
		for (auto xi = x.begin() + 1; xi != x.end(); ++xi)
		{
			result += (*yi + *(yi - 1)) * (*xi - *(xi - 1));
			++yi;
		}
		result *= TypeTraits<returnType>::unitlessType(0.5);
		return result;
	}

	template <std::ranges::forward_range T, std::ranges::forward_range U, class V, class W>
	auto integrate(T x, U y, V minx, W maxx)
	{
		using returnType = decltype((*(std::ranges::begin(x)) - *(std::ranges::begin(x)))* (*(std::ranges::begin(y))));
		returnType result(0);
		auto yi = y.begin() + 1;
		bool swappedLimits = false;
		if (maxx < minx)
		{
			std::swap(minx, maxx);
			swappedLimits = true;
		}
		sci::assertThrow(minx >= x[0], sci::err(sci::SERR_VECTOR, 0, sU("sci::integrate called with a min bound outside the data range.")));
		sci::assertThrow(maxx <= x.back(), sci::err(sci::SERR_VECTOR, 0, sU("sci::integrate called with a max bound outside the data range.")));
		for (auto xi = x.begin() + 1; xi != x.end(); ++xi, ++yi)
		{
			typename T::value_type x0 = *(xi - 1);
			typename T::value_type x1 = *xi;
			typename U::value_type y0 = *(yi - 1);
			typename U::value_type y1 = *yi;
			if (x1 < minx)
				continue;
			if (x0 > maxx)
				continue;
			if (x0 < minx)
			{
				y0 = typename U::value_type((y1 - y0) / (x1 - x0) * (minx - x0) + y0);
				x0 = minx;
			}
			if (x1 > maxx)
			{
				y1 = typename U::value_type((y1 - y0) / (x1 - x0) * (maxx - x0) + y0);
				x1 = maxx;
			}

			result += returnType((y0 + y1) * (x1 - x0));
		}
		result *= TypeTraits<returnType>::unitless(0.5);
		if (swappedLimits)
			return -result;
		return result;
	}

	template<std::ranges::forward_range T, std::ranges::forward_range U, class V>
	void assign(T& destination, const U& filter, const V& value)
	{
		auto iterD = destination.begin();
		auto iterF = filter.begin();
		for (; iterD != destination.end(); ++iterD, ++iterF)
			if (*iterF != 0)
				assign(*iterD, value);
	}

	template<std::ranges::random_access_range T, std::ranges::forward_range U>
	void reorder(T& v, const U& newLocations)
	{
		T result=v;
		size_t i = 0;
		for (auto locIter = newLocations.begin(); locIter!=newLocations.end(); ++locIter, ++i)
			assign(result[*locIter], v[i]);
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

	template< std::ranges::forward_range T, std::ranges::forward_range U, class V, class W>
	void fitstraightline(const T& x, const U& y, V& grad, W& intercept)
	{
		auto meanX = mean(x);
		auto varX = varianceNoBessel(x, meanX);
		auto meanY = mean(y);
		auto covarXY = covarianceNoBessel(x, meanX, y, meanY);

		grad = covarXY / varX;
		intercept = meanY - grad * meanX;
	}

	template< std::ranges::forward_range T, std::ranges::forward_range U, class V, class W, class X, class Y, class Z>
	void fitstraightline(const T& x, const U& y, V& grad, W& intercept, X& vargrad, Y& varintercept, Z& covar)
	{
		auto meanX = mean(x);
		auto varX = varianceNoBessel(x, meanX);
		auto meanY = mean(y);
		auto covarXY = covarianceNoBessel(x, meanX, y, meanY);

		grad = covarXY / varX;
		intercept = meanY - grad * meanX;

		auto xIter = x.begin();
		auto yIter = y.begin();
		auto xEnd = x.end();

		decltype((*yIter - (grad * *xIter + intercept)) * (*yIter - (grad * *xIter + intercept))) varYResiduals(0.0);

		for (; xIter != xEnd; ++xIter, ++yIter)
		{
			auto residual = *yIter - (grad * *xIter + intercept);
			varYResiduals += residual * residual;
		}
		varYResiduals /= typename TypeTraits<decltype(varYResiduals)>::unitlessType(y.size() - 2);

		vargrad = varYResiduals / varX / (double)y.size();
		varintercept = varYResiduals / (double)y.size() * (1.0 + meanX * meanX / varX);
		covar = -varYResiduals * meanX / varX * double(y.size() - 2) / double(y.size());
	}
	//fits a straight line, removing any outliers with residuals bigger than maxresiduals
	//This is performed one at a time, removing the worst residual then fitting to the
	//remaining data set
	//Returns the number of data points used in the fit
	template< std::ranges::forward_range T, std::ranges::forward_range U, class V, class W, class X>
	size_t fitstraightlinewithoutlierremoval(const T& x, U& y, V& grad, W& intercept, X maxResidualLimit)
	{
		std::vector<typename T::value_type> fitx (x.begin(), x.end());
		std::vector<typename U::value_type> fity (y.begin(), y.end());
		auto maxResidualLimitSquared = maxResidualLimit * maxResidualLimit;
		fitstraightline(fitx, fity, grad, intercept);

		size_t maxResidualIndex = 0;
		auto firstResidual = fity[0] - fitx[0] * grad - intercept;
		auto firstResidualSquared = firstResidual * firstResidual;
		auto maxResidualSquared = firstResidualSquared;
		for (size_t i = 0; i < fitx.size(); ++i)
		{
			auto residual = fity[i] - fitx[i] * grad - intercept;
			auto residualSquared = sci::pow(fity[0] - fitx[0] * grad - intercept, 2);
			if (residualSquared > maxResidualSquared)
			{
				maxResidualSquared = residualSquared;
				maxResidualIndex = i;
			}
		}
		
		while (maxResidualSquared > maxResidualLimit)
		{
			//this is a really bad and slow way to do it!
			//Would be much better to skip points we don't want rather than 
			//erasing and moving
			fitx.erase(fitx.begin() + maxResidualIndex);
			fity.erase(fity.begin() + maxResidualIndex);
			fitstraightline(fitx, fity, grad, intercept);

			maxResidualIndex = 0;
			firstResidual = fity[0] - fitx[0] * grad - intercept;
			firstResidualSquared = firstResidual * firstResidual;
			maxResidualSquared = firstResidualSquared;
			for (size_t i = 0; i < fitx.size(); ++i)
			{
				auto residual = fity[i] - fitx[i] * grad - intercept;
				auto residualSquared = sci::pow(fity[0] - fitx[0] * grad - intercept, 2);
				if (residualSquared > maxResidualSquared)
				{
					maxResidualSquared = residualSquared;
					maxResidualIndex = i;
				}
			}
		}

		return fitx.size();
	}
}