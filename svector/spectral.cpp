#include "../include/svector/Spectral.h"
#include "../include/svector/serr.h"
#include<libresample.h>
#include"kiss_fft/kiss_fftr.h"
#include<vector>

void sci::resample(const double* input, size_t n, double factor, double* output, size_t& outputSize)
{
	size_t maxSize = (size_t)ceil((n + 1) * factor);
	if (n > (size_t)std::numeric_limits<int>::max() || maxSize > (size_t)std::numeric_limits<int>::max())
		throw(sci::err(SERR_LIBRESAMPLE, -999, "sci::resample called with input too large - it can only be the maximum int in size."));
	
	void* rs = resample_opend(1, factor, factor);
	if(!rs)
		throw(sci::err(SERR_LIBRESAMPLE, -999, "sci::resample - failed to allocate memory"));
	int used;
	int err = resample_processd(rs, factor, &input[0], (int)n, 1, &used, &output[0], (int)maxSize);
	outputSize = size_t(used);
	resample_closed(rs);
	if (err < 0)
		throw(sci::err(SERR_LIBRESAMPLE, -999, "sci::resample - buffer overflow occurred"));

}

void sci::resample(const float* input, size_t n, float factor, float* output, size_t& outputSize)
{
	size_t maxSize = (size_t)ceil((n + 1) * factor);
	if (n > (size_t)std::numeric_limits<int>::max() || maxSize > (size_t)std::numeric_limits<int>::max())
		throw(sci::err(SERR_LIBRESAMPLE, -999, "sci::resample called with input too large - it can only be the maximum int in size."));

	void* rs = resample_opend(1, factor, factor);
	if (!rs)
		throw(sci::err(SERR_LIBRESAMPLE, -999, "sci::resample - failed to allocate memory"));
	int used;
	int err = resample_process(rs, factor, &input[0], (int)n, 1, &used, &output[0], (int)maxSize);
	outputSize = size_t(used);
	resample_closed(rs);
	if (err < 0)
		throw(sci::err(SERR_LIBRESAMPLE, -999, "sci::resample - buffer overflow occurred"));
}

//Fast Fourier Transform. The input is real, so the output will only include the amplitude for positive frequencies. You can choose to normalise
//the output for the number of points and for the missing amplitude in the negative frequencies. If you wish to do this yourself then to normalise
//for the number of points divide the outputs by the number of points and to normalise for the missing negative frequencies multiply all elements
//of the output except the first and last by 2.
void sci::fft(const double* re_input, size_t n, double* re_output, double* im_output, bool normaliseForNumberOfPoints, bool performNegativeFrequencyAmplitudedoubling)
{
	sci::assertThrow(n % 2 == 0, sci::err(SERR_KISSFFT, 1, "Can only do a FFT with an even number of points."));
	sci::assertThrow(n <= std::numeric_limits<int>::max(), sci::err(SERR_KISSFFT, 3, "Too many points for fft"));

	//set up the state
	kiss_fftr_cfg myCfg = NULL;
	myCfg = kiss_fftr_alloc((int)n, 0, NULL, NULL);

	sci::assertThrow(myCfg, sci::err(SERR_KISSFFT, 2, "Error allocating memeory for FFT"));

	try
	{
		//allocate working memory
		std::vector<kiss_fft_cpx> result(n / 2 + 1);
		//do fft
		kiss_fftr(myCfg, &re_input[0], &result[0]);

		//copy the data to the output
		//re_output.resize(re_input.size() / 2 + 1);
		//im_output.resize(re_input.size() / 2 + 1);
		size_t outputSize = n / 2 + 1;
		if (normaliseForNumberOfPoints || performNegativeFrequencyAmplitudedoubling)
		{
			double normalisationConstant = performNegativeFrequencyAmplitudedoubling ? 2.0 : 1.0;
			double endNormalisationConstant = 1.0;
			if (normaliseForNumberOfPoints)
			{
				normalisationConstant /= double(n);
				endNormalisationConstant /= double(n);
			}

			re_output[0] = result[0].r * endNormalisationConstant;
			im_output[0] = result[0].i * endNormalisationConstant;
			for (size_t i = 1; i < outputSize - 1; ++i)
			{
				re_output[i] = result[i].r * normalisationConstant;
				im_output[i] = result[i].i * normalisationConstant;
			}
			re_output[outputSize - 1] = result.back().r * endNormalisationConstant;
			im_output[outputSize - 1] = result.back().i * endNormalisationConstant;
		}
		else
		{
			for (size_t i = 0; i < outputSize; ++i)
			{
				re_output[i] = result[i].r;
				im_output[i] = result[i].i;
			}
		}
	}
	catch (...)
	{
		kiss_fftr_free(myCfg);
		throw;
	}
	kiss_fftr_free(myCfg);
}


/*void sci::fft(const std::vector<double>& re_input, std::vector<double>& re_output, std::vector<double>& im_output, bool normaliseForNumberOfPoints, bool performNegativeFrequencyAmplitudedoubling)
{
	re_output.resize(re_input.size() / 2 + 1);
	im_output.resize(re_input.size() / 2 + 1);
	fft(&re_input[0], re_input.size(), &re_output[0], &im_output[0], normaliseForNumberOfPoints, performNegativeFrequencyAmplitudedoubling);
}*/


/* {
	sci::assertThrow(re_input.size() % 2 == 0, sci::err(SERR_KISSFFT, 1, "Can only do a FFT with an even number of points."));
	sci::assertThrow(re_input.size() <= std::numeric_limits<int>::max(), sci::err(SERR_KISSFFT, 3, "Too many points for fft"));

	size_t n = re_input.size();

	//set up the state
	kiss_fftr_cfg myCfg = NULL;
	myCfg = kiss_fftr_alloc((int)re_input.size(), 0, NULL, NULL);

	sci::assertThrow(myCfg, sci::err(SERR_KISSFFT, 2, "Error allocating memeory for FFT"));

	try
	{
		//allocate working memory
		std::vector<kiss_fft_cpx> result(re_input.size() / 2 + 1);
		//do fft
		kiss_fftr(myCfg, &re_input[0], &result[0]);

		//copy the data to the output
		re_output.resize(re_input.size()/2+1);
		im_output.resize(re_input.size() / 2 + 1);
		if (normaliseForNumberOfPoints || performNegativeFrequencyAmplitudedoubling)
		{
			double normalisationConstant = performNegativeFrequencyAmplitudedoubling ? 2.0 : 1.0;
			double endNormalisationConstant = 1.0;
			if (normaliseForNumberOfPoints)
			{
				normalisationConstant /= n;
				endNormalisationConstant /= n;
			}

			re_output[0] = result[0].r * endNormalisationConstant;
			im_output[0] = result[0].i * endNormalisationConstant;
			for (size_t i = 1; i < re_output.size()-1; ++i)
			{
				re_output[i] = result[i].r * normalisationConstant;
				im_output[i] = result[i].i * normalisationConstant;
			}
			re_output.back() = result.back().r * endNormalisationConstant;
			im_output.back() = result.back().i * endNormalisationConstant;
		}
		else
		{
			for (size_t i = 0; i < re_output.size(); ++i)
			{
				re_output[i] = result[i].r;
				im_output[i] = result[i].i;
			}
		}
	}
	catch (...)
	{
		kiss_fftr_free(myCfg);
		throw;
	}
	kiss_fftr_free(myCfg);
}*/

void sci::ifft(const double* re_input, const double* im_input, size_t n, double* re_output, bool performNegativeFrequencyAmplitudeHalving)
{
	size_t nOutputPoints = (n - 1) * 2;

	sci::assertThrow(nOutputPoints <= std::numeric_limits<int>::max(), sci::err(SERR_KISSFFT, 3, "Too many points for ifft"));

	//set up the state
	kiss_fftr_cfg myCfg = NULL;
	myCfg = kiss_fftr_alloc((int)nOutputPoints, 1, NULL, NULL);

	sci::assertThrow(myCfg, sci::err(SERR_KISSFFT, 2, "Error allocating memeory for FFT"));

	try
	{
		//copy the input
		std::vector<kiss_fft_cpx> intermediate(n);
		if (performNegativeFrequencyAmplitudeHalving)
		{
			intermediate[0].r = re_input[0];
			intermediate[0].i = im_input[0];
			for (size_t i = 0; i < n; ++i)
			{
				intermediate[i].r = re_input[i] * 0.5;
				intermediate[i].i = im_input[i] * 0.5;
			}
			intermediate.back().r = re_input[n - 1];
			intermediate.back().i = im_input[n - 1];
		}
		else
		{
			for (size_t i = 0; i < n; ++i)
			{
				intermediate[i].r = re_input[i];
				intermediate[i].i = im_input[i];
			}
		}
		//do ifft
		kiss_fftri(myCfg, &intermediate[0], &re_output[0]);

	}
	catch (...)
	{
		kiss_fftr_free(myCfg);
		throw;
	}
	kiss_fftr_free(myCfg);
}

/*void sci::ifft(const std::vector<double>& re_input, std::vector<double>& im_input, std::vector<double>& re_output, bool undoNormaliseForNumberOfPoints, bool performNegativeFrequencyAmplitudedoubling)
{
	size_t nOutputPoints = (re_input.size() - 1) * 2;

	sci::assertThrow(nOutputPoints <= std::numeric_limits<int>::max(), sci::err(SERR_KISSFFT, 3, "Too many points for ifft"));

	//set up the state
	kiss_fftr_cfg myCfg = NULL;
	myCfg = kiss_fftr_alloc((int)nOutputPoints, 1, NULL, NULL);

	sci::assertThrow(myCfg, sci::err(SERR_KISSFFT, 2, "Error allocating memeory for FFT"));

	try
	{
		//copy the input
		std::vector<kiss_fft_cpx> intermediate(re_input.size());
		//The normalisation is done by default in the ifft, so we need to undo it 
		if (undoNormaliseForNumberOfPoints || performNegativeFrequencyAmplitudedoubling)
		{
			double normalisationConstant = performNegativeFrequencyAmplitudedoubling ? 0.5 : 1.0;
			double endNormalisationConstant = 1.0;
			if (undoNormaliseForNumberOfPoints)
			{
				normalisationConstant /= nOutputPoints;
				endNormalisationConstant /= nOutputPoints;
			}
			intermediate[0].r = re_input[0] * endNormalisationConstant;
			intermediate[0].i = im_input[0] * endNormalisationConstant;
			for (size_t i = 0; i < re_input.size(); ++i)
			{
				intermediate[i].r = re_input[i] * normalisationConstant;
				intermediate[i].i = im_input[i] * normalisationConstant;
			}
			intermediate.back().r = re_input.back() * endNormalisationConstant;
			intermediate.back().i = im_input.back() * endNormalisationConstant;
		}
		else
		{
			for (size_t i = 0; i < re_input.size(); ++i)
			{
				intermediate[i].r = re_input[i];
				intermediate[i].i = im_input[i];
			}
		}
		//do ifft
		re_output.resize(nOutputPoints);
		kiss_fftri(myCfg, &intermediate[0], &re_output[0]);

	}
	catch (...)
	{
		kiss_fftr_free(myCfg);
		throw;
	}
	kiss_fftr_free(myCfg);
}*/

/*std::vector<double> sci::powerspectrum(const std::vector<double>& v)
{
	std::vector<double>im;
	std::vector<double>re;
	sci::fft(v, re, im);
	double size = (double)re.size();
	return (re * re + im * im) / size / size;
}

std::vector<double> sci::cospectrum(const std::vector<double>& x, const std::vector<double>& y)
{
	if (x.size() != y.size()) return std::vector<double>(0);
	std::vector<double>imX;
	std::vector<double>reX;
	std::vector<double>imY;
	std::vector<double>reY;
	sci::fft(x, reX, imX);
	sci::fft(y, reY, imY);
	double size = (double)reX.size();
	return(reX * reY + imX * imY) / size / size;
}*/