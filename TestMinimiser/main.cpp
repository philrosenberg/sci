#define NOMINMAX
#include"../include/svector/Units.h"
#include"../include/svector/sminimiser2.h"
#include<iostream>

double easyQuadratic(double x)
{
	return (x - 1.5) * (x - 1.5) + 3.5;
}

template<sci::IsGrid GRID>
double easyQuadratic2d(GRID x)
{
	std::cout << "x = " << x[0] << "," << x[1] << " result = " << (x[0] - 1.5) * (x[0] - 1.5) + (x[1] + 4.0) * (x[1] + 4.0) + 3.5 << "\n";
	return (x[0] - 1.5) * (x[0] - 1.5) + (x[1] + 4.0) * (x[1] + 4.0) + 3.5;
}

using metre = sci::Physical<sci::Metre<>, double>;
using second = sci::Physical<sci::Second<>, double>;
using metrePerSecond = decltype(metre() / second());
using unitless = sci::Physical<sci::Unitless, double>;

second unitsQuadratic(metre x)
{
	return (x - metre(1.5)) * (x - metre(1.5)) / metre(1) / metrePerSecond(1) + second(3.5);
}

template<sci::IsGrid GRID>
second unitsQuadratic2d(GRID x)
{
	//grid must be a grid of metres or this ill generate compile errors
	std::cout << "x = " << x[0] << "," << x[1] << " result = " << second(((x[0] - metre(1.5)) * (x[0] - metre(1.5)) + (x[1] + metre(4.0)) * (x[1] + metre(4.0)))/metre(1)/metrePerSecond(1) + second(3.5)) << "\n";
	return second(((x[0] - metre(1.5)) * (x[0] - metre(1.5)) + (x[1] + metre(4.0)) * (x[1] + metre(4.0))) / metre(1) / metrePerSecond(1) + second(3.5));
}

class PowellStopper
{
public:
	template<class T, class U>
	bool operator()(const T&, const U&) const
	{
		return false;
	}
};

int main()
{
	//test brent
	{
		double guess = 3;
		sci::BrentStopper<double, double> stopper(0.01, 0.00001);
		double result = sci::minimiseBrent(easyQuadratic, guess, 1, stopper);
		std::cout << "Brent minimum found at " << result << " (should be 1.5)\n\n";
	}

	//test brent where the input and output types are not the same
	{
		metre guess(3);
		sci::BrentStopper<metre, second> stopper(metre(0.01), unitless(0.00001));
		metre result = sci::minimiseBrent(unitsQuadratic, guess, metre(1), stopper);
		std::cout << "Brent minimum found at " << result << " (should be 1.5 m)\n\n";
	}

	//test brent along line
	{
		//do a search along the x direction only
		sci::GridData<double, 1> guess{ 0.0, 0.0 };
		sci::minimiseOnLineBrent(easyQuadratic2d<sci::GridData < double, 1>>, guess, sci::GridData < double, 1>{1, 1}, sci::GridData < double, 1>{1, 0}, -1);
		std::cout << "Brent minimum found at " << guess[0] << ", " << guess[1] << " (should be 1.5, 0)\n\n";

		//do a search along the y direction only
		guess = sci::GridData<double, 1>{ 0.0, 0.0 };
		sci::minimiseOnLineBrent(easyQuadratic2d<sci::GridData < double, 1>>, guess, sci::GridData < double, 1>{1, 1}, sci::GridData < double, 1>{0, 1}, -1);
		std::cout << "Brent minimum found at " << guess[0] << ", " << guess[1] << " (should be 0, -4)\n\n";

		//do a search in the direction of the known minima
		guess = sci::GridData<double, 1>{ 0.0, 0.0 };
		sci::minimiseOnLineBrent(easyQuadratic2d<sci::GridData < double, 1>>, guess, sci::GridData < double, 1>{1, 1}, sci::GridData < double, 1>{1, -4.0/1.5}, 1);
		std::cout << "Brent minimum found at " << guess[0] << ", " << guess[1] << " (should be 1.5, -4)\n\n";
	}


	//test brent along line where the input and output types are not the same 
	{
		//do a search along the x direction only
		sci::GridData<metre, 1> guess{ metre(0.0), metre(0.0) };
		sci::minimiseOnLineBrent(unitsQuadratic2d<sci::GridData < metre, 1>>, guess, sci::GridData < metre, 1>{metre(1), metre(1)}, sci::GridData < metre, 1>{metre(1), metre(0)}, -1);
		std::cout << "Brent minimum found at " << guess[0] << ", " << guess[1] << " (should be 1.5 m, 0 m)\n\n";

		//do a search along the y direction only
		guess = sci::GridData<metre, 1>{ metre(0.0), metre(0.0) };
		sci::minimiseOnLineBrent(unitsQuadratic2d<sci::GridData < metre, 1>>, guess, sci::GridData < metre, 1>{metre(1), metre(1)}, sci::GridData < metre, 1>{metre(0), metre(1)}, -1);
		std::cout << "Brent minimum found at " << guess[0] << ", " << guess[1] << " (should be 0 m, -4 m)\n\n";

		//do a search in the direction of the known minima
		guess = sci::GridData<metre, 1>{ metre(0.0), metre(0.0) };
		sci::minimiseOnLineBrent(unitsQuadratic2d<sci::GridData < metre, 1>>, guess, sci::GridData < metre, 1>{metre(1), metre(1)}, sci::GridData < metre, 1>{metre(1), metre(-4.0 / 1.5)}, -1);
		std::cout << "Brent minimum found at " << guess[0] << ", " << guess[1] << " (should be 1.5 m, -4 m)\n\n";
	}

	//test powell
	{
		sci::GridData<double, 1> guess{ 0.0, 0.0 };
		PowellStopper stopper;
		sci::minimisePowell(easyQuadratic2d<sci::GridData < double, 1>>,stopper, guess, sci::GridData < double, 1>{1, 1}, sci::GridData < double, 1>{0.001, 0.001}, sci::GridData < double, 1>{0.001, 0.001}, 1);
		std::cout << "Powell minimum found at " << guess[0] << ", " << guess[1] << " (should be 1.5, -4)\n\n";
	}

	//test powell where the input and output types are not the same
	{
		sci::GridData<metre, 1> guess{ metre(0.0), metre(0.0) };
		PowellStopper stopper;
		sci::minimisePowell(unitsQuadratic2d<sci::GridData < metre, 1>>, stopper, guess, sci::GridData < metre, 1>{metre(1), metre(1)}, sci::GridData < metre, 1>{metre(0.001), metre(0.001)}, sci::GridData < unitless, 1>{unitless(0.001), unitless(0.001)}, 1);
		std::cout << "Powell minimum found at " << guess[0] << ", " << guess[1] << " (should be 1.5 m, -4 m)\n\n";
	}
}