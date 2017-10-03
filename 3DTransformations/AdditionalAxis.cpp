#include "stdafx.h"
#include "AdditionalAxis.h"
#include "CoordinateSystem.h"


AdditionalAxis* AdditionalAxis::GetInstance()
{
	static AdditionalAxis instance;
	return &instance;
}


AdditionalAxis::AdditionalAxis()
	: LinearGraphicObject({})
{
	//cs::CoordinateSystem::GetInstance()->AddGraphicObject(this, false);
}


void AdditionalAxis::SetPoints(const LogicPoint& p1, const LogicPoint& p2)
{
	_points.clear();

	_points.push_back(p1);
	_points.push_back(p2);
}
