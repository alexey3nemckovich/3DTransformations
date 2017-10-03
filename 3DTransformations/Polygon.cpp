#include "stdafx.h"
#include "Geometry.h"
//
#include "Polygon.h"
#include "CoordinateSystem.h"
using namespace cs;


Polygon::Polygon(const vector<LogicPoint>& points, bool rightHandNormalVector/* = true*/, int penStyle/* = PS_SOLID*/, int penWidth/* = 1*/, COLORREF penColor/* = RGB(0, 0, 0)*/, COLORREF brushColor/* = RGB(0, 0, 0)*/)
	: RasterizableGraphicObject(points, penStyle, penWidth, penColor, brushColor),
	_rightHandNormalVector(rightHandNormalVector)
{
	Init();
}


Polygon::Polygon(const Polygon& other)
	: RasterizableGraphicObject(other),
	_rightHandNormalVector(other._rightHandNormalVector)
{
	Init();
}


void Polygon::Init()
{
	_brush.CreateSolidBrush(_brushColor);
	_nVector = FindNormalVectorToPlane(_points[0], _points[1], _points[2], 100, _rightHandNormalVector);
}


void Polygon::Render(const CoordinateSystem* cs, CDC *dc) const
{
	bool isVisible = true;
	auto watcherVector = cs->GetWatcherVector();
	double angleToWatcher = FindAnglebetweenVectors(watcherVector, _nVector);
	if (angleToWatcher > M_PI_2)
	{
		isVisible = false;
	}

	if (isVisible)
	{
		int countPoints = _points.size();

		//Get phys points
		CPoint* physPoints = new CPoint[_points.size()];
		for (int i = 0; i < countPoints; i++)
		{
			physPoints[i] = cs->ConvertLogicPointToPhys(_points[i]);
		}

		//Render carcas
		auto oldPoint = dc->MoveTo(
			physPoints[0]
		);

		for (int i = 1; i < countPoints; i++)
		{
			dc->LineTo(
				physPoints[i]
			);
		}
		dc->LineTo(
			physPoints[0]
		);

		dc->MoveTo(oldPoint);

		////Fill carcas with brush
		dc->SelectObject(const_cast<CBrush*>(&_brush));
		dc->Polygon(physPoints, countPoints);
	}
}
