// RoundSysMenuButtonsDlg.h : header file
//

#if !defined(AFX_ROUNDSYSMENUBUTTONSDLG_H__AE992BD9_BAF2_47C4_B743_90A623C6C1CC__INCLUDED_)
#define AFX_ROUNDSYSMENUBUTTONSDLG_H__AE992BD9_BAF2_47C4_B743_90A623C6C1CC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRoundSysMenuButtonsDlg dialog

class CRoundSysMenuButtonsDlg : public CDialog
{
// Construction
public:
	CRoundSysMenuButtonsDlg(CWnd* pParent = NULL);	// standard constructor
	DWORD HitTest(CPoint pt);
// Dialog Data
	//{{AFX_DATA(CRoundSysMenuButtonsDlg)
	enum { IDD = IDD_ROUNDSYSMENUBUTTONS_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRoundSysMenuButtonsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRoundSysMenuButtonsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcPaint();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point) ;
	afx_msg BOOL OnNcActivate(BOOL bActive);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROUNDSYSMENUBUTTONSDLG_H__AE992BD9_BAF2_47C4_B743_90A623C6C1CC__INCLUDED_)
