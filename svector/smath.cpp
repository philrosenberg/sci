#include"math_internal.h"
#include<alg/specialfunctions.h>

double sci::erf(double value)
{
	return alglib::errorfunction(value);
}

double sci::erfInverse(double value)
{
	return alglib::inverf(value);
}
