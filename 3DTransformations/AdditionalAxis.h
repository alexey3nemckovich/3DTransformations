#pragma once
#include "CoordinateSystem.h"
using namespace cs;


class AdditionalAxis
{
public:
	static AdditionalAxis* GetInstance();

public:
	AdditionalAxis();

public:
	AxisPoints& GetPoints();
	void SetPoints(const LogicPoint& p1, const LogicPoint& p2);

private:
	AxisPoints _points;
};
