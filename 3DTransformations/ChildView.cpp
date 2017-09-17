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
	static auto coordSystem = cs::CoordinateSystem::GetInstance();
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
	default:
		{
			if (nChar == VK_ADD)
			{
				OnIncreaseAngle();
			}
			if (nChar == VK_SUBTRACT)
			{
				OnDecreaseAngle();
			}
		}
		break;
	}

	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}
