#pragma once
#include "GraphicObject.h"


namespace cs
{


	class SimpleShapesGenerator
	{
	public:
		static SimpleShapesGenerator* GetInstance();

	public:
		GraphicObject* CreateTriangle(int h);
	};


}
