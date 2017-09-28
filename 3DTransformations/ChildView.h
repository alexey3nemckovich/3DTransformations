#pragma once
#include "CoordinateSystem.h"


class CChildView : public CWnd
{
public:
	CChildView();
	virtual ~CChildView();

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	afx_msg void OnPaint();
	afx_msg void OnIncreaseAngle();
	afx_msg void OnDecreaseAngle();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	enum class WorkingMode {
		RotatingSystemAroundAxis,
		RotatingObjectsArountAdditionalAxis
	};

	WorkingMode _mode = WorkingMode::RotatingSystemAroundAxis;
	cs::CoordinateAxisName _workingAxis;

	bool _moving = false;
	bool _firstMove = true;
};
