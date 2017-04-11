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

#include "CDlgBuy01.h"
#include "CRegMng.h"

#include "Resource.h"

#include "USystemFunc.h"

CDlgBuy01::CDlgBuy01(HINSTANCE hInst, HWND hParent)
	: CDlgBase (hInst, hParent)
	, m_pBuy01 (NULL)
	, m_nTimerAnalyse (0)
	, m_pWndKXT (NULL)
	, m_llStartDate (0)
	, m_llEndDate (0)
	, m_pBuySellInfo (NULL)
{
	m_pBuy01 = new CStockAnalyseBuy01 ();
}

CDlgBuy01::~CDlgBuy01(void)
{
	QC_DEL_P (m_pBuy01);
	QC_DEL_A (m_pBuySellInfo);

	char szValue[32];
	GetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_1DAYS), szValue, sizeof (szValue));
	CRegMng::g_pRegMng->SetTextValue ("Buy01Day1", szValue);
	GetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_1MIN), szValue, sizeof (szValue));
	CRegMng::g_pRegMng->SetTextValue ("Buy01Min1", szValue);
	GetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_1MAX), szValue, sizeof (szValue));
	CRegMng::g_pRegMng->SetTextValue ("Buy01Max1", szValue);

	GetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_2MAX), szValue, sizeof (szValue));
	CRegMng::g_pRegMng->SetTextValue ("Buy01MaxDown", szValue);

	GetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_3MAX), szValue, sizeof (szValue));
	CRegMng::g_pRegMng->SetTextValue ("Buy01MaxUp", szValue);
	GetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_3DAYS), szValue, sizeof (szValue));
	CRegMng::g_pRegMng->SetTextValue ("Buy01Max3Days", szValue);
}

bool CDlgBuy01::OnSelectStock (void)
{
	memset (&m_tmStart, 0, sizeof (SYSTEMTIME));
	memset (&m_tmEnd, 0, sizeof (SYSTEMTIME));
	m_tmStart.wYear = 2015;m_tmStart.wMonth = 1;m_tmStart.wDay = 1;
	char szText[16];
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_YEAR), szText, sizeof (szText));
	m_tmEnd.wYear = atoi (szText);
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_MONTH), szText, sizeof (szText));
	m_tmEnd.wMonth = atoi (szText);
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_DAY), szText, sizeof (szText));
	m_tmEnd.wDay = atoi (szText);
	if (m_tmEnd.wMonth >= 6)
		m_tmStart.wYear = m_tmEnd.wYear - 1;
	else
		m_tmStart.wYear = m_tmEnd.wYear - 2;
	m_pBuy01->SetStartEndDate (&m_tmStart, &m_tmEnd);
	SystemTimeToFileTime (&m_tmStart, (LPFILETIME)&m_llStartDate);
	SystemTimeToFileTime (&m_tmEnd, (LPFILETIME)&m_llEndDate);
	
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

	SetWindowText (GetDlgItem (m_hDlg, IDC_BUTTON_PAUSE), "暂停");

	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_RESETCONTENT, 0, 0);
	QC_DEL_A (m_pBuySellInfo);
	m_nTimerAnalyse = SetTimer (m_hDlg, WM_TIMER_ANALYSE, 10, NULL);
	return true;
}

bool CDlgBuy01::OnTimer (WPARAM wParam, LPARAM lParam)
{
//	m_nTimerAnalyse = SetTimer (m_hDlg, WM_TIMER_ANALYSE, 10, NULL);
	if (m_pBuy01 == NULL)
		return false;
	if (m_pBuy01->Analyse (true) == QC_ERR_FINISH)
	{
		KillTimer (m_hDlg, m_nTimerAnalyse);
		m_nTimerAnalyse = 0;
		MessageBox (m_hDlg, "Analyse stock finished!", "Information", MB_OK);
	}
	else
	{
		SendMessage (GetDlgItem (m_hDlg, IDC_PROGRESS_STATUS), PBM_SETPOS, (WPARAM)m_pBuy01->GetCurIndex (), 0); 
	}
	int nSelected = SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_GETCOUNT, 0, 0);
	if (nSelected < m_pBuy01->GetResultCount ())
	{
		SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_ADDSTRING, 0, (LPARAM)m_pBuy01->GetResultCode (nSelected));
	}

	return true;
}


int CDlgBuy01::CreateDlg (void)
{
	CDlgBase::CreateDlg ();
	m_hDlg = CreateDialog (m_hInst, MAKEINTRESOURCE(IDD_DIALOG_BUY01), m_hParent, baseDlgProc);
	if (m_hDlg == NULL)
		return -1;
	SetWindowLong (m_hDlg, GWL_USERDATA, (LONG)this);
	//m_hBrushBG = CreateSolidBrush (MSC_GRAY_2);

	InitParam ();
	CenterDlg ();

	return 0;
}

bool CDlgBuy01::OnKeyUp (WPARAM wParam, LPARAM lParam)
{
	return true;
}

LRESULT CDlgBuy01::OnResize (void)
{
	RECT rcWnd;
	GetWindowRect (m_hParent, &rcWnd);
	RECT rcDlg;
	GetWindowRect (m_hDlg, &rcDlg);
	if ((rcDlg.bottom - rcDlg.top) < 100)
		SetWindowPos (m_hDlg, NULL, rcWnd.right-(rcDlg.right-rcDlg.left), rcWnd.bottom- (rcDlg.bottom-rcDlg.top), 0, 0, SWP_NOSIZE);

	return S_OK;
}

bool CDlgBuy01::UpdateKXTView (void)
{
	if (m_pWndKXT == NULL)
		return false;
	int nIndex = SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_GETCURSEL, 0, 0);
	if (nIndex < 0)
		return false;
	char szCode[64];
	memset (szCode, 0, sizeof (szCode));
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_GETTEXT, nIndex, (LPARAM)szCode);		
	szCode[6] = 0;

	CStockKXTInfo *	pKXTInfo = m_pWndKXT->GetKXTInfo ();
	if (m_llStartDate >0 && m_llEndDate >0)
	{
		if (SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_NEWDATE), BM_GETCHECK, 0, 0) == BST_CHECKED)
		{
			UpdateEndCheckDate ();
			pKXTInfo->SetStartEndDate (m_llStartDate, m_llEndCheck);
		}
		else
			pKXTInfo->SetStartEndDate (m_llStartDate, m_llEndDate);
	}
	m_pWndKXT->SetCode (szCode);

	if (SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_NEWDATE), BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		m_pWndKXT->SetBuyTime (m_tmEnd);
		if (m_pBuySellInfo != NULL)
			m_pWndKXT->SetSellTime (m_pBuySellInfo[nIndex].tmSell);
	}

	InvalidateRect (m_pWndKXT->GetWnd (), NULL, FALSE);
	return true;
}

void CDlgBuy01::OnUpdateDaysCtrl (void)
{
	char szText[8];
	GetWindowText (GetDlgItem (m_hDlg,  IDC_COMBO_YEAR), szText, sizeof (szText));
	int nYear = atoi (szText);
	GetWindowText (GetDlgItem (m_hDlg,  IDC_COMBO_MONTH), szText, sizeof (szText));
	int nMonth = atoi (szText);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_DAY), CB_RESETCONTENT, 0, 0);
	char szDay[12];
	for (int i = 1; i <= qcMonthDays[nMonth-1]; i++)
	{
		if (qcIsHoliday (nYear, nMonth, i))
			continue;
		sprintf (szDay, "%d", i);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_DAY), CB_ADDSTRING, NULL, (LPARAM)szDay);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_DAY_END), CB_ADDSTRING, NULL, (LPARAM)szDay);
	}
	SYSTEMTIME tmNow;
	GetLocalTime (&tmNow);
	if (tmNow.wDayOfWeek == 6)
		tmNow.wDay -= 1;
	else if (tmNow.wDayOfWeek == 0)
		tmNow.wDay -= 2;
	sprintf (szDay, "%d", tmNow.wDay);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_DAY), CB_SELECTSTRING, -1, (LPARAM)szDay);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_DAY_END), CB_SELECTSTRING, -1, (LPARAM)szDay);
}

void CDlgBuy01::UpdateEndCheckDate (void)
{
	memset (&m_tmEndCheck, 0, sizeof (SYSTEMTIME));
	char szText[16];
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_YEAR_END), szText, sizeof (szText));
	m_tmEndCheck.wYear = atoi (szText);
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_MONTH_END), szText, sizeof (szText));
	m_tmEndCheck.wMonth = atoi (szText);
	GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_DAY_END), szText, sizeof (szText));
	m_tmEndCheck.wDay = atoi (szText);
	long long llEndDate = 0;
	SystemTimeToFileTime (&m_tmEndCheck, (LPFILETIME)&m_llEndCheck);

}

void CDlgBuy01::InitParam (void)
{
	SYSTEMTIME	tmNow;
	GetLocalTime (&tmNow);
	int		i = 0;
	char *	szYear[] = {"2000", "2001", "2002", "2003", "2004", "2005", "2006", "2007", "2008", "2009", "2010", "2011", "2012", "2013", "2014", "2015", "2016", "2017"};
	for (i = 0; i < 18; i++)
	{
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_YEAR), CB_ADDSTRING, NULL, (LPARAM)szYear[i]);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_YEAR_END), CB_ADDSTRING, NULL, (LPARAM)szYear[i]);
	}
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_YEAR), CB_SETCURSEL, tmNow.wYear - 2000, NULL);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_YEAR_END), CB_SETCURSEL, tmNow.wYear - 2000, NULL);

	char szMonth[12];
	for (i = 0; i < 12; i++)
	{
		sprintf (szMonth, "%d", i + 1);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_MONTH), CB_ADDSTRING, NULL, (LPARAM)szMonth);
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_MONTH_END), CB_ADDSTRING, NULL, (LPARAM)szMonth);
	}
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_MONTH), CB_SETCURSEL, tmNow.wMonth - 1, NULL);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_MONTH_END), CB_SETCURSEL, tmNow.wMonth - 1, NULL);

	OnUpdateDaysCtrl ();

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

INT_PTR CDlgBuy01::OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_TIMER:
		OnTimer (wParam, lParam);
		break;

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
		if (wmEvent == LBN_SELCHANGE && wmId == IDC_LIST_RESULT)
			UpdateKXTView ();
		else if (wmEvent == BN_CLICKED && wmId == IDC_CHECK_NEWDATE)
			UpdateKXTView ();
		else if (wmEvent == CBN_SELENDOK && wmId == IDC_COMBO_MONTH)
			OnUpdateDaysCtrl ();
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
		case IDC_BUTTON_SELECT:
			OnSelectStock ();
			break;

		case IDC_BUTTON_CHECK:
			CheckWithMaxMin ();
			break;

		case IDC_BUTTON_SELL:
			TryToSell ();
			break;

		case IDC_BUTTON_PAUSE:
			if (m_nTimerAnalyse != NULL)
			{
				KillTimer (m_hDlg, m_nTimerAnalyse);
				m_nTimerAnalyse = 0;
				SetWindowText (GetDlgItem (m_hDlg, IDC_BUTTON_PAUSE), "继续");
			}
			else
			{
				m_nTimerAnalyse = SetTimer (m_hDlg, WM_TIMER_ANALYSE, 10, NULL);
				SetWindowText (GetDlgItem (m_hDlg, IDC_BUTTON_PAUSE), "暂停");
			}
			break;

		case IDC_BUTTON_DELETE:
		{
			int nSel = SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_GETCURSEL, 0, 0);
			SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_DELETESTRING, nSel, 0);
			SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_SETCURSEL, nSel, 0);
			UpdateKXTView ();
		}
			break;

		case IDC_BUTTON_COPY:
		{
			char	szCodeList[2048];
			char	szCode[16];
			int		nNum = SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_GETCOUNT, 0, 0);
			SYSTEMTIME tmNow;
			GetLocalTime (&tmNow);
			sprintf (szCodeList, "%d-%02d-%02d\r\n", tmNow.wYear, tmNow.wMonth, tmNow.wDay);
			for (int i = 0; i < nNum; i++)
			{
				SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_GETTEXT, i, (LPARAM)szCode);
				strcat (szCodeList, szCode);
				strcat (szCodeList, "\r\n");
			}
			qcCopyToClipBoard (szCodeList);
			break;
		}

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

int	CDlgBuy01::CheckWithMaxMin (void)
{
	int	nNum = SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_GETCOUNT, 0, 0);
	if (nNum <= 0)
		return QC_ERR_FAILED;
	int			i = 0;
	int			nIndex = 0;
	char		szCode[256];
	char		szItem[1024];
	char		szLine[8096];
	double		dBuyPrice = 0;
	double		dMaxPrice = 0;
	double		dMinPrice = 1000;

	UpdateEndCheckDate ();
	int			nCheckDay = qcGetDaysFrom2000 (m_tmEndCheck.wYear, m_tmEndCheck.wMonth, m_tmEndCheck.wDay);
	int			nStockDay = 0;

	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_CHECK), LB_RESETCONTENT, 0, 0);

	strcpy (szLine, "Date        ");
	for (i = 0; i < nNum; i++)
	{
		SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_GETTEXT, i, (LPARAM)szCode);
		szCode[6] = 0;
		strcat (szLine, szCode);
		strcat (szLine, "        ");
	}
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_CHECK), LB_ADDSTRING, 0, (LPARAM)szLine);

	qcStockKXTInfoItem *	pItem = NULL;
	NODEPOS					pos = NULL;
	CStockFileHist *		pStockInfo = new CStockFileHist ();
	for (i = 0; i < nNum; i++)
	{
		SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_GETTEXT, i, (LPARAM)szCode);
		pStockInfo->SetStartEndDate (m_llEndDate, -1);
		szCode[6] = 0;
		pStockInfo->SetCode (szCode);

		nIndex = 0;
		dBuyPrice = 0;
		dMaxPrice = 0;
		dMinPrice = 1000;
		pos = pStockInfo->m_lstItem.GetHeadPosition ();
		while (pos != NULL)
		{
			pItem = pStockInfo->m_lstItem.GetNext (pos);
			if (dBuyPrice == 0)
				dBuyPrice = pItem->m_dClose;
			if (dMaxPrice < pItem->m_dMax)
				dMaxPrice = pItem->m_dMax;
			if (dMinPrice > pItem->m_dMin)
				dMinPrice = pItem->m_dMin;

			if (i == 0)
			{
				memset (szLine, 0, sizeof (szLine));
				sprintf (szItem, "%d-%02d-%02d", pItem->m_nYear, pItem->m_nMonth, pItem->m_nDay);
				strcpy (szLine, szItem);
				FormatDouble (pItem->m_dClose, szItem);
				strcat (szLine, szItem);
				FormatDouble ((pItem->m_dClose - dBuyPrice) * 100 / dBuyPrice, szItem);
				strcat (szLine, szItem);
				SendMessage (GetDlgItem (m_hDlg, IDC_LIST_CHECK), LB_ADDSTRING, nIndex + 1, (LPARAM)szLine);
			}
			else
			{
				SendMessage (GetDlgItem (m_hDlg, IDC_LIST_CHECK), LB_GETTEXT, nIndex + 1, (LPARAM)szLine);
				FormatDouble (pItem->m_dClose, szItem);
				strcat (szLine, szItem);
				FormatDouble ((pItem->m_dClose - dBuyPrice) * 100 / dBuyPrice, szItem);
				strcat (szLine, szItem);

				SendMessage (GetDlgItem (m_hDlg, IDC_LIST_CHECK), LB_DELETESTRING, nIndex+1, 0);
				SendMessage (GetDlgItem (m_hDlg, IDC_LIST_CHECK), LB_INSERTSTRING, nIndex+1, (LPARAM)szLine);

			}
			nIndex++;
			nStockDay = qcGetDaysFrom2000 (pItem->m_nYear, pItem->m_nMonth, pItem->m_nDay);
			if (nStockDay > nCheckDay)
				break;
		}

		SendMessage (GetDlgItem (m_hDlg, IDC_LIST_CHECK), LB_GETTEXT, 0, (LPARAM)szLine);
		int nPos = 12 + 14 * i + 6;
		FormatDouble ((dMaxPrice - dBuyPrice) * 100 / dBuyPrice, szItem);
		nIndex = 0;
		while (szItem[nIndex+1] != 0)
		{
			szLine[nPos + nIndex] = szItem[nIndex+1];
			nIndex++;
		}
		SendMessage (GetDlgItem (m_hDlg, IDC_LIST_CHECK), LB_DELETESTRING, 0, 0);
		SendMessage (GetDlgItem (m_hDlg, IDC_LIST_CHECK), LB_INSERTSTRING, 0, (LPARAM)szLine);

		SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_GETTEXT, i, (LPARAM)szLine);
		szLine[6] = 0;
		strcat (szLine, szItem);
		FormatDouble ((dMinPrice - dBuyPrice) * 100 / dBuyPrice, szItem);
		strcat (szLine, szItem);
		SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_DELETESTRING, i, 0);
		SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_INSERTSTRING, i, (LPARAM)szLine);
	}
	delete pStockInfo;

	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_CHECK), LB_GETTEXT, 1, (LPARAM)szLine);
	SIZE szSize;
	HDC hDC = GetDC (GetDlgItem (m_hDlg, IDC_LIST_CHECK));
	GetTextExtentPoint (hDC, szLine, strlen (szLine), &szSize);
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_CHECK), LB_SETHORIZONTALEXTENT, (WPARAM)szSize.cx * 2, 0);
	ReleaseDC (GetDlgItem (m_hDlg, IDC_LIST_CHECK), hDC);

	return QC_ERR_NONE;
}

int CDlgBuy01::TryToSell (void)
{
	int	nNum = SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_GETCOUNT, 0, 0);
	if (nNum <= 0)
		return QC_ERR_FAILED;

	int			i = 0;
	int			nIndex = 0;
	char		szCode[256];
	char		szItem[1024];
	char		szLine[8096];
	double		dMaxPrice = 0;
	double		dMinPrice = 1000;
	double		dDiffPrice = 0;
	double		dPrevLine5 = 0;

	QC_DEL_A (m_pBuySellInfo);
	m_pBuySellInfo = new sStockBuySellInfo[nNum];

	UpdateEndCheckDate ();
	int			nCheckDay = qcGetDaysFrom2000 (m_tmEndCheck.wYear, m_tmEndCheck.wMonth, m_tmEndCheck.wDay);
	int			nStartDay = qcGetDaysFrom2000 (m_tmEnd.wYear, m_tmEnd.wMonth, m_tmEnd.wDay);
	int			nStockDay = 0;

	qcStockKXTInfoItem *	pItem = NULL;
	NODEPOS					pos = NULL;
	CStockFileHist *		pStockInfo = new CStockFileHist ();
	for (i = 0; i < nNum; i++)
	{
		SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_GETTEXT, i, (LPARAM)szCode);
		szCode[6] = 0;
		pStockInfo->SetStartEndDate (m_llEndDate, -1);
		pStockInfo->SetCode (szCode);

		nIndex = 0;
		m_pBuySellInfo[i].dBuyPrice = 0;
		dMaxPrice = 0;
		dMinPrice = 1000;
		pos = pStockInfo->m_lstItem.GetHeadPosition ();
		while (pos != NULL)
		{
			pItem = pStockInfo->m_lstItem.GetNext (pos);
			nStockDay = qcGetDaysFrom2000 (pItem->m_nYear, pItem->m_nMonth, pItem->m_nDay);
			if (m_pBuySellInfo[i].dBuyPrice == 0)
			{
				m_pBuySellInfo[i].dBuyPrice = pItem->m_dClose;
				memset (&m_pBuySellInfo[i].tmBuy, 0, sizeof (SYSTEMTIME));
				m_pBuySellInfo[i].tmBuy.wYear = pItem->m_nYear;
				m_pBuySellInfo[i].tmBuy.wMonth = pItem->m_nMonth;
				m_pBuySellInfo[i].tmBuy.wDay = pItem->m_nDay;
			}
			nIndex++;
			if (nIndex == 1)
				continue;

			if (dMaxPrice < pItem->m_dMax)
				dMaxPrice = pItem->m_dMax;
			if (dMinPrice > pItem->m_dMin)
				dMinPrice = pItem->m_dMin;

			// 1. 跌破五日均线卖掉。
			if (nStockDay - nStartDay >= 5)
			{
				if (pItem->m_dMin < pItem->m_pDayLine->m_dLine5 && pItem->m_pDayLine->m_dLine5 < dPrevLine5)
				{
					m_pBuySellInfo[i].dSellPrice = pItem->m_pDayLine->m_dLine5;
					break;
				}
			}
			dPrevLine5 = pItem->m_pDayLine->m_dLine5;

			// 2. 跌破 10 个百分点卖掉
			if (pItem->m_dMin < m_pBuySellInfo[i].dBuyPrice * 0.9)
			{
				m_pBuySellInfo[i].dSellPrice = m_pBuySellInfo[i].dBuyPrice * 0.9;
				break;
			}

			// 3. 涨 8 个百分点卖掉
			if (pItem->m_dMax > m_pBuySellInfo[i].dBuyPrice * 1.08)
			{
//				m_pBuySellInfo[i].dSellPrice = m_pBuySellInfo[i].dBuyPrice * 1.08;
//				break;
			}

			if (nStockDay >= nCheckDay)
			{
			// 4. 超过一个月卖掉
				m_pBuySellInfo[i].dSellPrice = pItem->m_dClose;
				break;
			}
		}
		memset (&m_pBuySellInfo[i].tmSell, 0, sizeof (SYSTEMTIME));
		m_pBuySellInfo[i].tmSell.wYear = pItem->m_nYear;
		m_pBuySellInfo[i].tmSell.wMonth = pItem->m_nMonth;
		m_pBuySellInfo[i].tmSell.wDay = pItem->m_nDay;
	}
	delete pStockInfo;

	double dTotalDiff = 0;
	for (i = 0; i < nNum; i++)
	{
		SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_GETTEXT, i, (LPARAM)szLine);
		szLine[6] = 0;
		FormatDouble (m_pBuySellInfo[i].dBuyPrice, szItem);
		strcat (szLine, szItem + 1);
		FormatDouble (m_pBuySellInfo[i].dSellPrice, szItem);
		strcat (szLine, szItem + 1);
		dDiffPrice = (m_pBuySellInfo[i].dSellPrice - m_pBuySellInfo[i].dBuyPrice) * 100 / m_pBuySellInfo[i].dBuyPrice;
		FormatDouble (dDiffPrice, szItem);
		strcat (szLine, szItem + 1);
		SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_DELETESTRING, i, 0);
		SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_INSERTSTRING, i, (LPARAM)szLine);

		dTotalDiff += dDiffPrice;
	}
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_SETHORIZONTALEXTENT, 500, 0);

	dTotalDiff = dTotalDiff / nNum;
	strcpy (szLine, "Total: ");
	FormatDouble (dTotalDiff, szItem);
	strcat (szLine, szItem);
	SetWindowText (GetDlgItem (m_hDlg, IDC_STATIC_TOTAL), szLine);

	return QC_ERR_NONE;
}
