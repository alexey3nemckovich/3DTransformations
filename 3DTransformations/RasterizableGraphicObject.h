#pragma once
#include "GraphicObject.h"
#include "PrimitiveTypes.h"
#include "RasterizationPrimitiveTypes.h"
using namespace std;


namespace cs
{


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
		vector<LogicPoint> GetPoints() const;
		virtual vector<const RasterizableGraphicObject*> GetRasterizationPrimitives() const override;

	public:
		void Move(int d);

	public:
		vector<LogicPoint> CalcProjectionSystemPoints(const CoordinateSystem*) const;
		virtual const Rasterization::Ptr CalcRasterization(const CoordinateSystem*) const;

	private:
		void Init();

	private:
		Rasterization::Ptr RasterizeToLine(const CoordinateSystem*) const;
		Rasterization::Ptr Rasterize(const CoordinateSystem*, const Plane& planeInProjectionSystem) const;
		pair<LogicPoint, LogicPoint> FindMaxDistanceProjectionLineSegment(const CoordinateSystem*) const;

	protected:
		COLORREF _brushColor;

	protected:
		bool _line;
		Plane::Ptr _plane;
		vector<Axis> _axises;
		vector<LogicPoint> _points;
	};


}
