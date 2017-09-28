#include "stdafx.h"
#include "GraphicObject.h"
#include "CoordinateSystem.h"
#include "RasterizableGraphicObject.h"
using namespace cs;


GraphicObject::GraphicObject(int penStyle/* = PS_SOLID*/, int penWidth/* = 1*/, COLORREF penColor/* = RGB(0, 0, 0)*/):
	_penStyle(penStyle),
	_penWidth(penWidth), 
	_penColor(penColor)
{

}


GraphicObject::GraphicObject(const GraphicObject& other):
	_penStyle(other._penStyle),
	_penWidth(other._penWidth),
	_penColor(other._penColor)
{

}


GraphicObject::~GraphicObject()
{

}


GraphicObject* GraphicObject::operator=(const GraphicObject& other)
{
	_penStyle = other._penStyle;
	_penWidth = other._penWidth;
	_penColor = other._penColor;
	return this;
}
