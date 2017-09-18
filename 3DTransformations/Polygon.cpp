#include "stdafx.h"
#include "CoordinateSystem.h"
using namespace cs;


Polygon::Polygon(const vector<LogicPoint>& points, int penStyle/* = PS_SOLID*/, int penWidth/* = 1*/, COLORREF penColor/* = RGB(0, 0, 0)*/)
	: LinearGraphicsObject(points, penStyle, penWidth, penColor)
{
	Init();
}


Polygon::Polygon(const Polygon& other)
	: LinearGraphicsObject(other)
{
	Init();
}


void Polygon::Init()
{
	LogicPoint& A = _points[0];
	LogicPoint& B = _points[1];
	LogicPoint& C = _points[2];
	double vx1 = A.x - B.x;
	double vy1 = A.y - B.y;
	double vz1 = A.z - B.z;
	double vx2 = C.x - B.x;
	double vy2 = C.y - B.y;
	double vz2 = C.z - B.z;

	_nVector.x = vy1 * vz2 - vz1 * vy2;
	_nVector.y = vz1 * vx2 - vx1 * vz2;
	_nVector.z = vx1 * vy2 - vy1 * vx2;
}


void Polygon::Render(const CoordinateSystem* cs, CPaintDC *dc)
{
	bool isVisible = true;
	/*CPoint physOrigin = cs->GetOriginPhysPoint();
	CPoint nVectorPhysPoint = cs->ConvertLogicPointToPhys(_nVector);
	if (nVectorPhysPoint.y > physOrigin.y)
	{
		isVisible = false;
	}*/

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
	//LinearGraphicsObject::Render(cs, dc);

	/*auto countPoints = _points.size();
	POINT* physPoints = new POINT[countPoints];
	for (int i = 0; i < countPoints; i++)
	{
		physPoints[i] = cs->ConvertLogicPointToPhys(_points[i]);
	}

	dc->Polygon(physPoints, countPoints);*/
	/*auto oldPoint = dc->MoveTo(
		cs->ConvertLogicPointToPhys(_points[_points.size() - 1])
	);

	dc->LineTo(
		cs->ConvertLogicPointToPhys(_points[0])
	);

	dc->MoveTo(oldPoint);*/
}
