
// 3DTransformations.h : main header file for the 3DTransformations application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CMy3DTransformationsApp:
// See 3DTransformations.cpp for the implementation of this class
//

class CMy3DTransformationsApp : public CWinApp
{
public:
	CMy3DTransformationsApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	afx_msg void OnShowTriangle();
	afx_msg void OnConfigureAdditionalAxis();
	DECLARE_MESSAGE_MAP()
};

extern CMy3DTransformationsApp theApp;
