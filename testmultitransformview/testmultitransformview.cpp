// testmultitransformview.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "../include/scieng/multitransformview.h"
#include "../include/scieng/algorithm.h"
#include<vector>
#include<list>
#include<functional>
#include<iostream>

double plus2(double d1, double d2)
{
    return d1 + d2;
}

double plus3(double d1, double d2, double d3)
{
    return d1 + d2 + d3;
}

bool is_equal(double d1, double d2)
{
    return d1 == d2;
}

int main()
{
    std::vector<double>v1{ 1.0, 2.0, 3.0 };
    std::vector<double>v2{ 1.0, 2.0, 3.0 };

    using vtuple = std::tuple<const std::vector<double>&, const std::vector<double>&>;
    using iter = std::remove_reference_t<std::tuple_element_t<0, vtuple>>::const_iterator;
    using size_type = std::vector<double>::size_type;

    sci::multitransform_view<plus2, std::vector<double>, std::vector<double>> view1(v1, v2);
    auto view2 = sci::make_multitransform_view<plus2> (v1, v2);
    auto view3 = sci::make_multitransform_view<plus3>(v1, v2, v1);
    std::cout << view1[0] << " " << view1[1] << " " << view1[2] << "\n";
    std::cout << view2[0] << " " << view2[1] << " " << view2[2] << "\n";
    std::cout << view3[0] << " " << view3[1] << " " << view3[2] << "\n";

    std::list<double> l1{ 1.0, 2.0, 3.0 };
    auto view4 = sci::make_multitransform_view<plus2>(v1, l1);
    for (auto iter = view4.begin(); iter != view4.end(); ++iter )
        std::cout << *iter << " ";
    std::cout << "\n";

    using iterator = decltype(view2.begin());
    static_assert(std::input_or_output_iterator<iterator>);
    static_assert(std::input_iterator<iterator>);
    static_assert(std::forward_iterator<iterator>);


    std::vector<double> v3{ 1.0, 4.0, 3.0 };
    std::vector<int> toFilter{ 1, 2, 3 };
    std::vector<int> filtered(3);
    auto end = sci::copy_if_multi<is_equal>(toFilter.begin(), toFilter.end(), filtered.begin(), v1.begin(), v3.begin());
    filtered.resize(end - filtered.begin());

}
