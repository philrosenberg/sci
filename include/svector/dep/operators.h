#ifndef svectoroperators_h
#define svectoroperators_h

#include<vector>

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

template<class T, class U, class V>
inline V add(T &a, U &b){return a+b;}
template<class T, class U, class V>
inline V plusAss(T &a, U &b){return a+=b;}
template<class T, class U, class V>
inline V postIncrement(T &a, U &){return a++;}
template<class T, class U, class V>
inline V preIncrement(T &a, U &){return ++a;}

namespace sci_internal
{
	//note the consts are missing as they can be (and are) specified using the template parameters
	template<class T, class U, class V>
	inline V add(T &a, U &b){return a+b;}
	template<class T, class U, class V>
	inline V subtract(T &a, U &b){return a-b;}
	template<class T, class U, class V>
	inline V multiply(T &a, U &b){return a*b;}
	template<class T, class U, class V>
	inline V divide(T &a, U &b){return a/b;}

	
	template<class T, class U, class V>
	inline V iseq(T &a, U &b){return a==b;}
	template<class T, class U, class V>
	inline V noteq(T &a, U &b){return a!=b;}
	template<class T, class U, class V>
	inline V lt(T &a, U &b){return a<b;}
	template<class T, class U, class V>
	inline V gt(T &a, U &b){return a>b;}
	template<class T, class U, class V>
	inline V lteq(T &a, U &b){return a<=b;}
	template<class T, class U, class V>
	inline V gteq(T &a, U &b){return a>=b;}

	template<class T, class U, class V>
	inline V remainder(T &a, U &b){return a%b;}
	template<class T, class U, class V>
	inline V preincrement( T &a, U &){return ++a;}
	template<class T, class U, class V>
	inline V predecrement( T &a, U &){return --a;}
	template<class T, class U, class V>
	inline V postincrement( T &a, U &){return a++;}
	template<class T, class U, class V>
	inline V postdecrement( T &a, U &){return a--;}
	template<class T, class U, class V>
	inline V unaryminus( T &a, U &){return -a;}

	//Note the s_ here to avoid name clashes with C operators
	//which can use names for a certain ISO
	template<class T, class U, class V>
	inline V s_and(T &a, U &b){return a&&b;}
	template<class T, class U, class V>
	inline V s_or(T &a, U &b){return a||b;}
	template<class T, class U, class V>
	inline V s_not(T &a, U &){return !a;}
	
	template<class T, class U, class V>
	inline V bitwiseand(T &a, U &b){return a&b;}
	template<class T, class U, class V>
	inline V bitwiseor(T &a, U &b){return a|b;}
	template<class T, class U, class V>
	inline V bitwisenot(T &a, U &){return ~a;}
	template<class T, class U, class V>
	inline V bitwisexor(T &a, U &b){return a^b;}
	template<class T, class U, class V>
	inline V bitwiseleftshift(T &a, U &b){return a<<b;}
	template<class T, class U, class V>
	inline V bitwiserightshift(T &a, U &b){return a>>b;}
	
	template<class T, class U, class V>
	inline V addass(T &a, U &b){return a+=b;}
	template<class T, class U, class V>
	inline V subtractass(T &a, U &b){return a-=b;}
	template<class T, class U, class V>
	inline V multiplyass(T &a, U &b){return a*=b;}
	template<class T, class U, class V>
	inline V divideass(T &a, U &b){return a/=b;}
	
	template<class T, class U, class V>
	inline V remainderass(T &a, U&b){return a%=b;}
	
	template<class T, class U, class V>
	inline V bitwiseandass(T &a, const U &b){return a&=b;}
	template<class T, class U, class V>
	inline V bitwiseorass(T &a, const U &b){return a|=b;}
	template<class T, class U, class V>
	inline V bitwisexorass(T &a, const U &b){return a^=b;}
	template<class T, class U, class V>
	inline V bitwiseleftshiftass(T &a, const U &b){return a<<=b;}
	template<class T, class U, class V>
	inline V bitwiserightshiftass(T &a, const U &b){return a>>=b;}

	//deals with +, -, *, /, etc for two vectors
	template <class T, class U, class V, V FUNC ( const T &, const U & ) > 
	std::vector<V> operate(const std::vector<T> &a, const std::vector<U> &b)
	{
		std::vector<V> result;
		//sci::assertThrow( a.size() == b.size(), sci::err() )
		size_t len=a.size();
		result.resize( len );
		if(len==0)
			return result;
		auto ai=&a[0];
		auto bi=&b[0];
		auto resulti=&result[0];
		auto aiend=ai;
		aiend+=len;
		for( ; ai != aiend; ++ai, ++bi, ++resulti )
			*resulti=FUNC(*ai,*bi);
		return result;
	}

	//deals with +=, -=, *=, /=, etc for two vectors - note here the first vector is not const
	template <class T, class U, class V, V FUNC ( T &, const U & ) > 
	std::vector<V> operate(std::vector<T> &a, const std::vector<U> &b)
	{
		std::vector<V> result;
		//sci::assertThrow( a.size() == b.size(), sci::err() )
		size_t len=a.size();
		result.resize( len );
		if(len==0)
			return result;
		auto ai=&a[0];
		auto bi=&b[0];
		auto resulti=&result[0];
		auto aiend=ai;
		aiend+=len;
		for( ; ai != aiend; ++ai, ++bi, ++resulti )
			*resulti=FUNC(*ai,*bi);
		return result;
	}

	//deals with +, -, *, /, etc when the second parameter is a scalar
	template <class T, class U, class V, V FUNC ( const T &, const U & ) > 
	std::vector<V> operate(const std::vector<T> &a, const U &b)
	{
		std::vector<V> result;
		//sci::assertThrow( a.size() == b.size(), sci::err() )
		size_t len=a.size();
		result.resize( len );
		if(len==0)
			return result;
		auto ai=&a[0];
		auto resulti=&result[0];
		auto aiend=ai;
		aiend+=len;
		for( ; ai != aiend; ++ai, ++resulti )
			*resulti=FUNC(*ai,b);
		return result;
	}
	//deals with +, -, *, /, etc when the first parameter is a scalar
	template <class T, class U, class V, V FUNC ( const T &, const U & ) > 
	std::vector<V> operate(const T &a, const std::vector<U> &b)
	{
		std::vector<V> result;
		//sci::assertThrow( a.size() == b.size(), sci::err() )
		size_t len=b.size();
		result.resize( len );
		if(len==0)
			return result;
		auto bi=&b[0];
		auto resulti=&result[0];
		auto biend=bi;
		biend+=len;
		for( ; bi != biend; ++bi, ++resulti )
			*resulti=FUNC(a,*bi);
		return result;
	}

	//deals with +=, -=, *=, /=, etc when the second parameter is a scalar and ++, --, ! unary -, ~ etc
	//where there is only the vector used, in which case a dummy variable is passed for the scalar
	template <class T, class U, class V, V FUNC ( T &, const U & ) > 
	std::vector<V> operate(std::vector<T> &a, const U &b)
	{
		std::vector<V> result;
		//sci::assertThrow( a.size() == b.size(), sci::err() )
		size_t len=a.size();
		result.resize( len );
		if(len ==0)
			return result;
		auto ai=&a[0];
		auto resulti=&result[0];
		auto aiend=ai;
		aiend+=len;
		for( ; ai != aiend; ++ai, ++resulti )
			*resulti=FUNC(*ai,b);
		return result;
	}
}

//The prefix used when declaring an operator for a vector which takes
//two templated types
#define OPPREFIX template <class T, class U> auto operator
//The prefix used when declaring an operator for a vector which takes
//one templated type
#define OPPREFIXSINGLE template <class T> auto operator
//The prefix usef or the comparison functions because the operators are already defined in the vector header
#define NONOPPREFIX template <class T, class U> auto

//Quick use prototypes for a operator functions that acts mixes of vectors and scalars
//FUNC is the name of the function that will be called to operate on each element
//TYPE is the type of each element of the returned vector, TYPE must not be const
//  unless the vector is of const types so be careful when using decltype on an element
//  of a const vector& which which will actually return a const reference type
//CONST must be const if both arguments are const ( e.g +) otherwise leave it empty (e.g +=).
//  some versions do not have const options as the operators that use them are always const
//  (e.g. scalar followed by vector has no +=) or are never const (e.g. vector int has only
//  increment and decrement type operators).
//See operator+= and operator + for examples which use non const and const versions

#define VECVECFUNC(FUNC, CONST, TYPE)(CONST std::vector<T> &a, const std::vector<U> &b) -> std::vector<TYPE>\
{return sci_internal::operate< T, U, TYPE, sci_internal::FUNC< CONST T, const U, TYPE> >(a, b);}

#define SCALVECFUNC(FUNC, TYPE)(const T &a, const std::vector<U> &b) -> std::vector<TYPE>\
{return sci_internal::operate< T, U, TYPE, sci_internal::FUNC< const T, const U, TYPE > >(a, b);}

#define VECSCALFUNC(FUNC, CONST, TYPE) (CONST std::vector<T> &a, const U &b) -> std::vector<TYPE>\
{return sci_internal::operate< T, U, TYPE, sci_internal::FUNC< CONST T, const U, TYPE > >(a, b);}

#define VECFUNC(FUNC, CONST, TYPE) (std::vector<T> &a) -> std::vector<TYPE>\
{return sci_internal::operate< T, const int, TYPE, sci_internal::FUNC< T, const int, TYPE> >(a, 0);}

#define VECINTFUNC(FUNC, TYPE) (std::vector<T> &a, int) -> std::vector<TYPE>\
{return sci_internal::operate< T, const int, TYPE, sci_internal::FUNC< T, const int, TYPE> >(a, 0);}

OPPREFIX+VECVECFUNC(add, const, decltype(T()+U()))
OPPREFIX-VECVECFUNC(subtract, const, decltype(T()-U()))
OPPREFIX*VECVECFUNC(multiply, const, decltype(T()*U()))
OPPREFIX/VECVECFUNC(divide, const, decltype(T()/U()))
OPPREFIX+VECSCALFUNC(add, const, decltype(T()+U()))
OPPREFIX-VECSCALFUNC(subtract, const, decltype(T()-U()))
OPPREFIX*VECSCALFUNC(multiply, const, decltype(T()*U()))
OPPREFIX/VECSCALFUNC(divide, const, decltype(T()/U()))
OPPREFIX+SCALVECFUNC(add, decltype(T()+U()))
OPPREFIX-SCALVECFUNC(subtract, decltype(T()-U()))
OPPREFIX*SCALVECFUNC(multiply, decltype(T()*U()))
OPPREFIX/SCALVECFUNC(divide, decltype(T()/U()))

OPPREFIX+=VECVECFUNC(addass, , T)
OPPREFIX-=VECVECFUNC(subtractass, , T)
OPPREFIX*=VECVECFUNC(multiplyass, , T)
OPPREFIX/=VECVECFUNC(divideass, , T)
OPPREFIX+=VECSCALFUNC(addass, , T)
OPPREFIX-=VECSCALFUNC(subtractass, , T)
OPPREFIX*=VECSCALFUNC(multiplyass, , T)
OPPREFIX/=VECSCALFUNC(divideass, , T)

//OPPREFIX==VECVECFUNC(iseq, const, SBOOL)// - already defined by std::vector
//OPPREFIX<VECVECFUNC(lt, const, SBOOL)
//OPPREFIX>VECVECFUNC(gt, const, SBOOL)
//OPPREFIX<=VECVECFUNC(lteq, const, SBOOL)
//OPPREFIX>=VECVECFUNC(gteq, const, SBOOL)
namespace sci
{
	NONOPPREFIX isEq VECVECFUNC(iseq, const, SBOOL)// - use these instead
	NONOPPREFIX lt VECVECFUNC(lt, const, SBOOL)
	NONOPPREFIX gt VECVECFUNC(gt, const, SBOOL)
	NONOPPREFIX ltEq VECVECFUNC(lteq, const, SBOOL)
	NONOPPREFIX gtEq VECVECFUNC(gteq, const, SBOOL)
}
OPPREFIX==VECSCALFUNC(iseq, const, SBOOL)
OPPREFIX!=VECSCALFUNC(noteq, const, SBOOL)
OPPREFIX<VECSCALFUNC(lt, const, SBOOL)
OPPREFIX>VECSCALFUNC(gt, const, SBOOL)
OPPREFIX<=VECSCALFUNC(lteq, const, SBOOL)
OPPREFIX>=VECSCALFUNC(gteq, const, SBOOL)
OPPREFIX==SCALVECFUNC(iseq, SBOOL)
OPPREFIX!=SCALVECFUNC(noteq, SBOOL)
OPPREFIX<SCALVECFUNC(lt, SBOOL)
OPPREFIX>SCALVECFUNC(gt, SBOOL)
OPPREFIX<=SCALVECFUNC(lteq, SBOOL)
OPPREFIX>=SCALVECFUNC(gteq, SBOOL)

OPPREFIX%VECVECFUNC(remainder, const, decltype(T()%U()))
OPPREFIX%VECSCALFUNC(remainder, const, decltype(T()%U()))
OPPREFIX%SCALVECFUNC(remainder, decltype(T()%U()))
OPPREFIX%=VECVECFUNC(remainder, , T)
OPPREFIX%=VECSCALFUNC(remainder, , T)

OPPREFIXSINGLE++VECFUNC( preincrement, , T )
OPPREFIXSINGLE++VECINTFUNC( postincrement, T )
OPPREFIXSINGLE--VECFUNC( predecrement, , T )
OPPREFIXSINGLE--VECINTFUNC( postdecrement, T )
OPPREFIXSINGLE-VECFUNC( unaryminus, const, T )

OPPREFIX&&VECVECFUNC(s_and, const, SBOOL)
OPPREFIX||VECVECFUNC(s_or, const, SBOOL)
OPPREFIX&&VECSCALFUNC(s_and, const, SBOOL)
OPPREFIX||VECSCALFUNC(s_or, const, SBOOL)
OPPREFIX&&SCALVECFUNC(s_and, SBOOL)
OPPREFIX||SCALVECFUNC(s_or, SBOOL)
OPPREFIX!VECFUNC(s_not, const, SBOOL)

OPPREFIX&VECVECFUNC(bitwiseand, const, decltype(T()&U()))
OPPREFIX|VECVECFUNC(bitwiseor, const, decltype(T()|U()))
OPPREFIX^VECVECFUNC(bitwisexor, const, decltype(T()^U()))
OPPREFIX<<VECVECFUNC(bitwiseleftshift, const, decltype(T()<<U()))
OPPREFIX>>VECVECFUNC(bitwiserightshift, const, decltype(T()>>U()))
OPPREFIX&VECSCALFUNC(bitwiseand, const, decltype(T()&U()))
OPPREFIX|VECSCALFUNC(bitwiseor, const, decltype(T()|U()))
OPPREFIX^VECSCALFUNC(bitwisexor, const, decltype(T()^U()))
OPPREFIX<<VECSCALFUNC(bitwiseleftshift, const, decltype(T()<<U()))
OPPREFIX>>VECSCALFUNC(bitwiserightshift, const, decltype(T()>>U()))
OPPREFIX&SCALVECFUNC(bitwiseand, decltype(T()&U()))
OPPREFIX|SCALVECFUNC(bitwiseor, decltype(T()|U()))
OPPREFIX^SCALVECFUNC(bitwisexor, decltype(T()^U()))
OPPREFIX<<SCALVECFUNC(bitwiseleftshift, decltype(T()<<U()))
OPPREFIX>>SCALVECFUNC(bitwiserightshift, decltype(T()>>U()))

OPPREFIX&=VECVECFUNC(bitwiseandass, , T)
OPPREFIX|=VECVECFUNC(bitwiseorass, , T)
OPPREFIX^=VECVECFUNC(bitwisexorass, , T)
OPPREFIX<<=VECVECFUNC(bitwiseleftshiftass, , T)
OPPREFIX>>=VECVECFUNC(bitwiserightshiftass, , T)
OPPREFIX&=VECSCALFUNC(bitwiseandass, , T)
OPPREFIX|=VECSCALFUNC(bitwiseorass, , T)
OPPREFIX^=VECSCALFUNC(bitwisexorass, , T)
OPPREFIX<<=VECSCALFUNC(bitwiseleftshiftass, , T)
OPPREFIX>>=VECSCALFUNC(bitwiserightshiftass, , T)

OPPREFIXSINGLE~VECFUNC(bitwisenot, const, decltype(~T()))

#endif
