#include "stdafx.h"
#include "ZBuffer.h"
#include "CoordinateSystem.h"
#include "LinearGraphicObject.h"
#include "RasterizableGraphicObject.h"
using namespace cs;


ZBuffer& ZBuffer::GetInstance()
{
	static ZBuffer zBuffer;
	return zBuffer;
}


ZBuffer::ZBuffer()
	: _buffer(0, 0)
{

}


void ZBuffer::Render(CDC* dc)
{
	int cRows = _buffer.GetCountRows();
	int cCols = _buffer.GetCountColumns();

	for (int x = 0; x < cRows; ++x)
	{
    vector<Element>& curElementLine = _buffer[x];
		for (int y = 0; y < cCols; ++y)
		{
			if (RGB(255, 255, 255) != curElementLine[y].color)
			{
				dc->SetPixel(y, x, curElementLine[y].color);
			}
		}
	}
}


void ZBuffer::Init(const CoordinateSystem* coordinateSystem, const vector<GraphicObject::Ptr>& objList, bool renderInvisibleLinesAsDash/* = true*/)
{
	map<const RasterizableGraphicObject*, Rasterization::Ptr> objRastersMap;

	for (auto& curObj : objList)
	{
		if (renderInvisibleLinesAsDash)
		{
			ProcessObj(coordinateSystem, curObj.get(), true, &objRastersMap);
		}
		else
		{
			ProcessObj(coordinateSystem, curObj.get(), false);
		}
	}

	if (renderInvisibleLinesAsDash)
	{
		RenderInvisibleLinesAsDash(objRastersMap);
	}
}


void ZBuffer::RenderInvisibleLinesAsDash(const map<const RasterizableGraphicObject*, Rasterization::Ptr>& objRastersMap)
{
	int countPointsPerCurSegment = 0;
	const int countPointsPerDashSegment = 30;
	for (auto objRaster : objRastersMap)
	{
		bool drawing = false;
		bool renderingDashLine = false;
		for (auto& cBorderPoint : objRaster.second->borderPoints)
		{
			ProcessRasterBorderPoint(cBorderPoint, countPointsPerCurSegment, renderingDashLine, drawing);
		}
	}
}


void ZBuffer::ProcessRasterBorderPoint(RasterizationPoint::Ptr rasterPoint, int& countPointsPerCurSegment, bool& renderingDashLine, bool& drawing)
{
	const int countPointsPerDashSegment = 30;

	if (PtInRect(&_buffRect, rasterPoint->point))
	{
		if (_buffer[rasterPoint->point.y][rasterPoint->point.x].zValue > rasterPoint->zValue)
		{
			if (!renderingDashLine)
			{
				renderingDashLine = true;

				drawing = true;
				countPointsPerCurSegment = 0;
			}
		}
		else
		{
			renderingDashLine = false;
		}

		if (renderingDashLine)
		{
			if (drawing)
			{
				_buffer[rasterPoint->point.y][rasterPoint->point.x].color = rasterPoint->color;
			}

			countPointsPerCurSegment++;
			if (countPointsPerCurSegment == countPointsPerDashSegment)
			{
				if (drawing)
				{
					drawing = false;
				}
				else
				{
					drawing = true;
				}

				countPointsPerCurSegment = 0;
			}
		}
	}
}


void ZBuffer::Resize(int cRows, int cCols)
{
	if (cRows != _buffer.GetCountRows() || cCols != _buffer.GetCountColumns())
	{
		_buffer = Matrix<Element>(cRows, cCols);
		_buffRect.left = _buffRect.top = 0;
		_buffRect.right = cCols;
		_buffRect.bottom = cRows;
	}
}


ZBuffer::Element* ZBuffer::operator[](int row)
{
  return &(_buffer[row])[0];
}


void ZBuffer::ProcessObj(__in const CoordinateSystem* coordinateSystem, __in const GraphicObject* obj, __in bool storeRasterInfo, __out map<const RasterizableGraphicObject*, Rasterization::Ptr>* rasterMap/* = nullptr*/)
{
	auto objRasterizationPrimitivesList = obj->GetRasterizationPrimitives();
	auto cRasterPrimitives = objRasterizationPrimitivesList.size();

	for (auto& objRasterizationPrimitivesBlock : objRasterizationPrimitivesList)
	{
		auto rasterization = objRasterizationPrimitivesBlock->CalcRasterization(coordinateSystem);

		if (storeRasterInfo)
		{
			(*rasterMap)[objRasterizationPrimitivesBlock] = rasterization;
		}

		for (auto& objRasterPoint : (*rasterization).points)
		{
			if (PtInRect(&_buffRect, objRasterPoint->point))
			{
				Element& buffElement = _buffer[objRasterPoint->point.y][objRasterPoint->point.x];

				if (objRasterPoint->zValue > buffElement.zValue)
				{
					buffElement.color = objRasterPoint->color;
					buffElement.zValue = objRasterPoint->zValue;
				}
			}
		}
	}
}
