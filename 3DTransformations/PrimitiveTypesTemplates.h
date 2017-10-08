#pragma once
#include <array>
#include <assert.h>
#include <memory>
using namespace std;


namespace cs
{


	template<typename Element>
	class Matrix
	{
	public:
        typedef shared_ptr<Element> ElementPtr;

        Matrix();
	    Matrix(int countRows, int countColumns);
	    Matrix(const Matrix& other);
        virtual ~Matrix();
        
    public:
        Matrix<Element> Transpose() const;

    //getters
	public:
		int GetCountRows() const;
		int GetCountColumns() const;

    //operators
	public:
		Element& operator()(int row, int column) const;
		Matrix<Element> operator*(const Matrix<Element>& other) const;
		Matrix<Element>& operator=(const Matrix<Element>& other);
		Matrix<Element>& operator*=(const Matrix<Element>& other);

    protected:
        inline int GetIndex(int row, int col) const;

	protected:
        ElementPtr _elements;    

    private:
        int _countRows;
        int _countColumns;
        int _totalCountElements;
	};


	template<typename Element>
	Matrix<Element>::Matrix():
        _countRows(0),
        _countColumns(0),
        _totalCountElements(0),
        _elements(NULL)
	{

	}


	template<typename Element>
	Matrix<Element>::Matrix(int countRows, int countColumns):
        _countRows(countRows),
        _countColumns(countColumns),
        _totalCountElements(countRows * countColumns),
        _elements(new Element[countRows * countColumns])
	{
        for (int i = 0; i < _totalCountElements; i++)
        {
            _elements.get()[i] = Element();
        }
	}


	template<typename Element>
	Matrix<Element>::Matrix(const Matrix& other): 
        _countRows(other._countRows),
        _countColumns(other._countColumns),
        _totalCountElements(other._countRows * other._countColumns),
        _elements(other._elements)
	{

	}


    template<typename Element>
    Matrix<Element>::~Matrix()
    {

    }


	template<typename Element>
	inline Matrix<Element> Matrix<Element>::Transpose() const
	{
		Matrix<Element> result(
            GetCountColumns(), 
            GetCountRows()
        );

        for (int i = 0; i < GetCountRows(); ++i)
        {
            for (int j = 0; j < GetCountColumns(); ++j)
            {
                result(j, i) = _elements.get()[GetIndex(i, j)];
            }
        }
		
		return result;
	}


	template<typename Element>
	int Matrix<Element>::GetCountRows() const
	{
		return _countRows;
	}


	template<typename Element>
	int Matrix<Element>::GetCountColumns() const
	{
        return _countColumns;
	}


	template<typename Element>
	Matrix<Element> Matrix<Element>::operator*(const Matrix<Element>& other) const
	{
        assert(GetCountColumns() == other.GetCountRows());

        Matrix<Element> result(_countRows, other._countColumns);

        Element* pElements = _elements.get();
        Element* pResElements = result._elements.get();
        Element* pOtherElements = other._elements.get();

        int i1 = 0;
        int i2 = 0;

        for (int i = 0; i < result._countRows; i++)
        {
            for (int j = 0; j < result._countColumns; j++)
            {
                int index = GetIndex(i, j);
                pResElements[index] = 0;

                i1 = i * _countColumns;
                i2 = j;

                for (int k = 0; k < _countColumns; k++)
                {
                    pResElements[index] += pElements[i1] * pOtherElements[i2];

                    i1++;
                    i2 += other._countColumns;
                }
            }
        }

        return result;
	}


	template<typename Element>
	Matrix<Element>& Matrix<Element>::operator=(const Matrix<Element>& other)
	{
        if (this == &other)
        {
            return *this;
        }

        _countRows = other._countRows;
        _countColumns = other._countColumns;
        _totalCountElements = other._totalCountElements;
        _elements = other._elements;

        return *this;
	}


	template<typename Element>
	Matrix<Element>& Matrix<Element>::operator*=(const Matrix<Element>& other)
	{
        assert(_countColumns == other._countRows);

        auto res = this * other;

        _countRows = res._countRows;
        _countColumns = res._countColumns;
        _totalCountElements = res._totalCountElements;
        _elements = res._elements;

        return *this;
	}


	template<typename Element>
	Element& Matrix<Element>::operator()(int row, int col) const
	{
        assert(row < _countRows);
        assert(col < _countColumns);

		return _elements.get()[GetIndex(row, col)];
	}


    template<typename Element>
    inline int Matrix<Element>::GetIndex(int row, int col) const
    {
        return row * _countColumns + col;
    }


	template<typename Element>
	class HomogeneousPoint
		: public Matrix<Element>
	{
	public:
		HomogeneousPoint();
		HomogeneousPoint(Element x, Element y, Element z);

	public:
		Element x() const;
		Element y() const;
		Element z() const;

	public:
		Element& operator[](int coordinateIndex);
        Matrix<Element> operator*(const Matrix<Element>& other) const;
	};


	template<typename Element>
	HomogeneousPoint<Element>::HomogeneousPoint()
		: Matrix<Element>(1, 4)
	{
		_elements.get()[3] = 1;
	}


	template<typename Element>
	HomogeneousPoint<Element>::HomogeneousPoint(Element x, Element y, Element z)
		: Matrix<Element>(1, 4)
	{
		_elements.get()[0] = x;
		_elements.get()[1] = y;
		_elements.get()[2] = z;
		_elements.get()[3] = 1;
	}


	template<typename Element>
	Element HomogeneousPoint<Element>::x() const
	{
		return _elements.get()[0];
	}


	template<typename Element>
	Element HomogeneousPoint<Element>::y() const
	{
		return _elements.get()[1];
	}


	template<typename Element>
	Element HomogeneousPoint<Element>::z() const
	{
		return _elements.get()[2];
	}


	template<typename Element>
	Element& HomogeneousPoint<Element>::operator[](int coordinateIndex)
	{
        assert(coordinateIndex < 3);

		return _elements.get()[coordinateIndex];
	}


    template<typename Element>
    Matrix<Element> HomogeneousPoint<Element>::operator*(const Matrix<Element>& other) const
    {
        return this->Matrix<Element>::operator*(other);
    }


}
