#include "stdafx.h"
#include "AdditionalAxis.h"


AdditionalAxis* AdditionalAxis::GetInstance()
{
	static AdditionalAxis instance;
	return &instance;
}


AdditionalAxis::AdditionalAxis()
{
	
}


AxisPoints& AdditionalAxis::GetPoints()
{
	return _points;
}


void AdditionalAxis::SetPoints(const LogicPoint& p1, const LogicPoint& p2)
{
	_points.first = p1;
	_points.second = p2;
}
