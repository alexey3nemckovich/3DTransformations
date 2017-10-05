#pragma once
#include "PrimitiveTypes.h"
#include "PrimitiveTypesTemplates.h"
#include "GraphicObject.h"
#include "RasterizationPrimitiveTypes.h"
#include <map>
using namespace std;


namespace cs
{


	class RasterizableGraphicObject;
	class ZBuffer
	{
	public:
		struct Element
		{
			Element()
			{
				color = RGB(255, 255, 255);
				zValue = INT_MIN;
			}

			double zValue;
			COLORREF color;
		};

		static ZBuffer& GetInstance();

	public:
		ZBuffer();

	public:
		void Render(CDC*);

	public:
		void Init(const CoordinateSystem* coordinateSystem, const vector<GraphicObject::Ptr>& objList, bool invisibleLinesAsDash = true);
		void RenderInvisibleLinesAsDash(const map<const RasterizableGraphicObject*, Rasterization::Ptr>&);
		void ProcessRasterBorderPoint(RasterizationPoint::Ptr rasterPoint, int& countPointsPerCurSegment, bool& renderingDashLine, bool& drawing);
		void Resize(int cRows, int cCols);

	public:
		Element* operator[](int row);

	private:
		void ProcessObj(__in const CoordinateSystem* coordinateSystem, __in const GraphicObject*, bool storeRasterInfo, map<const RasterizableGraphicObject*, Rasterization::Ptr>* = nullptr);

	private:
		CRect _buffRect;
		Matrix<Element> _buffer;
	};


}
