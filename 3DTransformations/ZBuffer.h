#pragma once
#include "PrimitiveTypes.h"
#include "PrimitiveTypesTemplates.h"
#include "GraphicObject.h"
#include "RasterizationPrimitiveTypes.h"
#include <map>
#include <functional>
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
        void Reset();

	public:
		void Init(const CoordinateSystem* coordinateSystem, const vector<GraphicObject::Ptr>& objList, bool invisibleLinesAsDash = true);
		void RenderInvisibleLinesAsDash(const map<const RasterizableGraphicObject*, Rasterization::Ptr>&);
		void ProcessRasterBorderPoint(const RasterizationPoint& rasterPoint, int& countPointsPerCurSegment, bool& renderingDashLine, bool& drawing);
		void Resize(int cRows, int cCols);

	private:
        typedef void(*ProcessRasterizationPrimitiveFunction)(
            __in ZBuffer* const buffer,
            __in const CoordinateSystem* coordinateSystem, 
            __in RasterizableGraphicObject* rasterizableGraphicObject,
            __out map<const RasterizableGraphicObject*, Rasterization::Ptr>* rasterMap
        );

		void ProcessObj(
            __in const CoordinateSystem* coordinateSystem, 
            __in const GraphicObject*, 
            __out map<const RasterizableGraphicObject*, Rasterization::Ptr>* = nullptr
        );
        void ProcessRasterizationPrimitive(
            __in const CoordinateSystem* coordinateSystem,
            __in const RasterizableGraphicObject* rasterizableGraphicObject,
            __out map<const RasterizableGraphicObject*, Rasterization::Ptr>* rasterMap
        );

	private:
        int _cRows;
        int _cCols;
		CRect _buffRect;
		Matrix<Element> _buffer;
        vector<DWORD>   _colors;

    private:
        std::function<void(int xStart, int xEnd)> _segmentProcessingLambda;
	};


}
