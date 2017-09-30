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


	class Axis
	{
	public:
		enum class PointSideToAxis
		{
			LEFT,
			RIGHT,
			ON_AXIS,
		};

		typedef struct 
		{
			PointSideToAxis sideToAxis;
			double distance;
		}PointPositionToAxis;

	public:
		Axis() = default;
		Axis(LogicPoint a, LogicPoint b);
		Axis(LogicPoint directionVector, int x0, int y0, int z0 = 0);

	public:
		LogicPoint GetPointByProjection(int x, int y) const;
		LogicPoint GetPointByProjection(const CPoint&) const;
		bool FindIntersectionWith(const Axis&, LogicPoint&);
		PointPositionToAxis FindPointRelativePosition(int x, int y) const;
		PointPositionToAxis FindPointRelativePosition(const CPoint& p) const;

	private:
		int _x0, _y0, _z0;
		LogicPoint _directionVector;

	private:
		double _A, _B, _C;
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
		bool IsParallelToOx() const;
		bool IsParallelToOy() const;
		bool IsParallelToOz() const;

	public:
		double A() const;
		double B() const;
		double C() const;
		double D() const;

	private:
		double _A;
		double _B;
		double _C;
		double _D;
	};


}
