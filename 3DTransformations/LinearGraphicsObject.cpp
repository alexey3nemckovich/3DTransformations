#include "stdafx.h"
#include "CoordinateSystem.h"
using namespace cs;


LinearGraphicsObject::LinearGraphicsObject(const vector<LogicPoint>& points, int penStyle/* = PS_SOLID*/, int penWidth/* = 1*/, COLORREF penColor/* = RGB(0, 0, 0)*/)
	: GraphicsObject(penStyle, penWidth, penColor),
	_points(points)
{

}


LinearGraphicsObject::LinearGraphicsObject(const LinearGraphicsObject& other)
{

}


void LinearGraphicsObject::Render(const CoordinateSystem* cs, CDC *dc)
{
	auto oldPoint = dc->MoveTo(
		cs->ConvertLogicPointToPhys(_points[0])
	);

	int countPoints = _points.size();
	for (int i = 1; i < countPoints; i++)
	{
		dc->LineTo(
			cs->ConvertLogicPointToPhys(_points[i])
		);
	}

	dc->MoveTo(oldPoint);
}
