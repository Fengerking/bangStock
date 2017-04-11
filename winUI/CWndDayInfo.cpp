/*******************************************************************************
	File:		CWndDayInfo.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "CRegMng.h"

#include "CWndDayInfo.h"
#include "USystemFunc.h"

CWndDayInfo::CWndDayInfo(HINSTANCE hInst)
	: CWndBase (hInst)
	, m_bNeedUpdate (false)
	, m_pDayInfo (NULL)
{
	_tcscpy (m_szClassName, _T("bangStockFSTWindow"));
	_tcscpy (m_szWindowName, _T("bangStockFSTWindow"));
}

CWndDayInfo::~CWndDayInfo(void)
{
	QC_DEL_P (m_pDayInfo);
}

int CWndDayInfo::SetCode (char * pCode)
{
	strcpy (m_szCode, pCode);
	m_bNeedUpdate = true;
	InvalidateRect (m_hWnd, NULL, FALSE);
	return QC_ERR_NONE;
}

int CWndDayInfo::UpdateView (HDC hDC)
{
	if (m_bNeedUpdate)
	{
		if (m_pDayInfo == NULL)
			m_pDayInfo = new CStockDayInfo ();
		if (m_pDayInfo == NULL || hDC == NULL)
			return QC_ERR_STATUS;
		if (m_pDayInfo->SetCode (m_szCode) != QC_ERR_NONE)
		{
			char szErr[256];
			sprintf (szErr, "The day info window Get %s data Failed!", m_szCode);
			SetWindowText (m_hParent, szErr);
			//MessageBox (m_hParent, szErr, "Error", MB_OK);
			return QC_ERR_FAILED;
		}
		m_dClosePrice = m_pDayInfo->m_dClosePrice;
	}

	if (m_hMemDC == NULL)
		m_hMemDC = CreateCompatibleDC (hDC);
	if (m_hBmpBack == NULL)
		DrawBackImage (m_hMemDC);
	if (m_hBmpInfo == NULL)
		m_hBmpInfo = CreateBitmap (m_rcWnd.right, m_rcWnd.bottom, 1, 32, NULL);
	if (m_hBmpOld != NULL)
		SelectObject (m_hMemDC, m_hBmpOld);
	SetBitmapBits (m_hBmpInfo, m_rcWnd.right * m_rcWnd.bottom * 4, m_pBufBack);
	m_hBmpOld = (HBITMAP)SelectObject (m_hMemDC, m_hBmpInfo);
	SetBkMode (m_hMemDC, TRANSPARENT);

	if (m_pDayInfo != NULL && m_pDayInfo->m_lstItem.GetCount () > 0)
	{
		qcStockDayInfoItem *	pItem = NULL;

		pItem = m_pDayInfo->m_lstItem.GetTail ();
		if (pItem != NULL)
		{
			if (pItem->m_dNowPrice > m_dClosePrice)
				DrawDblText (m_hMemDC, pItem->m_dNowPrice, m_hFntXLL, m_rcMornPrice.right, m_rcMornNum.top - 24, "", MSC_RED_1, false, 2);
			else if (pItem->m_dNowPrice == m_dClosePrice)
				DrawDblText (m_hMemDC, pItem->m_dNowPrice, m_hFntXLL, m_rcMornPrice.right, m_rcMornNum.top - 24, "", MSC_GRAY_2, false, 2);
			else
				DrawDblText (m_hMemDC, pItem->m_dNowPrice, m_hFntXLL, m_rcMornPrice.right, m_rcMornNum.top - 24, "", MSC_GREEN_2, false, 2);
		}

		int				nNoonStep = 0;
		int				nIndex = 0;
		int				nX = m_rcMornPrice.left; 
		int				nY = m_rcMornPrice.top;
		double			dX = (m_rcMornPrice.right - m_rcMornPrice.left) / 120.0;
		double			dY = m_pDayInfo->m_dMaxPrice - m_dClosePrice;
		int				nYHigh = (m_rcMornPrice.bottom - m_rcMornPrice.top) / 2;
		int				nYStart = m_rcMornPrice.top + (m_rcMornPrice.bottom - m_rcMornPrice.top) / 2;
		NODEPOS					pos = m_pDayInfo->m_lstItem.GetHeadPosition ();
		m_hPenOld = (HPEN)SelectObject (m_hMemDC, m_hPenWhite);
		while (pos != NULL)
		{
			pItem = m_pDayInfo->m_lstItem.GetNext (pos);
			nX = m_rcMornPrice.left + (int)(dX * nIndex) + nNoonStep;
			nY = nYStart - (int)(((pItem->m_dNowPrice - m_dClosePrice) / dY) * nYHigh);
			if (nIndex == 0)
				MoveToEx (m_hMemDC, nX, nY, NULL);
			LineTo (m_hMemDC, nX, nY);
			nIndex++;
			if (nIndex > 120)
				nNoonStep = 0;
		}

		m_hPenOld = (HPEN)SelectObject (m_hMemDC, m_hPenYellow);
		nX = m_rcMornPrice.left; 
		nY = m_rcMornPrice.top;
		nIndex = 0;
		pos = m_pDayInfo->m_lstItem.GetHeadPosition ();
		while (pos != NULL)
		{
			pItem = m_pDayInfo->m_lstItem.GetNext (pos);
			nX = m_rcMornPrice.left + (int)(dX * nIndex);
			nY = nYStart - (int)(((pItem->m_dAvgPrice - m_dClosePrice) / dY) * nYHigh);
			if (nIndex == 0)
				MoveToEx (m_hMemDC, nX, nY, NULL);
			LineTo (m_hMemDC, nX, nY);
			nIndex++;
		}
		SelectObject (m_hMemDC, m_hPenOld);

		double dLastPrice = m_dClosePrice;
		nYHigh = m_rcMornNum.bottom - m_rcMornNum.top;
		nYStart = m_rcMornNum.bottom;
		nIndex = 0;
		pos = m_pDayInfo->m_lstItem.GetHeadPosition ();
		while (pos != NULL)
		{
			pItem = m_pDayInfo->m_lstItem.GetNext (pos);
			if (pItem->m_dNowPrice > dLastPrice)
				SelectObject (m_hMemDC, m_hPenRed);
			else if (pItem->m_dNowPrice < dLastPrice)
				SelectObject (m_hMemDC, m_hPenGreen);
			else
				SelectObject (m_hMemDC, m_hPenWhite);
			dLastPrice = pItem->m_dNowPrice;

			nX = m_rcMornNum.left + (int)(dX * nIndex);
			nY = m_rcMornNum.bottom;
			MoveToEx (m_hMemDC, nX, nY, NULL);
			if (m_pDayInfo->m_nMaxVolume > 0)
				nY = nYStart - pItem->m_nVolume * nYHigh / m_pDayInfo->m_nMaxVolume;
			LineTo (m_hMemDC, nX, nY);
			nIndex++;
		}

		SelectObject (m_hMemDC, m_hPenOld);

		DrawDblText (m_hMemDC, m_pDayInfo->m_dMaxPrice, m_hFntSml, m_rcMornPrice.left, m_rcMornPrice.top, "", -1, false, 0);
		double dPercent = ((m_pDayInfo->m_dMaxPrice - m_dClosePrice) / m_dClosePrice) * 100;
		DrawDblText (m_hMemDC, dPercent, m_hFntSml, m_rcAftnPrice.right, m_rcMornPrice.top, "+", MSC_RED, true, 1);
		nY = m_rcMornPrice.top + (m_rcMornPrice.bottom - m_rcMornPrice.top) * 1 / 4 - m_nFntSmlHeight - 4;
		DrawDblText (m_hMemDC, m_dClosePrice + (m_pDayInfo->m_dMaxPrice - m_dClosePrice) / 2, m_hFntSml, m_rcMornPrice.left, nY, "", MSC_GRAY_4, false, 0);
		nY = m_rcMornPrice.top + (m_rcMornPrice.bottom - m_rcMornPrice.top) / 2 - m_nFntSmlHeight - 4;
		DrawDblText (m_hMemDC, m_dClosePrice, m_hFntSml, m_rcMornPrice.left, nY, "", MSC_GRAY_4, false, 0);

		nY = m_rcMornPrice.bottom - (m_rcMornPrice.bottom - m_rcMornPrice.top) * 1 / 4 - m_nFntSmlHeight - 4;
		DrawDblText (m_hMemDC, m_pDayInfo->m_dMinPrice +  + (m_pDayInfo->m_dMaxPrice - m_dClosePrice) / 2, m_hFntSml, m_rcMornPrice.left, nY, "", -1, false, 0);
		nY = m_rcMornPrice.bottom - m_nFntSmlHeight - 4;
		DrawDblText (m_hMemDC, m_pDayInfo->m_dMinPrice, m_hFntSml, m_rcMornPrice.left, nY, "", -1, false, 0);
		dPercent = ((m_dClosePrice - m_pDayInfo->m_dMinPrice) / m_dClosePrice) * 100;
		DrawDblText (m_hMemDC, dPercent, m_hFntSml, m_rcAftnPrice.right, nY, "-", MSC_GREEN, true, 1);

		nY = m_rcAftnNum.top + 4;
		DrawIntText (m_hMemDC, m_pDayInfo->m_nMaxVolume, m_hFntSml, m_rcAftnNum.right, nY, "", MSC_GRAY_4, 0, 1);
	}

	BitBlt (hDC, 0, 0, m_rcWnd.right, m_rcWnd.bottom, m_hMemDC, 0, 0, SRCCOPY);
	if (m_bNeedUpdate)
		m_bNeedUpdate = false;

	return QC_ERR_NONE;
}

int CWndDayInfo::DrawBackImage (HDC hDC)
{
	if (m_hBmpBack == NULL)
	{
		m_pBufBack = new unsigned char[m_rcWnd.right * m_rcWnd.bottom * 4];
		m_hBmpBack = CreateBitmap (m_rcWnd.right, m_rcWnd.bottom, 1, 32, NULL);
	}
	m_hBmpOld = (HBITMAP)SelectObject (hDC, m_hBmpBack);
	FillRect (hDC, &m_rcWnd, m_hBKBrush);
	DrawRect (hDC, &m_rcWnd, 5, MSC_GRAY_4);

	int i, nX, nY;
	DrawRect (hDC, &m_rcMornPrice, 1, MSC_GRAY_3);
	DrawRect (hDC, &m_rcAftnPrice, 1, MSC_GRAY_3);
	for (i = 1; i < 4; i++)
	{
		nY = m_rcMornPrice.top + (m_rcMornPrice.bottom - m_rcMornPrice.top) * i / 4;
		DrawLine (hDC, m_rcMornPrice.left, nY, m_rcMornPrice.right, nY, 1, MSC_GRAY_3);
		DrawLine (hDC, m_rcAftnPrice.left, nY, m_rcAftnPrice.right, nY, 1, MSC_GRAY_3);
	}
	nX = m_rcMornPrice.left + (m_rcMornPrice.right - m_rcMornPrice.left) / 2;
	DrawLine (hDC, nX, m_rcMornPrice.top, nX, m_rcMornPrice.bottom, 1, MSC_GRAY_3);
	nX = m_rcAftnPrice.left + (m_rcAftnPrice.right - m_rcAftnPrice.left) / 2;
	DrawLine (hDC, nX, m_rcAftnPrice.top, nX, m_rcAftnPrice.bottom, 1, MSC_GRAY_3);

	DrawRect (hDC, &m_rcMornNum, 1, MSC_GRAY_3);
	DrawRect (hDC, &m_rcAftnNum, 1, MSC_GRAY_3);
	nY = m_rcMornNum.top + (m_rcMornNum.bottom - m_rcMornNum.top) / 2;
	DrawLine (hDC, m_rcMornNum.left, nY, m_rcMornNum.right, nY, 1, MSC_GRAY_3);
	DrawLine (hDC, m_rcAftnNum.left, nY, m_rcAftnNum.right, nY, 1, MSC_GRAY_3);
	nX = m_rcMornNum.left + (m_rcMornNum.right - m_rcMornNum.left) / 2;
	DrawLine (hDC, nX, m_rcMornNum.top, nX, m_rcMornNum.bottom, 1, MSC_GRAY_3);
	nX = m_rcAftnNum.left + (m_rcAftnNum.right - m_rcAftnNum.left) / 2;
	DrawLine (hDC, nX, m_rcAftnNum.top, nX, m_rcAftnNum.bottom, 1, MSC_GRAY_3);

	SetBkMode (hDC, TRANSPARENT);
	DrawStrText (hDC, "09:30", m_hFntSml, m_rcMornPrice.left, m_rcMornPrice.bottom + 4, MSC_GRAY_4, 0);
	DrawStrText (hDC, "11:30", m_hFntSml, m_rcMornPrice.right + 2, m_rcMornPrice.bottom + 4, MSC_GRAY_4, 2);
	DrawStrText (hDC, "15:00", m_hFntSml, m_rcAftnPrice.right, m_rcAftnPrice.bottom + 4, MSC_GRAY_4, 1);

	if (m_hBmpOld != NULL)
		SelectObject (m_hMemDC, m_hBmpOld);
	GetBitmapBits (m_hBmpBack, m_rcWnd.right * m_rcWnd.bottom * 4, m_pBufBack);
	SelectObject (hDC, m_hBmpOld);

	return QC_ERR_NONE;
}

LRESULT CWndDayInfo::OnResize (void)
{
	GetClientRect (m_hWnd, &m_rcWnd);
	GetClientRect (m_hWnd, &m_rcDraw);

	SetRect (&m_rcMornPrice, m_rcWnd.left + 8, m_rcWnd.top + 8, m_rcWnd.right / 2 - 2, m_rcWnd.bottom * 2 / 3);
	SetRect (&m_rcAftnPrice, m_rcWnd.right / 2 + 2, m_rcWnd.top + 8, m_rcWnd.right - 8, m_rcWnd.bottom * 2 / 3);

	SetRect (&m_rcMornNum, m_rcWnd.left + 8, m_rcWnd.bottom * 2 / 3 + 32, m_rcWnd.right / 2 - 2, m_rcWnd.bottom - 8);
	SetRect (&m_rcAftnNum, m_rcWnd.right / 2 + 2, m_rcWnd.bottom * 2 / 3 + 32, m_rcWnd.right - 8, m_rcWnd.bottom - 8);

	if (m_hBmpInfo != NULL)
	{
		SelectObject (m_hMemDC, m_hBmpOld);
		SAFE_DEL_OBJ (m_hBmpInfo);
	}
	SAFE_DEL_OBJ (m_hBmpBack);
	QC_DEL_A (m_pBufBack);
	InvalidateRect (m_hWnd, NULL, FALSE);
	return S_OK;
}

bool CWndDayInfo::CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG)
{
	if (!CWndBase::CreateWnd (hParent, rcView, clrBG))
		return false;

	CBaseGraphics::OnCreateWnd (m_hWnd);
	OnResize ();

	m_nTimerFirst = SetTimer (m_hWnd, WM_TIMER_FIRST, 10, NULL);

	int nTimer = CRegMng::g_pRegMng->GetIntValue ("TimerFST", 10000);
	m_nTimerUpdate = SetTimer (m_hWnd, WM_TIMER_RTINFO, nTimer, NULL);

	return true;
}

LRESULT CWndDayInfo::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
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

	case WM_SIZE:
		OnResize ();
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		UpdateView (hdc);
		EndPaint(hwnd, &ps);
	}
		break;

	case WM_RBUTTONUP:
		SendMessage (m_hParent, uMsg, wParam, lParam);
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

