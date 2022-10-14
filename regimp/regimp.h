/*************************************************
Copyright (c) 2000-2009 by Peica Chen.
MIT License

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*************************************************/

// regimp.h : main header file for the REGIMP application
//

#if !defined(AFX_REGIMP_H__C72C50D2_0C89_4468_94C1_87C44CED16C5__INCLUDED_)
#define AFX_REGIMP_H__C72C50D2_0C89_4468_94C1_87C44CED16C5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "HelpDlg.h"
#include "Registry.h"

/////////////////////////////////////////////////////////////////////////////
// CRegimpApp:
// See regimp.cpp for the implementation of this class
//
struct AppParams
{
	TCHAR * lpszTargetFileName;	// 目标文件
	BOOL fSilenceMode;	// 为TRUE时将不打开GUI,而真接使用命令行参数导入
	BOOL fBackupRegistry; // 指定是否在导入注册表后保存入Flash
};

class CRegimpApp : public CWinApp
{
public:
	VOID ProcessParam(TCHAR * lpCmdLine);
	CRegimpApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegimpApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRegimpApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()		

private:
	AppParams m_AppParams;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REGIMP_H__C72C50D2_0C89_4468_94C1_87C44CED16C5__INCLUDED_)
