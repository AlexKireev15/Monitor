#include "../Matrix.h"

#include <exception>

Matrix::Matrix(unsigned int size) : m_size(size)
{
	m_value = new double[size * size];
	for (auto idx = 0u; idx < size * size; ++idx)
	{
		m_value[idx] = 0.;
	}
}

Matrix operator*(const Matrix& lhs, const Matrix& rhs)
{
	if (lhs.m_size != rhs.m_size)
		throw std::exception("Matrices sizes don't matches");

	Matrix resultMatrix(lhs.m_size);
	for (auto rowIdx = 0u; rowIdx < lhs.m_size; ++rowIdx)
	{
		for (auto colIdx = 0u; colIdx < rhs.m_size; ++colIdx)
		{
			double res = 0.;
			for (auto tmpIdx = 0u; tmpIdx < lhs.m_size; ++tmpIdx)
			{
				res += lhs.Get(rowIdx, tmpIdx) * rhs.Get(tmpIdx, colIdx);
			}
			resultMatrix.Set(rowIdx, colIdx, res);
		}
	}

	return resultMatrix;
}