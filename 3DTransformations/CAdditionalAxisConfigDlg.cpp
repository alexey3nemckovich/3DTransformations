#include "stdafx.h"
#include "resource.h"
#include "CAdditionalAxisConfigDlg.h"
#include "AdditionalAxis.h"


CAdditionalAxisConfigDlg::CAdditionalAxisConfigDlg()
	: CDialogEx(IDD_ADDITIONAL_AXIS)
	, _p1X(0)
	, _p1Y(0)
	, _p2X(0)
	, _p2Y(0)
	, _p1Z(0)
	, _p2Z(0)
{

}


std::pair<cs::LogicPoint, cs::LogicPoint> CAdditionalAxisConfigDlg::GetAxisPoints()
{
	return std::pair<cs::LogicPoint, cs::LogicPoint>(
		cs::LogicPoint(_p1X, _p1Y, _p1Z),
		cs::LogicPoint(_p2X, _p2Y, _p2Z)
	);
}


void CAdditionalAxisConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_EDIT_POINT1_X, _p1X);
	DDX_Text(pDX, IDC_EDIT_POINT1_Y, _p1Y);
	DDX_Text(pDX, IDC_EDIT_POINT1_Z, _p1Z);
	DDX_Text(pDX, IDC_EDIT_POINT2_X, _p2X);
	DDX_Text(pDX, IDC_EDIT_POINT2_Y, _p2Y);
	DDX_Text(pDX, IDC_EDIT_POINT2_Z, _p2Z);
}


BEGIN_MESSAGE_MAP(CAdditionalAxisConfigDlg, CDialogEx)
	ON_COMMAND(IDOK, OnOk)
END_MESSAGE_MAP()


afx_msg void CAdditionalAxisConfigDlg::OnOk()
{
	if (UpdateData(true))
	{
		AdditionalAxis::GetInstance()->SetPoints(
			LogicPoint(_p1X, _p1Y, _p1Z),
			LogicPoint(_p2X, _p2Y, _p2Z)
		);
	}
}
