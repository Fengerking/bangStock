/*******************************************************************************
	File:		CWndMng.cpp

	Contains:	base object implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#include "stdio.h"

#include "CWndMng.h"
#include "CWndKXTFile.h"

#include "CStockHistInfo.h"
#include "CStockRTList.h"

#include "CDlgDownLoad.h"
#include "CDlgConfig.h"
#include "CDlgStockInfo.h"

#include "resource.h"
#include "USystemFunc.h"

CWndMng::CWndMng(HINSTANCE hInst)
	: m_hInst (hInst)
	, m_hMainWnd (NULL)
	, m_pWndRTInfo (NULL)
	, m_pWndDayInfo (NULL)
	, m_pWndKXTInfo (NULL)
	, m_pWndSelect (NULL)
	, m_pDlgDataAnalyse (NULL)
	, m_pDlgKXTStudy (NULL)
	, m_pWndKXTView (NULL)
	, m_pWndKXTSelect (NULL)
	, m_pWndDayInfo1 (NULL)
	, m_pWndDayInfo2 (NULL)
	, m_pWndDayInfo3 (NULL)
	, m_pWndDayInfo4 (NULL)
	, m_pWndCompInfo (NULL)
	, m_nShowWnd (WND_STOCK_KXT_SELECT)
	, m_pStockMng (NULL)
{
	m_pRegMng = new CRegMng ("Setting");
	if (qcIsTradeTime ())
		m_nShowWnd = WND_STOCK_FST;
}

CWndMng::~CWndMng(void)
{
	if (m_pWndRTInfo != NULL)
		m_pRegMng->SetTextValue ("LastCode", m_pWndRTInfo->GetCode ());

	QC_DEL_P (m_pWndRTInfo);
	QC_DEL_P (m_pWndDayInfo);
	QC_DEL_P (m_pWndKXTInfo);
	QC_DEL_P (m_pWndSelect);
	QC_DEL_P (m_pDlgDataAnalyse);
	QC_DEL_P (m_pDlgKXTStudy);
	QC_DEL_P (m_pWndKXTView);
	QC_DEL_P (m_pWndKXTSelect);

	QC_DEL_P (m_pWndDayInfo1);
	QC_DEL_P (m_pWndDayInfo2);
	QC_DEL_P (m_pWndDayInfo3);
	QC_DEL_P (m_pWndDayInfo4);

	QC_DEL_P (m_pWndCompInfo);

	QC_DEL_P (m_pStockMng);

	delete m_pRegMng;
}

int	CWndMng::OnCreateWnd (HWND hWnd)
{
	m_hMainWnd = hWnd;

	m_pWndRTInfo = new CWndRTInfo (m_hInst);
	m_pWndKXTInfo = new CWndKXTInfo (m_hInst);
	m_pWndDayInfo = new CWndDayInfo (m_hInst);
	m_pWndSelect = new CWndSelection (m_hInst);

	RECT rcWnd;
	GetClientRect (m_hMainWnd, &rcWnd);
	rcWnd.left = rcWnd.right - m_pWndRTInfo->GetWndWidth ();
	m_pWndRTInfo->CreateWnd (m_hMainWnd, rcWnd, MSC_BLACK);

	rcWnd.right = rcWnd.left;
	rcWnd.left = 0;
	m_pWndDayInfo->CreateWnd (m_hMainWnd, rcWnd, MSC_BLACK);
	m_pWndKXTInfo->CreateWnd (m_hMainWnd, rcWnd, MSC_BLACK);

	GetClientRect (m_hMainWnd, &rcWnd);
	m_pWndSelect->CreateWnd (m_hMainWnd, rcWnd, MSC_BLACK);

//	m_pStockMng = new CStockMng ();
//	m_pStockMng->Init (m_hMainWnd);

	ShowStockWnd ();

	char * pCode = m_pRegMng->GetTextValue ("LastCode");
	if (pCode != NULL && strlen (pCode) == 6 && m_pWndRTInfo != NULL)
		m_pWndRTInfo->SetCode (pCode);

	return QC_ERR_NONE;
}

LRESULT	CWndMng::OnResize (void)
{
	if (m_hMainWnd == NULL)
		return S_OK;

	RECT rcWnd;
	GetClientRect (m_hMainWnd, &rcWnd);
	rcWnd.left = rcWnd.right - m_pWndRTInfo->GetWndWidth ();
	SetWindowPos (m_pWndRTInfo->GetWnd (), NULL, rcWnd.left, rcWnd.top, rcWnd.right -rcWnd.left, rcWnd.bottom - rcWnd.top, 0);	
	rcWnd.right = rcWnd.left;
	rcWnd.left = 0;
	if (m_nShowWnd == WND_STOCK_FST_KXT)
	{
		SetWindowPos (m_pWndDayInfo->GetWnd (), NULL, rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom / 2, SWP_NOZORDER);
		SetWindowPos (m_pWndKXTInfo->GetWnd (), NULL, rcWnd.left, rcWnd.bottom / 2, rcWnd.right - rcWnd.left, rcWnd.bottom / 2, SWP_NOZORDER);
	}
	else
	{
		SetWindowPos (m_pWndKXTInfo->GetWnd (), NULL, rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom, SWP_NOZORDER);
		SetWindowPos (m_pWndDayInfo->GetWnd (), NULL, rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom, SWP_NOZORDER);
	}
	GetClientRect (m_hMainWnd, &rcWnd);
	SetWindowPos (m_pWndSelect->GetWnd (), NULL, rcWnd.left, rcWnd.top, rcWnd.right - rcWnd.left, rcWnd.bottom, SWP_NOZORDER);

	GetClientRect (m_hMainWnd, &rcWnd);
	if (m_pWndDayInfo1 != NULL)
	{
		SetWindowPos (m_pWndDayInfo1->GetWnd (), NULL, rcWnd.left, rcWnd.top, rcWnd.right/2, rcWnd.bottom/2, SWP_NOZORDER);
		SetWindowPos (m_pWndDayInfo2->GetWnd (), NULL, rcWnd.right/2, rcWnd.top, rcWnd.right/2, rcWnd.bottom/2, SWP_NOZORDER);
		SetWindowPos (m_pWndDayInfo3->GetWnd (), NULL, rcWnd.left, rcWnd.bottom/2, rcWnd.right/2, rcWnd.bottom/2, SWP_NOZORDER);
		SetWindowPos (m_pWndDayInfo4->GetWnd (), NULL, rcWnd.right/2, rcWnd.bottom/2, rcWnd.right/2, rcWnd.bottom/2, SWP_NOZORDER);
	}
	if (m_pDlgDataAnalyse != NULL && m_pDlgDataAnalyse->GetDlg () != NULL)
		SetWindowPos (m_pDlgDataAnalyse->GetDlg (), NULL, rcWnd.left, rcWnd.top, rcWnd.right -rcWnd.left, rcWnd.bottom - rcWnd.top, 0);
	if (m_pDlgKXTStudy != NULL && m_pDlgKXTStudy->GetDlg () != NULL)
		SetWindowPos (m_pDlgKXTStudy->GetDlg (), NULL, rcWnd.left, rcWnd.top, rcWnd.right -rcWnd.left, rcWnd.bottom - rcWnd.top, 0);
	if (m_pWndKXTView != NULL && m_pWndKXTView->GetWnd () != NULL)
		SetWindowPos (m_pWndKXTView->GetWnd (), NULL, rcWnd.left, rcWnd.top, rcWnd.right -rcWnd.left, rcWnd.bottom - rcWnd.top, 0);
	if (m_pWndKXTSelect != NULL && m_pWndKXTSelect->GetWnd () != NULL)
		SetWindowPos (m_pWndKXTSelect->GetWnd (), NULL, rcWnd.left, rcWnd.top, rcWnd.right -rcWnd.left, rcWnd.bottom - rcWnd.top, 0);

	if (m_pWndCompInfo != NULL && m_pWndCompInfo->GetWnd () != NULL)
	{
		GetClientRect (m_hMainWnd, &rcWnd);
		SetWindowPos (m_pWndCompInfo->GetWnd (), NULL, rcWnd.left + 200, rcWnd.top, rcWnd.right - rcWnd.left - 400, rcWnd.bottom - rcWnd.top, 0);
	}
	return S_FALSE;
}

void CWndMng::ShowStockWnd (void)
{
	ShowWindow (m_pWndRTInfo->GetWnd (), SW_HIDE);
	ShowWindow (m_pWndDayInfo->GetWnd (), SW_HIDE);
	ShowWindow (m_pWndKXTInfo->GetWnd (), SW_HIDE);
	ShowWindow (m_pWndSelect->GetWnd (), SW_HIDE);
	OnResize ();

	if (m_nShowWnd != WND_STOCK_DAY)
	{
		QC_DEL_P (m_pWndDayInfo1);
		QC_DEL_P (m_pWndDayInfo2);
		QC_DEL_P (m_pWndDayInfo3);
		QC_DEL_P (m_pWndDayInfo4);
	}
	if (m_nShowWnd != WND_STOCK_DATA_ANALYSE)
		QC_DEL_P (m_pDlgDataAnalyse);
	if (m_nShowWnd != WND_STOCK_KXT_STUDY)
		QC_DEL_P (m_pDlgKXTStudy);
	if (m_nShowWnd != WND_STOCK_KXT_RESEARCH)
		QC_DEL_P (m_pWndKXTView);
	if (m_nShowWnd != WND_STOCK_KXT_SELECT)
		QC_DEL_P (m_pWndKXTSelect);

	if (m_nShowWnd == WND_STOCK_FST)
	{
		ShowWindow (m_pWndRTInfo->GetWnd (), SW_SHOW);
		ShowWindow (m_pWndDayInfo->GetWnd (), SW_SHOW);
	}
	else if (m_nShowWnd == WND_STOCK_KXT)
	{
		ShowWindow (m_pWndRTInfo->GetWnd (), SW_SHOW);
		ShowWindow (m_pWndKXTInfo->GetWnd (), SW_SHOW);
	}
	else if (m_nShowWnd == WND_STOCK_FST_KXT)
	{
		ShowWindow (m_pWndRTInfo->GetWnd (), SW_SHOW);
		ShowWindow (m_pWndKXTInfo->GetWnd (), SW_SHOW);
		ShowWindow (m_pWndDayInfo->GetWnd (), SW_SHOW);
	}
	else if (m_nShowWnd == WND_STOCK_SEL_LIKE)
	{
		m_pWndSelect->SetSelectType (NULL, WND_SEL_TYPE_LIKE);
		ShowWindow (m_pWndSelect->GetWnd (), SW_SHOW);
	}
	else if (m_nShowWnd == WND_STOCK_SEL_BUY)
	{
		m_pWndSelect->SetSelectType (NULL, WND_SEL_TYPE_BUY);
		ShowWindow (m_pWndSelect->GetWnd (), SW_SHOW);
	}
	else if (m_nShowWnd == WND_STOCK_DAY)
	{
		RECT rcWnd;
		RECT rcDayWnd;
		GetClientRect (m_hMainWnd, &rcWnd);
		SetRect (&rcDayWnd, rcWnd.left, rcWnd.top, rcWnd.right/2, rcWnd.bottom/2);
		m_pWndDayInfo1 = new CWndDayInfo (m_hInst);
		m_pWndDayInfo1->CreateWnd (m_hMainWnd, rcDayWnd, MSC_BLACK);
		m_pWndDayInfo1->SetCode (m_pRegMng->GetTextValue ("DayInfo1"));
		SetRect (&rcDayWnd, rcWnd.right/2, rcWnd.top, rcWnd.right, rcWnd.bottom/2);
		m_pWndDayInfo2 = new CWndDayInfo (m_hInst);
		m_pWndDayInfo2->CreateWnd (m_hMainWnd, rcDayWnd, MSC_BLACK);	
		m_pWndDayInfo2->SetCode (m_pRegMng->GetTextValue ("DayInfo2"));
		SetRect (&rcDayWnd, rcWnd.left, rcWnd.bottom/2, rcWnd.right/2, rcWnd.bottom);
		m_pWndDayInfo3 = new CWndDayInfo (m_hInst);
		m_pWndDayInfo3->CreateWnd (m_hMainWnd, rcDayWnd, MSC_BLACK);	
		m_pWndDayInfo3->SetCode (m_pRegMng->GetTextValue ("DayInfo3"));
		SetRect (&rcDayWnd, rcWnd.right/2, rcWnd.bottom/2, rcWnd.right, rcWnd.bottom);
		m_pWndDayInfo4 = new CWndDayInfo (m_hInst);
		m_pWndDayInfo4->CreateWnd (m_hMainWnd, rcDayWnd, MSC_BLACK);	
		m_pWndDayInfo4->SetCode (m_pRegMng->GetTextValue ("DayInfo4"));
	}
	else if (m_nShowWnd == WND_STOCK_DATA_ANALYSE)
	{
		m_pDlgDataAnalyse = new CDlgDataAnalyse (m_hInst, m_hMainWnd);
		m_pDlgDataAnalyse->CreateDlg ();
	}
	else if (m_nShowWnd == WND_STOCK_KXT_STUDY)
	{
		m_pDlgKXTStudy = new CDlgKXTStudy (m_hInst, m_hMainWnd);
		m_pDlgKXTStudy->CreateDlg ();
	}
	else if (m_nShowWnd == WND_STOCK_KXT_RESEARCH)
	{
		RECT rcWnd;
		GetClientRect (m_hMainWnd, &rcWnd);
		m_pWndKXTView = new CWndKXTView (m_hInst);
		m_pWndKXTView->CreateWnd (m_hMainWnd, rcWnd, MSC_BLACK);
	}
	else if (m_nShowWnd == WND_STOCK_KXT_SELECT)
	{
		RECT rcWnd;
		GetClientRect (m_hMainWnd, &rcWnd);
		m_pWndKXTSelect = new CWndKXTSelect (m_hInst);
		m_pWndKXTSelect->CreateWnd (m_hMainWnd, rcWnd, MSC_BLACK);
	}
}

LRESULT	CWndMng::OnRButtonUp (UINT uMsg, WPARAM wParam, LPARAM lParam)
{
//	if (m_nShowWnd == WND_STOCK_KXT_RESEARCH)	
//		return SendMessage (m_pWndKXTView->GetWnd (), uMsg, wParam, lParam);
	if (m_nShowWnd == WND_STOCK_KXT_STUDY)	
		return SendMessage (m_pDlgKXTStudy->GetDlg (), uMsg, wParam, lParam);
	
	POINT pt;
	pt.x = LOWORD (lParam);
	pt.y = HIWORD (lParam);
	ClientToScreen (m_hMainWnd, &pt);

	HMENU	hMenu = GetMenu (m_hMainWnd);
	HMENU	hMenuView = GetSubMenu (hMenu, 1);

	TrackPopupMenu (hMenuView, TPM_LEFTALIGN, pt.x, pt.y, 0, m_hMainWnd, NULL);

	return S_OK;
}

LRESULT CWndMng::OnKeyUp (UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_HOME || wParam == VK_END)
	{
		if (m_nShowWnd <= WND_STOCK_FST_KXT)
		{
			m_nShowWnd = m_nShowWnd + 1;
			if (m_nShowWnd > WND_STOCK_FST_KXT)
				m_nShowWnd = WND_STOCK_FST;
			ShowStockWnd ();
		}
		return S_OK;
	}

	if (m_nShowWnd == WND_STOCK_KXT || m_nShowWnd == WND_STOCK_FST || m_nShowWnd == WND_STOCK_FST_KXT)
	{
		if (m_nShowWnd == WND_STOCK_KXT || m_nShowWnd == WND_STOCK_FST_KXT)
		{
			if (SendMessage (m_pWndKXTInfo->GetWnd (), WM_KEYUP, wParam, lParam) == S_OK)
				return S_OK;
		}
		if (m_pWndRTInfo != NULL)
			SendMessage (m_pWndRTInfo->GetWnd (), WM_KEYUP, wParam, lParam);

		if (wParam == VK_UP || wParam == VK_DOWN || wParam == VK_PRIOR || wParam == VK_NEXT)
		{
			if (wParam == VK_PRIOR)
				wParam = VK_UP;
			else if (wParam == VK_NEXT)
				wParam = VK_DOWN;

			SendMessage (m_pWndSelect->GetWnd (), uMsg, wParam, lParam);

			CStockFileCode * pCodeList = m_pWndSelect->GetCodeList ();
			if (pCodeList == NULL)
				return S_OK;
			char * pSelCode = pCodeList->GetSelCode ();
			if (pSelCode == NULL)
				return S_OK;
			if (m_pWndRTInfo != NULL)
				m_pWndRTInfo->SetCode (pSelCode);
			return S_OK;
		}
	}
	else if (m_nShowWnd >= WND_STOCK_SEL_LIKE && m_nShowWnd <= WND_STOCK_SEL_BUY)
	{
		if (m_pWndSelect != NULL)
			SendMessage (m_pWndSelect->GetWnd (), WM_KEYUP, wParam, lParam);
	}
	else if (m_nShowWnd == WND_STOCK_KXT_STUDY)
	{
		return SendMessage (m_pDlgKXTStudy->GetDlg (), uMsg, wParam, lParam);
	}
	else if (m_nShowWnd == WND_STOCK_KXT_RESEARCH)
	{
		return SendMessage (m_pWndKXTView->GetWnd (), uMsg, wParam, lParam);
	}
	else if (m_nShowWnd == WND_STOCK_KXT_SELECT)
	{
		return SendMessage (m_pWndKXTSelect->GetWnd (), uMsg, wParam, lParam);
	}

	return S_FALSE;
}

int CWndMng::OpenStockFile (int nType)
{
	char				szFile[256];
	DWORD				dwID = 0;
	OPENFILENAME		ofn;
	memset (szFile, 0, sizeof (szFile));
	memset( &(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = m_hMainWnd;
	ofn.lpstrFilter = TEXT("Stock File (*.txt)\0*.txt\0");	
	if (_tcsstr (szFile, _T(":/")) != NULL)
		_tcscpy (szFile, _T("*.txt"));
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = TEXT("Open Stock File");
	ofn.Flags = OFN_EXPLORER;
			
	if (!GetOpenFileName(&ofn))
		return QC_ERR_FAILED;	

	if (m_pWndSelect != NULL)
		m_pWndSelect->SetSelectType (szFile, nType);
	if (nType == WND_SEL_TYPE_LIKE)
		m_nShowWnd = WND_STOCK_SEL_LIKE;
	else
		m_nShowWnd = WND_STOCK_SEL_BUY;
	ShowStockWnd ();

	return QC_ERR_NONE;
}

LRESULT CWndMng::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (uMsg)
	{
	case WM_MSG_CODE_CHANGE:
		if (m_pWndDayInfo != NULL)
			m_pWndDayInfo->SetCode ((char *)wParam);
		if (m_pWndKXTInfo != NULL)
			m_pWndKXTInfo->SetCode ((char *)wParam);
		if ((LPARAM)m_pWndSelect == lParam)
		{
			if (m_pWndRTInfo != NULL)
				m_pWndRTInfo->SetCode ((char *)wParam);
			if (qcIsTradeTime ())
				m_nShowWnd = WND_STOCK_FST;
			else
				m_nShowWnd = WND_STOCK_KXT;
			ShowStockWnd ();
		}
		return S_OK;

	case WM_MSG_NEW_PRICE:
		if (m_pWndKXTInfo != NULL)
			m_pWndKXTInfo->OnNewPrice (wParam, lParam);
		return S_OK;

	case WM_KEYDOWN:
		if (OnKeyUp (WM_KEYUP, wParam, lParam) == S_OK)
			return S_OK;
		break;

	case WM_SIZE:
		OnResize ();
		break;

	case WM_LBUTTONDOWN:
		if (m_pWndCompInfo != NULL && m_pWndCompInfo->GetWnd () != NULL)
			m_pWndCompInfo->Close ();
		break;

	case WM_RBUTTONUP:
		OnRButtonUp (uMsg, wParam, lParam);
		return S_OK;

	case WM_MOUSEWHEEL:
		if (m_pWndCompInfo != NULL && m_pWndCompInfo->GetWnd () != NULL)
		{
			SendMessage (m_pWndCompInfo->GetWnd (), uMsg, wParam, lParam);
			return S_OK;
		}
		if (m_pWndSelect != NULL)
		{
			SendMessage (m_pWndSelect->GetWnd (), uMsg, wParam, lParam);
			if (m_nShowWnd == WND_STOCK_KXT || m_nShowWnd == WND_STOCK_FST)
			{
				CStockFileCode * pCodeList = m_pWndSelect->GetCodeList ();
				if (pCodeList == NULL)
					break;
				char * pSelCode = pCodeList->GetSelCode ();
				if (pSelCode == NULL)
					break;
				if (m_pWndRTInfo != NULL)
					m_pWndRTInfo->SetCode (pSelCode);
			}
		}
		break;

	case WM_CLOSE:
		return S_FALSE;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_FILE_STOCK_SELECT:
		{
			OpenStockFile (WND_SEL_TYPE_LIKE);
			break;
		}
		case ID_FILE_STOCK_BUY:
		{
			OpenStockFile (WND_SEL_TYPE_BUY);
			break;
		}

		case ID_FILE_DOWNLOAD:
		{
			CDlgDownLoad	dlgDown (m_hInst, m_hMainWnd);
			dlgDown.OpenDlg ();
			break;
		}
		case ID_FILE_CONFIG:
		{
			CDlgConfig	dlgConfig (m_hInst, m_hMainWnd);
			if (dlgConfig.OpenDlg () == IDOK)
			{
				if (m_pWndKXTInfo != NULL)
					m_pWndKXTInfo->UpdateDayLineSetting ();
				if (m_pWndKXTView != NULL)
					m_pWndKXTView->UpdateDayLineSetting ();
				if (m_pWndKXTSelect != NULL)
					m_pWndKXTSelect->UpdateDayLineSetting ();
			}
			break;
		}

		case ID_VIEW_KXPIC:
			m_nShowWnd = WND_STOCK_KXT;
			ShowStockWnd ();
			break;
		case ID_VIEW_FSPIC:
			m_nShowWnd = WND_STOCK_FST;
			ShowStockWnd ();
			break;
		case ID_VIEW_FSKXT:
			m_nShowWnd = WND_STOCK_FST_KXT;
			ShowStockWnd ();
			break;
		case ID_VIEW_DAYFOUR:
			m_nShowWnd = WND_STOCK_DAY;
			ShowStockWnd ();
			break;
		case ID_VIEW_MYSTOCK:
			m_nShowWnd = WND_STOCK_SEL_LIKE;
			ShowStockWnd ();
			break;
		case ID_STOCK_BUY:
			m_nShowWnd = WND_STOCK_SEL_BUY;
			ShowStockWnd ();
			break;

		case ID_VIEW_STOCKINFO:
			{
				if (m_pWndCompInfo == NULL)
					m_pWndCompInfo = new CWndCompInfo (m_hInst);
				if (m_nShowWnd == WND_STOCK_KXT_RESEARCH)	
					m_pWndCompInfo->SetCode (m_pWndKXTView->GetCode ());
				else if (m_nShowWnd == WND_STOCK_KXT_SELECT)	
					m_pWndCompInfo->SetCode (m_pWndKXTSelect->GetCode ());
				else	
					m_pWndCompInfo->SetCode (m_pWndRTInfo->GetCode ());
				RECT rcWnd;
				GetClientRect (m_hMainWnd, &rcWnd);
				rcWnd.left += 200;
				rcWnd.right -= 200;
				m_pWndCompInfo->CreateWnd (m_hMainWnd, rcWnd, MSC_BLACK);
			}
			break;

		case ID_STOCK_AUTOSELECT:
			m_nShowWnd = WND_STOCK_DATA_ANALYSE;
			ShowStockWnd ();
			break;

		case ID_KXT_STUDY:
			m_nShowWnd = WND_STOCK_KXT_STUDY;
			ShowStockWnd ();
			break;
		case ID_KXT_RESEARCH:
			m_nShowWnd = WND_STOCK_KXT_RESEARCH;
			ShowStockWnd ();
			break;
		case ID_MYSELECT_STOCK:
			m_nShowWnd = WND_STOCK_KXT_SELECT;
			ShowStockWnd ();
			break;

		case ID_MYSTOCK_INFO:
		{
			CDlgStockInfo dlgInfo (m_hInst, m_hMainWnd);
			dlgInfo.OpenDlg (m_pWndRTInfo->GetCode ());
		}
			break;

		default:
			break;
		}

	default:
		break;
	}

	LRESULT lRC = S_FALSE;
//	if (m_pWndDayInfo != NULL)
//		lRC = SendMessage (m_pWndDayInfo->GetWnd (), uMsg, wParam, lParam);
//	if (m_pWndKXTInfo != NULL)
//		lRC = SendMessage (m_pWndKXTInfo->GetWnd (), uMsg, wParam, lParam);

	return lRC;
}
