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
    _segmentProcessingLambda = [this](int firstRow, int lastRow)
    {
        for (int row = firstRow; row < lastRow; row++)
        {
            for (int col = 0; col < _cCols; col++)
            {
                COLORREF& curColor = _buffer(row, col).color;
                _colors[row * _cCols + col] = RGB(GetBValue(curColor), GetGValue(curColor), GetRValue(curColor));
            }
        }
    };
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
    constexpr int segmentHeight = 200;

    /*vector<future<void>> tasks;
    for (int row = 0; row < _cRows; row += segmentHeight)
    {
        tasks.push_back(
            async(
                launch::async,
                _segmentProcessingLambda,
                row,
                ((row + segmentHeight) > _cRows)? _cRows : row + segmentHeight
            )
        );
    }

    size_t cTasks = tasks.size();
    for (int i = 0; i < cTasks; i++)
    {
        tasks[i].wait();
    }*/
    for (int row = 0; row < _cRows; row++)
    {
        for (int col = 0; col < _cCols; col++)
        {
            COLORREF& curColor = _buffer(row, col).color;
            _colors[row * _cCols + col] = RGB(GetBValue(curColor), GetGValue(curColor), GetRValue(curColor));
        }
    }

    BITMAPINFO bitmapInfo = {};
    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = _cCols;
    bitmapInfo.bmiHeader.biHeight = -_cRows; //if negative, start top left
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = sizeof(DWORD) * 8;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    SetDIBitsToDevice(*dc, 0, 0, _cCols, _cRows, 0, 0, 0, _cRows, &_colors.front(), &bitmapInfo, DIB_RGB_COLORS);
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
	constexpr int countPointsPerDashSegment = 10;

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
    if (cRows != _cRows || cCols != _cCols)
	{
        _colors.resize(cRows * cCols);
		_buffer = Matrix<Element>(cRows, cCols);

		_buffRect.left = _buffRect.top = 0;
		_buffRect.right = cCols;
		_buffRect.bottom = cRows;
        _cRows = cRows;
        _cCols = cCols;
	}
}


void ZBuffer::ProcessObj(
  __in const CoordinateSystem* coordinateSystem,
  __in const GraphicObject* obj,
  __out map<const RasterizableGraphicObject*, Rasterization::Ptr>* rasterMap/* = nullptr*/
)
{
    auto& objRasterizationPrimitivesList = obj->GetRasterizationPrimitives();
    for (auto& rasterObj : objRasterizationPrimitivesList)
    {
        ProcessRasterizationPrimitive(coordinateSystem, rasterObj, rasterMap);
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

    //Brush points
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

    //Border points
    for (auto& objBorderPoint : (*rasterization).borderPoints)
    {
        if (PtInRect(&_buffRect, objBorderPoint.point))
        {
            Element& buffElement = _buffer(objBorderPoint.point.y, objBorderPoint.point.x);

            if ((objBorderPoint.zValue + 1) > buffElement.zValue)
            {
                buffElement.color = objBorderPoint.color;
                buffElement.zValue = objBorderPoint.zValue;
            }
        }
    }
}
