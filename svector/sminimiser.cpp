#include"sminimiser_internal.h"
#include"serr_internal.h"
#include"svector_internal.h"
#include"operators_internal.h"
#include"svi_internal.h"
#include <alg/interpolation.h>


void sci::minimiserFunction (const alglib::real_1d_array &fitParams, double &result, void *ptr)
{
	Minimiser *minimiser=(Minimiser*)ptr;
	std::vector<double> xsV;
	svi::algtovector(fitParams, xsV);
	minimiser->setXsWithoutSettingScales(xsV);
	result = minimiser->getValue();
}

void sci::Minimiser::minimise()
{
	sci::assertThrow(m_xs.size()>0, sci::err(sci::SERR_MINIMISER, -1, "Minimiser::minimise called but with no x data available (xs size is zero)."));

	//create tunable params
	alglib::real_1d_array algtunableparams;
	svi::vectortoalg(m_xs,algtunableparams);

	//create a state object that keeps track of the fitting
	alglib::mincgstate state;
	//alglib::mincgcreate(algtunableparams, state); //use this if we supply a function that reports the gradient
	alglib::mincgcreatef(algtunableparams,1.0e-6, state); //use this is if we only suply a function that reports values
	
	alglib::real_1d_array scales;
	svi::vectortoalg(m_scales,scales);
	alglib::mincgsetscale(state,scales);
	//set stopping condition,
	//	first number is gradient acceptance condition
	//	second is deltafunc/max(func,1) acceptance condition
	//	third number is deltaparams/params acceptance condition
	//	forth number is max itterations acceptance condition
	//setting all four to zero uses a small vale of deltaparams/params
	alglib::mincgsetcond(state, 0, 0, 0, 0);

	//do the minimising
	alglib::mincgoptimize(state, &minimiserFunction,NULL,this);

	//get the results
	alglib::mincgreport report;
	alglib::mincgresults(state, algtunableparams, report);

	svi::algtovector(algtunableparams,m_xs);

	//return the number of iterations
	m_nIterations=report.iterationscount;
}

std::vector<double> sci::Minimiser::getXs()
{
	return m_xs;
}

std::vector<double> sci::Minimiser::getXUncertainties()
{
	return m_xErrors;
}

void sci::Minimiser::setXs(const std::vector<double> &xs)
{
	sci::assertThrow(xs.size()>0,sci::err(SERR_MINIMISER, -1, "sci::Minimiser::setXs called with zero elements in xs."));
	m_xs=xs;

	//set the scale of the variables. Helps algorithm converge. Should
	//be about the magnitude of the variables or the magnitude of how much
	//they will change
	//we just set the scale as the initial guess or 1.0 if the initial guess is 0.0
	//We should allow this to be modified by the user as the magnitude of how much they
	//will change may not be of the same order as the guess -e.g. a guess might be 0.0
	//but we might expect the result to be in the range +/- 100 so the scale should be 100
	m_scales=xs;
	m_scales==m_scales;
	//sci::assign(m_scales, m_scales==0.0, 1.0);
}

void sci::Minimiser::setXs(const std::vector<double> &xs, const std::vector<double> &scales)
{
	sci::assertThrow(xs.size()>0,sci::err(SERR_MINIMISER, -1, "sci::Minimiser::setXs called with zero elements in xs."));
	sci::assertThrow(xs.size()==scales.size(), sci::err(SERR_MINIMISER, -1, "sci::Minimiser::setXs called with a different number of elements in xs and scales."));
	m_xs=xs;
	//set the scale of the variables. Helps algorithm converge. Should
	//be about the magnitude of the variables or the magnitude of how much
	//they will change
	//sci::assertThrow(sci::allFalse(scales==std::vector<double>(scales.size(),0.0)),sci::err());
}

void sci::Minimiser::setXsWithoutSettingScales(const std::vector<double> &xs)
{
	sci::assertThrow(xs.size()>0,sci::err(SERR_MINIMISER, -1, "sci::Minimiser::setXsWithoutSettingScales called with zero elements in xs."));
	m_xs=xs;
}

size_t sci::Minimiser::getNIterations()
{
	return m_nIterations;
}

sci::Fitter::FitMinimiser::FitMinimiser(Fitter *fitter)
{
	m_fitter=fitter;
}

double sci::Fitter::FitMinimiser::getValue()
{
	return m_fitter->getValueToMinimise();
}


sci::Fitter::Fitter()
	:m_minimiser(this), m_fitParameters(m_minimiser.m_xs)
{
}

void sci::Fitter::setFitParams(const std::vector<double> &fitParams)
{
	m_minimiser.setXs(fitParams);
}

void sci::Fitter::setFitParams(const std::vector<double> &fitParams, const std::vector<double> &scales)
{
	m_minimiser.setXs(fitParams, scales);
}

std::vector<double> sci::Fitter::getFitParams()
{
	return m_minimiser.getXs();
}

std::vector<double> sci::Fitter::getFitParamUncertainties()
{
	return m_minimiser.getXUncertainties();
}

sci::Fitter1d::Fitter1d()
{
}
sci::Fitter1d::Fitter1d(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> fittingParams, const std::vector<double> scales)
{
	setData(xs, ys);
	if(scales.size()==0)
		setFitParams(fittingParams);
	else
		setFitParams(fittingParams,scales);
}
void sci::Fitter1d::setData(const std::vector<double> &xs, const std::vector<double> &ys)
{
	sci::assertThrow(xs.size()>0,sci::err(SERR_MINIMISER, -1, "sci::Fitter1d::setData called with zero elements in xs."));
	sci::assertThrow(xs.size()==ys.size(),sci::err(SERR_MINIMISER, -1, "sci::Fitter1d::setData called with a different number of elements in xs and ys."));
	m_xs=xs;
	m_ys=ys;
}
double sci::Fitter1d::getSumSquaredErrors()
{
	double sum=0.0;
	double *xi=&m_xs[0];
	double *xEnd=xi+m_xs.size();
	double *yi=&m_ys[0];

	for(;xi<xEnd;++xi,++yi)
	{
		sum+=std::pow(getValue(*xi)-*yi,2);
	}
	return sum;
}
double sci::Fitter1d::getValueToMinimise()
{
	return getSumSquaredErrors();
}

sci::Fitter1dWeighted::Fitter1dWeighted(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &weights, const std::vector<double> fittingParams, const std::vector<double> scales)
{
	setData(xs, ys, weights);
	if(scales.size()==0)
		setFitParams(fittingParams);
	else
		setFitParams(fittingParams,scales);
}
void sci::Fitter1dWeighted::setData(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &weights)
{
	Fitter1d::setData(xs, ys);
	sci::assertThrow(xs.size()==weights.size(),sci::err(SERR_MINIMISER, -1, "sci::Fitter1dWeighted::setXs called with a different number of elements in xs and weights."));
	m_weights=weights;
}
double sci::Fitter1dWeighted::getWeightedSumSquaredErrors()
{
	double sum=0.0;
	double *xi=&m_xs[0];
	double *xEnd=xi+m_xs.size();
	double *yi=&m_ys[0];
	double *wi=&m_weights[0];

	for(;xi<xEnd;++xi,++yi,++wi)
	{
		sum+=std::pow(value(*xi)-*yi,2)**wi;
	}
	return sum;
}
double sci::Fitter1dWeighted::getValueToMinimise()
{
	return getWeightedSumSquaredErrors();
}



sci::Fitter1dXYErrors::Fitter1dXYErrors(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &xErrors, const std::vector<double> &yErrors, const std::vector<double> fittingParams, const std::vector<double> scales)
{
	setData(xs, ys, xErrors, yErrors);
	if(scales.size()==0)
		setFitParams(fittingParams);
	else
		setFitParams(fittingParams,scales);
}
void sci::Fitter1dXYErrors::setData(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &xErrors, const std::vector<double> &yErrors)
{
	Fitter1d::setData(xs, ys);
	sci::assertThrow(xs.size() == xErrors.size(), sci::err(SERR_MINIMISER, -1, "sci::Fitter1dXYErrors::setData called with a different number of elements in xs and xErrors."));
	sci::assertThrow(ys.size() == yErrors.size(), sci::err(SERR_MINIMISER, -1, "sci::Fitter1dXYErrors::setData called with a different number of elements in ys and yErrors."));
	m_xErrors=xErrors;
	m_yErrors=yErrors;
}
double sci::Fitter1dXYErrors::getSolutionProbability()
{
	//here we have to resort to a Bayesian method - it is pretty tricky to do this
	double *xi=&m_xs[0];
	double *xEnd=xi+m_xs.size();
	double *yi=&m_ys[0];
	double *xErrori=&m_xErrors[0];
	double *yErrori=&m_yErrors[0];

	//assume probability of getting value X, Y, when y=f(x) is proportional to 1/(x-X)^2+1/(y-Y)^2

	//sum this over +/-3 error bars in x

	double nErrorBars=3.0;
	size_t CalcsPerPoint=19; //this gives 3 per error bar plus one in the middle

	double probablilityTotal=1.0;

	for(; xi<xEnd; ++xi,++yi,++xErrori,++yErrori)
	{
		double xInterval= *xErrori*2.0/(double)(CalcsPerPoint-1); //distance in x direction for this section of the line
		double xMid=*xi-nErrorBars**xErrori;
		double probabilityPoint=0.0;
		double yBottom; //y value at the low x end of this section of the line
		double yTop=value(*xi-0.5*xInterval); //y value at the high x end of the line - note -0.5*xInterval as it gets swapped in next loop
		for(size_t i=0; i<CalcsPerPoint; ++i)
		{
			double xOffset=*xi-xMid; //distance in x direction from the centre of the pdf to the centre of this section of the line
			yBottom=yTop;
			yTop=value(xMid+0.5*xInterval);
			double yInterval=yTop-yBottom; //distance in y direction for this section of the line
			double yMid=value(xMid);
			double yOffset=*yi-yMid; //distance in y direction from the centre of the pdf to the centre of this section of the line
			//calculate the probability  of measureing the point xi, yi, when the function is at xi-xOffset,yi-yOffset
			// we normalise by the error bar width so that we are in space where sigma is 1 in both dimensions
			// This gives us  dimensionless values in the 
			double xOffsetNormalised=xOffset/(*xErrori);
			double yOffsetNormalised=yOffset/(*yErrori);
			double probabilityInterval=std::exp(-0.5*(std::pow(xOffsetNormalised,2)+std::pow(yOffsetNormalised,2)));
			//now integrate along our line section by multiplying by its length - note the importance of normalising 
			//wrt the error bars which gives us unitless values in the square root which can be added
			probabilityPoint+=probabilityInterval*std::sqrt(xInterval*xInterval/(*xErrori)/(*xErrori) +
				                                                       yInterval*yInterval/(*yErrori)/(*yErrori));

			//move to the next mid
			xMid+=xInterval;
		}
		probablilityTotal*=probabilityPoint;
	}

	//finally normalise the probabilities with the required factors of 1/sqrt(2PI)
	return probablilityTotal*std::pow(0.5/M_PI,m_xs.size()/2.0);
}

double sci::Fitter1dXYErrors::getValueToMinimise()
{
	return -getSolutionProbability();
}

double func1(double x, double p1)
{
	return x;
}
double func2(double x, double p1, double p2)
{
	return x;
}
double func3(double x, double p1, double p2, double p3)
{
	return x;
}
double func4(double x, double p1, double p2, double p3, double p4)
{
	return x;
}

sci::Fitter1DOneParameterFunction<func1> test1(std::vector<double>(1),std::vector<double>(1),0.0);
sci::Fitter1DTwoParameterFunction<func2> test2(std::vector<double>(1),std::vector<double>(1),0.0,0.0);
sci::Fitter1DThreeParameterFunction<func3> test3(std::vector<double>(1),std::vector<double>(1),0.0,0.0,0.0);
sci::Fitter1DFourParameterFunction<func4> test4(std::vector<double>(1),std::vector<double>(1),0.0,0.0,0.0,0.0);
