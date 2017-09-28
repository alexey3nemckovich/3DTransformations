#pragma once
#include "PrimitiveTypes.h"
#include "PrimitiveTypesTemplates.h"
#include "GraphicObject.h"


namespace cs
{


	class ZBuffer
	{
	public:
		struct Element
		{
			Element()
			{
				color = RGB(255, 255, 255);
				zValue = INT_MAX;
			}

			double zValue;
			COLORREF color;
		};

		static ZBuffer* GetInstance();

	public:
		ZBuffer();

	public:
		void Render(CDC*);

	public:
		void Init(const CoordinateSystem* coordinateSystem, const vector<GraphicObject::Ptr>& objList);
		void Resize(int cRows, int cCols);

	public:
		Element* operator[](int row) const;

	private:
		void ProcessObj(__in const CoordinateSystem* coordinateSystem, __in const GraphicObject*);

	private:
		Matrix<Element> _buffer;
	};


}
