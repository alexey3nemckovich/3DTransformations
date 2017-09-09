#include "stdafx.h"
#include "CoordinateSystem.h"
using namespace cs;


GraphicsObject::GraphicsObject(vector<LogicPoint> &points, int penStyle/* = PS_SOLID*/, int penWidth/* = 1*/, COLORREF penColor/* = RGB(0, 0, 0)*/)
	: _penStyle(penStyle), _penWidth(penWidth), _penColor(penColor), _points(std::move(points))
{

}


GraphicsObject::GraphicsObject(const GraphicsObject& other)
{
	_penStyle = other._penStyle;
	_penWidth = other._penWidth;
	_penColor = other._penColor;
	_points = other._points;
}


GraphicsObject::GraphicsObject(const GraphicsObject&& other)
{
	_penStyle = other._penStyle;
	_penWidth = other._penWidth;
	_penColor = other._penColor;
	_points = std::move(other._points);
}


GraphicsObject::~GraphicsObject()
{

}


GraphicsObject* GraphicsObject::operator=(const GraphicsObject& other)
{
	_penStyle = other._penStyle;
	_penWidth = other._penWidth;
	_penColor = other._penColor;
	_points = other._points;
	return this;
}


LogicPoint& GraphicsObject::operator[](int index)
{
	return _points[index];
}
