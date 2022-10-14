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

// regimpDlg.cpp : implementation file
//

#include "stdafx.h"
#include "regimp.h"
#include "regimpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegimpDlg dialog

CRegimpDlg::CRegimpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegimpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegimpDlg)
	m_filePath = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRegimpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegimpDlg)
	DDX_Text(pDX, IDC_EDIT1, m_filePath);
	DDV_MaxChars(pDX, m_filePath, 256);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRegimpDlg, CDialog)
	//{{AFX_MSG_MAP(CRegimpDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegimpDlg message handlers

BOOL CRegimpDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	CenterWindow(GetDesktopWindow());	// center to the hpc screen
	
	UpdateData();
	if (m_filePath.IsEmpty()) {	
		m_filePath = TEXT("\\registry.ini");
	}
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}



void CRegimpDlg::OnButton1() 
{	
	char filePath[256] = {0};
	wcstombs(filePath, m_filePath.GetBuffer(256), 255);
	
	if (CRegistry::ImportRegistry(filePath)) {
		MessageBox(TEXT("Finish Importing."));
	} else {
		MessageBox(TEXT("Fail Importing."));
	}
}

void CRegimpDlg::OnButton2() 
{	
	CFileDialog dialog(TRUE, NULL, NULL, 0, NULL, this);
	if (IDOK == dialog.DoModal())
	{
		UpdateData();
		m_filePath = dialog.GetPathName();
		UpdateData(FALSE);			
	}
}

void CRegimpDlg::OnButton3() 
{
	if (CRegistry::FlushRegistry(0)) {
		MessageBox(TEXT("Save Registry Successful!"));
	} else {
		MessageBox(TEXT("Save Registry Failure!"));
	}
}
