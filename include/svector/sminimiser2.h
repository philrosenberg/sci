#pragma once

#include<assert.h>
#include"ArrayManipulation.h"
#include<svector/svector.h>
/*
//minimiser must be a functor that accepts fitParams and returns the value to minimise
template <class MINIMISER>
void minimise(MINIMISER &minimiser, std::vector<double> &fitParams, std::vector<double> scales)
{
	const double fallRate = 0.999;

	double currentVal = minimiser(fitParams);
	if (currentVal != currentVal)
		return;
	double bestVal = currentVal;
	std::vector<double> bestParams = fitParams;
	double energy = currentVal / 100.0;

	sci::RangedRandomReal rand(0.0, 1.0);
	std::vector<double> nextGuess(fitParams.size());

	size_t counter = 0;
	size_t failCounter = 0;
	while (failCounter < 1000)
	{
		for (size_t i = 0; i < fitParams.size(); ++i)
		{
			nextGuess[i] = fitParams[i] + sci::InverseCumulativeNormalDistribution(0, scales[i]).getX(rand.get());
		}
		double nextVal = minimiser(nextGuess);
		if (nextVal < currentVal || std::exp((currentVal - nextVal) / energy)>rand.get())
		{
			fitParams = nextGuess;
			currentVal = nextVal;
			if (currentVal < bestVal)
			{
				bestVal = currentVal;
				bestParams = fitParams;
			}
			failCounter = 0;
		}
		else
			++failCounter;

		++counter;
		energy *= fallRate;
		//scales *= std::sqrt(fallRate);
	}
	fitParams = bestParams;
}

template<class FUNCTOR>
void differentiate(FUNCTOR &functor, const std::vector<double> x, size_t index, double deltaX)
{
	std::vector<double> x1 = x;
	std::vector<double> x2 = x;
	x1[index] -= deltaX;
	x2[index] += deltaX;
	double y1 = functor(x1);
	double y2 = functor(x2);
	return(y2 - y1) / 2.0 / deltaX;
}

template <class MINIMISER>
void minimiseGradientDescent(MINIMISER &minimiser, std::vector<double> &fitParams, std::vector<double> scales, std::vector<double> differentialDistances)
{

	std::vector<double> nextGuess(fitParams);

	size_t counter = 0;
	while (counter < 1000)
	{
		for (size_t i = 0; i < fitParams.size(); ++i)
			nextGuess[i] -= differentiate(minimiser, fitParams, i, differentialDistances[i])*scales[i];
		fitParams = nextGuess;
	}
}
*/
namespace sci
{
	//find two points that bracket one or more minima of a function
	//  function is the function to minimise. It may be a c-style function or a class with operator() defined. It must take one parameter, a double, and return the evaluation of the function as a double
	//  firstGuess is the guess for the location of the minima
	// lowerBracket and upperBracket are the outputs. The lowerBracket value will always be less than upperBracket.
	template <class FUNCTOR, class T, class U, class V, class W, class X, class A, class B, class C>
	void bracketMinimum(FUNCTOR& function, T firstGuess, U firstGuessUncertainty, V& lowerBracket, W& upperBracket, X& bestGuess, A& lowerValue, B&upperValue, C&bestValue)
	{
		using unitless = decltype(T() / T());
		using result_type = decltype(function(firstGuess));
		const unitless goldenRatioPlusOne((3.0 - std::sqrt(5.0)) / 2.0 + 1.0);

		firstGuessUncertainty = sci::abs(firstGuessUncertainty);

		//Find a non insignificant distance to start jumping - include checking if firstGuessUncertainty is zero 
		T difference = firstGuessUncertainty != T(0) ? T(firstGuessUncertainty * unitless(6)) : T(1);
		if (difference < sci::abs(firstGuess) * std::numeric_limits<unitless>::epsilon() * unitless(6))
			difference = sci::abs(firstGuess) * std::numeric_limits<unitless>::epsilon() * unitless(6);

		lowerBracket = firstGuess;
		upperBracket = firstGuess + difference;
		lowerValue = function(lowerBracket);
		upperValue = function(upperBracket);
		assert(lowerValue == lowerValue);
		assert(upperValue == upperValue);
		bestGuess = lowerBracket;
		bestValue = lowerValue;
		if (upperValue < bestValue)
		{
			bestGuess = upperBracket;
			bestValue = upperValue;
		}
		T previousBestGuess = std::numeric_limits<T>::quiet_NaN();
		result_type previousBestValue = std::numeric_limits<result_type>::quiet_NaN();
		//expand upperBracket until we find a significant increase
		bool significantIncrease = (upperValue-bestValue)/(upperValue+bestValue) > sci::sqrt(std::numeric_limits<unitless>::epsilon());
		while (!significantIncrease)
		{
			auto difference = upperBracket - lowerBracket;
			upperBracket = lowerBracket + difference * goldenRatioPlusOne;
			upperValue = function(upperBracket);
			assert(upperValue == upperValue);
			if (upperValue < bestValue)
			{
				previousBestGuess = bestGuess;
				previousBestValue = bestValue;
				bestGuess = upperBracket;
				bestValue = upperValue;
			}
			significantIncrease = (upperValue - bestValue) / (upperValue + bestValue) > sci::sqrt(std::numeric_limits<unitless>::epsilon());

			//if we reach infinity without finding an increase, then we are looking bad!
			if (upperBracket == std::numeric_limits<W>::infinity())
				throw(std::exception("Failed to find a gradient increase during minima searching"));
		}
		//if bestValue is less than previousBestValue, this means we descended, then ascended again - we can set lowerBracket to
		//previousBestGuess and we are done
		if (bestValue < previousBestValue)
		{
			lowerBracket = previousBestGuess;
			lowerValue = previousBestValue;
			return;
		}
		//now expand lowerBracket until we find a significant increase
		significantIncrease = (lowerValue - bestValue) / (lowerValue + bestValue) > sci::sqrt(std::numeric_limits<unitless>::epsilon());
		while (!significantIncrease)
		{
			auto difference = upperBracket - lowerBracket;
			lowerBracket = upperBracket - difference * goldenRatioPlusOne;
			lowerValue = function(lowerBracket);
			assert(lowerValue == lowerValue);
			if (lowerValue < bestValue)
			{
				previousBestGuess = bestGuess;
				previousBestValue = bestValue;
				bestGuess = lowerBracket;
				bestValue = lowerValue;
			}
			significantIncrease = (lowerValue - bestValue) / (lowerValue + bestValue) > sci::sqrt(std::numeric_limits<unitless>::epsilon());

			//if we reach infinity without finding an increase, then we are looking bad!
			if (lowerBracket == std::numeric_limits<W>::infinity())
				throw(std::exception("Failed to find a gradient increase during minima searching"));
		}

		//if bestValue < secondBestValue then when expanding leftwards we descended then ascended, we can improve
		//our bracket bey setting  upperBracket to previousBestGuess
		if (bestValue < previousBestValue)
		{
			upperBracket = previousBestGuess;
			upperValue = previousBestValue;
			return;
		}




		/*
		//check to make sure we have a significant difference in the values we are comparing
		while (sci::abs(lowerValue - upperValue) < (sci::abs(lowerValue) + sci::abs(upperValue)) * sci::sqrt(std::numeric_limits<unitless>::epsilon()))
		{
			if (upperBracket == std::numeric_limits<T>::infinity())
			{
				//if we have reached "infinity" then set the uppervalue bigger than lowervalue to trigger looking the other direction
				upperValue = lowerValue + result_type(1);
				break;
			}
			difference *= unitless(2.0);
			lowerBracket = firstGuess - goldenRatio * difference;
			upperBracket = lowerBracket + difference;
			lowerValue = function(lowerBracket);
			upperValue = function(upperBracket);
			if (upperValue < bestValue)
			{
				bestGuess = upperBracket;
				bestValue = upperValue;
			}
			if (lowerValue < bestValue)
			{
				bestGuess = lowerBracket;
				bestValue = lowerValue;
			}
		}

		//if the upperValue is bigger than lowerVaue, then we are searching the wrong way
		if (upperValue > lowerValue)
		{
			//check the other direction
			upperBracket = firstGuess - difference;
			upperValue = function(upperBracket);
			if (upperValue < bestValue)
			{
				bestGuess = upperBracket;
				bestValue = upperValue;
			}
			//again check to make sure we have a significant difference in the values we are comparing
			while (sci::abs(lowerValue - upperValue) < (sci::abs(lowerValue) + sci::abs(upperValue)) * sci::sqrt(std::numeric_limits<unitless>::epsilon()))
			{
				if (upperBracket == std::numeric_limits<T>::infinity())
				{
					//There is no minimum or alternatively everywhere is the minimum.
					//return upper and lower brackets identical which will cause the minimisation routines to do nothing
					upperBracket = lowerBracket;
					upperValue = lowerValue;
					return;
				}
				difference *= unitless(2);
				upperBracket = firstGuess - difference;
				upperValue = function(upperBracket);
				if (upperValue < bestValue)
				{
					bestGuess = upperBracket;
					bestValue = upperValue;
				}
			}
			//if this is also bigger then we have already bracketed a minimum
			if (upperValue > lowerValue)
			{
				return;
			}
			//referse the direction we search
			difference *= unitless(-1);
		}

		//while we keep descending keep going, once we start ascending again we have bracketed the minimum
		while (upperValue < lowerValue)
		{
			//go in ever increasing steps
			difference *= unitless(2);
			lowerValue = upperValue;
			lowerBracket = upperBracket;
			upperBracket += difference;
			upperValue = function(upperBracket);
			if (upperValue < bestValue)
			{
				bestGuess = upperBracket;
				bestValue = upperValue;
			}
		}
		//we may have passed the minimum, so expand backwards one step
		lowerBracket -= difference / unitless(2);

		// if we were searching in the negative direction then our upper and lower brackets
		//will be the wrong way round so swap them
		if (upperBracket < lowerBracket)
			std::swap(upperBracket, lowerBracket);
			*/
	}

	//template <class FUNCTOR, class T, class U>
	//void bracketMinimum(FUNCTOR& function, T& lowerBracket, U& upperBracket)
	//{
	//	bracketMinimum(function, 0, lowerBracket, upperBracket);
	//}

	//Find the minimum of a function using the "Golden Ratio" method
	//  function is the function to minimise. It may be a c-style function or a class with operator() defined. It must take one parameter, a double, and return the evaluation of the function as a double
	//  firstGuess is a guess at the location of the minimum
	//  tollerance is the fractional tollerance of the result. It defaults to sqrt(epsilon) and there is little point making it smaller than this.
	//  maxIterations is the maximum number of iterations to perform. It defaults to the maximum size_t
	template <class FUNCTOR>
	double minimiseGoldenSection(FUNCTOR& function, double firstGuess, double absoluteTollerance = 0.0, double tollerance = std::sqrt(std::numeric_limits<double>::epsilon()), size_t maxIterations = -1)
	{
		const double goldenRatio = (3.0 - std::sqrt(5.0)) / 2.0;

		double lower;
		double upper;
		bracketMinimum(function, firstGuess, lower, upper);
		double mid = lower + (upper - lower) * goldenRatio;

		double midVal = function(mid);
		double lowerVal = function(lower);
		double upperVal = function(upper);

		size_t count = 0;
		bool accurate = (upper - lower) < (mid + (upper + lower) / 2.0) * tollerance; //we combine mid with average of upper and lower for cases where mid is very close to zero - note we sum as we have the full max to min span
		accurate |= (upper - lower) < absoluteTollerance;
		while (!accurate && count < maxIterations)
		{
			double lowerDiff = mid - lower;
			double upperDiff = upper - mid;

			double next;
			double nextVal;
			if (lowerDiff > upperDiff)
			{
				next = mid - (mid - lower) * goldenRatio;
				nextVal = function(next);

				if (nextVal > midVal)
				{
					lower = next;
					lowerVal = nextVal;
				}
				else
				{
					upper = mid;
					upperVal = midVal;
					mid = next;
					midVal = nextVal;
				}
			}
			else
			{
				next = mid + (upper - mid) * goldenRatio;
				nextVal = function(next);
				if (nextVal > midVal)
				{
					upper = next;
					upperVal = nextVal;
				}
				else
				{
					lower = mid;
					lowerVal = midVal;
					mid = next;
					midVal = nextVal;
				}
			}
			accurate = (upper - lower) < (mid + (upper + lower) / 2.0) * tollerance; //we combine mid with average of upper and lower for cases where mid is very close to zero - note we sum as we have the full max to min span
			++count;
		}
		return mid;
	}

	//Find the minimum of a multi-d function along a given direction using the "Golden Ratio" method
	//  function is the function to minimise. It may be a c-style function or a class with operator() defined. It must take one parameter, a const std::vector<double>, and return the evaluation of the function at that point as a double
	//  firstGuess is a guess at the location of the minimum
	//  tollerance is the fractional tollerance of the result in each dimension. It is futile making this less than sqrt(epsilon)
	//  maxIterations is the maximum number of iterations to perform. It defaults to the maximum size_t
	template <class FUNCTOR>
	std::vector<double> minimiseOnLineGoldenSection(FUNCTOR& function, const std::vector<double>& firstGuess, const std::vector<double>& direction, const std::vector<double>& absoluteTollerance, const std::vector<double>& tollerance, size_t maxIterations = -1)
	{
		//lamda to move a distance x in the given direction
		auto move = [&](double x) mutable
		{
			std::vector<double> result(firstGuess.size());
			for (size_t i = 0; i < firstGuess.size(); ++i)
				result[i] = firstGuess[i] + direction[i] * x;
			return result;
		};
		double distance = minimiseGoldenSection(move, 1.0, sci::abs(sci::dot(direction, absoluteTollerance)), sci::abs(sci::dot(direction, tollerance)), maxIterations);
		return firstGuess + direction * distance;
	}

	//Find the minimum of a multi-d function along a given direction using the "Golden Ratio" method. In this version a default fractional tollerance of sqrt(epsilon) is used
	//  function is the function to minimise. It may be a c-style function or a class with operator() defined. It must take one parameter, a const std::vector<double>, and return the evaluation of the function at that point as a double
	//  firstGuess is a guess at the location of the minimum
	//  tollerance is the fractional tollerance of the result in each dimension. It defaults to sqrt(epsilon) and there is little point making it smaller than this.
	//  maxIterations is the maximum number of iterations to perform. It defaults to the maximum size_t

	template <class FUNCTOR>
	std::vector<double> minimiseOnLineGoldenSection(FUNCTOR& function, const std::vector<double>& firstGuess, const std::vector<double>& direction, size_t maxIterations = -1)
	{
		//lamda to move a distance x in the given direction
		auto move = [&](double x) mutable
		{
			std::vector<double> result(firstGuess.size());
			for (size_t i = 0; i < firstGuess.size(); ++i)
				result[i] = firstGuess[i] + direction[i] * x;
			return result;
		};
		double distance = minimiseGoldenSection(move, 1.0, 0.0, std::sqrt(std::numeric_limits<double>::epsilon()), maxIterations);
		return firstGuess + direction * distance;
	}

	/*template <class FUNCTOR>
	std::vector<double> minimiseOnLineGoldenSection(FUNCTOR function, const std::vector<double> &firstGuess, const std::vector<double> &direction, const std::vector<double> &accuracy, size_t maxIterations=-1)
	{
	const double goldenRatio = (3.0 - std::sqrt(5.0)) / 2.0;
	size_t firstNonZeroIndex = 0;
	for (size_t i = 0; i < direction.size(); ++i)
	{
	if (direction[i] != 0)
	{
	firstNonZeroIndex = i;
	break;
	}
	}

	std::vector<double> mid = firstGuess;
	std::vector<double> lower = mid - direction;
	std::vector<double> upper = mid + direction / goldenRatio;

	double midVal = function(mid);
	double lowerVal = function(lower);
	double upperVal = function(upper);

	while (upperVal < midVal)
	{
	lower = mid;
	lowerVal = midVal;
	mid = upper;
	midVal = upperVal;
	upper = mid*(1.0 + 1.0/goldenRatio) - lower/goldenRatio;
	upperVal = function(upper);
	}

	while (lowerVal < midVal)
	{
	upper = mid;
	upperVal = midVal;
	mid = lower;
	midVal = lowerVal;
	lower = mid*(1.0 + 1.0/goldenRatio) - upper/goldenRatio;
	lowerVal = function(lower);
	}

	size_t count = 0;
	bool accurate = true;
	for (size_t i = 0; i < accuracy.size(); ++i)
	accurate &= std::abs(lower[i] - upper[i]) < accuracy[i];
	while (!accurate && count<maxIterations)
	{
	double lowerDiff0 = sci::abs(mid[firstNonZeroIndex] - lower[firstNonZeroIndex]);
	double upperDiff0 = sci::abs(upper[firstNonZeroIndex] - mid[firstNonZeroIndex]);

	std::vector<double> next;
	double nextVal;
	if (lowerDiff0 > upperDiff0)
	{
	next = mid*(1 - goldenRatio) + lower*goldenRatio;
	nextVal = function(next);

	if (nextVal > midVal)
	{
	std::swap(lower,next);
	lowerVal = nextVal;
	}
	else
	{
	std::swap(upper,mid);
	upperVal = midVal;
	mid = next;
	midVal = nextVal;
	}
	}
	else
	{
	next = mid*(1 - goldenRatio) + upper*goldenRatio;
	nextVal = function(next);
	if (nextVal > midVal)
	{
	std::swap(upper,next);
	upperVal = nextVal;
	}
	else
	{
	std::swap(lower,mid);
	lowerVal = midVal;
	std::swap(mid,next);
	midVal = nextVal;
	}
	}
	accurate = true;
	for (size_t i = 0; i < accuracy.size(); ++i)
	accurate &= std::abs(lower[i] - upper[i]) < accuracy[i];
	++count;
	}
	return mid;
	}*/

	template<class INPUT_TYPE, class RESULT_TYPE>
	class BrentStopper
	{
	public:
		using unitless_input_type = decltype(INPUT_TYPE() / INPUT_TYPE());
		using unitless_result_type = decltype(RESULT_TYPE() / RESULT_TYPE());
		BrentStopper(INPUT_TYPE absoluteTolerance = INPUT_TYPE(0), unitless_input_type relativeTolerance = std::numeric_limits<unitless_input_type>::epsilon(), INPUT_TYPE originOffset = INPUT_TYPE(0), RESULT_TYPE valueAbsoluteTolerance = RESULT_TYPE(0), unitless_result_type valueRelativeTolerance = std::numeric_limits<unitless_result_type>::epsilon(), size_t maxCount = -1)
		{
			m_absoluteTolerance = absoluteTolerance;
			m_relativeTolerance = relativeTolerance;
			m_originOffset = originOffset;
			m_valueAbsoluteTolerance = valueAbsoluteTolerance;
			m_valueRelativeTolerance = valueRelativeTolerance;
			m_maxCount = maxCount;
			m_count = 0;
		}
		bool operator()(INPUT_TYPE bestGuess, INPUT_TYPE previousBestGuess, RESULT_TYPE bestValue, RESULT_TYPE previousBestValue)
		{
			++m_count;
			if (m_count >= m_maxCount)
				return true;

			if (sci::abs(bestGuess - previousBestGuess) < m_absoluteTolerance)
				return true;

			if (sci::abs(bestValue - previousBestValue) < m_valueAbsoluteTolerance)
				return true;

			if (sci::abs(bestGuess - previousBestGuess) / sci::abs(bestGuess + m_originOffset) < m_relativeTolerance)
				return true;

			if (sci::abs(bestValue - previousBestValue) / sci::abs(bestValue) < m_valueRelativeTolerance)
				return true;

			return false;
		}
		void reset()
		{
			m_count = 0;
		}
	private:
		size_t m_count;
		INPUT_TYPE m_absoluteTolerance;
		INPUT_TYPE m_originOffset;
		unitless_input_type m_relativeTolerance;
		RESULT_TYPE m_valueAbsoluteTolerance;
		unitless_result_type m_valueRelativeTolerance;
		size_t m_maxCount;
	};

	template<class T, class U>
	T minOfQuadraticInterpolation(T a, T b, T c, U fa, U fb, U fc)
	{
		using unitless = decltype(T() / T());
		if (a > b)
		{
			std::swap(a, b);
			std::swap(fa, fb);
		}
		if (b > c)
		{
			std::swap(b, c);
			std::swap(fb, fc);
		}
		if (a > b)
		{
			std::swap(a, b);
			std::swap(fa, fb);
		}
		return b - unitless(0.5) * ((b - a) * (b - a) * (fb - fc) - (b - c) * (b - c) * (fb - fa)) / ((b - a) * (fb - fc) - (b - c) * (fb - fa));
	}

	//Find the minimum of a function using the "Brent" method
	//  function is the function to minimise. It may be a c-style function or a class with operator() defined. It must take one parameter, a double, and return the evaluation of the function as a double
	//  guess is a guess at the location of the minimum
	//  firstGuessUncertainty is used  to optimise finding a range within which a minima can be found.The algorithm will work if this is set badly, but it will be fasterand less likely to find a local minima if it is set correctly if it is set correctly
	//  tollerance is the fractional tollerance of the result. It defaults to sqrt(epsilon) and there is little point making it smaller than this.
	//  maxIterations is the maximum number of iterations to perform. It defaults to the maximum size_t
	//  The guess variable will be modified to the best estimate and the return value will be the residual
	template <class FUNCTOR, class STOPPER, class T, class U>
	auto minimiseBrent(const FUNCTOR& function, T &guess, U firstGuessUncertainty, STOPPER& stopper) -> decltype(function(guess))
	{
		using result_type = decltype(function(guess));
		using unitless = decltype(T() / T());
		assert(guess == guess);
		const unitless goldenRatio = unitless((3.0 - std::sqrt(5.0)) / 2.0);

		T lower;
		T upper;
		T best;
		result_type lowerVal;
		result_type upperVal;
		result_type bestVal;
		bracketMinimum(function, guess, firstGuessUncertainty, lower, upper, best, lowerVal, upperVal, bestVal);

		T secondBest;
		T previousSecondBest;
		result_type secondBestVal;
		result_type previousSecondBestVal;
		if(lowerVal < upperVal)
		{
			secondBest = lower;
			previousSecondBest = upper;
			secondBestVal = lowerVal;
			previousSecondBestVal = upperVal;
		}
		else
		{
			secondBest = upper;
			previousSecondBest = lower;
			secondBestVal = upperVal;
			previousSecondBestVal = lowerVal;
		}

		T previousStep = best-secondBest;
		T previousPreviousStep = secondBest-previousSecondBest;
		bool stop = stopper(best, secondBest, bestVal, secondBestVal);
		while (!stop)
		{
			T next;
			if (sci::abs(previousPreviousStep) < std::max(sci::abs(best * sci::sqrt(std::numeric_limits<unitless>::epsilon())), sci::abs(upper * sci::sqrt(std::numeric_limits<unitless>::epsilon()))))
			{
				//no point in even comparing this stap with previousPreviousStep as it is too small
				T lowerDiff = best - lower;
				T upperDiff = upper - best;
				next = (lowerDiff > upperDiff) ? best - (best - lower) * goldenRatio : best + (upper - best) * goldenRatio;
				assert(next == next);
			}
			else
			{
				T nextQuadratic = minOfQuadraticInterpolation(best, secondBest, previousSecondBest, bestVal, secondBestVal, previousSecondBestVal);
				//check if the change is less than half previousPreviousStep, if so use this step, otherwise use golden ratio
				if (sci::abs(best - nextQuadratic) < unitless(0.5) * sci::abs(previousPreviousStep) && nextQuadratic >= lower && nextQuadratic <= upper)
				{
					next = nextQuadratic;
					assert(next == next);
				}
				else
				{
					T lowerDiff = best - lower;
					T upperDiff = upper - best;
					next = (lowerDiff > upperDiff) ? best - (best - lower) * goldenRatio : best + (upper - best) * goldenRatio;
					assert(next == next);
				}
			}

			//set the previousSteps
			previousPreviousStep = previousStep;
			previousStep = next - best;

			//evaluate the function
			result_type nextVal = function(next);

			//Sort out which limit we are getting rid of an whether the new value is
			//a limit or the best (i.e the middle) 
			if (next < best)
			{
				if (nextVal > bestVal)
				{
					lower = next;
					lowerVal = nextVal;
					if (nextVal < secondBestVal)
					{
						previousSecondBest = secondBest;
						previousSecondBestVal = secondBestVal;
						secondBest = next;
						secondBestVal = nextVal;
					}
				}
				else
				{
					upper = best;
					upperVal = bestVal;
					previousSecondBest = secondBest;
					previousSecondBestVal = secondBestVal;
					secondBest = best;
					secondBestVal = bestVal;
					best = next;
					bestVal = nextVal;
				}
			}
			else
			{
				if (nextVal > bestVal)
				{
					upper = next;
					upperVal = nextVal;
					if (nextVal < secondBestVal)
					{
						previousSecondBest = secondBest;
						previousSecondBestVal = secondBestVal;
						secondBest = next;
						secondBestVal = nextVal;
					}
				}
				else
				{
					lower = best;
					lowerVal = bestVal;
					previousSecondBest = secondBest;
					previousSecondBestVal = secondBestVal;
					secondBest = best;
					secondBestVal = bestVal;
					best = next;
					bestVal = nextVal;
				}
			}
			stop = stopper(best, secondBest, bestVal, secondBestVal);
		}
		return bestVal;
	}

	//Find the minimum of a multi-d function along a given direction using the "Golden Ratio" method
	//  function is the function to minimise. It may be a c-style function or a class with operator() defined. It must take one parameter, a const std::vector<double>, and return the evaluation of the function at that point as a double
	//  guess is a guess at the location of the minimum
	//  tollerance is the fractional tollerance of the result in each dimension. It is futile making this less than sqrt(epsilon)
	//  maxIterations is the maximum number of iterations to perform. It defaults to the maximum size_t
	template <class FUNCTOR, IsGrid GRID1, IsGrid GRID2, IsGrid GRID3, IsGrid GRID4, IsGrid UNITLESS_GRID>
	auto  minimiseOnLineBrent(FUNCTOR& function, GRID1& guess, const GRID2& firstGuessUncertainty, const GRID3& direction, const GRID4& absoluteTollerance, const UNITLESS_GRID& tollerance, size_t maxIterations = -1)
	{
		using unitless = decltype(GRID1::value_type() / GRID1::value_type());
		using result_type = decltype(function(guess));
		using unitless_result_type = decltype(result_type() / result_type());

		bool nonZeroDirection = false;
		for( auto &dir : direction)
			nonZeroDirection |= dir != GRID3::value_type(0);
		if (!nonZeroDirection)
			return std::numeric_limits<result_type>::quiet_NaN();

		decltype(GRID3::value_type() * GRID3::value_type()) sumSqDirection(0);
		for (auto& dir : direction)
			sumSqDirection += dir * dir;
		typename GRID3::value_type magnitudeDirection = sci::sqrt(sumSqDirection);
		auto unitDirection = direction / sci::sqrt(sumSqDirection);


		//do dot product of absoluteTollerance, tollerance and firstGuessUncertainty with unitDirection
		//to get the magnitude in the search direction. Then scale by the magnitudeDirection to match the scaling
		//in the function passed to minimiseBrent
		typename GRID4::value_type absoluteTolleranceDirected(0);
		for (size_t i = 0; i < unitDirection.size(); ++i)
			absoluteTolleranceDirected += *(absoluteTollerance.begin() + i) * *(unitDirection.begin() + i);
		typename GRID1::value_type originOffset(0);
		for (size_t i = 0; i < unitDirection.size(); ++i)
			originOffset += *(guess.begin() + i) * *(unitDirection.begin() + i);
		typename UNITLESS_GRID::value_type tolleranceDirected(0);
		for (size_t i = 0; i < unitDirection.size(); ++i)
			tolleranceDirected += *(tollerance.begin() + i) * *(unitDirection.begin() + i);
		typename GRID2::value_type uncertaintyDirected(0);
		for (size_t i = 0; i < unitDirection.size(); ++i)
			uncertaintyDirected += *(firstGuessUncertainty.begin() + i) * *(unitDirection.begin() + i);

		BrentStopper<unitless, result_type> stopper(sci::abs(absoluteTolleranceDirected/magnitudeDirection), sci::abs(tolleranceDirected), originOffset/magnitudeDirection, result_type(0), std::numeric_limits<unitless_result_type>::epsilon(), maxIterations);

		unitless distance(0);
		result_type residual = minimiseBrent([&](unitless x) {return function(guess + direction * x); }, distance, uncertaintyDirected/magnitudeDirection, stopper);
		guess = guess + direction * distance;
		return residual;
	}

	//Find the minimum of a multi-d function along a given direction using the "Golden Ratio" method. In this version a default fractional tollerance of sqrt(epsilon) is used
	//  function is the function to minimise. It may be a c-style function or a class with operator() defined. It must take one parameter, a const std::vector<double>, and return the evaluation of the function at that point as a double
	//  guess is a guess at the location of the minimum
	//  tollerance is the fractional tollerance of the result in each dimension. It defaults to sqrt(epsilon) and there is little point making it smaller than this.
	//  maxIterations is the maximum number of iterations to perform. It defaults to the maximum size_t

	template <class FUNCTOR, IsGrid GRID1, IsGrid GRID2, IsGrid GRID3>
	void minimiseOnLineBrent(FUNCTOR& function, GRID1& guess, const GRID2& firstGuessUncertainty, const GRID3& direction, size_t maxIterations = -1)
	{
		using unitless = decltype(GRID1::value_type() / GRID1::value_type());
		using result_type = decltype(function(guess));
		using unitless_result_type = decltype(result_type() / result_type());

		bool nonZeroDirection = false;
		for (size_t i = 0; i < direction.size(); ++i)
			nonZeroDirection |= direction[i] != GRID1::value_type(0);
		if (!nonZeroDirection)
			return;

		decltype(GRID3::value_type() * GRID3::value_type()) sumSqDirection(0);
		for (auto& dir : direction)
			sumSqDirection += dir * dir;
		typename GRID3::value_type magnitudeDirection = sci::sqrt(sumSqDirection);
		auto unitDirection = direction / sci::sqrt(sumSqDirection);

		BrentStopper<unitless, result_type> stopper(unitless(0), std::numeric_limits<unitless>::epsilon(), unitless(0), result_type(0), std::numeric_limits<unitless_result_type>::epsilon(), maxIterations);

		typename GRID2::value_type uncertaintyDirected(0);
		for (size_t i = 0; i < unitDirection.size(); ++i)
			uncertaintyDirected += *(firstGuessUncertainty.begin() + i) * *(unitDirection.begin() + i);

		unitless distance(0);
		unitless residual = minimiseBrent([&](unitless x) {return function(guess + direction * x); }, distance, uncertaintyDirected/magnitudeDirection, stopper);
		guess = guess + direction * distance;
		return residual;
	}

	//Find the minimum of a multi-d function using the Powel method
	//Each "full step" of this minimisation does N line minimisations in different directions, where N is the number of dimensions.
	//
	//  function is the function to minimise. It may be a c-style function or a class with operator() defined (i.e. a functor). It must take one parameter, a std::vector<double> or a const std::vector<double>&, and return the evaluation of the function as a double
	//  stopper is a functor that decides whether the routine has converged, it must have one parameter which will be the most recent guess as a std::vector<double> or a const std::reference<double>. It must return true if you wish to stop or false if you wish to continue
	//  guess is a guess at the location of the minimum
	//  tollerance is the fractional tollerance of the result in each dimension that will be used for each line minimisation. 
	//  maxIterations is the maximum number of iterations to perform. It defaults to the maximum size_t
	template <class FUNCTOR, class STOPPER, IsGrid GRID1, IsGrid GRID2, IsGrid GRID3, IsGrid UNITLESS_GRID>
	/*requires (std::is_convertible_v< typename GRID1::value_type, typename GRID2::value_type()>
		&& std::is_convertible_v< typename GRID1::value_type, typename GRID3::value_type()>
		&& std::is_convertible_v< decltype(typename GRID1::value_type() / typename GRID1::value_type()), typename UNITLESS_GRID::value_type()>)*/
	void minimisePowell(FUNCTOR& function, STOPPER& stopper, GRID1& guess, const GRID2& firstGuessUncertainty, const GRID3& absoluteTollerance, const UNITLESS_GRID& tollerance, size_t maxIterations = -1, size_t maxBrentIterations = -1)
	{
		//directions is a set of direction in space along which we will search for the solution
		// we initially set the directions to the orthoganal directions with magnitudes equal to firstGuessUncertainty
		// directions will have one more dimension than guess. Usually guess will be 1d, but we permit it to be higher dimensions
		// for ease of use in some scenarios
		assert(guess.size() == firstGuessUncertainty.size());
		std::array<size_t, GRID2::ndims + 1> directionsShape;
		directionsShape[0] = guess.size();
		for (size_t i = 0; i < GRID2::ndims; ++i)
			directionsShape[i + 1] = firstGuessUncertainty.shape()[i];
		sci::GridData< GRID2::value_type, GRID2::ndims+1> directions(directionsShape, GRID2::value_type(0));
		for (size_t i = 0; i < guess.size(); ++i)
		{
			*(directions.begin()+i*(guess.size()+1)) = *(firstGuessUncertainty.begin() + i); //this slightly complicated assignment accounts for the GRIDS being multi dimensional
		}

		bool stop = stopper(guess, function(guess));
		size_t count = 0;
		sci::GridData<GRID1::value_type, GRID1::ndims> iterStartGuess;
		sci::GridData<GRID1::value_type, GRID1::ndims> subIterStartGuess;
		while ((!stop) && (count < maxIterations))
		{
			iterStartGuess = guess;
			//The following will exactly solve a quadratic form
			for (size_t j = 0; j < directionsShape[0]; ++j)
			{
				//record the start guess for the sub iteration
				subIterStartGuess = guess;
				//minimise in each of the directions
				for (size_t i = 0; i < directions.shape()[0]; ++i)
				{
					minimiseOnLineBrent(function, guess, firstGuessUncertainty, directions[i], absoluteTollerance, tollerance, maxBrentIterations);
				}
				//discard the first direction
				for (size_t i = 0; i < directions.shape()[0] - 1; ++i)
				{
					directions[i].assign( directions[i + 1] );
				}
				//append the total move of this iteration as a direction for the next iteration - this is known as a conjugate direction
				directions[directions.shape()[0] - 1].assign (guess - subIterStartGuess);
				//perform a minimisation in the direction of the just found conjugate direction
				auto residual = minimiseOnLineBrent(function, guess, firstGuessUncertainty, directions[directions.shape()[0] - 1], absoluteTollerance, tollerance, maxBrentIterations);
				stop = stopper(guess, residual);
				if (stop)
					break;
			}

			//now we have performed a full step, check if we need to stop
			stop = stopper(guess, function(guess));


			//we must reset the directions otherwise they tend to fold up on each other
			for (auto& dir : directions)
				dir = GRID2::value_type(0);
			for (size_t i = 0; i < guess.size(); ++i)
			{
				*(directions.begin() + i * (guess.size() + 1)) = *(firstGuessUncertainty.begin() + i); //this slightly complicated assignment accounts for the GRIDS being multi dimensional
			}

			++count;
		}
	}



	//Find the minimum of a multi-d function using the Powel method
	//  function is the function to minimise. It may be a c-style function or a class with operator() defined. It must take one parameter, a double, and return the evaluation of the function as a double
	//  guess is a guess at the location of the minimum
	//  firstGuessUncertainty is used  to optimise finding a range within which a minima can be found. The algorithm will work if this is set badly, but it will be faster and less likely to find a local minima if it is set correctly if it is set correctly
	//  tollerance is the fractional tollerance of the result in each dimension. 
	//  maxIterations is the maximum number of iterations to perform. It defaults to the maximum size_t
	template <class FUNCTOR, class STOPPER, IsGrid GRID1, IsGrid GRID2>
	//requires std::is_convertible_v< typename GRID1::value_type, typename GRID2::value_type()>
	void minimisePowell(FUNCTOR& function, STOPPER& stopper, GRID1 &guess, const GRID2& firstGuessUncertainty, size_t maxIterations = -1, size_t maxBrentIterations = -1)
	{
		using unitless = decltype(GRID1::value_type() / GRID1::value_type());
		GridData<unitless, GRID1::ndims> tollerance(guess.shape(), sci::sqrt(std::numeric_limits<unitless>::epsilon()));
		GridData<GRID1::value_type, GRID1::ndims> absoluteTollerance(guess.shape(), GRID1::value_type(0));
		minimisePowell(function, stopper, guess, firstGuessUncertainty, absoluteTollerance, tollerance, maxIterations, maxBrentIterations);
	}

	template <class FUNCTOR, class STOPPER, IsGrid GRID1, IsGrid GRID3, IsGrid UNITLESS_GRID>
	void minimiseGradientDescentWithLineSearch(FUNCTOR& function, STOPPER& stopper, GRID1& guess, const GRID3& absoluteTollerance, const UNITLESS_GRID& tollerance, size_t maxIterations = -1, size_t maxBrentIterations = -1)
	{
		using gradient_type = decltype(function(guess) / GRID1::value_type());
		//directions is a set of direction in space along which we will search for the solution
		// we initially set the directions to the orthoganal directions with magnitudes equal to firstGuessUncertainty
		// directions will have one more dimension than guess. Usually guess will be 1d, but we permit it to be higher dimensions
		// for ease of use in some scenarios
		assert(guess.size() == absoluteTollerance.size());

		bool stop = stopper(guess, function(guess));
		size_t count = 0;
		while ((!stop) && (count < maxIterations))
		{
			for (size_t i = 0; i < guess.size(); ++i)
			{
				std::pair<sci::GridData< GRID1::value_type, GRID1::ndims>, sci::GridData< GRID1::value_type, GRID1::ndims>> directionsAndUncertainties = function.gradients(guess);
				auto residual = minimiseOnLineBrent(function, guess, directionsAndUncertainties.second, directionsAndUncertainties.first, absoluteTollerance, tollerance, maxBrentIterations);
				stop = stopper(guess, residual);
				if (stop)
					break;
			}
			++count;
		}
	}
	template <class FUNCTOR, class STOPPER, IsGrid GRID1>
	void minimiseGradientDescentWithLineSearch(FUNCTOR& function, STOPPER& stopper, GRID1& guess, size_t maxIterations = -1, size_t maxBrentIterations = -1)
	{
		using unitless = decltype(GRID1::value_type() / GRID1::value_type());
		GridData<unitless, GRID1::ndims> tollerance(guess.shape(), sci::sqrt(std::numeric_limits<unitless>::epsilon()));
		GridData<GRID1::value_type, GRID1::ndims> absoluteTollerance(guess.shape(), GRID1::value_type(0));
		minimiseGradientDescentWithLineSearch(function, stopper, guess, absoluteTollerance, tollerance, maxIterations, maxBrentIterations);
	}
}