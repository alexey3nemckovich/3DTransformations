#pragma once
#include "PrimitiveTypes.h"
#include "LinearGraphicObject.h"
using namespace cs;


class AdditionalAxis
	: public LinearGraphicObject
{
public:
	static AdditionalAxis* GetInstance();

public:
	AdditionalAxis();

public:
	void SetPoints(const LogicPoint& p1, const LogicPoint& p2);
};
