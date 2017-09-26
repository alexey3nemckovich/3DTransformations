#include "stdafx.h"
#include "GraphicObject.h"
#include "CoordinateSystem.h"
using namespace cs;


GraphicObject::GraphicObject(int penStyle/* = PS_SOLID*/, int penWidth/* = 1*/, COLORREF penColor/* = RGB(0, 0, 0)*/)
	: _penStyle(penStyle),
	_penWidth(penWidth), 
	_penColor(penColor),
	_rasterized(false)
{

}


GraphicObject::GraphicObject(const GraphicObject& other)
	: _penStyle(other._penStyle),
	_penWidth(other._penWidth),
	_penColor(other._penColor),
	_rasterized(false)
{

}


GraphicObject::~GraphicObject()
{

}


const vector<GraphicObject::RasterizationUnit>& GraphicObject::GetRasterization() const
{
	if (!_rasterized)
	{
		Rasterize();
		_rasterized = true;
	}

	return _rasterizaion;
}


GraphicObject* GraphicObject::operator=(const GraphicObject& other)
{
	_penStyle = other._penStyle;
	_penWidth = other._penWidth;
	_penColor = other._penColor;
	return this;
}
