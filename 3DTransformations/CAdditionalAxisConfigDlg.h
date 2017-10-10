#pragma once

#include <utility>

namespace cs
{
  struct LogicPoint;
}
class CAdditionalAxisConfigDlg
	: public CDialogEx
{
public:
	CAdditionalAxisConfigDlg();
	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADDITIONAL_AXIS};
#endif

public:
	std::pair<cs::LogicPoint, cs::LogicPoint> GetAxisPoints();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	afx_msg void OnOk();
	DECLARE_MESSAGE_MAP()

private:
	int _p1X;
	int _p1Y;
	int _p1Z;
	int _p2X;
	int _p2Y;
	int _p2Z;
};
