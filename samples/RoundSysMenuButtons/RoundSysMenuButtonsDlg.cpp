// RoundSysMenuButtonsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RoundSysMenuButtons.h"
#include "RoundSysMenuButtonsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
CPictureHolder pict,pict1;
long cx=0,cy=0,WD=0,HT=0;
long cx1=0,cy1=0,WD1=0,HT1=0;

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoundSysMenuButtonsDlg dialog

CRoundSysMenuButtonsDlg::CRoundSysMenuButtonsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRoundSysMenuButtonsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRoundSysMenuButtonsDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRoundSysMenuButtonsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRoundSysMenuButtonsDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRoundSysMenuButtonsDlg, CDialog)
	//{{AFX_MSG_MAP(CRoundSysMenuButtonsDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	ON_WM_NCPAINT()
	ON_WM_NCLBUTTONDOWN() 
	ON_WM_NCACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoundSysMenuButtonsDlg message handlers

BOOL CRoundSysMenuButtonsDlg::OnInitDialog()
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
	
	// TODO: Add extra initialization here
	/*
	//ShowWindow(SW_MAXIMIZE);
	HWND hnd=::GetDesktopWindow();
	CDC *dc=CDC::FromHandle(::GetDC(hnd));
	CComBSTR bstr;
	///*
	//bstr="C:\\Sonali.jpg";
	//bstr="C:\\Documents and Settings\\sai\\My Documents\\My Pictures\\Sonali\\sona031b.jpg";//sona018b.jpg";
	//bstr="C:\\Documents and Settings\\sai\\My Documents\\My Pictures\\ASH\\aish1.jpg";
	//bstr="C:\\Documents and Settings\\sai\\My Documents\\My Pictures\\kar\\karisma14Shrunk.jpg";
	bstr="C:\\Documents and Settings\\sai\\My Documents\\My Pictures\\kar\\kar6a.jpg";
	HRESULT hr=::OleLoadPicturePath(bstr,0,0,0,IID_IPicture,(void**)&pict);
	if(SUCCEEDED(hr))
	{
		pict.m_pPict->get_Width(&WD);		
		pict.m_pPict->get_Height(&HT);
		//AfxMessageBox("SUCCEEDED");	
		cx=MulDiv(WD,dc->GetDeviceCaps(LOGPIXELSX), 2540);
		cy=MulDiv(HT,dc->GetDeviceCaps(LOGPIXELSY), 2540);
	}
	else
	{
		AfxMessageBox("failed Sonali");
		exit(0);
	}
	//CAboutDlg *dlg = new CAboutDlg;
	//dlg->Create(IDD_ABOUTBOX);
	//dlg->ShowWindow(SW_SHOW);
	//AfxMessageBox("lalalaal");
	*/
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRoundSysMenuButtonsDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CRoundSysMenuButtonsDlg::OnPaint() 
{
	/*
	CPaintDC dc(this); // device context for painting
	CRect rc,rc1;
	
	GetClientRect(rc);

	
	HRGN hRgnCaption = ::CreateRectRgnIndirect(CRect(40,40,200,100));
	
	HRGN hRgnLeft = ::CreateRectRgnIndirect(CRect(200,150,240,240));
	HRGN hRgnNew=::CreateRectRgnIndirect(&rc);
	
	::CombineRgn(hRgnNew,hRgnLeft,hRgnCaption,RGN_OR);

	//pict.Render(&dc,rc,rc1);
	//::SelectClipRgn(dc.m_hDC,hRgnCaption);
	//::SelectClipRgn(dc.m_hDC,hRgnLeft);
	//::SelectClipRgn(dc.m_hDC,hRgnNew);
	pict.Render(&dc,CRect(0,0,cx,cy),rc1);

	DeleteObject(hRgnCaption);
	DeleteObject(hRgnLeft);
	DeleteObject(hRgnNew);
	*/
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRoundSysMenuButtonsDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


CRect m_rcButMin;
CRect m_rcButClose;
CRect m_rcButMaxRes;
int DownClose=0;
int DownMin=0;
int DownmaxRes=0;
int MaxOrRes=1; //1 means maximized, 0 means restore
int WhereAmI=0;
void CRoundSysMenuButtonsDlg::OnNcPaint()
{
	CRect rc;
	GetWindowRect(rc);

	CRect rb;
	int x=rc.top+22;
	rb=CRect(0,x,rc.right,rc.bottom); //Actually 220
	
	CRect rl;
	rl=CRect(0,x,rc.left+6,rc.bottom);
	
	HRGN hRgnCaption = ::CreateRectRgnIndirect(&rb);
	//HRGN hRgnLeft = ::CreateRectRgnIndirect(rl);
	//HRGN hRgnNew;//=::CreateRectRgnIndirect(&rb);
	
	//::CombineRgn(hRgnNew,hRgnCaption,hRgnLeft,RGN_AND);

	//MSG& msg1 = AfxGetThreadState()->m_lastSentMsg;
	//WPARAM savewp1 = msg1.wParam;	// save original wParam
	//msg1.wParam = (WPARAM)hRgnCaption;	// set new region for DefWindowProc
	//msg1.wParam = (WPARAM)hRgnNew;	// set new region for DefWindowProc
	//msg1.wParam = (WPARAM)hRgnLeft;	// set new region for DefWindowProc

	DefWindowProc(WM_NCPAINT,(WPARAM)hRgnCaption,NULL);
	DeleteObject(hRgnCaption);
	//return;
	
	CWindowDC dc(this);	
	rc.bottom = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME);

	CBrush brYellow;
	brYellow.CreateSolidBrush(RGB(255,255,0));
	dc.SelectObject(brYellow);

	//dc.FillSolidRect(0,0,rc.Width(),22,RGB(255,0,0));// &br);
	//dc.SetBkMode(TRANSPARENT);
	//dc.FillSolidRect(0,0,rc.Width(),22,RGB(255,0,0));// &br);

	CRect closeRect;//=rc;
	///*
	closeRect.left = rc.right - rc.left - 25;
	closeRect.top = 0;//GetSystemMetrics(SM_CYFRAME);
	closeRect.right = rc.right - rc.left - 5;
	closeRect.bottom = 22;//GetSystemMetrics( SM_CYSIZE );
	m_rcButClose=closeRect;
	//*/
	
	// Put the close button on the caption
	
	CRgn rgnClose;
	rgnClose.CreateEllipticRgnIndirect(m_rcButClose);

	
	dc.SelectClipRgn(&rgnClose);
	dc.DrawFrameControl(m_rcButClose,DFC_CAPTION,DFCS_CAPTIONCLOSE);
	dc.SetTextColor(RGB(rand() % 255,0,rand() % 255));
	dc.FloodFill(m_rcButClose.left+2,m_rcButClose.top+5,RGB(0,0,0));
	
///*
	m_rcButMin = m_rcButClose;
	//m_rcButMin.top-=2;
	m_rcButMin.right-=30;
	m_rcButMin.left-=30;
	//m_rcButMin.bottom+=3;

	CRgn rgnMin;
	rgnMin.CreateEllipticRgnIndirect(m_rcButMin);

	
	dc.SelectClipRgn(&rgnMin);
	dc.DrawFrameControl(m_rcButMin,DFC_CAPTION,DFCS_CAPTIONMIN);
	dc.SetTextColor(RGB(rand() % 255,0,rand() % 255));
	dc.FloodFill(m_rcButMin.left+5,m_rcButMin.top+5,RGB(0,0,0));
	//dc.FrameRgn(&rgnMin,&CBrush(RGB(0,0,0)),1,1);


	m_rcButMaxRes=m_rcButMin;
	m_rcButMaxRes.right-=35;
	m_rcButMaxRes.left-=35;
	
	CRgn rgnMaxRes;
	rgnMaxRes.CreateEllipticRgnIndirect(m_rcButMaxRes);

	
	dc.SelectClipRgn(&rgnMaxRes);
	if(MaxOrRes)
	dc.DrawFrameControl(m_rcButMaxRes,DFC_CAPTION,DFCS_CAPTIONMAX);
	else
	dc.DrawFrameControl(m_rcButMaxRes,DFC_CAPTION,DFCS_CAPTIONRESTORE);

	dc.SetTextColor(RGB(rand() % 255,0,rand() % 255));
	dc.FloodFill(m_rcButMaxRes.left+5,m_rcButMaxRes.top+5,RGB(0,0,0));
	dc.FloodFill((m_rcButMaxRes.left+m_rcButMaxRes.right)/2,m_rcButMaxRes.top+8,RGB(0,0,0));


	rb=CRect(0,0,rc.right,22);
	HRGN hRgnCap = ::CreateRectRgnIndirect(&rb);
	HRGN rgnTemp=::CreateRectRgnIndirect(&rb);
	
	
	::CombineRgn(rgnTemp,hRgnCap,rgnClose,RGN_DIFF);	
	::CombineRgn(rgnTemp,rgnTemp,rgnMin,RGN_DIFF);	
	::CombineRgn(rgnTemp,rgnTemp,rgnMaxRes,RGN_DIFF);	
	::SelectClipRgn(dc.m_hDC,rgnTemp);
	dc.FillSolidRect(0,0,rc.Width(),22,RGB(255,0,0));// &br);

	DeleteObject(rgnTemp);
	DeleteObject(hRgnCap);
	DeleteObject(rgnClose);
	DeleteObject(rgnMaxRes);
//*/
//	dc.FrameRgn(&rgnMaxRes,&CBrush(RGB(0,0,0)),1,1);
}

void CRoundSysMenuButtonsDlg::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{

	CWindowDC dc(this);
	CRect rc=m_rcButClose;
	CPoint pt=point;
	// Convert for relative to client area
	ScreenToClient(&pt);
	
	pt.y += GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME);
	pt.x += 5;

	DWORD fl=HitTest(pt);
	
	CBrush brYellow;
	brYellow.CreateSolidBrush(RGB(240,240,0));
	dc.SelectObject(brYellow);
	
	switch(fl)
	{
	case DHT_CLOSE:
	{
		CRgn rgnClose;
		rgnClose.CreateEllipticRgnIndirect(m_rcButClose);

		dc.SelectClipRgn(&rgnClose);
		dc.DrawFrameControl(m_rcButClose,DFC_CAPTION,DFCS_CAPTIONCLOSE|DFCS_PUSHED);
		dc.SetTextColor(RGB(rand() % 255,0,rand() % 255));
		dc.FloodFill(m_rcButClose.left+5,m_rcButClose.top+5,RGB(0,0,0));
		//dc.FrameRgn(&rgnClose,&CBrush(RGB(0,0,0)),1,1);
		DownClose=1;
		WhereAmI=DHT_CLOSE;
		SetCapture();
		break;
	}
	case DHT_MIN:
	{
		CRgn rgnMin;
		rgnMin.CreateEllipticRgnIndirect(m_rcButMin);

		
		dc.SelectClipRgn(&rgnMin);
		dc.DrawFrameControl(m_rcButMin,DFC_CAPTION,DFCS_CAPTIONMIN|DFCS_PUSHED);
		dc.SetTextColor(RGB(rand() % 255,0,rand() % 255));
		dc.FloodFill(m_rcButMin.left+5,m_rcButMin.top+5,RGB(0,0,0));
		//dc.FrameRgn(&rgnMin,&CBrush(RGB(0,0,0)),1,1);
		DownMin=1;
		WhereAmI=DHT_MIN;
		SetCapture();
		break;
	}
	case DHT_MAXRES:
	{
		CRgn rgnMaxRes;
		rgnMaxRes.CreateEllipticRgnIndirect(m_rcButMaxRes);

	
		dc.SelectClipRgn(&rgnMaxRes);
		if(MaxOrRes)
		dc.DrawFrameControl(m_rcButMaxRes,DFC_CAPTION,DFCS_CAPTIONMAX|DFCS_PUSHED);
		else
		dc.DrawFrameControl(m_rcButMaxRes,DFC_CAPTION,DFCS_CAPTIONRESTORE|DFCS_PUSHED);

		dc.SetTextColor(RGB(rand() % 255,0,rand() % 255));
		dc.FloodFill(m_rcButMaxRes.left+2,m_rcButMaxRes.top+5,RGB(0,0,0));
		dc.FloodFill(m_rcButMaxRes.left+5,m_rcButMaxRes.top+11,RGB(0,0,0));
		//dc.FloodFill((m_rcButMaxRes.left+m_rcButMaxRes.right)/2,m_rcButMaxRes.top+8,RGB(0,0,0));
//		dc.FrameRgn(&rgnMaxRes,&CBrush(RGB(0,0,0)),1,1);
		DownmaxRes=1;
		WhereAmI=DHT_MAXRES;
		SetCapture();
		break;
	}
	default:
		Default(); 
		break;
	}
}
void CRoundSysMenuButtonsDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CRgn rgnClose;
	rgnClose.CreateEllipticRgnIndirect(m_rcButClose);
	
	CRgn rgnMin;
	rgnMin.CreateEllipticRgnIndirect(m_rcButMin);
	
	CRgn rgnMaxRes;
	rgnMaxRes.CreateEllipticRgnIndirect(m_rcButMaxRes);

	CWnd*wnd=GetDlgItem(IDC_STAT);
	
	//ScreenToClient(&point);
	point.y += GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME);
	point.x += 5;

	if(rgnClose.PtInRegion(point) && DownClose==1 && nFlags==MK_LBUTTON)
	//if(m_rcButClose.PtInRect(point) && DownClose==1 && nFlags==MK_LBUTTON)
	{
		wnd->SetWindowText("Inside");
	
		CWindowDC dc(this);
		CBrush brYellow;
		brYellow.CreateSolidBrush(RGB(255,255,0));
		dc.SelectObject(brYellow);
		
		dc.SelectClipRgn(&rgnClose);
		dc.DrawFrameControl(m_rcButClose,DFC_CAPTION,DFCS_CAPTIONCLOSE | DFCS_PUSHED);
		dc.SetTextColor(RGB(rand() % 255,0,rand() % 255));
		dc.FloodFill(m_rcButClose.left+5,m_rcButClose.top+5,RGB(0,0,0));
		//dc.FrameRgn(&rgnClose,&CBrush(RGB(0,0,0)),1,1);
		//return;	
	}
	else
		if(DownClose==1 && nFlags==MK_LBUTTON)
	{
		wnd->SetWindowText("Outside");
	
		CWindowDC dc(this);
		CBrush brYellow;
		brYellow.CreateSolidBrush(RGB(255,255,0));
		dc.SelectObject(brYellow);
		
		dc.SelectClipRgn(&rgnClose);
		dc.DrawFrameControl(m_rcButClose,DFC_CAPTION,DFCS_CAPTIONCLOSE);
		dc.SetTextColor(RGB(rand() % 255,0,rand() % 255));
		dc.FloodFill(m_rcButClose.left+7,m_rcButClose.top+5,RGB(0,0,0));
		//dc.FrameRgn(&rgnClose,&CBrush(RGB(0,0,0)),1,1);	
		//CDialog::OnMouseMove(nFlags, point);
		//return;
	}

	if(rgnMin.PtInRegion(point) && DownMin==1 && nFlags==MK_LBUTTON)
	//if(m_rcButClose.PtInRect(point) && DownClose==1 && nFlags==MK_LBUTTON)
	{
		wnd->SetWindowText("Inside");
	
		CWindowDC dc(this);
		CBrush brYellow;
		brYellow.CreateSolidBrush(RGB(255,255,0));
		dc.SelectObject(brYellow);
		
		dc.SelectClipRgn(&rgnMin);
		dc.DrawFrameControl(m_rcButMin,DFC_CAPTION,DFCS_CAPTIONMIN | DFCS_PUSHED);
		dc.SetTextColor(RGB(rand() % 255,0,rand() % 255));
		dc.FloodFill(m_rcButMin.left+7,m_rcButMin.top+5,RGB(0,0,0));
		//dc.FrameRgn(&rgnMin,&CBrush(RGB(0,0,0)),1,1);
		//return;
	}
	else
		if(DownMin==1 && nFlags==MK_LBUTTON)
	{
		wnd->SetWindowText("Outside");
	
		CWindowDC dc(this);
		CBrush brYellow;
		brYellow.CreateSolidBrush(RGB(255,255,0));
		dc.SelectObject(brYellow);
		
		dc.SelectClipRgn(&rgnMin);
		dc.DrawFrameControl(m_rcButMin,DFC_CAPTION,DFCS_CAPTIONMIN);
		dc.SetTextColor(RGB(rand() % 255,0,rand() % 255));
		dc.FloodFill(m_rcButMin.left+5,m_rcButMin.top+5,RGB(0,0,0));
		//dc.FrameRgn(&rgnMin,&CBrush(RGB(0,0,0)),1,1);	
		//CDialog::OnMouseMove(nFlags, point);
		//return;
	}

	if(rgnMaxRes.PtInRegion(point) && DownmaxRes==1 && nFlags==MK_LBUTTON)
	//if(m_rcButClose.PtInRect(point) && DownClose==1 && nFlags==MK_LBUTTON)
	{
		wnd->SetWindowText("Inside");
	
		CWindowDC dc(this);
		CBrush brYellow;
		brYellow.CreateSolidBrush(RGB(255,255,0));
		dc.SelectObject(brYellow);
		
		dc.SelectClipRgn(&rgnMaxRes);

		if(MaxOrRes)
		dc.DrawFrameControl(m_rcButMaxRes,DFC_CAPTION,DFCS_CAPTIONMAX|DFCS_PUSHED);
		else
		dc.DrawFrameControl(m_rcButMaxRes,DFC_CAPTION,DFCS_CAPTIONRESTORE|DFCS_PUSHED);

		dc.SetTextColor(RGB(rand() % 255,0,rand() % 255));
		dc.FloodFill(m_rcButMaxRes.left+5,m_rcButMaxRes.top+5,RGB(0,0,0));
		dc.FloodFill(7+m_rcButMaxRes.left,m_rcButMaxRes.top+11,RGB(0,0,0));
		//dc.FloodFill(7+(m_rcButMaxRes.left+m_rcButMaxRes.right)/2,m_rcButMaxRes.top+7,RGB(0,0,0));
		//dc.FrameRgn(&rgnMaxRes,&CBrush(RGB(0,0,0)),1,1);
		//return;	
	}
	else
		if(DownmaxRes==1 && nFlags==MK_LBUTTON)
	{
		wnd->SetWindowText("Outside");
	
		CWindowDC dc(this);
		CBrush brYellow;
		brYellow.CreateSolidBrush(RGB(255,255,0));
		dc.SelectObject(brYellow);
		
		dc.SelectClipRgn(&rgnMaxRes);
		if(MaxOrRes)
		dc.DrawFrameControl(m_rcButMaxRes,DFC_CAPTION,DFCS_CAPTIONMAX);
		else
		dc.DrawFrameControl(m_rcButMaxRes,DFC_CAPTION,DFCS_CAPTIONRESTORE);

		dc.SetTextColor(RGB(rand() % 255,0,rand() % 255));
		dc.FloodFill(m_rcButMaxRes.left+5,m_rcButMaxRes.top+5,RGB(0,0,0));
		dc.FloodFill((m_rcButMaxRes.left+5+m_rcButMaxRes.right)/2,m_rcButMaxRes.top+8,RGB(0,0,0));
		//dc.FrameRgn(&rgnMaxRes,&CBrush(RGB(0,0,0)),1,1);	
		return;
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CRoundSysMenuButtonsDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CRoundSysMenuButtonsDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWindowDC dc(this);

	CBrush brYellow;
	brYellow.CreateSolidBrush(RGB(255,255,0));
	dc.SelectObject(brYellow);
		
	CRect rc=m_rcButClose;
	CPoint pt=point;

	// Convert for relative to client area

	//if(this != GetCapture())
	//	return;

	//ScreenToClient(&pt);

	pt.y += GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME);
	pt.x += 5;

	DWORD fl=HitTest(pt);
	
	if(DownClose==1 && fl==WhereAmI)
	{
		//SendMessage(WM_NCPAINT);
		//AfxMessageBox("Finally");
		///*
		CRgn rgnClose;
		rgnClose.CreateEllipticRgnIndirect(m_rcButClose);


		dc.SelectClipRgn(&rgnClose);
		dc.DrawFrameControl(m_rcButClose,DFC_CAPTION,DFCS_CAPTIONCLOSE);
		dc.SetTextColor(RGB(rand() % 255,0,rand() % 255));
		dc.FloodFill(m_rcButClose.left+5,m_rcButClose.top+5,RGB(0,0,0));
		//dc.FrameRgn(&rgnClose,&CBrush(RGB(0,0,0)),1,1);
		//*/
		DownClose=0;
		WhereAmI=-1;
		SendMessage(WM_CLOSE);
		//AfxMessageBox("Finally");
		
		//ReleaseCapture();
		//CDialog::OnLButtonUp(nFlags, point);
		
		//return;
	}
	if(DownMin==1 && fl==WhereAmI)
	{
		//AfxMessageBox("Finally");
		CRgn rgnMin;
		rgnMin.CreateEllipticRgnIndirect(m_rcButMin);

		
		dc.SelectClipRgn(&rgnMin);
		dc.DrawFrameControl(m_rcButMin,DFC_CAPTION,DFCS_CAPTIONMIN);
		dc.SetTextColor(RGB(rand() % 255,0,rand() % 255));
		dc.FloodFill(m_rcButMin.left+5,m_rcButMin.top+5,RGB(0,0,0));
		//dc.FrameRgn(&rgnMin,&CBrush(RGB(0,0,0)),1,1);
		DownMin=0;
		WhereAmI=-1;
		ShowWindow(SW_MINIMIZE);
		//ReleaseCapture();
		//CDialog::OnLButtonUp(nFlags, point);
		
		//return;
	}
	if(DownmaxRes==1 && fl==WhereAmI)
	{
		CRgn rgnMaxRes;
		rgnMaxRes.CreateEllipticRgnIndirect(m_rcButMaxRes);

	
		dc.SelectClipRgn(&rgnMaxRes);
		if(MaxOrRes)
		{
			dc.DrawFrameControl(m_rcButMaxRes,DFC_CAPTION,DFCS_CAPTIONMAX);
			MaxOrRes=0;
			ShowWindow(SW_MAXIMIZE);
		}
		else
		{
			dc.DrawFrameControl(m_rcButMaxRes,DFC_CAPTION,DFCS_CAPTIONRESTORE);
			MaxOrRes=1;
			ShowWindow(SW_RESTORE);
		}

		dc.SetTextColor(RGB(rand() % 255,0,rand() % 255));
		dc.FloodFill(m_rcButMaxRes.left+1,m_rcButMaxRes.top+5,RGB(0,0,0));
		dc.FloodFill((m_rcButMaxRes.left+m_rcButMaxRes.right)/2,m_rcButMaxRes.top+8,RGB(0,0,0));
		//dc.FrameRgn(&rgnMaxRes,&CBrush(RGB(0,0,0)),1,1);
		DownmaxRes=0;
		WhereAmI=-1;
		//ReleaseCapture();
		//CDialog::OnLButtonUp(nFlags, point);
		
		//return;	
	}
	
	DownmaxRes=0;
	DownMin=0;
	WhereAmI=-1;
	DownClose=0;
	
	ReleaseCapture();
	CDialog::OnLButtonUp(nFlags, point);
}

void CRoundSysMenuButtonsDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
}
BOOL CRoundSysMenuButtonsDlg::OnNcActivate(BOOL bActive)
{
	return 1;
	 //return FALSE;
}
DWORD CRoundSysMenuButtonsDlg::HitTest(CPoint pt)
{
	// Check for buttons
	CRect rect=m_rcButClose;
	
	if(rect.PtInRect(pt))
		return (DWORD) DHT_CLOSE;
	else
	{
		rect=m_rcButMin;
		if(rect.PtInRect(pt))
			return DHT_MIN;
		else
		{
			rect=m_rcButMaxRes;
			if(rect.PtInRect(pt))
				return DHT_MAXRES;
		}
	}

}

void CRoundSysMenuButtonsDlg::OnOK() 
{
	// TODO: Add extra validation here	
	AfxMessageBox("lalalaal");
	//CDialog::OnOK();
}


/*
Round SysMenu Buttons

I have seen a lot of code on the Internet about round buttons but none on Round SysMenu 
buttons, so here it is, The code alos explains how to do Flicker free painting in the
Caption and throwsw light how to do the same in other non client area.
As usual I expaing a lot.

To handle the painting and message processing on the Caption aread you need to handle 
WM_NCPAINT, WM_LBUTTONUP and the WM_MOUSEMOVE.
First of all I remove the syamenu while desining the Dialog, the see the code how to add 
the WM_NCPAINT, WM_LBUTTONUP and the WM_MOUSEMOVE messages.
//In WM_NCPAINT flicker occurs because 

By default windows paints Blue in the caption area then over that you paint red 
( say you want the caption color to be red ), and when  windows-paints-Blue-over
-that-you-paint-red happens again and again, a flicker is generated.

to avoid that in

when a WM_NCPAINT message is generated, the region to be invalidated will be sent in the 
WPARAM of the MSG structure that contains the WM_NCPAINT.
We intercept the message and give windows a new region to be invalidated and windows will 
faithfully use that region invalidates only that region.
If you look into to code in OnNcPaint(..) function I create a rect region which is the 
whole window minus the caption area (if you return form there then windows will not paint
the caption area at all). Then I call DefWindowProc(WM_NCPAINT,(WPARAM)hRgnCaption,NULL)
to Inavlidate all the window except the caption area.
Then Continuing further I locate the place where to put the close button Create an 
elliptic region int hat area select that region into the Window DC and do the folowing

dc.SelectClipRgn(&rgnClose); // paint only in this region
dc.DrawFrameControl(m_rcButClose,DFC_CAPTION,DFCS_CAPTIONCLOSE); // draw the close button

Go inside and fill the region will the selected brush till you meet the RGB(0,0,0) color 
that happens to be the color of the cross mark in the close button.

dc.FloodFill(m_rcButClose.left+2,m_rcButClose.top+5,RGB(0,0,0));

Repeat the same thing for all the other buttons. 

If you get too excited and paint the captin area with red you will generate a filcker
int the recently drawn buttons, because again and agin you will paint the cation area with 
red, and paint the buttons. SO not the right idea, 
Just take care to remove the recently painted areas the three buttons from the cation area

HRGN hRgnCap = ::CreateRectRgnIndirect(&rb); //region = full caption area
HRGN rgnTemp=::CreateRectRgnIndirect(&rb);


::CombineRgn(rgnTemp,hRgnCap,rgnClose,RGN_DIFF);	//remove the Close button region
::CombineRgn(rgnTemp,rgnTemp,rgnMin,RGN_DIFF);	//remove the Minimize button region
::CombineRgn(rgnTemp,rgnTemp,rgnMaxRes,RGN_DIFF);	//remove the maximize/restore button region
::SelectClipRgn(dc.m_hDC,rgnTemp); //Select this region 
dc.FillSolidRect(0,0,rc.Width(),22,RGB(255,0,0)); // now boldly paint it fully eiht red.

*/