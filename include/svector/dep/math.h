#ifndef svectormath_h
#define svectormath_h

#include"../svector.h"
#include<cmath>
#include<vector>
#include"operators.h"
namespace sci{

	//********************************************************
	//*************Cmaths and similar functions***************
	//***Templated but will only work with numerical values***
	//********************************************************
	

	//********************************************************
	//******Templates - these can be used where a ************
	//*******function needs to be called on every ************
	//******* element of a 1 or multi d vector ***************
	//********************************************************

	//template for calling a stdfunction on every element of a 1dvector
	//one version returns a new vector the other modifies a provided vector;
	template<class T, class U>
	std::vector<T> stdfuncv(const std::vector<T> &v, T(*stdfunc)(U))
	{
		std::vector<T> result(v.size());
		typename std::vector<T>::iterator resulti=result.begin();
		for(typename std::vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
		{
			*resulti=stdfunc(*vi);
			++resulti;
		}
		return result;
	}
	template<class T, class U>
	void stdfuncv(const std::vector<T> &v, std::vector<T> &result, T(*stdfunc)(U))
	{
		result.resize(v.size());
		typename std::vector<T>::iterator resulti=result.begin();
		for(typename std::vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
		{
			*resulti=stdfunc(*vi);
			++resulti;
		}
	}


	//template for calling a stdfunction on every element of a multi-dvector
	//one version returns a new vector the other modifies a provided vector;
	template<class T, class U>
	std::vector< std::vector<T> > stdfuncv(const std::vector< std::vector<T> > &v,  T(*stdfunc)(U))
	{
		std::vector< std::vector<T> > result(v.size());
		typename std::vector< std::vector<T> >::iterator resulti=result.begin();
		for(typename std::vector< std::vector<T> >::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
		{
			stdfuncv(*vi,*resulti,stdfunc);
			++resulti;
		}
		return result;
	}
	template<class T, class U>
	void stdfuncv(const std::vector< std::vector<T> > &v, std::vector< std::vector<T> > &result,  T(*stdfunc)(U))
	{
		result.resize(v.size());
		typename std::vector< std::vector<T> >::iterator resulti=result.begin();
		for(typename std::vector< std::vector<T> >::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
		{
			*resulti=stdfuncv(*vi,stdfunc);
			++resulti;
		}
	}

	//template for calling a stdfunction on every element of a 1dvector where
	//a second constant value is also passed to the function.
	//one version returns a new vector the other modifies a provided vector.
	template<class T, class U, class V>
	std::vector<T> stdfuncv(const std::vector<T> &v, const U & d, T(*stdfunc)(V,U))
	{
		std::vector<T> result(v.size());
		typename std::vector<T>::iterator resulti=result.begin();
		for(typename std::vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
		{
			*resulti=stdfunc(*vi,d);
			++resulti;
		}
		return result;
	}
	template<class T, class U, class V>
	void stdfuncv(const std::vector<T> &v, const U & d, std::vector<T> &result, T(*stdfunc)(V,U))
	{
		result.resize(v.size());
		typename std::vector<T>::iterator resulti=result.begin();
		for(typename std::vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
		{
			*resulti=stdfunc(*vi,d);
			++resulti;
		}
	}


	//template for calling a stdfunction on every element of a multi-dvector where
	//a second constant value is also passed to the function.
	//one version returns a new vector the other modifies a provided vector.
	template<class T, class U, class V>
	std::vector< std::vector<T> > stdfuncv(const std::vector< std::vector<T> > &v, const U & d,  T(*stdfunc)(V,U))
	{
		std::vector< std::vector<T> > result(v.size());
		typename std::vector< std::vector<T> >::iterator resulti=result.begin();
		for(typename std::vector< std::vector<T> >::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
		{
			stdfuncv(*vi,d,*resulti,stdfunc);
			++resulti;
		}
		return result;
	}
	template<class T, class U, class V>
	void stdfuncv(const std::vector< std::vector<T> > &v, const U & d, std::vector< std::vector<T> > &result,  T(*stdfunc)(V,U))
	{
		result.resize(v.size());
		typename std::vector< std::vector<T> >::iterator resulti=result.begin();
		for(typename std::vector< std::vector<T> >::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
		{
			*resulti=stdfuncv(*vi,d,stdfunc);
			++resulti;
		}
	}


	//******************************************
	//********Cmath Functions*******************
	//*****despit only having 1d templates *****
	//****these will work for multi d using ****
	//****the templates above*******************
	//******************************************


	//compute exponent of each vector element
	template<class T>
	inline std::vector<T> exp(const std::vector<T> &v)
	{
		return stdfuncv(v,&std::exp);
	}
	template<class T>
	inline void exp(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&std::exp);
	}
	template<class T>
	inline T exp(const T &v)
	{
		return std::exp(v);
	}

	//compute natural logarithm of each vector element
	template<class T>
	inline std::vector<T> ln(const std::vector<T> &v)
	{
		return stdfuncv(v,&std::log);
	}
	template<class T>
	inline void ln(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&std::log);
	}
	template<class T>
	inline T ln(const T &v)
	{
		return std::log(v);
	}
	
	//compute base 10 logarithm of each vector element
	template<class T>
	inline std::vector<T> log10(const std::vector<T> &v)
	{
		return stdfuncv(v,&std::log10);
	}
	template<class T>
	inline void log10(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&std::log10);
	}
	template<class T>
	inline T log10(const T &v)
	{
		return std::log10(v);
	}
	
	//compute any base logarithm of each vector element
	template<class T>
	inline std::vector<T> log(const std::vector<T> &v, double base)
	{
		if(base==10.0) return stdfuncv(v,&std::log10);
		return stdfuncv(v,&std::log10)/std::log10(base);
	}
	template<class T>
	inline void log(const std::vector<T> &input, std::vector<T> &output, double base)
	{
		stdfuncv(input,output,&std::log10);
		if(base!=10.0)  output/=std::log10(base);
	}
	template<class T>
	inline void log(const T &input, double base)
	{
		if(base==10.0) return std::log10(input);
		return std::log10(input)/std::log10(base);
	}
	
	//compute absolute value of each vector element
	template<class T>
	inline std::vector<T> abs(const std::vector<T> &v)
	{
		return stdfuncv(v,&std::abs);
	}
	template<class T>
	inline void abs(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&std::abs);
	}
	template<class T>
	inline T abs(const T &v)
	{
		return std::abs(v);
	}
	
	//compute absolute value of each vector element
	template<class T>
	inline std::vector<T> sqrt(const std::vector<T> &v)
	{
		return stdfuncv(v,&std::sqrt);
	}
	template<class T>
	inline void sqrt(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&std::sqrt);
	}
	template<class T>
	inline T sqrt(const T &v)
	{
		return std::sqrt(v);
	}


	//compute sin of each vector element
	template<class T>
	inline std::vector<T> sin(const std::vector<T> &v)
	{
		return stdfuncv(v,&std::sin);
	}
	template<class T>
	inline void sin(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&std::sin);
	}
	template<class T>
	inline T sin(const T &v)
	{
		return std::sin(v);
	}

	//compute cos of each vector element
	template<class T>
	inline std::vector<T> cos(const std::vector<T> &v)
	{
		return stdfuncv(v,&std::cos);
	}
	template<class T>
	inline void cos(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&std::cos);
	}
	template<class T>
	inline T cos(const T &v)
	{
		return std::cos(v);
	}

	//compute tan of each vector element
	template<class T>
	inline std::vector<T> tan(const std::vector<T> &v)
	{
		return stdfuncv(v,&std::tan);
	}
	template<class T>
	inline void tan(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&std::tan);
	}
	template<class T>
	inline T tan(const T &v)
	{
		return std::tan(v);
	}


	//compute asin of each vector element
	template<class T>
	inline std::vector<T> asin(const std::vector<T> &v)
	{
		return stdfuncv(v,&std::asin);
	}
	template<class T>
	inline void asin(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&std::asin);
	}
	template<class T>
	inline T asin(const T &v)
	{
		return std::asin(v);
	}

	//compute acos of each vector element
	template<class T>
	inline std::vector<T> acos(const std::vector<T> &v)
	{
		return stdfuncv(v,&std::acos);
	}
	template<class T>
	inline void acos(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&std::acos);
	}
	template<class T>
	inline T acos(const T &v)
	{
		return std::acos(v);
	}

	//compute atan of each vector element
	template<class T>
	inline std::vector<T> atan(const std::vector<T> &v)
	{
		return stdfuncv(v,&std::atan);
	}
	template<class T>
	inline void atan(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&std::atan);
	}
	template<class T>
	inline T atan(const T &v)
	{
		return std::atan(v);
	}

	//compute erf of each vector element
	template<class T>
	inline std::vector<T> erf(const std::vector<T> &v)
	{
		return stdfuncv(v,&sci::erf);
	}
	template<class T>
	inline void erf(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&sci::erf);
	}

	//compute atan of each vector element given the x and y components, this overcomes the sign ambiguity
	template<class T>
	inline std::vector<T> atan2(const std::vector<T> &x, const std::vector<T> &y)
	{
		size_t size=std::max(x.size(),y.size());
		std::vector<T> result(size,std::numeric_limits<T>::quiet_NaN());
		size=std::min(x.size(),y.size());
		if(size==0) return result;
		const T *xi=&x[0];
		const T *yi=&y[0];
		T* resulti=&result[0];
		T*resultend=resulti+size;
		for(; resulti<resultend; ++resulti)
		{
			*resulti=std::atan2(*yi,*xi);
			++xi;
			++yi;
		}
		return result;
	}

	//compute atan of each vector element given the x and y components, this overcomes the sign ambiguity
	template<class T>
	inline std::vector<std::vector<T>> atan2(const std::vector<std::vector<T>> &x, const std::vector<std::vector<T>> &y)
	{
		size_t size=std::max(x.size(),y.size());
		std::vector<std::vector<T>> result(size);
		size=std::min(x.size(),y.size());
		if(size==0) return result;
		const std::vector<T> *xi=&x[0];
		const std::vector<T> *yi=&y[0];
		std::vector<T> *resulti=&result[0];
		std::vector<T> *resultend=resulti+size;

		for(; resulti<resultend; ++resulti)
		{
			*resulti=atan2(*yi,*xi);
			++xi;
			++yi;

		}
		return result;
	}

	//compute atan of each vector element given the x and y components, this overcomes the sign ambiguity
	//return the results in the positive domain only
	template<class T>
	inline std::vector<T> atan2pos(const std::vector<T> &x, const std::vector<T> &y)
	{
		size_t size=std::max(x.size(),y.size());
		std::vector<T> result(size,std::numeric_limits<T>::quiet_NaN());
		size=std::min(x.size(),y.size());
		if(size==0) return result;
		const T *xi=&x[0];
		const T *yi=&y[0];
		T* resulti=&result[0];
		T*resultend=resulti+size;
		for(; resulti<resultend; ++resulti)
		{
			*resulti=std::atan2(*yi,*xi);
			if(*resulti<0.0) *resulti+=2.0*M_PI;
			++xi;
			++yi;

		}
		return result;
	}

	//compute atan of each vector element given the x and y components, this overcomes the sign ambiguity
	//return the results in the positive domain only
	template<class T>
	inline std::vector<std::vector<T>> atan2pos(const std::vector<std::vector<T>> &x, const std::vector<std::vector<T>> &y)
	{
		size_t size=std::max(x.size(),y.size());
		std::vector<std::vector<T>> result(size);
		size=std::min(x.size(),y.size());
		if(size==0) return result;
		const std::vector<T> *xi=&x[0];
		const std::vector<T> *yi=&y[0];
		std::vector<T> *resulti=&result[0];
		std::vector<T> *resultend=resulti+size;

		for(; resulti<resultend; ++resulti)
		{
			*resulti=atan2pos(*yi,*xi);
			++xi;
			++yi;

		}
		return result;
	}
	template<class T>
	inline T atan2(const T &x, const T &y)
	{
		return std::atan2(y,x);
	}
	//**************************************************
	//********************pow***************************
	//*******This is a special case as it requires******
	//*******two parameters maybe of different types****
	//****with the second or first a scalar or vector***
	//**************************************************

	//power of a vector to a non vector
	template<class T, class U>
	inline std::vector<T> pow(const std::vector<T> &v, const U &power)
	{
		std::vector<T> result(v.size());
		typename std::vector<T>::iterator resulti=result.begin();
		for(typename std::vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
		{
			*resulti=std::pow(*vi,power);
			++resulti;
		}
		return result;
	}

	//power of a vector to a vector
	template<class T, class U>
	inline std::vector<T> pow(const std::vector<T> &v, const std::vector<U> &power)
	{
		std::vector<T> result(v.size());
		typename std::vector<T>::iterator resulti=result.begin();
		typename std::vector<T>::const_iterator poweri=power.begin();
		for(typename std::vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
		{
			*resulti=std::pow(*vi,*poweri);
			++resulti;
			++poweri;
		}
		return result;
	}

	//power of a vector to a non vector when the result is provided by reference
	template<class T, class U>
	inline void pow(const std::vector<T> &v, const U &power, std::vector<T> &result)
	{
		result.resize(v.size());
		typename std::vector<T>::iterator resulti=result.begin();
		for(typename std::vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
		{
			*resulti=std::pow(*vi,power);
			++resulti;
		}
	}

	//power of a vector to a vector when the result is provided by reference
	template<class T, class U>
	inline void pow(const std::vector<T> &v, const std::vector<U> &power, std::vector<T> &result)
	{
		result.resize(v.size());
		typename std::vector<T>::iterator resulti=result.begin();
		typename std::vector<T>::const_iterator poweri=power.begin();
		for(typename std::vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
		{
			*resulti=std::pow(*vi,*poweri);
			++resulti;
			++poweri;
		}
	}

	//power of a multi dimensional vector to a non vector
	template<class T, class U>
	inline std::vector< std::vector<T> > pow(const std::vector< std::vector<T> > &v, const U &power)
	{
		std::vector< std::vector<T> > result(v.size());
		typename std::vector< std::vector<T> >::iterator resulti=result.begin();
		for(typename std::vector< std::vector<T> >::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
		{
			pow(*vi,power,*resulti);
			++resulti;
		}
		return result;
	}

	//power of a multi dimensional vector to a non vector where the result is provided as a reference
	template<class T, class U>
	inline void pow(const std::vector< std::vector<T> > &v, const U &power, std::vector< std::vector<T> > &result)
	{
		result.resize(v.size());
		typename std::vector< std::vector<T> >::iterator resulti=result.begin();
		for(typename std::vector< std::vector<T> >::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
		{
			pow(*vi,power,*resulti);
			++resulti;
		}
	}

	//power of a multi dimensional vector to a multi dimensional vector
	template<class T, class U>
	inline std::vector< std::vector<T> > pow(const std::vector< std::vector<T> > &v, const std::vector< std::vector<U> > &power)
	{
		std::vector< std::vector<T> > result(v.size());
		typename std::vector< std::vector<T> >::iterator resulti=result.begin();
		typename std::vector< std::vector<T> >::const_iterator poweri=power.begin();
		for(typename std::vector< std::vector<T> >::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
		{
			pow(*vi,*poweri,*resulti);
			++resulti;
			++poweri;
		}
		return result;
	}

	//power of a multi dimensional vector to a multi dimensional vector where the result is provided as a reference
	template<class T, class U>
	inline void pow(const std::vector< std::vector<T> > &v, const std::vector< std::vector<U> > &power, std::vector< std::vector<T> > &result)
	{
		result.resize(v.size());
		typename std::vector< std::vector<T> >::iterator resulti=result.begin();
		typename std::vector< std::vector<T> >::const_iterator poweri=power.begin();
		for(typename std::vector< std::vector<T> >::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
		{
			pow(*vi,*poweri,*resulti);
			++resulti;
			++poweri;
		}
	}

	//power of a non vector to a vector
	template<class T>
	inline std::vector<T> pow(const T &base, const std::vector<T> &power)
	{
		std::vector<T> result(power.size());
		typename std::vector<T>::iterator resulti=result.begin();
		for(typename std::vector<T>::const_iterator poweri=power.begin(); poweri!=power.end(); ++poweri) 
		{
			*resulti=std::pow(base,*poweri);
			++resulti;
		}
		return result;
	}

	//power of a non vector to a vector where result is provided as a reference
	template<class T, class U, class V>
	inline void pow(const U &base, const std::vector<T> &power, const std::vector<V> &result)
	{
		typename std::vector<V>::iterator resulti=result.begin();
		for(typename std::vector<T>::const_iterator poweri=power.begin(); poweri!=power.end(); ++poweri) 
		{
			*resulti=std::pow(base,*poweri);
			++resulti;
		}
		return result;
	}

	//power of a non vector to a multi dimensional vector
	template<class T, class U>
	inline std::vector< std::vector<U> > pow(const U &base, const std::vector< std::vector<T> > &power)
	{
		std::vector< std::vector<U> > result(power.size());
		typename std::vector<U>::iterator resulti=result.begin();
		for(typename std::vector<T>::const_iterator poweri=power.begin(); poweri!=power.end(); ++poweri) 
		{
			sci::pow(base,*poweri,*resulti);
			++resulti;
		}
		return result;
	}
		
	//power of a non vector to a multi dimensional vector where the vector is passed by reference
	template<class T, class U, class V>
	inline void pow(const U &base, const std::vector< std::vector<T> > &power, std::vector< std::vector<V> > result)
	{
		typename std::vector<V>::iterator resulti=result.begin();
		for(typename std::vector<T>::const_iterator poweri=power.begin(); poweri!=power.end(); ++poweri) 
		{
			sci::pow(base,*poweri,*resulti);
			++resulti;
		}
		return result;
	}

	//compute sinh of each vector element
	template<class T>
	inline std::vector<T> sinh(const std::vector<T> &v)
	{
		return stdfuncv(v,&std::sinh);
	}
	template<class T>
	inline void sinh(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&std::sinh);
	}
	template<class T>
	inline T sinh(const T &v)
	{
		return std::sinh(v);
	}

	//compute cosh of each vector element
	template<class T>
	inline std::vector<T> cosh(const std::vector<T> &v)
	{
		return stdfuncv(v,&std::cosh);
	}
	template<class T>
	inline void cosh(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&std::cosh);
	}
	template<class T>
	inline T cosh(const T &v)
	{
		return std::cosh(v);
	}

	//compute tanh of each vector element
	template<class T>
	inline std::vector<T> tanh(const std::vector<T> &v)
	{
		return stdfuncv(v,&std::tanh);
	}
	template<class T>
	inline void tanh(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&std::tanh);
	}
	template<class T>
	inline T tanh(const T &v)
	{
		return std::tanh(v);
	}

	//inverse hyperbolic functions - not implemented until C++11
	inline double asinh(double x)
	{
		return std::log(x+std::sqrt(x*x+1.0));
	}
	inline float asinh(float x)
	{
		return std::log(x+std::sqrt(x*x+1.0f));
	}
	inline long double asinh(long double x)
	{
		return std::log(x+std::sqrt(x*x+1.0L));
	}
	inline double acosh(double x)
	{
		return std::log(x+std::sqrt(x+1.0)*std::sqrt(x-1.0));
	}
	inline float acosh(float x)
	{
		return std::log(x+std::sqrt(x+1.0f)*std::sqrt(x-1.0f));
	}
	inline long double acosh(long double x)
	{
		return std::log(x+std::sqrt(x+1.0L)*std::sqrt(x-1.0L));
	}
	inline double atanh(double x)
	{
		return 0.5*std::log((1.0+x)/(1.0-x));
	}
	inline float atanh(float x)
	{
		return 0.5f*std::log((1.0f+x)/(1.0f-x));
	}
	inline long double atanh(long double x)
	{
		return 0.5L*std::log((1.0L+x)/(1.0L-x));
	}
	//compute asinh of each vector element
	template<class T>
	inline std::vector<T> asinh(const std::vector<T> &v)
	{
		return stdfuncv(v,&sci::asinh);
	}
	template<class T>
	inline void asinh(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&sci::asinh);
	}

	//compute acosh of each vector element
	template<class T>
	inline std::vector<T> acosh(const std::vector<T> &v)
	{
		return stdfuncv(v,&sci::acosh);
	}
	template<class T>
	inline void acosh(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&sci::acosh);
	}

	//compute atanh of each vector element
	template<class T>
	inline std::vector<T> atanh(const std::vector<T> &v)
	{
		return stdfuncv(v,&sci::atanh);
	}
	template<class T>
	inline void atanh(const std::vector<T> &input, std::vector<T> &output)
	{
		stdfuncv(input,output,&sci::atanh);
	}

	double erf(double x);

}
#endif
