#include "stdafx.h"
#include "CoordinateSystem.h"
using namespace cs;


CoordinateSystem *CoordinateSystem::GetInstance()
{
	static CoordinateSystem coordSystem(
		"x", "y", "z", RGB(0, 0, 0), RGB(0, 255, 0)
	);
	return &coordSystem;
}


CoordinateSystem::CoordinateSystem(
	CString xAxisName/* = "x"*/,
	CString yAxisName/* = "y"*/,
	COLORREF axisColor/* = RGB(0, 0, 0)*/,
	COLORREF gridColor/* = RGB(0, 0, 0)*/
)
{

}


CoordinateSystem::CoordinateSystem(
	CString xAxisName/* = "x"*/,
	CString yAxisName/* = "y"*/,
	CString zAxisName/* = "z"*/,
	COLORREF axisColor/* = RGB(0, 0, 0)*/,
	COLORREF gridColor/* = RGB(0, 0, 0)*/
)
{

}


CoordinateSystem::~CoordinateSystem()
{

}


void CoordinateSystem::Render(CPaintDC *dc)
{
    RenderAxes(dc);
	RenderGraphicObjects(dc);

	RenerColorPoints(dc);
	RenderDetectPoints(dc);

	RenderTexts(dc);
}


void CoordinateSystem::Clear()
{
	_texts.clear();
	_objects.clear();
	_points.clear();
	_detectPoints.clear();
}


void CoordinateSystem::AddText(LogicPoint p, CString text)
{
	_texts.push_back(Text(p, text));
}


void CoordinateSystem::AddLogicPoint(LogicPoint point, COLORREF color/* = RGB(0, 0, 0)*/, bool xDetection/* = true*/, bool yDetection/* = true*/, bool zDetection/* = true*/, bool xDetectionLine/* = true*/, bool yDetectionLine/* = true*/, bool zDetectionLine/* = true*/)
{
	bool exist = false;
	if (xDetection || yDetection || zDetection)
	{
		DetectLogicPoint newDetectPoint(point, color, xDetection, yDetection, xDetectionLine, yDetectionLine, zDetectionLine);
		if (find(_detectPoints.begin(), _detectPoints.end(), newDetectPoint) == _detectPoints.end())
		{
			_detectPoints.push_back(newDetectPoint);
		}
		else
		{
			exist = true;
		}
	}
	else
	{
		ColorLogicPoint newPoint(point, color);
		if (find(_points.begin(), _points.end(), newPoint) == _points.end())
		{
			_points.push_back(newPoint);
		}
		else
		{
			exist = true;
		}
	}

	if (!exist)
	{
		CheckAxisBounds(Axis::X, point.x);
		CheckAxisBounds(Axis::Y, point.y);
		CheckAxisBounds(Axis::Z, point.z);
	}
}


void CoordinateSystem::AddLogicPoint(double x, double y, COLORREF cl/* = RGB(0, 0, 0)*/, bool xDetection/* = false*/, bool yDetection/* = false*/, bool xDetectionLine/* = true*/, bool yDetectionLine/* = true*/)
{
	AddLogicPoint(LogicPoint(x, y), cl, xDetection, yDetection, false, xDetectionLine, yDetectionLine, false);
}


void CoordinateSystem::AddLogicPoint(double x, double y, double z, COLORREF cl/* = RGB(0, 0, 0)*/, bool xDetection/* = true*/, bool yDetection/* = true*/, bool zDetection/* = true*/, bool xDetectionLine/* = true*/, bool yDetectionLine/* = true*/, bool zDetectionLine/* = true*/)
{
	AddLogicPoint(LogicPoint(x, y, z), cl, xDetection, yDetection, zDetection, xDetectionLine, yDetectionLine, zDetectionLine);
}


void CoordinateSystem::AddGraphicObject(GraphicsObject* obj, COLORREF color /*= RGB(0, 0, 0)*/)
{
	_objects.push_back(shared_ptr<GraphicsObject>(obj));
}


GraphicsObject* CoordinateSystem::LastGraphicObject()
{
	if (_objects.size())
	{
		return  _objects[_objects.size() - 1].get();
	}

	return nullptr;
}


const vector<GraphicsObject::Ptr>& CoordinateSystem::GetGraphicObejctsList()
{
	return _objects;
}


void CoordinateSystem::EnableGridRendering(bool enable/* = true*/)
{
	_gridRender = enable;
}


void CoordinateSystem::EnableAxisRendering(Axis axis, bool enable/* = true*/)
{
	_axisInfoMap[axis].rendering = enable;
}


void CoordinateSystem::SetAxisRenderingName(Axis axis, CString renderingName)
{
	_axisInfoMap[axis].renderingName = renderingName;
}


void CoordinateSystem::SetAxisRenderingBorders(Axis axis, double minVal, double maxVal)
{
	_axisInfoMap[axis].minRenderingValue = minVal;
	_axisInfoMap[axis].maxRenderingValue = minVal;
}


void CoordinateSystem::SetAxisRenderingDivisions(Axis axis, double value)
{
	_axisInfoMap[axis].divisionValue = value;
}


void CoordinateSystem::SetPhysOrigin(CPoint physOrigin)
{
	_physOrigin = physOrigin;
}


LogicPoint CoordinateSystem::ConvertPhysPointToLogic(const CPoint& point) const
{
	LogicPoint lp;

	return lp;
}


CPoint CoordinateSystem::ConvertLogicPointToPhys(const LogicPoint& lp) const
{
	CPoint p;

	return p;
}


void CoordinateSystem::CheckAxisBounds(Axis ax, double v)
{
	if (v > _axisInfoMap[ax].maxRenderingValue)
	{
		_axisInfoMap[ax].maxRenderingValue = v + 1;
	}
	else if (v < _axisInfoMap[ax].minRenderingValue)
	{
		_axisInfoMap[ax].maxRenderingValue = v - 1;
	}
}


void CoordinateSystem::RenderAxes(CPaintDC *dc)
{
	CPoint tmp = dc->GetCurrentPosition();
	CPen *oldPen = (CPen *)dc->SelectObject(&_axisPen);

	bool renderSystemOrigin = false;
	for each(auto& axisInfo in _axisInfoMap)
	{
		if (axisInfo.second.rendering)
		{
			if (!renderSystemOrigin)
			{
				renderSystemOrigin = true;
			}

			RenderAxis(dc, axisInfo.second);
		}
	}

	dc->MoveTo(tmp);
	dc->SelectObject(oldPen);
}


void CoordinateSystem::RenderGraphicObjects(CPaintDC *dc)
{
	for each(GraphicsObject::Ptr pObj in _objects)
	{
		pObj->Render(this, dc);
	}
}


void CoordinateSystem::RenerColorPoints(CPaintDC *dc)
{
	for each(const ColorLogicPoint& p in _points)
	{
		RenderColorPoint(dc, p);
	}
}


void CoordinateSystem::RenderDetectPoints(CPaintDC *dc)
{
	for each(const DetectLogicPoint& p in _detectPoints)
	{
		RenderDetectPoint(dc, p);
	}
}


void CoordinateSystem::RenderTexts(CPaintDC *dc)
{
	for each(const Text& text in _texts)
	{
		RenderText(dc, text);
	}
}


void CoordinateSystem::RenderAxis(CPaintDC *dc, const AxisInfo&)
{
	//    dc->MoveTo(_physOrigin.x, _physRect.bottom);
	//    dc->LineTo(_physOrigin.x, 0);
	//    RenderArrow(dc, TOP);
	//    CSize size;
	//    size = dc->GetTextExtent(_vertAxisName);
	//    dc->TextOut(
	//        _physOrigin.x + AXIS_INDENT,
	//        size.cy,
	//        _vertAxisName
	//    );
	//
	//    vector<LogicPoint> divisions;
	//    int vertLogicLen = _logicRect.top - _logicRect.bottom;
	//    int division = vertLogicLen / 10;
	//    if (!division) division = 1;
	//    int y = 0;
	//    while (y += division, y < _logicRect.top)
	//    {
	//        divisions.push_back(LogicPoint(0, y));   
	//    }
	//    y = 0;
	//    while (y -= division, y > _logicRect.bottom)
	//    {
	//        divisions.push_back(LogicPoint(0, y));
	//    }
	//
	//    for each(LogicPoint lp in divisions)
	//    {
	//        CPoint p = ConvertLogicPointToPhys(lp);
	//        dc->MoveTo(p.x - DIVISION_HALF_LEN, p.y);
	//        dc->LineTo(p.x + DIVISION_HALF_LEN, p.y);
	//        RenderDivision(dc, Axis::Y, int(lp.y));
	//        if (_gridRender)
	//        {
	//            dc->MoveTo(0, p.y);
	//            CPen *oldPen = (CPen*)dc->SelectObject(&_gridPen);
	//            dc->LineTo(_physRect.right, p.y);
	//            dc->SelectObject(oldPen);
	//        }
	//    }
}


void CoordinateSystem::RenderArrow(CPaintDC *dc, Side type)
{
#define ARROW_LEN 50
#define ARROW_HEIGHT 10
	CPoint tmp = dc->GetCurrentPosition();
	CPoint p1, p2;
	switch (type)
	{
	case RIGHT:case LEFT:
		p1.y = tmp.y - ARROW_HEIGHT;
		p2.y = tmp.y + ARROW_HEIGHT;
		if (RIGHT == type)
		{
			p2.x = p1.x = tmp.x - ARROW_LEN;
		}
		else
		{
			p2.x = p1.x = ARROW_LEN;
		}
		break;
	case TOP: case BOTTOM:
		p1.x = tmp.x - ARROW_HEIGHT;
		p2.x = tmp.x + ARROW_HEIGHT;
		if (TOP == type)
		{
			p2.y = p1.y = ARROW_LEN;
		}
		else
		{
			p2.y = p1.y = tmp.y - ARROW_LEN;
		}
		break;
	}
	dc->LineTo(p1);
	dc->MoveTo(tmp);
	dc->LineTo(p2);
	dc->MoveTo(tmp);
}


void CoordinateSystem::RenderColorPoint(CPaintDC *dc, const ColorLogicPoint &colorPoint)
{
#define POINT_RECT_SZ 5
	CPoint pp = ConvertLogicPointToPhys(colorPoint.logicPoint);
	CBrush pointBrush(colorPoint.color);
	CBrush *oldBrush = (CBrush*)dc->SelectObject(&pointBrush);
	dc->Ellipse(pp.x - POINT_RECT_SZ, pp.y - POINT_RECT_SZ, pp.x + POINT_RECT_SZ, pp.y + POINT_RECT_SZ);
	dc->SelectObject(oldBrush);
}


void CoordinateSystem::RenderDetectPoint(CPaintDC *dc, const DetectLogicPoint &detectPoint)
{
	//CPoint pp = ConvertLogicPointToPhys(detectPoint.logicPoint);
	//CPoint tmp = dc->GetCurrentPosition();
	//CPen detectPen(PS_DASH, 1, RGB(0, 0, 0));
	//CPen *oldPen = (CPen*)dc->SelectObject(&detectPen);
	////horizontal detect line
	//if (detectPoint.horzDetect)
	//{
	//	if (detectPoint.horzDetectLine)
	//	{
	//		dc->MoveTo(pp.x, _physOrigin.y);
	//		dc->LineTo(pp.x, pp.y);
	//	}
	//	RenderDivision(dc, Axis::X, detectPoint.logicPoint.x);
	//}
	////vert detect line
	//if (detectPoint.vertDetect)
	//{
	//	if (detectPoint.vertDetectLine)
	//	{
	//		dc->MoveTo(_physOrigin.x, pp.y);
	//		dc->LineTo(pp.x, pp.y);
	//	}
	//	RenderDivision(dc, Axis::Y, detectPoint.logicPoint.y);
	//}
	//dc->MoveTo(tmp);
	//dc->SelectObject(oldPen);
	//RenderColorPoint(dc, detectPoint);
}


void CoordinateSystem::RenderDivision(CPaintDC *dc, Axis axis, double value)
{
	CString str;
	str.Format("%.1f", value);
	//RenderTextOnAxis(dc, axis, value, str);
}


void CoordinateSystem::RenderText(CPaintDC *dc, const Text& text)
{
	/*int tmp = dc->SetBkMode(TRANSPARENT);
	dc->TextOut(p.x, p.y, text);
	dc->SetBkMode(tmp);*/
}


void CoordinateSystem::RenderText(CPaintDC *dc, const CPoint& physPoint, CString text)
{

}


void CoordinateSystem::RenderTextOnAxis(CPaintDC *dc, Axis axis, double value, CString text)
{
	/*CPoint p;
	if (Axis::Y == axis)
	{
		p.x = ConvertLogicXToPhys(0) + AXIS_INDENT;
		p.y = ConvertLogicYToPhys(value);
	}
	else
	{
		CSize size = dc->GetTextExtent(text);
		p.x = ConvertLogicXToPhys(value);
		p.y = ConvertLogicYToPhys(0) - size.cy - AXIS_INDENT;
	}*/
	//RenderText(dc, p, text);
}
