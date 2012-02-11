// RoundSysMenuButtons.h : main header file for the ROUNDSYSMENUBUTTONS application
//

#if !defined(AFX_ROUNDSYSMENUBUTTONS_H__DDBDD808_7C03_47CF_B5C8_43FAF24EF708__INCLUDED_)
#define AFX_ROUNDSYSMENUBUTTONS_H__DDBDD808_7C03_47CF_B5C8_43FAF24EF708__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRoundSysMenuButtonsApp:
// See RoundSysMenuButtons.cpp for the implementation of this class
//

class CRoundSysMenuButtonsApp : public CWinApp
{
public:
	CRoundSysMenuButtonsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRoundSysMenuButtonsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRoundSysMenuButtonsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROUNDSYSMENUBUTTONS_H__DDBDD808_7C03_47CF_B5C8_43FAF24EF708__INCLUDED_)
