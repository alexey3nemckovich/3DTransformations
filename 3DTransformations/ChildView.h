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
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP()

private:
	enum class WorkingMode {
		RotatingSystemAroundAxis,
		RotatingObjectsArountAxis
	};

	WorkingMode _mode = WorkingMode::RotatingSystemAroundAxis;
	cs::CoordinateSystem::Axis _workingAxis;
};
