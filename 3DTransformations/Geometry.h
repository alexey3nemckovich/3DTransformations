#pragma once
#include "CoordinateSystem.h"


namespace cs
{

	HomogeneousPoint<double> FindNormalVectorToPlane(const LogicPoint& a, const LogicPoint& b, const LogicPoint& c, bool rightHande = true)
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

		return vector;
		/*LogicPoint& A = _points[0];
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
		_nVector.z = vx1 * vy2 - vy1 * vx2;*/
	}


	template<typename Element>
	double FindAnglebetweenVectors(HomogeneousPoint<Element> a, HomogeneousPoint<Element> b)
	{
		double scalarMul = a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
		double aModule = FindVectorModule(a);
		double bModule = FindVectorModule(b);

		double cosVal = scalarMul / (aModule * bModule);
		return acos(cosVal);
	}


	template<typename Element>
	double FindVectorModule(HomogeneousPoint<Element> a)
	{
		return sqrt(a.x() * a.x() + a.y() * a.y() + a.z() * a.z());
	}

}
