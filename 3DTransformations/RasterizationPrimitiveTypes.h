#pragma once
#include <vector>
#include <memory>
#include <afxwin.h>
using namespace std;


namespace cs
{


	struct RasterizationPoint
	{
	public:
		typedef shared_ptr<RasterizationPoint> Ptr;

	public:
		RasterizationPoint(const CPoint& point, double zValue, COLORREF color)
			: point(point),
			zValue(zValue),
			color(color)
		{

		}

	public:
		CPoint point;
		double zValue;
		COLORREF color;
	};


	struct Rasterization
	{
	public:
    Rasterization()
    {
      points.reserve(100000);
      borderPoints.reserve(10000);
    }

		typedef shared_ptr<Rasterization> Ptr;

	public:
		void AddPoint(const CPoint& point, double zValue, COLORREF color, bool borderPoint)
		{
			RasterizationPoint::Ptr pPoint = make_shared<RasterizationPoint>(
        point,
        zValue,
        color
      );
			points.push_back(pPoint);
			if (borderPoint)
			{
				RasterizationPoint::Ptr pBorderPoint = pPoint;
				borderPoints.push_back(pBorderPoint);
			}
		}


		void AddPoint(int x, int y, double zValue, COLORREF color, bool borderPoint)
		{
			AddPoint(
				CPoint(x, y),
				zValue,
				color,
				borderPoint
			);
		}

	public:
		vector<RasterizationPoint::Ptr> points;
		vector<RasterizationPoint::Ptr> borderPoints;
	};


}
