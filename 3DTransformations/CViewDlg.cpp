#include "stdafx.h"
#include "CViewDlg.h"
#include "CoordinateSystem.h"

CViewDlg::Data CViewDlg::data{};

CViewDlg::CViewDlg()
  : CDialogEx(IDD_DIALOG_VIEW)
{
}

void CViewDlg::bindField(CDataExchange* pDX, int idcName, double& bindVal)
{
  DDX_Text(pDX, idcName, bindVal);
  DDV_MinMaxDouble(pDX, bindVal, -100, 100);
}

void CViewDlg::DoDataExchange(CDataExchange* pDX)
{
  bindField(pDX, IDC_EDIT_EYE_X, data.eye[0]);
  bindField(pDX, IDC_EDIT_EYE_Y, data.eye[1]);
  bindField(pDX, IDC_EDIT_EYE_Z, data.eye[2]);

  bindField(pDX, IDC_EDIT_UP_X, data.up[0]);
  bindField(pDX, IDC_EDIT_UP_Y, data.up[1]);
  bindField(pDX, IDC_EDIT_UP_Z, data.up[2]);
}


BEGIN_MESSAGE_MAP(CViewDlg, CDialogEx)
  ON_COMMAND(IDOK, OnOk)
END_MESSAGE_MAP()


afx_msg void CViewDlg::OnOk()
{
  if (UpdateData(true))
  {
    cs::CoordinateSystem::GetInstance()->changeView(data.eye[0], data.eye[1], data.eye[2], data.up[0], data.up[1], data.up[2]);

    GetParent()->RedrawWindow();
    EndDialog(true);
  }
}
