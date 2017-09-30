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

		_A = normVector.x();
		_B = normVector.y();
		_C = normVector.z();

		_D = - (
			normVector.x() * a.x + 
			normVector.y() * a.y + 
			normVector.z() * a.z
		);
	}


	LogicPoint Plane::FindIntersectionWithAxis(const Axis& axis)
	{

		return LogicPoint();
	}


	bool Plane::IsParallelToOx() const
	{
		return 0 == _A;
	}


	bool Plane::IsParallelToOy() const
	{
		return 0 == _B;
	}


	bool Plane::IsParallelToOz() const
	{
		return 0 == _C;
	}


	double Plane::A() const
	{
		return _A;
	}


	double Plane::B() const
	{
		return _B;
	}


	double Plane::C() const
	{
		return _C;
	}


	double Plane::D() const
	{
		return _D;
	}


}
