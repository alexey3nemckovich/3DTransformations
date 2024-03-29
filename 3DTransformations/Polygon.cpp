﻿#include "stdafx.h"
#include "CoordinateSystem.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Geometry.h"
using namespace cs;


Polygon::Polygon(const vector<LogicPoint>& points, bool rightHandNormalVector/* = true*/, int penStyle/* = PS_SOLID*/, int penWidth/* = 1*/, COLORREF penColor/* = RGB(0, 0, 0)*/)
	: LinearGraphicsObject(points, penStyle, penWidth, penColor)
{
	_rightHandNormalVector = rightHandNormalVector;
	Init();
}


Polygon::Polygon(const Polygon& other)
	: LinearGraphicsObject(other)
{
	_rightHandNormalVector = other._rightHandNormalVector;
	Init();
}


void Polygon::Init()
{
	_brush.CreateSolidBrush(RGB(0, 255, 0));
	_nVector = FindNormalVectorToPlane(_points[0], _points[1], _points[2], 100, _rightHandNormalVector);
}


void Polygon::Render(const CoordinateSystem* cs, CDC *dc)
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
		//dc->SelectObject(&_brush);
		//dc->Polygon(physPoints, countPoints);
	}
}
