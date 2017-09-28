#include "stdafx.h"
#include "ZBuffer.h"
#include "CoordinateSystem.h"
#include "LinearGraphicObject.h"
using namespace cs;


ZBuffer* ZBuffer::GetInstance()
{
	static ZBuffer zBuffer;
	return &zBuffer;
}


ZBuffer::ZBuffer()
	: _buffer(0, 0)
{

}


void ZBuffer::Render(CDC* dc)
{
	int cRows = _buffer.GetCountRows();
	int cCols = _buffer.GetCountColumns();

	CPoint point;
	for (int i = 0; i < cRows; i++)
	{
		for (int j = 0; j < cCols; j++)
		{
			if (RGB(255, 255, 255) != _buffer[i][j].color)
			{
				point.y = i;
				point.x = j;

				dc->SetPixel(point, _buffer[i][j].color);
			}
		}
	}
}


void ZBuffer::Init(const CoordinateSystem* coordinateSystem, const vector<GraphicObject::Ptr>& objList)
{
	int cObj = objList.size();
	for (int i = 0; i < cObj; i++)
	{
		ProcessObj(coordinateSystem, objList[i].get());
	}
}


void ZBuffer::Resize(int cRows, int cCols)
{
	if (cRows != _buffer.GetCountRows() || cCols != _buffer.GetCountColumns())
	{
		_buffer = Matrix<Element>(cRows, cCols);
	}
}


ZBuffer::Element* ZBuffer::operator[](int row) const
{
	return _buffer[row];
}


void ZBuffer::ProcessObj(__in const CoordinateSystem* coordinateSystem, __in const GraphicObject* obj)
{
	auto objRasterizationPrimitivesList = obj->GetRasterizationPrimitives();
	auto cRasterPrimitives = objRasterizationPrimitivesList.size();

	for (int i = 0; i < cRasterPrimitives; i++)
	{
		auto rasterization = objRasterizationPrimitivesList[i]->CalcRasterization(coordinateSystem);
		auto cRasterPoints = (*rasterization).points.size();

		for (int j = 0; j < cRasterPoints; j++)
		{
			RasterizationPoint* objRasterPoint = (*rasterization).points[j].get();
			if (objRasterPoint->point.x >= 0 && objRasterPoint->point.y >= 0)
			{
				Element& buffElement = _buffer[objRasterPoint->point.y][objRasterPoint->point.x];

				if (objRasterPoint->zValue < buffElement.zValue)
				{
					buffElement.color = objRasterPoint->color;
					buffElement.zValue = objRasterPoint->zValue;
				}
			}
		}
	}
}
