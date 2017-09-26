#pragma once
#include "LinearGraphicObject.h"
#include "PrimitiveTypesTemplates.h"


namespace cs
{


	class Polygon
		: public LinearGraphicObject
	{
	public:
		friend class CoordinateSystem;
		friend class Polyhedron;

		typedef shared_ptr<Polygon> Ptr;

	public:
		Polygon(const vector<LogicPoint>& points, bool rightHandNormalVector = true, int penStyle = PS_SOLID, int penWidth = 1, COLORREF penColor = RGB(0, 0, 0));
		Polygon(const Polygon& other);

	protected:
		virtual void Render(const CoordinateSystem*, CDC *dc) const override;
		virtual void Rasterize() const override;

	private:
		void Init();

	private:
		CBrush _brush;
		bool _rightHandNormalVector;
		HomogeneousPoint<double> _nVector;
	};


}
