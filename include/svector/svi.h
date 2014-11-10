
#ifndef svi_h
#define svi_h

#include"svector.h"
#include<vector>
#include<alg/ap.h>
#include<complex>

namespace sci
{
	class Minimiser;
	void minimiserFunction (const alglib::real_1d_array &fitParams, double &result, void *ptr);
}

//class containing private functions only available to other functions defined here
class svi
{
private:
	//convert between vector<double> and 1d alg array
	static void vectortoalg(const std::vector<double> &src, alglib::real_1d_array &dst);
	static void algtovector(const alglib::real_1d_array &src, std::vector<double> &dst);
	static void vectorstoalgc(const std::vector<double> &re_src, const std::vector<double> &im_src, alglib::complex_1d_array &dst);
	static void algctovectors(const alglib::complex_1d_array &src, std::vector<double> &re_dst, std::vector<double> &im_dst);
	static void vectorctoalgc(const std::vector<std::complex<double>> &src, alglib::complex_1d_array &dst);
	static void algctovectorc(const alglib::complex_1d_array &src, std::vector<std::complex<double>> &dst);

	//convert between vector< vector<double> > and 2d alg array
	static void vectortoalg(const std::vector< std::vector<double> > &src, alglib::real_2d_array &dst);
	static void algtovector(const alglib::real_2d_array &src, std::vector< std::vector<double> > &dst);

	
	//convert between vector< vector<double> > and 2d alg array transposing along the way
	static void transposedvectortoalg(const std::vector< std::vector<double> > &src, alglib::real_2d_array &dst);
	static void transposedalgtovector(const alglib::real_2d_array &src, std::vector< std::vector<double> > &dst);

	
	//friends that need access to these functions
	friend void sci::fft(const std::vector<double> &re_input, std::vector<double> &re_output, std::vector<double> &im_output);
	friend void sci::fitstraightline(const std::vector<double> &x, const std::vector<double> &y, double &grad, double &intercept, double &vargrad, double &varintercept, double &covar);
	friend void sci::fitstraightline(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &weights, double &grad, double &intercept, double &vargrad, double &varintercept, double &covar);
	friend void sci::crosscorr(const std::vector<double> &f, const std::vector<double> &g, std::vector<double> &r, std::vector<long> &fshifts, bool circular);
	friend void minimisablefunction1d (const alglib::real_1d_array &x, double &func, void *ptr);
	friend void minimisablefunction2d (const alglib::real_1d_array &x, double &func, void *ptr);
	friend size_t sci::minimise(std::vector<double> &tunableparams, const std::vector<double> &fixedparams, double (*functiontominimise)(const std::vector<double> &,const std::vector<double> &));
	friend size_t sci::minimise(std::vector<double> &tunableparams, const std::vector<std::vector <double> > &fixedparams, double (*functiontominimise)(const std::vector<double> &,const std::vector< std::vector<double> > &));
	friend size_t sci::fitnonlinear(std::vector<double> &tunableparams, std::vector<double> &paramErrs, std::vector<double> tunableparamlowerlimits, std::vector<double> tunableparamupperlimits, double (*function)(const std::vector<double> &,const std::vector<double> &), const std::vector<std::vector<double>> &xs, const std::vector<double> &ys, const std::vector<double> &weights);
	friend size_t sci::fitnonlinear(std::vector<double> &tunableparams, std::vector<double> &paramErrs, std::vector<double> tunableparamlowerlimits, std::vector<double> tunableparamupperlimits, double (*function)(const std::vector<double> &,double), const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &weights);
	friend std::vector< std::vector<double> > sci::inverse(const std::vector< std::vector<double> > &mat);
	friend void sci::fitpowerlaw(double power, const std::vector<double> &x, const std::vector<double> &y, double &k, double &vark);
	friend void fitfunction1d (const alglib::real_1d_array &fitParams, const alglib::real_1d_array &x, double &func, void *ptr);
	friend void fitfunctionmultid (const alglib::real_1d_array &fitParams, const alglib::real_1d_array &x, double &func, void *ptr);
	friend void sci::eigenvalues(const std::vector<std::vector<double>> &matrix, std::vector<double> &eigenValuesReal, std::vector<double> &eigenValuesImaginary);
	friend void sci::eigenvector(const std::vector<std::vector<double>> &matrix, std::vector<double> &eigenvaluesReal, std::vector<double> &eigenvaluesImaginary, std::vector<std::vector<double>> &eigenvectors);
	friend class sci::Minimiser;
	friend void sci::minimiserFunction (const alglib::real_1d_array &fitParams, double &result, void *ptr);
	 
};


//class algwrapperrealvector : public alglib::real_1d_array
//{


#endif