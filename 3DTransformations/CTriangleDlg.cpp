#include "stdafx.h"
#include "CTriangleDlg.h"
#include "CoordinateSystem.h"
#include "SimpleShapesGenerator.h"


CTriangleDlg::CTriangleDlg()
	: CDialogEx(IDD_DIALOG_TRIANGLE)
	, _h(0)
{

}


void CTriangleDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX, IDC_EDIT1, _h);
	DDV_MinMaxUInt(pDX, _h, 1, 10000);
}


BEGIN_MESSAGE_MAP(CTriangleDlg, CDialogEx)
	ON_COMMAND(IDOK, OnOk)
END_MESSAGE_MAP()


afx_msg void CTriangleDlg::OnOk()
{
	if (UpdateData(true))
	{
		auto triangle = cs::SimpleShapesGenerator::GetInstance()->CreateTriangle(_h);
		cs::CoordinateSystem::GetInstance()->AddGraphicObject(triangle);

		GetParent()->RedrawWindow();
		EndDialog(true);
	}
}
