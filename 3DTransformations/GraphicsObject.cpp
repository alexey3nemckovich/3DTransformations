#include "stdafx.h"
#include "CoordinateSystem.h"
using namespace cs;


GraphicsObject::GraphicsObject(int penStyle/* = PS_SOLID*/, int penWidth/* = 1*/, COLORREF penColor/* = RGB(0, 0, 0)*/)
	: _penStyle(penStyle), _penWidth(penWidth), _penColor(penColor)
{

}


GraphicsObject::GraphicsObject(const GraphicsObject& other)
{
	_penStyle = other._penStyle;
	_penWidth = other._penWidth;
	_penColor = other._penColor;
}


GraphicsObject::~GraphicsObject()
{

}


GraphicsObject* GraphicsObject::operator=(const GraphicsObject& other)
{
	_penStyle = other._penStyle;
	_penWidth = other._penWidth;
	_penColor = other._penColor;
	return this;
}
