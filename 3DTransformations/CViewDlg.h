#pragma once
#include "resource.h"


class CViewDlg
  : public CDialogEx
{
public:
  struct Data
  {
    double eye[3]{};
    double up[3]{};
  };

  CViewDlg();

  // Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_DIALOG_VIEW};
#endif

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  afx_msg void OnOk();
  DECLARE_MESSAGE_MAP()
public:
  static Data data;
private:
  void bindField(CDataExchange* pDX, int idcName, double& bindVal);
};
