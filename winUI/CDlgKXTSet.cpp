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

#include "CDlgKXTSet.h"

#include "Resource.h"

#include "USystemFunc.h"

CDlgKXTSet::CDlgKXTSet(HINSTANCE hInst, HWND hParent)
	: CDlgBase (hInst, hParent)
	, m_pBuy01 (NULL)
{
}

CDlgKXTSet::~CDlgKXTSet(void)
{
	QC_DEL_P (m_pBuy01);
}

int CDlgKXTSet::CreateDlg (void)
{
	CDlgBase::CreateDlg ();
	m_hDlg = CreateDialog (m_hInst, MAKEINTRESOURCE(IDD_DIALOG_KXT_SET), m_hParent, baseDlgProc);
	if (m_hDlg == NULL)
		return -1;
	SetWindowLong (m_hDlg, GWL_USERDATA, (LONG)this);
	//m_hBrushBG = CreateSolidBrush (MSC_GRAY_2);

	InitParam ();
	OnResize ();

	return 0;
}

bool CDlgKXTSet::OnKeyUp (WPARAM wParam, LPARAM lParam)
{
	return true;
}

LRESULT CDlgKXTSet::OnResize (void)
{
	RECT rcWnd;
	GetWindowRect (m_hParent, &rcWnd);
	RECT rcDlg;
	GetWindowRect (m_hDlg, &rcDlg);
	if ((rcDlg.bottom - rcDlg.top) < 200)
		SetWindowPos (m_hDlg, NULL, rcWnd.right-(rcDlg.right-rcDlg.left), rcWnd.bottom- (rcDlg.bottom-rcDlg.top), 0, 0, SWP_NOSIZE);
	else
		CenterDlg ();

	return S_OK;
}

void CDlgKXTSet::InitParam (void)
{
	int		i = 0;
	char *	szYear[] = {"2000", "2001", "2002", "2003", "2004", "2005", "2006", "2007", "2008", "2009", "2010", "2011", "2012", "2013", "2014", "2015", "2016", "2017"};
	for (i = 0; i < 18; i++)
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_YEAR), CB_ADDSTRING, NULL, (LPARAM)szYear[i]);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_YEAR), CB_SETCURSEL, 16, NULL);

	char szMonth[12];
	for (i = 0; i < 12; i++)
	{
		sprintf (szMonth, "%d", i + 1);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_MONTH), CB_ADDSTRING, NULL, (LPARAM)szMonth);
	}
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_MONTH), CB_SETCURSEL, 2, NULL);
	char szDay[12];
	for (i = 0; i < 31; i++)
	{
		sprintf (szDay, "%d", i + 1);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_DAY), CB_ADDSTRING, NULL, (LPARAM)szDay);
	}
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_DAY), CB_SETCURSEL, 5, NULL);

	SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_1DAYS), "5");
	SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_1MIN), "1");
	SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_1MAX), "5");

	SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_2MAX), "30");

	SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_3MAX), "50");
	SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_3DAYS), "365");

}

INT_PTR CDlgKXTSet::OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
		OnResize ();
		break;

	case WM_KEYDOWN:
		OnKeyUp (wParam, lParam);
		break;

	case WM_COMMAND:
	{
		int wmId    = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDOK:
		case IDCANCEL:
			SendMessage (hDlg, WM_SYSCOMMAND, SC_MINIMIZE, 0);
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
