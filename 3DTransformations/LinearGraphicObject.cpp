#include "stdafx.h"
#include "LinearGraphicObject.h"
#include "CoordinateSystem.h"
using namespace cs;


LinearGraphicObject::LinearGraphicObject(const vector<LogicPoint>& points, int penStyle/* = PS_SOLID*/, int penWidth/* = 1*/, COLORREF penColor/* = RGB(0, 0, 0)*/)
	: RasterizableGraphicObject(points, penStyle, penWidth, penColor)
{

}


LinearGraphicObject::LinearGraphicObject(const LinearGraphicObject& other)
	: RasterizableGraphicObject(other)
{

}


void LinearGraphicObject::Render(const CoordinateSystem* cs, CDC *dc) const
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
