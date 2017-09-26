#pragma once
#include <memory>
#include <vector>
using namespace std;


namespace cs
{

	class GraphicObject
	{
	public:
		typedef shared_ptr<GraphicObject> Ptr;
		typedef struct
		{
			CPoint point;
			double zValue;
			COLORREF color;
		}RasterizationUnit;
		friend class CoordinateSystem;

		GraphicObject(int penStyle = PS_SOLID, int penWidth = 1, COLORREF penColor = RGB(0, 0, 0));
		GraphicObject(const GraphicObject& other);
		~GraphicObject();

	public:
		const vector<RasterizationUnit>& GetRasterization() const;

	public:
		inline virtual GraphicObject* operator=(const GraphicObject& other);

	protected:
		virtual void Render(const CoordinateSystem*, CDC *dc) const = 0;
		virtual void Rasterize() const = 0;

	protected:
		int _penStyle;
		int _penWidth;
		COLORREF _penColor;
		mutable bool _rasterized;
		mutable vector<GraphicObject::RasterizationUnit> _rasterizaion;
	};

}
