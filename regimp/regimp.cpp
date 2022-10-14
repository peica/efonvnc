// regimp.cpp : Defines the class behaviors for the application.
//

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

#include "stdafx.h"
#include "regimp.h"
#include "regimpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegimpApp

BEGIN_MESSAGE_MAP(CRegimpApp, CWinApp)
	//{{AFX_MSG_MAP(CRegimpApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegimpApp construction

CRegimpApp::CRegimpApp()
	: CWinApp()
{
	m_AppParams.lpszTargetFileName = NULL;
	m_AppParams.fSilenceMode = FALSE;	
	m_AppParams.fBackupRegistry = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRegimpApp object

CRegimpApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRegimpApp initialization

BOOL CRegimpApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	ProcessParam(m_lpCmdLine);

	if (m_AppParams.fSilenceMode) {
		char filePath[256] = {0};

		if (m_AppParams.lpszTargetFileName == NULL) return FALSE;

		wcstombs(filePath, m_AppParams.lpszTargetFileName, 255);
		CRegistry::ImportRegistry(filePath);

		if (m_AppParams.fBackupRegistry) {
			CRegistry::FlushRegistry(0);
		}

		return FALSE;
	}

	CRegimpDlg dlg;
	m_pMainWnd = &dlg;

	if (m_AppParams.lpszTargetFileName) {	
		dlg.m_filePath = m_AppParams.lpszTargetFileName;
	}

	int nResponse = dlg.DoModal();
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

VOID CRegimpApp::ProcessParam(TCHAR * lpCmdLine)
{
	UINT nCmdLen;
	if (lpCmdLine == NULL || * lpCmdLine == 0)
	{
		return;
	}

	nCmdLen = _tcslen(lpCmdLine);
	for (UINT iCmd=0; iCmd<nCmdLen; iCmd++)
	{
		if (lpCmdLine[iCmd]!='/') {
			continue;
		}

		iCmd++;
		switch (lpCmdLine[iCmd]) {
		case 'f':
		case 'F':
			{
			UINT iEnd = iCmd;;

			while(lpCmdLine[iCmd] != 0 && lpCmdLine[iCmd] != ':' && lpCmdLine[iCmd] != ' ') {iCmd++;} iCmd++;			
			while(lpCmdLine[iEnd] != 0 && lpCmdLine[iEnd] != ' ') iEnd++;			

			lpCmdLine[iEnd] = 0;
			m_AppParams.lpszTargetFileName = _tcsdup(&lpCmdLine[iCmd]);
			}
			break;
		case 's':
		case 'S':
			m_AppParams.fSilenceMode = TRUE;
			break;
		case 'b':
		case 'B':
			m_AppParams.fBackupRegistry = TRUE;
			break;
		case 'h':
		case 'H':
		case '?':
		default:
			CHelpDlg dlg;
			dlg.DoModal();
		}

	}
}

int CRegimpApp::ExitInstance() 
{
	if (m_AppParams.lpszTargetFileName) {
		free(m_AppParams.lpszTargetFileName);
	}

	return CWinApp::ExitInstance();
}
