#include "stdafx.h"
#include "CoordinateSystem.h"
using namespace cs;


Polyhedron::Polyhedron(const vector<cs::Polygon::Ptr>& facets, int penStyle/* = PS_SOLID*/, int penWidth/* = 1*/, COLORREF penColor/* = RGB(0, 0, 0)*/)
	: GraphicsObject(penStyle, penWidth, penColor)
{
	_facets = facets;
}


Polyhedron::Polyhedron(const Polyhedron& other)
	: GraphicsObject(other)
{

}


void Polyhedron::Render(const CoordinateSystem* cs, CDC *dc)
{
	int countFacets = _facets.size();
	for (int i = 0; i < countFacets; i++)
	{
		_facets[i]->Render(cs, dc);
	}
}
