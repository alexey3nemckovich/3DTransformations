#pragma once
#include "GraphicObject.h"
#include "Polygon.h"


namespace cs
{


	class Polyhedron
		: public GraphicObject
	{
	public:
		friend class CoordinateSystem;

	public:
		Polyhedron(const vector<Polygon::Ptr>& facets, int penStyle = PS_SOLID, int penWidth = 1, COLORREF penColor = RGB(0, 0, 0));
		Polyhedron(const Polyhedron& other);

	public:
		Polygon const* operator[](int index);

	protected:
		virtual void Render(const CoordinateSystem*, CDC *dc) const override;
		virtual void Rasterize() const override;

	private:
		vector<Polygon::Ptr> _facets;
	};


}
