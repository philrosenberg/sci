#include <algorithm>

namespace sci
{

	template<class T>
	T min(T var1, T var2)
	{
		if(var1!=var1) return var2;
		if(var2!=var2)return var1;
		return std::min(var1,var2);
	}

	template<class T>
	T max(T var1, T var2)
	{
		if(var1!=var1) return var2;
		if(var2!=var2)return var1;
		return std::max(var1,var2);
	}
}