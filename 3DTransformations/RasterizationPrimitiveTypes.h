#pragma once
using namespace std;


namespace cs
{


	struct RasterizationPoint
	{
	public:

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
            points.reserve(20000);
            borderPoints.reserve(2000);
        }

	public:
		void AddPoint(const CPoint& point, double zValue, COLORREF color, bool borderPoint)
		{
            if (borderPoint)
            {
                borderPoints.emplace_back(RasterizationPoint(point, zValue, color));
            }
            else
            {
                points.emplace_back(
                    RasterizationPoint(point, zValue, color)
                );
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
		vector<RasterizationPoint> points;
		vector<RasterizationPoint> borderPoints;
	};


}
