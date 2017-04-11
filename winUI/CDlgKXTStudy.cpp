/*******************************************************************************
	File:		CVideoRender.cpp

	Contains:	file info dialog implement code

	Written by:	Fenger King

	Change History (most recent first):
	2013-04-01		Fenger			Create file

*******************************************************************************/
#include "windows.h"
#include "commctrl.h"
#include "qcStock.h"

#include "CStockRTList.h"
#include "CStockFileHist.h"

#include "CDlgKXTStudy.h"

#include "CRegMng.h"
#include "Resource.h"

#include "USystemFunc.h"


CDlgKXTStudy::CDlgKXTStudy(HINSTANCE hInst, HWND hParent)
	: m_hInst (hInst)
	, m_hParent (hParent)
	, m_hDlg (NULL)
	, m_pWndKXTInfo1 (NULL)
	, m_pWndKXTInfo2 (NULL)
{
	memset (m_szKeyCode, 0, sizeof (m_szKeyCode));
}

CDlgKXTStudy::~CDlgKXTStudy(void)
{
	char szCode[32];
	GetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_CODE1), szCode, sizeof (szCode));
	CRegMng::g_pRegMng->SetTextValue ("KXTStudyCode", szCode);
	QC_DEL_P (m_pWndKXTInfo1);
	QC_DEL_P (m_pWndKXTInfo2);
	if (m_hDlg != NULL)
		DestroyWindow (m_hDlg);
}

bool CDlgKXTStudy::OnDateChange (WPARAM wParam, LPARAM lParam)
{
	SYSTEMTIME	tmStart1;
	SYSTEMTIME	tmEnd1;
	SYSTEMTIME	tmStart2;
	SYSTEMTIME	tmEnd2;
	memset (&tmStart1, 0, sizeof (SYSTEMTIME));
	memset (&tmEnd1, 0, sizeof (SYSTEMTIME));
	memset (&tmStart2, 0, sizeof (SYSTEMTIME));
	memset (&tmEnd2, 0, sizeof (SYSTEMTIME));

	char szValue[32];
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_YEAR1), szValue, sizeof (szValue));
	tmStart1.wYear = atoi (szValue);
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_MONTH1), szValue, sizeof (szValue));
	tmStart1.wMonth = atoi (szValue);
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_DAY1), szValue, sizeof (szValue));
	tmStart1.wDay = atoi (szValue);
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_YEAR2), szValue, sizeof (szValue));
	tmEnd1.wYear = atoi (szValue);
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_MONTH2), szValue, sizeof (szValue));
	tmEnd1.wMonth = atoi (szValue);
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_DAY2), szValue, sizeof (szValue));
	tmEnd1.wDay = atoi (szValue);

	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_YEAR3), szValue, sizeof (szValue));
	tmStart2.wYear = atoi (szValue);
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_MONTH3), szValue, sizeof (szValue));
	tmStart2.wMonth = atoi (szValue);
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_DAY3), szValue, sizeof (szValue));
	tmStart2.wDay = atoi (szValue);
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_YEAR4), szValue, sizeof (szValue));
	tmEnd2.wYear = atoi (szValue);
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_MONTH4), szValue, sizeof (szValue));
	tmEnd2.wMonth = atoi (szValue);
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_DAY4), szValue, sizeof (szValue));
	tmEnd2.wDay = atoi (szValue);

	long long	llStart1;
	long long	llEnd1;
	long long	llStart2;
	long long	llEnd2;
	SystemTimeToFileTime (&tmStart1, (LPFILETIME)&llStart1);
	SystemTimeToFileTime (&tmEnd1, (LPFILETIME)&llEnd1);
	SystemTimeToFileTime (&tmStart2, (LPFILETIME)&llStart2);
	SystemTimeToFileTime (&tmEnd2, (LPFILETIME)&llEnd2);

	m_pWndKXTInfo1->SetStartEndDate (llStart1, llEnd1);
	m_pWndKXTInfo2->SetStartEndDate (llStart2, llEnd2);

	return true;
}

int CDlgKXTStudy::CreateDlg (void)
{
	m_hDlg = CreateDialog (m_hInst, MAKEINTRESOURCE(IDD_DIALOG_KXT_STUDY), m_hParent, KXTStudyDlgProc);
	if (m_hDlg == NULL)
		return -1;
	SetWindowLong (m_hDlg, GWL_USERDATA, (LONG)this);
	//CBaseGraphics::OnCreateWnd (m_hDlg);
	m_hBrushGray = CreateSolidBrush (MSC_GRAY_3);

	InitParam ();

	RECT rcWnd;
	GetClientRect (m_hParent, &rcWnd);
	m_pWndKXTInfo1 = new CWndKXTInfo (m_hInst);
	m_pWndKXTInfo1->CreateWnd (m_hDlg, rcWnd, MSC_BLACK);
	m_pWndKXTInfo1->SetCode (CRegMng::g_pRegMng->GetTextValue ("KXTStudyCode"));
	m_pWndKXTInfo2 = new CWndKXTInfo (m_hInst);
	m_pWndKXTInfo2->CreateWnd (m_hDlg, rcWnd, MSC_BLACK);
	m_pWndKXTInfo2->SetCode (CRegMng::g_pRegMng->GetTextValue ("KXTStudyCode"));

	OnResize ();

	ShowWindow (GetDlgItem (m_hDlg, IDC_EDIT_CODE2), SW_HIDE);

	return 0;
}

INT_PTR CALLBACK CDlgKXTStudy::KXTStudyDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int				wmId, wmEvent;
	CDlgKXTStudy *	pDlgStudy = NULL;

	if (hDlg != NULL)
		pDlgStudy = (CDlgKXTStudy *) GetWindowLong (hDlg, GWL_USERDATA);

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_SIZE:
		if (pDlgStudy != NULL)
			pDlgStudy->OnResize ();
		break;

	case WM_KEYDOWN:
		pDlgStudy->OnKeyUp (wParam, lParam);
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		if (HIWORD(wParam) == EN_CHANGE && pDlgStudy->m_pWndKXTInfo1 != NULL)
		{
			char szCode[32];
			GetWindowText (GetDlgItem (hDlg, IDC_EDIT_CODE1), szCode, sizeof (szCode));
			if (strlen(szCode) == 6)
			{
				pDlgStudy->m_pWndKXTInfo1->SetCode (szCode);
				pDlgStudy->m_pWndKXTInfo2->SetCode (szCode);
			}
		}
		if (wmEvent == CBN_SELENDOK)
		{
			pDlgStudy->OnDateChange (wParam, lParam);
		}

		// Parse the menu selections:
		switch (wmId)
		{
		case IDC_BUTTON_ZOOMIN1:
			SendMessage (pDlgStudy->m_pWndKXTInfo1->GetWnd (), WM_KEYUP, VK_UP, 0);
			break;
		case IDC_BUTTON_ZOOMOUT1:
			SendMessage (pDlgStudy->m_pWndKXTInfo1->GetWnd (), WM_KEYUP, VK_DOWN, 0);
			break;
		case IDC_BUTTON_MOVELEFT1:
			SendMessage (pDlgStudy->m_pWndKXTInfo1->GetWnd (), WM_KEYUP, VK_LEFT, 0);
			break;
		case IDC_BUTTON_MOVERIGHT1:
			SendMessage (pDlgStudy->m_pWndKXTInfo1->GetWnd (), WM_KEYUP, VK_RIGHT, 0);
			break;
		case IDC_BUTTON_ZOOMIN2:
			SendMessage (pDlgStudy->m_pWndKXTInfo2->GetWnd (), WM_KEYUP, VK_UP, 0);
			break;
		case IDC_BUTTON_ZOOMOUT2:
			SendMessage (pDlgStudy->m_pWndKXTInfo2->GetWnd (), WM_KEYUP, VK_DOWN, 0);
			break;
		case IDC_BUTTON_MOVELEFT2:
			SendMessage (pDlgStudy->m_pWndKXTInfo2->GetWnd (), WM_KEYUP, VK_LEFT, 0);
			break;
		case IDC_BUTTON_MOVERIGHT2:
			SendMessage (pDlgStudy->m_pWndKXTInfo2->GetWnd (), WM_KEYUP, VK_RIGHT, 0);
			break;

		case IDOK:
		case IDCANCEL:
			DestroyWindow (hDlg);
			break;

		default:
			break;
		}
		break;

	case WM_ERASEBKGND:
	{
		HDC hdc = (HDC)wParam;
		RECT rcView;
		GetClientRect (hDlg, &rcView);
		FillRect (hdc, &rcView, pDlgStudy->m_hBrushGray);
		return S_FALSE;
	}
	default:
		break;
	}
	return (INT_PTR)FALSE;
}

bool CDlgKXTStudy::OnKeyUp (WPARAM wParam, LPARAM lParam)
{
	long	nLen = strlen (m_szKeyCode);
	if ((wParam == VK_DELETE || wParam == VK_BACK) && nLen > 0)
	{
		m_szKeyCode[nLen-1] = 0;
		SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_CODE1), m_szKeyCode);
		return true;	
	}

	char cKey = '0';
	if (wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9)
		cKey = wParam - VK_NUMPAD0 + '0';
	else if (wParam >= '0' && wParam <= '9')
		cKey = (char)wParam;
	else
		return false;
	if (nLen >= 6)
	{
		m_szKeyCode[0] = cKey;
		m_szKeyCode[1] = 0;
	}
	else
	{
		m_szKeyCode[nLen] = cKey;
		m_szKeyCode[nLen+1] = 0;
	}
	SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_CODE1), m_szKeyCode);
	return true;
}

LRESULT CDlgKXTStudy::OnResize (void)
{
	RECT rcWnd;
	GetClientRect (m_hParent, &rcWnd);
	SetWindowPos (m_hDlg, NULL, rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, 0);

	int		nWidth = 260;
	int		nX, nY, nW = 0, nH = 0;
	RECT	rcDlg;
	GetClientRect (m_hDlg, &rcDlg);

	nX = rcDlg.right - nWidth;
	nY = 6;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_EDIT_CODE1), NULL, nX, nY, 100, 38, SWP_NOZORDER);
	nX = rcDlg.right - nWidth;
	nY += 48;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_COMBO_YEAR1), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nX += 108;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_COMBO_MONTH1), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nX += 80;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_COMBO_DAY1), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nX = rcDlg.right - nWidth;
	nY += 48;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_COMBO_YEAR2), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nX += 108;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_COMBO_MONTH2), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nX += 80;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_COMBO_DAY2), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nX = rcDlg.right - nWidth / 2 - 50;
	nY += 68;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_BUTTON_ZOOMIN1), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nY += 98;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_BUTTON_ZOOMOUT1), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nX = rcDlg.right - nWidth + 24;
	nY -= 48;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_BUTTON_MOVELEFT1), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nX = rcDlg.right - nWidth + 140;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_BUTTON_MOVERIGHT1), NULL, nX, nY, 0, 0, SWP_NOSIZE);


	nX = rcDlg.right - nWidth;
	nY = rcDlg.bottom / 2 + 6;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_EDIT_CODE2), NULL, nX, nY, 90, 38, 0);
	nX = rcDlg.right - nWidth;
	nY += 48;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_COMBO_YEAR3), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nX += 108;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_COMBO_MONTH3), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nX += 80;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_COMBO_DAY3), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nX = rcDlg.right - nWidth;
	nY += 48;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_COMBO_YEAR4), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nX += 108;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_COMBO_MONTH4), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nX += 80;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_COMBO_DAY4), NULL, nX, nY, 0, 0, SWP_NOSIZE);

	nX = rcDlg.right - nWidth / 2 - 50;
	nY += 68;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_BUTTON_ZOOMIN2), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nY += 98;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_BUTTON_ZOOMOUT2), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nX = rcDlg.right - nWidth + 24;
	nY -= 48;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_BUTTON_MOVELEFT2), NULL, nX, nY, 0, 0, SWP_NOSIZE);
	nX = rcDlg.right - nWidth + 140;
	SetWindowPos (GetDlgItem (m_hDlg, IDC_BUTTON_MOVERIGHT2), NULL, nX, nY, 0, 0, SWP_NOSIZE);

	SetWindowPos (m_pWndKXTInfo1->GetWnd (), NULL, rcDlg.left, rcDlg.top, rcDlg.right - nWidth, rcDlg.bottom / 2, 0);
	SetWindowPos (m_pWndKXTInfo2->GetWnd (), NULL, rcDlg.left, rcDlg.bottom / 2, rcDlg.right - nWidth, rcDlg.bottom / 2, 0);

	SendMessage (GetDlgItem (m_hDlg, IDC_EDIT_CODE1), WM_SETFOCUS, 0, 0);
	return S_OK;
}

void CDlgKXTStudy::InitParam (void)
{
	int		i = 0;
	char *	szYear[] = {"2000", "2001", "2002", "2003", "2004", "2005", "2006", "2007", "2008", "2009", "2010", "2011", "2012", "2013", "2014", "2015", "2016", "2017"};
	for (i = 0; i < 18; i++)
	{
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_YEAR1), CB_ADDSTRING, NULL, (LPARAM)szYear[i]);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_YEAR2), CB_ADDSTRING, NULL, (LPARAM)szYear[i]);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_YEAR3), CB_ADDSTRING, NULL, (LPARAM)szYear[i]);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_YEAR4), CB_ADDSTRING, NULL, (LPARAM)szYear[i]);
	}
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_YEAR1), CB_SETCURSEL, 0, NULL);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_YEAR2), CB_SETCURSEL, 17, NULL);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_YEAR3), CB_SETCURSEL, 0, NULL);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_YEAR4), CB_SETCURSEL, 17, NULL);

	char szMonth[12];
	for (i = 0; i < 12; i++)
	{
		sprintf (szMonth, "%d", i + 1);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_MONTH1), CB_ADDSTRING, NULL, (LPARAM)szMonth);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_MONTH2), CB_ADDSTRING, NULL, (LPARAM)szMonth);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_MONTH3), CB_ADDSTRING, NULL, (LPARAM)szMonth);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_MONTH4), CB_ADDSTRING, NULL, (LPARAM)szMonth);
	}
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_MONTH1), CB_SETCURSEL, 0, NULL);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_MONTH2), CB_SETCURSEL, 1, NULL);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_MONTH3), CB_SETCURSEL, 0, NULL);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_MONTH4), CB_SETCURSEL, 11, NULL);

	char szDay[12];
	for (i = 0; i < 31; i++)
	{
		sprintf (szDay, "%d", i + 1);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_DAY1), CB_ADDSTRING, NULL, (LPARAM)szDay);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_DAY2), CB_ADDSTRING, NULL, (LPARAM)szDay);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_DAY3), CB_ADDSTRING, NULL, (LPARAM)szDay);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_DAY4), CB_ADDSTRING, NULL, (LPARAM)szDay);
	}
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_DAY1), CB_SETCURSEL, 0, NULL);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_DAY2), CB_SETCURSEL, 4, NULL);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_DAY3), CB_SETCURSEL, 0, NULL);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_DAY4), CB_SETCURSEL, 30, NULL);

	char *pLastCode = CRegMng::g_pRegMng->GetTextValue ("KXTStudyCode");
	if (pLastCode != NULL)
	{
		SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_CODE1), pLastCode);
		SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_CODE2), pLastCode);
	}
}
