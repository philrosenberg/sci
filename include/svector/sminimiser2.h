#pragma once

#include<svector\svector.h>
#include<vector>
#include<assert.h>

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

//find two points that bracket one or more minima of a function
//  function is the function to minimise. It may be a c-style function or a class with operator() defined. It must take one parameter, a double, and return the evaluation of the function as a double
//  firstGuess is the guess for the location of the minima
// lowerBracket and upperBracket are the outputs. The lowerBracket value will always be less than upperBracket.
template <class FUNCTOR>
void bracketMinimum(FUNCTOR &function, double firstGuess, double firstGuessUncertainty, double &lowerBracket, double &upperBracket)
{
	const double goldenRatio = (3.0 - std::sqrt(5.0)) / 2.0;

	//Find a non insignificant distance from the first guess
	firstGuessUncertainty = std::abs(firstGuessUncertainty);
	double difference = firstGuessUncertainty !=0 ? firstGuessUncertainty*6 : 1.0;
	if (difference < std::abs(firstGuess) * std::numeric_limits<double>::epsilon() * 6)
		difference = std::abs(firstGuess) * std::numeric_limits<double>::epsilon() * 6;
	
	lowerBracket = firstGuess - goldenRatio*difference;
	upperBracket = lowerBracket + difference;
	double lowerValue = function(lowerBracket);
	double upperValue = function(upperBracket);
	//check to make sure we have a significant difference in the values we are comparing
	while (std::abs(lowerValue - upperValue) < (std::abs(lowerValue) + std::abs(upperValue)) * std::sqrt(std::numeric_limits<double>::epsilon()))
	{
		if (upperBracket == std::numeric_limits<double>::infinity())
		{
			//if we have reached "infinity" then set the uppervalue bigger than lowervalue to trigger looking the other direction
			upperValue = lowerValue + 1.0;
			break;
		}
		difference *= 2.0;
		lowerBracket = firstGuess - goldenRatio*difference;
		upperBracket = lowerBracket + difference;
		lowerValue = function(lowerBracket);
		upperValue = function(upperBracket);
	}

	//if the upperValue is bigger than lowerVaue, then we are searching the wrong way
	if (upperValue > lowerValue)
	{
		//check the other direction
		upperBracket = firstGuess - difference;
		upperValue = function(upperBracket);
		//again check to make sure we have a significant difference in the values we are comparing
		while (std::abs(lowerValue - upperValue) < (std::abs(lowerValue) + std::abs(upperValue)) * std::sqrt(std::numeric_limits<double>::epsilon()))
		{
			if (upperBracket == std::numeric_limits<double>::infinity())
			{
				//There is no minimum or alternatively everywhere is the minimum.
				//return upper and lower brackets identical which will cause the minimisation routines to do nothing
				upperBracket = lowerBracket;
				return;
			}
			difference *= 2.0;
			upperBracket = firstGuess - difference;
			upperValue = function(upperBracket);
		}
		//if this is also bigger then we have already bracketed a minimum
		if (upperValue > lowerValue)
		{
			lowerBracket = firstGuess - difference;
			upperBracket = firstGuess + difference;
			return;
		}
		//referse the direction we search
		difference *= -1.0;
	}

	//while we keep descending keep going, once we start ascending again we have bracketed the minimum
	while (upperValue < lowerValue)
	{
		//go in ever increasing steps
		difference *= 2.0;
		lowerValue = upperValue;
		lowerBracket = upperBracket;
		upperBracket += difference;
		upperValue = function(upperBracket);
	}
	//we may have passed the minimum, so expand backwards one step
	lowerBracket -= difference / 2.0;

	// if we were searching in the negative direction then our upper and lower brackets
	//will be the wrong way round so swap them
	if (upperBracket < lowerBracket)
		std::swap(upperBracket, lowerBracket);
}

template <class FUNCTOR>
void bracketMinimum(FUNCTOR &function, double &lowerBracket, double &upperBracket)
{
	bracketMinimum(function, 0, lowerBracket, upperBracket);
}

//Find the minimum of a function using the "Golden Ratio" method
//  function is the function to minimise. It may be a c-style function or a class with operator() defined. It must take one parameter, a double, and return the evaluation of the function as a double
//  firstGuess is a guess at the location of the minimum
//  tollerance is the fractional tollerance of the result. It defaults to sqrt(epsilon) and there is little point making it smaller than this.
//  maxIterations is the maximum number of iterations to perform. It defaults to the maximum size_t
template <class FUNCTOR>
double minimiseGoldenSection(FUNCTOR &function, double firstGuess, double absoluteTollerance = 0.0, double tollerance = std::sqrt(std::numeric_limits<double>::epsilon()), size_t maxIterations = -1)
{
	const double goldenRatio = (3.0 - std::sqrt(5.0)) / 2.0;

	double lower;
	double upper;
	bracketMinimum(function, firstGuess, lower, upper);
	double mid = lower + (upper - lower)*goldenRatio;

	double midVal = function(mid);
	double lowerVal = function(lower);
	double upperVal = function(upper);

	size_t count = 0;
	bool accurate = (upper - lower) < (mid + (upper + lower) / 2.0)*tollerance; //we combine mid with average of upper and lower for cases where mid is very close to zero - note we sum as we have the full max to min span
	accurate |= (upper - lower) < absoluteTollerance;
	while (!accurate && count<maxIterations)
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
			next = mid + (upper - mid)*goldenRatio;
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
		accurate = (upper - lower) < (mid + (upper + lower) / 2.0)*tollerance; //we combine mid with average of upper and lower for cases where mid is very close to zero - note we sum as we have the full max to min span
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
std::vector<double> minimiseOnLineGoldenSection(FUNCTOR &function, const std::vector<double> &firstGuess, const std::vector<double> &direction, const std::vector<double> &absoluteTollerance, const std::vector<double> &tollerance, size_t maxIterations = -1)
{
	double distance = minimiseGoldenSection([=](double x) mutable {return function(firstGuess + direction*x);}, 1.0, sci::abs(sci::dot(direction, absoluteTollerance)), sci::abs(sci::dot(direction, tollerance)), maxIterations);
	return firstGuess + direction * distance;
}

//Find the minimum of a multi-d function along a given direction using the "Golden Ratio" method. In this version a default fractional tollerance of sqrt(epsilon) is used
//  function is the function to minimise. It may be a c-style function or a class with operator() defined. It must take one parameter, a const std::vector<double>, and return the evaluation of the function at that point as a double
//  firstGuess is a guess at the location of the minimum
//  tollerance is the fractional tollerance of the result in each dimension. It defaults to sqrt(epsilon) and there is little point making it smaller than this.
//  maxIterations is the maximum number of iterations to perform. It defaults to the maximum size_t

template <class FUNCTOR>
std::vector<double> minimiseOnLineGoldenSection(FUNCTOR &function, const std::vector<double> &firstGuess, const std::vector<double> &direction, size_t maxIterations = -1)
{
	double distance = minimiseGoldenSection([=](double x) mutable {return function(firstGuess + direction*x);}, 1.0, 0.0, std::sqrt(std::numeric_limits<double>::epsilon()), maxIterations);
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

class BrentStopper
{
public:
	BrentStopper(double absoluteTolerance = 0.0, double relativeTolerance = std::numeric_limits<double>::epsilon(), double valueAbsoluteTolerance = 0.0, double valueRelativeTolerance = std::numeric_limits<double>::epsilon(), size_t maxCount = -1)
	{
		m_absoluteTolerance = absoluteTolerance;
		m_relativeTolerance = relativeTolerance;
		m_valueAbsoluteTolerance = valueAbsoluteTolerance;
		m_valueRelativeTolerance = valueRelativeTolerance;
		m_maxCount = maxCount;
		m_count = 0;
	}
	bool operator()(double bestGuess, double lowerGuess, double upperGuess, double bestValue, double lowerValue, double upperValue)
	{
		++m_count;
		if (m_count > m_maxCount)
			return true;

		if (upperGuess - lowerGuess < m_absoluteTolerance)
			return true;
		if ((upperGuess - lowerGuess) / std::abs(bestGuess) < m_relativeTolerance)
			return true;

		double valueChange = std::max(lowerValue - bestValue, upperValue - bestValue);
		if (valueChange < m_valueAbsoluteTolerance)
			return true;
		if (valueChange / std::abs(bestValue) < m_valueRelativeTolerance)
			return true;

		return false;
	}
	void reset()
	{
		m_count = 0;
	}
private:
	size_t m_count;
	double m_absoluteTolerance;
	double m_relativeTolerance;
	double m_valueAbsoluteTolerance;
	double m_valueRelativeTolerance;
	size_t m_maxCount;
};

class AlongLineBrentStopper
{
public:
	AlongLineBrentStopper(const std::vector<double> &start, const std::vector<double> &direction, double absoluteTolerance = 0.0, double relativeTolerance = std::numeric_limits<double>::epsilon(), double valueAbsoluteTolerance = 0.0, double valueRelativeTolerance = std::numeric_limits<double>::epsilon(), size_t maxCount = -1)
	{
		m_absoluteTolerance = absoluteTolerance;
		m_relativeTolerance = relativeTolerance;
		m_valueAbsoluteTolerance = valueAbsoluteTolerance;
		m_valueRelativeTolerance = valueRelativeTolerance;
		m_maxCount = maxCount;
		m_start = start;
		m_direction = direction;
		m_count = 0;
	}
	bool operator()(double bestGuess, double lowerGuess, double upperGuess, double bestValue, double lowerValue, double upperValue)
	{
		++m_count;
		if (m_count > m_maxCount)
			return true;

		if (upperGuess - lowerGuess < m_absoluteTolerance)
			return true;
		std::vector<double> lowerVect = m_start + m_direction*lowerGuess;
		std::vector<double> upperVect = m_start + m_direction*upperGuess;
		std::vector<double> bestVect = m_start + m_direction*bestGuess;
		bool withinRelativeTolerance = true;
		for (size_t i = 0; i < lowerVect.size(); ++i)
			withinRelativeTolerance &= std::abs(upperVect[i] - lowerVect[i]) / std::abs(bestVect[i]) < m_relativeTolerance;
		if(withinRelativeTolerance)
			return true;

		double valueChange = std::max(lowerValue - bestValue, upperValue - bestValue);
		if (valueChange < m_valueAbsoluteTolerance)
			return true;
		if (valueChange / std::abs(bestValue) < m_valueRelativeTolerance)
			return true;

		return false;
	}
	void reset()
	{
		m_count = 0;
	}
private:
	size_t m_count;
	double m_absoluteTolerance;
	double m_relativeTolerance;
	double m_valueAbsoluteTolerance;
	double m_valueRelativeTolerance;
	size_t m_maxCount;
	std::vector<double> m_start;
	std::vector<double> m_direction;
};

double minOfQuadraticInterpolation(double a, double b, double c, double fa, double fb, double fc)
{
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
	return b - 0.5*((b - a)*(b - a)*(fb - fc) - (b - c)*(b - c)*(fb - fa)) / ((b - a)*(fb - fc) - (b - c)*(fb - fa));
}

//Find the minimum of a function using the "Golden Ratio" method
//  function is the function to minimise. It may be a c-style function or a class with operator() defined. It must take one parameter, a double, and return the evaluation of the function as a double
//  firstGuess is a guess at the location of the minimum
//  tollerance is the fractional tollerance of the result. It defaults to sqrt(epsilon) and there is little point making it smaller than this.
//  maxIterations is the maximum number of iterations to perform. It defaults to the maximum size_t
template <class FUNCTOR, class STOPPER>
double minimiseBrent(FUNCTOR &function, double firstGuess, double firstGuessUncertainty, STOPPER &stopper)
{
	assert(firstGuess == firstGuess);
	const double goldenRatio = (3.0 - std::sqrt(5.0)) / 2.0;

	double lower;
	double upper;
	bracketMinimum(function, firstGuess, firstGuessUncertainty, lower, upper);

	double lowerVal = function(lower);
	double upperVal = function(upper);

	double best = lower+(upper-lower)*goldenRatio;
	double bestVal = function(best);
	double secondBest = lowerVal<upperVal ? lower : upper;
	double secondBestVal = lowerVal<upperVal ? lowerVal : upperVal;
	double previousSecondBest = best;
	double previousSecondBestVal = bestVal;

	double previousStep = 0.0;
	double previousPreviousStep = 0.0;
	bool usedQuadratic = false;
	bool prevUsedQuadratic = false;
	bool stop = stopper(best, lower, upper, bestVal, lowerVal, upperVal);
	while (!stop)
	{
		prevUsedQuadratic = usedQuadratic;
		usedQuadratic = false;
		double next;
		if (std::abs(previousPreviousStep) < std::max(abs(best*std::sqrt(std::numeric_limits<double>::epsilon())), std::abs(upper*std::sqrt(std::numeric_limits<double>::epsilon()))))
		{
			//no point in even comparing this stap with previousPreviousStep as it is too small
			double lowerDiff = best - lower;
			double upperDiff = upper - best;
			next = (lowerDiff > upperDiff) ? best - (best - lower) * goldenRatio : best + (upper - best)*goldenRatio;
			assert(next == next);
		}
		else
		{
			double nextQuadratic = minOfQuadraticInterpolation(best, secondBest, previousSecondBest, bestVal, secondBestVal, previousSecondBestVal);
			//check if the change is less than half previousPreviousStep, if so use this step, otherwise use golden ratio
			if (abs(best - nextQuadratic) < 0.5*std::abs(previousPreviousStep) && nextQuadratic > lower && nextQuadratic < upper)
			{
				next = nextQuadratic;
				usedQuadratic = true;
				assert(next == next);
			}
			else
			{
				double lowerDiff = best - lower;
				double upperDiff = upper - best;
				next = (lowerDiff > upperDiff) ? best - (best - lower) * goldenRatio : best + (upper - best)*goldenRatio;
				assert(next == next);
			}
		}

		//set the previousSteps
		previousPreviousStep = previousStep;
		previousStep = next - best;
		//if (usedQuadratic &&prevUsedQuadratic && abs(next - best) < std::max(std::max(std::abs(best*tollerance), std::abs(next*tollerance)), absoluteTollerance))
		//{
			//we have converged, return
		//	return best;
		//}

		//evaluate the function
		double nextVal = function(next);

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
		stop = stopper(best, lower, upper, bestVal, lowerVal, upperVal);
	}
	return best;
}


//Find the minimum of a multi-d function along a given direction using the "Golden Ratio" method
//  function is the function to minimise. It may be a c-style function or a class with operator() defined. It must take one parameter, a const std::vector<double>, and return the evaluation of the function at that point as a double
//  firstGuess is a guess at the location of the minimum
//  tollerance is the fractional tollerance of the result in each dimension. It is futile making this less than sqrt(epsilon)
//  maxIterations is the maximum number of iterations to perform. It defaults to the maximum size_t
template <class FUNCTOR>
std::vector<double> minimiseOnLineBrent(FUNCTOR &function, const std::vector<double> &firstGuess, const std::vector<double> &firstGuessUncertainty, const std::vector<double> &direction, const std::vector<double> &absoluteTollerance, const std::vector<double> &tollerance, size_t maxIterations = -1)
{
	bool nonZeroDirection = false;
	for (size_t i = 0; i < direction.size(); ++i)
		nonZeroDirection |= direction[i] != 0.0;
	if (!nonZeroDirection)
		return firstGuess;

	std::vector<double> unitDirection = direction / std::sqrt(sci::sum(direction*direction));

	AlongLineBrentStopper stopper(firstGuess, unitDirection, sci::abs(sci::dot(unitDirection, absoluteTollerance)), sci::abs(sci::dot(unitDirection, tollerance)), 0.0, std::numeric_limits<double>::epsilon(), maxIterations);

	double distance = minimiseBrent([&](double x) mutable {return function(firstGuess + unitDirection*x);}, 0.0, sci::dot(unitDirection, firstGuessUncertainty), stopper);
	return firstGuess + unitDirection * distance;
}

//Find the minimum of a multi-d function along a given direction using the "Golden Ratio" method. In this version a default fractional tollerance of sqrt(epsilon) is used
//  function is the function to minimise. It may be a c-style function or a class with operator() defined. It must take one parameter, a const std::vector<double>, and return the evaluation of the function at that point as a double
//  firstGuess is a guess at the location of the minimum
//  tollerance is the fractional tollerance of the result in each dimension. It defaults to sqrt(epsilon) and there is little point making it smaller than this.
//  maxIterations is the maximum number of iterations to perform. It defaults to the maximum size_t

template <class FUNCTOR>
std::vector<double> minimiseOnLineBrent(FUNCTOR &function, const std::vector<double> &firstGuess, const std::vector<double> &firstGuessUncertainty, const std::vector<double> &direction, size_t maxIterations = -1)
{
	bool nonZeroDirection = false;
	for (size_t i = 0; i < direction.size(); ++i)
		nonZeroDirection |= direction[i] != 0.0;
	if (!nonZeroDirection)
		return firstGuess;

	AlongLineBrentStopper stopper(firstGuess, unitDirection, 0.0, std::numeric_limits<double>::epsilon(), 0.0, std::numeric_limits<double>::epsilon(), maxIterations);

	std::vector<double> unitDirection = direction / std::sqrt(sci::sum(direction*direction));
	double distance = minimiseBrent([&](double x) mutable {return function(firstGuess + unitDirection*x);}, 0.0, sci::dot(unitDirection, firstGuessUncertainty), stopper);
	return firstGuess + unitDirection * distance;
}

//Find the minimum of a multi-d function using the Powel method
//Each "full step" of this minimisation does N line minimisations in different directions, where N is the number of dimensions.
//
//  function is the function to minimise. It may be a c-style function or a class with operator() defined (i.e. a functor). It must take one parameter, a std::vector<double> or a const std::vector<double>&, and return the evaluation of the function as a double
//  stopper is a functor that decides whether the routine has converged, it must have one parameter which will be the most recent guess as a std::vector<double> or a const std::reference<double>. It must return true if you wish to stop or false if you wish to continue
//  firstGuess is a guess at the location of the minimum
//  tollerance is the fractional tollerance of the result in each dimension that will be used for each line minimisation. 
//  maxIterations is the maximum number of iterations to perform. It defaults to the maximum size_t
template <class FUNCTOR, class STOPPER>
std::vector<double> minimisePowel(FUNCTOR &function, STOPPER &stopper, std::vector<double> firstGuess, std::vector<double> firstGuessUncertainty, std::vector<double> absoluteTollerance, std::vector<double> tollerance, size_t maxIterations = -1)
{
	std::vector<std::vector<double>> directions(firstGuess.size());
	for (size_t i = 0; i < directions.size(); ++i)
	{
		directions[i].resize(directions.size());
		directions[i][i] = 1.0;
	}
	std::vector<std::vector<double>> points(directions.size() + 1);
	points[0] = firstGuess;

	std::vector<double> step(points.size(), std::numeric_limits<double>::infinity());
	std::vector<double> previous;
	bool stop = stopper(points[0], function(points[0]));
	size_t count = 0;
	while ((!stop) && (count < maxIterations))
	{
		previous = points[0];
		//The following will exactly solve a quadratic form
		for (size_t j = 0; j < directions.size(); ++j)
		{
			for (size_t i = 0; i < directions.size(); ++i)
			{
				points[i + 1] = minimiseOnLineBrent(function, points[i], firstGuessUncertainty, directions[i], absoluteTollerance, tollerance);
			}
			for (size_t i = 0; i < directions.size() - 1; ++i)
			{
				directions[i] = directions[i + 1];
			}
			directions.back() = points.back() - points[0];
			points[0] = minimiseOnLineBrent(function, points.back(), firstGuessUncertainty, directions.back(), absoluteTollerance, tollerance);
		}

		step = sci::abs(points[0] - previous);
		stop = stopper(points[0], function(points[0]));

		//we must reset the directions otherwise they tend to fold up of each other
		for (size_t i = 0; i < directions.size(); ++i)
		{
			directions[i].assign(directions.size(), 0.0);
			directions[i][i] = 1.0;
		}

		++count;
	}
	return points[0];
}



//Find the minimum of a multi-d function using the Powel method
//  function is the function to minimise. It may be a c-style function or a class with operator() defined. It must take one parameter, a double, and return the evaluation of the function as a double
//  firstGuess is a guess at the location of the minimum
//  tollerance is the fractional tollerance of the result in each dimension. 
//  maxIterations is the maximum number of iterations to perform. It defaults to the maximum size_t
template <class FUNCTOR, class STOPPER>
std::vector<double> minimisePowel(FUNCTOR &function, STOPPER &stopper, std::vector<double> firstGuess, size_t maxIterations = -1)
{
	std::vector<double> tollerance(firstGuess.size(), std::sqrt(std::numeric_limits<double>::epsilon()));
	return minimisePowel(function, stopper, firstGuess, tollerance, maxIterations);
}