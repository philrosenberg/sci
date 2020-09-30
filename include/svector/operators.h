#ifndef svectoroperators_h
#define svectoroperators_h

#include<vector>
#include<stddef.h>

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
	inline V& preincrement( T &a, U &){return ++a;}
	template<class T, class U, class V>
	inline V& predecrement( T &a, U &){return --a;}
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
	inline V& addass(T &a, U &b){return a+=b;}
	template<class T, class U, class V>
	inline V& subtractass(T &a, U &b){return a-=b;}
	template<class T, class U, class V>
	inline V& multiplyass(T &a, U &b){return a*=b;}
	template<class T, class U, class V>
	inline V& divideass(T &a, U &b){return a/=b;}
	
	template<class T, class U, class V>
	inline V& remainderass(T &a, U&b){return a%=b;}
	
	template<class T, class U, class V>
	inline V& bitwiseandass(T &a, const U &b){return a&=b;}
	template<class T, class U, class V>
	inline V& bitwiseorass(T &a, const U &b){return a|=b;}
	template<class T, class U, class V>
	inline V& bitwisexorass(T &a, const U &b){return a^=b;}
	template<class T, class U, class V>
	inline V& bitwiseleftshiftass(T &a, const U &b){return a<<=b;}
	template<class T, class U, class V>
	inline V& bitwiserightshiftass(T &a, const U &b){return a>>=b;}

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

	//deals with +=, -=, *=, /=, etc for two vectors - note here the first vector is not const and we return a reference to a
	template <class T, class U, class V, V &FUNC ( T &, const U & ) > 
	std::vector<V> &operate(std::vector<T> &a, const std::vector<U> &b)
	{
		//sci::assertThrow( a.size() == b.size(), sci::err() )
		size_t len=a.size();
		if(len==0)
			return a;
		auto ai=&a[0];
		auto bi=&b[0];
		auto aiend=ai;
		aiend+=len;
		for( ; ai != aiend; ++ai, ++bi )
			FUNC(*ai,*bi); //no need for assignment, these functions do assignment themselves
		return a;
	}

	//deals with +, -, *, /, etc when the second parameter is a scalar
	//this will also deal with unary operators where the vector is const
	//as these use a dummy const int as the second parameter
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
	//this version is basically here for postfix ++ and -- operators
	//It is for unary operators (or vector then scalar operators)
	//where the vector is modified, but the return value is not a
	//reference.
	template <class T, class U, class V, V FUNC(T&, const U&) >
	std::vector<V> operate(std::vector<T>& a, const U& b)
	{
		std::vector<V> result;
		//sci::assertThrow( a.size() == b.size(), sci::err() )
		size_t len = a.size();
		result.resize(len);
		if (len == 0)
			return result;
		auto ai = &a[0];
		auto resulti = &result[0];
		auto aiend = ai;
		aiend += len;
		for (; ai != aiend; ++ai, ++resulti)
			*resulti = FUNC(*ai, b);
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

	//deals with +=, -=, *=, /=, etc when the second parameter is a scalar.
	//Note here that the vector is not const and we return a reference to the vector itself
	//this will also deal with unary operators where the vector is not const
	//as these use a dummy const int as the second parameter
	template <class T, class U, class V, V &FUNC ( T &, const U & ) > 
	std::vector<V> &operate(std::vector<T> &a, const U &b)
	{
		//sci::assertThrow( a.size() == b.size(), sci::err() )
		size_t len=a.size();
		if(len ==0)
			return a;
		auto ai=&a[0];
		auto aiend=ai;
		aiend+=len;
		for( ; ai != aiend; ++ai )
			FUNC(*ai,b); //no need for assignment, these functions do assignment themselves
		return a;
	}

	template< class ORIGVECT, class NEW >
	struct ArrayEquiv
	{
		typedef NEW type;
	};

	template< class ORIGVECT, class NEW >
	struct ArrayEquiv< std::vector<ORIGVECT>, NEW>
	{
		typedef std::vector<NEW> type;
	};

	template< class ORIGVECT, class NEW >
	struct ArrayEquiv< std::vector<std::vector<ORIGVECT>>, NEW>
	{
		typedef std::vector< typename ArrayEquiv<std::vector<ORIGVECT>, NEW>::type > type;
	};

	template<class T>
	struct BaseType
	{
		typedef T type;
	};

	template<class T>
	struct BaseType<std::vector<T>>
	{
		typedef T type;
	};

	template<class T>
	struct BaseType<std::vector<std::vector<T>>>
	{
		typedef typename BaseType< std::vector<T>>::type type;
	};

	//functions to return the right type for comparison etc
	template<class T, class U>
	SBOOL comp( T t, U u )
	{
		return 0;
	}
	template<class T, class U>
	std::vector<SBOOL> comp( std::vector<T> t, U u )
	{
		return std::vector<SBOOL>(0);
	}
	template<class T, class U>
	std::vector<SBOOL> comp( T t, std::vector<U> u )
	{
		return std::vector<SBOOL>(0);
	}
	template<class T, class U>
	auto comp( std::vector<std::vector<T>> t, U u ) -> std::vector<decltype(comp(t[0],u))>{}
	template<class T, class U>
	auto comp( T t, std::vector<std::vector<U>> u ) -> std::vector<decltype(comp(t,u[0]))>{}
	template<class T, class U>
	auto comp( std::vector<std::vector<T>> t, std::vector<U> u ) -> std::vector<decltype(comp(t[0],u[0]))>{}
	template<class T, class U>
	auto comp( std::vector<T> t, std::vector<std::vector<U>> u ) -> std::vector<decltype(comp(t[0],u[0]))>{}
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

#define VECVECFUNCREFRETURN(FUNC, CONST, TYPE)(CONST std::vector<T> &a, const std::vector<U> &b) -> std::vector<TYPE>&\
{return sci_internal::operate< T, U, TYPE, sci_internal::FUNC< CONST T, const U, TYPE> >(a, b);}

#define SCALVECFUNC(FUNC, TYPE)(const T &a, const std::vector<U> &b) -> std::vector<TYPE>\
{return sci_internal::operate< T, U, TYPE, sci_internal::FUNC< const T, const U, TYPE > >(a, b);}

#define VECSCALFUNC(FUNC, CONST, TYPE) (CONST std::vector<T> &a, const U &b) -> std::vector<TYPE>\
{return sci_internal::operate< T, U, TYPE, sci_internal::FUNC< CONST T, const U, TYPE > >(a, b);}

#define VECSCALFUNCREFRETURN(FUNC, CONST, TYPE) (CONST std::vector<T> &a, const U &b) -> std::vector<TYPE>&\
{return sci_internal::operate< T, U, TYPE, sci_internal::FUNC< CONST T, const U, TYPE > >(a, b);}

#define VECFUNC(FUNC, CONST, TYPE) (std::vector<T> &a) -> std::vector<TYPE>\
{return sci_internal::operate< T, int, TYPE, sci_internal::FUNC< CONST T, const int, TYPE> >(a, 0);}

#define VECFUNCREFRETURN(FUNC, CONST, TYPE) (std::vector<T> &a) -> std::vector<TYPE>&\
{return sci_internal::operate< T, int, TYPE, sci_internal::FUNC< CONST T, const int, TYPE> >(a, 0);}

#define VECINTFUNC(FUNC, TYPE) (std::vector<T> &a, int) -> std::vector<TYPE>\
{return sci_internal::operate< T, int, TYPE, sci_internal::FUNC< T, const int, TYPE> >(a, 0);}

#define COMMA ,

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

OPPREFIX+=VECVECFUNCREFRETURN(addass, , T)
OPPREFIX-=VECVECFUNCREFRETURN(subtractass, , T)
OPPREFIX*=VECVECFUNCREFRETURN(multiplyass, , T)
OPPREFIX/=VECVECFUNCREFRETURN(divideass, , T)
OPPREFIX+=VECSCALFUNCREFRETURN(addass, , T)
OPPREFIX-=VECSCALFUNCREFRETURN(subtractass, , T)
OPPREFIX*=VECSCALFUNCREFRETURN(multiplyass, , T)
OPPREFIX/=VECSCALFUNCREFRETURN(divideass, , T)

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
	NONOPPREFIX isEq VECSCALFUNC(iseq, const, SBOOL)
	NONOPPREFIX notEq VECSCALFUNC(noteq, const, SBOOL)
	NONOPPREFIX lt VECSCALFUNC(lt, const, SBOOL)
	NONOPPREFIX gt VECSCALFUNC(gt, const, SBOOL)
	NONOPPREFIX ltEq VECSCALFUNC(lteq, const, SBOOL)
	NONOPPREFIX gtEq VECSCALFUNC(gteq, const, SBOOL)
	NONOPPREFIX isEq SCALVECFUNC(iseq, SBOOL)
	NONOPPREFIX notEq SCALVECFUNC(noteq, SBOOL)
	NONOPPREFIX lt SCALVECFUNC(lt, SBOOL)
	NONOPPREFIX gt SCALVECFUNC(gt, SBOOL)
	NONOPPREFIX ltEq SCALVECFUNC(lteq, SBOOL)
	NONOPPREFIX gtEq SCALVECFUNC(gteq, SBOOL)
}
//OPPREFIX==VECSCALFUNC(iseq, const, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
//OPPREFIX!=VECSCALFUNC(noteq, const, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
OPPREFIX<VECSCALFUNC(lt, const, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
OPPREFIX>VECSCALFUNC(gt, const, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
OPPREFIX<=VECSCALFUNC(lteq, const, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
OPPREFIX>=VECSCALFUNC(gteq, const, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
//OPPREFIX==SCALVECFUNC(iseq, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
//OPPREFIX!=SCALVECFUNC(noteq, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
OPPREFIX<SCALVECFUNC(lt, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
OPPREFIX>SCALVECFUNC(gt, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
OPPREFIX<=SCALVECFUNC(lteq, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
OPPREFIX>=SCALVECFUNC(gteq, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)

OPPREFIX%VECVECFUNC(remainder, const, decltype(T()%U()))
OPPREFIX%VECSCALFUNC(remainder, const, decltype(T()%U()))
OPPREFIX%SCALVECFUNC(remainder, decltype(T()%U()))
OPPREFIX%=VECVECFUNC(remainder, , T)
OPPREFIX%=VECSCALFUNC(remainder, , T)

OPPREFIXSINGLE++VECFUNCREFRETURN( preincrement, , T )
OPPREFIXSINGLE++VECINTFUNC( postincrement, T )
OPPREFIXSINGLE--VECFUNCREFRETURN( predecrement, , T )
OPPREFIXSINGLE--VECINTFUNC( postdecrement, T )
OPPREFIXSINGLE-VECFUNC( unaryminus, const, T )

OPPREFIX&&VECVECFUNC(s_and, const, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
OPPREFIX||VECVECFUNC(s_or, const, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
OPPREFIX&&VECSCALFUNC(s_and, const, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
OPPREFIX||VECSCALFUNC(s_or, const, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
OPPREFIX&&SCALVECFUNC(s_and, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
OPPREFIX||SCALVECFUNC(s_or, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type)
OPPREFIXSINGLE!VECFUNC(s_not, const, typename sci_internal::ArrayEquiv<T COMMA SBOOL>::type )

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
