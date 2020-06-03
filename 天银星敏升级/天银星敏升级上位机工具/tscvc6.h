// tscvc6.h : main header file for the TSCVC6 application
//

#if !defined(AFX_TSCVC6_H__FF1A932C_77D3_4DD2_B19C_20A1219E3269__INCLUDED_)
#define AFX_TSCVC6_H__FF1A932C_77D3_4DD2_B19C_20A1219E3269__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTscvc6App:
// See tscvc6.cpp for the implementation of this class
//

class CTscvc6App : public CWinApp
{
public:
	CTscvc6App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTscvc6App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTscvc6App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TSCVC6_H__FF1A932C_77D3_4DD2_B19C_20A1219E3269__INCLUDED_)
