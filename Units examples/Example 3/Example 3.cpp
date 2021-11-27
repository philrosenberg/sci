#include <iostream>
#include <Units.h>

typedef sci::Physical<sci::Metre<>, double> metre;
typedef sci::Physical<sci::Second<>, double> second;
typedef sci::Physical<sci::Metre<2>, double> metreSquared;
typedef sci::Physical<sci::Metre<3, sci::micro>, double> microMetreCubed;
typedef decltype(sci::Physical<sci::Metre<6, sci::milli>, double>()/sci::Physical<sci::Metre<3>,double>()) radarReflectivity;
//below is an alternate way to define radarReflectivity
//using the DividedUnit class
//typedef sci::Physical<sci::DividedUnit<sci::Metre<6, sci::milli>, sci::Metre<3>>, double> radarReflectivity;

int main()
{
    metre length(21.0);
    second time(5.0);
    auto area1 = length * length;
    auto area2 = sci::pow<2>(length);
    microMetreCubed volume = sci::pow<3>(length);
    std::cout << "Speed = " << length / time << std::endl;
    std::cout << "Area1 = " << metreSquared(area1) << std::endl;
    std::cout << "Area2 = " << area2 << std::endl;
    std::cout << "Length1 = " << metre(sci::sqrt(area1)) << std::endl;
    std::cout << "Length2 = " << sci::sqrt(area2) << std::endl;
    std::cout << "Volume = " << volume << std::endl;
    std::cout << radarReflectivity(1) << " = " << microMetreCubed(radarReflectivity(1)) << std::endl;

    return 0;
}
