/*******************************************************************************
	File:		CWndCompInfo.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"
#include "qcErr.h"

#include "CFileIO.h"
#include "CStockFileHist.h"

#include "CWndCompInfo.h"
#include "USystemFunc.h"

CWndCompInfo::CWndCompInfo(HINSTANCE hInst)
	: CWndBase (hInst)
	, m_pCompInfoData (NULL)
	, m_nCompInfoSize (0)
	, m_nBmpWidth (0)
	, m_nBmpHeight (0)
	, m_nBmpYPos (0)
	, m_nBmpYMax (0)
{
	_tcscpy (m_szClassName, _T("bangCompInfoWindow"));
	_tcscpy (m_szWindowName, _T("bangCompInfoWindow"));
	m_pStockFinance = new CStockFileFinance ();
}

CWndCompInfo::~CWndCompInfo(void)
{
	QC_DEL_A (m_pCompInfoData);
	QC_DEL_P (m_pStockFinance);
}

int	CWndCompInfo::SetCode (char * pCode)
{
	QC_DEL_A (m_pCompInfoData);
	SAFE_DEL_OBJ (m_hBmpInfo);

	strcpy (m_szCode, pCode);
	m_pStockFinance->Open (m_szCode, false);
	return QC_ERR_NONE;
}

int CWndCompInfo::UpdateView (HDC hDC, bool bNew)
{
	if (m_hMemDC == NULL)
		m_hMemDC = CreateCompatibleDC (hDC);
	if (m_hBmpInfo == NULL)
		DrawCompInfo (m_hMemDC);

	BitBlt (hDC, 0, 0, m_rcWnd.right, m_rcWnd.bottom, m_hMemDC, 0, m_nBmpYPos, SRCCOPY);

	return QC_ERR_NONE;
}

int CWndCompInfo::DrawCompInfo (HDC hDC)
{
	if (m_hBmpInfo != NULL)
		return QC_ERR_NONE;

	RECT rcBmp;
	SetRect (&rcBmp, 0, 0, m_rcWnd.right, m_rcWnd.bottom * 4);
	m_nBmpWidth = m_rcWnd.right;
	m_nBmpHeight = m_rcWnd.bottom * 4;
	m_hBmpInfo = CreateBitmap (m_nBmpWidth, m_nBmpHeight, 1, 32, NULL);
	m_hBmpOld = (HBITMAP)SelectObject (hDC, m_hBmpInfo);
	SetBkMode (hDC, TRANSPARENT);

	FillRect (hDC, &rcBmp, m_hBKBrush);
	DrawRect (hDC, &rcBmp, 8, MSC_GRAY_4);

	if (m_pCompInfoData == NULL)
	{
		CFileIO filIO;
		char szFile[256];
		qcGetAppPath (NULL, szFile, sizeof (szFile));
		sprintf (szFile, "%sdata\\compinfo\\%s.txt", szFile, m_szCode);
		if (filIO.Open (szFile, 0, QCIO_FLAG_READ) != QC_ERR_NONE)
			return QC_ERR_FAILED;
		m_nCompInfoSize = (int)filIO.GetSize ();
		m_pCompInfoData = new char[m_nCompInfoSize+1];
		m_pCompInfoData[m_nCompInfoSize] = 0;
		filIO.Read ((unsigned char *)m_pCompInfoData, m_nCompInfoSize, true, 0);
	}
	SetBkMode (hDC, TRANSPARENT);

	char	szLine[8192];
	char	szItem [64];
	char *	pBuff = m_pCompInfoData;
	int		nYPos = 8;
	int		nXPos = 8;
	int nLineSize = qcReadTextLine (pBuff, m_nCompInfoSize - (pBuff - m_pCompInfoData), szLine, sizeof (szLine));
	pBuff += nLineSize;
	DrawStrText (hDC, szLine, m_hFntMid, nXPos, nYPos, MSC_GREEN, 0); 
	nYPos += m_nFntMidHeight + 8;
	nLineSize = qcReadTextLine (pBuff, m_nCompInfoSize - (pBuff - m_pCompInfoData), szLine, sizeof (szLine));
	pBuff += nLineSize;
	DrawStrText (hDC, szLine, m_hFntMid, nXPos, nYPos, MSC_GREEN, 0); 
	nYPos += m_nFntMidHeight + 8;
	nLineSize = qcReadTextLine (pBuff, m_nCompInfoSize - (pBuff - m_pCompInfoData), szLine, sizeof (szLine));
	pBuff += nLineSize;
	DrawStrText (hDC, szLine, m_hFntMid, nXPos, nYPos, MSC_GREEN, 0); 
	nYPos += m_nFntMidHeight + 8;
	nLineSize = qcReadTextLine (pBuff, m_nCompInfoSize - (pBuff - m_pCompInfoData), szLine, sizeof (szLine));
	pBuff += nLineSize;
	DrawStrText (hDC, szLine, m_hFntMid, nXPos, nYPos, MSC_GREEN, 0); 
	nYPos += 8;

	char	szDouble[32];
	strcpy (szLine, "  日  期      净资产  收益  现金流  公积金");
	nYPos += m_nFntMidHeight + 8;
	DrawStrText (hDC, szLine, m_hFntMid, nXPos, nYPos, MSC_GREEN, 0); 

	qcStockFinanceInfoItem *	pFinanceItem = NULL;
	NODEPOS						pos = m_pStockFinance->m_lstItem.GetHeadPosition ();
	while (pos != NULL)
	{
		pFinanceItem = m_pStockFinance->m_lstItem.GetNext (pos);
		sprintf (szLine, "%d-%02d-%02d  ", pFinanceItem->m_nYear, pFinanceItem->m_nMonth, pFinanceItem->m_nDay);

		FormatDouble (pFinanceItem->m_dValue, szDouble);
		strcat (szLine, szDouble);
		FormatDouble (pFinanceItem->m_dIncome, szDouble);
		strcat (szLine, szDouble);
		FormatDouble (pFinanceItem->m_dCash, szDouble);
		strcat (szLine, szDouble);
		FormatDouble (pFinanceItem->m_dFund, szDouble);
		strcat (szLine, szDouble);

		nYPos += m_nFntMidHeight + 8;
		DrawStrText (hDC, szLine, m_hFntMid, nXPos, nYPos, MSC_GREEN, 0); 
	}

	CStockFileHist			filKXT;
	qcStockKXTInfoItem *	pKXTItem = NULL;
	SYSTEMTIME	tmNow;
	long long	llStartDate = 0;
	double		dTotal5Exchange = 0;
	double		dTotal60Exchange = 0;
	int			nIndex = 0;
	GetLocalTime (&tmNow);
	tmNow.wYear -= 1;
	SystemTimeToFileTime (&tmNow, (LPFILETIME)&llStartDate);
	filKXT.SetStartEndDate (llStartDate, -1);
	filKXT.SetCode (m_szCode);
	pKXTItem = filKXT.m_lstItem.GetTail ();
	sprintf (szLine, "流通股： %d 万股,  流通市值:  %d  亿元   ", (int)(pKXTItem->m_nVolume / pKXTItem->m_dExchange), (int)(pKXTItem->m_nMoney / pKXTItem->m_dExchange / 1000000) );
	pos = filKXT.m_lstItem.GetTailPosition ();
	while (pos != NULL)
	{
		pKXTItem = filKXT.m_lstItem.GetPrev (pos);
		if (nIndex < 5)
			dTotal5Exchange += pKXTItem->m_dExchange;
		dTotal60Exchange += pKXTItem->m_dExchange;
		nIndex++;
		if (nIndex >= 60)
			break;
	}
	sprintf (szItem, "换手率 5日：%.2f%%  60日：%.2f%%", dTotal5Exchange / 5, dTotal60Exchange / 60);
	strcat (szLine, szItem);
	nYPos += m_nFntMidHeight + 8 + 24;
	DrawStrText (hDC, szLine, m_hFntMid, nXPos, nYPos, MSC_GREEN, 0); 

	nLineSize = qcReadTextLine (pBuff, m_nCompInfoSize - (pBuff - m_pCompInfoData), szLine, sizeof (szLine));
	pBuff += nLineSize;
	nYPos += m_nFntMidHeight + 8;
	nYPos += 24;

	int nLineNum = DrawLineText (hDC, nXPos, nYPos, szLine);

	nYPos += nLineNum * (m_nFntMidHeight + 8);
	nLineSize = qcReadTextLine (pBuff, m_nCompInfoSize - (pBuff - m_pCompInfoData), szLine, sizeof (szLine));
	pBuff += nLineSize;
	nLineNum = DrawLineText (hDC, nXPos, nYPos, szLine);

	m_nBmpYMax = nYPos + (nLineNum + 2) * (m_nFntMidHeight + 8);

	return QC_ERR_NONE;
}

int CWndCompInfo::DrawLineText (HDC hDC, int nXPos, int nYPos, char * pLine)
{
	SIZE	szSize;
	char *	pTxt = pLine;
	char	szTxt[256];
	int		nTxtNum = 0;
	int		i = 0;
	int		nCharNum = 0;
	int		nLineNum = 0;
	GetTextExtentPoint (hDC, "A", 1, &szSize);
	while (pTxt - pLine < strlen (pLine))
	{
		nTxtNum = (m_rcWnd.right / szSize.cx) / 2 * 2;
		if (pTxt - pLine > 8)
		{
			strcpy (szTxt, "          ");
			nTxtNum -= 10;
		}

		nCharNum = 0;
		for (i = 0; i < nTxtNum; i++)
		{
			if (pTxt[i] < 0)
				nCharNum++;
		}
		if (nCharNum % 2 != 0)
			nTxtNum += 1;

		if (pTxt - pLine > 8)
		{
			strncpy (szTxt + 10, pTxt, nTxtNum);
			szTxt[nTxtNum + 10] = 0;
		}
		else
		{
			strncpy (szTxt, pTxt, nTxtNum);
			szTxt[nTxtNum] = 0;
		}
		pTxt += nTxtNum;
		DrawStrText (hDC, szTxt, m_hFntMid, nXPos, nYPos, MSC_GREEN, 0); 
		nYPos += m_nFntMidHeight + 8;
		nLineNum++;
	}

	return nLineNum;
}

bool CWndCompInfo::CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG)
{
	if (!CWndBase::CreateWnd (hParent, rcView, clrBG))
		return false;
	CBaseGraphics::OnCreateWnd (m_hWnd);
	return true;
}

LRESULT CWndCompInfo::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
		GetClientRect (m_hWnd, &m_rcWnd);
		GetClientRect (m_hWnd, &m_rcDraw);
		m_nBmpYPos = 0;
		SAFE_DEL_OBJ (m_hBmpInfo);
		break;

	case WM_MOUSEWHEEL:
	{
		short	sDelta = HIWORD(wParam);
		if (sDelta > 0)
		{
			m_nBmpYPos -= 12;
		}
		else
		{
			m_nBmpYPos += 12;
			if (m_nBmpYPos > m_nBmpYMax - m_rcWnd.bottom)
				m_nBmpYPos = m_nBmpYMax - m_rcWnd.bottom;
		}
		if (m_nBmpYPos < 0)
			m_nBmpYPos = 0;
		InvalidateRect (m_hWnd, NULL, FALSE);
		return S_FALSE;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		UpdateView (hdc, true);
		EndPaint(hwnd, &ps);
	}
	case WM_ERASEBKGND:
		return S_FALSE;

	default:
		break;
	}

	return	CWndBase::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}

