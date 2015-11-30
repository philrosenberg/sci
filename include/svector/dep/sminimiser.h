
#ifndef SMINIMISER_H
#define SMINIMISER_H

#include<vector>
#include<stddef.h>
namespace alglib
{
	class real_1d_array;
}


namespace sci
{
	class Minimiser
	{
		friend void minimiserFunction (const alglib::real_1d_array &fitParams, double &result, void *ptr);
	public:
		virtual double getValue()=0;
		void setXs(const std::vector<double> &xs);
		void setXs(const std::vector<double> &xs, const std::vector<double> &scales);
		void minimise();
		std::vector<double> getXs();
		std::vector<double> getXUncertainties();
		size_t getNIterations();
	protected:
		void setXsWithoutSettingScales(const std::vector<double> &xs);
		std::vector<double> m_xs;
		std::vector<double> m_xErrors;
	private:
		size_t m_nIterations;
		std::vector<double> m_scales;
	};


	class Fitter
	{
	private:
		friend class FitMinimiser;
		class FitMinimiser :public Minimiser
		{
		private:
			friend class Fitter;
			FitMinimiser(Fitter *fitter);
			FitMinimiser & operator=(const FitMinimiser &rhs);
			double getValue();
			Fitter *m_fitter;
		};
	public:
		Fitter();
		void setFitParams(const std::vector<double> &fitParams);
		void setFitParams(const std::vector<double> &fitParams, const std::vector<double> &scales);
		std::vector<double> getFitParams();
		std::vector<double> getFitParamUncertainties();
		void fit();
	private:
		Fitter::FitMinimiser m_minimiser;
		virtual double getValueToMinimise()=0;
	protected:
		std::vector<double> &m_fitParameters;
	};

	class Fitter1d : public Fitter
	{
	public:
		Fitter1d();
		Fitter1d(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> fittingParams, const std::vector<double> scales=std::vector<double>(0));
		virtual double getValue(double x)=0;
		void setData(const std::vector<double> &xs, const std::vector<double> &ys);
		double getSumSquaredErrors();
		virtual double getValueToMinimise();
	protected:
		std::vector<double> m_xs;
		std::vector<double> m_ys;
	};

	class Fitter1dWeighted : public Fitter1d
	{
	public:
		Fitter1dWeighted();
		Fitter1dWeighted(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &weights, const std::vector<double> fittingParams, const std::vector<double> scales=std::vector<double>(0));
		virtual double value(double x)=0;
		void setData(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &weights);
		virtual double getWeightedSumSquaredErrors();
	protected:
		std::vector<double> m_weights;
	private:
		virtual double getValueToMinimise();
	};

	class Fitter1dXYErrors : public Fitter1d
	{
	public:
		Fitter1dXYErrors();
		Fitter1dXYErrors(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &xErrors, const std::vector<double> &yErrors, const std::vector<double> fittingParams, const std::vector<double> scales=std::vector<double>(0));
		virtual double value(double x)=0;
		void setData(const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &xErrors, const std::vector<double> &yErrors);
		virtual double getSolutionProbability();
	protected:
		std::vector<double> m_xErrors;
		std::vector<double> m_yErrors;
	private:
		virtual double getValueToMinimise();
	};


	//These are shortcuts for fitting to functions with 1-4 fitting parameters.
	//Instead of inheriting from a Fitter and overloading the getValue function
	//you can pass a free function as a template parameter
	//e.g. Fitter1DTwoParameterFunction<sci::distribution::lognormal> lognormalFitter

	//one parameter fitter
	template < double (*FUNC)(double, double) >
	class Fitter1DOneParameterFunction : public Fitter1d
	{
	public:
		Fitter1DOneParameterFunction(const std::vector<double> &xs, const std::vector<double> &ys, double fittingParameter);
		Fitter1DOneParameterFunction(const std::vector<double> &xs, const std::vector<double> &ys, double fittingParameter, double scale);
		double getValue(double x);
	};
	template < double (*FUNC)(double, double) >
	Fitter1DOneParameterFunction< FUNC >::Fitter1DOneParameterFunction(const std::vector<double> &xs, const std::vector<double> &ys, double fittingParameter)
	{
		setData(xs, ys);
		setFitParams(std::vector<double>(1,fittingParameter));
	}
	template < double (*FUNC)(double, double) >
	Fitter1DOneParameterFunction< FUNC >::Fitter1DOneParameterFunction(const std::vector<double> &xs, const std::vector<double> &ys, double fittingParameter, double scale)
	{
		setData(xs, ys);
		setFitParams(std::vector<double>(1,fittingParameter),std::vector<double>(1,scale));
	}
	template < double (*FUNC)(double, double) >
	double Fitter1DOneParameterFunction< FUNC >::getValue(double x)
	{
		return FUNC(x, m_fitParameters[0]);
	}

	//two parameter fitter
	template < double (*FUNC)(double, double, double) >
	class Fitter1DTwoParameterFunction : public Fitter1d
	{
	public:
		Fitter1DTwoParameterFunction(const std::vector<double> &xs, const std::vector<double> &ys, double fittingParameter1, double fittingParameter2);
		Fitter1DTwoParameterFunction(const std::vector<double> &xs, const std::vector<double> &ys, double fittingParameter1, double fittingParameter2, double scale1, double scale2);
		double getValue(double x);
	};
	template < double (*FUNC)(double, double, double) >
	Fitter1DTwoParameterFunction< FUNC >::Fitter1DTwoParameterFunction(const std::vector<double> &xs, const std::vector<double> &ys, double fittingParameter1, double fittingParameter2)
	{
		setData(xs, ys);
		std::vector<double> params(2);
		params[0]=fittingParameter1;
		params[1]=fittingParameter2;
		setFitParams(params);
	}
	template < double (*FUNC)(double, double, double) >
	Fitter1DTwoParameterFunction< FUNC >::Fitter1DTwoParameterFunction(const std::vector<double> &xs, const std::vector<double> &ys, double fittingParameter1, double fittingParameter2, double scale1, double scale2)
	{
		setData(xs, ys);
		std::vector<double> params(2);
		params[0]=fittingParameter1;
		params[1]=fittingParameter2;
		std::vector<double> scales(2);
		scales[0]=scale1;
		scales[1]=scale2;
		setFitParams(params, scales);
	}
	template < double (*FUNC)(double, double, double) >
	double Fitter1DTwoParameterFunction< FUNC >::getValue(double x)
	{
		return FUNC(x, m_fitParameters[0], m_fitParameters[1]);
	}

	//three parameter fitter
	template < double (*FUNC)(double, double, double, double) >
	class Fitter1DThreeParameterFunction : public Fitter1d
	{
	public:
		Fitter1DThreeParameterFunction(const std::vector<double> &xs, const std::vector<double> &ys, double fittingParameter1, double fittingParameter2, double fittingParameter3);
		Fitter1DThreeParameterFunction(const std::vector<double> &xs, const std::vector<double> &ys, double fittingParameter1, double fittingParameter2, double fittingParameter3, double scale1, double scale2, double scale3);
		double getValue(double x);
	};
	template < double (*FUNC)(double, double, double, double) >
	Fitter1DThreeParameterFunction< FUNC >::Fitter1DThreeParameterFunction(const std::vector<double> &xs, const std::vector<double> &ys, double fittingParameter1, double fittingParameter2, double fittingParameter3)
	{
		setData(xs, ys);
		std::vector<double> params(3);
		params[0]=fittingParameter1;
		params[1]=fittingParameter2;
		params[2]=fittingParameter3;
		setFitParams(params);
	}
	template < double (*FUNC)(double, double, double, double) >
	Fitter1DThreeParameterFunction< FUNC >::Fitter1DThreeParameterFunction(const std::vector<double> &xs, const std::vector<double> &ys, double fittingParameter1, double fittingParameter2, double fittingParameter3, double scale1, double scale2, double scale3)
	{
		setData(xs, ys);
		std::vector<double> params(3);
		params[0]=fittingParameter1;
		params[1]=fittingParameter2;
		params[2]=fittingParameter3;
		std::vector<double> scales(3);
		scales[0]=scale1;
		scales[1]=scale2;
		scales[2]=scale3;
		setFitParams(params, scales);
	}
	template < double (*FUNC)(double, double, double, double) >
	double Fitter1DThreeParameterFunction< FUNC >::getValue(double x)
	{
		return FUNC(x, m_fitParameters[0], m_fitParameters[1], m_fitParameters[2]);
	}

	//four parameter fitter
	template < double (*FUNC)(double, double, double, double, double) >
	class Fitter1DFourParameterFunction : public Fitter1d
	{
	public:
		Fitter1DFourParameterFunction(const std::vector<double> &xs, const std::vector<double> &ys, double fittingParameter1, double fittingParameter2, double fittingParameter3, double fittingParameter4);
		Fitter1DFourParameterFunction(const std::vector<double> &xs, const std::vector<double> &ys, double fittingParameter1, double fittingParameter2, double fittingParameter3, double fittingParameter4, double scale1, double scale2, double scale3, double scale4);
		double getValue(double x);
	};
	template < double (*FUNC)(double, double, double, double, double) >
	Fitter1DFourParameterFunction< FUNC >::Fitter1DFourParameterFunction(const std::vector<double> &xs, const std::vector<double> &ys, double fittingParameter1, double fittingParameter2, double fittingParameter3, double fittingParameter4)
	{
		setData(xs, ys);
		std::vector<double> params(4);
		params[0]=fittingParameter1;
		params[1]=fittingParameter2;
		params[2]=fittingParameter3;
		params[3]=fittingParameter4;
		setFitParams(params);
	}
	template < double (*FUNC)(double, double, double, double, double) >
	Fitter1DFourParameterFunction< FUNC >::Fitter1DFourParameterFunction(const std::vector<double> &xs, const std::vector<double> &ys, double fittingParameter1, double fittingParameter2, double fittingParameter3, double fittingParameter4, double scale1, double scale2, double scale3, double scale4)
	{
		setData(xs, ys);
		std::vector<double> params(4);
		params[0]=fittingParameter1;
		params[1]=fittingParameter2;
		params[2]=fittingParameter3;
		params[3]=fittingParameter4;
		std::vector<double> scales(4);
		scales[0]=scale1;
		scales[1]=scale2;
		scales[2]=scale3;
		scales[3]=scale4;
		setFitParams(params, scales);
	}
	template < double (*FUNC)(double, double, double, double, double) >
	double Fitter1DFourParameterFunction< FUNC >::getValue(double x)
	{
		return FUNC(x, m_fitParameters[0], m_fitParameters[1], m_fitParameters[2], m_fitParameters[3]);
	}
}

#endif
