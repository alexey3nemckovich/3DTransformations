#include "stdafx.h"
//
#include <vector>
#include <future>
//
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


void ZBuffer::Reset()
{
    int cRows = _buffer.GetCountRows();
    int cCols = _buffer.GetCountColumns();

    for (int i = 0; i < cRows; i++)
    {
        for (int j = 0; j < cCols; j++)
        {
            _buffer(i, j).zValue = INT_MIN;
            _buffer(i, j).color = RGB(255, 255, 255);
        }
    }
}


void ZBuffer::Render(CDC* dc)
{
  int cRows = _buffer.GetCountRows();
  int cCols = _buffer.GetCountColumns();

  vector<DWORD> colors;
  colors.reserve(cRows* cCols);

  for (int x = 0; x < cRows; ++x)
  {
    for (int y = 0; y < cCols; ++y)
    {
      COLORREF curColor = _buffer(x, y).color;
      colors.emplace_back(RGB(GetBValue(curColor), GetGValue(curColor), GetRValue(curColor)));
    }
  }

  BITMAPINFO bitmapInfo = {};
  bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
  bitmapInfo.bmiHeader.biWidth = cCols;
  bitmapInfo.bmiHeader.biHeight = -cRows; //if negative, start top left
  bitmapInfo.bmiHeader.biPlanes = 1;
  bitmapInfo.bmiHeader.biBitCount = sizeof(DWORD) * 8;
  bitmapInfo.bmiHeader.biCompression = BI_RGB;

  SetDIBitsToDevice(*dc, 0, 0, cCols, cRows, 0, 0, 0, cRows, &colors.front(), &bitmapInfo, DIB_RGB_COLORS);
}


void ZBuffer::Init(const CoordinateSystem* coordinateSystem, const vector<GraphicObject::Ptr>& objList, bool renderInvisibleLinesAsDash/* = true*/)
{
	map<const RasterizableGraphicObject*, Rasterization::Ptr> objRastersMap;

	for (auto& curObj : objList)
	{
		if (renderInvisibleLinesAsDash)
		{
			ProcessObj(coordinateSystem, curObj.get(), &objRastersMap);
		}
		else
		{
			ProcessObj(coordinateSystem, curObj.get());
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
	for (auto& objRaster : objRastersMap)
	{
		bool drawing = false;
		bool renderingDashLine = false;
		for (auto& cBorderPoint : objRaster.second->borderPoints)
		{
			ProcessRasterBorderPoint(cBorderPoint, countPointsPerCurSegment, renderingDashLine, drawing);
		}
	}
}


void ZBuffer::ProcessRasterBorderPoint(const RasterizationPoint& rasterPoint, int& countPointsPerCurSegment, bool& renderingDashLine, bool& drawing)
{
	constexpr int countPointsPerDashSegment = 30;

	if (PtInRect(&_buffRect, rasterPoint.point))
	{
		if (_buffer(rasterPoint.point.y, rasterPoint.point.x).zValue > rasterPoint.zValue)
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
				_buffer(rasterPoint.point.y, rasterPoint.point.x).color = rasterPoint.color;
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


void ZBuffer::ProcessObj(
  __in const CoordinateSystem* coordinateSystem,
  __in const GraphicObject* obj,
  __out map<const RasterizableGraphicObject*, Rasterization::Ptr>* rasterMap/* = nullptr*/
)
{
  auto& objRasterizationPrimitivesList = obj->GetRasterizationPrimitives();

  auto&& taskFunc = [
    this,
      coordinateSystem,
      rasterMap
  ](auto&& rasterObj)
  {
    this->ProcessRasterizationPrimitive(coordinateSystem, rasterObj, rasterMap);
  };
    vector<future<void>> asyncTasks;
    asyncTasks.reserve(objRasterizationPrimitivesList.size());
    for (auto& rasterObj : objRasterizationPrimitivesList)
    {
      asyncTasks.emplace_back(async(
        launch::async,
        taskFunc,
        rasterObj
        ));
    }

    for (auto& asyncTask : asyncTasks)
    {
      asyncTask.wait();
    }
}


void ZBuffer::ProcessRasterizationPrimitive(
    __in const CoordinateSystem* coordinateSystem,
    __in const RasterizableGraphicObject* rasterizableGraphicObject,
    __out map<const RasterizableGraphicObject*, Rasterization::Ptr>* rasterMap/* = nullptr*/
)
{
    auto rasterization = rasterizableGraphicObject->CalcRasterization(coordinateSystem);

    if (rasterMap)
    {
        (*rasterMap)[rasterizableGraphicObject] = rasterization;
    }

    for (auto& objRasterPoint : (*rasterization).points)
    {
        if (PtInRect(&_buffRect, objRasterPoint.point))
        {
            Element& buffElement = _buffer(objRasterPoint.point.y, objRasterPoint.point.x);

            if (objRasterPoint.zValue > buffElement.zValue)
            {
                buffElement.color = objRasterPoint.color;
                buffElement.zValue = objRasterPoint.zValue;
            }
        }
    }
}
