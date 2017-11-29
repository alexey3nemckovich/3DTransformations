#include "stdafx.h"
#include "CPerspectiveViewDlg.h"
#include "CoordinateSystem.h"


CPerspectiveViewDlg::Data CPerspectiveViewDlg::data{};

CPerspectiveViewDlg::CPerspectiveViewDlg()
  : CDialogEx(IDD_DIALOG_PERSPECTIVE_VIEW)
{
}

void CPerspectiveViewDlg::bindRangeField(CDataExchange* pDX, int idcName, double& bindVal)
{
  DDX_Text(pDX, idcName, bindVal);
  DDV_MinMaxDouble(pDX, bindVal, -100, 100);
}

void CPerspectiveViewDlg::bindAngleField(CDataExchange * pDX, int idcName, double & bindVal)
{
  DDX_Text(pDX, idcName, bindVal);
  DDV_MinMaxDouble(pDX, bindVal, 0, 359);
}

void CPerspectiveViewDlg::DoDataExchange(CDataExchange* pDX)
{
  bindRangeField(pDX, IDC_EDIT_P, data.p);
  bindRangeField(pDX, IDC_EDIT_D, data.d);
  bindAngleField(pDX, IDC_EDIT_FOVY, data.fovy);
  bindAngleField(pDX, IDC_EDIT_SIGMA, data.sigma);
  bindAngleField(pDX, IDC_EDIT_FI, data.fi);
}


BEGIN_MESSAGE_MAP(CPerspectiveViewDlg, CDialogEx)
  ON_COMMAND(IDOK, OnOk)
END_MESSAGE_MAP()


afx_msg void CPerspectiveViewDlg::OnOk()
{
  if (UpdateData(true))
  {
    cs::CoordinateSystem::GetInstance()->changePerspectiveView(data.fovy, data.d, data.p, data.sigma, data.fi);

    GetParent()->RedrawWindow();
    EndDialog(true);
  }
}
