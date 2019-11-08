// TestFitting.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "../include/svector/svector.h"

int main()
{
	std::vector<double> x
	{
		1.93910483,
		2.294070002,
		3.714949909,
		4.700894048,
		5.887615215,
		6.95536325,
		7.018919912,
		8.839051685,
		9.01705505,
		10.54294996,
		11.35024081,
		12.94475775,
		13.40999665,
		14.74566103,
		15.51923212,
		16.24044425,
		17.57290957,
		18.58641194,
		19.85006693,
		20.77786764,
		21.06630269,
		22.50673249,
		23.29119031,
		24.56356222,
		25.89527839
	};
	std::vector<double> y
	{
		1.394707489,
		2.283951116,
		3.633780807,
		4.937928139,
		5.145177807,
		6.949405712,
		7.26833171,
		8.142447131,
		9.702345138,
		10.22805781,
		11.53533125,
		12.37623364,
		13.60425677,
		14.37904791,
		15.21270137,
		16.54428409,
		17.18325352,
		18.94607653,
		19.65466969,
		20.5002227,
		21.05459311,
		22.88678747,
		23.94438812,
		24.30915245,
		25.48955107
	};
	double gradient;
	double intercept;
	double varGradient;
	double varIntercept;
	double covar;

	sci::fitstraightline(x, y, gradient, intercept);

	sci::assertThrow(gradient > 1.006942, sci::err(sci::SERR_USER, 0, sU("Error in TestFitting")));
	sci::assertThrow(gradient < 1.006943, sci::err(sci::SERR_USER, 0, sU("Error in TestFitting")));
	sci::assertThrow(intercept > -0.17116, sci::err(sci::SERR_USER, 0, sU("Error in TestFitting")));
	sci::assertThrow(intercept < -0.17115, sci::err(sci::SERR_USER, 0, sU("Error in TestFitting")));

	sci::fitstraightline(x, y, gradient, intercept,varGradient, varIntercept, covar);

	sci::assertThrow(gradient > 1.006942, sci::err(sci::SERR_USER, 0,sU("Error in TestFitting")));
	sci::assertThrow(gradient < 1.006943, sci::err(sci::SERR_USER, 0, sU("Error in TestFitting")));
	sci::assertThrow(intercept > -0.17116, sci::err(sci::SERR_USER, 0, sU("Error in TestFitting")));
	sci::assertThrow(intercept < -0.17115, sci::err(sci::SERR_USER, 0, sU("Error in TestFitting")));
	sci::assertThrow(varGradient > 0.00012572, sci::err(sci::SERR_USER, 0, sU("Error in TestFitting")));
	sci::assertThrow(varGradient < 0.00012573, sci::err(sci::SERR_USER, 0, sU("Error in TestFitting")));
	sci::assertThrow(varIntercept > 0.029634, sci::err(sci::SERR_USER, 0, sU("Error in TestFitting")));
	sci::assertThrow(varIntercept < 0.029635, sci::err(sci::SERR_USER, 0, sU("Error in TestFitting")));
}

