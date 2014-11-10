#ifndef svectoroperators_h
#define svectoroperators_h

//#include"../include/svector/svector.h"
#include<vector>
//#include<set>
//#include<type_traits>

//operators for operation on vectors in an elementwise fashion.
//currently works for 2 vectors of the same number of dimensions
//and 1d vectors with scalars. Currently limited by the fact that
//allowing n-dimension vectors and scalars would also allow vectors
//of different types, which leads to things like + giving different
//return types for different order. this will be fixed using c++11
//features.


#ifndef SBOOL
#define SBOOL char
#endif

namespace sci
{
	//note the consts are missing as they can be (and are) specified using the template parameters
	template<class T, class U, class V>
	inline V add(T &a, U &b){return a+b;}
	template<class T, class U, class V>
	inline V subtract(T &a, T &b){return a-b;}
	template<class T, class U, class V>
	inline V multiply(T &a, T &b){return a*b;}
	template<class T, class U, class V>
	inline V divide(T &a, T &b){return a/b;}

	
	template<class T, class U, class V>
	inline SBOOL iseq(T &a, U &b){return a==b;}
	template<class T, class U, class V>
	inline SBOOL noteq(T &a, T &b){return a!=b;}
	template<class T, class U, class V>
	inline SBOOL lt(T &a, T &b){return a<b;}
	template<class T, class U, class V>
	inline SBOOL gt(T &a, T &b){return a>b;}
	template<class T, class U, class V>
	inline SBOOL lteq(T &a, T &b){return a<=b;}
	template<class T, class U, class V>
	inline SBOOL gteq(T &a, T &b){return a>=b;}

	template<class T, class U, class V>
	inline V remainder(T &a, U &b){return a%b;}
	template<class T>
	inline T preincrement( T &a){return ++a;}
	template<class T>
	inline T predecrement( T &a){return --a;}
	template<class T>
	inline T postincrement( T &a){return a++;}
	template<class T>
	inline T postdecrement( T &a){return a--;}
	template<class T>
	inline T unaryminus( const T &a){return -a;} //note we include the const here because all single vecor operators for which the return is needed are const

	template<class T, class U, class V>
	inline SBOOL and(T &a, T &b){return a&&b;}
	template<class T, class U, class V>
	inline SBOOL or(T &a, T &b){return a||b;}
	template<class T>
	inline SBOOL not(const T &a){return !a;} //note we include the const here because all single vecor operators for which the return is needed are const

	template<class T, class U, class V>
	inline V bitwiseand(T &a, U &b){return a&b;}
	template<class T, class U, class V>
	inline V bitwiseor(T &a, U &b){return a|b;}
	template<class T>
	inline T bitwisenot(const T &a){return ~a;} //note we include the const here because all single vecor operators for which the return is needed are const
	template<class T, class U, class V>
	inline V bitwisexor(T &a, U &b){return a^b;}
	template<class T, class U, class V>
	inline V bitwiseleftshift(T &a, U &b){return a<<b;}
	template<class T, class U, class V>
	inline V bitwiserightshift(T &a, U &b){return a>>b;}
	
	template<class T, class U>
	inline T addass(T &a, U &b){return a+=b;}
	template<class T, class U>
	inline T subtractass(T &a, U &b){return a-=b;}
	template<class T, class U>
	inline T multiplyass(T &a, const U &b){return a*=b;}
	template<class T, class U>
	inline T divideass(T &a, const U &b){return a/=b;}
	
	template<class T, class U>
	inline T remainderass(T &a, const U&b){return a%=b;}
	
	template<class T, class U>
	inline T bitwiseandass(T &a, const U &b){return a&=b;}
	template<class T, class U>
	inline T bitwiseorass(T &a, const U &b){return a|=b;}
	template<class T, class U>
	inline T bitwisexorass(T &a, const U &b){return a^=b;}
	template<class T, class U>
	inline T bitwiseleftshiftass(T &a, const U &b){return a<<=b;}
	template<class T, class U>
	inline T bitwiserightshiftass(T &a, const U &b){return a>>=b;}


	//vector and vector requiring a result vector
	//T is the base type of the vectors without any constness
	//VEC is the type of the firstvector input, it must be either const std::vector<T> or std::vector<T>
	//U is the base type of the result vector, it should not be const
	//V and W are the base types of the two input vectors with constness, V must have same constness as Vec
	//U Func(V&,W&) is the function to be called with each element of the input vectors to get each element of the output vectors
	template <class T, class VEC, class U, class V, class W, U Func(V&,W&) > //use our function as a template parameter which allows it to be inlined which gives some performance boost
	void operate(VEC &a, const std::vector<T> &b, std::vector<U> &result)
	{
		size_t len=std::max(a.size(),b.size());
		size_t copylen=std::min(a.size(),b.size());
		if(result.size()>copylen)result.clear();
		result.resize(len,std::numeric_limits<U>::quiet_NaN());
		if(a.size()==0 || b.size()==0) return;
		V *ai=&a[0];
		W *bi=&b[0];
		U *resulti=&result[0];
		V *aiend=ai+copylen;
		for(;ai!=aiend;++ai)
		{
			*resulti=Func(*ai,*bi);
			++bi;
			++resulti;
		}
	}

	//multi-d vector and multi-d vector requiring a result vector
	//T is the base type of the vectors without any constness
	//VEC is the type of the firstvector input, it must be either const std::vector<T> or std::vector<T>
	//U is the base type of the result vector, it should not be const
	//V and W are the base types of the two input vectors with constness, V must have same constness as Vec
	//U Func(V&,W&) is the function to be called with each element of the input vectors to get each element of the output vectors
	/*template <class T, class VEC, class U, class V, class W, U Func(V&,W&) > //use our function as a template parameter which allows it to be inlined which gives some performance boost
	void operate(std::vector<VEC> &a, const std::vector<std::vector<T>> &b, std::vector<std::vector<U>> &result)
	{
		size_t len=std::max(a.size(),b.size());
		size_t copylen=std::min(a.size(),b.size());
		if(result.size()>copylen)result.clear();
		result.resize(len);
		if(a.size()==0 || b.size()==0) return;
		V *ai=&a[0];
		W *bi=&b[0];
		U *resulti=&result[0];
		V *aiend=ai+copylen;
		for(;ai!=aiend;++ai)
		{
			operate<T, std::vector<T>, U, V, W, U, Func>(*ai, *bi, resulti);
			++bi;
			++resulti;
		}
	}*/
	//vector and scalar - in all cases the scalar (second parameter) is const so we need fewer templates parameters
	//T is the base type of the vectors without any constness
	//VEC is the type of the firstvector input, it must be either const std::vector<T> or std::vector<T>
	//U is the base type of the result vector, it should not be const
	//V is the base types of the input vector with constness, it must have same constness as Vec
	//U Func(V&,W&) is the function to be called with each element of the input vectors to get each element of the output vectors
	template <class T, class VEC, class U, class V, U Func(V&,const T&) >
	void operate(VEC &a, const T &b, std::vector<U> &result)
	{
		result.resize(a.size());
		if(a.size()==0) return;
		V *ai=&a[0];
		U *resulti=&result[0];
		V *aiend=ai+a.size();
		for(;ai!=aiend;++ai)
		{
			*resulti=Func(*ai,b);
			++resulti;
		}
	}
	//vector and scalar - in all cases the scalar (first parameter) is const so we need fewer templates parameters
	//T is the base type of the vectors without any constness
	//VEC is the type of the firstvector input, it must be either const std::vector<T> or std::vector<T>
	//U is the base type of the result vector, it should not be const
	//V is the base types of the input vector with constness, it must have same constness as Vec
	//U Func(V&,W&) is the function to be called with each element of the input vectors to get each element of the output vectors
	template <class T, class VEC, class U, class V, U Func(V&,const T&) >
	void operate(const T &a, VEC &b, std::vector<U> &result)
	{
		result.resize(b.size());
		if(b.size()==0) return;
		V *bi=&b[0];
		U *resulti=&result[0];
		V *biend=bi+b.size();
		for(;bi!=biend;++bi)
		{
			*resulti=Func(a,*bi);
			++resulti;
		}
	}
	//vector only, returning a result vector, this is always a const vector
	//T is the base type of the vectors without any constness
	template <class T, class U, U Func(const T&) >
	void operate(const std::vector<T> &a, std::vector<U> &result)
	{
		result.resize(a.size());
		if(a.size()==0) return;
		const T *ai=&a[0];
		U *resulti=&result[0];
		const T *aiend=ai+a.size();
		for(;ai!=aiend;++ai)
		{
			*resulti=Func(*ai);
			++resulti;
		}
	}
	
	//vector and vector returning no result vector, in this case we always have a non-const and const vector
	//T is the base type of the vectors without any constness
	//VEC is the type of the firstvector input, it must be either const std::vector<T> or std::vector<T>
	//U is the base type of the result vector, it should not be const
	//V and W are the base types of the two input vectors with constness, V must have same constness as Vec
	//U Func(V&,W&) is the function to be called with each element of the input vectors to get each element of the output vectors
	template < class T, T Func(T&,const T&) > 
	void operate(std::vector<T> &a, const std::vector<T> &b)
	{
		size_t len=std::max(a.size(),b.size());
		size_t copylen=std::min(a.size(),b.size());
		if(a.size()>copylen)a.resize(copylen);
		a.resize(len,std::numeric_limits<T>::quiet_NaN());
		if(a.size()==0 || b.size()==0) return;
		T *ai=&a[0];
		const T *bi=&b[0];
		T *aiend=ai+copylen;
		for(;ai!=aiend;++ai)
		{
			Func(*ai,*bi);
			++bi;
		}
	}
	//vector and scalar returning no result - in all cases the scalar (second parameter) is const so we need fewer templates parameters
	//T is the base type of the vectors without any constness
	template <class T, T Func(T&,const T&) >
	void operate(std::vector<T> &a, const T &b)
	{
		if(a.size()==0) return;
		T *ai=&a[0];
		T *aiend=ai+a.size();
		for(;ai!=aiend;++ai)
		{
			Func(*ai,b);
		}
	}
	//vector only with no result this is always a non-const vector
	//T is the base type of the vector without any constness
	template <class T, T Func(T&) >
	void operate(std::vector<T> &a)
	{
		if(a.size()==0) return;
		T *ai=&a[0];
		T *aiend=ai+a.size();
		for(;ai!=aiend;++ai)
		{
			Func(*ai);
		}
	}

}

//****************************************************************
//***********Add subtract multiply divide*************************
//****************************************************************

class svi;

template <class T, class U>
auto operator+(const std::vector<T> &a, const std::vector<U> &b) -> std::vector<decltype(T()+U())>
{
	static_assert(sci::sameNDims<T,U>::value, "Vector operations requive a vector and scalar or two vectors with the same number of dimensions");
	size_t len=std::max(a.size(),b.size());
	std::vector<decltype(T()+U())> result(len);
	size_t iterLen=std::min(a.size(), b.size());
	if(iterLen==0)
		return result;
	const T* ai=&a[0];
	const U* bi=&b[0];
	decltype(T()+U())* resulti=&result[0];
	const T* aEnd=ai+iterLen;
	for(; ai<aEnd; ++ai, ++bi, ++resulti)
		*resulti = *ai + *bi;
	return result;
}

template <class T, class U>
auto operator+(const std::vector<T> &a, const U &b) -> std::vector< decltype(T()+U()) >
{
	size_t len=a.size();
	std::vector<decltype(T()+U())> result(len);
	if(len==0)
		return result;
	const T* ai=&a[0];
	decltype(T()+U())* resulti=&result[0];
	const T* aEnd=ai+len;
	for(; ai<aEnd; ++ai, ++resulti)
		*resulti = *ai + b;
	return result;
}

template <class T, class U>
auto operator+(const T &a, const std::vector<U> &b) -> std::vector< decltype(T()+U()) >
{
	size_t len=b.size();
	std::vector<decltype(T()+U())> result(len);
	if(len==0)
		return result;
	const T* bi=&b[0];
	decltype(T()+U())* resulti=&result[0];
	const T* bEnd=bi+len;
	for(; bi<bEnd; ++bi, ++resulti)
		*resulti = a + *bi;
	return result;
}

template <class T, class U>
auto operator-(const std::vector<T> &a, const std::vector<U> &b) -> std::vector<decltype(T()-U())>
{
	static_assert(sci::sameNDims<T,U>::value, "Vector operations require a vector and scalar or two vectors with the same number of dimensions");
	size_t len=std::max(a.size(),b.size());
	std::vector<decltype(T()-U())> result(len);
	size_t iterLen=std::min(a.size(), b.size());
	if(iterLen==0)
		return result;
	const T* ai=&a[0];
	const U* bi=&b[0];
	decltype(T()-U())* resulti=&result[0];
	const T* aEnd=ai+iterLen;
	for(; ai<aEnd; ++ai, ++bi, ++resulti)
		*resulti = *ai - *bi;
	return result;
}

template <class T, class U>
auto operator-(const std::vector<T> &a, const U &b) -> std::vector< decltype(T()-U()) >
{
	size_t len=a.size();
	std::vector<decltype(T()-U())> result(len);
	if(len==0)
		return result;
	const T* ai=&a[0];
	decltype(T()-U())* resulti=&result[0];
	const T* aEnd=ai+len;
	for(; ai<aEnd; ++ai, ++resulti)
		*resulti = *ai - b;
	return result;
}

template <class T, class U>
auto operator-(const T &a, const std::vector<U> &b) -> std::vector< decltype(T()-U()) >
{
	size_t len=b.size();
	std::vector<decltype(T()-U())> result(len);
	if(len==0)
		return result;
	const T* bi=&b[0];
	decltype(T()-U())* resulti=&result[0];
	const T* bEnd=bi+len;
	for(; bi<bEnd; ++bi, ++resulti)
		*resulti = a - *bi;
	return result;
}

template <class T, class U>
auto operator*(const std::vector<T> &a, const std::vector<U> &b) -> std::vector<decltype(T()*U())>
{
	static_assert(sci::sameNDims<T,U>::value, "Vector operations require a vector and scalar or two vectors with the same number of dimensions");
	size_t len=std::max(a.size(),b.size());
	std::vector<decltype(T()*U())> result(len);
	size_t iterLen=std::min(a.size(), b.size());
	if(iterLen==0)
		return result;
	const T* ai=&a[0];
	const U* bi=&b[0];
	decltype(T()*U())* resulti=&result[0];
	const T* aEnd=ai+iterLen;
	for(; ai<aEnd; ++ai, ++bi, ++resulti)
		*resulti = *ai * *bi;
	return result;
}

template <class T, class U>
auto operator*(const std::vector<T> &a, const U &b) -> std::vector< decltype(T()*U()) >
{
	size_t len=a.size();
	std::vector<decltype(T()*U())> result(len);
	if(len==0)
		return result;
	const T* ai=&a[0];
	decltype(T()*U())* resulti=&result[0];
	const T* aEnd=ai+len;
	for(; ai<aEnd; ++ai, ++resulti)
		*resulti = *ai * b;
	return result;
}

template <class T, class U>
auto operator*(const T &a, const std::vector<U> &b) -> std::vector< decltype(T()*U()) >
{
	size_t len=b.size();
	std::vector<decltype(T()*U())> result(len);
	if(len==0)
		return result;
	const U* bi=&b[0];
	decltype(T()*U())* resulti=&result[0];
	const U* bEnd=bi+len;
	for(; bi<bEnd; ++bi, ++resulti)
		*resulti = a * *bi;
	return result;
}

template <class T, class U>
auto operator/(const std::vector<T> &a, const std::vector<U> &b) -> std::vector<decltype(T()/U())>
{
	static_assert(sci::sameNDims<T,U>::value, "Vector operations require a vector and scalar or two vectors with the same number of dimensions");
	size_t len=std::max(a.size(),b.size());
	std::vector<decltype(T()/U())> result(len);
	size_t iterLen=std::min(a.size(), b.size());
	if(iterLen==0)
		return result;
	const T* ai=&a[0];
	const U* bi=&b[0];
	decltype(T()/U())* resulti=&result[0];
	const T* aEnd=ai+iterLen;
	for(; ai<aEnd; ++ai, ++bi, ++resulti)
		*resulti = *ai / *bi;
	return result;
}

template <class T, class U>
auto operator/(const std::vector<T> &a, const U &b) -> std::vector< decltype(T()/U()) >
{
	size_t len=a.size();
	std::vector<decltype(T()/U())> result(len);
	if(len==0)
		return result;
	const T* ai=&a[0];
	decltype(T()/U())* resulti=&result[0];
	const T* aEnd=ai+len;
	for(; ai<aEnd; ++ai, ++resulti)
		*resulti = *ai / b;
	return result;
}

template <class T, class U>
auto operator/(const T &a, const std::vector<U> &b) -> std::vector< decltype(T()/U()) >
{
	size_t len=b.size();
	std::vector<decltype(T()/U())> result(len);
	if(len==0)
		return result;
	const U* bi=&b[0];
	decltype(T()/U())* resulti=&result[0];
	const U* bEnd=bi+len;
	for(; bi<bEnd; ++bi, ++resulti)
		*resulti = a / *bi ;
	return result;
}

/*template <class T>
std::vector<T> operator+(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,const T,sci::add<const T, const T,T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator-(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,const T,sci::subtract<const T, const T,T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator*(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,const T,sci::multiply<const T, const T,T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator/(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,const T,sci::divide<const T, const T,T> >(a,b,result);
	return result;
}

/*template <class T>
std::vector<T> operator+(const std::vector<T> &a, const T &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,sci::add<const T, const T, T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator-(const std::vector<T> &a, const T &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,sci::subtract<const T, const T, T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator*(const std::vector<T> &a, const T &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,sci::multiply<const T, const T, T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator/(const std::vector<T> &a, const T &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,sci::divide<const T, const T, T> >(a,b,result);
	return result;
}

/*template <class T>
std::vector<T> operator+(const T &a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,sci::add<const T, const T, T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator-(const T &a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,sci::subtract<const T, const T, T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator*(const T &a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,sci::multiply<const T, const T, T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator/(const T &a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,sci::divide<const T, const T, T> >(a,b,result);
	return result;
}
*/

//************************************************
//******************comparison********************
//************************************************


template <class T>
std::vector<SBOOL> operator==(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,const T,sci::iseq<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator!=(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,const T,sci::noteq<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator<(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,const T,sci::lt<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator>(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,const T,sci::gt<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator<=(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,const T,sci::lteq<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator>=(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,const T,sci::gteq<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator==(const std::vector<T> &a, const T &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,sci::iseq<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator!=(const std::vector<T> &a, const T &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,sci::noteq<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator<(const std::vector<T> &a, const T &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,sci::lt<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator>(const std::vector<T> &a, const T &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,sci::gt<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator<=(const std::vector<T> &a, const T &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,sci::lteq<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator>=(const std::vector<T> &a, const T &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,sci::gteq<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator==(const T &a, const std::vector<T> &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,sci::iseq<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator!=(const T &a, const std::vector<T> &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,sci::noteq<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator<(const T &a, const std::vector<T> &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,sci::lt<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator>(const T &a, const std::vector<T> &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,sci::gt<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator<=(const T &a, const std::vector<T> &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,sci::lteq<const T, const T, SBOOL> >(a,b,result);
	return result;
}
template <class T>
std::vector<SBOOL> operator>=(const T &a, const std::vector<T> &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>, SBOOL,const T,sci::gteq<const T, const T, SBOOL> >(a,b,result);
	return result;
}



template <class T>
std::vector< std::vector<SBOOL> > operator<(const std::vector< std::vector<T> > &a, T b)
{
	size_t len=a.size();
	std::vector<std::vector <SBOOL> > result(len);
	for(size_t i=0; i<len; i++)	result[i]=a[i]<b;
	return result;
}

template <class T>
std::vector< std::vector<SBOOL> > operator>(const std::vector< std::vector<T> > &a, T b)
{
	size_t len=a.size();
	std::vector<std::vector <SBOOL> > result(len);
	for(size_t i=0; i<len; i++)	result[i]=a[i]>b;
	return result;
}

template <class T>
std::vector< std::vector<SBOOL> > operator<=(const std::vector< std::vector<T> > &a, T b)
{
	size_t len=a.size();
	std::vector<std::vector <SBOOL> > result(len);
	for(size_t i=0; i<len; i++)	result[i]=a[i]<=b;
	return result;
}

template <class T>
std::vector< std::vector<SBOOL> > operator>=(const std::vector< std::vector<T> > &a, T b)
{
	size_t len=a.size();
	std::vector<std::vector <SBOOL> > result(len);
	for(size_t i=0; i<len; i++)	result[i]=a[i]>=b;
	return result;
}

template <class T>
std::vector< std::vector<SBOOL> > operator==(T a, const std::vector< std::vector<T> > &b)
{
	size_t len=b.size();
	std::vector<std::vector <SBOOL> > result(len);
	for(size_t i=0; i<len; i++)	result[i]=a==b[i];
	return result;
}

template <class T>
std::vector< std::vector<SBOOL> > operator!=(T a, const std::vector< std::vector<T> > &b)
{
	size_t len=b.size();
	std::vector<std::vector <SBOOL> > result(len);
	for(size_t i=0; i<len; i++)	result[i]=a!=b[i];
	return result;
}

template <class T>
std::vector< std::vector<SBOOL> > operator<(T a, const std::vector< std::vector<T> > &b)
{
	size_t len=b.size();
	std::vector<std::vector <SBOOL> > result(len);
	for(size_t i=0; i<len; i++)	result[i]=a<b[i];
	return result;
}

template <class T>
std::vector< std::vector<SBOOL> > operator>(T a, const std::vector< std::vector<T> > &b)
{
	size_t len=b.size();
	std::vector<std::vector <SBOOL> > result(len);
	for(size_t i=0; i<len; i++)	result[i]=a>b[i];
	return result;
}

template <class T>
std::vector< std::vector<SBOOL> > operator<=(T a, const std::vector< std::vector<T> > &b)
{
	size_t len=b.size();
	std::vector<std::vector <SBOOL> > result(len);
	for(size_t i=0; i<len; i++)	result[i]=a<=b[i];
	return result;
}

template <class T>
std::vector< std::vector<SBOOL> > operator>=(T a, const std::vector< std::vector<T> > &b)
{
	size_t len=b.size();
	std::vector<std::vector <SBOOL> > result(len);
	for(size_t i=0; i<len; i++)	result[i]=a>=b[i];
	return result;
}


template <class T>
std::vector<std::vector<SBOOL>> operator==(const std::vector<std::vector<T>> &a, const std::vector<std::vector<T>> &b)
{
	size_t len=std::max(a.size(), b.size());
	std::vector<std::vector <SBOOL> > result(len);
	size_t iterLen=std::min(a.size(), b.size());
	if(iterLen==0)
		return result;
	const std::vector<T>* aEnd=&a[0]+iterLen;
	const std::vector<T>* ai=&a[0];
	const std::vector<T>* bi=&b[0];
	std::vector<SBOOL>* resulti=&result[0];
	for(; ai<aEnd; ++ai, ++bi, ++resulti)
		*resulti = (*ai) == (*bi);
	return result;
}
template <class T>
std::vector<std::vector<SBOOL>> operator!=(const std::vector<std::vector<T>> &a, const std::vector<std::vector<T>> &b)
{
	size_t len=std::max(a.size(), b.size());
	std::vector<std::vector <SBOOL> > result(len);
	size_t iterLen=std::min(a.size(), b.size());
	if(iterLen==0)
		return result;
	const std::vector<T>* aEnd=&a[0]+iterLen;
	const std::vector<T>* ai=&a[0];
	const std::vector<T>* bi=&b[0];
	std::vector<SBOOL>* resulti=&result[0];
	for(; ai<aEnd; ++ai, ++bi, ++resulti)
		*resulti = (*ai) != (*bi);
	return result;
}
template <class T>
std::vector<std::vector<SBOOL>> operator>(const std::vector<std::vector<T>> &a, const std::vector<std::vector<T>> &b)
{
	size_t len=std::max(a.size(), b.size());
	std::vector<std::vector <SBOOL> > result(len);
	size_t iterLen=std::min(a.size(), b.size());
	if(iterLen==0)
		return result;
	const std::vector<T>* aEnd=&a[0]+iterLen;
	const std::vector<T>* ai=&a[0];
	const std::vector<T>* bi=&b[0];
	std::vector<SBOOL>* resulti=&result[0];
	for(; ai<aEnd; ++ai, ++bi, ++resulti)
		*resulti = (*ai) > (*bi);
	return result;
}
template <class T>
std::vector<std::vector<SBOOL>> operator<(const std::vector<std::vector<T>> &a, const std::vector<std::vector<T>> &b)
{
	size_t len=std::max(a.size(), b.size());
	std::vector<std::vector <SBOOL> > result(len);
	size_t iterLen=std::min(a.size(), b.size());
	if(iterLen==0)
		return result;
	const std::vector<T>* aEnd=&a[0]+iterLen;
	const std::vector<T>* ai=&a[0];
	const std::vector<T>* bi=&b[0];
	std::vector<SBOOL>* resulti=&result[0];
	for(; ai<aEnd; ++ai, ++bi, ++resulti)
		*resulti = (*ai) < (*bi);
	return result;
}
template <class T>
std::vector<std::vector<SBOOL>> operator>=(const std::vector<std::vector<T>> &a, const std::vector<std::vector<T>> &b)
{
	size_t len=std::max(a.size(), b.size());
	std::vector<std::vector <SBOOL> > result(len);
	size_t iterLen=std::min(a.size(), b.size());
	if(iterLen==0)
		return result;
	const std::vector<T>* aEnd=&a[0]+iterLen;
	const std::vector<T>* ai=&a[0];
	const std::vector<T>* bi=&b[0];
	std::vector<SBOOL>* resulti=&result[0];
	for(; ai<aEnd; ++ai, ++bi, ++resulti)
		*resulti = (*ai) >= (*bi);
	return result;
}
template <class T>
std::vector<std::vector<SBOOL>> operator<=(const std::vector<std::vector<T>> &a, const std::vector<std::vector<T>> &b)
{
	size_t len=std::max(a.size(), b.size());
	std::vector<std::vector <SBOOL> > result(len);
	size_t iterLen=std::min(a.size(), b.size());
	if(iterLen==0)
		return result;
	const std::vector<T>* aEnd=&a[0]+iterLen;
	const std::vector<T>* ai=&a[0];
	const std::vector<T>* bi=&b[0];
	std::vector<SBOOL>* resulti=&result[0];
	for(; ai<aEnd; ++ai, ++bi, ++resulti)
		*resulti = (*ai) <= (*bi);
	return result;
}

//*****************************************************
//****************remainder****************************
//*****************************************************


template <class T>
std::vector<T> operator%(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,const T,sci::remainder<const T, const T,T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator%(const std::vector<T> &a, const T &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,sci::remainder<const T, const T, T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator%(const T &a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,sci::remainder<const T, const T, T> >(a,b,result);
	return result;
}

//*****************************************************
//***************incriment decrement*******************
//*****************************************************

template <class T>
std::vector<T>& operator++(std::vector<T> &a)
{
	sci::operate< T,sci::preincrement<T> >(a);
	return a;
}

template <class T>
std::vector<T>& operator--(std::vector<T> &a)
{
	sci::operate< T,sci::predecrement<T> >(a);
	return a;
}

template <class T>
std::vector<T> operator++(std::vector<T> &a, int dummy)
{
	std::vector<T> copy=a;
	sci::operate< T,sci::postincrement<T> >(a);
	return copy;
}

template <class T>
std::vector<T> operator--(std::vector<T> &a, int dummy)
{
	std::vector<T> copy=a;
	sci::operate< T,sci::postdecrement<T> >(a);
	return copy;
}


//*********************************************************
//**************unary + unary -****************************
//*********************************************************

template <class T>
std::vector<T> operator-(const std::vector<T> &a)
{
	std::vector<T> result;
	sci::operate< T,T,sci::unaryminus<T> >(a,result); 
	return result;
}

//unary plus does integral promotion - not sure what this is. Something to do with
//converting char to wider variable such as int. As I'm not sure what the result type is
//I've left this out


//*****************************************************
//**************logical combination********************
//*****************************************************

template <class T>
std::vector<SBOOL> operator||(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>,SBOOL,const T,const T,sci::or<const T, const T,T> >(a,b,result);
	return result;
}

template <class T>
std::vector<SBOOL> operator&&(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>,SBOOL,const T,const T,sci::and<const T, const T,T> >(a,b,result);
	return result;
}

template <class T>
std::vector<SBOOL> operator||(const std::vector<T> &a, T b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>,SBOOL,const T,sci::or<const T, const T, SBOOL> >(a,b,result);
	return result;
}

template <class T>
std::vector<SBOOL> operator&&(const std::vector<T> &a, T b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>,SBOOL,const T,sci::and<const T, const T, SBOOL> >(a,b,result);
	return result;
}

template <class T>
std::vector<SBOOL> operator||(T a, const std::vector<T> &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>,SBOOL,const T,sci::or<const T, const T, SBOOL> >(a,b,result);
	return result;
}

template <class T>
std::vector<SBOOL> operator&&(T a, const std::vector<T> &b)
{
	std::vector<SBOOL> result;
	sci::operate< T,const std::vector<T>,SBOOL,const T,sci::and<const T, const T, SBOOL> >(a,b,result);
	return result;
}

std::vector< std::vector<SBOOL> > operator||(const std::vector< std::vector<SBOOL> > &a, const std::vector< std::vector<SBOOL> > &b);

std::vector< std::vector<SBOOL> > operator&&(const std::vector< std::vector<SBOOL> > &a, const std::vector< std::vector<SBOOL> > &b);


template <class T>
std::vector<SBOOL> operator!(const std::vector<T> &a)
{
	std::vector<SBOOL> result;
	sci::operate< T,SBOOL,sci::not<T> >(a,result); 
	return result;
}


//***********************************************************
//***************bitwise*************************************
//***********************************************************
//some currently not used as the types can easily be incompatible

template <class T>
std::vector<T> operator|(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,const T,sci::bitwiseor<const T, const T,T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator&(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,const T,sci::bitwiseand<const T, const T,T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator^(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,const T,sci::bitwisexor<const T, const T,T> >(a,b,result);
	return result;
}
/*
template <class T>
std::vector<T> operator<<(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,const T,sci::bitwiseleftshift<const T, const T,T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator>>(const std::vector<T> &a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,const T,sci::bitwiserightshift<const T, const T,T> >(a,b,result);
	return result;
}*/

template <class T>
std::vector<T> operator|(const std::vector<T> &a, T b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,sci::bitwiseor<const T, const T, T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator&(const std::vector<T> &a, T b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,sci::bitwiseand<const T, const T, T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator^(const std::vector<T> &a, T b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,sci::bitwisexor<const T, const T, T> >(a,b,result);
	return result;
}

/*
template <class T>
std::vector<T> operator<<(const std::vector<T> &a, int b)
{
	size_t len=a.size();
	std::vector <T> result(len);
	for(size_t i=0; i<len; i++) result[i]=a[i]<<b;
	return result;
}

template <class T>
std::vector<T> operator>>(const std::vector<T> &a, int b)
{
	size_t len=a.size();
	std::vector <T> result(len);
	for(size_t i=0; i<len; i++) result[i]=a[i]>>b;
	return result;
}
*/
template <class T>
std::vector<T> operator|(T a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,sci::bitwiseor<const T, const T, T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator&(T a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,sci::bitwiseand<const T, const T, T> >(a,b,result);
	return result;
}

template <class T>
std::vector<T> operator^(T a, const std::vector<T> &b)
{
	std::vector<T> result;
	sci::operate< T,const std::vector<T>,T,const T,sci::bitwisexor<const T, const T, T> >(a,b,result);
	return result;
}
/*
template <class T>
std::vector<T> operator<<(int a, const std::vector<T> &b)
{
	size_t len=b.size();
	std::vector <T> result(len);
	for(size_t i=0; i<len; i++) result[i]=a<<b[i];
	return result;
}

template <class T>
std::vector<T> operator>>(int a, const std::vector<T> &b)
{
	size_t len=b.size();
	std::vector <T> result(len);
	for(size_t i=0; i<len; i++) result[i]=a>>b[i];
	return result;
}
*/
template <class T>
std::vector<T> operator~(const std::vector<T> &a)
{
	std::vector<T> result;
	sci::operate< T,T,sci::bitwisenot<T> >(a,result); 
	return result;
}

//*********************************************************
//*************compound assignment*************************
//*********************************************************


template <class T>
std::vector<T>& operator+=(std::vector<T> &a, const std::vector<T> &b)
{
	sci::operate< T,sci::addass<T,const T> >(a,b);
	return a;
}

template <class T>
std::vector<T>& operator-=(std::vector<T> &a, const std::vector<T> &b)
{
	sci::operate< T,sci::subtractass<T,const T> >(a,b);
	return a;
}

template <class T>
std::vector<T>& operator*=(std::vector<T> &a, const std::vector<T> &b)
{
	sci::operate< T,sci::multiplyass<T,const T> >(a,b);
	return a;
}

template <class T>
std::vector<T>& operator/=(std::vector<T> &a, const std::vector<T> &b)
{
	sci::operate< T,sci::divideass<T,const T> >(a,b);
	return a;
}

template <class T>
std::vector<T>& operator%=(std::vector<T> &a, const std::vector<T> &b)
{
	sci::operate< T,sci::remainderass<T,const T> >(a,b);
	return a;
}

template <class T>
std::vector<T>& operator|=(std::vector<T> &a, const std::vector<T> &b)
{
	sci::operate< T,sci::bitwiseorass<T,const T> >(a,b);
	return a;
}

template <class T>
std::vector<T>& operator&=(std::vector<T> &a, const std::vector<T> &b)
{
	sci::operate< T,sci::bitwiseandass<T,const T> >(a,b);
	return a;
}

template <class T>
std::vector<T>& operator^=(std::vector<T> &a, const std::vector<T> &b)
{
	sci::operate< T,sci::bitwisexorass<T,const T> >(a,b);
	return a;
}
/*
template <class T>
std::vector<T>& operator<<=(std::vector<T> &a, const std::vector<T> &b)
{
	size_t len=std::max(a.size(),b.size());
	a.resize(len,std::numeric_limits<T>::quiet_NaN());
	for(size_t i=0; i<b.size(); i++) a[i]<<=b[i];
	return a;
}

template <class T>
std::vector<T>& operator>>=(std::vector<T> &a, const std::vector<T> &b)
{
	size_t len=std::max(a.size(),b.size());
	a.resize(len,std::numeric_limits<T>::quiet_NaN());
	for(size_t i=0; i<b.size(); i++) a[i]>>=b[i];
	return a;
}
*/
template <class T>
std::vector<T>& operator+=(std::vector<T> &a, double b)
{
	sci::operate< T,sci::addass<T,const T> >(a,b);
	return a;
}

template <class T>
std::vector<T>& operator-=(std::vector<T> &a, double b)
{
	sci::operate< T,sci::subtractass<T,const T> >(a,b);
	return a;
}

template <class T>
std::vector<T>& operator*=(std::vector<T> &a, double b)
{
	sci::operate< T,sci::multiplyass<T,const T> >(a,b);
	return a;
}

template <class T>
std::vector<T>& operator/=(std::vector<T> &a, double b)
{
	sci::operate< T,sci::divideass<T,const T> >(a,b);
	return a;
}

template <class T>
std::vector<T>& operator%=(std::vector<T> &a, int b)
{
	sci::operate< T,sci::remainderass<T,const T> >(a,b);
	return a;
}

template <class T>
std::vector<T>& operator|=(std::vector<T> &a, int b)
{
	sci::operate< T,sci::bitwiseorass<T,const T> >(a,b);
	return a;
}

template <class T>
std::vector<T>& operator&=(std::vector<T> &a, int b)
{
	sci::operate< T,sci::bitwiseandass<T,const T> >(a,b);
	return a;
}

template <class T>
std::vector<T>& operator^=(std::vector<T> &a, int b)
{
	sci::operate< T,sci::bitwisexorass<T,const T> >(a,b);
	return a;
}
/*
template <class T>
std::vector<T>& operator<<=(std::vector<T> &a, int b)
{
	size_t len=a.size();
	for(size_t i=0; i<len; i++) a[i]<<=b;
	return a;
}

template <class T>
std::vector<T>& operator>>=(std::vector<T> &a, int b)
{
	size_t len=a.size();
	for(size_t i=0; i<len; i++) a[i]>>=b;
	return a;
}
*/
#endif