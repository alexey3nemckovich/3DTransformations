#include "stdafx.h"
#include "ZBuffer.h"
#include "CoordinateSystem.h"
#include "LinearGraphicObject.h"
using namespace cs;


ZBuffer* ZBuffer::GetInstance()
{
	static ZBuffer zBuffer;
	return &zBuffer;
}


ZBuffer::ZBuffer()
	: _buffer(0, 0)
{

}


void ZBuffer::Render(CDC* dc)
{
	int cRows = _buffer.GetCountRows();
	int cCols = _buffer.GetCountColumns();

	CPoint point;
	for (int i = 0; i < cRows; i++)
	{
		for (int j = 0; j < cCols; j++)
		{
			point.y = i;
			point.x = j;

			dc->SetPixel(point, _buffer[i][j].color);
		}
	}
}


void ZBuffer::Init(const vector<GraphicObject::Ptr>& objList)
{
	int cObj = objList.size();
	for (int i = 0; i < cObj; i++)
	{
		ProcessObj(objList[i].get());
	}
}


void ZBuffer::Resize(int cRows, int cCols)
{
	if (cRows != _buffer.GetCountRows() || cCols != _buffer.GetCountColumns())
	{
		_buffer = Matrix<Element>(cRows, cCols);
	}
}


ZBuffer::Element* ZBuffer::operator[](int row) const
{
	return _buffer[row];
}


void ZBuffer::ProcessObj(const GraphicObject* obj)
{
	static auto coordSystem = CoordinateSystem::GetInstance();
	
	
}
