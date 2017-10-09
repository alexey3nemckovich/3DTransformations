#pragma once

#include <vector>
#include <assert.h>
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
    Matrix(Matrix&& m);
    virtual ~Matrix() {};
    Matrix<MatrixElement> Transpose() const;


  public:
    int GetCountRows() const;
    int GetCountColumns() const;

  public:
    MatrixElement& operator()(int row, int column);
    Matrix<MatrixElement> operator*(const Matrix<MatrixElement>& other) const;
    Matrix<MatrixElement>& operator=(const Matrix<MatrixElement>& other);
    Matrix<MatrixElement>& operator=(Matrix<MatrixElement>&& other);
    Matrix<MatrixElement>& operator*=(const Matrix<MatrixElement>& other);

  protected:
    vector<vector<MatrixElement>> elements_;
  };


  template<typename MatrixElement>
  Matrix<MatrixElement>::Matrix()
  {
  }

  template<typename MatrixElement>
  Matrix<MatrixElement>::Matrix(int countRows, int countColumns)
    : elements_(vector< vector<MatrixElement> >(countRows, vector<MatrixElement>(countColumns)))
  {
  }

  template<typename MatrixElement>
  Matrix<MatrixElement>::Matrix(const Matrix& other) : elements_(other.elements_)
  {
  }

  template<typename MatrixElement>
  Matrix<MatrixElement>::Matrix(Matrix&& m) : elements_(std::move(m.elements_))
  {
  }

  template<typename MatrixElement>
  inline Matrix<MatrixElement> Matrix<MatrixElement>::Transpose() const
  {
    Matrix<MatrixElement> result(GetCountColumns(), GetCountRows());
    for (int i = 0; i < GetCountRows(); ++i)
      for (int j = 0; j < GetCountColumns(); ++j)
        result(j,i) = elements_[i][j];
    return result;
  }

  template<typename MatrixElement>
  int Matrix<MatrixElement>::GetCountRows() const
  {
    return elements_.size();
  }

  template<typename MatrixElement>
  int Matrix<MatrixElement>::GetCountColumns() const
  {
    return elements_.size() > 0 ? elements_[0].size() : 0;
  }

  template<typename MatrixElement>
  inline MatrixElement& Matrix<MatrixElement>::operator()(int row, int column)
  {
    assert(row < GetCountRows());
    assert(column < GetCountColumns());
    return elements_[row][column];
  }

  template<typename MatrixElement>
  Matrix<MatrixElement> Matrix<MatrixElement>::operator*(const Matrix<MatrixElement>& other) const
  {
    assert(GetCountColumns() == other.GetCountRows());
    Matrix<MatrixElement> otherTranspose(other.Transpose());

    int countElements = GetCountColumns();
    int countRows = GetCountRows();
    int countColumns = other.GetCountColumns();
    Matrix<MatrixElement> result(countRows, countColumns);

    for (int i = 0; i < GetCountRows(); ++i)
      for (int j = 0; j < GetCountColumns(); ++j)
      {
        result(i,j) = 0;
        for (int k = 0; k < countElements; ++k)
          result(i,j) += elements_[i][k] * otherTranspose(j,k);
      }
    return result;
  }

  template<typename MatrixElement>
  Matrix<MatrixElement>& Matrix<MatrixElement>::operator=(const Matrix<MatrixElement>& other)
  {
    if (this == &other)
      return *this;

    elements_ = other.elements_;
    return *this;
  }

  template<typename MatrixElement>
  Matrix<MatrixElement>& Matrix<MatrixElement>::operator=(Matrix<MatrixElement>&& other)
  {
    elements_ = std::move(other.elements_);
    return *this;
  }

  template<typename MatrixElement>
  Matrix<MatrixElement>& Matrix<MatrixElement>::operator*=(const Matrix<MatrixElement>& other)
  {
    elements_ = this * other;
    return *this;
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
    elements_[0][3] = 1;
  }


  template<typename Element>
  HomogeneousPoint<Element>::HomogeneousPoint(Element x, Element y, Element z)
    : Matrix<Element>(1, 4)
  {
    elements_[0][0] = x;
    elements_[0][1] = y;
    elements_[0][2] = z;
    elements_[0][3] = 1;
  }


  template<typename Element>
  Element HomogeneousPoint<Element>::x() const
  {
    return elements_[0][0];
  }


  template<typename Element>
  Element HomogeneousPoint<Element>::y() const
  {
    return elements_[0][1];
  }


  template<typename Element>
  Element HomogeneousPoint<Element>::z() const
  {
    return elements_[0][2];
  }


  template<typename Element>
  Element& HomogeneousPoint<Element>::operator[](int coordinateIndex)
  {
    return elements_[0][coordinateIndex];
  }


  template<typename Element>
  Matrix<Element> HomogeneousPoint<Element>::operator*(const Matrix<Element>& other) const
  {
    return this->Matrix<Element>::operator*(other);// (Matrix<Element>)this * other;
  }


  template<typename Element>
  Matrix<Element> operator*(const Matrix<Element>& point, const Matrix<Element>& other)
  {
    return point * other;
  }
}