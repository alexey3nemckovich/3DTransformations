#include "stdafx.h"
#include <functional>
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
		Init();
	}


	RasterizableGraphicObject::RasterizableGraphicObject(const RasterizableGraphicObject& other)
		: GraphicObject(other),
		_points(other._points),
		_brushColor(other._brushColor)
	{
		Init();
	}


	RasterizableGraphicObject::~RasterizableGraphicObject()
	{

	}


	vector<LogicPoint> RasterizableGraphicObject::GetPoints() const
	{
		return _points;
	}


	vector<const RasterizableGraphicObject*> RasterizableGraphicObject::GetRasterizationPrimitives() const
	{
		return {this};
	}


	void RasterizableGraphicObject::Move(int d)
	{
		for (auto& point : _points)
		{
            point.x += d;
            point.y += d;
            point.z += d;
		}
	}


	vector<LogicPoint> RasterizableGraphicObject::CalcProjectionSystemPoints(const CoordinateSystem* coordSystem) const
	{
		vector<LogicPoint> v;
        v.reserve(_points.size());
		for (auto& point : _points)
		{
			v.emplace_back(coordSystem->ConvertToProjectionSytemPoint(point));
		}

		return v;
	}


	const Rasterization::Ptr RasterizableGraphicObject::CalcRasterization(const CoordinateSystem* coordSystem) const
	{
		if (_line)
		{
			return  RasterizeToLine(coordSystem);
		}
		else
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
				return RasterizeToLine(coordSystem);
			}
		}
	}


	void RasterizableGraphicObject::Init()
	{
		if (_points.size() > 2)
		{
			_plane = make_shared<Plane>(
				_points[0],
				_points[1],
				_points[2]
			);

			_line = false;
		}
		else
		{
			_line = true;
		}
	}


	Rasterization::Ptr RasterizableGraphicObject::RasterizeToLine(const CoordinateSystem* coordSystem) const
	{
		auto projectionSegment = FindMaxDistanceProjectionLineSegment(coordSystem);
        Rasterization* rasterization = new Rasterization();
        
        RasterizeLineSegment(
			coordSystem,
			rasterization,
			projectionSegment.first,
			projectionSegment.second,
			_penColor,
			_penWidth
		);

        return Rasterization::Ptr(rasterization);
	}


	Rasterization::Ptr RasterizableGraphicObject::Rasterize(const CoordinateSystem* coordSystem, const Plane& planeInProjectionSystem) const
	{
		//Find points screen coordinates
        auto physOrigin = coordSystem->GetPhysOrigin();
        pair<CPoint, int> topPointAndIndex;
        pair<CPoint, int> bottomPointAndIndex;
        topPointAndIndex.first.y = INT_MIN;
        bottomPointAndIndex.first.y = INT_MAX;
		vector<CPoint> cornersPointsScreenCoordinates;
		auto cPoints = _points.size();
        cornersPointsScreenCoordinates.reserve(cPoints);
		for (int i = 0; i < cPoints; i++)
		{
			cornersPointsScreenCoordinates.emplace_back(
                coordSystem->ConvertLogicPointToPhys(_points[i])
            );

            if (topPointAndIndex.first.y < cornersPointsScreenCoordinates[i].y)
            {
                topPointAndIndex.first = cornersPointsScreenCoordinates[i];
                topPointAndIndex.second = i;
            }

            if (bottomPointAndIndex.first.y > cornersPointsScreenCoordinates[i].y)
            {
                bottomPointAndIndex.first = cornersPointsScreenCoordinates[i];
                bottomPointAndIndex.second = i;
            }
		}

		//Find rasterization
        Rasterization* rasterization = new Rasterization();

        int leftPointIndex = cornersPointsScreenCoordinates.size() - 1;
        int rightPointIndex = 0;

        if (topPointAndIndex.second)
        {
            leftPointIndex = topPointAndIndex.second - 1;
        }

        if (topPointAndIndex.second != (cornersPointsScreenCoordinates.size() - 1))
        {
            rightPointIndex = topPointAndIndex.second + 1;
        }

        pair<CPoint, int> leftPointAndIndex{
            cornersPointsScreenCoordinates[leftPointIndex],
            leftPointIndex
        };

        pair<CPoint, int> rightPointAndIndex{
            cornersPointsScreenCoordinates[rightPointIndex],
            rightPointIndex
        };

        Axis leftAxis(topPointAndIndex.first, leftPointAndIndex.first);
        Axis rightAxis(topPointAndIndex.first, rightPointAndIndex.first);

        double zValue = 0;
        int lx = 0;
        int ly = 0;

        //
        double A = planeInProjectionSystem.A();
        double B = planeInProjectionSystem.B();
        double C = planeInProjectionSystem.C();
        double D = planeInProjectionSystem.D();
        std::function<double(double, double)> getZValue = [A, B, C, D](double lx, double ly)
        {
            return -(A * lx + B * ly + D) / C;
        };

        //
        int y = topPointAndIndex.first.y;
        while (y > bottomPointAndIndex.first.y)
        {
            while (y >= leftPointAndIndex.first.y &&
                   y >= rightPointAndIndex.first.y)
            {
                int xLeft = -1, xRight = -1;
                bool resLeft = leftAxis.FindIntersectionWithY(y, xLeft);
                bool resRight = rightAxis.FindIntersectionWithY(y, xRight);

                if (resLeft && resRight)
                {
                    int minX = min(xLeft, xRight);
                    int maxX = max(xLeft, xRight);
                    int x = minX;
                    while (x <= maxX)
                    {
                        lx = x - physOrigin.x;
                        ly = y - physOrigin.y;
                        zValue = getZValue(lx, ly);

                        rasterization->AddPoint(x, y, zValue, _brushColor, false);

                        x++;
                    }
                }

                y--;
            }

            if (y < leftPointAndIndex.first.y)
            {
                CPoint prevPoint = leftPointAndIndex.first;

                leftPointAndIndex.second--;
                if (leftPointAndIndex.second < 0)
                {
                    leftPointAndIndex.second = cornersPointsScreenCoordinates.size() - 1;
                }

                leftPointAndIndex.first = cornersPointsScreenCoordinates[leftPointAndIndex.second];
                leftAxis = Axis(prevPoint, leftPointAndIndex.first);
            }
            else
            {
                CPoint prevPoint = rightPointAndIndex.first;

                rightPointAndIndex.second++;
                if (rightPointAndIndex.second > (cornersPointsScreenCoordinates.size() - 1))
                {
                    rightPointAndIndex.second = 0;
                }

                rightPointAndIndex.first = cornersPointsScreenCoordinates[rightPointAndIndex.second];
                rightAxis = Axis(prevPoint, rightPointAndIndex.first);
            }
        }

        //Rasterize borders
        for (int i = 0; i < cPoints; i++)
        {
            RasterizeLineSegment(
                coordSystem,
                rasterization,
                getZValue,
                _points[i],
                _points[(i + 1) % cPoints],
                _penColor,
                _penWidth
            );
        }

		return Rasterization::Ptr(rasterization);
	}


	pair<LogicPoint, LogicPoint> RasterizableGraphicObject::FindMaxDistanceProjectionLineSegment(const CoordinateSystem* coordSystem) const
	{
		double maxDistance = -1;
		pair<LogicPoint, LogicPoint> line;

		line.first = _points[0];
		line.second = _points[1];

		size_t cPoints = _points.size();
		for (size_t i = 0; i < cPoints; i++)
		{
			for (size_t j = i + 1; j < cPoints; j++)
			{
				double dist = FindDistance(
					coordSystem->ConvertToProjectionSytemPoint(_points[i]),
					coordSystem->ConvertToProjectionSytemPoint(_points[j])
				);

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
