#include "stdafx.h"
#include "Polyhedron.h"
using namespace cs;


Polyhedron::Polyhedron(const vector<cs::Polygon::Ptr>& facets, int penStyle/* = PS_SOLID*/, int penWidth/* = 1*/, COLORREF penColor/* = RGB(0, 0, 0)*/)
	: GraphicObject(penStyle, penWidth, penColor),
	_facets(facets)
{
	Init();
}


Polyhedron::Polyhedron(const Polyhedron& other)
	: GraphicObject(other),
	_facets(other._facets)
{
	Init();
}


void Polyhedron::Init()
{
	for (auto& facet : _facets)
	{
		_rasterPrimitives.push_back(facet.get());
	}
}


cs::Polygon const* Polyhedron::operator[](int index)
{
	return _facets[index].get();
}


void Polyhedron::Render(const CoordinateSystem* cs, CDC *dc) const
{
	for (auto& facet : _facets)
	{
    facet->Render(cs, dc);
	}
}


vector<const RasterizableGraphicObject*> Polyhedron::GetRasterizationPrimitives() const
{
	return _rasterPrimitives;
}
