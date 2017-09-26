#pragma once
#include "PrimitiveTypes.h"
#include "PrimitiveTypesTemplates.h"
#include "GraphicObject.h"


namespace cs
{


	class ZBuffer
	{
	public:
		typedef struct
		{
			double zValue;
			COLORREF color;
		}Element;

		static ZBuffer* GetInstance();

	public:
		ZBuffer();

	public:
		void Render(CDC*);

	public:
		void Init(const vector<GraphicObject::Ptr>& objList);
		void Resize(int cRows, int cCols);

	public:
		Element* operator[](int row) const;

	private:
		void ProcessObj(const GraphicObject* obj);

	private:
		Matrix<Element> _buffer;
	};


}
