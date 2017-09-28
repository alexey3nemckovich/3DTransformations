#include "stdafx.h"
#include "Geometry.h"


namespace cs
{


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


	bool DoesPointBelongToPolygon(
		const vector<CPoint>& cornersPoints,
		int borderThickness,
		int x,
		int y,
		bool& borderPoint
	)
	{
		borderPoint = false;
		double haldBorderThickness = (double)borderThickness / 2;
		auto cCornerPoints = cornersPoints.size();
		for (int i = 0; i < cCornerPoints; i++)
		{
			if (cornersPoints[i] == cornersPoints[(i + 1) % cCornerPoints])
			{
				continue;
			}

			auto positionToAxis = FindPointPositionToAxis(cornersPoints[i], cornersPoints[(i + 1) % cCornerPoints], x, y);

			if (borderThickness)
			{
				if (PointSideToAxis::LEFT == positionToAxis.sideToAxis &&
					positionToAxis.distance > haldBorderThickness
				)
				{
					return false;
				}
			}
			else
			{
				if (PointSideToAxis::LEFT == positionToAxis.sideToAxis)
				{
					return false;
				}
			}

			if (positionToAxis.distance <= haldBorderThickness)
			{
				borderPoint = true;
				return true;
			}
		}

		return true;
	}


	PointPositionToAxis FindPointPositionToAxis(const CPoint& axisA, const CPoint& axisB, const CPoint& p)
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
	}


}
