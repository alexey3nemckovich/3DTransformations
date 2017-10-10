#pragma once
#include "GraphicObject.h"
using namespace std;


namespace cs
{

  struct LogicPoint;
	class CoordinateSystem;
  struct Plane;
  class Axis;
  struct Rasterization;
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
		virtual const shared_ptr<Rasterization> CalcRasterization(const CoordinateSystem*) const;

	private:
		void Init();

	private:
    shared_ptr<Rasterization> RasterizeToLine(const CoordinateSystem*) const;
    shared_ptr<Rasterization> Rasterize(const CoordinateSystem*, const Plane& planeInProjectionSystem) const;
		pair<LogicPoint, LogicPoint> FindMaxDistanceProjectionLineSegment(const CoordinateSystem*) const;

	protected:
		COLORREF _brushColor;

	protected:
		bool _line;
		shared_ptr<Plane> _plane;
		vector<Axis> _axises;
		vector<LogicPoint> _points;
	};


}
