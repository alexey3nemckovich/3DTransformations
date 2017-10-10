#pragma once
using namespace std;


namespace cs
{


	class CoordinateSystem;
	class RasterizableGraphicObject;
	class GraphicObject
	{
	public:
		typedef shared_ptr<GraphicObject> Ptr;
		
		GraphicObject(int penStyle = PS_SOLID, int penWidth = 1, COLORREF penColor = RGB(0, 0, 0));
		GraphicObject(const GraphicObject& other);
    GraphicObject(GraphicObject&& o) = default;
    virtual ~GraphicObject();

	public:
		virtual void Render(const CoordinateSystem*, CDC *dc) const = 0;
		virtual vector<const RasterizableGraphicObject*> GetRasterizationPrimitives() const = 0;

	public:
		inline virtual GraphicObject* operator=(const GraphicObject& other);
    GraphicObject& operator=(GraphicObject&& other) = default;
	protected:
		int _penStyle;
		int _penWidth;
		COLORREF _penColor;
	};


}
