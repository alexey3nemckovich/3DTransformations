#pragma once
#include <utility>


namespace cs
{


	enum class Axis {
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


}
