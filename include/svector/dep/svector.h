#ifndef svector_h
#define svector_h


//2*M_PI
#define M_2PI     6.28318530717958647692
//sqrt(M_PI)
#define M_SQRTPI  1.77245385090551602729
//sqrt(2.0*M_PI)
#define M_SQRT2PI 2.50662827463100050241

#include<vector>
#include<limits>
#include<algorithm>
#include<numeric>
#include<complex>
#include<cmath>
#include<type_traits>
#include<random>
#include"../serr.h"

/*#ifdef WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG   
#ifndef DBG_NEW      
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )      
#define new DBG_NEW   
#endif
#endif  // _DEBUG
#endif*/


/*#include <alg/ap.h>
#include <alg/fasttransforms.h>
#include <alg/interpolation.h>*/

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
//****************************************************************
//Copyright R-Space
//Version 1.0
//26/9/2011
//****************************************************************




#ifndef SBOOL
#define SBOOL char
#endif

namespace sci
{
}


namespace sci
{
	size_t randInt(size_t maxVal);
	double round(double n);
	double linearinterpolate(double x, double x1, double x2, double y1, double y2);
	
	//enumeration of base types
	enum basetype
	{
		SBASE_UNKNOWN=0,
		SBASE_DOUBLE=1,
		SBASE_FLOAT=2,
		SBASE_LONGDOUBLE=3,
		SBASE_BOOL=4,
		SBASE_SIGNEDCHAR=5,
		SBASE_UNSIGNEDCHAR=6,
		SBASE_WCHAR=7,
		SBASE_SIGNEDSHORT=8,
		SBASE_UNSIGNEDSHORT=9,
		SBASE_SIGNEDINT=10,
		SBASE_UNSIGNEDINT=11,
		SBASE_SIGNEDLONG=13,
		SBASE_UNSIGNEDLONG=14,
		SBASE_COMPLEX=15,
		SBASE_STRING=16
	};

	//quick iterator
	template<class T>
	class qic
	{
	public:
		inline qic(const std::vector<T> &v){m_start=&v[0];}
		inline const T& operator[](size_t index) const 
		{
			return *(m_start+index);
		}
	private:
		const T *m_start;
	};
	//quick const iterator
	template<class T>
	class qi
	{
	public:
		inline qi(std::vector<T> &v){m_start=&v[0];}
		inline T& operator[](size_t index) const 
		{
			return *(m_start+index);
		}
	private:
		T *m_start;
	};
	
	//********************************************************
	//****struct which gives shorthand notation for **********
	//****delcaring multidimensional vectors        **********
	//********************************************************

	//you can use this to declare different dimension vectors as 
	//1d - sci::vector<double>::d1
	//2d - sci::vector<double>::d2
	//3d - sci::vector<double>::d3
	//etc up to 10 dimensions
	template<class T>
	struct vector
	{
		typedef std::vector<T> d1;
		typedef std::vector<std::vector<T> > d2;
		typedef std::vector<std::vector<std::vector<T> > > d3;
		typedef std::vector<std::vector<std::vector<std::vector<T> > > > d4;
		typedef std::vector<std::vector<std::vector<std::vector<std::vector<T> > > > > d5;
		typedef std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<T> > > > > > d6;
		typedef std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<T> > > > > > > d7;
		typedef std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<T> > > > > > > > d8;
		typedef std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<T> > > > > > > > > d9;
		typedef std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<T> > > > > > > > > > d10;
	};


	
	//********************************************************
	//**********Functions*************************************
	//********************************************************

	//********************************************************
	//*************General vector helpers*********************
	//********************************************************
	
	//concatenate two vectors
	template <class T>
	std::vector<T> concat(const std::vector<T> &a, const std::vector<T> &b)
	{
		std::vector<T> result(a.size()+b.size());
		std::copy(a.begin(),a.end(),result.begin());
		std::copy(b.begin(),b.end(),result.begin()+a.size());
		return result;
	}

	//concatenate a scalar and then a vector
	template <class T>
	std::vector<T> concat(const T &a, const std::vector<T> &b)
	{
		std::vector<T> result=b;
		result.insert(result.begin(),a);
		return result;
	}

	//concatenate a vector and then a scalar
	template <class T>
	std::vector<T> concat(const std::vector<T> &b, const T &a)
	{
		std::vector<T> result=b;
		result.push_back(a);
		return result;
	}

	//concatenate two scalars into a vector creating a new dimension
	template <class T>
	std::vector<T> concatnewd(const T &a, const T &b)
	{
		std::vector<T> result(2);
		result[0]=a;
		result[1]=b;
		return result;
	}

	template <class T>
	bool rectangular(const std::vector< std::vector<T> > &v)
	{
		if (v.size() == 0 || v.size() == 1) return true;
		size_t size = v[0].size();
		const std::vector<T> *vEnd = &v[0] + v.size();
		for (const std::vector<T> *vi = &v[1]; vi != vEnd; ++vi)
		{
			if (vi->size() != size)
				return false;
		}
		return true;
	}

	template <class T>
	bool square(const std::vector< std::vector<T> > &v)
	{
		if (v.size()==0) return true;
		size_t size=v.size();
		const std::vector<T> *vEnd = &v[0] + v.size();
		for(const std::vector<T> *vi = &v[0]; vi != vEnd; ++vi)
		{
			if(vi->size()!=size) return false;
		}
		return true;
	}


	//creates a vector from a c style array
	template<class T>
	std::vector<T> vectorfromarray(size_t nelements, T *data)
	{
		std::vector<T> result(nelements);
		memcpy(&result[0],data,nelements*sizeof(T));
		return result;
	}

	//checks if the vector has enough reserved space for the element then adds it to the
	//back of the vector. If there is no spare reserved space then this function doubles
	//the reserved space. This increases the speed with which a large data set can
	//be built up element-by-element by avoiding having to copy the entire vector for
	//every push_back call at the expense of using up to double the memory required
	//for the vector.
	template<class T>
	void reservedpush_back(std::vector<T> &v, const T &val)
	{
		size_t cap=v.capacity();
		size_t max=v.max_size();
		size_t size=v.size();
		if(v.capacity()==0) v.reserve(2);
		else if((v.size())==v.capacity()) 
		{
			v.reserve(std::min(v.capacity()*2,v.max_size()));
		}
		v.push_back(val);
	}

	//as above but adds multiple copies
	template<class T>
	void reservedpush_back(std::vector<T> &v, const T &val, size_t n_copies)
	{
		if(v.capacity()==0) v.reserve(n_copies*2);
		else if((v.size()+n_copies)>=v.capacity()) 
		{
			v.reserve(std::min(v.capacity()*2,v.max_size()));
		}
		v.insert(v.end(),n_copies,val);
	}

	//as above push pushes back a whole vector
	template<class T>
	void reservedpush_back(std::vector<T> &v, const std::vector<T> &val)
	{
		if(v.capacity()==0) v.reserve(val.size()*2);
		else if((v.size()+val.size())>=v.capacity()) 
		{
			v.reserve(std::min(v.capacity()*2,v.max_size()));
		}
		v.insert(v.end(),val.begin(),val.end());
	}
	
	//as above push pushes back a subvector
	template<class T>
	void reservedpush_back(std::vector<T> &v, const std::vector<T> &val, size_t begin, size_t n_elements)
	{
		if(v.capacity()==0) v.reserve(n_elements*2);
		else if ((v.size()+n_elements)>=v.capacity())
		{
			v.reserve(std::min(v.capacity()*2,v.max_size()));
		}
		if(begin>val.size()) v.insert(v.end(),n_elements,T());
		else if((begin+n_elements)>val.size())
		{
			v.insert(v.end(),val.begin()+begin,val.end());
			v.insert(v.end(),n_elements-(val.end()-(val.begin()+begin)),T());
		}
		else v.insert(v.end(),val.begin()+begin,val.begin()+begin+n_elements);
	}

	template <class T>
	size_t maxsize(const std::vector< std::vector<T> > &v)
	{
		if (v.size()==0) return 0;
		size_t maxsize=v[0].size();
		for(typename std::vector<T>::const_iterator vi=v.begin()+1; vi!=v.end(); ++vi)
		{
			maxsize=max(maxsize,vi->size());
		}
		return maxsize;
	}
	
	//set all values in a vector to the given value
	template <class T, class U>
	inline void setallvalues(std::vector<T> &v, const U &val)
	{
		if(v.size()==0) return;
		for(typename std::vector<T>::iterator vi=v.begin(); vi!=v.end(); ++vi) *vi=val;
	}
	template <class T, class U>
	inline void setallvalues(std::vector< std::vector<T> > &v, const U &val)
	{
		if(v.size()==0) return;
		for(typename std::vector< std::vector<T> >::iterator vi=v.begin(); vi!=v.end(); ++vi) setallvalues(*vi,val);
	}

	template <class T>
	size_t minsize(const std::vector< std::vector<T> > &v)
	{
		if (v.size()==0) return 0;
		size_t minsize=v[0].size();
		for(typename std::vector<T>::const_iterator vi=v.begin()+1; vi!=v.end(); ++vi)
		{
			minsize=min(minsize,vi->size());
		}
		return minsize;
	}
	

	//return the number of dimensions in a scalar
	template<class T>
	inline size_t ndims(const T &v)
	{
		return 0;
	}
	//return the number of dimensions in a vector v
	template<class T>
	inline size_t ndims(const std::vector< T > &v)
	{
		return 1;
	}
	//return the number of dimensions in a multi dimension vector v
	template<class T>
	size_t ndims(const std::vector< std::vector< T > > &v)
	{
		return (v.size()>0?ndims(v[0]):ndims(std::vector<T>()))+1;
	}
	
	//create a 2 d vector with dimensions as given
	template <class T>
	std::vector <std::vector <T> > makevector(T fillvalue, size_t dim1, size_t dim2)
	{
		std::vector <std::vector <T> > result(dim1);
		if (dim1 == 0)
			return result;

		std::vector<T> *resultEnd = &result[0] + result.size();
		for(std::vector<T> *resulti=&result[0]; resulti!=resultEnd; ++resulti) 
			resulti->resize(dim2,fillvalue);
		return result;
	}

	//create a subvector from a vector
	template <class T>
	std::vector <T>  subvector(const std::vector<T> &src, size_t first_elem, size_t length)
	{
		if(first_elem>=src.size()) return std::vector<T>(0);
		typename std::vector<T>::const_iterator first=src.begin()+first_elem;
		typename std::vector<T>::const_iterator last;

		if(first_elem+length>src.size()) last=src.end();
		else last=src.begin()+first_elem+length;

		return std::vector<T>(first,last);
	}

	//create a subvector from specified elements of a vector
	template <class T>
	std::vector <T>  subvector(const std::vector<T> &src, const std::vector<size_t> &elements)
	{
		std::vector<T> result(elements.size());
		for(size_t i=0; i<elements.size(); ++i) result[i]=elements[i]<src.size() ? src[elements[i]] : std::numeric_limits<T>::quiet_NaN();
		return result;
	}

	//create a subvector from a vector of values and a vector of bools which indicates if the value should be used
	template <class T>
	std::vector <T>  subvector(const std::vector<T> &src, const std::vector<bool> &included)
	{
		if(src.size()!=included.size()) return std::vector<T>(0);
		size_t count=0;
		for(typename std::vector<bool>::const_iterator inci=included.begin(); inci!=included.end(); ++inci )
		{
			if(*inci) ++count;
		}
		std::vector<T> result=src;
		return result;
	}
	
	//create a subvector from a vector of values and a vector of chars which indicates if the value should be used
	//a char value of 0 indicates not included anything else indicates included. This is hoped to be faster than
	//the bool implimentation but uses more memory
	template <class T>
	std::vector <T>  subvector(const std::vector<T> &src, const std::vector<SBOOL> &included, bool sparse=false)
	{
		if(src.size()!=included.size() || included.size()==0) return std::vector<T>(0);
		size_t count=0;
		const SBOOL *includedi=&included[0];
		const SBOOL *includedend=includedi+included.size();
		for(; includedi!=includedend; ++includedi )
		{
			if(*includedi) ++count;
		}
		std::vector<T> result=src;
		result.resize(count);
		if(count==0) return result;

		includedi=&included[0];
		const T *srci=&src[0];

		if(sparse)
		{
			T *insertpoint=&result[0];
			T *resultend=insertpoint+result.size();
			for(; includedi!=includedend; ++includedi)
			{
				if(insertpoint==resultend) break;
				if(*includedi)
				{
					*insertpoint=*srci;
					++insertpoint;
				}
				++includedi;
				++srci;
			}

		}
		else
		{
			T *insertpoint=&result[0];
			//cycle through again picking out the needed sections
			while(includedi!=includedend)
			{
				SBOOL copying=*includedi;
				const SBOOL *includedemptybegin=includedi;
				//scan through any false points
				while(!copying&&includedi!=includedend) 
				{
					++includedi;
					copying=*includedi;
				}
				size_t emptylen=includedi-includedemptybegin;
				srci+=emptylen;
				//when we find a true section record the start and end
				const T *sectbegin=srci;
				const SBOOL *includedsectbegin=includedi;
				while(copying&&includedi!=includedend) 
				{
					++includedi;
					copying=*includedi;
				}
				size_t sectlen=includedi-includedsectbegin;
				const T *sectend=srci+sectlen;
#ifdef _MSC_VER
#pragma warning( suppress : 4996 )
#endif
				std::copy(sectbegin,sectend,insertpoint);
				insertpoint+=sectlen;
				srci+=sectlen;
			}
		}
		
		return result;
	}

	//create a subvector from a vector of values and a multi-d vector of bools which indicates if the value should be used
	template <class T,class U>
	std::vector< std::vector<T> >  subvector(const std::vector< std::vector<T> > &src, const std::vector< std::vector<U> > &included)
	{
		std::vector< std::vector<T> > result(included.size());
		for(size_t i=0; i<std::min(result.size(),src.size()); ++i) result[i]=subvector(src[i],included[i]);
		return result;
	}
	
	//create a subvector from a multi-d vector and a 1-d vector of bools to be applied to the specified dimension
	//note that dimension is zero indexed so should be 0 to apply to the first dimension, 1 to apply to the second etc.
	template <class T>
	std::vector<std::vector<T>> subvector(const std::vector<std::vector<T>> &src, const std::vector<bool> &included, size_t dimension)
	{
		
		if(sci::ndims(src)<dimension-1) return std::vector<std::vector<T>>(0);
		if(sci::ndims(src)==dimension-1)
		{
			if(src.size()!=included.size()) return std::vector<std::vector<T>>(0);
			size_t count=0;
			for(typename std::vector<bool>::const_iterator inci=included.begin(); inci!=included.end(); ++inci)
			{
				if(*inci) ++count;
			}

			std::vector<std::vector<T>> result(count);
			typename std::vector<std::vector<T>>::iterator resulti=result.begin();
			typename std::vector<std::vector<T>>::const_iterator srci=src.begin();

			for(typename std::vector<bool>::const_iterator inci=included.begin(); inci!=included.end(); ++inci)
			{
				if(*inci) 
				{
					*resulti=*srci;
					++resulti;
				}
				++srci;
			}
			return result;
		}
		else
		{
			std::vector<std::vector<T>> result(src.size());
			for(size_t i=0; i<result.size(); ++i)
			{
				result[i]=sci::subvector(src[i],included,dimension-1);
			}
			return result;
		}
	}

	template <class T>
	std::vector<T> subvector(const std::vector<T> &src, const std::vector<bool> &included, size_t dimension)
	{
		if(dimension!=0) return std::vector<T>(0);
		else return sci::subvector(src,included);
	}
	

	//create a vector with each element equal to the vector index
	template<class T>
	std::vector<T> indexvector(size_t size)
	{
		std::vector<T>result(size);
		size_t val=0;
		for(typename std::vector<T>::iterator resulti=result.begin(); resulti!=result.end(); ++resulti)
		{
			*resulti=(T)val;
			++val;
		}
		return result;
	}

	//transposes a 2D vector.
	template <class T>
	std::vector< std::vector<T> > transpose(const std::vector< std::vector<T> > &v)
	{
		if (v.size() == 0)
			return std::vector<std::vector<T>>(0);
		sci::assertThrow(sci::rectangular(v), sci::err(SERR_VECTOR, -1, "sci::transpose called with a vector that isn't rectangular."));


		std::vector< std::vector<T> > result;
		std::vector<const T*> transposed(v.size());

		const std::vector <T> *vEnd = &v[0] + v.size();
		const T **transposedi = &transposed[0];
		for (const std::vector <T> *vi = &v[0]; vi != vEnd; ++vi, ++transposedi)
		{
			*transposedi = &((*vi)[0]);
		}
		result = sci::makevector<T>(0.0, v[0].size(), v.size());

		std::vector <T> *resultEnd = &result[0] + result.size();
		for (std::vector <T> *resulti = &result[0]; resulti != resultEnd; ++resulti)
		{
			const T **transposedi = &transposed[0];
			T *resultiEnd = &((*resulti)[0]) + resulti->size();
			for (T *resultij = &((*resulti)[0]); resultij != resultiEnd; ++transposedi, ++resultij)
			{
				*resultij = **transposedi;
				++*transposedi;
			}
		}
		return result;
	}

	//create vector with multiple dimensions filled with a constant value
	template<class T>
	std::vector<T>  vector1d(size_t size1, const T &value)
	{
		std::vector< T >result(size1,value);
		return result;
	}
	template<class T>
	std::vector< std::vector<T> > vector2d(size_t size1, size_t size2, const T &value)
	{
		std::vector< std::vector<T> >result(size1);
		for(typename std::vector< std::vector<T> >::iterator resulti=result.begin(); resulti!=result.end(); ++resulti) 
			resulti->resize(size2,value);
		return result;
	}
	
	//These functions reshape, access and resize vectors which have a number of dimensions which
	//are unknown at the time of writing the library but will be known by the user at compile time
	//Note that you can pass in a multi-d vector, but only the first dimension will be reshaped
	template<class T, class A>
	bool reshape(std::vector<T> &result, const std::vector<A> &v, const std::vector<size_t> &shape)
	{
		
		size_t expectedlength=1;
		for(typename std::vector<size_t>::const_iterator shapei=shape.begin(); shapei!=shape.end(); ++shapei) 
			expectedlength*=*shapei;
		if(expectedlength!=v.size())
		{
			return false;
		}
		size_t sublength=1;
		for(typename std::vector<size_t>::const_iterator shapei=shape.begin()+1; shapei!=shape.end(); ++shapei) 
			sublength*=*shapei;
		std::vector<size_t> nextshape(shape.begin()+1,shape.end());
		result.resize(shape[0]);
		for(size_t i=0; i<shape[0]; ++i) 
		{
			std::vector<A> subv(v.begin()+i*sublength,v.begin()+i*sublength+sublength);
			reshape(*(result.begin()+i),subv,nextshape);
		}
		return true;
	}

	template<class T>
	inline bool reshape(std::vector<T> &result, std::vector<T> v, std::vector<size_t> shape)
	{
		result=v;
		return true;
	}

	template<class T>
	size_t nelements(const std::vector<std::vector<T> > &v)
	{
		size_t result=0;
		for(typename std::vector<std::vector<T> >::const_iterator vi=v.begin(); vi!=v.end(); ++vi) result+=nelements(*vi);
		return result;
	}

	template<class T>
	size_t nelements(const std::vector<T> &v)
	{
		return v.size();
	}

	template<class T,class A>
	void flatten(std::vector<T> &result, const std::vector< std::vector<A> > &v)
	{
		if(v.size()==0)
		{
			result.resize(0);
			return;
		}

		result.reserve(nelements(v));
		flatten(result,*v.begin());
		std::vector<T> nextsection;
		for(size_t i=1; i<v.size(); ++i)
		{
			flatten(nextsection,*(v.begin()+i));
			result.insert(result.end(),nextsection.begin(),nextsection.end());
		}
	}


	template<class T>
	inline void flatten(std::vector<T> &result, const  std::vector<T>  &v)
	{
		result=v;
	}

	//access an element of a mulitidimensional vector
	template<class T, class A>
	inline T& at(std::vector< std::vector< A > > &v, const std::vector<size_t> &indices)
	{
		return at<T>(v, indices.begin(), std::min( sci::ndims(v), indices.size()) );
	}
	template<class T, class A>
	inline T& at(std::vector< std::vector< A > > &v, std::vector<size_t>::const_iterator firstindex, size_t numdims)
	{
		//if(numdims==1) return std::numeric_limits<T>::quiet_NaN();
		return at <T>(*(v.begin()+*firstindex),firstindex+1,numdims-1);
	}
	template<class T, class A>
	inline T& at(std::vector<A> &v, std::vector<size_t>::const_iterator firstindex, size_t numdims)
	{
		return *(v.begin()+*firstindex);
	}

	//resize a multidimensional vector
	template<class T>
	void resize(std::vector< std::vector<T> > &v, const std::vector<size_t> &size)
	{
		size_t numdims=sci::ndims(v);
		if(size.size()<numdims)
		{
			std::vector<size_t> newsize=size;
			newsize.resize(numdims,0);
			resize(v,newsize.begin());
		}
		else resize(v,size.begin());
	}
	template<class T>
	void resize(std::vector< std::vector<T> > &v, std::vector<size_t>::const_iterator firstindex)
	{
		v.resize(*firstindex);
		if(*firstindex==0) return;
		for(typename std::vector< std::vector<T> >::iterator vi=v.begin(); vi!=v.end(); ++vi)resize(*vi,firstindex+1);

	}
	template<class T>
	void resize(std::vector<T> &v, std::vector<size_t>::const_iterator firstindex)
	{
		v.resize(*firstindex);
	}

	template<class T, class U>
	struct sameNDims
		: std::true_type
	{
	};
	template<class T, class U>
	struct sameNDims<std::vector<T>,U>
		: std::false_type
	{
	};
	template<class T, class U>
	struct sameNDims<T,std::vector<U>>
		: std::false_type
	{
	};
	template<class T, class U>
	struct sameNDims<std::vector<T>,std::vector<U>>
		: sameNDims<decltype(T()), decltype(U())>
	{
	};

	//return the shape of a multi dimension vector v
	template<class T>
	std::vector<size_t> shape(const std::vector< std::vector< T > > &v)
	{
		size_t thissize=v.size();
		std::vector<size_t> othershape;
		if (thissize > 0)
		{
			othershape = sci::shape(v[0]);
			for (size_t i = 1; i < v.size(); ++i)
			{
				sci::assertThrow(othershape == sci::shape(v[i]), sci::err(SERR_VECTOR, -999, "sci::shape called with some elements of different size to tthose in the same dimension."));
			}
		}
		else
			othershape=std::vector<size_t>(sci::ndims(v[0]),0);
		othershape.insert(othershape.begin(),thissize);
		return othershape;
	}
	template<class T>
	inline std::vector<size_t> shape(const std::vector< T > &v)
	{
		return std::vector<size_t>(1,v.size());
	}

	//return the base type of a multi dimensional vector
	template<class T>
	inline sci::basetype getbasetype(const std::vector< std::vector< T > > &v)
	{
		if(v.size()>0 && v[0].size()>0) return getbasetype(v[0]);//use the zeroth element and get the basetype of that
		//note we don't use v[0][0] as this might mean we miss calling the 1d vector version
		else return getbasetype (std::vector<T>()); //else create a new vector of Ts and check the base type of that
	}
	//return the base type of a vector, if this function gets called and not a specialisation then
	//we have an unknown type, return 0;
	template<class T>
	inline sci::basetype getbasetype(const std::vector< T > &v)
	{
		return sci::SBASE_UNKNOWN;
	}
	//specialisations of the 1d version 
	//template<class T>
	inline sci::basetype getbasetype(const std::vector< double > &v)
	{
		return sci::SBASE_DOUBLE;
	}
	//template<class T>
	inline sci::basetype getbasetype(const std::vector< float > &v)
	{
		return sci::SBASE_FLOAT;
	}
	//template<class T>
	inline sci::basetype getbasetype(const std::vector< long double > &v)
	{
		return sci::SBASE_LONGDOUBLE;
	}
	//template<class T>
	inline sci::basetype getbasetype(const std::vector< bool > &v)
	{
		return sci::SBASE_BOOL;
	}
	//template<class T>
	inline sci::basetype getbasetype(const std::vector< signed char > &v)
	{
		return sci::SBASE_SIGNEDCHAR;
	}
	//template<class T>
	inline sci::basetype getbasetype(const std::vector< unsigned char > &v)
	{
		return sci::SBASE_UNSIGNEDCHAR;
	}
	//template<class T>
	inline sci::basetype getbasetype(const std::vector< wchar_t > &v)
	{
		return sci::SBASE_WCHAR;
	}
	//template<class T>
	inline sci::basetype getbasetype(const std::vector< signed short > &v)
	{
		return sci::SBASE_SIGNEDSHORT;
	}
	//template<class T>
	inline sci::basetype getbasetype(const std::vector< unsigned short > &v)
	{
		return sci::SBASE_UNSIGNEDSHORT;
	}
	//template<class T>
	inline sci::basetype getbasetype(const std::vector< signed int > &v)
	{
		return sci::SBASE_SIGNEDINT;
	}
	//template<class T>
	inline sci::basetype getbasetype(const std::vector< unsigned int > &v)
	{
		return sci::SBASE_UNSIGNEDINT;
	}
	//template<class T>
	inline sci::basetype getbasetype(const std::vector< signed long > &v)
	{
		return sci::SBASE_SIGNEDLONG;
	}
	//template<class T>
	inline sci::basetype getbasetype(const std::vector< unsigned long > &v)
	{
		return sci::SBASE_UNSIGNEDLONG;
	}
	template<class T>
	inline sci::basetype getbasetype(const std::vector< std::complex<T> > &v)
	{
		return sci::SBASE_COMPLEX;
	}
	//template<class T>
	inline sci::basetype getbasetype(const std::vector< std::string > &v)
	{
		return sci::SBASE_STRING;
	}

	//recast one vector type to another. The destination vector will be resized
	//to the same size as source if needed
	template<class T, class U>
	inline void convert(std::vector<T> &destination, const std::vector<U> &source)
	{
		destination.resize(source.size());
		if (source.size() == 0)
			return;
		if(destination.size()>0)
		{
			T * di = &destination[0];
			const  U  *sEnd = &source[0]+source.size();
			for(const  U  *si = &source[0]; si!=sEnd; ++si)
			{
				*di=static_cast<T>(*si);
				++di;
			}
		}
	}
	
	//recast one multi-d vector type to another. The destination vector will be resized
	//to the same size as source if needed
	template<class T, class U>
	inline void convert(std::vector< std::vector<T> > &destination, const std::vector< std::vector<U> > &source)
	{
		destination.resize(source.size());
		if (source.size() == 0)
			return;
		if(destination.size()>0)
		{
			std::vector<T> * di=&destination[0];
			const std::vector<U> *sEnd=&source[0]+source.size();
			for(const std::vector<U> *si=&source[0]; si!=sEnd; ++si)
			{
				convert(*di,*si);
				++di;
			}
		}
	}

	//this should never get called, it is to catch any odd conversions that
	//need to be compiled ut will never run
	template<class T, class U>
	inline void convert(T &destination, const U &source)
	{
	}
	
	//Uses the Fisher Yates method to shuffle a vector in place
	template<class T>
	void shuffle(std::vector<T> &v)
	{
		T* pbegin=&v[0];
		T* pi;
		T* pj;
		for(pi=pbegin+v.size()-1; pi>pbegin; --pi)
		{
			pj=pbegin+sci::randInt(pi-pbegin+1);
			std::swap(pi,pj);
		}
	}

			
	//********************************************************
	//**********Functions giving information******************
	//********************************************************


	//********************************************************
	//**********statistics of the vector**********************
	//********************************************************	
	
	template<class T>
	T sum(const std::vector<T> &v)
	{
		if(v.size()==0) return T(0.0); //this handilly returns the value zero or if T is a vector a zero size vector
		T result=v[0];
		for(typename std::vector<T>::const_iterator vi=v.begin()+1; vi!=v.end(); ++vi) result+=*vi;
		//std::accumulate(v.begin(), v.end(),result);
		return result;
	}
	
	template<class T>
	std::vector<T> sum(const std::vector<std::vector<T>> &v)
	{
		if(v.size()==0) return std::vector<T>(0);
		std::vector<T> result(v[0].size(),0.0);
		for(typename std::vector<std::vector<T>>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) result+=*vi;
		//std::accumulate(v.begin(), v.end(),result);
		return result;
	}

	template<class T>
	T product(const std::vector<T> &v)
	{
		if(v.size()==0) return 0;
		T result=T(1.0);
		for(typename std::vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) result*=*vi;
		//std::accumulate(v.begin(), v.end(),result);
		return result;
	}

	template<class T>
	std::vector<T> product(const std::vector<std::vector<T>> &v)
	{
		if(v.size()==0) return std::vector<T>(0);
		std::vector<T> result(v[0].size(), 1.0);
		for(typename std::vector<std::vector<T>>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) result*=*vi;
		//std::accumulate(v.begin(), v.end(),result);
		return result;
	}

	template<class T>
	T anyBaseVal(const std::vector<T> &v)
	{
		T val;
		return val;
	}
	template<class T>
	auto anyBaseVal(const std::vector<std::vector<T>> &v) -> decltype( anyBaseVal(T(0)) )
	{
		if(v.size()>0)
			return anyBaseVal(T(0));
		else
			return anyBaseVal(T(0));
	}

	template<class T>
	T mean(const std::vector<T> &v)
	{
		if(v.size()==0) return std::numeric_limits<T>::quiet_NaN();
		T result=v[0];			
		const T *vi=&v[0]+1;
		const T *vilimit=&v[0]+v.size();
		for(; vi<vilimit; ++vi) 
			result+=*vi;
		double count=double(v.size());
		result/=count;
		return result;


		//T result(0.0);
		//for(std::vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) result+=*vi;
		//std::accumulate(v.begin(), v.end(),result);
		//return result/(T)v.size();
	}

	template<class T>
	T mean(const std::vector<T> &v, const std::vector<T> &weights)
	{
		if(v.size()==0) return std::numeric_limits<T>::quiet_NaN();
		sci::assertThrow( v.size() == weights.size(), sci::err() );
		T result=0.0;
		T weight=0.0;
		const T *vi=&v[0];
		const T *vilimit=&v[0]+v.size();
		const T *weighti=&weights[0];
		for(; vi<vilimit; ++vi,++weighti)
		{
			result += *vi * *weighti;
			weight += *weighti;
		}
		return result/weight;
	}

	template<class T>
	T meanIgnoreNans(const std::vector<T> &v)
	{
		if(v.size()==0) return std::numeric_limits<T>::quiet_NaN();
		T result(0.0);
		size_t count=0;
		const T *vi=&v[0];
		const T *vilimit=vi+v.size();
		for(; vi<vilimit; ++vi)
		{
			if(*vi==*vi)
			{
				result+=*vi;
				++count;
			}
		}
		return result/decltype( anyBaseVal(v) )(count);


		//T result(0.0);
		//for(std::vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) result+=*vi;
		//std::accumulate(v.begin(), v.end(),result);
		//return result/(T)v.size();
	}

	template<class T>
	T meanIgnoreNans(const std::vector<T> &v, const std::vector<T> &weights)
	{
		if(v.size()==0) return std::numeric_limits<T>::quiet_NaN();
		sci::assertThrow( v.size() == weights.size(), sci::err(SERR_VECTOR, 0, "When performing a weighted mean the weights must have the same size as the data.") );
		T result=0.0;
		T weight=0.0;
		const T *vi=&v[0];
		const T *vilimit=&v[0]+v.size();
		const T *weighti=&weights[0];
		for(; vi<vilimit; ++vi,++weighti)
		{
			if( *vi == *vi )
			{
				result += *vi * *weighti;
				weight += *weighti;
			}
		}
		return result/weight;
	}

	template<class T>
	std::vector<T> mean(const std::vector<std::vector<T>> &v)
	{
		if(v.size()==0) return std::vector<T>(0);
		std::vector<T> result(v[0].size(), 0.0);
		for(typename std::vector<std::vector<T>>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) result+=*vi;
		return result/decltype( anyBaseVal(v) )(v.size());
	}

	template<class T>
	T centralmoment(int moment, const std::vector<T> &v, const T &mean)
	{
		T result(0.0);
		for(typename std::vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
			result+=std::pow((*vi-mean), moment);
		return result/decltype( anyBaseVal(v) )(v.size()-1);
	}

	template<class T>
	T centralmomentnobessel(int moment, const std::vector<T> &v, const T &mean)
	{
		T result(0.0);
		for(typename std::vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
			result+=std::pow((*vi-mean), moment);
		return result/decltype( anyBaseVal(v) )(v.size());
	}

	template<class T>
	T variance(const std::vector<T> &v)
	{
		if(v.size()==0) 
			return std::numeric_limits<T>::quiet_NaN();
		T meanval=sci::mean(v);
		T result(0.0);
		for(typename std::vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
			result+=(*vi-meanval)*(*vi-meanval);
		//std::accumulate(v.begin(), v.end(),result);
		return result/decltype( anyBaseVal(v) )(v.size()-1);
	}

	template<class T>
	T varianceIgnoreNans(const std::vector<T> &v)
	{
		if(v.size()==0) 
			return std::numeric_limits<T>::quiet_NaN();
		T meanval=sci::meanIgnoreNans(v);
		T result(0.0);
		const T *vi = &vi[0];
		const T *vEnd =vi + v.size();
		size_t count =0;
		for(; vi != vEnd; ++vi)
		{
			if( *vi == *vi )
			{
				result+=(*vi-meanval)*(*vi-meanval);
				++count;
			}
		}
		//std::accumulate(v.begin(), v.end(),result);
		return result/decltype( anyBaseVal(v) )( count - 1 );
	}

	template<class T>
	T variance(const std::vector<T> &v, const T &mean)
	{
		if(v.size()==0) 
			return std::numeric_limits<T>::quiet_NaN();
		T result(0.0);
		for(typename std::vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
			result+=(*vi-mean)*(*vi-mean);
		//std::accumulate(v.begin(), v.end(),result);
		return result/decltype( anyBaseVal(v) )(v.size()-1);
	}

	template<class T>
	T varianceIgnoreNans(const std::vector<T> &v, const T &mean)
	{
		if(v.size()==0) 
			return std::numeric_limits<T>::quiet_NaN();
		T result(0.0);
		const T *vi = &vi[0];
		const T *vEnd =vi + v.size();
		size_t count =0;
		for(; vi != vEnd; ++vi)
		{
			if( *vi == *vi )
			{
				result+=(*vi-mean)*(*vi-mean);
				++count;
			}
		}
		//std::accumulate(v.begin(), v.end(),result);
		return result/decltype( anyBaseVal(v) )( count - 1 );
	}

	template<class T>
	T variance(const std::vector<T> &v, const std::vector<T> &weights)
	{
		if(v.size()==0) 
			return std::numeric_limits<T>::quiet_NaN();
		sci::assertThrow( v.size() == weights.size(), sci::err() );
		T meanval=sci::mean( v, weights );
		T result = 0.0;
		T weight = 0.0;
		const T *vi = &v[0];
		const T *vEnd = vi+v.size();
		const T *weighti = & weights[0];
		for(; vi != vEnd; ++vi, ++weighti)
		{
			result+=(*vi-meanval)*(*vi-meanval) * *weighti;
			weight += *weighti;
		}
		return result/weight;
	}

	template<class T>
	T varianceIgnoreNans(const std::vector<T> &v, const std::vector<T> &weights)
	{
		if(v.size()==0) 
			return std::numeric_limits<T>::quiet_NaN();
		sci::assertThrow( v.size() == weights.size(), sci::err() );
		T meanval=sci::mean( v, weights );
		T result = 0.0;
		T weight = 0.0;
		const T *vi = &v[0];
		const T *vEnd = vi+v.size();
		const T *weighti = & weights[0];
		for(; vi != vEnd; ++vi, ++weighti)
		{
			if( *vi == *vi )
			{
				result+=(*vi-meanval)*(*vi-meanval) * *weighti;
				weight += *weighti;
			}
		}
		return result/weight;
	}

	template<class T>
	T variance(const std::vector<T> &v, const std::vector<T> &weights, const T &mean)
	{
		if(v.size()==0) 
			return std::numeric_limits<T>::quiet_NaN();
		sci::assertThrow( v.size() == weights.size(), sci::err() );
		T result = 0.0;
		T weight = 0.0;
		const T *vi = &v[0];
		const T *vEnd = vi+v.size();
		const T *weighti = & weights[0];
		for(; vi != vEnd; ++vi, ++weighti)
		{
			result+=(*vi-mean)*(*vi-mean) * *weighti;
			weight += *weighti;
		}
		return result/weight;
	}

	template<class T>
	T varianceIgnoreNans(const std::vector<T> &v, const std::vector<T> &weights, const T &mean)
	{
		if(v.size()==0) 
			return std::numeric_limits<T>::quiet_NaN();
		sci::assertThrow( v.size() == weights.size(), sci::err(SERR_VECTOR, 0, "When calculating a weighted variance, the wheits must have the same size as the data") );
		T result = 0.0;
		T weight = 0.0;
		const T *vi = &v[0];
		const T *vEnd = vi+v.size();
		const T *weighti = & weights[0];
		for(; vi != vEnd; ++vi, ++weighti)
		{
			if( *vi ==*vi )
			{
				result+=(*vi-mean)*(*vi-mean) * *weighti;
				weight += *weighti;
			}
		}
		return result/weight;
	}

	template<class T>
	T variancenobessel(const std::vector<T> &v, const T &mean)
	{
		T result(0.0);
		for(typename std::vector<T>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
			result+=(*vi-mean)*(*vi-mean);
		//std::accumulate(v.begin(), v.end(),result);
		return result/decltype( anyBaseVal(v) )(v.size());
	}

	template<class T>
	T stdev(const std::vector<T> &v)
	{
		T meanval=sci::mean(v);
		return sci::stdev(v, meanval);
	}

	template<class T>
	T stdevIgnoreNans(const std::vector<T> &v)
	{
		T meanval=sci::meanIgnoreNans(v);
		return sci::stdevIgnoreNans(v, meanval);
	}

	template<class T>
	T stdev(const std::vector<T> &v, const std::vector<T> &weights)
	{
		T meanval=sci::mean(v, weights);
		return sci::stdev(v, weights, meanval);
	}

	template<class T>
	T stdevIgnoreNans(const std::vector<T> &v, const std::vector<T> &weights)
	{
		T meanval=sci::meanIgnoreNans(v, weights);
		return sci::stdevIgnoreNans(v, weights, meanval);
	}

	template<class T>
	T stdev(const std::vector<T> &v, const T &mean)
	{
		return std::sqrt( variance( v, mean ) );
	}

	template<class T>
	T stdevIgnoreNans(const std::vector<T> &v, const T &mean)
	{
		return std::sqrt( varianceIgnoreNans( v, mean ) );
	}

	template<class T>
	T stdev(const std::vector<T> &v, const std::vector<T> &weights, const T &mean)
	{
		return std::sqrt( variance( v, weights, mean ) );
	}

	template<class T>
	T stdevIgnoreNans(const std::vector<T> &v, const std::vector<T> &weights, const T &mean)
	{
		return std::sqrt( varianceIgnoreNans( v, weights, mean ) );
	}

	template<class T>
	T stdevnobessel(const std::vector<T> &v, const T &mean)
	{
		return std::sqrt( variancenobessel( v, mean ) );
	}

	template<class T>
	T varianceofthevariance( const T &fourthMoment, const T &variance, const T &mean, T n )
	{
		return ( fourthMoment - T(n-3.0)/T(n-1.0)*variance*variance )/n;
	}

	template<class T>
	T varianceofthevariance( const std::vector<T> &v )
	{
		T var;
		T varOfVar;
		sci::variance( v, var, varOfVar);
		return varOfVar;
	}

	template<class T>
	T varianceofthevariancenobessel( const std::vector<T> &v )
	{
		T var;
		T varOfVar;
		sci::variancenobessel( v, var, varOfVar);
		return varOfVar;
	}

	template<class T>
	void variance(const std::vector<T> &v, T &variance, T &varianceofthevariance )
	{
		T meanval=sci::mean(v);
		variance = sci::variance( v, meanval );
		T fourthMoment = sci::centralmoment(4, v, meanval);
		varianceofthevariance = sci::varianceofthevariance( fourthMoment, variance, meanval, (T)v.size() );
	}

	template<class T>
	void variancenobessel(const std::vector<T> &v, const T &mean, T &variance, T &varianceofthevariance )
	{
		variance = sci::variancenobessel( v, mean );
		T fourthMoment = centralmomentnobessel(4, v, mean);
		varianceofthevariancenobessel = sci::varianceofthevariance( fourthMoment, variance, mean, (T)v.size() );
	}

	template<class T>
	T centralmoment(int moment, const std::vector<T> &v)
	{
		T mean = sci::mean(v);
		return centralmoment(moment, v, mean);
	}

	template<class T>
	T geometricMean(const std::vector<T> &v)
	{
		if(v.size()==0) return std::numeric_limits<T>::quiet_NaN();
		T result=v[0];			
		const T *vi=&v[0]+1;
		const T *vilimit=&v[0]+v.size();
		for(; vi<vilimit; ++vi) 
			result+=*vi**vi;
		auto count=decltype( anyBaseVal(v) )(v.size());
		return std::sqrt(result)/count;
	}

	template<class T>
	T geometricMeanIgnoreNans(const std::vector<T> &v)
	{
		if(v.size()==0) return std::numeric_limits<T>::quiet_NaN();
		T result(0.0);
		size_t count=0;
		const T *vi=&v[0];
		const T *vilimit=vi+v.size();
		for(; vi<vilimit; ++vi)
		{
			if(*vi==*vi)
			{
				result+=*vi**vi;
				++count;
			}
		}
		return std::sqrt(result)/decltype( anyBaseVal(v) )(count);
	}

	template<class T>
	std::vector<T> geometricMean(const std::vector<std::vector<T>> &v)
	{
		if(v.size()==0) return std::vector<T>(0);
		std::vector<T> result(v[0].size(), 0.0);
		for(typename std::vector<std::vector<T>>::const_iterator vi=v.begin(); vi!=v.end(); ++vi) 
			result+=*vi**vi;
		return sci::sqrt(result)/decltype( anyBaseVal(v) )(v.size());
	}

	template<class T>
	std::vector<T> cumsum(const std::vector<T> &v)
	{
		if(v.size()==0) return std::vector<T>(0);
		std::vector<T> result(v.size());
		result[0]=v[0];
		typename std::vector<T>::iterator resulti=result.begin()+1;
		for(typename std::vector<T>::const_iterator vi=v.begin()+1; vi!=v.end(); ++vi) 
		{
			*resulti=(*vi+*(resulti-1));
			++resulti;
		}
		return result;
	}


	template< class T>
	inline T min(const T& v)
	{
		return v;
	}

	template< class U, class T>
	inline U min(const std::vector<T>& v)
	{
		if(v.size()==0) return std::numeric_limits<U>::quiet_NaN();
		typename std::vector<T>::const_iterator vi=v.begin();
		U result = min<U>(*vi);
		++vi;
		while(result!=result && vi!=v.end())
		{
			result = min<U>(*vi);
			++vi;
		}
		for(; vi!=v.end(); ++vi) 
		{
			U newmin=min<U>(*vi);
			result=newmin<result ? newmin : result;
		}
		return result;
	}

	//return the minimum value from a vector greater than a given limit
	template< class U, class T>
	U mingtlimit(const std::vector<T>& v, U limit)
	{
		//this will only be called by 1d vectors as we have a 2d specialisation
		U result=std::numeric_limits<U>::quiet_NaN();
		if(v.size()==0) return std::numeric_limits<U>::quiet_NaN();
		typename std::vector<T>::const_iterator vi=v.begin();
		while (result!=result && vi!=v.end() && !(*vi>limit))
		{
			++vi;
		}
		if(vi==v.end()) return result;
		result=*vi;
		//search the remaining numbers
		for(; vi!=v.end(); ++vi) result=(*vi)<result && (*vi)>limit ? (*vi) : result;
		return result;
	}
	//return the minimum value from a vector greater than a given limit
	template< class U, class T>
	U mingtlimit(const std::vector< std::vector<T> >& v, U limit)
	{
		//this is the 2d specialisation
		U result=std::numeric_limits<U>::quiet_NaN();
		if(v.size()==0) return std::numeric_limits<U>::quiet_NaN();
		typename std::vector< std::vector<T> >::const_iterator vi=v.begin();
		while (result!=result && vi!=v.end())
		{
			result=mingtlimit(*vi,limit);
			++vi;
		}
		//search the remaining numbers
		for(; vi!=v.end(); ++vi) 
		{
			U newmin=mingtlimit(*vi,limit);
			result=newmin<result ? newmin : result;
		}
		return result;
	}
	
	//return the minimum value from a vector greater than or equal to a given limit
	template< class U, class T>
	U mingtelimit(const std::vector<T>& v, U limit)
	{
		//this will only be called by 1d vectors as we have a 2d specialisation
		if(v.size()==0) return std::numeric_limits<U>::quiet_NaN();
		U result=std::numeric_limits<U>::quiet_NaN();
		typename std::vector<T>::const_iterator vi=v.begin();
		while (result!=result && vi!=v.end() &&!(*vi>=limit))
		{
			++vi;
		}
		if(vi==v.end()) return result;
		result=*vi;
		//search the remaining numbers
		for(; vi!=v.end(); ++vi) result=(*vi)<result && (*vi)>=limit ? (*vi) : result;
		return result;
	}
	//return the minimum value from a vector greater than or equal to a given limit
	template< class U, class T>
	U mingtelimit(const std::vector< std::vector<T> >& v, U limit)
	{
		//this is the 2d specialisation
		U result=std::numeric_limits<U>::quiet_NaN();
		if(v.size()==0) return std::numeric_limits<U>::quiet_NaN();
		typename std::vector< std::vector<T> >::const_iterator vi=v.begin();
		while (result!=result && vi!=v.end())
		{
			result=mingtelimit(*vi,limit);
			++vi;
		}
		//search the remaining numbers
		for(; vi!=v.end(); ++vi) 
		{
			U newmin=mingtelimit(*vi,limit);
			result=newmin<result ? newmin : result;
		}
		return result;
	}
	

	//return the maximum value from a vector less than a given limit
	template< class U, class T>
	U maxltlimit(const std::vector<T>& v, U limit)
	{
		//this will only be called by 1d vectors as we have a 2d specialisation
		if(v.size()==0) return std::numeric_limits<U>::quiet_NaN();
		U result=std::numeric_limits<U>::quiet_NaN();
		typename std::vector<T>::const_iterator vi=v.begin();
		while (result!=result && vi!=v.end() && !(*vi<limit))
		{
			++vi;
		}
		if(vi==v.end()) return result;
		result=*vi;
		//search the remaining numbers
		for(; vi!=v.end(); ++vi) result=(*vi)>result && (*vi)<limit ? (*vi) : result;
		return result;
	}
	//return the maximum value from a vector less than a given limit
	template< class U, class T>
	U maxltlimit(const std::vector< std::vector<T> >& v, U limit)
	{
		//this is the 2d specialisation
		U result=std::numeric_limits<U>::quiet_NaN();
		if(v.size()==0) return std::numeric_limits<U>::quiet_NaN();
		typename std::vector< std::vector<T> >::const_iterator vi=v.begin();
		while (result!=result && vi!=v.end())
		{
			result=maxltlimit(*vi,limit);
			++vi;
		}
		//search the remaining numbers
		for(; vi!=v.end(); ++vi) 
		{
			U newmax=maxltlimit(*vi,limit);
			result=newmax>result ? newmax : result;
		}
		return result;
	}
	
	//return the maximum value from a vector less than or equal to a given limit
	template< class U, class T>
	U maxltelimit(const std::vector<T>& v, U limit)
	{
		//this will only be called by 1d vectors as we have a 2d specialisation
		if(v.size()==0) return std::numeric_limits<U>::quiet_NaN();
		U result=std::numeric_limits<U>::quiet_NaN();
		typename std::vector<T>::const_iterator vi=v.begin();
		while (result!=result && vi!=v.end() && !(*vi<=limit))
		{
			++vi;
		}
		if(vi==v.end()) return result;
		result=*vi;
		//search the remaining numbers
		for(; vi!=v.end(); ++vi) result=(*vi)>result && (*vi)<=limit ? result : (*vi);
		return result;
	}

	//return the maximum value from a vector less than or equal to a given limit
	template< class U, class T>
	U maxltelimit(const std::vector< std::vector<T> >& v, U limit)
	{
		//this is the 2d specialisation
		U result=std::numeric_limits<U>::quiet_NaN();
		if(v.size()==0) return std::numeric_limits<U>::quiet_NaN();
		typename std::vector< std::vector<T> >::const_iterator vi=v.begin();
		while (result!=result && vi!=v.end())
		{
			result=maxltelimit(*vi,limit);
			++vi;
		}
		//search the remaining numbers
		for(; vi!=v.end(); ++vi) 
		{
			U newmax=maxltelimit(*vi,limit);
			result=newmax>result ? newmax : result;
		}
		return result;
	}
	
	
	template< class T>
	inline T max(const T& v)
	{
		return v;
	}

	template<class U,class T >
	U max(const std::vector<T> &v)
	{
		if(v.size()==0) return std::numeric_limits<U>::quiet_NaN();
		typename std::vector<T>::const_iterator vi=v.begin();
		U result = max<U>(*vi);
		++vi;
		while(result!=result && vi!=v.end())
		{
			result = max<U>(*vi);
			++vi;
		}
		for(; vi!=v.end(); ++vi) 
		{
			U newmax=max<U>(*vi);
			result=newmax>result ? newmax : result;
		}
		return result;
	}

	template<class T>
	size_t indexofmax(std::vector<T> v)
	{
		if(v.size()==0) return std::numeric_limits<size_t>::quiet_NaN();
		size_t result=0;
		typename std::vector<T>::const_iterator vi=v.begin();
		T currentmax = *vi;
		++vi;
		while(currentmax!=currentmax && vi!=v.end())
		{
			currentmax = *vi;
			++vi;
			++result;
		}
		for(; vi!=v.end(); ++vi) 
		{
			if(*vi>currentmax)
			{
				result=vi-v.begin();
				currentmax=*vi;
			}
		}
		return result;
	}

	template<class T>
	size_t indexofmin(std::vector<T> v)
	{
		if(v.size()==0) return std::numeric_limits<size_t>::quiet_NaN();
		size_t result=0;
		typename std::vector<T>::const_iterator vi=v.begin();
		T currentmin = *vi;
		++vi;
		while(currentmin!=currentmin && vi!=v.end())
		{
			currentmin = *vi;
			++vi;
			++result;
		}
		for(; vi!=v.end(); ++vi) 
		{
			if(*vi<currentmin)
			{
				result=vi-v.begin();
				currentmin=*vi;
			}
		}
		return result;
	}

	template<class T>
	std::vector<T> max(const std::vector<T> &v1, const std::vector<T> &v2)
	{
		std::vector<T> result(std::min(v1.size(),v2.size()));
		if(result.size()==0) return result;
		typename std::vector<T>::const_iterator v1i=v1.begin();
		typename std::vector<T>::const_iterator v2i=v2.begin();
		for(typename std::vector<T>::iterator resulti=result.begin(); resulti!=result.end(); ++resulti)
		{
			*resulti=std::max(*v1i,*v2i);
			++v1i;
			++v2i;
		}
		return result;
	}

	template<class T>
	std::vector<T> min(const std::vector<T> &v1, const std::vector<T> &v2)
	{
		std::vector<T> result(std::min(v1.size(),v2.size()));
		if(result.size()==0) return result;
		typename std::vector<T>::const_iterator v1i=v1.begin();
		typename std::vector<T>::const_iterator v2i=v2.begin();
		for(typename std::vector<T>::iterator resulti=result.begin(); resulti!=result.end(); ++resulti)
		{
			*resulti=std::min(*v1i,*v2i);
			++v1i;
			++v2i;
		}
		return result;
	}



	//replace nans with another value
	template<class T>
	void replacenans(std::vector<T> &v, const T &replacement)
	{
		if(v.size()==0)
			return;
		T* iter = &v[0];
		T* end = iter + v.size();
		for( ; iter!=end; ++iter)
			if(*iter!=*iter)
				*iter=replacement;
	}
	template<class T, class U>
	void replacenans(std::vector<std::vector<T>> &v, const U &replacement)
	{
		if (v.size() == 0)
			return;
		std::vector<T>* iter = &v[0];
		std::vector<T>* end = iter + v.size();
		for (; iter != end; ++iter)
			replacenans(*iter, replacement);
	}

	template<class T>
	void replaceinfs(std::vector<T> &v, const T &replacement)
	{
		if (v.size() == 0)
			return;
		T* iter = &v[0];
		T* end = iter + v.size();
		for (; iter != end; ++iter)
			if (*iter == std::numeric_limits<T>::infinity() || *iter == -std::numeric_limits<T>::infinity())
				*iter = replacement;
	}
	template<class T, class U>
	void replaceinfs(std::vector<std::vector<T>> &v, const U &replacement)
	{
		if (v.size() == 0)
			return;
		std::vector<T>* iter = &v[0];
		std::vector<T>* end = iter + v.size();
		for (; iter != end; ++iter)
			replaceinfs(*iter, replacement);
	}

	template<class T>
	void replaceNegativeInfs(std::vector<T> &v, const T &replacement)
	{
		if (v.size() == 0)
			return;
		T* iter = &v[0];
		T* end = iter + v.size();
		for (; iter != end; ++iter)
			if (*iter == -std::numeric_limits<T>::infinity())
				*iter = replacement;
	}
	template<class T, class U>
	void replaceNegativeInfs(std::vector<std::vector<T>> &v, const U &replacement)
	{
		if (v.size() == 0)
			return;
		std::vector<T>* iter = &v[0];
		std::vector<T>* end = iter + v.size();
		for (; iter != end; ++iter)
			replaceNegativeInfs(*iter, replacement);
	}

	template<class T>
	void replacePositiveInfs(std::vector<T> &v, const T &replacement)
	{
		if (v.size() == 0)
			return;
		T* iter = &v[0];
		T* end = iter + v.size();
		for (; iter != end; ++iter)
			if (*iter == std::numeric_limits<T>::infinity())
				*iter = replacement;
	}
	template<class T, class U>
	void replacePositiveInfs(std::vector<std::vector<T>> &v, const U &replacement)
	{
		if (v.size() == 0)
			return;
		std::vector<T>* iter = &v[0];
		std::vector<T>* end = iter + v.size();
		for (; iter != end; ++iter)
			replacePositiveInfs(*iter, replacement);
	}
	

	template<class T>
	std::vector<T> reverse(const std::vector<T> &v)
	{
		std::vector<T> result(v.size());
		if(v.size()==0)
			return result;
		const T *vlower=&v[0];
		const T *vupper=vlower+v.size()-1;
		T *resultlower=&result[0];
		T *resultupper=resultlower+v.size()-1;
		while(vupper>=vlower)
		{
			*resultlower=*vupper;
			*resultupper=*vlower;
			++resultlower;
			++vlower;
			--resultupper;
			--vupper;
		}
		return result;
	}
	
	//returns a vector in which the ith element consists of the mean of period elements of v starting at 
	//element i*step.
	//the new vector will have a size of floor((v.size()-period)/step)+1, unless this gives a size
	//less than zero in which case an empty vector is returned
	template<class T>
	std::vector<T> rollingaverage(const std::vector<T> &v, size_t period, size_t step=1)
	{
		if(v.size()<period) return std::vector<double>(0);
		std::vector<T> result((v.size()-period)/step+1,0);
		typename std::vector<T>::const_iterator vsubbegin=v.begin();
		for(typename std::vector<T>::iterator resulti=result.begin(); resulti!=result.end(); ++resulti)
		{
			for(typename std::vector<T>::const_iterator vi=vsubbegin; vi!=vsubbegin+period; ++vi) *resulti+=*vi;
			*resulti/=(T)period;
			vsubbegin+=step;;
		}
		return result;
	}

	//As above but with user defined weights. If weights doesn't have the same size as v then an empty vector is
	//returned
	template<class T>
	std::vector<T> rollingaverage(const std::vector<T> &v, const std::vector<T> weight, size_t period, size_t step=1)
	{
		if(v.size()!=weight.size())
			return std::vector<T>(0);
		if(v.size()<period) return std::vector<double>(0);
		std::vector<T> result((v.size()-period)/step+1,0);
		typename std::vector<T>::const_iterator vsubbegin=v.begin();
		typename std::vector<T>::const_iterator weightsubbegin=weight.begin();
		T totalweight;
		for(typename std::vector<T>::iterator resulti=result.begin(); resulti!=result.end(); ++resulti)
		{
			totalweight=0;
			for(typename std::vector<T>::const_iterator vi=vsubbegin; vi!=vsubbegin+period; ++vi) 
			{
				typename std::vector<T>::const_iterator weighti=weightsubbegin;
				*resulti+=*vi**weighti;
				totalweight+=*weighti;
				++weighti;
			}
			*resulti/=totalweight;
			vsubbegin+=step;;
			weightsubbegin+=step;
		}
		return result;
	}

	//returns a vector which consists of the boxcar mean of v over period elements
	//the new vector will have a size of floor(v.size()/period).
	template<class T>
	std::vector<T> boxcaraverage(const std::vector<T> &v, size_t period)
	{
		std::vector<T> result(v.size()/period,0); //integer division so no need to round down
		typename std::vector<T>::const_iterator vsubbegin=v.begin();
		for(typename std::vector<T>::iterator resulti=result.begin(); resulti!=result.end(); ++resulti)
		{
			for(typename std::vector<T>::const_iterator vi=vsubbegin; vi!=vsubbegin+period; ++vi) *resulti+=*vi;
			*resulti/=(T)period;
			vsubbegin+=period;
		}
		return result;
	}
	template<class T, class U, class V>
	std::vector<T> boxcaraverage(const std::vector<T> &v, size_t period, U divider(V numerator, size_t denominator))
	{
		std::vector<T> result(v.size() / period, 0); //integer division so no need to round down
		typename std::vector<T>::const_iterator vsubbegin = v.begin();
		for (typename std::vector<T>::iterator resulti = result.begin(); resulti != result.end(); ++resulti)
		{
			for (typename std::vector<T>::const_iterator vi = vsubbegin; vi != vsubbegin + period; ++vi) *resulti += *vi;
			*resulti = divider(*resulti, period);
			vsubbegin += period;
		}
		return result;
	}

	//returns a vector for which element 0 is the average over the first period elements of
	//v, element 1 is the average over the next period*base elements of v, element 2 
	//is the average over the next period*base*base ements of v, etc. This is particularly
	//useful when the averaged vector is to be plotted on a log scale as these points will
	//then be evenly spaced
	template<class T>
	std::vector<T> logboxcaraverage(const std::vector<T> &v, size_t period, size_t base)
	{
		if(base<2)return std::vector<double>(0);
		if(period==0) return std::vector<double>(0);
		size_t size=0;
		size_t oldsizeest=period;
		size_t lastaddition=period;
		while(oldsizeest<=v.size())
		{
			++size;
			lastaddition*=base;
			oldsizeest+=lastaddition;
		}
		std::vector<T> result(size,0); //integer division so no need to round down
		typename std::vector<T>::const_iterator vsubbegin=v.begin();
		for(typename std::vector<T>::iterator resulti=result.begin(); resulti!=result.end(); ++resulti)
		{
			for(typename std::vector<T>::const_iterator vi=vsubbegin; vi!=vsubbegin+period; ++vi) *resulti+=*vi;
			*resulti/=(T)period;
			vsubbegin+=period;
			period*=base;
		}
		return result;
	}

	//returns a vector which consists of the boxcar sum of v over period elements
	//the new vector will have a size of floor(v.size()/period).
	template<class T>
	std::vector<T> boxcarsum(const std::vector<T> &v, size_t period)
	{
		std::vector<T> result(v.size()/period,0); //integer division so no need to round down
		typename std::vector<T>::const_iterator vsubbegin=v.begin();
		for(typename std::vector<T>::iterator resulti=result.begin(); resulti!=result.end(); ++resulti)
		{
			for(typename std::vector<T>::const_iterator vi=vsubbegin; vi!=vsubbegin+period; ++vi) *resulti+=*vi;
			vsubbegin+=period;
		}
		return result;
	}

	//returns a vector for which element 0 is the average over the first period elements of
	//v, element 1 is the average over the next period*base elements of v, element 2 
	//is the average over the next period*base*base ements of v, etc. This is particularly
	//useful when the summed vector is to be plotted on a log x scale as these points will
	//then be evenly spaced and the areas under the curve will be equivalent throughout the
	//x range
	template<class T>
	std::vector<T> logboxcarsum(const std::vector<T> &v, size_t period, size_t base)
	{
		if(base<2)return std::vector<double>(0);
		if(period==0) return std::vector<double>(0);
		size_t size=0;
		size_t oldsizeest=period;
		size_t lastaddition=period;
		while(oldsizeest<=v.size())
		{
			++size;
			lastaddition*=base;
			oldsizeest+=lastaddition;
		}
		std::vector<T> result(size,0); //integer division so no need to round down
		typename std::vector<T>::const_iterator vsubbegin=v.begin();
		for(typename std::vector<T>::iterator resulti=result.begin(); resulti!=result.end(); ++resulti)
		{
			for(typename std::vector<T>::const_iterator vi=vsubbegin; vi!=vsubbegin+period; ++vi) *resulti+=*vi;
			vsubbegin+=period;
			period*=base;
		}
		return result;
	}


	template <class T>
	bool ascending(const std::vector<T> &v)
	{
		if (v.size()==0) return true;
		for(typename std::vector<T>::const_iterator vi=v.begin()+1; vi!=v.end(); ++vi)
		{
			if(*vi<=*(vi-1)) return false;
		}
		return true;
	}

	template <class T>
	bool descending(const std::vector<T> &v)
	{
		if (v.size()==0) return true;
		for(typename std::vector<T>::const_iterator vi=v.begin()+1; vi!=v.end(); ++vi)
		{
			if(*vi>=*(vi-1)) return false;
		}
		return true;
	}

	template <class T>
	void assign(std::vector<T> &v, const T &val)
	{
		if(v.size()==0) return;
		T* vi=&v[0];
		T* vend=vi+v.size();
		for(; vi<vend; ++vi) *vi=val;
	}

	template <class T>
	void assign(std::vector<T> &v, std::vector<SBOOL> locations, const T &val)
	{
		if(v.size()==0) return;
		if(v.size()!=locations.size()) return;
		T* vi=&v[0];
		T* vend=vi+v.size();
		SBOOL *loci=&locations[0];
		for(; vi<vend; ++vi) 
		{
			if(*loci) *vi=val;
			++loci;
		}
	}

	template <class T, class U>
	void assign(std::vector<std::vector<T>> &v, std::vector<std::vector<U>> locations, const T &val)
	{
		if(v.size()==0) return;
		if(v.size()!=locations.size()) return;
		std::vector<T>* vi=&v[0];
		std::vector<T>* vend=vi+v.size();
		std::vector<U> *loci=&locations[0];
		for(; vi<vend; ++vi, ++loci) 
			assign(*vi, *loci, val);
	}

	template <class T>
	void reverseinplace(std::vector<T> &v)
	{
		std::reverse(v.begin(), v.end());
	}


	//********************************************************
	//*******Specialized function with external support*******
	//********************************************************


	//resample at a differeent frequency for either double or float vectors
	//input is the original vector
	//factor is the new frequency divided by the old frequency
	//output will be filled with the resampled data. Note that output can have a size from 
	//floor((inputsize-1)*factor) to ceil((inputsize+1)*factor)
	void resample(const std::vector<double> &input, double factor, std::vector<double> &output);
	void resample(const std::vector<float> &input, float factor, std::vector<float> &output);
	//fft
	void fft(const std::vector<double> &re_input, std::vector<double> &re_output, std::vector<double> &im_output);
	void ifft(const std::vector<double> &re_input, std::vector<double> &im_input, std::vector<double> &re_output);
	//discrete power spectrum normalised so that the sum of the output is equal to the variance of the input
	std::vector<double> powerspectrum(const std::vector<double> &v);
	//discrete cospectrum normalised so that the sum of the output is equal to the covariance of the inputs
	std::vector<double> cospectrum(const std::vector<double> &x, const std::vector<double> &y);
	//resamples parameters x and y such that when logspacex and and logspacey are plotted with a logarithmic x axis
	//all areas accross the plota are equivalent and the data point are approximately evenly spaced. Often in
	//this case y will be a differential parameter i.e. dparam/dx such as in a probability distribution or 
	//a spectrum. The nth value of logspacex and logspacey will be taken over floor(newxinterval/xinterval*base^n) 
	//points giving approximately equal spacin on a log scaled plot. base must be >=1 .0 and xinterval and newxinterval must be > 0.0
	// otherwise empty vectors will be returned
	void logspace(double xstart, double xinterval, std::vector<double> y, double newxinterval, double newxbase, std::vector<double> &logspacex, std::vector<double> &logspacey);

	template <class T>
	T integrate(const std::vector<T> &x, const std::vector<T> &y)
	{
		if(x.size()!=y.size()) return std::numeric_limits<T>::quiet_NaN();
		if(x.size()==0) return 0;
		T result=0;
		typename std::vector<T>::const_iterator yi=y.begin()+1;
		for(typename std::vector<T>::const_iterator xi=x.begin()+1; xi!=x.end(); ++xi)
		{
			result+=(*yi+*(yi-1))*(*xi-*(xi-1));
			++yi;
		}
		result*=0.5;
		return result;
	}

		template <class T>
	T integrate(const std::vector<T> &x, const std::vector<T> &y, T minx, T maxx)
	{
		if(x.size()!=y.size()) return std::numeric_limits<T>::quiet_NaN();
		T result=0;
		typename std::vector<T>::const_iterator yi=y.begin()+1;
		bool swappedLimits = false;
		if( maxx < minx )
		{
			std::swap( minx, maxx );
			swappedLimits = true;
		}
		for(typename std::vector<T>::const_iterator xi=x.begin()+1; xi!=x.end(); ++xi,++yi)
		{
			T x0=*(xi-1);
			T x1=*xi;
			T y0=*(yi-1);
			T y1=*yi;
			if(x1 && x0 < minx)
				continue;
			if(x1 && x0 > maxx)
				continue;
			if(x0<minx)
			{
				y0=sci::linearinterpolate(minx,x0,x1,y0,y1);
				x0=minx;
			}
			if(x1<minx)
			{
				y1=sci::linearinterpolate(minx,x0,x1,y0,y1);
				x1=minx;
			}
			if(x0>maxx)
			{
				y0=sci::linearinterpolate(maxx,x0,x1,y0,y1);
				x0=maxx;
			}
			if(x1>maxx)
			{
				y1=sci::linearinterpolate(maxx,x0,x1,y0,y1);
				x1=maxx;
			}

			result+=(y0+y1)*(x1-x0);
		}
		result*=0.5;
		if(swappedLimits)
			result*=-1.0;
		return result;
	}

	template <class T>
	void integrate(const std::vector<T> &x, const std::vector<T> &y, const std::vector<T> &deltay, T &integral, T &deltaintegral)
	{
		if(x.size()!=y.size() || x.size()!=deltay.size()) 
		{
			integral=std::numeric_limits<T>::quiet_NaN();
			deltaintegral=std::numeric_limits<T>::quiet_NaN();
		}

		integral=0;
		deltaintegral=0;
		for(size_t i=1; i<x.size(); ++i)
		{
			integral+=(y[i]+y[i-1])*(x[i]-x[i-1]);
			deltaintegral+=(x[i]-x[i-1])*(x[i]-x[i-1])*(deltay[i]*deltay[i]+deltay[i-1]*deltay[i-1]);
		}
		integral*=0.5;
		deltaintegral=0.5*sqrt(deltaintegral);
	}

	template <class T>
	T integrate(const std::vector<T> &data, T interval)
	{
		T result=0;
		for(typename std::vector<T>::const_iterator datai=data.begin()+1; datai!=data.end(); ++datai) result+=(*datai+*(datai-1));
		result*=interval*0.5;
		return result;
	}

	template <class T>
	std::vector<T> differentiate(std::vector<T> x, std::vector<T> y)
	{
		if(x.size()==0 && y.size()==0)
			return std::vector<T>(0);
		std::vector<T> result(std::max(x.size(),y.size())-1,std::numeric_limits<T>::quiet_NaN());
		size_t n=std::min(x.size(),y.size());
		T* xi=&x[0]+1;
		T* yi=&y[0]+1;
		T* xend=&x[0]+n;
		T* resulti=&result[0];
		for(;xi!=xend; ++xi,++yi,++resulti)
			*resulti=(*yi-*(yi-1))/(*xi-*(xi-1));
		return result;
	}
	
	/*void fitstraightline(const std::vector<double> &x, const std::vector<double> &y, double &grad, double &intercept, double &vargrad, double &varintercept, double &covar);
	void fitstraightline(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &weights, double &grad, double &intercept, double &vargrad, double &varintercept, double &covar);
	void fitstraightline(const std::vector<double> &x, const std::vector<double> &y, double &grad, double &intercept);
	//fit y=mx[i]+c, i.e. multiple x parameters. The covariance matrix returned had size x.size()+1
	// in both dimensions. The last element represents the covariance with the intercept.
	//void fitstraightline(const std::vector<std::vector<double>> &x, const std::vector<double> &y, 
	//	std::vector<double> &grad, double &intercept, std::vector<std::vector<double>> &covar);*/
	void fitProportional(const std::vector<std::vector<double>> &x, const std::vector<double> &y, 
	std::vector<double> &grad, std::vector<std::vector<double>> &covar);
	void fitpolynomial(const std::vector<double> &x, const std::vector<double> &y, size_t max_power, std::vector<double> &coefs, std::vector<std::vector<double>> &covariancematrix);
	size_t fitstraightlinewithoutlierremoval(const std::vector<double> &x, const std::vector<double> &y, double &grad, double &intercept, double maxresidual);
	void fitpowerlaw(double power, const std::vector<double> &x, const std::vector<double> &y, double &k, double &vark);
	void crosscorr(const std::vector<double> &f, const std::vector<double> &g, std::vector<double> &r, std::vector<long> &fshifts, bool circular=false);
	size_t minimise(std::vector<double> &tunableparams, const std::vector<double> &fixedparams, double (*functiontominimise)(const std::vector<double> &,const std::vector<double> &));
	size_t minimise(std::vector<double> &tunableparams, const std::vector< std::vector<double> > &fixedparams, double (*functiontominimise)(const std::vector<double> &,const std::vector< std::vector<double> > &));
	size_t fitnonlinear(std::vector<double> &tunableparams, std::vector<double> &paramErrs, std::vector<double> tunableparamlowerlimits, std::vector<double> tunableparamupperlimits, double (*function)(const std::vector<double> &,const std::vector<double> &), const std::vector<std::vector<double>> &xs, const std::vector<double> &ys, const std::vector<double> &weights);
	size_t fitnonlinear(std::vector<double> &tunableparams, std::vector<double> &paramErrs, std::vector<double> tunableparamlowerlimits, std::vector<double> tunableparamupperlimits, double (*function)(const std::vector<double> &,double), const std::vector<double> &xs, const std::vector<double> &ys, const std::vector<double> &weights);

	/*template< class T > 
	void minimiseCaller(const alglib::real_1d_array &fitParams, double &result, void *ptr )
	{
		T* minimisable = (T*)(ptr);
		std::vector<double> stdFitParams( &fitParams[0], &fitParams[0]+fitParams.length() );
		result = (*minimisable)( stdFitParams );
	}
	template< class T >
	void minimise( T &minimisable, std::vector< double > &tunableParams, std::vector< double > scales )
	{
		//create tunable params
		alglib::real_1d_array algtunableparams;
		algtunableparams.setcontent( tunableParams.size(), &tunableParams[0] );

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
		alglib::real_1d_array algScales=algtunableparams;
		for(int i=0; i<algScales.length() && i < scales.size(); ++i)
			if( scales[i] > 0.0 )
				algScales[i]=scales[i];
		alglib::mincgsetscale(state,algScales);

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
			alglib::mincgoptimize( state, &minimiseCaller<T>, NULL, (void*)&minimisable );
		}
		catch(alglib::ap_error err)
		{
			throw sci::err( err, 0);
		}

		//get the results
		alglib::mincgreport report;
		alglib::mincgresults(state, algtunableparams, report);

		tunableParams = std::vector<double>( &algtunableparams[0], &algtunableparams[0]+algtunableparams.length() );
	}

	double integrate(double xMin, double xMax, double intervalMax, double (*functionToIntegrate)(double x, const std::vector<double> &params), const std::vector<double> &params);
	template<class T>
	double integrateZeroToInfinity( T *functionToIntegrate, size_t nNodes )
	{
		//get the gauss-Laguerre quadrature xs and weights. Note that these are constants
		//for a given nNodes, which feels a bit odd. Basically there is an assumption that
		//our function can be modelled by a polynomial of order nNodes-1 multiplied by 
		//exp(-x) and some clever maths to prove that the given nodes work.
		alglib::real_1d_array x;
		alglib::real_1d_array w;
		x.setlength( nNodes );
		w.setlength( nNodes );
		alglib::ae_int_t algresult;
		alglib::gqgenerategausslaguerre( nNodes, 0.0, algresult, x, w );
		sci::assertThrow( algresult == 1, sci::err() );

		double result = 0.0;
		for(size_t i=0; i<x.length(); ++i)
			result += functionToIntegrate( x[i] ) * w[i] * std::exp( x[i] );
		return result;
	}
	template< class T >
	void integrateAdaptiveCallback( double x, double, double, double &y, void *ptr )
	{
		T* integratable = (T*)(ptr);
		y = (*integratable)( x );
	}
	template< class T >
	double integrateAdaptive( T *functionToIntegrate, double limit1, double limit2 )
	{
		double result;
		alglib::autogkstate state;
		alglib::autogkreport report;
		alglib::autogksmooth( limit1, limit2, state );
		alglib::autogkintegrate( state, integrateAdaptiveCallback<T>, functionToIntegrate );
		alglib::autogkresults( state, result, report );
		sci::assertThrow( report.terminationtype == 1, sci::err() );
		return result;
	}*/

	//Integrates from a starting point outwards into the tails of a distribution.
	//Will integrate until either we reach the limits, or the change is less than
	//a given amount or the change fraction is less than a given amount.
	//Each step will move out from the centre integrating an amount chunkWindth.
	//Pass 0 for the changeConditions to ignore them 
	template< class T >
	double integrateTailsAdaptive( T *functionToIntegrate, double limit1, double limit2, double start, double chunkWidth, double changeCondition, double fractionCangeCondition )
	{
		double result = 0.0;
		double limit1Step = -chunkWidth;
		double limit2Step = chunkWidth;
		if( limit1 > limit2 )
			std::swap( limit1Step, limit2Step );
		double limit1MaxDist= std::abs( start - limit1 );
		double limit2MaxDist= std::abs( start - limit2 );
		double limit11 = start;
		double limit22 = start;
		double limit12;
		double limit21;
		double thisChunk;
		do
		{
			limit12 = limit11;
			limit11 += limit1Step;
			limit21 = limit22;
			limit22 += limit2Step;
			if( std::abs( start- limit11 ) > limit1MaxDist )
				limit11 = limit1;
			if( std::abs( start- limit22 ) > limit2MaxDist )
				limit22 = limit2;

			thisChunk = integrateAdaptive( functionToIntegrate, limit11, limit12 ) +
				integrateAdaptive( functionToIntegrate, limit21, limit22 );
			result += thisChunk;
		}
		while( std::abs( thisChunk ) > std::abs( changeCondition ) && std::abs( thisChunk/result ) > std::abs( fractionCangeCondition ) );

		return result;
	}

	//********************************************************
	//*********************My own functions*******************
	//********************************************************
	
	//generates a histogram of data based on limits. If limits has size zero then counts will be returned with size
	//zero.
	//Otherwise counts will have a size one higher than limits. counts[0]will include data too small
	//to be counted in other bins and counts[counts.size()-1] will include data too large to be included in other bins.
	//If roundup is true then data which falls exactly on a limit will be counted in the bin above the limit
	//If roundup is false then data which falls exactly on a limit will be counted in the bin below
	//the limit or belowlimits.
	//Limits must be monotonically nondecreasing for this function to work.
	template<class T>
	void histogram(std::vector<size_t> &counts, const std::vector<T> &limits, const std::vector<T> &data, bool roundup=true)
	{
		if(limits.size()==0)
		{
			counts.resize(0);
			return;
		}
		
		size_t nbins=limits.size()+1;
		sci::setallvalues(counts,0);
		counts.resize(nbins,0);
		std::vector<size_t>::iterator count0=counts.begin();
		if(roundup)
		{
			for(typename std::vector<T>::const_iterator datai=data.begin(); datai!=data.end(); ++datai)
			{
				if(*datai<limits[0]) ++*count0;
				else if(*datai>=limits.back()) ++*(count0+nbins-1);
				else
				{
					for(typename std::vector<T>::const_iterator limiti=limits.begin()+1; limiti!=limits.end(); ++limiti)
					{
						if(*datai>=*(limiti-1) && *datai <*limiti)
						{
							++(*(count0+(size_t)(limiti-limits.begin())));
							break;
						}
					}
				}
			}
		}
		else
		{
			for(typename std::vector<T>::const_iterator datai=data.begin(); datai!=data.end(); ++datai)
			{
				if(*datai<=limits[0]) ++*count0;
				else if(*datai>limits.back()) ++*(count0+nbins-1);
				else
				{
					for(typename std::vector<T>::const_iterator limiti=limits.begin()+1; limiti!=limits.end(); ++limiti)
					{
						if(*datai>*(limiti-1) && *datai <=*limiti)
						{
							++(*(count0+(size_t)(limiti-limits.begin())));
							break;
						}
					}
				}
			}
		}
	}
	
	//generates a histogram of data based on limitstart nlimits and limitspacing. If nlimits is zero 
	//then counts will be returned with size zero and both belowlimits and abovelimits will be set to zero.
	//Otherwise counts will have a size one lower than limits and belowlimits will include data too small
	//to be counted in counts and abovelimits will include data too large to be included in counts;
	//if roundup is true then data which falls exactly on a limit will be counted in the bin above the limit or
	//abovelimits. If roundup is false then data which falls exactly on a limit will be counted in the bin below
	//the limit or belowlimits.
	template<class T>
	void histogram(std::vector<size_t> &counts, const T &limitstart, const T &limitspacing, size_t nlimits, const std::vector<T> &data, bool roundup=true)
	{
		if(nlimits==0)
		{
			counts.resize(0);
			return;
		}
		
		sci::setallvalues(counts,0);
		counts.resize(nlimits+1);
		typename std::vector<size_t>::iterator count0=counts.begin();
		T limittop=limitstart+(T)(nlimits-1)*limitspacing;
		if(roundup)
		{
			for(typename std::vector<T>::const_iterator datai=data.begin(); datai!=data.end(); ++datai)
			{
				if(*datai<limitstart) ++*count0;
				else if(*datai>=limittop) ++*(count0+nlimits);
				else
				{
					size_t bin=(size_t)((*datai-limitstart)/limitspacing)+1;
					++*(count0+bin);
				}
			}
		}
		else
		{
			for(typename std::vector<T>::const_iterator datai=data.begin(); datai!=data.end(); ++datai)
			{
				if(*datai<=limitstart) ++*count0;
				else if(*datai>limittop) ++*(count0+nlimits);
				else
				{
					T Tbin=((*datai-limitstart)/limitspacing)+1;
					size_t bin=(size_t)Tbin;
					if(Tbin==(T)bin) --bin;
					++*(count0+bin);
				}
			}
		}
	}
	
	//generates a histogram of data based on limits. If limits has size zero or any of the elements of limits has
	//size zero then counts will be returned with size zero.
	//Otherwise the nth dimension of counts will have a size one higher than the nth element of limits.
	//For each dimension odf counts the first element will include data too small to be counted in other 
	//bins and the last element will include data too large to be included in other bins.
	//If roundup is true then data which falls exactly on a limit will be counted in the bin above the limit
	//If roundup is false then data which falls exactly on a limit will be counted in the bin below
	//the limit or belowlimits.
	//Limits must be monotonically nondecreasing for this function to work.
	template<class T, class U>
	void histogram(std::vector< std::vector<U> > &counts, const std::vector< std::vector<T> >&limits, const std::vector< std::vector<T> > &data, bool roundup=true)
	{
		if(limits.size()==0)
		{
			counts.resize(0);
			return;
		}
		size_t numdims=sci::ndims(counts);
		if (numdims!=limits.size())
		{
			counts.resize(0);
			return;
		}
		std::vector<size_t> nbins(limits.size());
		for(size_t i=0; i<limits.size(); ++i)nbins[i]=limits[i].size()+1;
		sci::setallvalues(counts,0);
		sci::resize(counts,nbins);
		
		if(roundup)
		{
			std::vector<size_t> index(numdims);
			size_t npoints=data[0].size();
			for(size_t i=0; i<npoints; ++i)
			{
				for(size_t j=0; j<numdims; ++j)
				{
					if(data[j][i]<limits[j][0]) index[j]=0;
					else if(data[j][i]>=limits[j].back()) index[j]=nbins[j]-1;
					else 
					{
						for(size_t k=1; k<limits[j].size(); ++k)
						{
							if(data[j][i]<limits[j][k] &&data[j][i]>=limits[j][k-1])
							{
								index[j]=k;
								break;
							}
						}
					}
				}
				sci::at<double>(counts,index)+=1;
			}
		}
		else
		{
			std::vector<size_t> index(numdims);
			size_t npoints=data[0].size();
			for(size_t i=0; i<npoints; ++i)
			{
				for(size_t j=0; j<numdims; ++j)
				{
					if(data[j][i]<=limits[j][0]) index[j]=0;
					else if(data[j][i]>limits[j].back()) index[j]=nbins[j]-1;
					else 
					{
						for(size_t k=1; k<limits[j].size(); ++k)
						{
							if(data[j][i]<=limits[j][k] &&data[j][i]>limits[j][k-1])
							{
								index[j]=k;
								break;
							}
						}
					}
				}
				sci::at<double>(counts,index)+=1;
			}
		}
	}

	
	//generates a histogram of data based on limitstart nlimits and limitspacing. If nlimits is zero 
	//then counts will be returned with size zero.
	//Otherwise counts will have a size one larger than limits with the first and last element in each 
	//dimension being counts below and above the limits respectively.
	//If roundup is true then data which falls exactly on a limit will be counted in the bin above the limit or
	//if roundup is false then data which falls exactly on a limit will be counted in the bin below the limit.
	template<class T, class U>
	void histogram(std::vector< std::vector<U> > &counts, const std::vector<T> &limitstarts, const std::vector<T> &limitspacings, const std::vector<size_t> &nlimits, const std::vector< std::vector<T> > &data, bool roundup=true)
	{
		size_t numdims=sci::ndims(counts);
		if (numdims!=data.size() || numdims!=limitstarts.size() || numdims!=limitspacings.size() || numdims!=nlimits.size())
		{
			counts.resize(0);
			return;
		}
		if(!sci::rectangular(data))
		{
			counts.resize(0);
			return;
		}
		sci::setallvalues(counts,0);
		sci::resize(counts,nlimits+(size_t)1);
		//copy the sizes into a T vector
		std::vector<T> nlimitsd(nlimits.size());
		for(size_t i=0; i<nlimits.size(); ++i) nlimitsd[i]=(T)nlimits[i];
		std::vector<T> limitstops=limitstarts+limitspacings*nlimitsd;
		
		if(roundup)
		{
			std::vector<size_t> index(numdims);
			size_t npoints=data[0].size();
			for(size_t i=0; i<npoints; ++i)
			{
				for(size_t j=0; j<numdims; ++j)
				{
					if(data[j][i]<limitstarts[j]) index[j]=0;
					else if(data[j][i]>=limitstops[j]) index[j]=nlimits[j];
					else index[j]=(size_t)((data[j][i]-limitstarts[j]) / limitspacings[j])+1;
				}
				sci::at<U>(counts,index)+=1;
			}
		}
		else
		{
			std::vector<size_t> index(numdims);
			size_t npoints=data[0].size();
			for(size_t i=0; i<npoints; ++i)
			{
				for(size_t j=0; j<numdims; ++j)
				{
					if(data[j][i]<=limitstarts[j]) index[j]=0;
					else if(data[j][i]>limitstops[j]) index[j]=nlimits[j];
					else 
					{
						T bin=((data[j][i]-limitstarts[j]) / limitspacings[j])+1;
						index[j]=(size_t)bin;
						if(bin==(T)index[j]) index[j]--;
					}
				}
				sci::at<U>(counts,index)+=1;
			}
		}
	}


	template<class T>
	void erase(std::vector<T> &v, size_t index)
	{
		if(index<v.size()) v.erase(v.begin()+index);
	}

	template<class T>
	void erase(std::vector<T> &v, size_t index, size_t n_elements)
	{
		if (n_elements==0) return;
		if(index<v.size())
		{
			if(index+n_elements-1<v.size()) v.erase(v.begin()+index,v.begin()+index+n_elements);
		}
		else
		{
			v.erase(v.begin()+index,v.end());
		}
	}

	template<class T>
	void erase(std::vector<std::vector<T>> &v, std::vector<size_t> index, std::vector<size_t> n_elements)
	{
		if(index.size()!=n_elements.size()) return;
		if(sci::ndims(v)!=index.size()) return;
		for(size_t i=0; i<v.size(); ++i) 
		{
			erase(v[i],index[1],n_elements[1]);
		}
		sci::erase(v,index[0],n_elements[0]);
	}

	template<class T>
	void erase(std::vector<T> &v, std::vector<size_t> index, std::vector<size_t> n_elements)
	{
		if(index.size()!=n_elements.size()) return;
		if(index.size()!=1) return;
		erase(v,index[0],n_elements[0]);
	}


	//*****************************************************************
	//***********matrix multiplication*********************************
	//*****************************************************************

	//dot product of vectors
	template <class T>
	T dot(const std::vector<T> &a, const std::vector<T> &b)
	{
		//check validity
		if(a.size()!=b.size()) return std::numeric_limits<T>::quiet_NaN();
		if(a.size()==0) return T(0);
		//create quick iterators to vectors
		const T* ai = &a[0];
		const T* bi = &b[0];
		//do the sum
		T sum=0.0;
		for(size_t i=0; i<a.size(); ++i)
		{
			sum+=ai[i]*bi[i];
		}
		return sum;
	}

	//generate invalid output for multi d vectors
	template <class T>
	T dot(const std::vector< std::vector<T> > &a, const std::vector<T> &b)
	{
		return std::numeric_limits<T>::quiet_NaN();
	}
	template <class T>
	T dot(const std::vector<T> &a, const std::vector< std::vector<T> > &b)
	{
		return std::numeric_limits<T>::quiet_NaN();
	}

	//cross product of vectors
	template <class T>
	std::vector<T> cross(const std::vector<T> &a, const std::vector<T> &b)
	{
		//check validity, must have 3 elements, return empty vector otherwise
		if(a.size()!=3) return std::vector<T>();
		if(b.size()!=3) return std::vector<T>();

		//create the result
		std::vector<T> result(3);
		//create quick iterators to vectors
		T* ai = &a[0];
		T* bi = &a[0];
		T* resulti = &result[0];

		//set the values of result
		resulti[0]=ai[1]*bi[2]-ai[2]*bi[1];
		resulti[1]=ai[2]*bi[0]-ai[0]*bi[2];
		resulti[2]=ai[0]*bi[1]-ai[1]*bi[0];

		//return the result
		return result;
	}

	//generate invalid output for multi d vectors
	template <class T>
	std::vector<T> cross(const std::vector< std::vector<T> > &a, const std::vector<T> &b)
	{
		return std::vector<T>();
	}
	template <class T>
	std::vector<T> cross(const std::vector<T> &a, const std::vector< std::vector<T> > &b)
	{
		return std::vector<T>();
	}

	//matrix multiplication. note the first index is the row and the second is the column
	template <class T>
	std::vector< std::vector<T> > matrixmult(const std::vector< std::vector<T> > &a, const std::vector< std::vector<T> > &b)
	{
		//check validity, must be rectangular and a.size() must be the same as b[0].size()
		//return an empty vector if they fail
		if(a.size()==0) return std::vector< std::vector<T> >();
		if(b.size()==0) return std::vector< std::vector<T> >();
		if(!sci::rectangular(a)) return std::vector< std::vector<T> >();
		if(!sci::rectangular(b)) return std::vector< std::vector<T> >();
		if(a[0].size()==0) return std::vector< std::vector<T> >();
		if(a[0].size()!=b.size()) return std::vector< std::vector<T> >();

		std::vector< std::vector<T> > result(b.size());
		size_t dim1=a.size();
		size_t dim2=b[0].size();
		qic< std::vector<T> > resulti(result);
		qic< std::vector<T> > ai(a);
		qic< std::vector<T> > bi(b);
		for(size_t i=0; i<dim1; ++i)
		{
			resulti[i].resize(b[0].size(),0.0);
			qi<T> resultii(resulti[i]);
			qic<T> aii(ai[i]);
			for(size_t j=0; j<dim2; ++j)
			{
				resultii[j]+=aii[j]*bi[j][i];
			}
		}
		return result;
	}

	//matrix multiplication. In the 2D vector the first index is the row and the second is the column
	//The 1D vector is assumed to be a column
	template <class T>
	std::vector<T> matrixmult(const std::vector< std::vector<T> > &a, const std::vector<T> &b)
	{
		//check validity, must be rectangular and a.size() must be the same as b[0].size()
		//return an empty vector if they fail
		if(a.size()==0) return std::vector<T>();
		if(!sci::rectangular(a)) return std::vector<T>();
		if(a[0].size()==0) return std::vector<T>();
		if(a[0].size()!=b.size()) return std::vector<T>();

		std::vector<T> result(a.size(),0.0);
		size_t dim1=a.size();
		size_t dim2=a[0].size();
		qi<T> resulti(result);
		qic< std::vector<T> > ai(a);
		qic<T> bi(b);
		for(size_t i=0; i<dim1; ++i)
		{
			qic<T> aii(ai[i]);
			for(size_t j=0; j<dim2; ++j)
			{
				resulti[i]+=aii[j]*bi[j];
			}
		}
		return result;
	}

	template<class T>
	bool constantStride(const std::vector<std::vector<T>> &v)
	{
		if (v.size() < 2)
			return true;
		auto stride = &v[1][0] - &v[0][0];
		for (size_t i = 2; i < v.size(); ++i)
			if (&v[i][0] - &v[i - 1][0] != stride)
				return false;
		return true;
	}

	//matrix multiplication. In the 2D vector the first index is the row and the second is the column
	//The 1D vector is assumed to be a row
	template <class T>
	std::vector<T> matrixmult(const std::vector<T> &a, const std::vector< std::vector<T> > &b)
	{
		//check validity, must be rectangular and a.size() must be the same as b[0].size()
		//return an empty vector if they fail
		if(b.size()==0) return std::vector<T>();
		if(!sci::rectangular(b)) return std::vector<T>();
		if(b[0].size()==0) return std::vector<T>();
		if(a.size()!=b.size()) return std::vector<T>();
		
		if (a.size() == 1)
			return(b[0] * a[0]);

		std::vector<T> result(b[0].size(),0.0);
		size_t dim1=b.size();
		size_t dim2=b[0].size();
		if (constantStride(b))
		{
			size_t bStride = &b[1][0] - &b[0][0];
			double *resulti = &result[0];
			double * resultEnd = resulti + result.size();
			const double *bi = &b[0][0];
			for (; resulti != resultEnd; ++bi, ++resulti)
			{
				*resulti = 0.0;
				const double *aj = &a[0];
				const double *aEnd = aj + a.size();
				const double *bji = bi;
				for (; aj != aEnd; ++aj, bji+=bStride)
				{
					*resulti += *aj * *bji;
				}
			}
		}
		else
		{
			for (size_t i = 0; i < dim2; ++i)
			{
				result[i] = 0;
				for (size_t j = 0; j < dim1; ++j)
					result[i] += a[j] * b[j][i];
			}
		}
		return result;
	}

	//matrix multiplication.
	//The first vector is assumed to be a row and the second a column
	//This is the same as doing a dot product
	template <class T>
	T matrixmult(const std::vector<T> &a, const std::vector<T> &b)
	{
		return sci::dot( a, b );
	}

	std::vector< std::vector<double> > inverse(const std::vector< std::vector<double> > &mat, double conditionNumberLimit = 1.0e10);
	
	//This is a very naive implementation. Should do something better
	template<class T>
	T determinant(const std::vector<std::vector<T>> &matrix)
	{
		sci::assertThrow(square(matrix), sci::err(SERR_VECTOR, -999, "sci::determinant called with a non-square matrix."));
		if (matrix.size() == 1)
			return matrix[0][0];
		else if (matrix.size() == 2)
			return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
		else
		{
			double result = 0;
			std::vector<std::vector<double>> minorMatrix(matrix.size() - 1);
			double multiplier = 1.0;
			for (size_t i = 0; i < matrix.size(); ++i)
			{
				for (size_t j = 0; j < minorMatrix.size(); ++j)
				{
					if (j < i)
						minorMatrix[j] = sci::subvector(matrix[j],1, matrix[j].size()-1);
					else
						minorMatrix[j] = sci::subvector(matrix[j+1], 1, matrix[j+1].size() - 1);
				}
				result += multiplier * matrix[0][i] * sci::determinant(minorMatrix);
				multiplier *= -1.0;
			}
			return result;
		}
	}

	template<class T>
	std::vector<T> reorder(const std::vector<T> &v, const std::vector<size_t> &newlocations)
	{
		std::vector<T> result(v.size());
		for (size_t i = 0; i<result.size(); ++i) result[newlocations[i]] = v[i];
		return result;
	}

	//sort
	template<class T>
	void sort(const std::vector<T> &v, std::vector<T> &sortedv, std::vector<size_t> &newlocations)
	{
		//create a vector of locations
		std::vector<size_t> originalLocations = sci::indexvector<size_t>(v.size());
		//sort the locations based on the data in v
		std::sort(originalLocations.begin(), originalLocations.end(), [v](size_t lhs, size_t rhs) {return v[lhs] < v[rhs];});

		//set up our newLocations
		newlocations.resize(originalLocations.size());
		for (size_t i = 0; i<originalLocations.size(); ++i)
		{
			newlocations[originalLocations[i]] = i;
		}

		//copy the data to the sorted vector
		sortedv = sci::reorder(v, newlocations);
	}

	template<class T, class SORTER>
	void sort(const std::vector<T> &v, std::vector<T> &sortedv, std::vector<size_t> &newlocations, SORTER sorter)
	{
		//create a vector of locations
		std::vector<size_t> originalLocations = sci::indexvector<size_t>(v.size());
		//sort the locations based on the data in v
		std::sort(originalLocations.begin(), originalLocations.end(), [v,sorter](size_t lhs, size_t rhs) {return sorter(v[lhs], v[rhs]);});

		//set up our newLocations
		newlocations.resize(originalLocations.size());
		for (size_t i = 0; i<originalLocations.size(); ++i)
		{
			newlocations[originalLocations[i]] = i;
		}

		//copy the data to the sorted vector
		sortedv = sci::reorder(v, newlocations);
	}

	template<class T>
	inline std::vector<T> sort(std::vector<T> v)
	{
		std::sort(v.begin(),v.end());
		return v;
	}

	template<class T, class SORTER>
	inline std::vector<T> sort(std::vector<T> v, SORTER sorter)
	{
		std::sort(v.begin(), v.end(), sorter);
		return v;
	}


	bool pointinpolygon(std::vector<double> polygonx, std::vector<double> polygony, double pointx, double pointy);
	void intersect(double xline1point1, double yline1point1, double xline1point2, double yline1point2, double xline2point1, double yline2point1, double xline2point2, double yline2point2, double &intersectx, double &intersecty, double &m1, double &m2);
	void eigenvalues(const std::vector<std::vector<double>> &matrix, std::vector<double> &eigenvaluesReal, std::vector<double> &eigenvaluesImaginary);
	void eigenvector(const std::vector<std::vector<double>> &matrix, std::vector<double> &eigenvaluesReal, std::vector<double> &eigenvaluesImaginary, std::vector<std::vector<double>> &eigenvectors);


	template<class T>
	class shared_ptr
	{
	public:
		shared_ptr(T *ptr=NULL);
		~shared_ptr();
		shared_ptr(const shared_ptr &other);
		shared_ptr<T> & operator=(const shared_ptr &rhs);
		shared_ptr<T> & operator=(T *rhs);
		T & operator*() const ;
		T* operator->() const;
		T* get() const;
		operator bool() const;
	private:
		void decrementPointer();
		size_t *m_count;
		T *m_ptr;
	};

	template<class T>
	shared_ptr<T>::shared_ptr(T *ptr)
	{
		m_ptr=ptr;
		m_count=new size_t;
		*m_count=1;
	}	
	template<class T>
	shared_ptr<T>::shared_ptr(const shared_ptr &other)
	{
		m_ptr=other.m_ptr;
		m_count=other.m_count;
		++*m_count;
	}	
	template<class T>
	shared_ptr<T> & shared_ptr<T>::operator=(const shared_ptr &rhs)
	{
		decrementPointer();
		m_ptr=rhs.m_ptr;
		m_count=rhs.m_count;
		++*m_count;
		return *this;
	}
	template<class T>
	shared_ptr<T> & shared_ptr<T>::operator=(T *rhs)
	{
		decrementPointer();
		m_ptr=rhs;
		m_count=new size_t;
		*m_count=1;
		return *this;
	}

	template<class T>
	shared_ptr<T>::~shared_ptr()
	{
		decrementPointer();
	}
	template<class T>
	void shared_ptr<T>::decrementPointer()
	{
		--*m_count;
		if(m_count==0)
		{
			delete m_ptr;
			delete m_count;
		}
	}
	template<class T>
	T & shared_ptr<T>::operator*() const
	{
		return *m_ptr;
	}
	template<class T>
	T* shared_ptr<T>::operator->() const
	{
		return m_ptr;
	}
	template<class T>
	T* shared_ptr<T>::get() const
	{
		return m_ptr;
	}
	template<class T>
	shared_ptr<T>::operator bool() const
	{
		return m_ptr!=NULL;
	}
	
	template<class T>
	bool operator == (const shared_ptr<T> &lhs, const T *rhs)
	{
		return lhs.get()==rhs;
	}
	template<class T>
	bool operator == (const T *rhs, const shared_ptr<T> &lhs )
	{
		return lhs==rhs.get();
	}
	template<class T>
	bool operator == (const shared_ptr<T> &lhs, const shared_ptr<T> &rhs)
	{
		return lhs.get()==rhs.get();
	}
	
	namespace distribution
	{
		double normal(double x, double mean, double standardDeviation);
		double cumulativeNormal(double x, double mean, double standardDeviation);
		double lognormal(double x, double logMean, double logStandardDeviation);
		double cumulativeLognormal(double x, double logMean, double logStandardDeviation);
		double exponential(double x, double rate);
		double cumulativeExponential(double x, double rate);
		/*double gamma(double x, double shape, double scale);
		double cumulativeGamma(double x, double shape, double scale);
		double generalisedGamma(double x, double shapeK, double shapeGamma, double scale);
		double cumulativeGeneralisedGamma(double x, double shapeK, double shapeGamma, double scale);*/
		double weibull(double x, double shape, double scale);
		double cumulativeWeibull(double x, double shape, double scale);
		double rayleigh(double x, double scale);
		double cumulativeRayleigh(double x, double scale);
		double boltzmann(double energy, double temperature);
		double cumulativeBoltzmann(double energy, double temperature);
		double cauchy(double x, double mean, double halfWidthAtHalfMaximum);
		double cumulativeCauchy(double x, double mean, double halfWidthAtHalfMaximum);
		
	}

	//double gamma( double x );
	//double lowerIncompleteGamma( double a, double x );
	//double upperIncompleteGamma( double a, double x );

	void solveQuadratic(double a, double b, double c, std::complex<double> &sln1, std::complex<double> &sln2);
	void solveCubic(double a, double b, double c, double d, std::complex<double> &sln1, std::complex<double> &sln2, std::complex<double> &sln3);
	void solveQuadratic(double a, double b, double c, double &sln1, double &sln2);
	void solveCubic(double a, double b, double c, double d, double &sln1, double &sln2, double &sln3);

	inline double realOrNan(std::complex<double> a)
	{
		return a.imag()==0.0 ? a.real() : std::numeric_limits<double>::quiet_NaN();
	}

	bool anyTrue(const std::vector<SBOOL> &v);
	bool allTrue(const std::vector<SBOOL> &v);
	bool anyFalse(const std::vector<SBOOL> &v);
	bool allFalse(const std::vector<SBOOL> &v);

	std::vector<SBOOL> rollingOr(const std::vector<SBOOL> &v, size_t n);
	std::vector<SBOOL> rollingAnd(const std::vector<SBOOL> &v, size_t n);

	//differentiate the function (or functor) FUNCTION, which takes a vector of x values
	//x is the x values where you wishe to differentiate
	//index is the x value you wish to differentiate against
	//n is the differential you want (1-4 for 1st to 4th differential)
	//order is the accuracy order - currently only 2 is supported
	/*template<double (*FUNCTION)(const std::vector<double> &)>
	double differential(const std::vector<double> x, size_t index, size_t n, size_t order=2)
	{
		sci::assertThrow(n>0 && n<5);
		sci::assertThrow(index<x.size());
		sci::assertThrow(order==2);
		double h=std::max(std::numeric_limits<double>::min(),std::sqrt(std::numeric_limits<double>::epsilon())*x[index]);

		std::vector<double> xMinusH=x;
		std::vector<double> xPlusH=x;
		xMinusH[index]-=h;
		xPlusH[index]+=h;
		if(n==2)
		{
			switch (n)
			{
			case 1:
				return( -1.0/2.0*(-FUNCTION(xMinusH)+FUNCTION(xPlusH)) )/h;
			case 2:
				return( FUNCTION(xMinusH)+FUNCTION(xPlusH)
					-2.0*FUNCTION(x) )/h/h;
			case 3:
				std::vector<double> xMinus2H=xMinusH;
				std::vector<double> xPlus2H=xPlusH;
				xMinus2H[index]-=h;
				xPlus2H[index]+=h;
				return( -1.0/2.0*(-FUNCTION(xMinus2H)+FUNCTION(xPlus2H))
					+FUNCTION(xMinusH)-FUNCTION(xPlusH) )/h/h/h;
			case 4:
				std::vector<double> xMinus2H=xMinusH;
				std::vector<double> xPlus2H=xPlusH;
				xMinus2H[index]-=h;
				xPlus2H[index]+=h;
				return( FUNCTION(xMinus2H)+FUNCTION(xPlus2H)
					-4.0*(xMinusH+xPlusH)
					+6.0*x )/h/h/h/h;
			}
		}
		else return std::numeric_limits<double>::quietNan();

	}*/

	
	//template<double (*FUNCTION)(const std::vector<double> &)>
	//double differential(const std::vector<double> x, size_t index, size_t n, size_t order=2)

	template<double (*FUNCTION)(const std::vector<double> &)>
	double differential(const std::vector<double> &x, size_t index)
	{
		double h=std::max(std::numeric_limits<double>::min(),std::numeric_limits<double>::epsilon());
		std::vector<double> xMinus=x;
		std::vector<double> xPlus=x;
		double upperLimit=std::numeric_limits<double>::max();
		double lowerLimit=-upperLimit;
		double result=0.0;
		int n=0;
		while(1)
		{
			xMinus[index]=x[index]-h;
			xPlus[index]=x[index]+h;
			double hActual=xPlus[index]-xMinus[index];
			double dif=(FUNCTION(xPlus)-FUNCTION(xMinus))/hActual;
			double err=std::numeric_limits<double>::epsilon()/hActual+dif*std::numeric_limits<double>::epsilon();
			upperLimit=std::min(upperLimit,dif+err);
			lowerLimit=std::max(lowerLimit,dif-err);
			if(lowerLimit>upperLimit)
				break;
			result=dif;
			h*=2.0;
			++n;
		}
		return result;
	}

	size_t count(const std::vector<SBOOL> &v);

	double dateToUnixTime( int year, int month, int day, int hour, int minute, double second, bool daylightSaving );

	class Random
	{
	public:
		Random(size_t seed);
		Random();
		virtual ~Random(){}
	protected:
		size_t getInt();
		size_t getMax() const;
		size_t getMin() const;
	private:
		std::mt19937_64 m_engine;
	};

	class RandomInt : public Random
	{
	public:
		RandomInt(size_t seed);
		RandomInt();
		size_t get();
	private:
	};

	class RandomReal :public Random
	{
	public:
		RandomReal(size_t seed);
		RandomReal();
		virtual double get();
	};

	class RangedRandomReal :public RandomReal
	{
	public:
		RangedRandomReal(size_t seed, double min, double max);
		RangedRandomReal(double min, double max);
		virtual double get();
	private:
		double m_min;
		double m_max;
	};

	class ProbabilityDistributionFunction
	{
	public:
		virtual double getProbability(const std::vector<double> &x) const = 0;
	};

	class MultivariateNormalDistribution : public ProbabilityDistributionFunction
	{
	public:
		MultivariateNormalDistribution(const std::vector<double> &means, const std::vector<double> &standardDeviations);
		MultivariateNormalDistribution(const std::vector<double> &means, const std::vector<std::vector<double>> &covarianceMatrix);
		double getProbability(const std::vector<double> &x) const;
	private:
		std::vector<double> m_means;
		std::vector<double> m_standardDeviations;
		std::vector<std::vector<double>> m_covarianceMatrix;
		double m_determinantCovariance;
	};

	class NormalDistribution : public MultivariateNormalDistribution
	{
	public:
		NormalDistribution(double mean, double standardDeviation);
		double getProbability(double x) const;
	};

	class MultivariateInverseCumulativeDistributionFunction
	{
	public:
		//get x from the cdf for one dimension given the values in all other dimensions. otherXs should have one element for each dimension
		//but the value in otherXs[undefinedIndex] will not be used
		virtual double getX(double cumulativeProbability, std::vector<double> otherXs, size_t undefinedIndex) const = 0;
		virtual ~MultivariateInverseCumulativeDistributionFunction() {}
	};

	class UnivariateInverseCumulativeDistributionFunction
	{
	public:
		virtual double getX(double cumulativeProbability) const = 0;
		virtual ~UnivariateInverseCumulativeDistributionFunction() {}
	};

	class MultivariateInverseCumulativeNormalDistribution : MultivariateInverseCumulativeDistributionFunction
	{
	public:
		MultivariateInverseCumulativeNormalDistribution(const std::vector<double> &means, const std::vector<double> &standardDeviations);
		double getX(double cumulativeProbability, std::vector<double> otherXs, size_t undefinedIndex) const;
	private:
		std::vector<double> m_means;
		std::vector<double> m_standardDeviations;
	};

	class InverseCumulativeNormalDistribution : public UnivariateInverseCumulativeDistributionFunction
	{
	public:
		InverseCumulativeNormalDistribution(double mean, double standardDeviation);
		double getX(double cumulativeProbability) const;
	private:
		double m_mean;
		double m_standardDeviation;
		virtual ~InverseCumulativeNormalDistribution() {}
	};

	class MarkovChain : public RandomReal
	{
	public:
		MarkovChain(size_t seed) : RandomReal(seed) {};
		MarkovChain() : RandomReal() {};
		virtual std::vector<double> getNext()=0;
		std::vector<double> getExpectation(size_t nIntegrationSteps);
		std::vector<std::vector<double>> getMoments(size_t nIntegrationSteps, std::vector<int> moments); //zeroth moment will be mean, 2nd variance, etc
	protected:
		//gets a random number over the range max-min for use internally
		double getUniformRandom(double min, double max);
	};

	class MetropolisHastingsMarkovChain : public MarkovChain
	{
	public:
		MetropolisHastingsMarkovChain(std::vector<double> startPoint, size_t nBurnInIterations, ProbabilityDistributionFunction *probabilityDistributionFunction, std::vector<UnivariateInverseCumulativeDistributionFunction *> jumpDistributions);
		MetropolisHastingsMarkovChain(std::vector<double> startPoint, size_t nBurnInIterations, ProbabilityDistributionFunction *probabilityDistributionFunction, std::vector<double> jumpStandardDeviations);
		~MetropolisHastingsMarkovChain();
		virtual std::vector<double> getNext();
		void resetAcceptanceRatio();
		double getAcceptanceRatio();
	private:
		std::vector<double> m_currentPoint;
		double m_currentProbability;
		ProbabilityDistributionFunction *m_probabilityDistributionFunction;
		std::vector<UnivariateInverseCumulativeDistributionFunction *> m_jumpDistributions;
		size_t m_nCalls;
		size_t m_nAccepted;
		bool m_deleteLocally;
	};

	class GibbsMarkovChain : public MarkovChain
	{
	public:
		GibbsMarkovChain(std::vector<double> startPoint, size_t nBurnInIterations, MultivariateInverseCumulativeDistributionFunction * inverseCumulativeDistributionFunction);
		virtual std::vector<double> getNext();
	private:
		std::vector<double> m_currentPoint;
		MultivariateInverseCumulativeDistributionFunction *m_inverseCumulativeDistributionFunction;
	};

	//end of namespace sci
}



#endif
