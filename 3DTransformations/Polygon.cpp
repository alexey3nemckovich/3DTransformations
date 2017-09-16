#include "stdafx.h"
#include "CoordinateSystem.h"
using namespace cs;


Polygon::Polygon(const vector<LogicPoint>& points, int penStyle/* = PS_SOLID*/, int penWidth/* = 1*/, COLORREF penColor/* = RGB(0, 0, 0)*/)
	: LinearGraphicsObject(points, penStyle, penWidth, penColor)
{

}


Polygon::Polygon(const Polygon& other)
	: LinearGraphicsObject(other)
{

}


void Polygon::Render(const CoordinateSystem*, CPaintDC *dc)
{

}
