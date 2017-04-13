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
#include "CRegMng.h"

#include "Resource.h"

#include "USystemFunc.h"

CDlgStockSelect::CDlgStockSelect(HINSTANCE hInst, HWND hParent)
	: CDlgBase (hInst, hParent)
	, m_pCodeList (NULL)
	, m_pWndKXT (NULL)
	, m_pBuy01 (NULL)
	, m_nTimerAnalyse (0)
{
}

CDlgStockSelect::~CDlgStockSelect(void)
{
	QC_DEL_P (m_pCodeList);
	QC_DEL_P (m_pBuy01);
}

char * CDlgStockSelect::GetStockCode (bool bNext)
{
	int nCount = SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_GETCOUNT, 0, 0);
	if (nCount <= 0)
		return NULL;

	int nSel = SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_GETCURSEL, 0, 0);
	if (nSel < 0)
	{
		nSel = 0;
	}
	else
	{
		if (bNext)
		{
			nSel = nSel + 1;
			if (nSel >= nCount)
				nSel = 0;
		}
		else
		{
			nSel = nSel - 1;
			if (nSel < 0)
				nSel = nCount - 1;
		}
	}
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_GETTEXT, nSel, (LPARAM)m_szCode);
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_SETCURSEL, nSel, 0);
	return m_szCode;
}

int CDlgStockSelect::CreateDlg (void)
{
	CDlgBase::CreateDlg ();
	m_hDlg = CreateDialog (m_hInst, MAKEINTRESOURCE(IDD_DIALOG_MYSELECT), m_hParent, baseDlgProc);
	if (m_hDlg == NULL)
		return -1;
	SetWindowLong (m_hDlg, GWL_USERDATA, (LONG)this);

	OnInitDlg ();

	InvalidateRect (m_hDlg, NULL, TRUE);

	return 0;
}

int CDlgStockSelect::OpenSelectFile (char * pFile)
{
	char				szFile[256];
	if (pFile == NULL)
	{
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
	}
	else
	{
		strcpy (szFile, pFile);
	}

	if (m_pCodeList == NULL)
		m_pCodeList = new CStockFileCode ();
	m_pCodeList->Open (szFile);
	int nCodeCount = m_pCodeList->GetCodeCount ();
	for (int i = 0; i < nCodeCount; i++)
		SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_ADDSTRING, 0, (LPARAM)m_pCodeList->GetCodeNum (i));

	return QC_ERR_NONE;
}


int CDlgStockSelect::OnInitDlg (void)
{
	RECT rcWnd;
	GetWindowRect (GetParent (m_hParent), &rcWnd);
	SetWindowPos (m_hDlg, NULL, rcWnd.right, rcWnd.top, 0, 0, SWP_NOSIZE);

	InitParam ();

	OpenSelectFile ("codeSelect.txt");

	return QC_ERR_NONE;
}

LRESULT CDlgStockSelect::OnResize (void)
{
	RECT rcWnd;
	GetWindowRect (GetParent (m_hParent), &rcWnd);

	RECT rcDlg;
	GetWindowRect (m_hDlg, &rcDlg);
	if ((rcDlg.bottom - rcDlg.top) < 200)
		SetWindowPos (m_hDlg, NULL, rcWnd.right-(rcDlg.right-rcDlg.left), rcWnd.bottom- (rcDlg.bottom-rcDlg.top), 0, 0, SWP_NOSIZE);

	return QC_ERR_NONE;
}

bool CDlgStockSelect::UpdateKXTView (void)
{
	if (m_pWndKXT == NULL)
		return false;
	int nIndex = SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_GETCURSEL, 0, 0);
	if (nIndex < 0)
		return false;
	char szCode[64];
	memset (szCode, 0, sizeof (szCode));
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_GETTEXT, nIndex, (LPARAM)szCode);		

	m_pWndKXT->SetCode (szCode);
	InvalidateRect (m_pWndKXT->GetWnd (), NULL, FALSE);

	return true;
}

bool CDlgStockSelect::OnSelectStock1 (void)
{
	if (m_pBuy01 == NULL)
		m_pBuy01 = new CStockAnalyseBuy01 ();

	SYSTEMTIME tmStart, tmEnd;
	GetLocalTime (&tmEnd);
	GetLocalTime (&tmStart);
	tmStart.wYear -= 2;
	m_pBuy01->SetStartEndDate (&tmStart, &tmEnd);

	char szText[16];
	int		nUpDays;
	double	dUpMin, dUpMax;
	GetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_1DAYS), szText, sizeof (szText));
	nUpDays = atoi (szText);
	GetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_1MIN), szText, sizeof (szText));
	dUpMin = atof (szText);
	GetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_1MAX), szText, sizeof (szText));
	dUpMax = atof (szText);
	m_pBuy01->SetFirstParam (nUpDays, dUpMin, dUpMax);

	GetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_2MAX), szText, sizeof (szText));
	double dMaxDown = atof (szText);
	m_pBuy01->SetSecondParam (dMaxDown);

	GetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_3MAX), szText, sizeof (szText));
	double	dMaxDiff = atof (szText);
	GetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_3DAYS), szText, sizeof (szText));
	int		nMaxDays = atoi (szText);
	m_pBuy01->SetThirdParam (dMaxDiff, nMaxDays);

	if (m_pBuy01->Init ("codeList.txt") != QC_ERR_NONE)
		return false;

	SendMessage (GetDlgItem (m_hDlg, IDC_PROGRESS_STATUS), PBM_SETRANGE, 0, MAKELPARAM(0, m_pBuy01->GetTotalNum ())); 
	SendMessage (GetDlgItem (m_hDlg, IDC_PROGRESS_STATUS), PBM_SETSTEP, (WPARAM) 1, 0); 
	SendMessage (GetDlgItem (m_hDlg, IDC_PROGRESS_STATUS), PBM_SETPOS, (WPARAM) 0, 0); 
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_RESETCONTENT, 0, 0);
	SetWindowText (GetDlgItem (m_hDlg, IDC_BUTTON_PAUSE), "ÔÝ Í£");

	if (m_nTimerAnalyse == 0)
		m_nTimerAnalyse = SetTimer (m_hDlg, WM_TIMER_ANALYSE, 10, NULL);

	return true;
}

bool CDlgStockSelect::OnTimer (WPARAM wParam, LPARAM lParam)
{
//	m_nTimerAnalyse = SetTimer (m_hDlg, WM_TIMER_ANALYSE, 10, NULL);
	if (m_pBuy01 == NULL)
		return false;
	if (m_pBuy01->Analyse (true) == QC_ERR_FINISH)
	{
		KillTimer (m_hDlg, m_nTimerAnalyse);
		m_nTimerAnalyse = 0;
		SetWindowText (GetDlgItem (m_hDlg, IDC_BUTTON_PAUSE), "ÔÝ Í£");
		MessageBox (m_hDlg, "Analyse stock finished!", "Information", MB_OK);
	}
	else
	{
		SendMessage (GetDlgItem (m_hDlg, IDC_PROGRESS_STATUS), PBM_SETPOS, (WPARAM)m_pBuy01->GetCurIndex (), 0); 
	}
	int nSelected = SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_GETCOUNT, 0, 0);
	if (nSelected < m_pBuy01->GetResultCount ())
	{
		SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_ADDSTRING, 0, (LPARAM)m_pBuy01->GetResultCode (nSelected));
	}

	return true;
}

void CDlgStockSelect::InitParam (void)
{
	SYSTEMTIME	tmNow;
	GetLocalTime (&tmNow);
	int		i = 0;

	char * pValue = CRegMng::g_pRegMng->GetTextValue ("Buy01Day1");
	if (strlen (pValue) > 0)
		SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_1DAYS), pValue);
	else
		SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_1DAYS), "5");
	pValue = CRegMng::g_pRegMng->GetTextValue ("Buy01Min1");
	if (strlen (pValue) > 0)
		SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_1MIN), pValue);
	else
		SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_1MIN), "1");
	pValue = CRegMng::g_pRegMng->GetTextValue ("Buy01Max1");
	if (strlen (pValue) > 0)
		SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_1MAX), pValue);
	else
		SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_1MAX), "5");

	pValue = CRegMng::g_pRegMng->GetTextValue ("Buy01MaxDown");
	if (strlen (pValue) > 0)
		SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_2MAX), pValue);
	else
		SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_2MAX), "30");

	pValue = CRegMng::g_pRegMng->GetTextValue ("Buy01MaxUp");
	if (strlen (pValue) > 0)
		SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_3MAX), pValue);
	else
		SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_3MAX), "50");
	pValue = CRegMng::g_pRegMng->GetTextValue ("Buy01Max3Days");
	if (strlen (pValue) > 0)
		SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_3DAYS), pValue);
	else
		SetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_3DAYS), "365");

	SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_DAYLINE5), BM_SETCHECK, BST_CHECKED, 0);
	SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_MACD), BM_SETCHECK, BST_CHECKED, 0);
}

INT_PTR CDlgStockSelect::OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
		OnResize ();
		break;

	case WM_TIMER:
		OnTimer (wParam, lParam);
		break;

	case WM_COMMAND:
	{
		int wmId    = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);

		if (wmEvent == LBN_SELCHANGE && wmId == IDC_LIST_STOCK)
			UpdateKXTView ();
		else if (wmEvent == BN_CLICKED && wmId == IDC_CHECK_DAYLINE5)
		{
			bool bDayLine5 = SendMessage (GetDlgItem (m_hDlg, wmId), BM_GETCHECK, 0, 0) == BST_CHECKED ? true : false;
			m_pBuy01->SetDayLine5 (bDayLine5);
		}
		else if (wmEvent == BN_CLICKED && wmId == IDC_CHECK_MACD)
		{
			bool bMACDKingX = SendMessage (GetDlgItem (m_hDlg, wmId), BM_GETCHECK, 0, 0) == BST_CHECKED ? true : false;
			m_pBuy01->SetMACDKingX (bMACDKingX);
		}

		switch (wmId)
		{
		case IDC_BUTTON_FILE:
			OpenSelectFile (NULL);
			break;

		case IDC_BUTTON_DELETE:
		{
			int nSel = SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_GETCURSEL, 0, 0);
			SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_DELETESTRING, nSel, 0);
			SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_SETCURSEL, nSel, 0);
			UpdateKXTView ();
		}
			break;

		case IDC_BUTTON_COPY:
		{
			char	szCodeList[2048];
			char	szCode[16];
			int		nNum = SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_GETCOUNT, 0, 0);
			SYSTEMTIME tmNow;
			GetLocalTime (&tmNow);
			sprintf (szCodeList, "%d-%02d-%02d\r\n", tmNow.wYear, tmNow.wMonth, tmNow.wDay);
			for (int i = 0; i < nNum; i++)
			{
				SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_GETTEXT, i, (LPARAM)szCode);
				strcat (szCodeList, szCode);
				strcat (szCodeList, "\r\n");
			}
			qcCopyToClipBoard (szCodeList);
			break;
		}

		case IDC_BUTTON_SELECT1:
			OnSelectStock1 ();
			break;

		case IDC_BUTTON_PAUSE:
			if (m_nTimerAnalyse != 0)
			{
				KillTimer (m_hDlg, m_nTimerAnalyse);
				m_nTimerAnalyse = 0;
				SetWindowText (GetDlgItem (m_hDlg, IDC_BUTTON_PAUSE), "¼Ì Ðø");
			}
			else
			{
				SetWindowText (GetDlgItem (m_hDlg, IDC_BUTTON_PAUSE), "ÔÝ Í£");
				m_nTimerAnalyse = SetTimer (m_hDlg, WM_TIMER_ANALYSE, 10, NULL);
			}
			break;


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
