#include "stdafx.h"
#include "Plot.h"
#include "CoordinateSystem.h"


typedef vector<Plot> Shape;


Plot::~Plot()
{
}


void Plot::Render(const StandardCoordinateSystem* cs, CPaintDC *dc)
{
    #define PLOT_Y_MAX_DIFF 100
    CPoint tmp = dc->GetCurrentPosition();
    int c = _points.size() - 1;
    CPen pen(_penStyle, _penWidth, _penColor);
    CPen *oldPen = (CPen*)dc->SelectObject(&pen);
    dc->MoveTo(cs->ConvertLogicPointToPhys(_points[0]));
    for (int i = 0; i < c; i++)
    {
		dc->MoveTo(cs->ConvertLogicPointToPhys(_points[i]));
		dc->LineTo(cs->ConvertLogicPointToPhys(_points[i + 1]));
    }
    dc->MoveTo(tmp);
    dc->SelectObject(oldPen);
}


LogicPoint* Plot::GetEqualPoint(const Plot &other)
{
    if (_points.size() != other._points.size())
    {
        return nullptr;
    }
    else
    {
        #define DELTA 0.01
        int c = _points.size();
        for (int i = 0; i < c; i++)
        {
            if (abs(_points[i].y - other._points[i].y) <= DELTA)
            {
                return &_points[i];
            }
        }
        return nullptr;
    }
}