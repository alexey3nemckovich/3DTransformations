#include "stdafx.h"
#include "3DTransformations.h"
#include "ChildView.h"
#include "CoordinateSystem.h"
#define _USE_MATH_DEFINES
#include <math.h>


CChildView::CChildView()
{

}


CChildView::~CChildView()
{

}


BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), NULL);

	return TRUE;
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_COMMAND(VK_ADD, OnIncreaseAngle)
	ON_COMMAND(VK_SUBTRACT, OnDecreaseAngle)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


afx_msg void CChildView::OnPaint()
{
	static auto coordSystem = cs::CoordinateSystem::GetInstance();

	CPaintDC dc(this);
	coordSystem->Render(&dc);
}


afx_msg void CChildView::OnIncreaseAngle()
{
	static auto coordSystem = cs::CoordinateSystem::GetInstance();
	const double deltaAngel = M_PI / 90;

	if (WorkingMode::RotatingSystemAroundAxis == _mode)
	{
		coordSystem->RotateAroundAxis(_workingAxis, deltaAngel);
	}
	else
	{

	}

	RedrawWindow();
}


afx_msg void CChildView::OnDecreaseAngle()
{
	auto coordSystem = cs::CoordinateSystem::GetInstance();
	const double deltaAngel = - M_PI / 180;

	if (WorkingMode::RotatingSystemAroundAxis == _mode)
	{
		coordSystem->RotateAroundAxis(_workingAxis, deltaAngel);
	}
	else
	{

	}

	RedrawWindow();
}


afx_msg void CChildView::OnZoomIn()
{
	static const double zoomVal = 1.5;
	cs::CoordinateSystem::GetInstance()->Zoom(zoomVal);

	RedrawWindow();
}


afx_msg void CChildView::OnZoomOut()
{
	static const double zoomVal = 0.5;
	cs::CoordinateSystem::GetInstance()->Zoom(zoomVal);

	RedrawWindow();
}


afx_msg void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	UINT ch = tolower(nChar);

	switch (ch)
	{
	case 'z':
		_workingAxis = cs::CoordinateSystem::Axis::Z;
		break;
	case 'y':
		_workingAxis = cs::CoordinateSystem::Axis::Y;
		break;
	case 'x':
		_workingAxis = cs::CoordinateSystem::Axis::X;
		break;
	case 'a':
		_mode = WorkingMode::RotatingSystemAroundAxis;
		break;
	case 's':
		_mode = WorkingMode::RotatingObjectsArountAxis;
		break;
	}

	switch (nChar)
	{
	case VK_ADD:
		OnIncreaseAngle();
		break;
	case VK_SUBTRACT:
		OnDecreaseAngle();
		break;
	case VK_UP:
		OnZoomIn();
		break;
	case VK_DOWN:
		OnZoomOut();
		break;
	}

	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	static CPoint prevPoint;

	if (_moving)
	{
		if (_firstMove)
		{
			prevPoint = point;
			_firstMove = false;
		}
		else
		{
			cs::CoordinateSystem::GetInstance()->Move(point.x - prevPoint.x, point.y - prevPoint.y);
			prevPoint = point;
			RedrawWindow();
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}


void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	_moving = true;
	_firstMove = true;

	CWnd::OnLButtonDown(nFlags, point);
}


void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	_moving = false;
	_firstMove = true;

	CWnd::OnLButtonUp(nFlags, point);
}
