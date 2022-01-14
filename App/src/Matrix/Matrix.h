#pragma once

class Matrix
{
	friend Matrix operator*(const Matrix& lhs, const Matrix& rhs);
public:
	Matrix(unsigned int size);
	Matrix(unsigned int size, double * values) : m_size(size), m_value(values) { }
	double Get(unsigned int row, unsigned int col) const
	{
		return m_value[row * m_size + col];
	}
	unsigned int GetSize() const
	{
		return m_size;
	}
	void Set(unsigned int row, unsigned int col, double value)
	{
		m_value[row * m_size + col] = value;
	}
	virtual ~Matrix()
	{
		delete[] m_value;
	}

private:
	unsigned int m_size;
	double * m_value = nullptr;
};