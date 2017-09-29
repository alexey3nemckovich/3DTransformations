#include "stdafx.h"
#include "Geometry.h"
#include "CoordinateSystem.h"
#include "RasterizableGraphicObject.h"


namespace cs
{


	RasterizableGraphicObject::RasterizableGraphicObject(const vector<LogicPoint>& points, int penStyle/* = PS_SOLID*/, int penWidth/* = 1*/, COLORREF penColor/* = RGB(0, 0, 0)*/, COLORREF brushColor/* = RGB(0, 0, 0)*/)
		: GraphicObject(penStyle, penWidth, penColor),
		_points(points),
		_brushColor(brushColor)
	{

	}


	RasterizableGraphicObject::RasterizableGraphicObject(const RasterizableGraphicObject& other)
		: GraphicObject(other),
		_points(other._points),
		_brushColor(other._brushColor)
	{

	}


	RasterizableGraphicObject::~RasterizableGraphicObject()
	{

	}


	vector<const RasterizableGraphicObject*> RasterizableGraphicObject::GetRasterizationPrimitives() const
	{
		return {this};
	}


	const Rasterization::Ptr RasterizableGraphicObject::CalcRasterization(const CoordinateSystem* coordSystem) const
	{
		Rasterization* rasterization = new Rasterization();

		//Find corners points screen coordinates
		vector<CPoint> cornersPoints;
		auto cPoints = _points.size();
		for (int i = 0; i < cPoints; i++)
		{
			cornersPoints.push_back(coordSystem->ConvertLogicPointToPhys(_points[i]));
		}

		//Find top left and bottom right points
		int maxY = cornersPoints[0].y;
		int minY = cornersPoints[0].y;
		int minX = cornersPoints[0].x;
		int maxX = cornersPoints[0].x;
		for (int i = 1; i < cPoints; i++)
		{
			if (maxY < cornersPoints[i].y)
			{
				maxY = cornersPoints[i].y;
				goto checkX;
			}

			if (minY > cornersPoints[i].y)
			{
				minY = cornersPoints[i].y;
			}

		checkX:

			if (maxX < cornersPoints[i].y)
			{
				maxX = cornersPoints[i].x;
				continue;
			}

			if (minX > cornersPoints[i].x)
			{
				minX = cornersPoints[i].x;
			}
		}

		//Find rasterization
		bool borderPoint = false;
		double zValue = 0;

		Plane plane(
			_points[0],
			_points[1],
			_points[2]
		);

		LogicPoint tmpPoint;

		int y = minY;
		while (y <= maxY)
		{
			int x = minX;
			while (x <= maxX)
			{
				if (DoesPointBelongToPolygon(cornersPoints, _penWidth, x, y, borderPoint))
				{
					tmpPoint.x = x;
					tmpPoint.y = y;

					if (false == plane.FindZByXAndY(x, y, tmpPoint.z))
					{
						tmpPoint.z = FindNearestPointToWatcherIntersectionWithBordersDepth(coordSystem, x, y);
					}

					if (borderPoint)
					{
						rasterization->AddPoint(x, y, zValue, _penColor, borderPoint);
					}
					else
					{
						rasterization->AddPoint(x, y, zValue, _brushColor, borderPoint);
					}
				}

				x++;
			}

			y++;
		}

		return Rasterization::Ptr(rasterization);
	}


	double RasterizableGraphicObject::FindNearestPointToWatcherIntersectionWithBordersDepth(const CoordinateSystem* coordSystem, int x, int y) const
	{
		return 0;
	}


}
