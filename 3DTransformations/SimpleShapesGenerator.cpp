#include "stdafx.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "AdditionalAxis.h"
#include "Polygon.h"
#include "Polyhedron.h"
#include "SimpleShapesGenerator.h"


namespace cs
{


	SimpleShapesGenerator* SimpleShapesGenerator::GetInstance()
	{
		static SimpleShapesGenerator shapesGenerator;
		return &shapesGenerator;
	}


	GraphicObject* SimpleShapesGenerator::CreateTriangle(int h)
	{
		static auto additionalAxis = AdditionalAxis::GetInstance();

		double r = 3 * sqrt(3) * h / 4;
		double R = r * 3;
		double pH = (double)h / 2;

		//Find points
		LogicPoint topLittleTrianglePoints[3];
		LogicPoint bottomLittleTrianglePoints[3];

		LogicPoint topBigTrianglePoints[3];
		LogicPoint bottomBigTrianglePoints[3];
		double angle = 0;
		for (int i = 0; i < 3; i++)
		{
			topLittleTrianglePoints[i].z = pH;
			bottomLittleTrianglePoints[i].z = -pH;
			topLittleTrianglePoints[i].x = bottomLittleTrianglePoints[i].x = r * cos(angle);
			topLittleTrianglePoints[i].y = bottomLittleTrianglePoints[i].y = r * sin(angle);
			
			topBigTrianglePoints[i].z = pH;
			bottomBigTrianglePoints[i].z = -pH;
			topBigTrianglePoints[i].x = bottomBigTrianglePoints[i].x = R * cos(angle);
			topBigTrianglePoints[i].y = bottomBigTrianglePoints[i].y = R * sin(angle);

			angle += (M_PI * 2) / 3;
		}

		additionalAxis->SetPoints(
			bottomBigTrianglePoints[0],
			LogicPoint(
				(topLittleTrianglePoints[1].x + topLittleTrianglePoints[2].x) / 2,
				(topLittleTrianglePoints[1].y + topLittleTrianglePoints[2].y) / 2,
				topBigTrianglePoints[1].z
			)
		);
		
		CoordinateSystem::GetInstance()->AddLogicPoint(
			additionalAxis->GetPoints().first, 0, false, false, false, false, false, false
		);

		CoordinateSystem::GetInstance()->AddLogicPoint(
			additionalAxis->GetPoints().second, 0, false, false, false, false, false, false
		);

		//Find facets
		vector<Polygon::Ptr> facets;

		/*facets.push_back(
			Polygon::Ptr(new Polygon({
				LogicPoint(0, 0),
				LogicPoint(100, 0),
				LogicPoint(200, 100),
				LogicPoint(0, 100)
			}, false, 0, 1, RGB(0, 0, 0), RGB(0, 255, 0)))
		);*/

		/*facets.push_back(
			Polygon::Ptr(new Polygon({
				LogicPoint(100, 0, 20),
				LogicPoint(100, 100, 20),
				LogicPoint(100, 100, 100),
				LogicPoint(100, 0, 100)
			}, false, 0, 1, RGB(0, 0, 0), RGB(0, 255, 0)))
		);*/
		for (int i = 0; i < 3; i++)
		{
			/*facets.push_back(
				Polygon::Ptr(new Polygon({
					topLittleTrianglePoints[i],
					bottomLittleTrianglePoints[i],
					bottomLittleTrianglePoints[(i + 1) % 3],
					topLittleTrianglePoints[(i + 1) % 3]
				}, false, 0, 1, RGB(0, 0, 0), RGB(100, 100, 100)))
			);

			facets.push_back(
				Polygon::Ptr(new Polygon({ 
						topBigTrianglePoints[i], 
						bottomBigTrianglePoints[i], 
						bottomBigTrianglePoints[(i + 1) % 3],
						topBigTrianglePoints[(i + 1) % 3]
				}, true, 0, 1, RGB(0, 0, 0), RGB(0, 255, 0)))
			);*/

			/*facets.push_back(
				Polygon::Ptr(new Polygon({
					bottomLittleTrianglePoints[i],
					bottomBigTrianglePoints[i],
					bottomBigTrianglePoints[(i + 1) % 3],
					bottomLittleTrianglePoints[(i + 1) % 3],
				}, false, 0, 1, RGB(0, 0, 0), RGB(100, 100, 100)))
			);*/

			facets.push_back(
				Polygon::Ptr(new Polygon({
					topLittleTrianglePoints[i],
					topBigTrianglePoints[i],
					topBigTrianglePoints[(i + 1) % 3],
					topLittleTrianglePoints[(i + 1) % 3]
				}, true, 0, 1, RGB(0, 0, 0), RGB(0, 0, 255)))
			);
		}

		return new Polyhedron(facets);
	}


}
