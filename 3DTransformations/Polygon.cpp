#include "stdafx.h"
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
	_nVector = FindNormalVectorToPlane(_points[0], _points[1], _points[2], _rightHandNormalVector);
	double module = FindVectorModule(_nVector);
	_nVector[0] = _nVector[0] * 100 / module;
	_nVector[1] = _nVector[1] * 100 / module;
	_nVector[2] = _nVector[2] * 100 / module;

	CoordinateSystem::GetInstance()->AddLogicPoint(
		LogicPoint(
			_nVector.x(),
			_nVector.y(),
			_nVector.z()
		),
		RGB(0, 0, 255),
		false,
		false,
		false
	);
}


void Polygon::Render(const CoordinateSystem* cs, CPaintDC *dc)
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
		dc->LineTo(
			cs->ConvertLogicPointToPhys(_points[0])
		);

		dc->MoveTo(oldPoint);
	}
}
