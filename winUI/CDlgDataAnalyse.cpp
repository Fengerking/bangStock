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
#include "CStockHistInfo.h"
#include "CStockFileHist.h"

#include "CDlgDataAnalyse.h"

#include "CRegMng.h"
#include "Resource.h"

#include "USystemFunc.h"

#define DLG_RIGHT_WIDTH		350

CDlgDataAnalyse::CDlgDataAnalyse(HINSTANCE hInst, HWND hParent)
	: m_hInst (hInst)
	, m_hParent (hParent)
	, m_hDlg (NULL)
	, m_nTimerProcess (0)
	, m_pCodeList (NULL)
	, m_pWndKXTInfo (NULL)
	, m_pStockKXTInfo (NULL)
{
}

CDlgDataAnalyse::~CDlgDataAnalyse(void)
{
	QC_DEL_P (m_pWndKXTInfo);
	if (m_hDlg != NULL)
		DestroyWindow (m_hDlg);
	QC_DEL_P (m_pStockKXTInfo);
	QC_DEL_P (m_pCodeList);
}

bool CDlgDataAnalyse::CheckStockInfo (CStockKXTInfo * pStockInfo)
{
	if (pStockInfo == NULL)
		return false;

	qcStockKXTInfoItem * pItem = NULL;
	char	szValue[32];
	double	dValue = 0;
	int		nDays = 0;
	int		nIndex = 1;
	NODEPOS	pos = NULL;
	char	szResult[64];
	// Today diff rate
	int nCheck = SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_TODAY_DIFFRATE), BM_GETCHECK, 0, 0);
	if (nCheck == BST_CHECKED)
	{
		GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_TODAY_DIFFRATE), szValue, sizeof (szValue));
		dValue = atof (szValue);
		pItem = pStockInfo->m_lstItem.GetTail ();
		if (pItem == NULL)
			return false;
		if (dValue > 0 && pItem->m_dDiffRate < dValue)
			return false;
		if (dValue < 0 && pItem->m_dDiffRate > dValue)
			return false;
	}
	// last days diff rate
	nCheck = SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LAST_DIFFRATE), BM_GETCHECK, 0, 0);
	if (nCheck == BST_CHECKED)
	{
		GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_LAST_DIFFRATE), szValue, sizeof (szValue));
		dValue = atof (szValue);
		GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_LAST_DIFF_DAYS), szValue, sizeof (szValue));
		nDays = atoi (szValue);

		int		nIndex = 0;
		int		nMaxIndex = 0;
		int		nMinIndex = 0;
		double	dMinPrice = 10000.0;
		double	dMaxPrice = 0;
		double	dNowPrice = 0;
		pos = pStockInfo->m_lstItem.GetTailPosition ();
		while (pos != NULL)
		{
			pItem = pStockInfo->m_lstItem.GetPrev (pos);
			if (dNowPrice == 0)
				dNowPrice = pItem->m_dClose;
			if (dMaxPrice < pItem->m_dClose)
			{
				dMaxPrice = pItem->m_dClose;
				nMaxIndex = nIndex;
			}
			if (dMinPrice > pItem->m_dClose)
			{
				dMinPrice = pItem->m_dClose;
				nMinIndex = nIndex;
			}
			nIndex++;
			if (nIndex > nDays)
				break;
		}
		if (dValue < 0)
		{
			if (((dMaxPrice - dMinPrice) * 100 / dNowPrice) < -dValue)
				return false;
			if (nMaxIndex < nMinIndex)
				return false;
		}
		if (dValue > 0)
		{
			if (((dMaxPrice - dMinPrice) * 100 / dNowPrice) < dValue)
				return false;
			if (nMaxIndex > nMinIndex)
				return false;
		}
	}
	// today exchange rate
	nCheck = SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_TODAY_EXCHANGE), BM_GETCHECK, 0, 0);
	if (nCheck == BST_CHECKED)
	{
		GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_TODAY_EXCHANGE), szValue, sizeof (szValue));
		dValue = atof (szValue);
		pItem = pStockInfo->m_lstItem.GetTail ();
		if (pItem == NULL)
			return false;
		if (dValue > 0 && pItem->m_dExchange < dValue)
			return false;
		if (dValue < 0 && pItem->m_dExchange > dValue)
			return false;
	}
	// last days total exchange
	nCheck = SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_LAST_EXCHANGE), BM_GETCHECK, 0, 0);
	if (nCheck == BST_CHECKED)
	{
		GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_LAST_EXCHANGE), szValue, sizeof (szValue));
		dValue = atof (szValue);
		GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_LAST_EXCHANGE), szValue, sizeof (szValue));
		nDays = atoi (szValue);

		int		nIndex = 0;
		double	dTotalExch = 0;
		pos = pStockInfo->m_lstItem.GetTailPosition ();
		while (pos != NULL)
		{
			pItem = pStockInfo->m_lstItem.GetPrev (pos);
			dTotalExch = dTotalExch + pItem->m_dExchange;
			nIndex++;
			if (nIndex > nDays)
				break;
		}
		if (dValue > 10)
		{
			if (dTotalExch < dValue)
				return false;
		}
		else
		{
			if (dTotalExch > dValue)
				return false;
		}
	}
	// total money
	nCheck = SendMessage (GetDlgItem (m_hDlg, IDC_CHECK_TOTAL_MONEY), BM_GETCHECK, 0, 0);
	if (nCheck == BST_CHECKED)
	{
		long long	llMoney = 0;
		GetWindowText (GetDlgItem (m_hDlg, IDC_COMBO_TOTAL_MONEY), szValue, sizeof (szValue));
		llMoney = atoi (szValue);
		pItem = pStockInfo->m_lstItem.GetTail ();
		if (pItem == NULL)
			return false;
		long long llItemMoney = (long long)(pItem->m_nMoney / pItem->m_dExchange) / 1000000;
		if (llItemMoney > llMoney)
			return false;
	}
	pItem = pStockInfo->m_lstItem.GetTail ();
	nIndex = SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_GETCOUNT, 0, 0) + 1;
	sprintf (szResult, "% 3d  %s", nIndex, m_pCodeList->GetSelCode ());
	if (pItem->m_dClose < 10)
		sprintf (szResult, "%s    %.2f", szResult, pItem->m_dClose);
	else if (pItem->m_dClose < 100)
		sprintf (szResult, "%s   %.2f", szResult, pItem->m_dClose);
	else
		sprintf (szResult, "%s  %.2f", szResult, pItem->m_dClose);
	if (pItem->m_dDiffRate > 0)
	{
		if (pItem->m_dDiffRate < 10)
			sprintf (szResult, "%s     %.2f", szResult, pItem->m_dDiffRate);
		else
			sprintf (szResult, "%s    %.2f", szResult, pItem->m_dDiffRate);
	}
	else 
	{
		if (pItem->m_dDiffRate < -10)
			sprintf (szResult, "%s   %.2f", szResult, pItem->m_dDiffRate);
		else
			sprintf (szResult, "%s    %.2f", szResult, pItem->m_dDiffRate);
	}
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_ADDSTRING, NULL, (LPARAM)szResult);
	return true;
}

LRESULT CDlgDataAnalyse::OnSelItemChanged (void)
{
	char szCode[64];
	memset (szCode, 0, sizeof (szCode));
	int nIndex = SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_GETCURSEL, 0, 0);
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_RESULT), LB_GETTEXT, nIndex, (LPARAM)szCode);
	char * pPos = szCode + 4;
	while (*pPos++ == ' ');
	pPos--;
	*(pPos + 6) = 0;

	m_pWndKXTInfo->SetCode (pPos);

	return S_OK;
}

LRESULT	CDlgDataAnalyse::OnTimer (WPARAM wParam, LPARAM lParam)
{
	int nCurSel = m_pCodeList->GetCurSel ();
	SendMessage (GetDlgItem (m_hDlg, IDC_PROGRESS_STATUS), PBM_SETPOS, (WPARAM) nCurSel, 0); 

	CStockKXTInfo * pStockInfo = m_pWndKXTInfo->GetKXTInfo ();
	m_pWndKXTInfo->SetCode (m_pCodeList->GetSelCode ());
	CheckStockInfo (pStockInfo);

	nCurSel++;
	if (nCurSel >= m_pCodeList->GetCodeCount())
	{
		KillTimer (m_hDlg, m_nTimerProcess);
		m_nTimerProcess = 0;
		SetWindowText (GetDlgItem (m_hDlg, IDC_BUTTON_START), "开  始");
		MessageBox (m_hDlg, "The analysis finished.", "Information", MB_OK);
		return S_FALSE;
	}
	m_pCodeList->SetCurSel (nCurSel);

	return S_OK;
}

int CDlgDataAnalyse::CreateDlg (void)
{
	m_hDlg = CreateDialog (m_hInst, MAKEINTRESOURCE(IDD_DIALOG_DATA_ANALYSE), m_hParent, DataAnalyseDlgProc);
	if (m_hDlg == NULL)
		return -1;
	SetWindowLong (m_hDlg, GWL_USERDATA, (LONG)this);
	//CBaseGraphics::OnCreateWnd (m_hDlg);

	RECT rcWnd;
	GetClientRect (m_hParent, &rcWnd);
	SetWindowPos (m_hDlg, NULL, rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, 0);

	rcWnd.right -= DLG_RIGHT_WIDTH;
	m_pWndKXTInfo = new CWndKXTInfo (m_hInst);
	m_pWndKXTInfo->CreateWnd (m_hDlg, rcWnd, MSC_BLACK);
	m_pWndKXTInfo->SetCode (CRegMng::g_pRegMng->GetTextValue ("LastCode"));

	InitParam ();

	m_pCodeList = new CStockFileCode ();
	m_pCodeList->Open ("codeList.txt");
	m_pCodeList->SetCurSel (0);

	SendMessage (GetDlgItem (m_hDlg, IDC_PROGRESS_STATUS), PBM_SETRANGE, 0, MAKELPARAM(0, m_pCodeList->GetCodeCount ())); 
	SendMessage (GetDlgItem (m_hDlg, IDC_PROGRESS_STATUS), PBM_SETSTEP, (WPARAM) 1, 0); 
	SendMessage (GetDlgItem (m_hDlg, IDC_PROGRESS_STATUS), PBM_SETPOS, (WPARAM) 0, 0); 

	return 0;
}

void CDlgDataAnalyse::InitParam (void)
{
	int		i = 0;
	char *	szRate[] = {"9.9", "9.0", "7.0", "5.0", "-5.0", "-7.0", "-9.0", "-9.9"};
	for (i = 0; i < 8; i++)
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_TODAY_DIFFRATE), CB_ADDSTRING, NULL, (LPARAM)szRate[i]);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_TODAY_DIFFRATE), CB_SETCURSEL, 0, NULL);
	char *	szDiffDays[] = {"5", "10", "20", "30", "60", "90", "120", "180", "360"};
	for (i = 0; i < 9; i++)
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LAST_DIFF_DAYS), CB_ADDSTRING, NULL, (LPARAM)szDiffDays[i]);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LAST_DIFF_DAYS), CB_SETCURSEL, 3, NULL);
	char *	szDiffRate[] = {"50", "30", "20", "10", "-10", "-20", "-30", "-50", "70"};
	for (i = 0; i < 9; i++)
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LAST_DIFFRATE), CB_ADDSTRING, NULL, (LPARAM)szDiffRate[i]);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LAST_DIFFRATE), CB_SETCURSEL, 6, NULL);
	for (i = 0; i < 9; i++)
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LAST_EXCH_DAYS), CB_ADDSTRING, NULL, (LPARAM)szDiffDays[i]);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LAST_EXCH_DAYS), CB_SETCURSEL, 3, NULL);

	char *	szExchange[] = {"70", "50", "40", "30", "20", "0.2", "0.1", "0.05"};
	for (i = 0; i < 8; i++)
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_TODAY_EXCHANGE), CB_ADDSTRING, NULL, (LPARAM)szExchange[i]);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_TODAY_EXCHANGE), CB_SETCURSEL, 1, NULL);

	char *	szExchTotal[] = {"300", "200", "150", "100", "80", "50", "0.2", "0.1"};
	for (i = 0; i < 8; i++)
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LAST_EXCHANGE), CB_ADDSTRING, NULL, (LPARAM)szExchTotal[i]);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_LAST_EXCHANGE), CB_SETCURSEL, 3, NULL);

	char *	szTotalMoney[] = {"10", "20", "50", "100", "150", "200", "300", "500"};
	for (i = 0; i < 8; i++)
		SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_TOTAL_MONEY), CB_ADDSTRING, NULL, (LPARAM)szTotalMoney[i]);
	SendMessage (GetDlgItem (m_hDlg, IDC_COMBO_TOTAL_MONEY), CB_SETCURSEL, 3, NULL);
}

LRESULT CDlgDataAnalyse::OnResize (void)
{
	int		nX, nY, nW = 0, nH = 0;
	RECT	rcDlg;
	GetClientRect (m_hDlg, &rcDlg);

	nX = rcDlg.right - DLG_RIGHT_WIDTH;
	nY = 10;
	nW = DLG_RIGHT_WIDTH - 8;
	nH = 350;
	HWND hCtrl = GetDlgItem (m_hDlg, IDC_STATIC_CONDITION);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, 0);

	nX += 10;
	nY += 30;
	hCtrl = GetDlgItem (m_hDlg, IDC_CHECK_TODAY_DIFFRATE);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, SWP_NOSIZE);
	nY += 45;
	hCtrl = GetDlgItem (m_hDlg, IDC_CHECK_LAST_DIFFRATE);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, SWP_NOSIZE);
	nY += 45;
	hCtrl = GetDlgItem (m_hDlg, IDC_CHECK_TODAY_EXCHANGE);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, SWP_NOSIZE);
	nY += 45;
	hCtrl = GetDlgItem (m_hDlg, IDC_CHECK_LAST_EXCHANGE);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, SWP_NOSIZE);
	nY += 45;
	hCtrl = GetDlgItem (m_hDlg, IDC_CHECK_TOTAL_MONEY);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, SWP_NOSIZE);


	nX = rcDlg.right - 110;
	nY = 45;
	hCtrl = GetDlgItem (m_hDlg, IDC_COMBO_TODAY_DIFFRATE);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, SWP_NOSIZE);
	nY += 45;
	hCtrl = GetDlgItem (m_hDlg, IDC_COMBO_LAST_DIFFRATE);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, SWP_NOSIZE);
	nY += 45;
	hCtrl = GetDlgItem (m_hDlg, IDC_COMBO_TODAY_EXCHANGE);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, SWP_NOSIZE);
	nY += 45;
	hCtrl = GetDlgItem (m_hDlg, IDC_COMBO_LAST_EXCHANGE);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, SWP_NOSIZE);
	nY += 45;
	hCtrl = GetDlgItem (m_hDlg, IDC_COMBO_TOTAL_MONEY);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, SWP_NOSIZE);

	nX = rcDlg.right - 208;
	nY = 90;
	hCtrl = GetDlgItem (m_hDlg, IDC_COMBO_LAST_DIFF_DAYS);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, SWP_NOSIZE);
	nY += 90;
	hCtrl = GetDlgItem (m_hDlg, IDC_COMBO_LAST_EXCH_DAYS);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, SWP_NOSIZE);

	nX = rcDlg.right - DLG_RIGHT_WIDTH + 10;
	nY = 45 * 6;
	hCtrl = GetDlgItem (m_hDlg, IDC_PROGRESS_STATUS);
	nW = 320;
	nH = 25;
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, 0);
	nX = rcDlg.right - DLG_RIGHT_WIDTH + 120;
	nY += 40;
	hCtrl = GetDlgItem (m_hDlg, IDC_BUTTON_START);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, SWP_NOSIZE);

	nX = rcDlg.right - DLG_RIGHT_WIDTH;
	nY = 360;
	nW = DLG_RIGHT_WIDTH - 8;
	nH = rcDlg.bottom - nY - 8;
	hCtrl = GetDlgItem (m_hDlg, IDC_STATIC_RESULT);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, 0);

	nX = rcDlg.right - DLG_RIGHT_WIDTH + 16;
	nY = 390;
	hCtrl = GetDlgItem (m_hDlg, IDC_BUTTON_RESET);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, SWP_NOSIZE);
	nX += 110;
	hCtrl = GetDlgItem (m_hDlg, IDC_BUTTON_COPY);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, SWP_NOSIZE);
	nX += 110;
	hCtrl = GetDlgItem (m_hDlg, IDC_BUTTON_SAVE);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, SWP_NOSIZE);


	nX = rcDlg.right - DLG_RIGHT_WIDTH + 8;
	nY = 428;
	nW = DLG_RIGHT_WIDTH - 24;
	nH = rcDlg.bottom - nY - 24;
	hCtrl = GetDlgItem (m_hDlg, IDC_LIST_RESULT);
	SetWindowPos (hCtrl, NULL, nX, nY, nW, nH, 0);

	nX = 0;
	nY = 0;
	nW = rcDlg.right - DLG_RIGHT_WIDTH - 8;
	nH = rcDlg.bottom;
	SetWindowPos (m_pWndKXTInfo->GetWnd (), NULL, nX, nY, nW, nH, 0);

	return S_OK;
}


INT_PTR CALLBACK CDlgDataAnalyse::DataAnalyseDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int					wmId, wmEvent;
	CDlgDataAnalyse *	pDlgAnalyse = NULL;

	if (hDlg != NULL)
		pDlgAnalyse = (CDlgDataAnalyse *)GetWindowLong (hDlg, GWL_USERDATA);

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_SIZE:
		if (pDlgAnalyse != NULL)
			pDlgAnalyse->OnResize ();
		break;

	case WM_TIMER:
		if (pDlgAnalyse != NULL)
			pDlgAnalyse->OnTimer (wParam, lParam);
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		if (wmEvent == LBN_SELCHANGE)
		{
			if (pDlgAnalyse != NULL)
				return pDlgAnalyse->OnSelItemChanged ();
		}

		// Parse the menu selections:
		switch (wmId)
		{
		case IDC_BUTTON_START:
			if (pDlgAnalyse != NULL)
			{
				if (pDlgAnalyse->m_nTimerProcess == 0)
				{
					SendMessage (GetDlgItem (hDlg, IDC_LIST_RESULT), LB_RESETCONTENT, 0, 0);
					SetWindowText (GetDlgItem (hDlg, IDC_BUTTON_START), "暂  停");
					pDlgAnalyse->m_nTimerProcess = SetTimer (hDlg, WM_TIMER_PROCESS, 10, NULL);
				}
				else
				{
					SetWindowText (GetDlgItem (hDlg, IDC_BUTTON_START), "开  始");
					KillTimer (hDlg, pDlgAnalyse->m_nTimerProcess);
					pDlgAnalyse->m_nTimerProcess = 0;
				}
			}
			break;

		case IDC_BUTTON_RESET:
			if (pDlgAnalyse != NULL)
				pDlgAnalyse->OnButtonReset ();
			break;

		case IDC_BUTTON_COPY:
			if (pDlgAnalyse != NULL)
				pDlgAnalyse->OnButtonCopy ();
			break;

		case IDC_BUTTON_SAVE:
			if (pDlgAnalyse != NULL)
				pDlgAnalyse->OnButtonSave ();
			break;

		case IDOK:
		case IDCANCEL:
			DestroyWindow (hDlg);
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}

	return (INT_PTR)FALSE;
}

void CDlgDataAnalyse::OnButtonReset (void)
{
	if (m_pCodeList != NULL)
	{
		m_pCodeList->SetCurSel (0);
		SendMessage (GetDlgItem (m_hDlg, IDC_PROGRESS_STATUS), PBM_SETPOS, 0, 0); 
	}
}

void CDlgDataAnalyse::OnButtonCopy (void)
{
	char	szResult[4096];
	char	szItem[64];
	HWND	hCtrl = GetDlgItem (m_hDlg, IDC_LIST_RESULT);
	int		nCount = SendMessage (hCtrl, LB_GETCOUNT, 0, 0);
	for (int i = 0; i < nCount; i++)
	{
		SendMessage (hCtrl, LB_GETTEXT, i, (LPARAM)szItem);
		strcat (szResult, szItem);
		strcat (szResult, "\r\n");
	}
	qcCopyToClipBoard (szResult);
}

void CDlgDataAnalyse::OnButtonSave (void)
{

}
