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
        typedef shared_ptr<Rasterization> Ptr;

        Rasterization()
        {
          //points.reserve(100000);
          //borderPoints.reserve(10000);
        }

	public:
		void AddPoint(const CPoint& point, double zValue, COLORREF color, bool borderPoint)
		{
            points.push_back(
                RasterizationPoint::Ptr(new RasterizationPoint(
                    point, zValue, color
                ))
            );
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
