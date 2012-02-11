// SimpleTextTransfer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SimpleTextTransfer.h"
#include "SimpleTextTransferDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSimpleTextTransferApp

BEGIN_MESSAGE_MAP(CSimpleTextTransferApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSimpleTextTransferApp construction

CSimpleTextTransferApp::CSimpleTextTransferApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CSimpleTextTransferApp object

CSimpleTextTransferApp theApp;


// CSimpleTextTransferApp initialization

BOOL CSimpleTextTransferApp::InitInstance()
{
	CWinApp::InitInstance();

	AfxEnableControlContainer();


	CSimpleTextTransferDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
