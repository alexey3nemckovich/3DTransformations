#include "stdafx.h"
#include "PrimitiveTypes.h"


namespace cs
{


	Axis::Axis(LogicPoint a, LogicPoint b)
		: _directionVector(
			b.x - a.x,
			b.y - a.y,
			b.z - a.z
		),
		_x0(a.x),
		_y0(a.y),
		_z0(a.z),
		_A(a.y - b.y),
		_B(b.x - a.x),
		_C(a.x * b.y - b.x * a.y)
	{

	}


	Axis::Axis(LogicPoint directionVector, int x0, int y0, int z0/* = 0*/)
		: _directionVector(directionVector),
		_x0(x0),
		_y0(y0),
		_z0(z0),
		_A(directionVector.y - y0),
		_B(x0 - directionVector.x),
		_C(directionVector.x * y0 - x0 * directionVector.y)
	{

	}


	Axis::PointPositionToAxis Axis::FindPointRelativePosition(int x, int y) const
	{
		PointPositionToAxis pos;
		pos.distance = abs((_A * x + _B * y + _C) / sqrt(_A * _A + _B * _B));

		if (pos.distance)
		{
			if ((_B * (y - _y0) + _A * (x - _x0)) > 0)
			{
				pos.sideToAxis = PointSideToAxis::RIGHT;
			}
			else
			{
				pos.sideToAxis = PointSideToAxis::LEFT;
			}
		}
		else
		{
			pos.sideToAxis = PointSideToAxis::ON_AXIS;
		}

		return pos;
	}


	Axis::PointPositionToAxis Axis::FindPointRelativePosition(const CPoint& p) const
	{
		return FindPointRelativePosition(p.x, p.y);
	}


	LogicPoint Axis::GetPointByProjection(int x, int y) const
	{
		double z = 0;

		if (_directionVector.x)
		{
			z = _z0 + (_directionVector.z * (x - _x0)) / _directionVector.x;
		}
		else
		{
			z = _z0 + (_directionVector.z * (y - _x0)) / _directionVector.y;
		}

		return LogicPoint(x, y, z);
	}


	LogicPoint Axis::GetPointByProjection(const CPoint& p) const
	{
		return GetPointByProjection(p.x, p.y);
	}


	bool Axis::FindIntersectionWith(const Axis& axis, LogicPoint& point)
	{
		return false;
	}


}
