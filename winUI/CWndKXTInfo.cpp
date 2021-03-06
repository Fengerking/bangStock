/*******************************************************************************
	File:		CWndKXTInfo.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "CWndKXTInfo.h"
#include "CStockFileHist.h"
#include "CRegMng.h"

#include "ULogFunc.h"

#define KXT_SHOW_DL5		0X01
#define KXT_SHOW_DL10		0X02
#define KXT_SHOW_DL20		0X04
#define KXT_SHOW_DL30		0X08
#define KXT_SHOW_DL60		0X10
#define KXT_SHOW_DL120		0X20

CWndKXTInfo::CWndKXTInfo(HINSTANCE hInst)
	: CWndBase (hInst)
	, m_nSourceType (KXT_SOURCE_FILE)
	, m_pKXTInfo (NULL)
	, m_nItemStart (-1)
	, m_nItemCount (0)
	, m_hItemPos (NULL)
	, m_nItemWidth (12)
	, m_nItemGap (3)
	, m_dItemWidth (0)
	, m_dScale (1.0)
	, m_bMouseDown(false)
	, m_nXPos (-1)
	, m_nYPos (-1)
	, m_nShowDayLine (0XFFFFFFFF)
{
	_tcscpy (m_szClassName, _T("bangStockKXTWindow"));
	_tcscpy (m_szWindowName, _T("bangStockKXTWindow"));
	m_pKXTInfo = new CStockFileHist ();
	UpdateDayLineSetting ();
	memset (&m_tmBuyTime, 0, sizeof (m_tmBuyTime));
	memset (&m_tmSellTime, 0, sizeof (m_tmSellTime));
}

CWndKXTInfo::~CWndKXTInfo(void)
{
	QC_DEL_P (m_pKXTInfo);
}

int CWndKXTInfo::SetSource (int nSource, char * pCode)
{
	if (m_nSourceType == nSource)
		return QC_ERR_NONE;
	m_nSourceType = nSource;
	QC_DEL_P (m_pKXTInfo);
	if (m_nSourceType == KXT_SOURCE_FILE)
		m_pKXTInfo = new CStockFileHist ();
	else
		m_pKXTInfo = new CStockKXTInfo ();
	if (pCode != NULL)
		m_pKXTInfo->SetCode (pCode);
	else if (strlen (m_szCode) == 6)
		m_pKXTInfo->SetCode (m_szCode);
	InvalidateRect (m_hWnd, NULL, FALSE);
	return QC_ERR_NONE;
}

int CWndKXTInfo::SetCode (char * pCode)
{
	strcpy (m_szCode, pCode);
	if (m_pKXTInfo->SetCode (m_szCode) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	m_nItemStart = -1;
	InvalidateRect (m_hWnd, NULL, FALSE);
	return QC_ERR_NONE;
}

int CWndKXTInfo::SetStartEndDate (long long llStartDate, long long llEndDate)
{
	m_nItemStart = -1;
	m_pKXTInfo->SetStartEndDate (llStartDate, llEndDate);
	if (m_pKXTInfo->SetCode (m_szCode) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	InvalidateRect (m_hWnd, NULL, FALSE);
	return QC_ERR_NONE;
}

int CWndKXTInfo::UpdateView (HDC hDC, bool bNew)
{
	if (m_pKXTInfo == NULL || hDC == NULL)
		return QC_ERR_STATUS;
	if (strcmp (m_szCode, m_pKXTInfo->m_szCode))
		bNew = true;
	if (bNew)
	{
		if (m_pKXTInfo->SetCode (m_szCode) != QC_ERR_NONE)
		{
			char szErr[256];
			sprintf (szErr, "Get %s data Failed!", m_szCode);
			MessageBox (m_hParent, szErr, "Error", MB_OK);
		}
		m_nItemStart = -1;
	}

	if (m_hMemDC == NULL)
		m_hMemDC = CreateCompatibleDC (hDC);
	if (m_hBmpBack == NULL)
		DrawBackImage (m_hMemDC);
	if (m_hBmpInfo == NULL)
		m_hBmpInfo = CreateBitmap (m_rcWnd.right, m_rcWnd.bottom, 1, 32, NULL);
	if (m_hBmpOld != NULL)
		SelectObject (m_hMemDC, m_hBmpOld);
	LONG lRC = SetBitmapBits (m_hBmpInfo, m_rcWnd.right * m_rcWnd.bottom * 4, m_pBufBack);
	m_hBmpOld = (HBITMAP)SelectObject (m_hMemDC, m_hBmpInfo);
	SetBkMode (m_hMemDC, TRANSPARENT);

	DrawKXImage (m_hMemDC);

	DrawDayLine (m_hMemDC);

	DrawMACDPic (m_hMemDC);

	DrawInfoText (m_hMemDC);

	DrawDayInfoPic (m_hMemDC);

	BitBlt (hDC, 0, 0, m_rcWnd.right, m_rcWnd.bottom, m_hMemDC, 0, 0, SRCCOPY);

	return QC_ERR_NONE;
}

int CWndKXTInfo::DrawKXImage (HDC hDC)
{
	int nX, nY, nLeft, nTop, nBottom;

	AdjustItemSize ();
	MoveToStartItem ();

	char	szText[32];
	double	dDiff = m_dMaxPrice - m_dMinPrice;
	int		nHeight = m_rcKXT.bottom - m_rcKXT.top;
	int		nVolumeY = m_rcNUM.top;
	int		nVolumeH = m_rcNUM.bottom - m_rcNUM.top;
	HBRUSH	hBrushDraw = NULL;
	int		nIndex = 0;
	RECT	rcItem;
	qcStockKXTInfoItem * pItem = NULL;
	if (dDiff == 0)
		dDiff = 1;
	while (m_hItemPos != NULL)
	{
		pItem = m_pKXTInfo->m_lstItem.GetNext (m_hItemPos);
		nLeft = m_rcKXT.left + (int)((m_dItemWidth + m_nItemGap) * nIndex) + 2;
		if (nLeft > m_rcKXT.right)
			break;
		if (pItem->m_dClose > pItem->m_dOpen)
		{
			nTop = m_rcKXT.top + (int)((m_dMaxPrice - pItem->m_dClose) * nHeight / dDiff);
			nBottom = m_rcKXT.top + (int)((m_dMaxPrice - pItem->m_dOpen) * nHeight / dDiff);
			hBrushDraw = m_hBrushRed;
		}
		else if (pItem->m_dClose == pItem->m_dOpen)
		{
			nTop = m_rcKXT.top + (int)((m_dMaxPrice - pItem->m_dOpen) * nHeight / dDiff);
			nBottom = m_rcKXT.top + (int)((m_dMaxPrice - pItem->m_dClose) * nHeight / dDiff);
			hBrushDraw = m_hBrushWhite;
		}
		else
		{
			nTop = m_rcKXT.top + (int)((m_dMaxPrice - pItem->m_dOpen) * nHeight / dDiff);
			nBottom = m_rcKXT.top + (int)((m_dMaxPrice - pItem->m_dClose) * nHeight / dDiff);
			hBrushDraw = m_hBrushCyan;
		}
		SetRect (&rcItem, nLeft, nTop, nLeft + m_nItemWidth, nBottom);
		if (pItem->m_dClose > pItem->m_dOpen)
		{
			DrawRect (hDC, &rcItem, 1, MSC_RED);
			SelectObject (hDC, m_hPenRed);
		}
		else if (pItem->m_dClose == pItem->m_dOpen)
		{
			DrawRect (hDC, &rcItem, 1, MSC_WHITE);
			SelectObject (hDC, m_hPenWhite);
		}
		else
		{
			FillRect (hDC, &rcItem, hBrushDraw);
			SelectObject (hDC, m_hPenCyan);
		}
		nX = nLeft + (m_nItemWidth + m_nItemGap) / 2 - 1;
		nY = m_rcKXT.top + (int)((m_dMaxPrice - pItem->m_dMax) * nHeight / dDiff);
		MoveToEx (hDC, nX, nY, NULL);
		LineTo (hDC, nX, nTop);
		MoveToEx (hDC, nX, nBottom, NULL);
		nY = m_rcKXT.top + (int)((m_dMaxPrice - pItem->m_dMin) * nHeight / dDiff);
		LineTo (hDC, nX, nY);

		if ((m_tmBuyTime.wYear == pItem->m_nYear && m_tmBuyTime.wMonth == pItem->m_nMonth && m_tmBuyTime.wDay == pItem->m_nDay) ||
			(m_tmSellTime.wYear == pItem->m_nYear && m_tmSellTime.wMonth == pItem->m_nMonth && m_tmSellTime.wDay == pItem->m_nDay))
		{
			SetRect (&rcItem, nLeft + 3, m_rcKXT.top, nLeft + (m_nItemWidth + m_nItemGap) - 5, m_rcKXT.bottom);
			DrawRect (hDC, &rcItem, 1, MSC_WHITE);
		}

		// Draw volume rect
		if (m_llMaxVolume > 0)
			nTop = nVolumeY + (int)(((m_llMaxVolume - pItem->m_nVolume) * nVolumeH) / m_llMaxVolume);
		else
			nTop = nVolumeY;
		SetRect (&rcItem, nLeft, nTop, nLeft + m_nItemWidth, m_rcNUM.bottom);
		//FillRect (hDC, &rcItem, hBrushDraw);
		if (pItem->m_dClose > pItem->m_dOpen)
			DrawRect (hDC, &rcItem, 1, MSC_RED);
		else
			FillRect (hDC, &rcItem, hBrushDraw);	
		if (nIndex == 0)
		{
			sprintf (szText, "%d-%02d-%02d", pItem->m_nYear, pItem->m_nMonth, pItem->m_nDay);
			DrawStrText (hDC, szText, m_hFntSml, m_rcKXT.left, m_rcKXT.bottom, MSC_GRAY_3, 0);
			DrawStrText (hDC, m_szCode, m_hFntSml, m_rcKXT.left + m_nFntSmlHeight * 6, m_rcKXT.bottom, MSC_GRAY_3, 0);
		}

		nIndex++;
	}
	if (pItem != NULL)
	{
		sprintf (szText, "%d-%02d-%02d", pItem->m_nYear, pItem->m_nMonth, pItem->m_nDay);
		DrawStrText (hDC, szText, m_hFntSml, m_rcKXT.right, m_rcKXT.bottom, MSC_GRAY_3, 1);
	}
	return QC_ERR_NONE;
}

int CWndKXTInfo::DrawDayLine (HDC hDC)
{
	if ((m_nShowDayLine & KXT_SHOW_DL5) > 0)
		DrawDayLine (hDC,  5);
	if ((m_nShowDayLine & KXT_SHOW_DL10) > 0)
		DrawDayLine (hDC,  10);
	if ((m_nShowDayLine & KXT_SHOW_DL20) > 0)
		DrawDayLine (hDC,  20);
	if ((m_nShowDayLine & KXT_SHOW_DL30) > 0)
		DrawDayLine (hDC,  30);
	if ((m_nShowDayLine & KXT_SHOW_DL60) > 0)
		DrawDayLine (hDC,  60);
	if ((m_nShowDayLine & KXT_SHOW_DL120) > 0)
		DrawDayLine (hDC,  120);
	return QC_ERR_NONE;
}

int CWndKXTInfo::DrawDayLine (HDC hDC, int nDays)
{
	qcStockKXTInfoItem * pItem = NULL;
	double	dDiff = m_dMaxPrice - m_dMinPrice;
	int		nHeight = m_rcKXT.bottom - m_rcKXT.top;
	int		nLeft, nTop;
	int		nIndex = 0;
	bool	bMovePoint = false;
	double	dLineNum = 0;

	// Draw day line
	if (nDays == 5)
		SelectObject (hDC, m_hPenWhite);
	else if (nDays == 10)
		SelectObject (hDC, m_hPenYellow);
	else if (nDays == 20)
		SelectObject (hDC, m_hPenPurple);
	else if (nDays == 30)
		SelectObject (hDC, m_hPenCyan);
	else if (nDays == 60)
		SelectObject (hDC, m_hPenBlue);
	else if (nDays == 120)
		SelectObject (hDC, m_hPenOrange);

	m_hItemPos = m_pKXTInfo->m_lstItem.GetHeadPosition ();
	int nMoveItems = m_nItemStart;
	while (nMoveItems > 0)
	{
		m_pKXTInfo->m_lstItem.GetNext (m_hItemPos);
		nMoveItems--;
	}
	while (m_hItemPos != NULL)
	{
		nLeft = m_rcKXT.left + (int)((m_dItemWidth + m_nItemGap) * nIndex) + (int)(m_dItemWidth + m_nItemGap) / 2  + 2;
		if (nLeft > m_rcKXT.right)
			break;
		pItem = m_pKXTInfo->m_lstItem.GetNext (m_hItemPos);
		if (pItem->m_pDayLine == NULL)
			break;
		if (nDays == 5)
			dLineNum = pItem->m_pDayLine->m_dLine5;
		else if (nDays == 10)
			dLineNum = pItem->m_pDayLine->m_dLine10;
		else if (nDays == 20)
			dLineNum = pItem->m_pDayLine->m_dLine20;
		else if (nDays == 30)
			dLineNum = pItem->m_pDayLine->m_dLine30;
		else if (nDays == 60)
			dLineNum = pItem->m_pDayLine->m_dLine60;
		else if (nDays == 120)
			dLineNum = pItem->m_pDayLine->m_dLine120;
		if (dLineNum <= 0)
			continue;
		nTop = m_rcKXT.top + (int)((m_dMaxPrice - dLineNum) * nHeight / dDiff);
		if (!bMovePoint)
		{
			MoveToEx (hDC, nLeft, nTop, NULL);
			bMovePoint = true;
		}
		else
		{
			LineTo (hDC, nLeft, nTop);
		}
		nIndex++;
	}
	return QC_ERR_NONE;
}

int CWndKXTInfo::DrawMACDPic (HDC hDC)
{
	qcStockKXTInfoItem * pItem = NULL;

	double	dDiff = m_dMaxMacdBar - m_dMinMacdBar;
	int		nHeight = m_rcJST.bottom - m_rcJST.top;
	int		nZeroY = m_rcJST.top + (int)(m_dMaxMacdBar / dDiff * nHeight);
	double	dPrevDIFF = 0;
	double	dPrevDEA = 0;
	int		nLeft, nTop;
	int		nIndex = 0;

	m_hItemPos = m_pKXTInfo->m_lstItem.GetHeadPosition ();
	int nMoveItems = m_nItemStart;
	while (nMoveItems > 0)
	{
		m_pKXTInfo->m_lstItem.GetNext (m_hItemPos);
		nMoveItems--;
	}
	while (m_hItemPos != NULL)
	{
		nLeft = m_rcJST.left + (int)((m_dItemWidth + m_nItemGap) * nIndex) + (int)(m_dItemWidth + m_nItemGap) / 2 + 2;
		if (nLeft > m_rcJST.right)
			break;
		pItem = m_pKXTInfo->m_lstItem.GetNext (m_hItemPos);
		if (pItem->m_pMACD == NULL)
			break;
		if(pItem->m_pMACD->m_dBAR > 0)
			SelectObject (hDC, m_hPenRed);
		else
			SelectObject (hDC, m_hPenGreen);
		nTop = nZeroY - (int)(pItem->m_pMACD->m_dBAR * nHeight / dDiff);
		MoveToEx (hDC, nLeft, nZeroY, NULL);
		LineTo (hDC, nLeft, nTop);

		if (nIndex > 0)
		{
			SelectObject (hDC, m_hPenOrange);
			nLeft = m_rcJST.left + (int)((m_dItemWidth + m_nItemGap) * (nIndex - 1)) + (int)(m_dItemWidth + m_nItemGap) / 2 + 2;
			nTop = nZeroY - (int)(dPrevDEA * nHeight / dDiff);
			MoveToEx (hDC, nLeft, nTop, NULL);
			nLeft = m_rcJST.left + (int)((m_dItemWidth + m_nItemGap) * nIndex) + (int)(m_dItemWidth + m_nItemGap) / 2 + 2;
			nTop = nZeroY - (int)(pItem->m_pMACD->m_dDEA * nHeight / dDiff);
			LineTo (hDC, nLeft, nTop);

			SelectObject (hDC, m_hPenWhite);
			nLeft = m_rcJST.left + (int)((m_dItemWidth + m_nItemGap) * (nIndex - 1)) + (int)(m_dItemWidth + m_nItemGap) / 2 + 2;
			nTop = nZeroY - (int)(dPrevDIFF * nHeight / dDiff);
			MoveToEx (hDC, nLeft, nTop, NULL);
			nLeft = m_rcJST.left + (int)((m_dItemWidth + m_nItemGap) * nIndex) + (int)(m_dItemWidth + m_nItemGap) / 2 + 2;
			nTop = nZeroY - (int)(pItem->m_pMACD->m_dDIFF * nHeight / dDiff);
			LineTo (hDC, nLeft, nTop);
		}
		dPrevDEA = pItem->m_pMACD->m_dDEA;
		dPrevDIFF = pItem->m_pMACD->m_dDIFF;

		nIndex++;
	}
	return QC_ERR_NONE;
}

int CWndKXTInfo::DrawDayInfoPic (HDC hDC)
{
	if (!m_bMouseDown || m_nXPos < 0 || m_nYPos < 0)
		return QC_ERR_STATUS;
	if (m_nXPos <= m_rcKXT.left || m_nXPos >= m_rcKXT.right)
		return S_FALSE;
	if (m_nYPos <= m_rcKXT.top || m_nYPos >= m_rcKXT.bottom)
		return S_FALSE;

	int nIndex = (int)((m_nXPos - m_rcKXT.left) / (m_dItemWidth + m_nItemGap));
	int nItem = m_nItemStart + nIndex;
	if (nItem >= m_pKXTInfo->m_lstItem.GetCount ())
		return S_FALSE;
	
	RECT	rcItem;
	int		nW = 190;
	int		nH = 640;
	int		nX = m_nXPos + 8;
	int		nY = m_nYPos + 8;
	if (nX + nW > m_rcKXT.right)
		nX = m_nXPos - nW - 8;
	SetRect (&rcItem, nX, nY, nX + nW, nY + nH);
	FillRect (hDC, &rcItem, m_hBrushGray);

	qcStockKXTInfoItem *	pItem = NULL;
	NODEPOS pos = m_pKXTInfo->m_lstItem.GetHeadPosition ();
	while (nItem >= 0)
	{
		pItem = m_pKXTInfo->m_lstItem.GetNext (pos);
		if (nItem == 1)
			m_dClosePrice = pItem->m_dClose;
		nItem--;
	}
	SetBkMode (hDC, TRANSPARENT);
	char szDate[32];
	sprintf (szDate, "%d-%02d-%02d", pItem->m_nYear, pItem->m_nMonth, pItem->m_nDay);
	nX = rcItem.left + 12;
	nY = rcItem.top + 16;
	DrawStrText (hDC, szDate, m_hFntMid, nX, nY, MSC_WHITE, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (hDC, pItem->m_dClose, m_hFntMid, nX, nY, "收盘 ", -1, false, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (hDC, pItem->m_dDiffRate, m_hFntMid, nX, nY, "涨幅 ", -2, true, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (hDC, pItem->m_dDiffNum, m_hFntMid, nX, nY, "涨跌 ", -2, false, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (hDC, pItem->m_dOpen, m_hFntMid, nX, nY, "开盘 ", -1, false, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (hDC, pItem->m_dMax, m_hFntMid, nX, nY, "最高 ", -1, false, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (hDC, pItem->m_dMin, m_hFntMid, nX, nY, "最低 ", -1, false, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (hDC, pItem->m_dExchange, m_hFntMid, nX, nY, "换手 ", MSC_WHITE, true, 0);
	nY += m_nFntMidHeight + 8;
	DrawIntText (hDC, pItem->m_nMoney / 10000, m_hFntMid, nX, nY, "成交 ", MSC_WHITE, 0, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (hDC, pItem->m_dSwing, m_hFntMid, nX, nY, "振幅 ", MSC_WHITE, true, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (hDC, pItem->m_pDayLine->m_dLine5, m_hFntMid, nX, nY, "5 日 ", MSC_WHITE, false, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (hDC, pItem->m_pDayLine->m_dLine10, m_hFntMid, nX, nY, "10日 ", MSC_WHITE, false, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (hDC, pItem->m_pDayLine->m_dLine20, m_hFntMid, nX, nY, "20日 ", MSC_WHITE, false, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (hDC, pItem->m_pDayLine->m_dLine30, m_hFntMid, nX, nY, "30日 ", MSC_WHITE, false, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (hDC, pItem->m_pMACD->m_dBAR, m_hFntMid, nX, nY, "BAR  ", -2, false, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (hDC, pItem->m_pMACD->m_dDIFF, m_hFntMid, nX, nY, "DIFF ", MSC_WHITE, false, 0);
	nY += m_nFntMidHeight + 8;
	DrawDblText (hDC, pItem->m_pMACD->m_dDEA, m_hFntMid, nX, nY, "DEA  ", MSC_WHITE, false, 0);

	DrawLine (hDC, m_rcKXT.left, m_nYPos, m_rcKXT.right, m_nYPos, 1, MSC_WHITE);
	DrawLine (hDC, m_nXPos, m_rcKXT.top, m_nXPos, m_rcWnd.bottom, 1, MSC_WHITE);

	return QC_ERR_NONE;
}

int CWndKXTInfo::MoveToStartItem (void)
{
	if (m_pKXTInfo == NULL)
		return QC_ERR_FAILED;

	int nItemLast = m_nItemStart + m_nItemCount;
	int	nNumView = (int)((m_rcKXT.right - m_rcKXT.left) / (m_dItemWidth + m_nItemGap));
	int nNumItem = m_pKXTInfo->m_lstItem.GetCount ();
	if (nNumItem <= nNumView)
	{
		m_nItemStart = 0;
		m_nItemCount = nNumItem;
	}
	else
	{
		if (m_nItemStart < 0)
			m_nItemStart = nNumItem - nNumView;
		else if (m_nItemStart + nNumView > nNumItem)
			m_nItemStart = nNumItem - nNumView;
		m_nItemCount = nNumView;
	}
	if (m_nItemStart + nNumView < nItemLast)
		m_nItemStart = nItemLast - nNumView;

	m_hItemPos = m_pKXTInfo->m_lstItem.GetHeadPosition ();
	int nMoveItems = m_nItemStart;
	while (nMoveItems > 0)
	{
		m_pKXTInfo->m_lstItem.GetNext (m_hItemPos);
		nMoveItems--;
	}

	qcStockKXTInfoItem *	pItem = NULL;
	m_dMaxPrice = 0;
	m_dMinPrice = 10000.0;
	m_llMaxVolume = 0;
	m_dMaxExchange = 0;
	m_dMaxMacdBar = -10000;
	m_dMinMacdBar = 10000;
	nMoveItems = 0;
	while (m_hItemPos != NULL)
	{
		pItem = m_pKXTInfo->m_lstItem.GetNext (m_hItemPos);
		if (m_dMaxPrice < pItem->m_dMax)
			m_dMaxPrice = pItem->m_dMax;
		if (m_dMinPrice > pItem->m_dMin)
			m_dMinPrice = pItem->m_dMin;
		if (m_llMaxVolume < pItem->m_nVolume)
		{
			m_llMaxVolume = pItem->m_nVolume;
			m_dMaxExchange = pItem->m_dExchange;
		}

		if (pItem->m_pMACD != NULL)
		{
			if (m_dMaxMacdBar < pItem->m_pMACD->m_dBAR)
				m_dMaxMacdBar = pItem->m_pMACD->m_dBAR;
			if (m_dMaxMacdBar < pItem->m_pMACD->m_dDEA)
				m_dMaxMacdBar = pItem->m_pMACD->m_dDEA;
			if (m_dMaxMacdBar < pItem->m_pMACD->m_dDIFF)
				m_dMaxMacdBar = pItem->m_pMACD->m_dDIFF;
			if (m_dMinMacdBar > pItem->m_pMACD->m_dBAR)
				m_dMinMacdBar = pItem->m_pMACD->m_dBAR;
			if (m_dMinMacdBar > pItem->m_pMACD->m_dDEA)
				m_dMinMacdBar = pItem->m_pMACD->m_dDEA;
			if (m_dMinMacdBar > pItem->m_pMACD->m_dDIFF)
				m_dMinMacdBar = pItem->m_pMACD->m_dDIFF;
		}

		nMoveItems++;
		if (nMoveItems > m_nItemCount)
			break;
	}

	m_hItemPos = m_pKXTInfo->m_lstItem.GetHeadPosition ();
	nMoveItems = m_nItemStart;
	while (nMoveItems > 0)
	{
		m_pKXTInfo->m_lstItem.GetNext (m_hItemPos);
		nMoveItems--;
	}

	return QC_ERR_NONE;
}

int CWndKXTInfo::AdjustItemSize (void)
{
	m_dItemWidth = KXT_ITEM_STD_WIDTH * m_dScale;
	if (m_dItemWidth > KXT_ITEM_MAX_WIDTH)
		m_dItemWidth = KXT_ITEM_MAX_WIDTH;
	m_nItemWidth = (int)m_dItemWidth; 
	if (m_nItemWidth == 0) 
		m_nItemWidth = 1;
	m_nItemGap = 3; 
	if (m_nItemWidth <= 1) 
		m_nItemGap = 0;
	else if (m_nItemWidth <= 3)
		m_nItemGap = 1;
	else if (m_nItemWidth <= 6)
		m_nItemGap = 2;

	return QC_ERR_NONE;
}

int CWndKXTInfo::DrawInfoText (HDC hDC)
{
	int nX = m_rcKXT.left;
	int nY = m_rcKXT.top;
	DrawDblText (hDC, m_dMaxPrice, m_hFntSml, nX, nY, "", MSC_WHITE, false, 0);
	nY = (m_rcKXT.bottom - m_rcKXT.top) * 1 / 4 - m_nFntSmlHeight;
	DrawDblText (hDC, m_dMaxPrice - (m_dMaxPrice - m_dMinPrice) / 4, m_hFntSml, nX, nY, "", MSC_WHITE, false, 0);
	nY = (m_rcKXT.bottom - m_rcKXT.top) * 2 / 4 - m_nFntSmlHeight;
	DrawDblText (hDC, m_dMaxPrice - (m_dMaxPrice - m_dMinPrice) / 2, m_hFntSml, nX, nY, "", MSC_WHITE, false, 0);
	nY = (m_rcKXT.bottom - m_rcKXT.top)  * 3 / 4 - m_nFntSmlHeight;
	DrawDblText (hDC, m_dMinPrice + (m_dMaxPrice - m_dMinPrice) / 4, m_hFntSml, nX, nY, "", MSC_WHITE, false, 0);
	nY = (m_rcKXT.bottom - m_rcKXT.top) * 4 / 4 - m_nFntSmlHeight;
	DrawDblText (hDC, m_dMinPrice, m_hFntSml, nX, nY, "", MSC_WHITE, false, 0);

	nX = m_rcNUM.right - 4;
	nY = m_rcNUM.top;
	DrawIntText (hDC, (int)m_llMaxVolume, m_hFntSml, nX, nY, "", MSC_WHITE, 0, 1);
	nY += m_nFntSmlHeight + 4;
	DrawDblText (hDC, m_dMaxExchange, m_hFntSml, nX, nY, "", MSC_WHITE, true, 1);

	return QC_ERR_NONE;
}


int CWndKXTInfo::DrawBackImage (HDC hDC)
{
	if (m_hBmpBack == NULL)
	{
		m_hBmpBack = CreateBitmap (m_rcWnd.right, m_rcWnd.bottom, 1, 32, NULL);
		m_pBufBack = new unsigned char[m_rcWnd.right * m_rcWnd.bottom * 4];
	}
	m_hBmpOld = (HBITMAP)SelectObject (hDC, m_hBmpBack);
	FillRect (hDC, &m_rcWnd, m_hBKBrush);
	DrawRect (hDC, &m_rcWnd, 5, MSC_GRAY_4);

	int	nX = m_rcKXT.left;
	int	nY = m_rcKXT.top;
	int	nH = m_rcKXT.bottom - m_rcKXT.top;

	DrawRect (hDC, &m_rcKXT, 1, MSC_GRAY_4);
	DrawLine (hDC, m_rcKXT.left, nY, m_rcKXT.right, nY, 1, MSC_GRAY_3);
	nY = m_rcKXT.top + nH * 1 / 4;
	DrawLine (hDC, m_rcKXT.left, nY, m_rcKXT.right, nY, 1, MSC_GRAY_3);
	nY = m_rcKXT.top + nH  * 2 / 4;
	DrawLine (hDC, m_rcKXT.left, nY, m_rcKXT.right, nY, 1, MSC_GRAY_3);
	nY = m_rcKXT.top + nH  * 3 / 4;
	DrawLine (hDC, m_rcKXT.left, nY, m_rcKXT.right, nY, 1, MSC_GRAY_3);
	nY = m_rcKXT.top + nH  * 4 / 4;
	DrawLine (hDC, m_rcKXT.left, nY, m_rcKXT.right, nY, 1, MSC_GRAY_3);


	DrawRect (hDC, &m_rcNUM, 1, MSC_GRAY_4);
	nY = m_rcNUM.top;
	DrawLine (hDC, m_rcNUM.left, nY, m_rcNUM.right, nY, 1, MSC_GRAY_3);
	nY += (m_rcNUM.bottom - m_rcNUM.top) / 2;
	DrawLine (hDC, m_rcNUM.left, nY, m_rcNUM.right, nY, 1, MSC_GRAY_3);
	nY += (m_rcNUM.bottom - m_rcNUM.top) / 2;
	DrawLine (hDC, m_rcNUM.left, nY, m_rcNUM.right, nY, 1, MSC_GRAY_3);

	DrawRect (hDC, &m_rcJST, 1, MSC_GRAY_4);
	nY = m_rcJST.top;
	DrawLine (hDC, m_rcJST.left, nY, m_rcJST.right, nY, 1, MSC_GRAY_3);
	nY += (m_rcJST.bottom - m_rcJST.top) / 2;
//	DrawLine (hDC, m_rcJST.left, nY, m_rcJST.right, nY, 1, MSC_GRAY_3);
	nY += (m_rcJST.bottom - m_rcJST.top) / 2;
	DrawLine (hDC, m_rcJST.left, nY, m_rcJST.right, nY, 1, MSC_GRAY_3);

	GetBitmapBits (m_hBmpBack, m_rcWnd.right * m_rcWnd.bottom * 4, m_pBufBack);
	SelectObject (hDC, m_hBmpOld);

	return QC_ERR_NONE;
}

int CWndKXTInfo::OnNewPrice (WPARAM wParam, LPARAM lParam)
{
	if (m_pKXTInfo == NULL || wParam == NULL)
		return QC_ERR_STATUS;
	SYSTEMTIME	tmSystem;
	GetLocalTime (&tmSystem);
	if (tmSystem.wDayOfWeek == 0 || tmSystem.wDayOfWeek == 6)
		return QC_ERR_NONE;

	qcStockRealTimeItem * pNewPrice = (qcStockRealTimeItem *)wParam;
	if (pNewPrice->m_dNowPrice == 0 || pNewPrice->m_dMaxPrice == 0)
		return QC_ERR_FAILED;

	int nOldItems = m_pKXTInfo->m_lstItem.GetCount ();
	m_pKXTInfo->UpdateTodayItem (pNewPrice);
	if (m_pKXTInfo->m_lstItem.GetCount () > nOldItems)
	{
		m_nItemStart = -1;
		AdjustItemSize ();
		MoveToStartItem ();
	}

	InvalidateRect (m_hWnd, NULL, FALSE);

	return QC_ERR_NONE;
}

LRESULT CWndKXTInfo::OnKeyUp (UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_UP || wParam == VK_DOWN)
	{
		if (wParam == VK_UP)
			m_dScale = m_dScale * 1.25;
		else
			m_dScale = m_dScale / 1.25;
		if (m_dScale > 10.0)
			m_dScale = 10.0;
		if (m_dScale < 0.01)
			m_dScale = 0.01;
		AdjustItemSize ();
		MoveToStartItem ();
		InvalidateRect (m_hWnd, NULL, FALSE);
		return S_OK;
	}
	else if (wParam == VK_LEFT || wParam == VK_RIGHT)
	{
		int nMoveDays = 0;
		if (wParam == VK_LEFT)
			nMoveDays = -(int)(5 / m_dScale);
		else
			nMoveDays = (int)(5 / m_dScale);

		int	nViewCount = (int)((m_rcKXT.right - m_rcKXT.left) / (m_dItemWidth + m_nItemGap));
		int nItemCount = m_pKXTInfo->m_lstItem.GetCount ();
		m_nItemStart = m_nItemStart + nMoveDays;
		if (m_nItemStart < 0)
			m_nItemStart = 0;
		//if (m_nItemStart > nItemCount - nViewCount)
		//	m_nItemStart = nItemCount - nViewCount;
		InvalidateRect (m_hWnd, NULL, FALSE);
	}
	return S_FALSE;
}

LRESULT CWndKXTInfo::OnResize (void)
{
	GetClientRect (m_hWnd, &m_rcWnd);
	GetClientRect (m_hWnd, &m_rcDraw);

	SetRect (&m_rcKXT, m_rcWnd.left + 8, m_rcWnd.top + 8, m_rcWnd.right - 8, m_rcWnd.bottom * 2 / 4);
	SetRect (&m_rcNUM, m_rcWnd.left + 8, m_rcWnd.bottom * 2 / 4 + 32, m_rcWnd.right - 8, m_rcWnd.bottom * 3 / 4 + 16);
	SetRect (&m_rcJST, m_rcWnd.left + 8, m_rcWnd.bottom * 3 / 4 + 16, m_rcWnd.right - 8, m_rcWnd.bottom - 8);

	if (m_hBmpInfo != NULL)
	{
		SelectObject (m_hMemDC, m_hBmpOld);
		SAFE_DEL_OBJ (m_hBmpInfo);
		SAFE_DEL_OBJ (m_hBmpBack);
	}
	m_nItemStart = -1;
	InvalidateRect (m_hWnd, NULL, FALSE);

	return S_OK;
}

int	CWndKXTInfo::UpdateDayLineSetting (void)
{
	int		nDays5, nDays10, nDays20, nDays30, nDays60, nDays120;
	char *pDays = CRegMng::g_pRegMng->GetTextValue ("Line5Days");
	if (strlen (pDays) > 0)
		nDays5 = atoi (pDays);
	else
		nDays5 = 5;
	pDays = CRegMng::g_pRegMng->GetTextValue ("Line10Days");
	if (strlen (pDays) > 0)
		nDays10 = atoi (pDays);
	else
		nDays10 = 10;
	pDays = CRegMng::g_pRegMng->GetTextValue ("Line20Days");
	if (strlen (pDays) > 0)
		nDays20 = atoi (pDays);
	else
		nDays20 = 20;
	pDays = CRegMng::g_pRegMng->GetTextValue ("Line30Days");
	if (strlen (pDays) > 0)
		nDays30 = atoi (pDays);
	else
		nDays30 = 30;
	pDays = CRegMng::g_pRegMng->GetTextValue ("Line60Days");
	if (strlen (pDays) > 0)
		nDays60 = atoi (pDays);
	else
		nDays60 = 60;
	pDays = CRegMng::g_pRegMng->GetTextValue ("Line120Days");
	if (strlen (pDays) > 0)
		nDays120 = atoi (pDays);
	else
		nDays120 = 120;
	m_pKXTInfo->SetDaysNum (nDays5,nDays10, nDays20, nDays30, nDays60, nDays120);

	m_nShowDayLine = 0;
	if (CRegMng::g_pRegMng->GetIntValue ("Line5Show", 1) > 0)
		m_nShowDayLine |= KXT_SHOW_DL5;
	if (CRegMng::g_pRegMng->GetIntValue ("Line10Show", 1) > 0)
		m_nShowDayLine |= KXT_SHOW_DL10;
	if (CRegMng::g_pRegMng->GetIntValue ("Line20Show", 1) > 0)
		m_nShowDayLine |= KXT_SHOW_DL20;
	if (CRegMng::g_pRegMng->GetIntValue ("Line30Show", 1) > 0)
		m_nShowDayLine |= KXT_SHOW_DL30;
	if (CRegMng::g_pRegMng->GetIntValue ("Line60Show", 1) > 0)
		m_nShowDayLine |= KXT_SHOW_DL60;
	if (CRegMng::g_pRegMng->GetIntValue ("Line120Show", 1) > 0)
		m_nShowDayLine |= KXT_SHOW_DL120;

	if (strlen (m_szCode) > 0)
		SetCode (m_szCode);
	return QC_ERR_NONE;
}

bool CWndKXTInfo::CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG)
{
	if (m_pKXTInfo == NULL)
	{
		if (m_nSourceType == KXT_SOURCE_FILE)
			m_pKXTInfo = new CStockFileHist ();
		else
			m_pKXTInfo = new CStockKXTInfo ();
	}

	if (!CWndBase::CreateWnd (hParent, rcView, clrBG))
		return false;

	CBaseGraphics::OnCreateWnd (m_hWnd);
	OnResize ();

	return true;
}

LRESULT CWndKXTInfo::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MSG_NEW_PRICE:
		OnNewPrice (wParam, lParam);
		break;

	case WM_KEYUP:
		return OnKeyUp (uMsg, wParam, lParam);

	case WM_SIZE:
		OnResize ();
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		UpdateView (hdc, false);
		EndPaint(hwnd, &ps);
		break;
	}

	case WM_LBUTTONDOWN:
		m_nXPos = LOWORD(lParam); 
		m_nYPos = HIWORD(lParam);
		m_bMouseDown = true;
		InvalidateRect (m_hWnd, NULL, FALSE);
		break;

	case WM_LBUTTONUP:
		if (m_bMouseDown)
		{
			m_bMouseDown = false;
			InvalidateRect (m_hWnd, NULL, FALSE);
		}
		break;

	case WM_RBUTTONUP:
		SendMessage (m_hParent, uMsg, wParam, lParam);
		break;

	case WM_MOUSEMOVE:
		if (wParam == MK_LBUTTON)
		{
//			m_nXPos = LOWORD(lParam); 
//			m_nYPos = HIWORD(lParam);
//			m_bMouseDown = true;
//			InvalidateRect (m_hWnd, NULL, FALSE);
		}
		break;

	case WM_ERASEBKGND:
		return S_FALSE;

	default:
		break;
	}

	return	CWndBase::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}

