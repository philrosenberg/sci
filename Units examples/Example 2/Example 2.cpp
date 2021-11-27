#include <iostream>
#include <Units.h>

typedef sci::Physical<sci::Metre<>, double> metre;
typedef sci::Physical<sci::Second<>, double> second;

int main()
{
    metre length(21.0);
    second time(5.0);
    auto area1 = length * length;
    auto area2 = sci::pow<2>(length);
    std::cout << "Speed = " << length / time << std::endl;
    std::cout << "Area1 = " << area1 << std::endl;
    std::cout << "Area2 = " << area2 << std::endl;
    std::cout << "Length1 = " << sci::sqrt(area1) << std::endl;
    std::cout << "Length2 = " << sci::sqrt(area2) << std::endl;

    return 0;
}
