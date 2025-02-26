#pragma once
#include<vector>
#include<array>
#include<svector/serr.h>
#include<svector/math.h>

namespace sci
{
	void eigenvalues(const std::vector<std::vector<double>>& matrix, std::vector<double>& eigenvaluesReal, std::vector<double>& eigenvaluesImaginary);
	void eigenvectors(const std::vector<std::vector<double>>& matrix, std::vector<double>& eigenvaluesReal, std::vector<double>& eigenvaluesImaginary, std::vector<std::vector<double>>& eigenvectorsReal, std::vector<std::vector<double>>& eigenvectorsImaginary);

	template <class T, class U, class V>
	auto solveQuadratic(const T& a, const U& b, const V& c)
	{
		using result_type = decltype((-b + sci::sqrt(b * b - 4 * a * c)) / (2 * a));
		std::array<result_type, 2> solution;
		solution[0] = (-b + sci::sqrt(b * b - 4 * a * c)) / (2 * a);
		solution[1] = (-b + sci::sqrt(b * b - 4 * a * c)) / (2 * a);
		return solution;
	}
}