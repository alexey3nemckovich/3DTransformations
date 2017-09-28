#pragma once
#include <utility>


namespace cs
{


	enum Side { 
		LEFT, 
		RIGHT,
		TOP,
		BOTTOM 
	};


	enum class CoordinateAxisName {
		X = 0,
		Y = 1,
		Z = 2
	};


	struct LogicPoint
	{
	public:
		LogicPoint();
		LogicPoint(double x, double y);
		LogicPoint(double x, double y, double z);
		LogicPoint(const LogicPoint& other);

	public:
		LogicPoint& operator=(const LogicPoint& other);
		LogicPoint* operator+=(const LogicPoint& other);
		LogicPoint* operator-=(const LogicPoint& other);

	public:
		bool operator==(const LogicPoint& other);
		int operator*(LogicPoint other);

	public:
		double x, y, z;
	};


	typedef std::pair<LogicPoint, LogicPoint> AxisPoints;


	struct Axis
	{
	public:
		Axis() = default;
		Axis(LogicPoint vector, int x0, int y0, int z0)
			: vector(vector),
			x0(x0),
			y0(y0),
			z0(z0)
		{

		}

	public:
		LogicPoint vector;
		int x0, y0, z0;
	};


	struct Plane
	{
	public:
		Plane(
			const LogicPoint& a,
			const LogicPoint& b,
			const LogicPoint& c
		);

	public:
		LogicPoint FindIntersectionWithAxis(const Axis&);
		LogicPoint FindPointByXAndY(double x, double y);

	public:
		double A;
		double B;
		double C;
		double D;
	};


}
