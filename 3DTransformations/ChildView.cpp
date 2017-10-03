#include "stdafx.h"
#include "3DTransformations.h"
#include "ChildView.h"
#include "CoordinateSystem.h"
#include "AdditionalAxis.h"
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
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


afx_msg void CChildView::OnPaint()
{
	static auto coordSystem = cs::CoordinateSystem::GetInstance();

	CPaintDC dc(this);
	coordSystem->Render(&dc, this);
}


afx_msg void CChildView::OnSetStdRenderingAlg()
{
	cs::CoordinateSystem::GetInstance()->SetRenderingAlgorithm(cs::CoordinateSystem::RenderingAlgorithm::STANDARD);
}


afx_msg void CChildView::OnSetZBufferRenderingAlg()
{
	cs::CoordinateSystem::GetInstance()->SetRenderingAlgorithm(cs::CoordinateSystem::RenderingAlgorithm::ZBUFFER);
}


afx_msg void CChildView::OnEnableInvisibleLines()
{
	_invisibleLines = !_invisibleLines;
	cs::CoordinateSystem::GetInstance()->EnableInvisibleLinesAsDash(_invisibleLines);
}


afx_msg void CChildView::OnIncreaseAngle()
{
	OnAction(true);
}


afx_msg void CChildView::OnDecreaseAngle()
{
	OnAction(false);
}


afx_msg void CChildView::OnAction(bool increase)
{
	static auto coordSystem = cs::CoordinateSystem::GetInstance();
	static auto additionalAxis = AdditionalAxis::GetInstance();

	double delta = 0;
	if (_mode == WorkingMode::RotatingObjectsArountAdditionalAxis || _mode == WorkingMode::RotatingSystemAroundAxis)
	{
		delta = M_PI / 45;
	}
	else
	{
		delta = 10;
	}

	if (!increase)
	{
		delta *= -1;
	}

	switch (_mode)
	{
	case WorkingMode::RotatingSystemAroundAxis:
		coordSystem->RotateAroundAxis(_workingAxis, delta);
		break;
	case WorkingMode::RotatingAxis:
		additionalAxis->Move(delta);
		break;
	case WorkingMode::RotatingObjectsArountAdditionalAxis:
		auto points = additionalAxis->GetPoints();
		coordSystem->RotateAroundAxis(pair<LogicPoint, LogicPoint>(points[0], points[1]), delta);
		break;
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
	case 'q':
		OnSetStdRenderingAlg();
		break;
	case 'w':
		OnSetZBufferRenderingAlg();
		break;
	case 'e':
		OnEnableInvisibleLines();
		break;
	case 'z':
		_workingAxis = cs::CoordinateAxisName::Z;
		break;
	case 'y':
		_workingAxis = cs::CoordinateAxisName::Y;
		break;
	case 'x':
		_workingAxis = cs::CoordinateAxisName::X;
		break;
	case 'a':
		_mode = WorkingMode::RotatingSystemAroundAxis;
		break;
	case 's':
		_mode = WorkingMode::RotatingObjectsArountAdditionalAxis;
		break;
	case 'c':
		_mode = WorkingMode::RotatingAxis;
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
			cs::CoordinateSystem::GetInstance()->MoveOriginPhysPoint(point.x - prevPoint.x, point.y - prevPoint.y);
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


BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
