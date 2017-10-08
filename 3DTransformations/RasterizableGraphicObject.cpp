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
		auto cPoints = _points.size();
		for (int i = 0; i < cPoints; i++)
		{
			_points[i].x += d;
			_points[i].y += d;
			_points[i].z += d;
		}
	}


	vector<LogicPoint> RasterizableGraphicObject::CalcProjectionSystemPoints(const CoordinateSystem* coordSystem) const
	{
		vector<LogicPoint> v(_points.size());

		for (auto& point : _points)
		{
			v.push_back(coordSystem->ConvertToProjectionSytemPoint(point));
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
		size_t cAxises = _points.size();

		for (size_t i = 0; i < cAxises; i++)
		{
			_axises.push_back(
				Axis(
					_points[i], 
					_points[(i+1) % cAxises]
				)
			);
		}

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
		Axis projectionAxis(
			projectionSegment.first,
			projectionSegment.second
		);

		return RasterizeLineSegment(
			coordSystem,
			projectionAxis,
			projectionSegment.first,
			projectionSegment.second,
			_penColor,
			_penWidth
		);
	}


	Rasterization::Ptr RasterizableGraphicObject::Rasterize(const CoordinateSystem* coordSystem, const Plane& planeInProjectionSystem) const
	{
		//Find points screen coordinates
        pair<CPoint, int> topPointAndIndex;
        pair<CPoint, int> bottomPointAndIndex;
        topPointAndIndex.first.y = INT_MIN;
        bottomPointAndIndex.first.y = INT_MAX;
		vector<CPoint> cornersPoints;
		auto cPoints = _points.size();
		for (int i = 0; i < cPoints; i++)
		{
			cornersPoints.push_back(coordSystem->ConvertLogicPointToPhys(_points[i]));

            if (topPointAndIndex.first.y < cornersPoints[i].y)
            {
                topPointAndIndex.first = cornersPoints[i];
                topPointAndIndex.second = i;
            }

            if (bottomPointAndIndex.first.y > cornersPoints[i].y)
            {
                bottomPointAndIndex.first = cornersPoints[i];
                bottomPointAndIndex.second = i;
            }
		}

		//Find rasterization
		auto physOrigin = coordSystem->GetPhysOrigin();
        Rasterization* rasterization = new Rasterization();

        int leftPointIndex = cornersPoints.size() - 1;
        int rightPointIndex = 0;

        if (topPointAndIndex.second)
        {
            leftPointIndex = topPointAndIndex.second - 1;
        }

        if (topPointAndIndex.second != (cornersPoints.size() - 1))
        {
            rightPointIndex = topPointAndIndex.second + 1;
        }

        pair<CPoint, int> leftPointAndIndex{
            cornersPoints[leftPointIndex],
            leftPointIndex
        };

        pair<CPoint, int> rightPointAndIndex{
            cornersPoints[rightPointIndex],
            rightPointIndex
        };

        Axis leftAxis(topPointAndIndex.first, leftPointAndIndex.first);
        Axis rightAxis(topPointAndIndex.first, rightPointAndIndex.first);

        double zValue = 0;
        int lx = 0;
        int ly = 0;

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
                    int x = min(xLeft, xRight);
                    int maxX = max(xLeft, xRight);
                    while (x <= maxX)
                    {
                        lx = x - physOrigin.x;
                        ly = y - physOrigin.y;
                        zValue = -(planeInProjectionSystem.A() * lx + planeInProjectionSystem.B() * ly + planeInProjectionSystem.D()) / planeInProjectionSystem.C();
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
                    leftPointAndIndex.second = cornersPoints.size() - 1;
                }

                leftPointAndIndex.first = cornersPoints[leftPointAndIndex.second];
                leftAxis = Axis(prevPoint, leftPointAndIndex.first);
            }
            else
            {
                CPoint prevPoint = rightPointAndIndex.first;

                rightPointAndIndex.second++;
                if (rightPointAndIndex.second > (cornersPoints.size() - 1))
                {
                    rightPointAndIndex.second = 0;
                }

                rightPointAndIndex.first = cornersPoints[rightPointAndIndex.second];
                rightAxis = Axis(prevPoint, rightPointAndIndex.first);
            }
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

		size_t cPoints = _points.size();
		for (size_t i = 0; i < cPoints; i++)
		{
			for (size_t j = i + 1; i < cPoints; i++)
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
