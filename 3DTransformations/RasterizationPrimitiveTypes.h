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
    RasterizationPoint(RasterizationPoint&& o) = default;
    RasterizationPoint& operator=(RasterizationPoint&& o) = default;
    RasterizationPoint(const RasterizationPoint& o) = default;
    RasterizationPoint& operator=(const RasterizationPoint& o) = default;

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

    Rasterization(Rasterization&& o) = default;
    Rasterization& operator=(Rasterization&& o) = default;
    Rasterization(const Rasterization& o) = default;
    Rasterization& operator=(const Rasterization& o) = default;


		typedef shared_ptr<Rasterization> Ptr;

	public:
		void AddPoint(const CPoint& point, double zValue, COLORREF color, bool borderPoint)
		{
			RasterizationPoint::Ptr pPoint = make_shared<RasterizationPoint>(
        point,
        zValue,
        color
      );
			if (borderPoint)
			{
				RasterizationPoint::Ptr pBorderPoint = pPoint;
				borderPoints.push_back(pBorderPoint);
			}
			points.push_back(std::move(pPoint));
		}


		void AddPoint(int x, int y, double zValue, COLORREF color, bool borderPoint)
		{
			AddPoint(
				std::move(CPoint(x, y)),
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
