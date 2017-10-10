#pragma once
//
#include "RasterizableGraphicObject.h"
using namespace std;


namespace cs
{

  class LinearGraphicObject
		: public RasterizableGraphicObject
	{
	public:
		friend class CoordinateSystem;

		LinearGraphicObject(const vector<LogicPoint>& points, int penStyle = PS_SOLID, int penWidth = 1, COLORREF penColor = RGB(0, 0, 0));
		LinearGraphicObject(const LinearGraphicObject& other);

	protected:
		virtual void Render(const CoordinateSystem*, CDC *dc) const override;
	};


}
