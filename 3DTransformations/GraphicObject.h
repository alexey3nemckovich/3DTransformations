#pragma once
#include <memory>
using namespace std;


namespace cs
{

	class GraphicObject
	{
	public:
		typedef shared_ptr<GraphicObject> Ptr;
		friend class CoordinateSystem;

		GraphicObject(int penStyle = PS_SOLID, int penWidth = 1, COLORREF penColor = RGB(0, 0, 0));
		GraphicObject(const GraphicObject& other);
		~GraphicObject();

	public:
		inline virtual GraphicObject* operator=(const GraphicObject& other);

	protected:
		virtual void Render(const CoordinateSystem*, CDC *dc) const = 0;

	protected:
		int _penStyle;
		int _penWidth;
		COLORREF _penColor;
	};

}
