#pragma once


namespace cs
{

  class GraphicObject;
	class SimpleShapesGenerator
	{
	public:
		static SimpleShapesGenerator* GetInstance();

	public:
		GraphicObject* CreateTriangle(int h);
	};


}
