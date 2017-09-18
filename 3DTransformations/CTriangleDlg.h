#pragma once
#include "resource.h"


class CTriangleDlg
	: public CDialogEx
{
public:
	CTriangleDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TRIANGLE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	afx_msg void OnOk();
	DECLARE_MESSAGE_MAP()
public:
	UINT _h;
};
