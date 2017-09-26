#pragma once
#include <vector>
#include <exception>
using namespace std;


namespace cs
{


	template<typename MatrixElement>
	class Matrix
	{
	public:
		Matrix();
		Matrix(int countRows, int countColumns);
		Matrix(const Matrix& other);
		virtual ~Matrix();

	public:
		int GetCountRows();
		int GetCountColumns();

	public:
		MatrixElement* operator[](int row) const;
		Matrix<MatrixElement> operator*(const Matrix<MatrixElement>& other);
		Matrix<MatrixElement>& operator=(const Matrix<MatrixElement>& other);
		Matrix<MatrixElement>& operator*=(const Matrix<MatrixElement>& other);

	private:
		void AllocElements();
		void AllocAndInitElements(const Matrix<MatrixElement>& other);
		void ChangeCountColumns(int newColumnsCount);
		void DeallocElements();
		void InitElements(const Matrix<MatrixElement>& other);

	protected:
		MatrixElement** _elements;

	private:
		int _countRows;
		int _countColumns;
	};


	template<typename MatrixElement>
	Matrix<MatrixElement>::Matrix()
	{
		_countRows = 0;
		_countColumns = 0;
	}


	template<typename MatrixElement>
	Matrix<MatrixElement>::Matrix(int countRows, int countColumns)
		: _countRows(countRows),
		_countColumns(countColumns)
	{
		AllocElements();
	}


	template<typename MatrixElement>
	Matrix<MatrixElement>::Matrix(const Matrix& other)
	{
		_countRows = other._countRows;
		_countColumns = other._countColumns;

		AllocAndInitElements(other);
	}


	template<typename MatrixElement>
	Matrix<MatrixElement>::~Matrix()
	{
		DeallocElements();
	}


	template<typename MatrixElement>
	void Matrix<MatrixElement>::AllocElements()
	{
		_elements = new MatrixElement*[_countRows];
		for (int i = 0; i < _countRows; i++)
		{
			_elements[i] = new MatrixElement[_countColumns];
			for (int j = 0; j < _countColumns; j++)
			{
				_elements[i][j] = MatrixElement();
			}
		}
	}


	template<typename MatrixElement>
	void Matrix<MatrixElement>::AllocAndInitElements(const Matrix<MatrixElement>& other)
	{
		_elements = new MatrixElement*[_countRows];
		for (int i = 0; i < _countRows; i++)
		{
			_elements[i] = new MatrixElement[_countColumns];
			for (int j = 0; j < _countColumns; j++)
			{
				_elements[i][j] = other[i][j];
			}
		}
	}


	template<typename MatrixElement>
	void Matrix<MatrixElement>::ChangeCountColumns(int newColumnsCount)
	{
		for (int i = 0; i < _countRows; i++)
		{
			delete _elements[i];
		}

		_countColumns = newColumnsCount;
		for (int i = 0; i < _countRows; i++)
		{
			_elements[i] = new MatrixElement[_countColumns];
			for (int j = 0; j < _countColumns; j++)
			{
				_elements[i][j] = MatrixElement();
			}
		}
	}


	template<typename MatrixElement>
	void Matrix<MatrixElement>::DeallocElements()
	{
		for (int i = 0; i < _countRows; i++)
		{
			delete[] _elements[i];
		}
		delete[] _elements;
	}


	template<typename MatrixElement>
	void Matrix<MatrixElement>::InitElements(const Matrix<MatrixElement>& other)
	{
		for (int i = 0; i < _countRows; i++)
		{
			for (int j = 0; j < _countColumns; j++)
			{
				_elements[i][j] = other[i][j];
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
	Matrix<MatrixElement>& Matrix<MatrixElement>::operator=(const Matrix<MatrixElement>& other)
	{
		if (_countRows == other._countRows)
		{
			ChangeCountColumns(other._countColumns);
			InitElements(other);
		}
		else
		{
			DeallocElements();

			_countRows = other._countRows;
			_countColumns = other._countColumns;
			AllocAndInitElements(other);
		}

		return *this;
	}


	template<typename MatrixElement>
	Matrix<MatrixElement>& Matrix<MatrixElement>::operator*=(const Matrix<MatrixElement>& other)
	{
		if (_countColumns != other._countRows)
		{
			throw exception();
		}

		if (other._countColumns != _countColumns)
		{
			ChangeCountColumns(other._countColumns);
		}

		Matrix<MatrixElement> prevState = *this;
		for (int i = 0; i < _countRows; i++)
		{
			for (int j = 0; j < other._countColumns; j++)
			{
				_elements[i][j] = 0;
				for (int k = 0; k < _countColumns; k++)
				{
					_elements[i][j] += prevState[i][k] * other[k][j];
				}
			}
		}
	}


	template<typename MatrixElement>
	MatrixElement* Matrix<MatrixElement>::operator[](int row) const
	{
		return _elements[row];
	}


	template<typename Element>
	class HomogeneousPoint
		: public Matrix<Element>
	{
	public:
		HomogeneousPoint();
		HomogeneousPoint(Element x, Element y, Element z);

	public:
		Element x();
		Element y();
		Element z();

	public:
		Element& operator[](int coordinateIndex) const;
		Matrix<Element> operator*(const Matrix<Element>& other);
	};


	template<typename Element>
	HomogeneousPoint<Element>::HomogeneousPoint()
		: Matrix<Element>(1, 4)
	{
		_elements[0][3] = 1;
	}


	template<typename Element>
	HomogeneousPoint<Element>::HomogeneousPoint(Element x, Element y, Element z)
		: Matrix<Element>(1, 4)
	{
		_elements[0][0] = x;
		_elements[0][1] = y;
		_elements[0][2] = z;
		_elements[0][3] = 1;
	}


	template<typename Element>
	Element HomogeneousPoint<Element>::x()
	{
		return _elements[0][0];
	}


	template<typename Element>
	Element HomogeneousPoint<Element>::y()
	{
		return _elements[0][1];
	}


	template<typename Element>
	Element HomogeneousPoint<Element>::z()
	{
		return _elements[0][2];
	}


	template<typename Element>
	Element& HomogeneousPoint<Element>::operator[](int coordinateIndex) const
	{
		return _elements[0][coordinateIndex];
	}


	template<typename Element>
	Matrix<Element> HomogeneousPoint<Element>::operator*(const Matrix<Element>& other)
	{
		return this->Matrix<Element>::operator*(other);// (Matrix<Element>)this * other;
	}


	template<typename Element>
	Matrix<Element> operator*(const HomogeneousPoint<Element>& point, const Matrix<Element>& other)
	{
		return (Matrix<Element>)point * other;
	}


}
