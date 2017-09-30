#include "stdafx.h"
#include "Geometry.h"
#include "CoordinateSystem.h"


namespace cs
{


	Rasterization::Ptr RasterizeLineSegment(const CoordinateSystem* coordSystem, const Axis& axis, const LogicPoint& la, const LogicPoint& lb, COLORREF color/* = 0*/, int thickness/* = 1*/)
	{
		Rasterization* rasterization = new Rasterization();

		CPoint physOrigin = coordSystem->GetPhysOrigin();
		CPoint a = coordSystem->ConvertLogicPointToPhys(la);
		CPoint b = coordSystem->ConvertLogicPointToPhys(lb);

		double zValue = -1;

		int dx = abs(b.x - a.x);
		int dy = abs(b.y - a.y);
		int sx = b.x >= a.x ? 1 : -1;
		int sy = b.y >= a.y ? 1 : -1;
		if (dy <= dx) {
			int d = (dy << 1) - dx;
			int d1 = dy << 1;
			int d2 = (dy - dx) << 1;

			auto lp = axis.GetPointByProjection(a.x - physOrigin.x, a.y - physOrigin.y);
			zValue = coordSystem->GetPointDistanceToProjectionPlane(lp);
			rasterization->AddPoint(
				coordSystem->ConvertLogicPointToPhys(lp), zValue, color, true
			);
			//dc->SetPixel(a, color);

			for (int x = a.x + sx, y = a.y, i = 1; i <= dx; i++, x += sx) {
				if (d > 0) {
					d += d2; y += sy;
				}
				else
					d += d1;

				auto lp = axis.GetPointByProjection(x - physOrigin.x, y - physOrigin.y);
				zValue = coordSystem->GetPointDistanceToProjectionPlane(lp);
				rasterization->AddPoint(
					coordSystem->ConvertLogicPointToPhys(lp), zValue, color, true
				);
				//dc->SetPixel(x, y, color);

			}
		}
		else {
			int d = (dx << 1) - dy;
			int d1 = dx << 1;
			int d2 = (dx - dy) << 1;

			auto lp = axis.GetPointByProjection(a.x - physOrigin.x, a.y - physOrigin.y);
			zValue = coordSystem->GetPointDistanceToProjectionPlane(lp);
			rasterization->AddPoint(
				coordSystem->ConvertLogicPointToPhys(lp), zValue, color, true
			);
			//dc->SetPixel(a, color);

			for (int x = a.x, y = a.y + sy, i = 1; i <= dy; i++, y += sy) {
				if (d > 0) {
					d += d2; x += sx;
				}
				else
					d += d1;

				auto lp = axis.GetPointByProjection(x - physOrigin.x, y - physOrigin.y);
				zValue = coordSystem->GetPointDistanceToProjectionPlane(lp);
				rasterization->AddPoint(
					coordSystem->ConvertLogicPointToPhys(lp), zValue, color, true
				);
				//dc->SetPixel(x, y, color);

			}
		}

		return Rasterization::Ptr(rasterization);
	}


	HomogeneousPoint<double> FindNormalVectorToPlane(const LogicPoint& a, const LogicPoint& b, const LogicPoint& c, double module, bool rightHande/* = true*/)
	{
		double vx1 = b.x - a.x;
		double vy1 = b.y - a.y;
		double vz1 = b.z - a.z;
		double vx2 = c.x - a.x;
		double vy2 = c.y - a.y;
		double vz2 = c.z - a.z;

		HomogeneousPoint<double> vector = HomogeneousPoint<double>(
			vy1 * vz2 - vz1 * vy2,
			-(vx1 * vz2 - vz1 * vx2),
			vx1 * vy2 - vy1 * vx2
		);

		if (!rightHande)
		{
			vector[0] *= -1;
			vector[1] *= -1;
			vector[2] *= -1;
		}

		double curModule = FindVectorModule<double>(vector);
		vector[0] = vector[0] * 100 / module;
		vector[1] = vector[1] * 100 / module;
		vector[2] = vector[2] * 100 / module;

		return vector;
	}


	double FindDistance(const LogicPoint& a, const LogicPoint& b)
	{
		return sqrt(
			(b.x - a.x) * (b.x - a.x) +
			(b.y - a.y) * (b.y - a.y) +
			(b.z - a.z) * (b.z - a.z)
		);
	}


	double FindDistance(const CPoint& a, const CPoint& b)
	{
		return sqrt(
			(b.x - a.x) * (b.x - a.x) +
			(b.y - a.y) * (b.y - a.y)
		);
	}


	bool DoesPointBelongToPolygon(
		const vector<Axis>& sidesAxises,
		int borderThickness,
		int x,
		int y,
		bool& borderPoint
	)
	{
		borderPoint = false;
		double halfBorderThickness = (double)borderThickness / 2;
		auto cAxises = sidesAxises.size();
		for (int i = 0; i < cAxises; i++)
		{
			auto positionToAxis = sidesAxises[i].FindPointRelativePosition(x, y);

			if (borderThickness)
			{
				if (Axis::PointSideToAxis::LEFT == positionToAxis.sideToAxis &&
					positionToAxis.distance > halfBorderThickness
				)
				{
					return false;
				}
			}
			else
			{
				if (Axis::PointSideToAxis::LEFT == positionToAxis.sideToAxis)
				{
					return false;
				}
			}

			if (positionToAxis.distance <= halfBorderThickness)
			{
				borderPoint = true;
			}
		}

		return true;
	}


	/*PointPositionToAxis FindPointPositionToAxis(const CPoint& axisA, const CPoint& axisB, const CPoint& p)
	{
		return FindPointPositionToAxis(axisA, axisB, p.x, p.y);
	}


	PointPositionToAxis FindPointPositionToAxis(const CPoint& axisA, const CPoint& axisB, int x, int y)
	{
		double A = axisA.y - axisB.y;
		double B = axisB.x - axisA.x;
		double C = axisA.x * axisB.y - axisB.x * axisA.y;

		PointPositionToAxis pos;
		pos.distance = abs((A * x + B * y + C) / sqrt(A * A + B * B));
	
		if (pos.distance)
		{
			if ((axisB.x - axisA.x)*(y - axisA.y) - (axisB.y - axisA.y) * (x - axisA.x) > 0)
			{
				pos.sideToAxis = PointSideToAxis::LEFT;
			}
			else
			{
				pos.sideToAxis = PointSideToAxis::RIGHT;
			}
		}
		else
		{
			pos.sideToAxis = PointSideToAxis::ON_AXIS;
		}

		return pos;
	}*/


}
