#pragma once
#include "GraphicObject.h"
#include "PrimitiveTypes.h"
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
		typedef shared_ptr<Rasterization> Ptr;

	public:
		void AddPoint(const CPoint& point, double zValue, COLORREF color, bool borderPoint)
		{
			RasterizationPoint* newPoint = new RasterizationPoint(
				point,
				zValue,
				color
			);

			RasterizationPoint::Ptr pPoint = RasterizationPoint::Ptr(newPoint);
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


	class CoordinateSystem;
	class RasterizableGraphicObject
		: public GraphicObject
	{
	public:
		friend class CoordinateSystem;

		RasterizableGraphicObject(const vector<LogicPoint>& points, int penStyle = PS_SOLID, int penWidth = 1, COLORREF penColor = RGB(0, 0, 0), COLORREF brushColor = RGB(0, 0, 0));
		RasterizableGraphicObject(const RasterizableGraphicObject& other);
		~RasterizableGraphicObject();

	public:
		virtual vector<const RasterizableGraphicObject*> GetRasterizationPrimitives() const override;

	public:
		virtual const Rasterization::Ptr CalcRasterization(const CoordinateSystem*) const;

	protected:
		COLORREF _brushColor;

	protected:
		vector<LogicPoint> _points;
	};


}
