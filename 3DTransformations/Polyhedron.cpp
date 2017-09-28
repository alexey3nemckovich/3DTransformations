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
	int cFacets = _facets.size();
	for (int i = 0; i < cFacets; i++)
	{
		_rasterPrimitives.push_back(_facets[i].get());
	}
}


cs::Polygon const* Polyhedron::operator[](int index)
{
	return _facets[index].get();
}


void Polyhedron::Render(const CoordinateSystem* cs, CDC *dc) const
{
	int countFacets = _facets.size();
	for (int i = 0; i < countFacets; i++)
	{
		_facets[i]->Render(cs, dc);
	}
}


vector<const RasterizableGraphicObject*> Polyhedron::GetRasterizationPrimitives() const
{
	return _rasterPrimitives;
}
