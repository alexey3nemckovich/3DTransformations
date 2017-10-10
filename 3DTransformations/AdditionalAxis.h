#pragma once
#include "LinearGraphicObject.h"
using namespace cs;

namespace cs
{
  struct LogicPoint;
};
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
