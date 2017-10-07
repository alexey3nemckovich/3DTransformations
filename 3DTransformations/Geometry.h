#pragma once
#include "PrimitiveTypes.h"
#include "RasterizationPrimitiveTypes.h"
#include "PrimitiveTypesTemplates.h"

#define _USE_MATH_DEFINES
#include <math.h>


namespace cs
{


	class CoordinateSystem;


	Rasterization::Ptr RasterizeLineSegment(const CoordinateSystem* coordSystem, const Axis& axis, const LogicPoint& a, const LogicPoint& b, COLORREF color = 0, int thickness = 1);


	HomogeneousPoint<double> FindNormalVectorToPlane(const LogicPoint& a, const LogicPoint& b, const LogicPoint& c, double module, bool rightHande = true);


	LogicPoint GetNearestToWatcherIntersectionPointWithBorder(const vector<LogicPoint>&, const LogicPoint&);


	double FindDistance(const CPoint& a, const CPoint& b);


	double FindDistance(const LogicPoint&, const LogicPoint&);


	bool DoesPointBelongToPolygon(
		const vector<Axis>& sidesAxises,
		int borderThickness,
		int x,
		int y,
		bool& borderPoint
	);


	template<typename Element>
	double FindVectorModule(const HomogeneousPoint<Element>& a)
	{
		return sqrt(a.x() * a.x() + a.y() * a.y() + a.z() * a.z());
	}


	template<typename Element>
	double FindAnglebetweenVectors(const HomogeneousPoint<Element>& a,const HomogeneousPoint<Element>& b)
	{
		double scalarMul = a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
		double aModule = FindVectorModule(a);
		double bModule = FindVectorModule(b);

		double cosVal = scalarMul / (aModule * bModule);
		return acos(cosVal);
	}


}
