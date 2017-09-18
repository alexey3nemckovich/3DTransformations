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


void Polygon::Render(const CoordinateSystem* cs, CPaintDC *dc)
{
	LinearGraphicsObject::Render(cs, dc);

	auto oldPoint = dc->MoveTo(
		cs->ConvertLogicPointToPhys(_points[_points.size() - 1])
	);

	dc->LineTo(
		cs->ConvertLogicPointToPhys(_points[0])
	);

	dc->MoveTo(oldPoint);
}
