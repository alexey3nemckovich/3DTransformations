#pragma once
#include "PrimitiveTypes.h"
#include "PrimitiveTypesTemplates.h"

#define _USE_MATH_DEFINES
#include <math.h>


namespace cs
{


	enum class PointSideToAxis
	{
		LEFT,
		RIGHT,
		ON_AXIS,
	};


	struct PointPositionToAxis
	{
		PointSideToAxis sideToAxis;
		double distance;
	};

	
	PointPositionToAxis FindPointPositionToAxis(const CPoint& axisA, const CPoint& axisB, int x, int y);


	PointPositionToAxis FindPointPositionToAxis(const CPoint& axisA, const CPoint& axisB, const CPoint& p);


	HomogeneousPoint<double> FindNormalVectorToPlane(const LogicPoint& a, const LogicPoint& b, const LogicPoint& c, double module, bool rightHande = true);


	LogicPoint GetNearestToWatcherIntersectionPointWithBorder(const vector<LogicPoint>&, const LogicPoint&);


	bool DoesPointBelongToPolygon(
		const vector<CPoint>& cornersPoints,
		int borderThickness,
		int x,
		int y,
		bool& borderPoint
	);


	template<typename Element>
	double FindVectorModule(HomogeneousPoint<Element> a)
	{
		return sqrt(a.x() * a.x() + a.y() * a.y() + a.z() * a.z());
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


}
