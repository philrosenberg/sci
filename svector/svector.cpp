#include"../include/svector/svector.h"
//#include <alg/ap.h>
//#include <alg/fasttransforms.h>
//#include <alg/interpolation.h>
#include<time.h>
#include"../include/svector/dep/svi.h"
#include"../include/svector/operators.h"
#include"../include/svector/math.h"
#include"../include/svector/string.h"
#include<assert.h>
#include"eigeneng/eigeneng.h"



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

*/

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

/*

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
	//for (size_t i = 0; i < moments.size(); ++i)
	//	result[i] = sci::pow(samples[0] - means, moments[i]);
	//the below for loop should replace the above more compact but deprecated and commented out lines
	for (size_t i = 0; i < result.size(); ++i)
	{
		result[i].resize(samples[0].size());
		for (size_t j = 0; j < result[i].size(); ++j)
			result[i][j] = std::pow(samples[0][j] - means[j], moments[i]);
	}

	int *momentsBegin = &moments[0];
	int *momentsEnd = momentsBegin + moments.size();
	std::vector<double>*resultBegin = &result[0];
	for (std::vector<double>* samplesi = samplesBegin + 1; samplesi < samplesEnd; ++samplesi)
	{
		std::vector<double>* resulti = resultBegin;
		for (int* momentsi = momentsBegin; momentsi < momentsEnd; ++momentsi,++resulti)
		{
			for (size_t j = 0; j < resulti->size(); ++j)
			(*resulti)[j] += sci::pow((*samplesi)[j] - means[j], *momentsi);
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
