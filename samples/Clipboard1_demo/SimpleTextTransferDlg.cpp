// SimpleTextTransferDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SimpleTextTransfer.h"
#include "SimpleTextTransferDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CSimpleTextTransferDlg dialog



CSimpleTextTransferDlg::CSimpleTextTransferDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSimpleTextTransferDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimpleTextTransferDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, ID_EDTTOCLIPBOARD, m_edtToClipboard);
  DDX_Control(pDX, ID_EDTFROMCLIPBOARD, m_edtFromClipboard);
}

BEGIN_MESSAGE_MAP(CSimpleTextTransferDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_BTNCOPY, OnBnClickedBtncopy)
	ON_BN_CLICKED(ID_BTNCUT, OnBnClickedBtncut)
	ON_BN_CLICKED(ID_BTNPASTE, OnBnClickedBtnpaste)
  ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()


// CSimpleTextTransferDlg message handlers

BOOL CSimpleTextTransferDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_edtToClipboard.SetWindowText("Sample data to copy to Clipboard");
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSimpleTextTransferDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSimpleTextTransferDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSimpleTextTransferDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSimpleTextTransferDlg::OnBnClickedBtncopy()
{
	if (UpdateData())
  {
    CString strData;
    m_edtToClipboard.GetWindowText(strData);

    // test to see if we can open the clipboard first before
    // wasting any cycles with the memory allocation
	  if (OpenClipboard())
	  {
      // Empty the Clipboard. This also has the effect
      // of allowing Windows to free the memory associated
      // with any data that is in the Clipboard
		  EmptyClipboard();

      // Ok. We have the Clipboard locked and it's empty. 
      // Now let's allocate the global memory for our data.

      // Here I'm simply using the GlobalAlloc function to 
      // allocate a block of data equal to the text in the
      // "to clipboard" edit control plus one character for the
      // terminating null character required when sending
      // ANSI text to the Clipboard.
		  HGLOBAL hClipboardData;
		  hClipboardData = GlobalAlloc(GMEM_DDESHARE, 
		                               strData.GetLength()+1);

		  // Calling GlobalLock returns to me a pointer to the 
		  // data associated with the handle returned from GlobalAlloc
		  char * pchData;
		  pchData = (char*)GlobalLock(hClipboardData);
		  
		  // At this point, all I need to do is use the standard 
		  // C/C++ strcpy function to copy the data from the local 
		  // variable to the global memory.
		  strcpy(pchData, LPCSTR(strData));
		  
		  // Once done, I unlock the memory - remember you don't call
		  // GlobalFree because Windows will free the memory 
		  // automatically when EmptyClipboard is next called.
		  GlobalUnlock(hClipboardData);
		  
		  // Now, set the Clipboard data by specifying that 
		  // ANSI text is being used and passing the handle to
		  // the global memory.
		  SetClipboardData(CF_TEXT,hClipboardData);
		  
		  // Finally, when finished I simply close the Clipboard
		  // which has the effect of unlocking it so that other
		  // applications can examine or modify its contents.
		  CloseClipboard();
	  }
  }
}

void CSimpleTextTransferDlg::OnBnClickedBtncut()
{
	OnBnClickedBtncopy();
	m_edtToClipboard.SetWindowText("");
}

void CSimpleTextTransferDlg::OnBnClickedBtnpaste()
{
 // Test to see if we can open the clipboard first.
 if (OpenClipboard()) 
 {
  if (::IsClipboardFormatAvailable(CF_TEXT)
  || ::IsClipboardFormatAvailable(CF_OEMTEXT))
  {
    // Retrieve the Clipboard data (specifying that 
    // we want ANSI text (via the CF_TEXT value).
    HANDLE hClipboardData = GetClipboardData(CF_TEXT);

    // Call GlobalLock so that to retrieve a pointer
    // to the data associated with the handle returned
    // from GetClipboardData.
    char *pchData = (char*)GlobalLock(hClipboardData);

    // Set a local CString variable to the data
    // and then update the dialog with the Clipboard data
    CString strFromClipboard = pchData;
    m_edtFromClipboard.SetWindowText(strFromClipboard);

    // Unlock the global memory.
    GlobalUnlock(hClipboardData);
  }
  else
  { 
    AfxMessageBox("There is no text (ANSI) data on the Clipboard.");
  }

  // Finally, when finished I simply close the Clipboard
  // which has the effect of unlocking it so that other
  // applications can examine or modify its contents.
  CloseClipboard();
 }
}

void CAboutDlg::OnBnClickedOk()
{
  CWaitCursor wait;

  CString strUrl = "http://www.thecodechannel.com/redirect.asp?u=/&s=vcnb";
  if (32 >= (int)ShellExecute(NULL, "open", strUrl, NULL, NULL, SW_SHOWNORMAL))
  {
    AfxMessageBox("::ShellExecute failed to open this link!");
  }
}

void CSimpleTextTransferDlg::OnBnClickedButton1()
{
  CAboutDlg().DoModal();
}
