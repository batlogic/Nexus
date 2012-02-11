// SimpleTextTransferDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CSimpleTextTransferDlg dialog
class CSimpleTextTransferDlg : public CDialog
{
// Construction
public:
	CSimpleTextTransferDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SIMPLETEXTTRANSFER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtncopy();
	afx_msg void OnBnClickedBtncut();
	afx_msg void OnBnClickedBtnpaste();
  CEdit m_edtToClipboard;
  CEdit m_edtFromClipboard;
  afx_msg void OnBnClickedButton1();
};
