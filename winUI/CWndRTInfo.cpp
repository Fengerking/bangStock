/*******************************************************************************
	File:		CWndRTInfo.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "CRegMng.h"

#include "CWndRTInfo.h"
#include "ULogFunc.h"
#include "USystemFunc.h"

CWndRTInfo::CWndRTInfo(HINSTANCE hInst)
	: CWndBase (hInst)
	, m_nWndWidth (398)
	, m_hEdtCode (NULL)
	, m_bNeedUpdate (false)
	, m_pRTInfo (NULL)
	, m_nFirstValume (0)
{
	_tcscpy (m_szClassName, _T("bangStockRTWindow"));
	_tcscpy (m_szWindowName, _T("bangStockRTWindow"));
	memset (m_szCode, 0, sizeof (m_szKeyCode));
}

CWndRTInfo::~CWndRTInfo(void)
{
	QC_DEL_P (m_pRTInfo);
	ReleaseHistory ();
}

int CWndRTInfo::SetCode (char * pCode)
{
	if (!strcmp (m_szCode, pCode))
		return QC_ERR_NONE;
	strcpy (m_szCode, pCode);
	ReleaseHistory ();
	strcpy (m_szKeyCode, "");
	m_bNeedUpdate = true;
	InvalidateRect (m_hWnd, NULL, FALSE);
	PostMessage (m_hParent, WM_MSG_CODE_CHANGE, (WPARAM)m_szCode, (LPARAM)this);
	return QC_ERR_NONE;
}

int CWndRTInfo::UpdateView (HDC hDC)
{
	if (m_bNeedUpdate)
	{
		if (m_pRTInfo == NULL)
			m_pRTInfo = new CStockRTInfo ();
		if (m_pRTInfo == NULL || hDC == NULL)
			return QC_ERR_STATUS;
		if (strcmp (m_szCode, m_pRTInfo->m_rtItem.m_szCode))
			m_nFirstValume = 0;
		if (m_pRTInfo->SetCode (m_szCode) != QC_ERR_NONE)
		{
			char szErr[256];
			sprintf (szErr, "The real time info window Get %s data Failed!", m_szCode);
			SetWindowText (m_hParent, szErr);
			//MessageBox (m_hParent, szErr, "Error", MB_OK);
			//return QC_ERR_FAILED;
		}
		m_dClosePrice = m_pRTInfo->m_rtItem.m_dClosePrice;
		sTradeHistory * pItem = new sTradeHistory ();
		GetLocalTime (&pItem->sTime);
		pItem->dPrice = m_pRTInfo->m_rtItem.m_dNowPrice;
		pItem->nNumber = m_pRTInfo->m_rtItem.m_nTradeNum - m_nFirstValume;
		if (m_nFirstValume == 0)
			m_nFirstValume = pItem->nNumber;
		m_lstHistory.AddHead (pItem);
		SendMessage (m_hParent, WM_MSG_NEW_PRICE, (WPARAM)&m_pRTInfo->m_rtItem, NULL);
		ShowIndexInfo ();
	}

	if (m_hMemDC == NULL)
		m_hMemDC = CreateCompatibleDC (hDC);
	if (m_hBmpInfo == NULL)
	{
		m_hBmpInfo = CreateBitmap (m_rcWnd.right, m_rcWnd.bottom, 1, 32, NULL);
		m_hBmpOld = (HBITMAP)SelectObject (m_hMemDC, m_hBmpInfo);
	}
	SetBkMode (m_hMemDC, TRANSPARENT);
	FillRect (m_hMemDC, &m_rcWnd, m_hBKBrush);
	DrawRect (m_hMemDC, &m_rcWnd, 5, MSC_GRAY_4);

	int		nX = m_rcDraw.left;
	int		nY = m_rcDraw.top;
	RECT	rcCode;
	RECT	rcText;
	if (m_pRTInfo != NULL)
	{
		char szEditCode[32];
		memset (szEditCode, 0, sizeof (szEditCode));
		GetWindowText (m_hEdtCode, szEditCode, sizeof (szEditCode));
		if (strcmp (m_pRTInfo->m_rtItem.m_szCode, szEditCode) && strlen (m_szKeyCode) == 0)
			SetWindowText (m_hEdtCode, m_pRTInfo->m_rtItem.m_szCode);
		GetClientRect (m_hEdtCode, &rcCode);
		SetRect (&rcText, rcCode.right + 20, m_rcDraw.top, m_rcDraw.right, m_rcDraw.top + m_nFntBigHeight);
		SetTextColor (m_hMemDC, MSC_WHITE);
		SelectObject (m_hMemDC, m_hFntBig);
		DrawTextW (m_hMemDC, m_pRTInfo->m_rtItem.m_wzName, wcslen (m_pRTInfo->m_rtItem.m_wzName), &rcText, DT_LEFT | DT_VCENTER);
	}
	nY = m_rcDraw.top + (m_nFntBigHeight + 8);
	DrawLine (m_hMemDC, m_rcWnd.left, nY, m_rcWnd.right, nY, 1, MSC_GRAY_3);
	//---------------------------------------------------------------------------------

	if (m_pRTInfo != NULL)
	{
		nY += 4;
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dDiffRate, m_hFntMid, m_rcDraw.left, nY, "涨幅: ", -2, true, 0);
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dDiffNum, m_hFntMid, m_rcDraw.right / 2 + 16, nY, "数量: ", -2, false, 0);
		nY += m_nFntMidHeight + 8;
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dTurnOver, m_hFntMid, m_rcDraw.left, nY, "换手: ", MSC_WHITE, true, 0);
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dSwing, m_hFntMid, m_rcDraw.right / 2 + 16, nY, "振幅: ", MSC_WHITE, true, 0);
		nY += m_nFntMidHeight + 8;
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dMaxPrice, m_hFntMid, m_rcDraw.left, nY, "最高: ", -1, false, 0);
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dMinPrice, m_hFntMid, m_rcDraw.right / 2 + 16, nY, "最低: ", -1, false, 0);
		nY += m_nFntMidHeight + 8;
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dOpenPrice, m_hFntMid, m_rcDraw.left, nY, "开盘: ", -1, false, 0);
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dClosePrice, m_hFntMid, m_rcDraw.right / 2 + 16, nY, "收盘: ", -1, false, 0);
		nY += m_nFntMidHeight + 8;
		DrawIntText (m_hMemDC, m_pRTInfo->m_rtItem.m_nTradeNum / 100, m_hFntMid, m_rcDraw.left, nY, "成交量 ", MSC_WHITE, 0, 0);
		DrawIntText (m_hMemDC, m_pRTInfo->m_rtItem.m_nTradeMoney / 10000, m_hFntMid, m_rcDraw.right / 2 + 16, nY, "成交额 ", MSC_WHITE, 0, 0);
	}
	nY = m_rcDraw.top + (m_nFntBigHeight + 8) + (m_nFntMidHeight + 8) * 5;
	DrawLine (m_hMemDC, m_rcWnd.left, nY, m_rcWnd.right, nY, 1, MSC_GRAY_3);
	//---------------------------------------------------------------------------------

	if (m_pRTInfo != NULL)
	{
		nY += 8;
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dSellPrice5, m_hFntSml, m_rcDraw.left, nY, "卖5: ", -1, false, 0);
		DrawIntText (m_hMemDC, m_pRTInfo->m_rtItem.m_nSellNum5, m_hFntSml, m_rcDraw.right / 2, nY, " ", -1, m_pRTInfo->m_rtItem.m_dSellPrice5, 0);
		nY += m_nFntSmlHeight + 4;
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dSellPrice4, m_hFntSml, m_rcDraw.left, nY, "卖4: ", -1, false, 0);
		DrawIntText (m_hMemDC, m_pRTInfo->m_rtItem.m_nSellNum4, m_hFntSml, m_rcDraw.right / 2, nY, " ", -1, m_pRTInfo->m_rtItem.m_dSellPrice4, 0);
		nY += m_nFntSmlHeight + 4;
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dSellPrice3, m_hFntSml, m_rcDraw.left, nY, "卖3: ", -1, false, 0);
		DrawIntText (m_hMemDC, m_pRTInfo->m_rtItem.m_nSellNum3, m_hFntSml, m_rcDraw.right / 2, nY, " ", -1, m_pRTInfo->m_rtItem.m_dSellPrice3, 0);
		nY += m_nFntSmlHeight + 4;
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dSellPrice2, m_hFntSml, m_rcDraw.left, nY, "卖2: ", -1, false, 0);
		DrawIntText (m_hMemDC, m_pRTInfo->m_rtItem.m_nSellNum2, m_hFntSml, m_rcDraw.right / 2, nY, " ", -1, m_pRTInfo->m_rtItem.m_dSellPrice2, 0);
		nY += m_nFntSmlHeight + 4;
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dSellPrice1, m_hFntSml, m_rcDraw.left, nY, "卖1: ", -1, false, 0);
		DrawIntText (m_hMemDC, m_pRTInfo->m_rtItem.m_nSellNum1, m_hFntSml, m_rcDraw.right / 2, nY, " ", -1, m_pRTInfo->m_rtItem.m_dSellPrice1, 0);
	}
	nY = m_rcDraw.top + (m_nFntBigHeight + 8) + (m_nFntMidHeight + 8) * 5 + (m_nFntSmlHeight + 4) * 5 + 8;;
	DrawLine (m_hMemDC, m_rcWnd.left, nY, m_rcWnd.right, nY, 1, MSC_GRAY_3);
	//---------------------------------------------------------------------------------

	if (m_pRTInfo != NULL)
	{
		nY += 4;
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dNowPrice, m_hFntLrg, m_rcDraw.left, nY, "现价", -1, false, 0);
	}
	nY = m_rcDraw.top + (m_nFntBigHeight + 8) + (m_nFntMidHeight + 8) * 5 + (m_nFntSmlHeight + 4) * 5 + 8 + m_nFntLrgHeight + 8;;
	DrawLine (m_hMemDC, m_rcWnd.left, nY, m_rcWnd.right, nY, 1, MSC_GRAY_3);
	//---------------------------------------------------------------------------------

	if (m_pRTInfo != NULL)
	{
		nY += 8;
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dBuyPrice1, m_hFntSml, m_rcDraw.left, nY, "买1: ", -1, false, 0);
		DrawIntText (m_hMemDC, m_pRTInfo->m_rtItem.m_nBuyNum1, m_hFntSml, m_rcDraw.right / 2, nY, " ", -1, m_pRTInfo->m_rtItem.m_dBuyPrice1, 0);
		nY += m_nFntSmlHeight + 4;
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dBuyPrice2, m_hFntSml, m_rcDraw.left, nY, "买2: ", -1, false, 0);
		DrawIntText (m_hMemDC, m_pRTInfo->m_rtItem.m_nBuyNum2, m_hFntSml, m_rcDraw.right / 2, nY, " ", -1, m_pRTInfo->m_rtItem.m_dBuyPrice2, 0);
		nY += m_nFntSmlHeight + 4;
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dBuyPrice3, m_hFntSml, m_rcDraw.left, nY, "买3: ", -1, false, 0);
		DrawIntText (m_hMemDC, m_pRTInfo->m_rtItem.m_nBuyNum3, m_hFntSml, m_rcDraw.right / 2, nY, " ", -1, m_pRTInfo->m_rtItem.m_dBuyPrice3, 0);
		nY += m_nFntSmlHeight + 4;
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dBuyPrice4, m_hFntSml, m_rcDraw.left, nY, "买4: ", -1, false, 0);
		DrawIntText (m_hMemDC, m_pRTInfo->m_rtItem.m_nBuyNum4, m_hFntSml, m_rcDraw.right / 2, nY, " ", -1, m_pRTInfo->m_rtItem.m_dBuyPrice4, 0);
		nY += m_nFntSmlHeight + 4;
		DrawDblText (m_hMemDC, m_pRTInfo->m_rtItem.m_dBuyPrice5, m_hFntSml, m_rcDraw.left, nY, "买5: ", -1, false, 0);
		DrawIntText (m_hMemDC, m_pRTInfo->m_rtItem.m_nBuyNum5, m_hFntSml, m_rcDraw.right / 2, nY, " ", -1, m_pRTInfo->m_rtItem.m_dBuyPrice5, 0);
	}
	nY = m_rcDraw.top + (m_nFntBigHeight + 8) + (m_nFntMidHeight + 8) * 5 + (m_nFntSmlHeight + 4) * 10 + 16 + m_nFntLrgHeight + 8;;
	DrawLine (m_hMemDC, m_rcWnd.left, nY, m_rcWnd.right, nY, 1, MSC_GRAY_3);
	//---------------------------------------------------------------------------------

	if (m_pRTInfo != NULL)
	{
		char			szTime[64];
		double			dLastPrice = m_dClosePrice;
		COLORREF		clrText = MSC_RED;
		sTradeHistory * pItem = NULL;
		sTradeHistory * pPrev = NULL;
		nY += 4; 
		NODEPOS pos = m_lstHistory.GetHeadPosition ();
		if (pos != NULL)
			pItem = m_lstHistory.GetNext (pos);
		while (pItem != NULL)
		{
			if (pos != NULL)
			{
				pPrev = m_lstHistory.GetNext (pos);
				dLastPrice = pPrev->dPrice;
			}
			else
			{
				pPrev = NULL;
				dLastPrice = m_dClosePrice;
			}
			sprintf (szTime, "时间: %02d:%02d:%02d  ",pItem->sTime.wHour, pItem->sTime.wMinute, pItem->sTime.wSecond);
			if (pItem->dPrice > dLastPrice)
				clrText = MSC_RED;
			else if (pItem->dPrice == dLastPrice)
				clrText = MSC_WHITE;
			else
				clrText = MSC_GREEN;
			DrawDblText (m_hMemDC, pItem->dPrice, m_hFntSml, m_rcDraw.left, nY, szTime, clrText, false, 0);
			DrawIntText (m_hMemDC, pItem->nNumber, m_hFntSml, m_rcDraw.right / 2 + 48, nY, " ", clrText, pItem->dPrice, 0);
			pItem = pPrev;

			nY += m_nFntSmlHeight + 4;
			if (nY >= m_rcDraw.bottom)
				break;
		}
	}

	BitBlt (hDC, 0, 0, m_rcWnd.right, m_rcWnd.bottom, m_hMemDC, 0, 0, SRCCOPY);
	if (m_bNeedUpdate)
		m_bNeedUpdate = false;
	return QC_ERR_NONE;
}

bool CWndRTInfo::OnKeyUp (WPARAM wParam, LPARAM lParam)
{
	long	nLen = strlen (m_szKeyCode);
	if ((wParam == VK_DELETE || wParam == VK_BACK) && nLen > 0)
	{
		m_szKeyCode[nLen-1] = 0;
		SetWindowText (m_hEdtCode, m_szKeyCode);
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
	SetWindowText (m_hEdtCode, m_szKeyCode);
	return true;
}

bool CWndRTInfo::OnEditCodeChanged (void)
{
	char szCode[32];
	GetWindowText (m_hEdtCode, szCode, sizeof (szCode));
	if (strlen (szCode) == 6)
	{
		if (strcmp (szCode, m_szCode))
			SetCode (szCode);
		strcpy (m_szKeyCode, "");
	}
	return true;
}

void CWndRTInfo::ShowIndexInfo (void)
{
	if (m_pRTInfo == NULL)
		return;

	char	szWinText[256];
	memset (szWinText, 0, sizeof (szWinText));
	strcpy (szWinText, "   指数信息：  ");
	qcStockIndexInfoItem *	pInfoItem = NULL;
	NODEPOS pos = m_pRTInfo->m_lstIndexItem.GetHeadPosition ();
	while (pos != NULL)
	{
		pInfoItem = m_pRTInfo->m_lstIndexItem.GetNext (pos);
		if (!strcmp (pInfoItem->m_szCode, "sh000001"))
			sprintf (szWinText, "%s %s %.2f   %.2f%%    ", szWinText, "上证指数", pInfoItem->m_dNow, pInfoItem->m_dDiffRate);
		else if (!strcmp (pInfoItem->m_szCode, "sz399001"))
			sprintf (szWinText, "%s %s %.2f   %.2f%%    ", szWinText, "深圳成指", pInfoItem->m_dNow, pInfoItem->m_dDiffRate);
		else if (!strcmp (pInfoItem->m_szCode, "sz399006"))
			sprintf (szWinText, "%s %s %.2f   %.2f%%    ", szWinText, "创业板指", pInfoItem->m_dNow, pInfoItem->m_dDiffRate);
	}
	SetWindowText (m_hParent, szWinText);
}


bool CWndRTInfo::ReleaseHistory (void)
{
	sTradeHistory * pItem = m_lstHistory.RemoveHead ();
	while (pItem != NULL)
	{
		delete pItem;
		pItem = m_lstHistory.RemoveHead ();
	}
	return true;
}

bool CWndRTInfo::CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG)
{
	if (!CWndBase::CreateWnd (hParent, rcView, clrBG))
		return false;

	CBaseGraphics::OnCreateWnd (m_hWnd);

	m_hEdtCode = CreateWindow ("EDIT", "", WS_VISIBLE | WS_CHILD | ES_NUMBER | ES_CENTER | ES_WANTRETURN, 
								m_rcDraw.left, m_rcDraw.top, m_rcDraw.right / 2 - 32, 42, m_hWnd, (HMENU)1001, m_hInst, NULL);
	SendMessage(m_hEdtCode,WM_SETFONT,(WPARAM)m_hFntBig,1);
	EnableWindow (m_hEdtCode, FALSE);

	m_nTimerFirst = SetTimer (m_hWnd, WM_TIMER_FIRST, 10, NULL);

	int nTimer = CRegMng::g_pRegMng->GetIntValue ("TimerRT", 10000);
	m_nTimerUpdate = SetTimer (m_hWnd, WM_TIMER_RTINFO, nTimer, NULL);

	return true;
}

LRESULT CWndRTInfo::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		if (HIWORD(wParam) == EN_CHANGE && lParam == (LPARAM)m_hEdtCode)
			OnEditCodeChanged ();
		if (HIWORD(wParam) == EN_MAXTEXT && lParam == (LPARAM)m_hEdtCode)
			SetWindowText (m_hEdtCode, "");
		break;

	case WM_TIMER:
		m_bNeedUpdate = true;
		if (wParam == WM_TIMER_FIRST)
		{
			KillTimer (m_hWnd, m_nTimerFirst);
			InvalidateRect (m_hWnd, NULL, FALSE);
			break;
		}
		if (qcIsTradeTime ())
			InvalidateRect (m_hWnd, NULL, FALSE);
		break;

	case WM_KEYUP:
		OnKeyUp (wParam, lParam);
		break;

	case WM_SIZE:
		GetClientRect (hwnd, &m_rcWnd);
		GetClientRect (hwnd, &m_rcDraw);
		m_rcDraw.left += 8;
		m_rcDraw.top += 8;
		m_rcDraw.bottom -= 8;
		m_rcDraw.right -= 8;
		if (m_hBmpInfo != NULL)
		{
			SelectObject (m_hMemDC, m_hBmpOld);
			SAFE_DEL_OBJ (m_hBmpInfo);
		}
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		UpdateView (hdc);
		EndPaint(hwnd, &ps);
	}
		break;

	case WM_ERASEBKGND:
		return S_FALSE;

	case WM_DESTROY:
		break;

	default:
		break;
	}

	return	CWndBase::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}

