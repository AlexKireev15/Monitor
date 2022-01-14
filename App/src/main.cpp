#include "Matrix/Matrix.h"

#include <iostream>
#include <thread>
#include <random>
#include <limits>

static bool running = true;

Matrix GetRandMatrix(unsigned int size)
{
	const double dMin = -1000000.;
	const double dMax = 1000000.;
	std::uniform_real_distribution<double> distr(dMin, dMax);
	std::default_random_engine eng;

	double * values = new double[size* size];
	for (auto idx = 0u; idx < size * size; ++idx)
	{
		values[idx] = distr(eng);
	}

	return Matrix(size, values);	
}

void PrintMatrix(const Matrix& m)
{
	std::cout.precision(2);
	for (auto rowIdx = 0u; rowIdx < m.GetSize(); ++rowIdx)
	{
		for (auto colIdx = 0u; colIdx < m.GetSize(); ++colIdx)
		{
			std::cout << m.Get(rowIdx, colIdx) << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

Matrix MultiplyRandMatrices(unsigned int size)
{
	Matrix a = GetRandMatrix(size), b = GetRandMatrix(size);
	return a * b;
}

void DoCalculations(unsigned int size) 
{
	while (running)
	{
		auto res = MultiplyRandMatrices(size);
		PrintMatrix(res);
	}
}


int main()
{
	unsigned int size = 1000;
	std::thread calculationThread(DoCalculations, size);
	std::cout << "Calculating random matrices mult until any key press..." << std::endl;
	while (running)
	{
		system("pause\n");
		running = false;
		calculationThread.join();
	}
	return 0;
}