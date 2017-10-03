#include "stdafx.h"
#include "Geometry.h"
#include "CoordinateSystem.h"
#include "RasterizableGraphicObject.h"


namespace cs
{


	RasterizableGraphicObject::RasterizableGraphicObject(const vector<LogicPoint>& points, int penStyle/* = PS_SOLID*/, int penWidth/* = 1*/, COLORREF penColor/* = RGB(0, 0, 0)*/, COLORREF brushColor/* = RGB(0, 0, 0)*/)
		: GraphicObject(penStyle, penWidth, penColor),
		_points(points),
		_brushColor(brushColor),
		_plane(
			points[0],
			points[1],
			points[2]
		)
	{
		Init();
	}


	RasterizableGraphicObject::RasterizableGraphicObject(const RasterizableGraphicObject& other)
		: GraphicObject(other),
		_points(other._points),
		_brushColor(other._brushColor),
		_plane(
			other._points[0],
			other._points[1],
			other._points[2]
		)
	{
		Init();
	}


	RasterizableGraphicObject::~RasterizableGraphicObject()
	{

	}


	vector<const RasterizableGraphicObject*> RasterizableGraphicObject::GetRasterizationPrimitives() const
	{
		return {this};
	}


	vector<LogicPoint> RasterizableGraphicObject::CalcProjectionSystemPoints(const CoordinateSystem* coordSystem) const
	{
		vector<LogicPoint> v;

		int cPoints = _points.size();
		for (int i = 0; i < cPoints; i++)
		{
			v.push_back(coordSystem->ConvertToProjectionSytemPoint(_points[i]));
		}

		return v;
	}


	const Rasterization::Ptr RasterizableGraphicObject::CalcRasterization(const CoordinateSystem* coordSystem) const
	{
		Plane planeInProjectionSystem(
			coordSystem->ConvertToProjectionSytemPoint(_points[0]),
			coordSystem->ConvertToProjectionSytemPoint(_points[1]),
			coordSystem->ConvertToProjectionSytemPoint(_points[2])
		);

		if (false == planeInProjectionSystem.IsParallelToOz())
		{
			return Rasterize(coordSystem, planeInProjectionSystem);
		}
		else
		{
			auto projectionSegment = FindMaxDistanceProjectionLineSegment(coordSystem);
			Axis projectionAxis(projectionSegment.first, projectionSegment.second);

			return RasterizeLineSegment(
				coordSystem, 
				projectionAxis,
				projectionSegment.first,
				projectionSegment.second,
				_penColor,
				_penWidth
			);
		}
	}


	void RasterizableGraphicObject::Init()
	{
		int cAxises = _points.size();

		for (int i = 0; i < cAxises; i++)
		{
			_axises.push_back(
				Axis(
					_points[i], 
					_points[(i+1) % cAxises]
				)
			);
		}
	}


	Rasterization::Ptr RasterizableGraphicObject::Rasterize(const CoordinateSystem* coordSystem, const Plane& planeInProjectionSystem) const
	{
		int minX = 0, minY = 0, maxX = 0, maxY = 0;
		CalcRasterizationBounds(coordSystem, minX, minY, maxX, maxY);

		//Find corners points screen coordinates
		vector<CPoint> cornersPoints;
		auto cPoints = _points.size();
		for (int i = 0; i < cPoints; i++)
		{
			cornersPoints.push_back(coordSystem->ConvertLogicPointToPhys(_points[i]));
		}

		vector<Axis> rasterizationAxises;
		for (int i = 0; i < cPoints; i++)
		{
			rasterizationAxises.push_back(Axis(
				LogicPoint(
					cornersPoints[i].x,
					cornersPoints[i].y
				),
				LogicPoint(
					cornersPoints[(i + 1) % cPoints].x,
					cornersPoints[(i + 1) % cPoints].y
				)
			));
		}

		//Find rasterization
		int y = minY;
		double zValue = 0;
		bool borderPoint = false;
		auto physOrigin = coordSystem->GetPhysOrigin();
		Rasterization* rasterization = new Rasterization();

		int lx = 0, ly = 0;
		while (y <= maxY)
		{
			int x = minX;
			while (x <= maxX)
			{
				if (DoesPointBelongToPolygon(rasterizationAxises, _penWidth, x, y, borderPoint))
				{
					lx = x - physOrigin.x;
					ly = y - physOrigin.y;
					zValue = -(planeInProjectionSystem.A() * lx + planeInProjectionSystem.B() * ly + planeInProjectionSystem.D()) / planeInProjectionSystem.C();

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


	void RasterizableGraphicObject::CalcRasterizationBounds(const CoordinateSystem* coordSystem, int& minX, int& minY, int& maxX, int& maxY) const
	{
		//Find corners points screen coordinates
		vector<CPoint> cornersPoints;
		auto cPoints = _points.size();
		for (int i = 0; i < cPoints; i++)
		{
			cornersPoints.push_back(coordSystem->ConvertLogicPointToPhys(_points[i]));
		}

		//Find top left and bottom right points
		minX = cornersPoints[0].x;
		minY = cornersPoints[0].y;
		maxX = cornersPoints[0].x;
		maxY = cornersPoints[0].y;
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

			if (maxX < cornersPoints[i].x)
			{
				maxX = cornersPoints[i].x;
				continue;
			}

			if (minX > cornersPoints[i].x)
			{
				minX = cornersPoints[i].x;
			}
		}
	}


	pair<LogicPoint, LogicPoint> RasterizableGraphicObject::FindMaxDistanceProjectionLineSegment(const CoordinateSystem* coordSystem) const
	{
		double maxDistance = -1;
		pair<LogicPoint, LogicPoint> line;

		line.first = _points[0];
		line.second = _points[1];

		int cPoints = _points.size();
		for (int i = 0; i < cPoints; i++)
		{
			for (int j = i + 1; i < cPoints; i++)
			{
				auto p1 = coordSystem->ConvertLogicPointToPhys(_points[i]);
				auto p2 = coordSystem->ConvertLogicPointToPhys(_points[j]);

				double dist = FindDistance(p1, p2);
				if (dist > maxDistance)
				{
					line.first = _points[i];
					line.second = _points[j];

					maxDistance = dist;
				}
			}
		}

		return line;
	}


}
