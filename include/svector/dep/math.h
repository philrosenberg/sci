#ifndef svectormath_h
#define svectormath_h

#include<cmath>
#include<vector>
#include<utility>
#include<algorithm>
#include<limits>

namespace sci_internal
{
	//a class which holds a pair of iterators for use when you wish to use two vectors
	//within a std::for_each, e.g. to give a source and a destination. Note that T
	//and U are iterator types, not value types.
	//It is a bit of a cheat of an iterator, because the dereference operator doesn't
	//actually dereference anything, it just returns a reference to the iterator. The
	//user must dereference the result of first() or second() instead. However this
	//model allows IteratorPair to be used with std::for_each - in this case the functor
	//must have a reference to the IteratorPair as its argument.
	template< class T, class U>
	class IteratorPair
	{
	public:
		IteratorPair( T iter1, U iter2 ) : m_pair( iter1, iter2 ){}
		T &first() { return m_pair.first; }
		U &second() { return m_pair.second; }
		T &first() const { return m_pair.first; }
		U &second() const { return m_pair.second; }
		IteratorPair<T,U>& operator++ (){ ++m_pair.first; ++m_pair.second; return *this; }
		IteratorPair<T,U> operator++ (int){ IteratorPair<T,U> result(*this); ++m_pair.first; ++m_pair.second; return result; }
		IteratorPair<T,U>& operator*(){ return *this; }
		bool operator!=( const IteratorPair<T,U>& other ){ return m_pair != other.m_pair; }
		bool operator==( const IteratorPair<T,U>& other ){ return m_pair == other.m_pair; }

		//define these things to allow iterator_traits to work with this iterator
		typedef std::forward_iterator_tag iterator_category;
		typedef IteratorPair<T&,U&> value_type;
		typedef size_t difference_type;
		typedef IteratorPair<T,U> pointer;
		typedef IteratorPair<T,U>& reference;
	private:
		std::pair<T, U> m_pair;
	};
	template< class T, class U>
	IteratorPair<T*,U*> getBeginRawPair(std::vector<T> &first, std::vector<U> &second)
	{
		sci::assertThrow( first.size() > 0, sci::err() );
		sci::assertThrow( second.size() > 0, sci::err() );
		return IteratorPair<T*,U*>(&first[0], &second[0] );
	}
	template< class T, class U>
	IteratorPair<const T*,U*> getBeginRawPair(const std::vector<T> &first, std::vector<U> &second)
	{
		sci::assertThrow( first.size() > 0, sci::err() );
		sci::assertThrow( second.size() > 0, sci::err() );
		return IteratorPair<const T*,U*>(&first[0], &second[0] );
	}
	template< class T, class U>
	IteratorPair<T*,const U*> getBeginRawPair(std::vector<T> &first, const std::vector<U> &second)
	{
		sci::assertThrow( first.size() > 0, sci::err() );
		sci::assertThrow( second.size() > 0, sci::err() );
		return IteratorPair<T*,U*>(&first[0], &second[0] );
	}
	template< class T, class U>
	IteratorPair<const T*,const U*> getBeginRawPair(const std::vector<T> &first, const std::vector<U> &second)
	{
		sci::assertThrow( first.size() > 0, sci::err() );
		sci::assertThrow( second.size() > 0, sci::err() );
		return IteratorPair<T*,U*>(&first[0], &second[0] );
	}
	template< class T, class U>
	IteratorPair<T*,U*> getEndRawPair(std::vector<T> &first, std::vector<U> &second)
	{
		size_t len = first.size();
		sci::assertThrow( len > 0, sci::err() );
		sci::assertThrow( second.size() == len, sci::err() );
		return IteratorPair<T*,U*>(&first[0]+len, &second[0]+len );
	}
	template< class T, class U>
	IteratorPair<const T*,U*> getEndRawPair(const std::vector<T> &first, std::vector<U> &second)
	{
		size_t len = first.size();
		sci::assertThrow( len > 0, sci::err() );
		sci::assertThrow( second.size() == len, sci::err() );
		return IteratorPair<const T*,U*>(&first[0]+len, &second[0]+len );
	}
	template< class T, class U>
	IteratorPair<T*,const U*> getEndRawPair(std::vector<T> &first, const std::vector<U> &second)
	{
		size_t len = first.size();
		sci::assertThrow( len > 0, sci::err() );
		sci::assertThrow( second.size() == len, sci::err() );
		return IteratorPair<T*,U*>(&first[0]+len, &second[0]+len );
	}
	template< class T, class U>
	IteratorPair<const T*,const U*> getEndRawPair(const std::vector<T> &first, const std::vector<U> &second)
	{
		size_t len = first.size();
		sci::assertThrow( len > 0, sci::err() );
		sci::assertThrow( second.size() == len, sci::err() );
		return IteratorPair<T*,U*>(&first[0]+len, &second[0]+len );
	}
}

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




#define SCIFUNCWRAP( SCINAME, STDNAME)\
	template<class T>\
	inline T SCINAME(const T &v)\
	{\
		return STDNAME(v);\
	}\
	template<class T>\
	inline std::vector<T> SCINAME(const std::vector<T> &v)\
	{\
		std::vector<T> result(v.size());\
		if( v.size() == 0 )\
			return result;\
		sci::transform( &v[0], &v[0] + v.size(), &result[0],\
			[](const T &in){ return sci::SCINAME(in); } );\
		return result;\
	}\

	/*template<class T>
	inline std::vector<T> log10(const std::vector<T> &v)
	{
		std::vector<T> result(v.size());
		if( v.size() == 0 )
			return result;
		std::transform( &v[0], &v[0] + v.size(), &result[0],
			[](const T &in){ return sci::log10(in); } );
		return result;
	}
	template<class T>
	inline T log10(const T &v)
	{
		return log10(v);
	}*/

	SCIFUNCWRAP( exp, std::exp )
	SCIFUNCWRAP( ln, std::log )
	SCIFUNCWRAP( log10, std::log10 )
	SCIFUNCWRAP( abs, std::abs )
	SCIFUNCWRAP( sqrt, std::sqrt )
	SCIFUNCWRAP( sin, std::sin )
	SCIFUNCWRAP( cos, std::cos )
	SCIFUNCWRAP( tan, std::tan )
	SCIFUNCWRAP( asin, std::asin )
	SCIFUNCWRAP( acos, std::acos )
	SCIFUNCWRAP( atan, std::atan )
	SCIFUNCWRAP( sinh, std::sinh )
	SCIFUNCWRAP( cosh, std::cosh )
	SCIFUNCWRAP( tanh, std::tanh )
	SCIFUNCWRAP( asinh, std::asinh )
	SCIFUNCWRAP( acosh, std::acosh )
	SCIFUNCWRAP( atanh, std::atanh )
	SCIFUNCWRAP( erf, sci::erf )
	
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

	

	double erf(double x);
	double erfInverse(double x);

}
#endif
