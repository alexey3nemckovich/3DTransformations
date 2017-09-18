#pragma once
#include "CoordinateSystem.h"


namespace cs
{


	class SimpleShapesGenerator
	{
	public:
		static SimpleShapesGenerator* GetInstance();

	public:
		GraphicsObject* CreateTriangle(int h);
	};


}
