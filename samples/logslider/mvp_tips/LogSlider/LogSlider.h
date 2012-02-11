/*****************************************************************************
*           Change Log
*  Date     | Change
*-----------+-----------------------------------------------------------------
* 31-Oct-03 | Created
* 31-Oct-03 | REQ #236: Handle log slider controls better
* 19-Nov-03 | REQ #315: Added way to set linear position needed for context saving
*****************************************************************************/
#if !defined(AFX_LOGSLIDER_H__C433BC64_687C_4410_A89F_14C89F9A4EE6__INCLUDED_)
#define AFX_LOGSLIDER_H__C433BC64_687C_4410_A89F_14C89F9A4EE6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogSlider.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLogSlider window

class CLogSlider : public CSliderCtrl
{
// Construction
public:
        CLogSlider();

// Attributes
public:

// Operations
public:

// Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CLogSlider)
        //}}AFX_VIRTUAL

// Implementation
public:
        virtual ~CLogSlider();
        void SetRange(double low, double high, UINT resolution);
        void GetRange(double & low, double & high);
        BOOL SetPos(double pos);
        double GetPos();

        int GetLinearPos();
        void SetLinearPos(int pos);
        void GetLinearRange(int & low, int & high);


        // Generated message map functions
protected:
        double low; // low end of range
        double high; // high end of range
        //{{AFX_MSG(CLogSlider)
                // NOTE - the ClassWizard will add and remove member functions here.
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGSLIDER_H__C433BC64_687C_4410_A89F_14C89F9A4EE6__INCLUDED_)
