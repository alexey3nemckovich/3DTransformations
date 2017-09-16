#include "stdafx.h"
#include "CoordinateSystem.h"
using namespace cs;


LinearGraphicsObject::LinearGraphicsObject(const vector<LogicPoint>& points, int penStyle/* = PS_SOLID*/, int penWidth/* = 1*/, COLORREF penColor/* = RGB(0, 0, 0)*/)
	: GraphicsObject(penStyle, penWidth, penColor)
{

}


LinearGraphicsObject::LinearGraphicsObject(const LinearGraphicsObject& other)
{

}


void LinearGraphicsObject::Render(const CoordinateSystem*, CPaintDC *dc)
{

}
