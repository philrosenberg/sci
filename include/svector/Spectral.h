#pragma once

#include"Ranges.h"
#include"serr.h"
#include"Statistics.h"
#include<cmath>
#include<algorithm>

namespace sci
{
	
	//resample at a differeent frequency
	//input is the original vector
	//factor is the new frequency divided by the old frequency
	//output will be filled with the resampled data. Note that output can have a size from 
	//floor((inputsize-1)*factor) to ceil((inputsize+1)*factor)
	//output must have at least ceil((inputsize+1)*factor) elements allocated before being passed to this function
	//outputSize will be set to the actual number of filled elements of output on return
	void resample(const double* input, size_t n, double factor, double* output, size_t &outputSize);

	//resample at a differeent frequency
	//input is the original vector
	//factor is the new frequency divided by the old frequency
	//output will be filled with the resampled data. Note that output can have a size from 
	//floor((inputsize-1)*factor) to ceil((inputsize+1)*factor).
	//output must have at least ceil((inputsize+1)*factor) elements allocated before being passed to this function
	//outputSize will be set to the actual number of filled elements of output on return
	void resample(const float* input, size_t n, float factor, float* output, size_t &outputSize);

	//resample at a differeent frequency
	//input is the original vector
	//factor is the new frequency divided by the old frequency
	//output will be filled with the resampled data. Note that output can have a size from 
	//floor((inputsize-1)*factor) to ceil((inputsize+1)*factor)
	template< sci::ranges::vector_range T, sci::ranges::vector_range_resizable U>
	void resample(const T& input, double factor, U& output)
	{
		size_t inputSize = std::ranges::size(input);
		size_t maxSize = (size_t)std::ceil((inputSize + 1) * factor);
		size_t actualSize = 0;
		output.resize(maxSize);
		try
		{
			resample(std::ranges::cdata(input), inputSize, factor, std::ranges::data(output), actualSize);
			output.resize(actualSize);
		}
		catch (...)
		{
			output.resize(0);
			throw;
		}
	}
	//resample at a differeent frequency
	//input is the original vector
	//factor is the new frequency divided by the old frequency
	//output will be filled with the resampled data. Note that output can have a size from 
	//floor((inputsize-1)*factor) to ceil((inputsize+1)*factor)
	template< sci::ranges::vector_range_f T, sci::ranges::vector_range_resizable_f U>
	void resample(const T& input, double factor, U& output)
	{
		size_t inputSize = std::ranges::size(input);
		size_t maxSize = (size_t)std::ceil((inputSize + 1) * factor);
		size_t actualSize = 0;
		output.resize(maxSize);
		try
		{
			resample(std::ranges::cdata(input), inputSize, factor, std::ranges::data(output), actualSize);
			output.resize(actualSize);
		}
		catch (...)
		{
			output.resize(0);
			throw;
		}
	}

	//fft
	//raw pointer C-style version
	//re_input is the real input, it must have an even number of points
	//n is the number of elements in re_input
	//re_output is the real output - n/2+1 elements of memory must be allocated by the user before the function is called
	//im_output is the imaginary output - n/2+1 elements of memory must be allocated by the user before the function is called
	//if normalise for number of points is true then amplitudes will be normalised to correct absolute values.
	//if performNegativeFrequencyAmplitudedoubling is true then the amplitude of elements other than each end are doubled, to include the amplitude of negative frequencies
	void fft(const double* re_input, size_t n, double* re_output, double* im_output, bool normaliseForNumberOfPoints, bool performNegativeFrequencyAmplitudedoubling);

	//ifft
	//raw pointer C-style version
	//re_input is the real input
	//im_input is the imaginary input
	//n is the number of elements in both re_input and im_input
	//re_output is the real output - (n-1)*2 elements of memory must be allocated by the user before the function is called
	//set performNegativeFrequencyAmplitudeHalfing if the amplitudes being input were doubled to include the amplitude of negative frequencies
	//there is no normalisation for number of points option, as the data are always normalised at no extra computational cost.
	//so to perform an fft and invert it to restore the initial input do:
	//	fft(signal, n, real_amplitudes, imaginary_amplitudes, true, true);
	//	ifft(real_amplitudes, real_amplitudes, n/2+1, signal, true);
	void ifft(const double* re_input, const double* im_input, size_t n, double* re_output, bool performNegativeFrequencyAmplitudeHalfing = true);

	//fft
	//C++ ranges style version
	//re_input is the real input, it must have an even number of points
	//n is the number of elements in re_input
	//re_output is the real output - n/2+1 elements of memory must be allocated by the user before the function is called
	//im_output is the imaginary output - n/2+1 elements of memory must be allocated by the user before the function is called
	//if normalise for number of points is true then amplitudes will be normalised to correct absolute values.
	//if performNegativeFrequencyAmplitudedoubling is true then the amplitude of elements other than each end are doubled, to include the amplitude of negative frequencies
	template<sci::ranges::vector_range T, sci::ranges::vector_range_resizable U, sci::ranges::vector_range_resizable V>
	void fft(const  T& re_input, U& re_output, V& im_output, bool normaliseForNumberOfPoints = true, bool performNegativeFrequencyAmplitudedoubling = true)
	{
		size_t inputSize = std::ranges::size(re_input);
		re_output.resize(inputSize / 2 + 1);
		im_output.resize(inputSize / 2 + 1);
		fft(std::ranges::cdata(re_input), re_input.size(), std::ranges::data(re_output), std::ranges::data(im_output), normaliseForNumberOfPoints, performNegativeFrequencyAmplitudedoubling);
	}

	//ifft
	//C++ ranges style version
	//re_input is the real input
	//im_input is the imaginary input
	//re_output is the real output - it will be resized to have (n-1)*2
	//set performNegativeFrequencyAmplitudeHalfing if the amplitudes being input were doubled to include the amplitude of negative frequencies
	//there is no normalisation for number of points option, as the data are always normalised at no extra computational cost.
	//so to perform an fft and invert it to restore the initial input do:
	//	fft(signal, n, real_amplitudes, imaginary_amplitudes, true, true);
	//	ifft(real_amplitudes, real_amplitudes, n/2+1, signal, true);
	template<sci::ranges::vector_range T, sci::ranges::vector_range U, sci::ranges::vector_range_resizable V>
	void ifft(const T& re_input, const U& im_input, V& re_output, bool performNegativeFrequencyAmplitudeHalfing = true)
	{
		size_t inputSize = std::ranges::size(re_input);
		if (inputSize != std::ranges::size(im_input))
			throw(sci::err(sci::errcategory::SERR_VECTOR, 0, sU("ifft - both inputs must have the same size.")));
		re_output.resize((inputSize - 1) * 2);
		ifft(std::ranges::cdata(re_input), std::ranges::cdata(im_input), inputSize, std::ranges::data(re_output), performNegativeFrequencyAmplitudeHalfing);
	}

	template<sci::ranges::vector_range T, sci::ranges::vector_range U, sci::ranges::vector_range_resizable V>
	void crossCorrelate(const T& input1, const U& input2, V& output)
	{
		//pad the data with zeros to the smallest power 2 size greater than or equal to input1.size()+input2.size()-1
		//also subract the mean
		size_t paddedSize = std::ranges::size(input1) + std::ranges::size(input2) - 1;
		size_t power = size_t(std::ceil(std::log2(double(paddedSize))));
		paddedSize = std::pow(2, power);
		V input1Padded;
		std::vector<double> input2Padded;
		input1Padded.resize(paddedSize);
		input2Padded.resize(paddedSize);

		//auto mean1 = mean(input1);
		//auto mean2 = mean(input2);
		auto mean1 = 0.0;
		auto mean2 = 0.0;

		auto it = std::ranges::cbegin(input1);
		auto itPad = std::ranges::begin(input1Padded);
		for (; it != std::ranges::cend(input1); ++it, ++itPad)
			*itPad = *it - mean1;
		for (; itPad != std::ranges::end(input1Padded);++itPad)
			*itPad = 0;
		
		it = std::ranges::cbegin(input2);
		itPad = std::ranges::begin(input2Padded);
		for (; it != std::ranges::cend(input2); ++it, ++itPad)
			*itPad = *it - mean2;
		for (; itPad != std::ranges::end(input2Padded);++itPad)
			*itPad = 0;

		V re_amplitude1;
		V im_amplitude1;
		V re_amplitude2;
		V im_amplitude2;

		fft(input1Padded, re_amplitude1, im_amplitude1, false, false);
		//std::reverse(input2Padded.begin(), input2Padded.end());
		fft(input2Padded, re_amplitude2, im_amplitude2, false, false);
		size_t outputPaddedSize = std::ranges::size(re_amplitude1);
		auto re_it1 = std::ranges::begin(re_amplitude1);
		auto im_it1 = std::ranges::begin(im_amplitude1);
		auto re_it2 = std::ranges::begin(re_amplitude2);
		auto im_it2 = std::ranges::begin(im_amplitude2);
		double normConstant = (double)std::ranges::size(input1) * 2.0;
		for (size_t i = 0; i < outputPaddedSize; ++i, ++re_it1, ++im_it1, ++re_it2, ++im_it2)
		{
			auto re = *re_it1 * *re_it2 + *im_it1 * *im_it2;
			auto im = *im_it1 * *re_it2 - *re_it1 * *im_it2;
			//auto re = *re_it1 * *re_it2 - *im_it1 * *im_it2;
			//auto im = *re_it1 * *im_it2 + *re_it2 * *im_it1;
			*re_it1 = re / normConstant;
			*im_it1 = im / normConstant;
		}

		ifft(re_amplitude1, im_amplitude1, output, false);

		//reorder the points
		size_t size1 = std::ranges::size(input1);
		for (size_t i = 0; i < size1 / 2; ++i)
			std::swap(*(std::ranges::begin(output) + i), *(std::ranges::begin(output) + size1 - 1 - i));
		size_t size2 = std::ranges::size(input2) - 1;
		size_t extraSize = paddedSize - (std::ranges::size(input1) + std::ranges::size(input2) - 1);
		for (size_t i = 0; i < size2; ++i)
			*(std::ranges::begin(output) + size1 + i) = *(std::ranges::begin(output) + size1 + i + extraSize);
		for(size_t i=0; i<size2/2; ++i)
			std::swap(*(std::ranges::begin(output) + size1 + i), *(std::ranges::begin(output) + size1 + size2 - 1 - i));

		//remove the padding
		output.resize(paddedSize-extraSize);

		//normalise by numberof points summed for each element
		size_t inputSize1 = std::ranges::size(input1);
		size_t inputSize2 = std::ranges::size(input2);
		size_t outputSize = paddedSize - extraSize;
		for (size_t i = 0; i < inputSize2 - 1; ++i)
			*(std::ranges::begin(output) + i) /= (double)(i + 1);
		for(size_t i=0; i< outputSize - 2 * inputSize2 + 2; ++i)
			*(std::ranges::begin(output) + i + inputSize2 - 1) /= (double)(inputSize2);
		for (size_t i = 0; i < inputSize2 - 1; ++i)
			*(std::ranges::begin(output) + outputSize - inputSize2 + 1 + i) /= (double)(inputSize2 - 1 - i);
	}

	template<sci::ranges::vector_range T, sci::ranges::vector_range U, sci::ranges::vector_range_resizable V>
	void crossCorrelateNaive(const T& input1, const U& input2, V& output)
	{
		size_t inputSize1 = std::ranges::size(input1);
		size_t inputSize2 = std::ranges::size(input2);
		output.resize(std::ranges::size(input1) + std::ranges::size(input2) - 1);

		size_t nNegative = inputSize1-1;
		auto it2 = std::ranges::cbegin(input2);
		for (size_t i = 0; i < nNegative; ++i)
		{
			size_t length = std::min(i + 1, inputSize2);
			size_t nOmitted = inputSize1 - i - 1;
			auto it1 = std::ranges::cbegin(input1) + nOmitted;
			auto itOut = std::ranges::begin(output) + i;
			std::span span1(it1, length);
			std::span span2(it2, length);
			*itOut = sci::covarianceNoBessel(span1, span2) / std::sqrt(sci::varianceNoBessel(span1) * sci::varianceNoBessel(span2));
		}
		auto it1 = std::ranges::cbegin(input1);
		for (size_t i = 0; i < inputSize2; ++i)
		{
			size_t length = std::min(inputSize1, inputSize2 - i);
			auto it2 = std::ranges::cbegin(input2) + i;
			auto itOut = std::ranges::begin(output) + i + nNegative;
			std::span span1(it1, length);
			std::span span2(it2, length);
			*itOut = sci::covarianceNoBessel(span1, span2 )/std::sqrt(sci::varianceNoBessel(span1)*sci::varianceNoBessel(span2));
		}
	}

	template<sci::ranges::vector_range T, sci::ranges::vector_range U, sci::ranges::vector_range_resizable V>
	void crossCorrelateNaiveNoNormalising(const T& input1, const U& input2, V& output)
	{
		size_t inputSize1 = std::ranges::size(input1);
		size_t inputSize2 = std::ranges::size(input2);
		output.resize(std::ranges::size(input1) + std::ranges::size(input2) - 1);

		size_t nNegative = inputSize1 - 1;
		auto it2 = std::ranges::cbegin(input2);
		for (size_t i = 0; i < nNegative; ++i)
		{
			size_t length = std::min(i + 1, inputSize2);
			size_t nOmitted = inputSize1 - i - 1;
			auto it1 = std::ranges::cbegin(input1) + nOmitted;
			auto itOut = std::ranges::begin(output) + i;
			*itOut = 0.0;
			for (size_t i = 0; i < length; ++i)
				*itOut += *(it1 + i) * *(it2 + i);
		}
		auto it1 = std::ranges::cbegin(input1);
		for (size_t i = 0; i < inputSize2; ++i)
		{
			size_t length = std::min(inputSize1, inputSize2 - i);
			auto it2 = std::ranges::cbegin(input2) + i;
			auto itOut = std::ranges::begin(output) + i + nNegative;
			*itOut = 0.0;
			for (size_t i = 0; i < length; ++i)
				*itOut += *(it1 + i) * *(it2 + i);
		}
	}

	//discrete power spectrum normalised so that the sum of the output is equal to the variance of the input
	//std::vector<double> powerspectrum(const std::vector<double>& v);
	//discrete cospectrum normalised so that the sum of the output is equal to the covariance of the inputs
	//std::vector<double> cospectrum(const std::vector<double>& x, const std::vector<double>& y);
}