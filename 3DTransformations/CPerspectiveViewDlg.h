#pragma once
#include "resource.h"


class CPerspectiveViewDlg
  : public CDialogEx
{
public:
  struct Data
  {
    double fovy{};
    double p{};
    double sigma{};
    double fi{};
    double d{};
  };

  CPerspectiveViewDlg();

  // Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_DIALOG_PERSPECTIVE_VIEW};
#endif

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  afx_msg void OnOk();
  DECLARE_MESSAGE_MAP()
public:
  static Data data;
private:
  void bindRangeField(CDataExchange* pDX, int idcName, double& bindVal);
  void bindAngleField(CDataExchange* pDX, int idcName, double& bindVal);
};
