/*******************************************************************************
	File:		CVideoRender.cpp

	Contains:	file info dialog implement code

	Written by:	Fenger King

	Change History (most recent first):
	2013-04-01		Fenger			Create file

*******************************************************************************/
#include "windows.h"
#include "commctrl.h"
#include "qcErr.h"
#include "qcStock.h"
#include "CFileIO.h"

#include "CDlgStockSelect.h"
#include "CStockFileHist.h"

#include "Resource.h"

#include "USystemFunc.h"

CDlgStockSelect::CDlgStockSelect(HINSTANCE hInst, HWND hParent)
	: CDlgBase (hInst, hParent)
	, m_pWndKXT (NULL)
	, m_nWidth (120)
{

}

CDlgStockSelect::~CDlgStockSelect(void)
{

}

int CDlgStockSelect::CreateDlg (void)
{
	CDlgBase::CreateDlg ();
	m_hDlg = CreateDialog (m_hInst, MAKEINTRESOURCE(IDD_DIALOG_MYSELECT), m_hParent, baseDlgProc);
	if (m_hDlg == NULL)
		return -1;
	SetWindowLong (m_hDlg, GWL_USERDATA, (LONG)this);

	OnResize ();
	InvalidateRect (m_hDlg, NULL, TRUE);

	return 0;
}

int CDlgStockSelect::OpenSelectFile (void)
{
	char				szFile[256];
	DWORD				dwID = 0;
	OPENFILENAME		ofn;
	memset (szFile, 0, sizeof (szFile));
	memset( &(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = m_hDlg;
	ofn.lpstrFilter = TEXT("Stock File (*.txt)\0*.txt\0");	
	if (_tcsstr (szFile, _T(":/")) != NULL)
		_tcscpy (szFile, _T("*.txt"));
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = TEXT("Open Stock File");
	ofn.Flags = OFN_EXPLORER;
			
	if (!GetOpenFileName(&ofn))
		return QC_ERR_FAILED;	


	return QC_ERR_NONE;
}


int CDlgStockSelect::OnInitDlg (void)
{
	return QC_ERR_NONE;
}

LRESULT CDlgStockSelect::OnResize (void)
{
	RECT rcWnd;
	GetClientRect (m_hParent, &rcWnd);

	SetWindowPos (m_hDlg, NULL, rcWnd.right - m_nWidth, rcWnd.top, m_nWidth, rcWnd.bottom, 0);

	return QC_ERR_NONE;
}

INT_PTR CDlgStockSelect::OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		OnInitDlg ();
		break;

	case WM_COMMAND:
	{
		int wmId    = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDC_BUTTON_FILE:
			OpenSelectFile ();
			break;

		default:
			break;
		}
		break;
	}
	default:
		break;
	}
	return FALSE;
}
