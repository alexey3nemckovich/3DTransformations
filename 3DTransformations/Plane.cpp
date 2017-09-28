#include "stdafx.h"
#include "PrimitiveTypes.h"
#include "Geometry.h"


namespace cs
{


	Plane::Plane(
		const LogicPoint& a,
		const LogicPoint& b,
		const LogicPoint& c
	)
	{
		HomogeneousPoint<double> normVector = FindNormalVectorToPlane(a, b, c, 100);

		A = normVector.x();
		B = normVector.y();
		C = normVector.z();

		D = - (
			normVector.x() * a.x + 
			normVector.y() * a.y + 
			normVector.z() * a.z
		);
	}


	LogicPoint Plane::FindIntersectionWithAxis(const Axis& axis)
	{

		return LogicPoint();
	}


	LogicPoint Plane::FindPointByXAndY(double x, double y)
	{
		double z = 0;

		if (C)
		{
			z = (-A*x -B*y -D) / C;
		}
		else
		{
			throw exception("Kick yourself");
		}

		return LogicPoint(x, y, z);
	}


}
