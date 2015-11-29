#include"math_internal.h"
#include<alg/specialfunctions.h>

double sci::erf(double value)
{
	return alglib::errorfunction(value);
}
