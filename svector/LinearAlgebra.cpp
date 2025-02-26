#include "../include/svector/LinearAlgebra.h"

#include"eigeneng/eigeneng.h"

void sci::eigenvalues(const std::vector<std::vector<double>>& matrix, std::vector<double>& eigenvaluesReal, std::vector<double>& eigenvaluesImaginary)
{
	//sci::assertThrow(sci::square(matrix), sci::err(SERR_VECTOR, -999, "sci::eigenvalues called with a non-square matrix."));
	if (matrix.size() == 0)
	{
		eigenvaluesReal.resize(0);
		eigenvaluesImaginary.resize(0);
		return;
	}
	size_t n = matrix.size();
	eigenvaluesReal.resize(n);
	eigenvaluesImaginary.resize(n);
	std::vector<double> workingMatrix(n * n);
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < n; ++j)
			workingMatrix[i * n + j] = matrix[i][j];

	int result = n_eigeng(&workingMatrix[0], n, &eigenvaluesReal[0], &eigenvaluesImaginary[0], NULL);
	sci::assertThrow(result == 0, sci::err(sci::SERR_EIGENENG, result, sU("Error calculating eigen values.")));
}

void sci::eigenvectors(const std::vector<std::vector<double>>& matrix, std::vector<double>& eigenvaluesReal, std::vector<double>& eigenvaluesImaginary, std::vector<std::vector<double>>& eigenvectorsReal, std::vector<std::vector<double>>& eigenvectorsImaginary)
{
	//sci::assertThrow(sci::square(matrix), sci::err(SERR_VECTOR, -999, "sci::eigenvalues called with a non-square matrix."));
	if (matrix.size() == 0)
	{
		eigenvaluesReal.resize(0);
		eigenvaluesImaginary.resize(0);
		return;
	}
	size_t n = matrix.size();
	eigenvaluesReal.resize(n);
	eigenvaluesImaginary.resize(n);
	std::vector<double> workingMatrix(n * n);
	for (size_t i = 0; i < n; ++i)
		for (size_t j = 0; j < n; ++j)
			workingMatrix[i * n + j] = matrix[i][j];

	std::vector<double> workingVector(n * n);

	int result = n_eigeng(&workingMatrix[0], n, &eigenvaluesReal[0], &eigenvaluesImaginary[0], &workingVector[0]);
	sci::assertThrow(result == 0, sci::err(sci::SERR_EIGENENG, result, sU("Error calculating eigen values.")));

	eigenvectorsReal.resize(n);
	eigenvectorsImaginary.resize(n);
	for (size_t i = 0; i < n; ++i)
	{
		eigenvectorsReal[i].resize(n);
		eigenvectorsImaginary[i].resize(n);
		for (size_t j = 0; j < n; ++j)
		{
			eigenvectorsReal[i][j] = workingVector[i * n + j];
		}
	}

	for (size_t i = 0; i < n - 1; ++i)
	{
		if (eigenvaluesImaginary[i] != 0.0)
		{
			eigenvectorsImaginary[i] = eigenvectorsReal[i + 1];
			eigenvectorsReal[i + 1] = eigenvectorsReal[i];

			eigenvectorsImaginary[i + 1] = eigenvectorsImaginary[i];
			for (size_t j = 0; j < eigenvectorsImaginary[i + 1].size(); ++j)
				eigenvectorsImaginary[i][j] = -eigenvectorsImaginary[i][j];
		}
	}
}