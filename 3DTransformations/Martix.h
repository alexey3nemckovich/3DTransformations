#include <vector>
#include <exception>
using namespace std;


template<typename MatrixElement>
class Matrix
{
public:
	Matrix(MatrixElement** elements);
	Matrix(int countRows, int countColumns);
	~Matrix();

public:
	int GetCountRows();
	int GetCountColumns();

public:
	MatrixElement* operator[](int row);
	Matrix<MatrixElement> operator*(const Matrix<MatrixElement>& other);

private:
	int _countRows;
	int _countColumns;

private:
	MatrixElement** _elements;
};


template<typename MatrixElement>
Matrix<MatrixElement>::Matrix(MatrixElement** elements)
{
	_countRows = sizeof(elements) / sizeof(MatrixElement*);

	if (_countRows)
	{
		_countColumns = sizeof(elements[0]) / sizeof(MatrixElement);
	}
}


template<typename MatrixElement>
Matrix<MatrixElement>::Matrix(int countRows, int countColumns)
	: _countRows(countRows),
	_countColumns(countColumns)
{
	_elements = new int*[_countRows];
	for (int i = 0; i < _countRows; i++)
	{
		_elements[i] = new int[_countColumns];
		for (int j = 0; j < _countColumns; j++)
		{
			_elements[i][j] = 0;
		}
	}
}


template<typename MatrixElement>
int Matrix<MatrixElement>::GetCountRows()
{
	return _countRows;
}


template<typename MatrixElement>
int Matrix<MatrixElement>::GetCountColumns()
{
	return _countColumns;
}


template<typename MatrixElement>
Matrix<MatrixElement>::~Matrix()
{
	for (int i = 0; i < _countRows; i++)
	{
		delete[] _elements[i];
	}
	delete _elements;
}


template<typename MatrixElement>
Matrix<MatrixElement> Matrix<MatrixElement>::operator*(const Matrix<MatrixElement>& other)
{
	if (_countColumns != other._countRows)
	{
		throw exception();
	}

	Matrix<MatrixElement> result(_countRows, other._countColumns);
	for (int i = 0; i < _countRows; i++)
	{
		for (int j = 0; j < other._countColumns; j++)
		{
			result[i][j] = 0;
			for (int k = 0; k < _countColumns; k++)
			{
				result[i][j] += _elements[i][k] * other[k][j];
			}
		}
	}
	return result;
}


template<typename MatrixElement>
MatrixElement* Matrix<MatrixElement>::operator[](int row)
{
	return _elements[row];
}
