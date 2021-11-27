#include"../include/svector/svector.h"
#include<libresample.h>
//#include <alg/ap.h>
//#include <alg/fasttransforms.h>
//#include <alg/interpolation.h>
#include<time.h>
#include"../include/svector/dep/svi.h"
#include"../include/svector/operators.h"
#include"../include/svector/math.h"
#include"../include/svector/sstring.h"
#include<assert.h>
#include"eigeneng/eigeneng.h"


void sci::resample(const std::vector<double> &input, double factor, std::vector<double> &output)
{
	size_t outputSize=(size_t)ceil((input.size()+1)*factor);
	sci::assertThrow(outputSize<=(size_t)std::numeric_limits<int>::max(),sci::err(SERR_VECTOR, -999, "sci::resample called with input too large - it can only be the maximum int in size."));
	output.resize(outputSize);
	void *rs=resample_opend(1,factor,factor);
	int used;
	int nwritten=resample_processd(rs,factor,&input[0], (int)input.size(),1,&used,&output[0],(int)output.size());
	resample_closed(rs);
	if(nwritten<0)
	{
		//we have an error - tbd how we pass that back
	}

}
void sci::resample(const std::vector<float> &input, float factor, std::vector<float> &output)
{
	size_t outputSize=(size_t)ceil((input.size()+1)*factor);
	sci::assertThrow(outputSize<=(size_t)std::numeric_limits<int>::max(),sci::err(SERR_VECTOR, -999, "sci::resample called with input too large - it can only be the maximum int in size."));
	output.resize(outputSize);
	void *rs=resample_open(1,factor,factor);
	int used;
	int nwritten=resample_process(rs,factor,&input[0], (int)input.size(),1,&used,&output[0],(int)output.size());
	resample_close(rs);
	if(nwritten<0)
	{
		//we have an error - tbd how we pass that back
	}

}

#include"kiss_fft/kiss_fftr.h"
//Fast Fourier Transform. The input is real, so the output will only include the amplitude for positive frequencies. You can choose to normalise
//the output for the number of points and for the missing amplitude in the negative frequencies. If you wish to do this yourself then to normalise
//for the number of points divide the outputs by the number of points and to normalise for the missing negative frequencies multiply all elements
//of the output except the first and last by 2.
void sci::fft(const std::vector<double> &re_input, std::vector<double> &re_output, std::vector<double> &im_output, bool normaliseForNumberOfPoints, bool performNegativeFrequencyAmplitudedoubling)
{
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
}

//This assumes that the input is just the positive frequencies, hence it will provide a real output. Unlike the fft function this implementation outputs results which
//are normalised for the number of points by default. This means if you are using this function to undo a sci::fft call, the undoNormalisedNumberOfPoints parameter
//here, must be the opposite of the normaliseForNumberOfPoints parameter of sci::fft. However, if you have doubled the positive frequency values to account for
//the negative frequencies, then you must pass true for normaliseForNegativeFrequencies. Hence the following two function calls will undo each other
//sci::fft(wave, freqReal, freqIm, true, true);
//sci::ifft(freqReal, freqIm, wave, false, true);
void sci::ifft(const std::vector<double> &re_input, std::vector<double> &im_input, std::vector<double> &re_output, bool undoNormaliseForNumberOfPoints, bool performNegativeFrequencyAmplitudedoubling)
{
	size_t nOutputPoints = (re_input.size() - 1) * 2;

	sci::assertThrow(nOutputPoints<=std::numeric_limits<int>::max(), sci::err(SERR_KISSFFT, 3, "Too many points for ifft"));

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
}

std::vector<double> sci::powerspectrum(const std::vector<double> &v)
{
	std::vector<double>im;
	std::vector<double>re;
	sci::fft(v,re,im);
	double size=(double)re.size();
	return (re*re+im*im)/size/size;
}

std::vector<double> sci::cospectrum(const std::vector<double> &x, const std::vector<double> &y)
{
	if(x.size()!=y.size()) return std::vector<double>(0);
	std::vector<double>imX;
	std::vector<double>reX;
	std::vector<double>imY;
	std::vector<double>reY;
	sci::fft(x,reX,imX);
	sci::fft(y,reY,imY);
	double size=(double)reX.size();
	return(reX*reY+imX*imY)/size/size;
}


double sci::round(double n)
{
	if(n>0.0)
	{
		return floor(n+0.5);
	}
	else
	{
		return ceil(n-0.5);
	}
}


void sci::logspace(double xstart, double xinterval, std::vector<double> y, double newxinterval, double newxbase, std::vector<double> &logspacex, std::vector<double> &logspacey)
{
	if(newxbase<1.0 || xinterval<=0.0 ||newxinterval<=0.0)
	{
		logspacex.resize(0);
		logspacey.resize(0);
	}
	newxinterval/=xinterval; //convert new x interval to number of points from x units.
	std::vector<double>x=sci::indexvector<double>(y.size())*xinterval+xstart;
	//work out how many points we'll have in our new vectors
	size_t newsize=0;
	size_t oldsizeest=(size_t)newxinterval;
	double lastaddition=newxinterval;
	while(oldsizeest<=y.size())
	{
		++newsize;
		lastaddition*=newxbase;
		oldsizeest+=(size_t)lastaddition;
	}
	logspacex.resize(newsize);
	logspacey.resize(newsize);
	std::vector<double>::iterator newxi=logspacex.begin();
	std::vector<double>::iterator newyi=logspacey.begin();
	std::vector<double>::const_iterator xsubbegin=x.begin();
	std::vector<double>::const_iterator ysubbegin=y.begin();
	for(; newxi!=logspacex.end(); ++newxi)
	{
		//calculate xrange and logarithmic x range for this subset and average the x values in this subset
		double linrange=floor(newxinterval)*xinterval; //linear range
		double minsubx=*xsubbegin-0.5*xinterval;//start of subset
		double maxsubx=minsubx+linrange; //end of subset
		double logrange=std::log10(maxsubx/minsubx); //log range
		//average the x values for this subset
		*newxi=(minsubx+maxsubx)/2.0;
		//std::vector<double>::const_iterator xi=xsubbegin;
		//for(; xi!=xsubbegin+(size_t)newxinterval; ++xi) *newxi+=*xi;
		//*newxi/=floor(newxinterval);

		//average the y values in the range
		for(std::vector<double>::const_iterator yi=ysubbegin; yi!=ysubbegin+(size_t)newxinterval; ++yi) *newyi+=*yi;
		*newyi/=floor(newxinterval);

		//normalise to log space
		*newyi*=linrange/logrange;

		//incriment the y iterator (the x one is done by the if statement
		++newyi;
		//incriment the subitterator
		xsubbegin+=(size_t)newxinterval;
		ysubbegin+=(size_t)newxinterval;

		//increase the period for the next subrange
		newxinterval*=newxbase;
	}
}

void sci::fitstraightline(const std::vector<double> &x, const std::vector<double> &y, double &grad, double &intercept)
{
	sci::assertThrow(x.size() == y.size(), sci::err(sci::SERR_VECTOR, 0));
	sci::assertThrow(x.size() > 1, sci::err(sci::SERR_VECTOR, 0));
	double meanX;
	double varX;
	double meanY;
	double covarXY;

	meanX = sci::mean(x);
	varX = sci::varianceNoBessel(x, meanX);
	meanY = sci::mean(y);
	covarXY = 0.0;
	const double* xIter = &x[0];
	const double* yIter = &y[0];
	const double* xEnd = xIter + x.size();
	for (; xIter != xEnd; ++xIter, ++yIter)
	{
		covarXY += (*xIter - meanX) * (*yIter - meanY);
	}
	covarXY /= (double)(y.size());

	grad = covarXY / varX;
	intercept = meanY - grad * meanX;
}

//fits a straight line, removing any outliers with residuals bigger than maxresiduals
//This is performed one at a time, removing the worst residual then fitting to the
//remaining data set
//Returns the number of data points used in the fit
size_t sci::fitstraightlinewithoutlierremoval(const std::vector<double>& x, const std::vector<double>& y, double& grad, double& intercept, double maxresidual)
{
	sci::assertThrow(x.size() > 1, sci::err(SERR_VECTOR, -999, "sci::fitstraightlinewithoutlierremoval called with 1 or fewer x values."));
	sci::assertThrow(x.size() == y.size(), sci::err(SERR_VECTOR, -999, "sci::fitstraightlinewithoutlierremoval called with a different number of x and y values."));
	std::vector<double> fitx = x;
	std::vector<double> fity = y;
	double maxresidualsquared = maxresidual * maxresidual;
	fitstraightline(fitx, fity, grad, intercept);
	std::vector<double> residualssquared = (sci::pow(fity - fitx * grad - intercept, 2));
	size_t maxindex = indexofmax(residualssquared);
	double thismaxresidualsquared = residualssquared[maxindex];
	while (thismaxresidualsquared > maxresidualsquared)
	{
		fitx.erase(fitx.begin() + maxindex);
		fity.erase(fity.begin() + maxindex);
		fitstraightline(fitx, fity, grad, intercept);
		residualssquared = (sci::pow(fity - fitx * grad - intercept, 2));
		maxindex = indexofmax(residualssquared);
		thismaxresidualsquared = residualssquared[maxindex];
	}

	return fitx.size();
}


void sci::fitstraightline(const std::vector<double> &x, const std::vector<double> &y, double &grad, double &intercept, double &vargrad, double &varintercept, double &covar)
{
	sci::assertThrow(x.size() == y.size(), sci::err(sci::SERR_VECTOR, 0));
	sci::assertThrow(x.size() > 1, sci::err(sci::SERR_VECTOR, 0));
	double meanX;
	double varX;
	double meanY;
	double covarXY;

	meanX = sci::mean(x);
	varX = sci::varianceNoBessel(x, meanX);
	meanY = sci::mean(y);
	covarXY = 0.0;
	const double* xIter = &x[0];
	const double* yIter = &y[0];
	const double* xEnd = xIter + x.size();
	for (; xIter != xEnd; ++xIter, ++yIter)
	{
		covarXY += (*xIter - meanX) * (*yIter - meanY);
	}
	covarXY /= (double)(y.size());

	grad = covarXY / varX;
	intercept = meanY - grad * meanX;

	double varYResiduals = 0.0;
	xIter = &x[0];
	yIter = &y[0];
	xEnd = xIter + x.size();
	for (; xIter != xEnd; ++xIter, ++yIter)
	{
		double residual = *yIter - (grad * *xIter + intercept);
		varYResiduals += residual*residual;
	}
	varYResiduals /= (double)(y.size() - 2);

	vargrad = varYResiduals / varX / (double)y.size();
	varintercept = varYResiduals / (double)y.size() * (1.0 + meanX * meanX / varX);
	covar = -varYResiduals * meanX / varX * double(y.size() - 2) / double(y.size());
}
/*
//fit y=mx[i]+c, i.e. multiple x parameters. The covariance matrix returned had size x.size()+1
// in both dimensions. The last element represents the covariance with the intercept.
void sci::fitstraightline(const std::vector<std::vector<double>> &x, const std::vector<double> &y, 
	std::vector<double> &grad, double &intercept, std::vector<std::vector<double>> &covar)
{
	//fit y=mx+c
	//Alglib needs us to separate our function into basis functions for linear fitting. Instead of submitting y, x
	//and a function to fit we submit y, F0(x), F1(x), F2(x),... and alglib finds parameters Ci such to give least squares
	//approximation of y=sum(Ci*Fi(x))
	//For linear fit our basises are F0=x, F1=1; C0 will then be the gradient and C1 the intercept
	sci::assertThrow(x.size()>0,sci::err(SERR_VECTOR, -999, "sci::fitstraightline called with 1 or fewer x values."));
	for(size_t i=1; i<x.size(); ++i)
		sci::assertThrow( x[i].size() == x[0].size(), sci::err(SERR_VECTOR, -999, "sci::fitProportional called with multi-dimensional x values, but not all x values with the same number of dimensions.") );
	sci::assertThrow( y.size()==x[0].size(), sci::err(SERR_VECTOR, -999, "sci::fitstraightline called with a different number of x and y values.") );
	grad.resize( x.size(), std::numeric_limits<double>::quiet_NaN() );
	intercept = std::numeric_limits<double>::quiet_NaN();
	covar = sci::makevector( std::numeric_limits<double>::quiet_NaN(), x.size()+1, x.size()+1 );


	std::vector < std::vector<double> > basises;
	for(size_t i=0; i<x.size(); ++i)
		basises.push_back(x[i]);
	basises.push_back(std::vector<double>(x[0].size(),1.0));
	//convert vectors to alg arrays
	alglib::real_1d_array algy;
	alglib::real_2d_array algbasises;
	svi::vectortoalg(y,algy);
	svi::transposedvectortoalg(basises,algbasises);

	//create output variables
	alglib::ae_int_t info;
	alglib::real_1d_array c;
	alglib::lsfitreport report;

	//run fit
	try
	{
		alglib::lsfitlinear(algy,algbasises,y.size(),x.size()+1,info,c,report);
	}
	catch(alglib::ap_error err)
	{
		assert(false);
		throw(sci::err(SERR_ALG, -999, err.msg));
	}

	//put results in outputs
	intercept=c[x.size()];
	svi::algtovector(c, grad);
	grad.resize(x.size()); //clip the last one off as this is the intercept;
	svi::algtovector( report.covpar, covar );
}

void sci::fitProportional(const std::vector<std::vector<double>> &x, const std::vector<double> &y, 
	std::vector<double> &grad, std::vector<std::vector<double>> &covar)
{
	//fit y=mx+c
	//Alglib needs us to separate our function into basis functions for linear fitting. Instead of submitting y, x
	//and a function to fit we submit y, F0(x), F1(x), F2(x),... and alglib finds parameters Ci such to give least squares
	//approximation of y=sum(Ci*Fi(x))
	//For linear fit our basises are F0=x, F1=1; C0 will then be the gradient and C1 the intercept
	sci::assertThrow(x.size()>0,sci::err(SERR_VECTOR, -999, "sci::fitProportional called with 0 x values."));
	for(size_t i=1; i<x.size(); ++i)
		sci::assertThrow( x[i].size() == x[0].size(), sci::err(SERR_VECTOR, -999, "sci::fitProportional called with multi-dimensional x values, but not all x values with the same number of dimensions.") );
	sci::assertThrow( y.size()==x[0].size(), sci::err(SERR_VECTOR, -999, "sci::fitProportional called with a different number of x and y values.") );
	grad.resize( x.size(), std::numeric_limits<double>::quiet_NaN() );
	covar = sci::makevector( std::numeric_limits<double>::quiet_NaN(), x.size(), x.size() );


	std::vector < std::vector<double> > basises;
	for(size_t i=0; i<x.size(); ++i)
		basises.push_back(x[i]);
	basises.push_back(std::vector<double>(x[0].size(),1.0));
	//convert vectors to alg arrays
	alglib::real_1d_array algy;
	alglib::real_2d_array algbasises;
	svi::vectortoalg(y,algy);
	svi::transposedvectortoalg(basises,algbasises);

	//create output variables
	alglib::ae_int_t info;
	alglib::real_1d_array c;
	alglib::lsfitreport report;

	//run fit
	try
	{
		alglib::lsfitlinear(algy,algbasises,y.size(),x.size(),info,c,report);
	}
	catch(alglib::ap_error err)
	{
		assert(false);
		throw(sci::err(SERR_ALG, -999, err.msg));
	}

	//put results in outputs
	svi::algtovector(c, grad);
	svi::algtovector( report.covpar, covar );
}

void sci::fitstraightline(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &weights, double &grad, double &intercept, double &vargrad, double &varintercept, double &covar)
{
	//fit y=mx+c
	//Alglib needs us to separate our function into basis functions for linear fitting. Instead of submitting y, x
	//and a function to fit we submit y, F0(x), F1(x), F2(x),... and alglib finds parameters Ci such to give least squares
	//approximation of y=sum(Ci*Fi(x))
	//For linear fit our basises are F0=x, F1=1; C0 will then be the gradient and C1 the intercept
	sci::assertThrow(x.size()>1,sci::err(SERR_VECTOR, -999, "sci::fitstraightline called with 1 or fewer x values."));
	sci::assertThrow(x.size()==y.size(),sci::err(SERR_VECTOR, -999, "sci::fitstraightline called with a different number of x and y values."));

	std::vector < std::vector<double> > basises;
	basises.push_back(x);
	basises.push_back(std::vector<double>(x.size(),1.0));
	//convert vectors to alg arrays
	alglib::real_1d_array algy;
	alglib::real_1d_array algw;
	alglib::real_2d_array algbasises;
	svi::vectortoalg(y,algy);
	svi::vectortoalg(weights,algw);
	svi::transposedvectortoalg(basises,algbasises);

	//create output variables
	alglib::ae_int_t info;
	alglib::real_1d_array c;
	alglib::lsfitreport report;

	//run fit
	try
	{
		alglib::lsfitlinearw(algy,algw,algbasises,y.size(),2,info,c,report);
	}
	catch(alglib::ap_error err)
	{
		assert(false);
		throw(sci::err(SERR_ALG, -999, err.msg));
	}


	//put results in outputs
	grad=c[0];
	intercept=c[1];
	vargrad=report.covpar[0][0];
	varintercept=report.covpar[1][1];
	covar=report.covpar[0][1];
}


void sci::fitpowerlaw(double power, const std::vector<double> &x, const std::vector<double> &y, double &k, double &vark)
{
	//fit y=mx+c
	//Alglib needs us to separate our function into basis functions for linear fitting. Instead of submitting y, x
	//and a function to fit we submit y, F0(x), F1(x), F2(x),... and alglib finds parameters Ci such to give least squares
	//approximation of y=sum(Ci*Fi(x))
	//For a power fit our basises are F0=x^power

	std::vector < std::vector<double> > basises;
	basises.push_back(sci::pow(x,power));
	//convert vectors to alg arrays
	alglib::real_1d_array algy;
	alglib::real_2d_array algbasises;
	svi::vectortoalg(y,algy);
	svi::transposedvectortoalg(basises,algbasises);

	//create output variables
	alglib::ae_int_t info;
	alglib::real_1d_array c;
	alglib::lsfitreport report;

	//run fit
	try
	{
		alglib::lsfitlinear(algy,algbasises,y.size(),1,info,c,report);
	}
	catch(alglib::ap_error err)
	{
		assert(false);
		throw(sci::err(SERR_ALG, -999, err.msg));
	}


	//put results in outputs
	k=c[0];
	vark=report.covpar[0][0];
}

void sci::crosscorr(const std::vector<double> &f, const std::vector<double> &g, std::vector<double> &r, std::vector<long> &fshifts, bool circular)
{
	//cross correlate f with g. The results are placed in r and the amount of shift of f for each element of r is stored in fshifts

	//convert vectors to alg arrays
	alglib::complex_1d_array algf;
	alglib::complex_1d_array algg;
	svi::vectorstoalgc(f,std::vector<double>(f.size(),0.0),algf);
	svi::vectorstoalgc(g,std::vector<double>(g.size(),0.0),algg);


	//create output variable
	alglib::complex_1d_array algr;

	//run fit
	try
	{
		if(!circular) alglib::corrc1d(algg,g.size(),algf,f.size(),algr);
		else alglib::corrc1dcircular(algg,g.size(),algf,f.size(),algr);
	}
	catch(alglib::ap_error err)
	{
		assert(false);
		throw(sci::err(SERR_ALG, -999, err.msg));
	}

	//put results in outputs
	std::vector<double> dummy;
	svi::algctovectors(algr,r,dummy);
	if(f.size()==1) fshifts=sci::indexvector<long>(r.size());
	else fshifts=sci::concat(sci::indexvector<long>(g.size()),sci::indexvector<long>(f.size()-1)-long(f.size()-1));
	
}



struct minimisablefunctionstructure1d
{
	const std::vector<double> *fixedparams;
	double (*functiontominimise)(const std::vector<double> &,const std::vector<double> &);
};
struct minimisablefunctionstructure2d
{
	const std::vector< std::vector<double> > *fixedparams;
	double (*functiontominimise)(const std::vector<double> &,const std::vector< std::vector<double> > &);
};
void minimisablefunction1d (const alglib::real_1d_array &x, double &func, void *ptr)
{
	minimisablefunctionstructure1d *struc=(minimisablefunctionstructure1d*)ptr;
	std::vector<double> tunableparams;
	svi::algtovector(x,tunableparams);
	func=struc->functiontominimise(tunableparams,*(struc->fixedparams));
}

void minimisablefunction2d (const alglib::real_1d_array &x, double &func, void *ptr)
{
	minimisablefunctionstructure2d *struc=(minimisablefunctionstructure2d*)ptr;
	std::vector<double> tunableparams;
	svi::algtovector(x,tunableparams);
	func=struc->functiontominimise(tunableparams,*(struc->fixedparams));
}

//minimises a function of tunableparams,fixedparams by changinging tunableparams.
//tunable params must contain an initial guess.
//returns the number of iterations
size_t sci::minimise(std::vector<double> &tunableparams, const std::vector<double> &fixedparams, double (*functiontominimise)(const std::vector<double> &,const std::vector<double> &))
{

	//create tunable params
	alglib::real_1d_array algtunableparams;
	svi::vectortoalg(tunableparams,algtunableparams);
	//create a minimisablefunctionstruct we can pass to the minimiser
	minimisablefunctionstructure1d struc={&fixedparams,functiontominimise};

	//create a state object that keeps track of the fitting
	alglib::mincgstate state;
	//alglib::mincgcreate(algtunableparams, state); //use this if we supply a function that reports the gradient
	alglib::mincgcreatef(algtunableparams,1.0e-6, state); //use this is if we only suply a function that reports values
	//set the scale of the variables. Helps algorithm converge. Should
	//be about the magnitude of the variables or the magnitude of how much
	//they will change
	//we just set the scale as the initial guess or 1.0 if the initial guess is 0.0
	//We should allow this to be modified by the user as the magnitude of how much they
	//will change may not be of the same order as the guess -e.g. a guess might be 0.0
	//but we might expect the result to be in the range +/- 100 so the scale should be 100
	alglib::real_1d_array scales=algtunableparams;
	for(int i=0; i<scales.length(); ++i)
		scales[i]= (scales[i]==0.0 ? 1.0 : scales[i]);
	alglib::mincgsetscale(state,scales);

	//set stopping condition,
	//	first number is gradient acceptance condition
	//	second is deltafunc/max(func,1) acceptance condition
	//	third number is deltaparams/params acceptance condition
	//	forth number is max itterations acceptance condition
	//setting all four to zero uses a small vale of deltaparams/params
	alglib::mincgsetcond(state, 0, 0, 0, 0);

	try
	{
		//do the minimising
		alglib::mincgoptimize(state, &minimisablefunction1d,NULL,&struc);
	}
	catch(alglib::ap_error err)
	{
		assert(false);
		throw(sci::err(SERR_ALG, -999, err.msg));
	}

	//get the results
	alglib::mincgreport report;
	alglib::mincgresults(state, algtunableparams, report);

	svi::algtovector(algtunableparams,tunableparams);

	//return the number of iterations
	return report.iterationscount;
}

//minimises a function of tunableparams,fixedparams by changinging tunableparams.
//tunable params must contain an initial guess.
//returns the number of iterations
size_t sci::minimise(std::vector<double> &tunableparams, const std::vector<std::vector <double> > &fixedparams, double (*functiontominimise)(const std::vector<double> &,const std::vector< std::vector<double> > &))
{
	//create tunable params
	alglib::real_1d_array algtunableparams;
	svi::vectortoalg(tunableparams,algtunableparams);
	//create a minimisablefunctionstruct we can pass to the minimiser
	minimisablefunctionstructure2d struc={&fixedparams,functiontominimise};

	//create a state object that keeps track of the fitting
	alglib::mincgstate state;
	//alglib::mincgcreate(algtunableparams, state); //use this if we supply a function that reports the gradient
	alglib::mincgcreatef(algtunableparams,1.0e-6, state); //use this is if we only suply a function that reports values
	//set the scale of the variables. Helps algorithm converge. Should
	//be about the magnitude of the variables or the magnitude of how much
	//they will change
	//we just set the scale as the initial guess or 1.0 if the initial guess is 0.0
	//We should allow this to be modified by the user as the magnitude of how much they
	//will change may not be of the same order as the guess -e.g. a guess might be 0.0
	//but we might expect the result to be in the range +/- 100 so the scale should be 100
	alglib::real_1d_array scales=algtunableparams;
	for(int i=0; i<scales.length(); ++i)
		scales[i]= (scales[i]==0.0 ? 1.0 : scales[i]);
	alglib::mincgsetscale(state,scales);
	//set stopping condition,
	//	first number is gradient acceptance condition
	//	second is deltafunc/max(func,1) acceptance condition
	//	third number is deltaparams/params acceptance condition
	//	forth number is max itterations acceptance condition
	//setting all four to zero uses a small vale of deltaparams/params
	alglib::mincgsetcond(state, 0, 0, 0, 0);

	//do the minimising
	alglib::mincgoptimize(state, &minimisablefunction2d,NULL,&struc);

	//get the results
	alglib::mincgreport report;
	alglib::mincgresults(state, algtunableparams, report);

	svi::algtovector(algtunableparams,tunableparams);

	//return the number of iterations
	return report.iterationscount;
}



struct fitfunctionstructure1d
{
	double (*functiontominimise)(const std::vector<double> &, double);
};
struct fitfunctionstructuremultid
{
	double (*functiontominimise)(const std::vector<double> &,const std::vector<double> &);
};
void fitfunctionmultid (const alglib::real_1d_array &fitParams, const alglib::real_1d_array &x, double &func, void *ptr)
{
	std::vector<double> vectFitParams;
	std::vector<double> vectX;
	svi::algtovector(fitParams,vectFitParams);
	svi::algtovector(x,vectX);
	fitfunctionstructuremultid * struc= (fitfunctionstructuremultid*)ptr;
	func=struc->functiontominimise(vectFitParams,vectX);
}
	
void fitfunction1d (const alglib::real_1d_array &fitParams, const alglib::real_1d_array &x, double &func, void *ptr)
{
	std::vector<double> vectFitParams;
	double doubleX;
	svi::algtovector(fitParams,vectFitParams);
	doubleX=x[0];
	fitfunctionstructure1d * struc= (fitfunctionstructure1d*)ptr;
	func=struc->functiontominimise(vectFitParams,doubleX);
}

//nonlinear least squares fitting 
//pass empty limits to not limit the tunable params or 
//set elements to infinity, -infinity ar a nan to not limit a particular element 
//each element of xs contains the values for all dimensions required to calculate
//ys. So if y is a function of 3 variables x would be xs[N][3]
//leave weigths as an empty vector for unweighted fitting
size_t sci::fitnonlinear(std::vector<double> &tunableparams, std::vector<double> &paramErrs, std::vector<double> tunableparamlowerlimits, std::vector<double> tunableparamupperlimits, double (*function)(const std::vector<double> &,const std::vector<double> &), const std::vector<std::vector<double>> &xs, const std::vector<double> &ys, const std::vector<double> &weights)
{
	
	if(xs.size()==0)
	{
		for(size_t i=0; i<tunableparams.size(); ++i)
			tunableparams[i]=std::numeric_limits<double>::quiet_NaN();
		return 0;
	}
	if(xs.size()!=ys.size())
		throw;
	if(weights.size() > 0 && weights.size() != xs.size())
		throw;
	if(tunableparams.size()==0)
		return 0;


	//deal with the limits
	tunableparamupperlimits.resize(tunableparams.size(),std::numeric_limits<double>::infinity());
	tunableparamlowerlimits.resize(tunableparams.size(),-std::numeric_limits<double>::infinity());
	sci::replacenans(tunableparamupperlimits,std::numeric_limits<double>::infinity());
	sci::replacenans(tunableparamlowerlimits,-std::numeric_limits<double>::infinity());
	//convert to alglib arrays
	alglib::real_1d_array algtunableparams;
	alglib::real_1d_array algtunableparamupperlimits;
	alglib::real_1d_array algtunableparamlowerlimits;
	alglib::real_2d_array algxs;
	alglib::real_1d_array algys;
	alglib::real_1d_array algweights;
	svi::vectortoalg(tunableparams,algtunableparams);
	svi::vectortoalg(xs,algxs);
	svi::vectortoalg(ys,algys);

	//create a minimisablefunctionstruct we can pass to the minimiser
	fitfunctionstructuremultid struc={function};

	//create a state object that keeps track of the fitting
	alglib::lsfitstate state;
	if(weights.size()==0)
	{
		alglib::lsfitcreatef(algxs, algys, algtunableparams, 1.0e-6, state);
	}
	else
	{
		svi::vectortoalg(weights,algweights);
		alglib::lsfitcreatewf(algxs, algys, algweights, algtunableparams, 1.0e-6, state);
	}




	//set the scale of the variables. Helps algorithm converge. Should
	//be about the magnitude of the variables or the magnitude of how much
	//they will change
	alglib::real_1d_array scales=algtunableparams;
	for(int i=0; i<scales.length(); ++i)
		scales[i]= (scales[i]==0.0 ? 1.0 : scales[i]);
	//alglib::lsfitsetscale(state,scales);

	//set boundaries
	if(	tunableparamupperlimits.size() > 0 || tunableparamlowerlimits.size() > 0 )
	{
		svi::vectortoalg(tunableparamupperlimits,algtunableparamupperlimits);
		svi::vectortoalg(tunableparamlowerlimits,algtunableparamlowerlimits);
		alglib::lsfitsetbc(state,algtunableparamlowerlimits,algtunableparamupperlimits);
	}


	//set stopping condition,
	//	first number is gradient acceptance condition
	//	second is deltafunc/max(func,1) acceptance condition
	//	third number is deltaparams/params acceptance condition
	//	forth number is max itterations acceptance condition
	//setting all four to zero uses a small vale of deltaparams/params
	alglib::lsfitsetcond(state, 0, 0, 0);

	//do the minimising
	alglib::lsfitfit(state, &fitfunctionmultid, NULL, &struc);

	//get the results
	alglib::lsfitreport report;
	alglib::ae_int_t errCode;
	alglib::lsfitresults(state, errCode, algtunableparams, report);

	svi::algtovector(algtunableparams,tunableparams);

	//return the number of iterations
	svi::algtovector(report.errpar,paramErrs);
	return report.iterationscount;
}

//nonlinear least squares fitting 
//pass empty limits to not limit the tunable params or 
//set elements to infinity, -infinity ar a nan to not limit a particular element 
//each element of xs contains the values for all dimensions required to calculate
//ys. So if y is a function of 3 variables x would be xs[N][3]
//leave weigths as an empty vector for unweighted fitting
size_t sci::fitnonlinear(std::vector<double> &tunableparams, std::vector<double> &paramErrs, std::vector<double> tunableparamlowerlimits, std::vector<double> tunableparamupperlimits, double (*function)(const std::vector<double> &,double), const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &weights)
{
	if(xs.size()==0)
	{
		for(size_t i=0; i<tunableparams.size(); ++i)
			tunableparams[i]=std::numeric_limits<double>::quiet_NaN();
		return 0;
	}
	if(xs.size()!=ys.size())
		throw;
	if(weights.size() > 0 && weights.size() != xs.size())
		throw;
	if(tunableparams.size()==0)
		return 0;


	//deal with the limits
	tunableparamupperlimits.resize(tunableparams.size(),std::numeric_limits<double>::infinity());
	tunableparamlowerlimits.resize(tunableparams.size(),-std::numeric_limits<double>::infinity());
	sci::replacenans(tunableparamupperlimits,std::numeric_limits<double>::infinity());
	sci::replacenans(tunableparamlowerlimits,-std::numeric_limits<double>::infinity());
	//convert to alglib arrays
	alglib::real_1d_array algtunableparams;
	alglib::real_1d_array algtunableparamupperlimits;
	alglib::real_1d_array algtunableparamlowerlimits;
	alglib::real_2d_array algxs;
	alglib::real_1d_array algys;
	alglib::real_1d_array algweights;
	svi::vectortoalg(tunableparams,algtunableparams);
	algxs.setcontent(xs.size(),1,&xs[0]);
	svi::vectortoalg(ys,algys);


	//create a minimisablefunctionstruct we can pass to the minimiser
	fitfunctionstructure1d struc={function};

	//create a state object that keeps track of the fitting
	alglib::lsfitstate state;
	if(weights.size()==0)
	{
		alglib::lsfitcreatef(algxs, algys, algtunableparams, 1.0e-6, state);
	}
	else
	{
		svi::vectortoalg(weights,algweights);
		alglib::lsfitcreatewf(algxs, algys, algweights, algtunableparams, 1.0e-6, state);
	}




	//set the scale of the variables. Helps algorithm converge. Should
	//be about the magnitude of the variables or the magnitude of how much
	//they will change
	alglib::real_1d_array scales=algtunableparams;
	for(int i=0; i<scales.length(); ++i)
		scales[i]= (scales[i]==0.0 ? 1.0 : scales[i]);
	//alglib::lsfitsetscale(state,scales);

	//set boundaries
	if(	tunableparamupperlimits.size() > 0 || tunableparamlowerlimits.size() > 0 )
	{
		svi::vectortoalg(tunableparamupperlimits,algtunableparamupperlimits);
		svi::vectortoalg(tunableparamlowerlimits,algtunableparamlowerlimits);
		alglib::lsfitsetbc(state,algtunableparamlowerlimits,algtunableparamupperlimits);
	}


	//set stopping condition,
	//	first number is gradient acceptance condition
	//	second is deltafunc/max(func,1) acceptance condition
	//	third number is deltaparams/params acceptance condition
	//	forth number is max itterations acceptance condition
	//setting all four to zero uses a small vale of deltaparams/params
	alglib::lsfitsetcond(state, 0, 0, 0);

	//do the minimising
	alglib::lsfitfit(state, &fitfunction1d, NULL, &struc);

	//get the results
	alglib::lsfitreport report;
	alglib::ae_int_t errCode;
	alglib::lsfitresults(state, errCode, algtunableparams, report);

	svi::algtovector(algtunableparams,tunableparams);

	//return the number of iterations
	svi::algtovector(report.errpar,paramErrs);
	return report.iterationscount;
}
*/
	


/*
std::vector< std::vector<double> > sci::inverse(const std::vector< std::vector<double> > &mat, double conditionNumberLimit)
{
	//check validity, must be square
	//return an empty vector if they fail
	if(mat.size()==0)  return std::vector< std::vector<double> >();
	if(!sci::rectangular(mat)) return std::vector< std::vector<double> >();
	if(mat.size()!=mat[0].size())  return std::vector< std::vector<double> >();

	alglib::real_2d_array algmat;
	svi::vectortoalg(mat,algmat);

	alglib::ae_int_t err;
	alglib::matinvreport rep;
	alglib::rmatrixinverse(algmat,err,rep);

	std::vector<std::vector<double> > result;
	if(err==-3 || rep.rinf<1.0/conditionNumberLimit)
	{
		//couldn't invert return a matrix of nans
		result=sci::makevector(std::numeric_limits<double>::quiet_NaN(),mat.size(),mat[0].size());
	}
	else
	{
		svi::algtovector(algmat,result);
	}
	return result;
}*/

bool sci::pointinpolygon(std::vector<double> polygonx, std::vector<double> polygony, double pointx, double pointy)
{
	if(polygonx.size()<3) return false;
	if(polygonx.size()!=polygony.size()) return false;
	size_t len=polygonx.size();

	long quadrant = polygonx[0]>pointx ? ( polygony[0]>pointy ? 0 : 1 ) : ( polygony[0]<pointy ? 2 :3 );
	long firstquadrant=quadrant;
	long quadrantchange=0;
	for(size_t i=1; i<len; ++i)
	{
		long newquadrant = polygonx[i]>pointx ? ( polygony[i]>pointy ? 0 : 1 ) : ( polygony[i]<pointy ? 2 :3 );
		
		if(newquadrant!=quadrant)
		{
			long dif=newquadrant-quadrant;
			long absdif=std::abs(dif);
			if(absdif==1) quadrantchange+=dif;
			else if(absdif==2)
			{
				//do cross product to find direction
				bool clockwise=( (polygonx[i-1]-pointx)*(polygony[i]-pointy)-(polygony[i-1]-pointy)*(polygonx[i]-pointx) ) < 0.0;
				if(clockwise) quadrantchange+=2;
				else quadrantchange-=2;
			}
			else quadrantchange-=1; //this is the quadrant 0 to 3 case;
		}
		quadrant=newquadrant;
	}
	
	//complete the polygon, going from the last point to the first
	if(firstquadrant!=quadrant)
	{
		long dif=firstquadrant-quadrant;
		long absdif=std::abs(dif);
		if(absdif==1) quadrantchange+=dif;
		else if(absdif==2)
		{
			//do cross product to find direction
			bool clockwise=( (polygonx.back()-pointx)*(polygony[0]-pointy)-(polygony.back()-pointy)*(polygonx[0]-pointx) ) < 0.0;
			if(clockwise) quadrantchange+=2;
			else quadrantchange-=2;
		}
		else quadrantchange-=1; //this is the quadrant 0 to 3 case;
	}

	if(quadrantchange==0) return false;
	return true;
}

void sci::intersect(double xline1point1, double yline1point1, double xline1point2, double yline1point2, double xline2point1, double yline2point1, double xline2point2, double yline2point2, double &intersectx, double &intersecty, double &m1, double &m2)
{
	m1=(yline1point1-yline1point2)/(xline1point1-xline1point2);
	m2=(yline2point1-yline2point2)/(xline2point1-xline2point2);
	if(m1==m2) 
	{
		intersectx = std::numeric_limits<double>::quiet_NaN();
		intersecty = std::numeric_limits<double>::quiet_NaN();
		return;
	}
	if(xline1point1==xline1point2)
	{
		intersectx=xline1point1;
		intersecty=m2*(intersectx-xline2point1)+yline2point1;
		return;
	}
	if(xline2point1==xline2point2)
	{
		intersectx=xline2point1;
		intersecty=m1*(intersectx-xline1point1)+yline1point1;
		return;
	}
	intersectx=(yline2point1-yline1point1-m1*xline1point1+m2*xline2point1)/(m1+m2);
	intersecty=m1*(intersectx-xline1point1)+yline1point1;
}



void sci::eigenvalues(const std::vector<std::vector<double>> &matrix, std::vector<double> &eigenvaluesReal, std::vector<double> &eigenvaluesImaginary)
{
	sci::assertThrow(sci::square(matrix), sci::err(SERR_VECTOR, -999, "sci::eigenvalues called with a non-square matrix."));
	if (matrix.size() == 0)
	{
		eigenvaluesReal.resize(0);
		eigenvaluesImaginary.resize(0);
		return;
	}
	size_t n = matrix.size();
	eigenvaluesReal.resize(n);
	eigenvaluesImaginary.resize(n);
	std::vector<double> workingMatrix(n*n);
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < n; ++j)
			workingMatrix[i*n + j] = matrix[i][j];

	int result = n_eigeng(&workingMatrix[0], n, &eigenvaluesReal[0], &eigenvaluesImaginary[0], NULL);
	sci::assertThrow(result == 0, sci::err(sci::SERR_EIGENENG, result, sU("Error calculating eigen values.")));
}

void sci::eigenvectors(const std::vector<std::vector<double>> &matrix, std::vector<double> &eigenvaluesReal, std::vector<double> &eigenvaluesImaginary, std::vector<std::vector<double>> &eigenvectorsReal, std::vector<std::vector<double>> &eigenvectorsImaginary)
{
	sci::assertThrow(sci::square(matrix), sci::err(SERR_VECTOR, -999, "sci::eigenvalues called with a non-square matrix."));
	if (matrix.size() == 0)
	{
		eigenvaluesReal.resize(0);
		eigenvaluesImaginary.resize(0);
		return;
	}
	size_t n = matrix.size();
	eigenvaluesReal.resize(n);
	eigenvaluesImaginary.resize(n);
	std::vector<double> workingMatrix(n*n);
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < n; ++j)
			workingMatrix[i*n + j] = matrix[i][j];

	std::vector<double> workingVector;

	int result = n_eigeng(&workingMatrix[0], n, &eigenvaluesReal[0], &eigenvaluesImaginary[0], &workingVector[0]);
	sci::assertThrow(result == 0, sci::err(sci::SERR_EIGENENG, result, sU("Error calculating eigen values.")));

	eigenvectorsReal.resize(n);
	eigenvectorsImaginary.resize(n);
	for (size_t i = 0; i < n; ++i)
	{
		eigenvectorsReal[i].resize(n);
		eigenvectorsImaginary[i].resize(n);
		for (size_t j = 0; j < n; ++j)
		{
			eigenvectorsReal[i][j] = workingVector[i*n + j];
		}
	}

	for (size_t i = 0; i < n-1; ++i)
	{
		if (eigenvaluesImaginary[i] != 0.0)
		{
			eigenvectorsImaginary[i] = eigenvectorsReal[i + 1];
			eigenvectorsReal[i + 1] = eigenvectorsReal[i];
			eigenvectorsImaginary[i + 1] = -eigenvectorsImaginary[i];
		}
	}
}

struct IntegrableData
{
	double (*functionToIntegrate)(double x, const std::vector<double> &params);
	const std::vector<double> *data;
};

void integrableFunction(double x, double xminusa, double bminusx, double &y, void *ptr)
{
	IntegrableData *integrableData=(IntegrableData*)ptr;
	y=integrableData->functionToIntegrate(x,*integrableData->data);
}
/*
double sci::integrate(double xMin, double xMax, double intervalMax, double (*functionToIntegrate)(double x, const std::vector<double> &params), const std::vector<double> &params)
{
    alglib::autogkstate state;
    double result;
    alglib::autogkreport rep;

	IntegrableData integrableData;
	integrableData.data=&params;
	integrableData.functionToIntegrate=functionToIntegrate;
	
	if(intervalMax>0.0)
		alglib::autogksmoothw(xMin, xMax, intervalMax, state);
	else
		alglib::autogksmooth(xMin, xMax, state);
    alglib::autogkintegrate(state, integrableFunction, &integrableData);
    alglib::autogkresults(state, result, rep);
	
	if (rep.terminationtype!=1)
		throw;
	return result;
}

size_t sci::randInt(size_t maxVal)
{
	return alglib::randominteger(maxVal);
}

void sci::eigenvalues(const std::vector<std::vector<double>> &matrix, std::vector<double> &eigenvaluesReal, std::vector<double> &eigenvaluesImaginary)
{
	sci::assertThrow(sci::square(matrix),sci::err(SERR_VECTOR, -999, "sci::eigenvalues called with a non-square matrix."));
	
	alglib::real_2d_array algMatrix;
	svi::vectortoalg(matrix,algMatrix);

	alglib::real_1d_array algEigenvalues;
	alglib::real_1d_array algImaginaryEigenvalues;
	alglib::real_2d_array algRightEigenvectors;
	alglib::real_2d_array algLeftEigenvectors;

	//get the eigen values, note that setting parameter 3 to zero means the eigenvectors aren't derived, but the parameters
	//still need to be passed
	sci::assertThrow(alglib::rmatrixevd(algMatrix, matrix.size(), 0, algEigenvalues, algImaginaryEigenvalues, algRightEigenvectors, algLeftEigenvectors),sci::err(SERR_ALG, -999, "Error when calling the ALG routine to determine eigenvectors."));

	svi::algtovector(algEigenvalues,eigenvaluesReal);
	svi::algtovector(algImaginaryEigenvalues,eigenvaluesImaginary);
}

void sci::eigenvector(const std::vector<std::vector<double>> &matrix, std::vector<double> &eigenvaluesReal, std::vector<double> &eigenvaluesImaginary, std::vector<std::vector<double>> &eigenvectors)
{
	sci::assertThrow(sci::square(matrix),sci::err(SERR_VECTOR, -999, "sci::eigenvalues called with a non-square matrix."));
	
	alglib::real_2d_array algMatrix;
	svi::vectortoalg(matrix,algMatrix);

	alglib::real_1d_array algEigenvalues;
	alglib::real_1d_array algImaginaryEigenvalues;
	alglib::real_2d_array algRightEigenvectors;
	alglib::real_2d_array algLeftEigenvectors;

	//get the eigen values, note that setting parameter 3 to one means the right eigenvectors are derived, but the left parameter
	//still need to be passed
	sci::assertThrow(alglib::rmatrixevd(algMatrix, matrix.size(), 1, algEigenvalues, algImaginaryEigenvalues, algRightEigenvectors, algLeftEigenvectors),sci::err(SERR_ALG, -999, "Error when calling the ALG routine to determine eigenvectors."));

	svi::algtovector(algEigenvalues,eigenvaluesReal);
	svi::algtovector(algImaginaryEigenvalues,eigenvaluesImaginary);
}*/

double sci::distribution::normal(double x, double mean, double standardDeviation)
{
	return std::exp((x-mean)*(x-mean)/-2.0/standardDeviation/standardDeviation) / standardDeviation / M_SQRT2PI;
}

double sci::distribution::cumulativeNormal(double x, double mean, double standardDeviation)
{
	return (1.0+sci::erf( (x-mean) / standardDeviation / M_SQRT2 ) ) /2.0;;
}

double sci::distribution::lognormal(double x, double geometricMean, double geometricStandardDeviation)
{
	return sci::distribution::normal(std::log(x), geometricMean, geometricStandardDeviation )/x;
}

double sci::distribution::cumulativeLognormal(double x, double geometricMean, double geometricStandardDeviation)
{
	return sci::distribution::cumulativeNormal(std::log(x), geometricMean, geometricStandardDeviation );
}

double sci::distribution::exponential(double x, double rate)
{
	return x < 0.0 ? 0.0 : rate*std::exp(-rate*x);
}
double sci::distribution::cumulativeExponential(double x, double rate)
{
	return x < 0.0 ? 0.0 : 1.0-std::exp(-rate*x);
}
/*double sci::distribution::gamma(double x, double shape, double scale)
{
	return std::pow(x/scale, shape)/x/sci::gamma(shape)*std::exp(-x/scale);
}
double sci::distribution::cumulativeGamma(double x, double shape, double scale)
{
	return sci::lowerIncompleteGamma(x/scale,shape)/sci::gamma(shape);
}
double sci::distribution::generalisedGamma(double x, double shapeK, double shapeGamma, double scale)
{
	return shapeGamma*std::pow(x/scale,shapeK)/x*std::exp(-std::pow(x/scale,shapeGamma))/sci::gamma(shapeK/shapeGamma);
}
double sci::distribution::cumulativeGeneralisedGamma(double x, double shapeK, double shapeGamma, double scale)
{
	return lowerIncompleteGamma(shapeK/shapeGamma,std::pow(x/scale,shapeGamma))/sci::gamma(shapeK/shapeGamma);
}*/
double sci::distribution::weibull(double x, double shape, double scale)
{
	return x < 0.0 ? 0.0 : shape/scale*std::pow(x/scale,shape-1)*std::exp(-std::pow(x/scale,shape));
}
double sci::distribution::cumulativeWeibull(double x, double shape, double scale)
{
	return x < 0.0 ? 0.0 : 1-std::exp(-std::pow(x/scale,shape));
}
double sci::distribution::rayleigh(double x, double scale)
{
	return sci::distribution::weibull(x,2,scale);
}
double sci::distribution::cumulativeRayleigh(double x, double scale)
{
	return sci::distribution::cumulativeWeibull(x,2,scale);
}
double sci::distribution::boltzmann(double energy, double temperature)
{
	return sci::distribution::exponential(energy, 1.3806488e-23*temperature);
}
double sci::distribution::cumulativeBoltzmann(double energy, double temperature)
{
	return sci::distribution::cumulativeExponential(energy, 1.3806488e-23*temperature);
}
double sci::distribution::cauchy(double x, double mean, double halfWidthAtHalfMaximum)
{
	return halfWidthAtHalfMaximum/M_PI/( (x-mean)*(x-mean)+halfWidthAtHalfMaximum*halfWidthAtHalfMaximum );
}
double sci::distribution::cumulativeCauchy(double x, double mean, double halfWidthAtHalfMaximum)
{
	return atan( (x-mean)/halfWidthAtHalfMaximum )/M_PI+0.5;
}
		
/*
double sci::gamma( double x )
{
	return alglib::gammafunction(x);
}

double sci::lowerIncompleteGamma(double a, double x)
{
	//note the alglib incomplete gamma function gives incomplete gamma(a,x) as a fraction of gamma(a)
	//which is not the general definition
	return alglib::gammafunction(a)*alglib::incompletegamma(a, x);
}

double sci::upperIncompleteGamma( double a, double x )
{
	//note the alglib inclomplete gamma function gives incomplete gamma(a,x) as a fraction of gamma(a)
	//which is not the general definition
	return sci::gamma(a)*(1.0-alglib::incompletegamma(a, x));
}*/


void sci::solveQuadratic(double a, double b, double c, std::complex<double> &sln1, std::complex<double> &sln2)
{
	std::complex<double> root=std::sqrt(std::complex<double>(b*b-4.0*a*c));
	sln1=(-b+root)/2.0/a;
	sln2=(-b-root)/2.0/a;
}

std::complex<double> cubicSln(double a, double b, std::complex<double> u, std::complex<double> C, std::complex<double> delta0)
{
	return -1.0/3.0/a*(b+u*C+delta0/u/C);
}

void sci::solveCubic(double a, double b, double c, double d, std::complex<double> &sln1, std::complex<double> &sln2, std::complex<double> &sln3)
{
	std::complex<double> delta0=b*b-3.*a*c;
	std::complex<double> delta1=2.0*b*b*b-9.0*a*b*c+27*a*a*d;
	std::complex<double> C=std::pow((delta1+std::sqrt(delta1*delta1-4.0*delta0*delta0*delta0))/2.0,1.0/3.0);

	sln1=cubicSln(a,b,1.0,C,delta0);
	sln1=cubicSln(a,b,std::complex<double>(-0.5,std::sqrt(0.75)),C,delta0);
	sln1=cubicSln(a,b,std::complex<double>(-0.5,-std::sqrt(0.75)),C,delta0);
}

void sci::solveQuadratic(double a, double b, double c, double &sln1, double &sln2)
{
	std::complex<double>sln1c;
	std::complex<double>sln2c;
	solveQuadratic(a, b, c, sln1c, sln2c);
	sln1=realOrNan(sln1c);
	sln2=realOrNan(sln2c);
}
void sci::solveCubic(double a, double b, double c, double d, double &sln1, double &sln2, double &sln3)
{
	std::complex<double>sln1c;
	std::complex<double>sln2c;
	std::complex<double>sln3c;
	solveCubic(a, b, c, d, sln1c, sln2c, sln3c);
	sln1=realOrNan(sln1c);
	sln2=realOrNan(sln2c);
	sln3=realOrNan(sln3c);
}


/*bool sci::anyTrue(const std::vector<SBOOL>& v)
{
	if(v.size()==0)
		return false;
	const SBOOL *current=&v[0];
	const SBOOL *end=current+v.size();
	for(; current!=end; ++current)
		if(*current)
			return true;
	return false;
}
bool sci::allTrue(const std::vector<SBOOL> &v)
{
	if(v.size()==0)
		return false;
	const SBOOL *current=&v[0];
	const SBOOL *end=current+v.size();
	for(; current!=end; ++current)
		if(!*current)
			return false;
	return true;
}
bool sci::anyFalse(const std::vector<SBOOL> &v)
{
	return !allTrue(v);
}
bool sci::allFalse(const std::vector<SBOOL> &v)
{
	return !anyTrue(v);
}*/

std::vector<SBOOL> sci::rollingOr(const std::vector<SBOOL> &v, size_t n)
{
	sci::assertThrow(n>0, sci::err(SERR_VECTOR, -999, "sci::rollingOr called with n equal to zero."));
	if(n>v.size())
		return std::vector<SBOOL>(0,0);
	std::vector<SBOOL> result(v.size()-n+1);
	const SBOOL *vStart=&v[0];
	const SBOOL *vEnd=vStart+result.size();
	SBOOL *resultCurrent=&result[0];
	for(; vStart<vEnd; ++vStart,++resultCurrent)
	{
		const SBOOL *vCurrent=vStart;
		const SBOOL *vLegEnd=vCurrent+n;
		for(; vCurrent!=vLegEnd; ++vCurrent)
			*resultCurrent |= *vCurrent;
	}
	return result;
}

std::vector<SBOOL> sci::rollingAnd(const std::vector<SBOOL> &v, size_t n)
{
	sci::assertThrow(n>0, sci::err(SERR_VECTOR, -999, "sci::rollingAnd called with n equal to zero."));
	if(n>v.size())
		return std::vector<SBOOL>(0,1);
	std::vector<SBOOL> result(v.size()-n+1);
	const SBOOL *vStart=&v[0];
	const SBOOL *vEnd=vStart+result.size();
	SBOOL *resultCurrent=&result[0];
	for(; vStart<vEnd; ++vStart,++resultCurrent)
	{
		const SBOOL *vCurrent=vStart;
		const SBOOL *vLegEnd=vCurrent+n;
		for(; vCurrent!=vLegEnd; ++vCurrent)
			*resultCurrent &= *vCurrent;
	}
	return result;
}

size_t sci::count(const std::vector<SBOOL> &v)
{
	if(v.size()==0)
		return 0;
	size_t result=0;
	const SBOOL *vi=&v[0];
	const SBOOL *vEnd=vi+v.size();
	for(; vi<vEnd; ++vi)
		if(*vi)
			++result;
	return result;
}

double sci::dateToUnixTime( int year, int month, int day, int hour, int minute, double second, bool daylightSaving )
{
	tm time;
	time.tm_sec = (int)second;
	time.tm_min = minute;
	time.tm_hour = hour;
	time.tm_mday = day;
	time.tm_mon = month - 1;
	time.tm_year = year - 1900;
	time.tm_isdst = daylightSaving ? 1 : 0;
	return (double)mktime( &time ) - floor( second ) + second;
}

sci::Random::Random(size_t seed)
{
	m_engine.seed(seed);
}

sci::Random::Random()
{
}

size_t sci::Random::getInt()
{
	return m_engine();
}

size_t sci::Random::getMin() const
{
	return m_engine.min();
}

size_t sci::Random::getMax() const
{
	return m_engine.max();
}

sci::RandomInt::RandomInt()
{
}

sci::RandomInt::RandomInt(size_t seed)
	:Random(seed)
{

}

size_t sci::RandomInt::get()
{
	return getInt();
}

sci::RandomReal::RandomReal(size_t seed)
	:Random(seed)
{
}

sci::RandomReal::RandomReal()
	: Random()
{
}

sci::RangedRandomReal::RangedRandomReal(size_t seed, double min, double max)
	:RandomReal(seed)
{
	m_min = min;
	m_max = max;
}

sci::RangedRandomReal::RangedRandomReal(double min, double max)
	:RandomReal()
{
	m_min = min;
	m_max = max;
}

double sci::RandomReal::get()
{
	return double(getInt()) / double(getMax() - getMin());
}

double sci::RangedRandomReal::get()
{
	return RandomReal::get()*(m_max - m_min) + m_min;
}

sci::MultivariateNormalDistribution::MultivariateNormalDistribution(const std::vector<double> &means, const std::vector<double> &standardDeviations)
{
	m_means = means;
	m_standardDeviations = standardDeviations;
	m_determinantCovariance = 1;
	for (size_t i = 0; i < standardDeviations.size(); ++i)
		m_determinantCovariance *= standardDeviations[i] * standardDeviations[i];
}

sci::MultivariateNormalDistribution::MultivariateNormalDistribution(const std::vector<double> &means, const std::vector<std::vector<double>> &covarianceMatrix)
{
	m_means = means;
	m_covarianceMatrix = covarianceMatrix;
	m_determinantCovariance = sci::determinant(covarianceMatrix);
}

double sci::MultivariateNormalDistribution::getProbability(const std::vector<double> &x) const
{
	sci::assertThrow(x.size() == m_means.size(), sci::err(SERR_DISTRIBUTIONS, -999, "sci::MultivariateNormalDistribution::getProbability called with the wrong number of x values."));
	double power;
	if (m_covarianceMatrix.size() > 0)
	{
		power = sci::matrixmult(x - m_means, sci::matrixmult(m_covarianceMatrix, x - m_means));
	}
	else
	{
		power = 0.0;
		for (size_t i = 0; i < x.size(); ++i)
			power += std::pow((x[i] - m_means[i]) / m_standardDeviations[i], 2);
	}

	return 1.0/std::sqrt(std::pow(M_2PI,x.size())*m_determinantCovariance)*std::exp(-0.5*power);
}

sci::NormalDistribution::NormalDistribution(double mean, double standardDeviation)
: MultivariateNormalDistribution(std::vector<double>(1, mean), std::vector<double>(1, standardDeviation))
{
}
double sci::NormalDistribution::getProbability(double x) const
{
	return MultivariateNormalDistribution::getProbability(std::vector<double>(1,x));
}


sci::MultivariateInverseCumulativeNormalDistribution::MultivariateInverseCumulativeNormalDistribution(const std::vector<double> &means, const std::vector<double> &standardDeviations)
{
	m_means = means;
	m_standardDeviations = standardDeviations;
}
double sci::MultivariateInverseCumulativeNormalDistribution::getX(double cumulativeProbability, std::vector<double> otherXs, size_t undefinedIndex) const
{
	double otherProbability = 1.0;
	for (size_t i = 0; i < otherXs.size(); ++i)
		if (i != undefinedIndex)
			otherProbability *= 1.0 + sci::erf((otherXs[i] - m_means[i] / m_standardDeviations[i] / M_SQRT2));
	if (otherProbability < cumulativeProbability)
		return std::numeric_limits<double>::quiet_NaN();
	else
		cumulativeProbability = cumulativeProbability / otherProbability;

	return M_SQRT2 * sci::erfInverse(2.0*cumulativeProbability - 1.0);
}

sci::InverseCumulativeNormalDistribution::InverseCumulativeNormalDistribution(double mean, double standardDeviation)
{
	m_mean = mean;
	m_standardDeviation = standardDeviation;
}

double sci::InverseCumulativeNormalDistribution::getX(double cumulativeProbability) const
{
	return m_mean + m_standardDeviation*M_SQRT2 * sci::erfInverse(2.0*cumulativeProbability - 1.0);
}

std::vector<double> sci::MarkovChain::getExpectation(size_t nIntegrationSteps)
{
	std::vector<double> result = getNext();
	for (size_t i = 1; i < nIntegrationSteps; ++i)
	{
		result += getNext();
	}
	result /= (double)nIntegrationSteps;

	return result;
}

std::vector<std::vector<double>> sci::MarkovChain::getMoments(size_t nIntegrationSteps, std::vector<int> moments)
{
	std::vector<std::vector<double>> samples(nIntegrationSteps);
	samples[0] = getNext();
	std::vector<double> means = samples[0];
	std::vector<double>* samplesBegin = &samples[0];
	std::vector<double>* samplesEnd = samplesBegin + samples.size();
	for (std::vector<double>* samplesi = samplesBegin + 1; samplesi < samplesEnd; ++samplesi)
	{
		*samplesi = getNext();
		means += *samplesi;
	}
	means /= (double)nIntegrationSteps;

	std::vector<std::vector<double>> result(moments.size());
	for (size_t i = 0; i < moments.size(); ++i)
		result[i] = sci::pow(samples[0] - means, moments[i]);
	int *momentsBegin = &moments[0];
	int *momentsEnd = momentsBegin + moments.size();
	std::vector<double>*resultBegin = &result[0];
	for (std::vector<double>* samplesi = samplesBegin + 1; samplesi < samplesEnd; ++samplesi)
	{
		std::vector<double>* resulti = resultBegin;
		for (int* momentsi = momentsBegin; momentsi < momentsEnd; ++momentsi,++resulti)
		{
			*resulti += sci::pow(*samplesi - means, *momentsi);
		}
	}
	result /= double(nIntegrationSteps);
	for (size_t i = 0; i < moments.size(); ++i)
		if (moments[i] == 0)
			result[i] = means;

	return result;
}

double sci::MarkovChain::getUniformRandom(double min, double max)
{
	return RandomReal::get()*(max-min)+min;
}

sci::MetropolisHastingsMarkovChain::MetropolisHastingsMarkovChain(std::vector<double> startPoint, size_t nBurnInIterations, ProbabilityDistributionFunction *probabilityDistributionFunction, std::vector<UnivariateInverseCumulativeDistributionFunction *> jumpDistributions)
{
	sci::assertThrow(startPoint.size() == jumpDistributions.size(), sci::err(SERR_ANALYSIS, -999, "sci::MetropolisHastingsMarkovChain constructor called with startPoint and jumpDistributions of different number of dimensions."));
	m_currentPoint = startPoint;
	m_probabilityDistributionFunction = probabilityDistributionFunction;
	m_jumpDistributions = jumpDistributions;
	m_currentProbability = m_probabilityDistributionFunction->getProbability(m_currentPoint);
	for (size_t i = 0; i < nBurnInIterations; ++i)
		getNext();
	resetAcceptanceRatio();
	m_deleteLocally = false;
}

sci::MetropolisHastingsMarkovChain::MetropolisHastingsMarkovChain(std::vector<double> startPoint, size_t nBurnInIterations, ProbabilityDistributionFunction *probabilityDistributionFunction, std::vector<double> jumpStandardDeviations)
{
	sci::assertThrow(startPoint.size() == jumpStandardDeviations.size(), sci::err(SERR_ANALYSIS, -999, "sci::MetropolisHastingsMarkovChain constructor called with startPoint and jumpDistributions of different number of dimensions."));
	m_currentPoint = startPoint;
	m_probabilityDistributionFunction = probabilityDistributionFunction;
	size_t nextToInitialise = 0;
	m_jumpDistributions.resize(jumpStandardDeviations.size());
	try
	{
		for (size_t i = 0; i < m_jumpDistributions.size(); ++i)
		{
			m_jumpDistributions[i] = new sci::InverseCumulativeNormalDistribution(0.0, jumpStandardDeviations[i]);
			++nextToInitialise;
		}
		m_currentProbability = m_probabilityDistributionFunction->getProbability(m_currentPoint);
		for (size_t i = 0; i < nBurnInIterations; ++i)
			getNext();
		resetAcceptanceRatio();
		m_deleteLocally = true;
	}
	catch (...)
	{
		for (size_t i = 0; i < nextToInitialise; ++i)
			delete m_jumpDistributions[i];
		throw;
	}
}

sci::MetropolisHastingsMarkovChain::~MetropolisHastingsMarkovChain()
{
	if (m_deleteLocally)
		for (size_t i = 0; i < m_jumpDistributions.size(); ++i)
			delete m_jumpDistributions[i];
}

std::vector<double> sci::MetropolisHastingsMarkovChain::getNext()
{
	std::vector<double> xDashes = m_currentPoint;
	for (size_t i = 0; i < xDashes.size(); ++i)
		xDashes[i] += m_jumpDistributions[i]->getX(getUniformRandom(0.0, 1.0));
	double probabilityDash = m_probabilityDistributionFunction->getProbability(xDashes);
	++m_nCalls;

	if (probabilityDash >= m_currentProbability || getUniformRandom(0.0, 1.0) <= probabilityDash / m_currentProbability)
	{
		m_currentProbability = probabilityDash;
		m_currentPoint = xDashes;
		++m_nAccepted;
	}
	return m_currentPoint;
	
}

void sci::MetropolisHastingsMarkovChain::resetAcceptanceRatio()
{
	m_nCalls = 0;
	m_nAccepted = 0;
}
double sci::MetropolisHastingsMarkovChain::getAcceptanceRatio()
{
	return double(m_nAccepted) / double(m_nCalls);
}

sci::GibbsMarkovChain::GibbsMarkovChain(std::vector<double> startPoint, size_t nBurnInIterations, MultivariateInverseCumulativeDistributionFunction * inverseCumulativeDistributionFunction)
{
	m_currentPoint = startPoint;
	m_inverseCumulativeDistributionFunction = inverseCumulativeDistributionFunction;

	for (size_t i = 0; i < nBurnInIterations; ++i)
		getNext();
}

std::vector<double> sci::GibbsMarkovChain::getNext()
{
	for (size_t i = 0; i < m_currentPoint.size(); ++i)
	{
		m_currentPoint[i] = m_inverseCumulativeDistributionFunction->getX(getUniformRandom(0.0, 1.0), m_currentPoint, i);
	}
	return m_currentPoint;
}
