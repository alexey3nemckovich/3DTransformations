﻿#include "stdafx.h"
#include "Geometry.h"
#include "CoordinateSystem.h"


namespace cs
{


	void ProcessPoint(const CoordinateSystem* coordSystem, Rasterization*, const Axis& axis, int x, int y, COLORREF color/* = 0*/);
	Rasterization::Ptr RasterizeLineSegment(const CoordinateSystem* coordSystem, const Axis& axis, const LogicPoint& la, const LogicPoint& lb, COLORREF color/* = 0*/, int thickness/* = 1*/)
	{
		Rasterization* rasterization = new Rasterization();

		CPoint physOrigin = coordSystem->GetPhysOrigin();
		CPoint a = coordSystem->ConvertLogicPointToPhys(la);
		CPoint b = coordSystem->ConvertLogicPointToPhys(lb);

		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = b.x - a.x;
		dy = b.y - a.y;
		dx1 = fabs(dx);
		dy1 = fabs(dy);
		px = 2 * dy1 - dx1;
		py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = a.x;
				y = a.y;
				xe = b.x;
			}
			else
			{
				x = b.x;
				y = b.y;
				xe = a.x;
			}

			ProcessPoint(coordSystem, rasterization, axis, x, y, color);

			for (i = 0; x<xe; i++)
			{
				x = x + 1;
				if (px<0)
				{
					px = px + 2 * dy1;
				}
				else
				{
					if ((dx<0 && dy<0) || (dx>0 && dy>0))
					{
						y = y + 1;
					}
					else
					{
						y = y - 1;
					}
					px = px + 2 * (dy1 - dx1);
				}

				ProcessPoint(coordSystem, rasterization, axis, x, y, color);

			}
		}
		else
		{
			if (dy >= 0)
			{
				x = a.x;
				y = a.y;
				ye = b.y;
			}
			else
			{
				x = b.x;
				y = b.y;
				ye = a.y;
			}

			ProcessPoint(coordSystem, rasterization, axis, x, y, color);

			for (i = 0; y<ye; i++)
			{
				y = y + 1;
				if (py <= 0)
				{
					py = py + 2 * dx1;
				}
				else
				{
					if ((dx<0 && dy<0) || (dx>0 && dy>0))
					{
						x = x + 1;
					}
					else
					{
						x = x - 1;
					}
					py = py + 2 * (dx1 - dy1);
				}

				ProcessPoint(coordSystem, rasterization, axis, x, y, color);
			}
		}

		return Rasterization::Ptr(rasterization);
	}


	void ProcessPoint(const CoordinateSystem* coordSystem, Rasterization* raster, const Axis& axis, int x, int y, COLORREF color/* = 0*/)
	{
		auto physOrigin = coordSystem->GetPhysOrigin();

		CPoint point(x, y);
		CPoint lp(
			point.x - physOrigin.x,
			point.y - physOrigin.y
		);
		auto p = axis.GetPointByProjection(lp);
		double z = coordSystem->GetPointDistanceToProjectionPlane(p);
		raster->AddPoint(point, z, color, true);
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
		const auto cAxises = sidesAxises.size();
		const double halfBorderThickness = (double)borderThickness / 2;

		int countAxisesForWitchPointIsOnLeftSide = 0;
		int countAxisesForWitchPointIsOnRightSide = 0;

		for (int i = 0; i < cAxises; i++)
		{
			auto positionToAxis = sidesAxises[i].FindPointRelativePosition(x, y);

			switch (positionToAxis.sideToAxis)
			{
			case Axis::PointSideToAxis::LEFT:
				countAxisesForWitchPointIsOnLeftSide++;
				break;
			case Axis::PointSideToAxis::RIGHT:
				countAxisesForWitchPointIsOnRightSide++;
				break;
			case Axis::PointSideToAxis::ON_AXIS:
				countAxisesForWitchPointIsOnLeftSide++;
				countAxisesForWitchPointIsOnRightSide++;
				break;
			default:
				break;
			}

			if (positionToAxis.distance <= borderThickness)
			{
				borderPoint = true;
			}
		}

		if (cAxises == countAxisesForWitchPointIsOnLeftSide ||
			cAxises == countAxisesForWitchPointIsOnRightSide)
		{
			return true;
		}
		else
		{
			return false;
		}
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
