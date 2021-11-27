#include <iostream>
#include <Units.h>

int main()
{
    sci::Physical<sci::Metre<>, double> length(21.0);
    sci::Physical<sci::Second<>, double> time(5.0);
    std::cout << "Speed = " << length / time;

    return 0;
}