#include "stdafx.h"
#include "CoordinateSystem.h"
using namespace cs;


LogicPoint::LogicPoint()
{
	x = y = z = 0;
}


LogicPoint::LogicPoint(double x, double y)
{
	this->x = x;
	this->y = y;
	this->z = 0;
}


LogicPoint::LogicPoint(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}


bool LogicPoint::operator==(const LogicPoint& other)
{
	if (
		this->x == other.x &&
		this->y == other.y &&
		this->z == other.z
		)
	{
		return true;
	}

	return false;
}


LogicPoint* LogicPoint::operator+=(const LogicPoint& other)
{
	this->x += other.x;
	this->y += other.y;
	this->z += other.z;
	return this;
}


LogicPoint* LogicPoint::operator-=(const LogicPoint& other)
{
	this->x -= other.x;
	this->y -= other.y;
	this->z -= other.z;
	return this;
}


int LogicPoint::operator*(LogicPoint other)
{
	return this->x * other.x + this->y * other.y + this->z * other.z;
}
