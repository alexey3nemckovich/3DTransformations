#include "stdafx.h"
#include "CoordinateSystem.h"


StandardCoordinateSystem::StandardCoordinateSystem(
    CString horzAxisName,
    CString vertAxisName,
    COLORREF axisColor,
    COLORREF gridColor
)
    :_gridPen(PS_SOLID, 1, gridColor), _axisPen(PS_SOLID, 2, axisColor),
    _horzAxisName(horzAxisName), _vertAxisName(vertAxisName)
{
	_xAxisRender = _yAxisRender = true;
}


StandardCoordinateSystem::~StandardCoordinateSystem()
{

}


StandardCoordinateSystem *StandardCoordinateSystem::GetInstance()
{
    static StandardCoordinateSystem coordSystem(
        "x", "y", RGB(0, 0, 0), RGB(0, 255, 0)
    );
    return &coordSystem;
}


void StandardCoordinateSystem::Render(CPaintDC *dc)
{
    InitSystemParameters(dc);
    RenderAxis(dc);
    int c = _plots.size();
    for (int i = 0; i < c; i++)
    {
        _plots[i].Render(this, dc);
    }
    for each(auto p in _detectPoints)
    {
        RenderDetectPoint(dc, p);
    }
    for each(auto p in _points)
    {
        RenderColorPoint(dc, p);
    }
}


void RenderVertAxis(bool render = true)
{

}


void RenderHorzAxis(bool render = true)
{

}


void StandardCoordinateSystem::Clear()
{
    _gridRender = false;
    _vertAxisName = "y";
    _horzAxisName = "x";
    _logicRect = CRect(0, 0, 0, 0);
    _plots.clear();
    _points.clear();
    _detectPoints.clear();
}


void StandardCoordinateSystem::AddLogicPoint(LogicPoint point, COLORREF cl/* = RGB(0, 0, 0)*/, bool xDetection/* = false*/, bool yDetection/* = false*/, bool xDetectionLine/* = true*/, bool yDetectionLine/* = true*/)
{
    bool exist = false;
    if (detectHorz || detectVert)
    {
        DetectLogicPoint newDetectPoint(point, cl, detectHorz, detectVert, horzDetectLine, vertDetectLine);
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
        ColorLogicPoint newPoint(point, cl);
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
        CheckHorzBounds(point.x);
        CheckVertBounds(point.y);
    }
}


void StandardCoordinateSystem::AddLogicPoint(double x, double y, COLORREF cl/* = RGB(0, 0, 0)*/, bool xDetection/* = false*/, bool yDetection/* = false*/, bool xDetectionLine/* = true*/, bool yDetectionLine/* = true*/)
{
    AddLogicPoint(LogicPoint(x, y), cl, detectHorz, detectVert, horzDetectLine, vertDetectLine);
}


void StandardCoordinateSystem::AddLogicPoint(double x, double y, double z, COLORREF cl/* = RGB(0, 0, 0)*/, bool xDetection/* = false*/, bool yDetection/* = false*/, bool xDetectionLine/* = true*/, bool yDetectionLine/* = true*/)
{

}


CPoint StandardCoordinateSystem::ConvertLogicPointToPhys(const LogicPoint& lp) const
{
    CPoint p;
    p.x = ConvertLogicXToPhys(lp.x);
    p.y = ConvertLogicYToPhys(lp.y);
    return p;
}


LogicPoint StandardCoordinateSystem::ConvertPhysPointToLogic(const CPoint& point) const
{
    LogicPoint lp;
    lp.x = ConvertPhysXToLogic(point.x);
    lp.y = ConvertPhysYToLogic(point.y);
    return lp;
}


int StandardCoordinateSystem::ConvertLogicXToPhys(double x) const
{
    return _physOrigin.x + _horzScale * x;
}


double StandardCoordinateSystem::ConvertPhysXToLogic(double x) const
{
    return (x - _physOrigin.x) / _horzScale;
}


int StandardCoordinateSystem::ConvertLogicYToPhys(double y) const
{
    return _physOrigin.y - _vertScale * y;
}


double StandardCoordinateSystem::ConvertPhysYToLogic(double y) const
{
    return (_physOrigin.y - y) / _vertScale;
}


void StandardCoordinateSystem::RenderDetectPoint(CPaintDC *dc, const DetectLogicPoint &detectPoint)
{
    CPoint pp = ConvertLogicPointToPhys(detectPoint.logicPoint);
    CPoint tmp = dc->GetCurrentPosition();
    CPen detectPen(PS_DASH, 1, RGB(0, 0, 0));
    CPen *oldPen = (CPen*)dc->SelectObject(&detectPen);
    //horizontal detect line
    if (detectPoint.horzDetect)
    {
        if (detectPoint.horzDetectLine)
        {
            dc->MoveTo(pp.x, _physOrigin.y);
            dc->LineTo(pp.x, pp.y);
        }
        RenderDivision(dc, Axis::X, detectPoint.logicPoint.x);
    }
    //vert detect line
    if (detectPoint.vertDetect)
    {
        if (detectPoint.vertDetectLine)
        {
            dc->MoveTo(_physOrigin.x, pp.y);
            dc->LineTo(pp.x, pp.y);
        }
        RenderDivision(dc, Axis::Y, detectPoint.logicPoint.y);
    }
    dc->MoveTo(tmp);
    dc->SelectObject(oldPen);
    RenderColorPoint(dc, detectPoint);
}


void StandardCoordinateSystem::RenderColorPoint(CPaintDC *dc, const ColorLogicPoint &colorPoint)
{
    #define POINT_RECT_SZ 5
    CPoint pp = ConvertLogicPointToPhys(colorPoint.logicPoint);
    CBrush pointBrush(colorPoint.color);
    CBrush *oldBrush = (CBrush*)dc->SelectObject(&pointBrush);
    dc->Ellipse(pp.x - POINT_RECT_SZ, pp.y - POINT_RECT_SZ, pp.x + POINT_RECT_SZ, pp.y + POINT_RECT_SZ);
    dc->SelectObject(oldBrush);
}


void StandardCoordinateSystem::RenderAxis(CPaintDC *dc)
{
	if (_yAxisRender || _xAxisRender)
	{
		CPoint tmp = dc->GetCurrentPosition();
		CPen *oldPen = (CPen *)dc->SelectObject(&_axisPen);
		if (_yAxisRender)
		{
			RenderVertAxis(dc);
		}
		if (_xAxisRender)
		{
			RenderHorzAxis(dc);
		}
		dc->MoveTo(_physOrigin);
		CSize size = dc->GetTextExtent("0");
		dc->TextOut(_physOrigin.x + AXIS_INDENT, _physOrigin.y - size.cy - AXIS_INDENT, "0");
		dc->MoveTo(tmp);
		dc->SelectObject(oldPen);
	}
}


void StandardCoordinateSystem::RenderVertAxis(CPaintDC *dc)
{
    dc->MoveTo(_physOrigin.x, _physRect.bottom);
    dc->LineTo(_physOrigin.x, 0);
    RenderArrow(dc, TOP);
    CSize size;
    size = dc->GetTextExtent(_vertAxisName);
    dc->TextOut(
        _physOrigin.x + AXIS_INDENT,
        size.cy,
        _vertAxisName
    );

    vector<LogicPoint> divisions;
    int vertLogicLen = _logicRect.top - _logicRect.bottom;
    int division = vertLogicLen / 10;
    if (!division) division = 1;
    int y = 0;
    while (y += division, y < _logicRect.top)
    {
        divisions.push_back(LogicPoint(0, y));   
    }
    y = 0;
    while (y -= division, y > _logicRect.bottom)
    {
        divisions.push_back(LogicPoint(0, y));
    }

    for each(LogicPoint lp in divisions)
    {
        CPoint p = ConvertLogicPointToPhys(lp);
        dc->MoveTo(p.x - DIVISION_HALF_LEN, p.y);
        dc->LineTo(p.x + DIVISION_HALF_LEN, p.y);
        RenderDivision(dc, Axis::Y, int(lp.y));
        if (_gridRender)
        {
            dc->MoveTo(0, p.y);
            CPen *oldPen = (CPen*)dc->SelectObject(&_gridPen);
            dc->LineTo(_physRect.right, p.y);
            dc->SelectObject(oldPen);
        }
    }
}


void StandardCoordinateSystem::RenderHorzAxis(CPaintDC *dc)
{
    dc->MoveTo(0, _physOrigin.y);
    dc->LineTo(_physRect.right, _physOrigin.y);
    RenderArrow(dc, RIGHT);
    CSize size;
    size = dc->GetTextExtent(_horzAxisName);
    dc->TextOut(
        _physRect.right - size.cx,
        _physOrigin.y - size.cy,
        _horzAxisName
    );
    
    vector<LogicPoint> divisions;
    int horzLogicLen = _logicRect.right - _logicRect.left;
    int division = horzLogicLen / 10;
    if (!division) division = 1;
    int x = 0;
    while (x+= division, x < _logicRect.right)
    {
        divisions.push_back(LogicPoint(x, 0));
    }
    x = 0;
    while (x-= division, x > _logicRect.left)
    {
        divisions.push_back(LogicPoint(x, 0));
    }

    for each(LogicPoint lp in divisions)
    {
        CPoint p = ConvertLogicPointToPhys(lp);
        dc->MoveTo(p.x, p.y - DIVISION_HALF_LEN);
        dc->LineTo(p.x, p.y + DIVISION_HALF_LEN);
        RenderDivision(dc, Axis::X, int(lp.x));
        if (_gridRender)
        {
            dc->MoveTo(p.x, 0);
            CPen *oldPen = (CPen*)dc->SelectObject(&_gridPen);
            dc->LineTo(p.x, _physRect.bottom);
            dc->SelectObject(oldPen);
        }
    }
}


void StandardCoordinateSystem::RenderDivision(CPaintDC *dc, Axis axis, int value)
{
    CString str;
    str.Format("%d", value);
    RenderTextOnAxis(dc, axis, value, str);
}


void StandardCoordinateSystem::RenderDivision(CPaintDC *dc, Axis axis, double value)
{
    CString str;
    str.Format("%.1f", value);
    RenderTextOnAxis(dc, axis, value, str);
}


void StandardCoordinateSystem::RenderTextOnAxis(CPaintDC *dc, Axis axis, double value, CString text)
{
    CPoint p;
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
    }
    RenderText(dc, p, text);
}


void StandardCoordinateSystem::RenderText(CPaintDC *dc, CPoint p, CString text)
{
    int tmp = dc->SetBkMode(TRANSPARENT);
    dc->TextOut(p.x, p.y, text);
    dc->SetBkMode(tmp);
}


void StandardCoordinateSystem::AddText(LogicPoint p, CString text)
{
    //RenderText(dc, LogicToPhysPoint(p), text);
}


void StandardCoordinateSystem::InitSystemParameters(CPaintDC *dc)
{
    RECT clipBox;
    dc->GetClipBox(&clipBox);
    _physRect = CRect(clipBox);
    if (!_logicRect.right) _logicRect.right = DEFAULT_LRIGHT_BOUND;
    if (!_logicRect.left) _logicRect.left = DEFAULT_LLEFT_BOUND;
    if (!_logicRect.bottom) _logicRect.bottom = DEFAULT_LBOTTOM_BOUND;
    if (!_logicRect.top) _logicRect.top = DEFAULT_LTOP_BOUND;
    _horzScale = (double)(_physRect.right - _physRect.left) / (_logicRect.right - _logicRect.left);
    _vertScale = (double)(_physRect.bottom - _physRect.top) / (_logicRect.top - _logicRect.bottom);
    _physOrigin.x = (-_logicRect.left) * _horzScale;
    _physOrigin.y = (_logicRect.top) * _vertScale;
}


void StandardCoordinateSystem::RenderArrow(CPaintDC *dc, Side type)
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


void StandardCoordinateSystem::CheckHorzBounds(int x)
{
    if (x > _logicRect.right)
    {
        _logicRect.right = x + 1;
    }
    else if (x < _logicRect.left)
    {
        _logicRect.left = x - 1;
    }
}


void StandardCoordinateSystem::CheckVertBounds(int y)
{
    if (y > _logicRect.top)
    {
        _logicRect.top = y + 1;
    }
    else if (y < _logicRect.bottom)
    {
        _logicRect.bottom = y - 1;
    }
}