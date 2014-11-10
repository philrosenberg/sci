#include"../include/svector\dep\math.h"
#include<alg/specialfunctions.h>

double sci::erf(double value)
{
	return alglib::errorfunction(value);
}
