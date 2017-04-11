/*******************************************************************************
	File:		CWndStockBuy.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "CWndStockBuy.h"
#include "USystemFunc.h"

CWndStockBuy::CWndStockBuy(HINSTANCE hInst)
	: CWndBase (hInst)
	, m_bNeedUpdate (false)
	, m_pRTInfoList (NULL)
	, m_pCodeList (NULL)
	, m_nSelIndex (-1)
	, m_nStartItem (0)
{
	InitParam ();
	memset (m_szSelCode, 0, sizeof (m_szSelCode));
	m_pCodeList = new CStockFileBuy ();
	m_pCodeList->Open ("codeSelect.txt");
}

CWndStockBuy::~CWndStockBuy(void)
{
	QC_DEL_P (m_pRTInfoList);
	QC_DEL_P (m_pCodeList);
	ReleaseItemData ();
}

int CWndStockBuy::UpdateView (HDC hDC)
{
	if (m_bNeedUpdate)
	{
		if (m_pCodeList->GetURLFormat () == NULL)
			return QC_ERR_STATUS;

		if (m_pRTInfoList == NULL)
			m_pRTInfoList = new CStockRTList ();
		if (m_pRTInfoList == NULL || hDC == NULL)
			return QC_ERR_STATUS;
		m_pRTInfoList->SetNeedIndex (true);
		if (m_pRTInfoList->SetCode (m_pCodeList->GetURLFormat ()) != QC_ERR_NONE)
		{
			char szErr[256];
			sprintf (szErr, "Get %s data Failed!", m_szCode);
			SetWindowText (m_hParent, szErr);
			return QC_ERR_FAILED;
		}
		ShowIndexInfo ();
		AdjustOrder ();
	}

	if (m_hMemDC == NULL)
		m_hMemDC = CreateCompatibleDC (hDC);
	if (m_hBmpBack == NULL)
		DrawBackImage (m_hMemDC);
	if (m_hBmpInfo == NULL)
		m_hBmpInfo = CreateBitmap (m_rcWnd.right, m_rcWnd.bottom, 1, 32, NULL);
	if (m_hBmpOld != NULL)
		SelectObject (m_hMemDC, m_hBmpOld);
	SetBitmapBits (m_hBmpInfo, m_rcWnd.right * (m_rcWnd.bottom) * 4, m_pBufBack);
	m_hBmpOld = (HBITMAP)SelectObject (m_hMemDC, m_hBmpInfo);
	SetBkMode (m_hMemDC, TRANSPARENT);

	if (m_pRTInfoList != NULL)
	{
		int		nIdxXPos = 0;
		int		nXPos = m_rcDraw.left;
		int		nYPos = m_rcDraw.top + 8 + m_nItemHeight;
		int		nIdxItem = 0;

		qcStockRealTimeItem *	pItem = NULL;
		NODEPOS					pos = m_pRTInfoList->m_lstItem.GetHeadPosition ();
		int						nSkip = m_nStartItem;
		while (nSkip-- > 0)
				pItem = m_pRTInfoList->m_lstItem.GetNext (pos);
		while (pos != NULL)
		{

			pItem = m_pRTInfoList->m_lstItem.GetNext (pos);
			m_dClosePrice = pItem->m_dClosePrice;
			//MultiByteToWideChar(CP_UTF8, NULL, pItem->m_szName, -1, pItem->m_wzName, sizeof (pItem->m_wzName));  

			if (nIdxItem == m_nSelIndex - m_nStartItem - 1)
			{
				RECT rcSel;
				SetRect (&rcSel, m_rcWnd.left + 4, (m_nSelIndex - m_nStartItem) * m_nItemHeight, m_rcWnd.right - 4, (m_nSelIndex - m_nStartItem + 1) * m_nItemHeight);
				FillRect (m_hMemDC, &rcSel, m_hBrushBlue);
				strcpy (m_szSelCode, pItem->m_szCode);
			}
			nIdxXPos = 0;
			nXPos = 0;
			DrawStrText (m_hMemDC, pItem->m_szCode, m_hFntMid, nXPos + 8, nYPos, MSC_WHITE, 0);
			nXPos += m_aItemWidth[nIdxXPos];
			nIdxXPos++;
			DrawWtrText (m_hMemDC, pItem->m_wzName, m_hFntMid, nXPos, nYPos, MSC_WHITE, 0);

			nXPos += m_aItemWidth[nIdxXPos];
			nIdxXPos++;
			nXPos += m_aItemWidth[nIdxXPos];
			nIdxXPos++;
			DrawDblText (m_hMemDC, pItem->m_dNowPrice, m_hFntMid, nXPos, nYPos, "", -1, false, 1);
			nXPos += m_aItemWidth[nIdxXPos];
			nIdxXPos++;
			DrawDblText (m_hMemDC, pItem->m_dDiffRate, m_hFntMid, nXPos, nYPos, "", -2, true, 1);

			nXPos += m_aItemWidth[nIdxXPos];
			nIdxXPos++;
			DrawDblText (m_hMemDC, pItem->m_dClosePrice - m_aLastPrice[nIdxItem + m_nStartItem], m_hFntMid, nXPos, nYPos, "", -2, false, 1);

			nXPos += m_aItemWidth[nIdxXPos];
			nIdxXPos++;
			DrawIntText (m_hMemDC, (pItem->m_nTradeNum - m_aLastTrade[nIdxItem + m_nStartItem])/ 100, m_hFntMid, nXPos, nYPos, "", -2, pItem->m_dNowPrice, 1);

			nXPos += m_aItemWidth[nIdxXPos];
			nIdxXPos++;
			DrawDblText (m_hMemDC, pItem->m_dDiffNum, m_hFntMid, nXPos, nYPos, "", -2, false, 1);

			nXPos += m_aItemWidth[nIdxXPos];
			nIdxXPos++;
			DrawDblText (m_hMemDC, pItem->m_dTurnOver, m_hFntMid, nXPos, nYPos, "", MSC_WHITE, true, 1);

			nXPos += m_aItemWidth[nIdxXPos];
			nIdxXPos++;
			DrawIntText (m_hMemDC, (pItem->m_nTradeNum )/ 100, m_hFntMid, nXPos, nYPos, "", -2, pItem->m_dNowPrice, 1);

			nXPos += m_aItemWidth[nIdxXPos];
			nIdxXPos++;
			DrawDblText (m_hMemDC, pItem->m_dSwing, m_hFntMid, nXPos, nYPos, "", MSC_WHITE, true, 1);

			nXPos += m_aItemWidth[nIdxXPos];
			nIdxXPos++;
			DrawDblText (m_hMemDC, pItem->m_dOpenPrice, m_hFntMid, nXPos, nYPos, "", -1, false, 1);

			nXPos += m_aItemWidth[nIdxXPos];
			nIdxXPos++;
			DrawDblText (m_hMemDC, pItem->m_dClosePrice, m_hFntMid, nXPos, nYPos, "", -1, false, 1);

			nXPos += m_aItemWidth[nIdxXPos];
			nIdxXPos++;
			DrawDblText (m_hMemDC, pItem->m_dMaxPrice, m_hFntMid, nXPos, nYPos, "", -1, false, 1);

			nXPos += m_aItemWidth[nIdxXPos];
			nIdxXPos++;
			DrawDblText (m_hMemDC, pItem->m_dMinPrice, m_hFntMid, nXPos, nYPos, "", -1, false, 1);

			int nSellNum = pItem->m_nSellNum1 + pItem->m_nSellNum2 + pItem->m_nSellNum3 + pItem->m_nSellNum4 + pItem->m_nSellNum5;
			if (nSellNum == 0)
				nSellNum = 1;
			int nBuyNum = pItem->m_nBuyNum1 + pItem->m_nBuyNum2 + pItem->m_nBuyNum3 + pItem->m_nBuyNum4 + pItem->m_nBuyNum5;
			nXPos += m_aItemWidth[nIdxXPos];
			nIdxXPos++;
			DrawDblText (m_hMemDC, (double)nBuyNum / nSellNum, m_hFntMid, nXPos, nYPos, "", -1, true, 1);

			nYPos += m_nItemHeight;
			nIdxItem++;

			if (nYPos >= m_rcWnd.bottom)
				break;
		}
	}
	BitBlt (hDC, 0, 0, m_rcWnd.right, m_rcWnd.bottom, m_hMemDC, 0, 0, SRCCOPY);
	if (m_bNeedUpdate)
	{
		SaveLastPrice ();
		m_bNeedUpdate = false;
	}
	return QC_ERR_NONE;
}

int CWndStockBuy::DrawBackImage (HDC hDC)
{
	if (m_hBmpBack == NULL)
	{
		m_pBufBack = new unsigned char[m_rcWnd.right * m_rcWnd.bottom * 4];
		m_hBmpBack = CreateBitmap (m_rcWnd.right, (m_rcWnd.bottom), 1, 32, NULL);
	}
	m_hBmpOld = (HBITMAP)SelectObject (hDC, m_hBmpBack);
	FillRect (hDC, &m_rcWnd, m_hBKBrush);
	DrawRect (hDC, &m_rcWnd, 5, MSC_GRAY_4);
	SetBkMode (hDC, TRANSPARENT);

	int nX = 0;
	int nY = m_nItemHeight;
	while (nY < m_rcWnd.bottom)
	{
		DrawLine (hDC, nX, nY, m_rcWnd.right, nY, 1, MSC_GRAY_2);
		nY += m_nItemHeight;
	}

	nX = 0;
	nY = m_rcDraw.top + 8;
	int i = 0;
	for (i = 0; i < m_nItemColNum; i++)
	{
		if (i == 1)
		{
			DrawStrText (hDC, m_aItemName[i], m_hFntMid, nX + m_aItemWidth[i] / 2, nY, MSC_WHITE, 2);
			nX += m_aItemWidth[i];
		}
		else
		{
			nX += m_aItemWidth[i];
			DrawStrText (hDC, m_aItemName[i], m_hFntMid, nX, nY, MSC_WHITE, 1);
		}
	}

	nX = 0;
	for (i = 0; i < m_nItemColNum; i++)
	{
//		DrawLine (hDC, nX, 0, nX, m_rcWnd.bottom, 1, MSC_GRAY_2);
		nX += m_aItemWidth[i];
	}

	GetBitmapBits (m_hBmpBack, m_rcWnd.right * m_rcWnd.bottom * 4, m_pBufBack);
	SelectObject (hDC, m_hBmpOld);

	return QC_ERR_NONE;
}

void CWndStockBuy::ShowIndexInfo (void)
{
	if (m_pCodeList == NULL)
		return;

	char	szWinText[256];
	memset (szWinText, 0, sizeof (szWinText));
	strcpy (szWinText, "   指数信息：  ");
	qcStocIndexInfoItem *	pInfoItem = NULL;
	NODEPOS pos = m_pRTInfoList->m_lstIndexItem.GetHeadPosition ();
	while (pos != NULL)
	{
		pInfoItem = m_pRTInfoList->m_lstIndexItem.GetNext (pos);
		if (!strcmp (pInfoItem->m_szCode, "sh000001"))
			sprintf (szWinText, "%s %s %.2f   %.2f%%    ", szWinText, "上证指数", pInfoItem->m_dNow, pInfoItem->m_dDiffRate);
		else if (!strcmp (pInfoItem->m_szCode, "sz399001"))
			sprintf (szWinText, "%s %s %.2f   %.2f%%    ", szWinText, "深圳成指", pInfoItem->m_dNow, pInfoItem->m_dDiffRate);
		else if (!strcmp (pInfoItem->m_szCode, "sz399006"))
			sprintf (szWinText, "%s %s %.2f   %.2f%%    ", szWinText, "创业板指", pInfoItem->m_dNow, pInfoItem->m_dDiffRate);
	}
	SetWindowText (m_hParent, szWinText);
}

bool CWndStockBuy::OnLButtonUp (WPARAM wParam, LPARAM lParam)
{
	if (m_pRTInfoList == NULL)
		return false;

	int nY = HIWORD (lParam);
	int	nItemCount = m_pRTInfoList->m_lstItem.GetCount ();
	m_nSelIndex = nY / m_nItemHeight + m_nStartItem;

	InvalidateRect (m_hWnd, NULL, FALSE);

	return true;
}

bool CWndStockBuy::OnDoubleClick (WPARAM wParam, LPARAM lParam)
{
	if (strlen (m_szSelCode) < 6)
		return false;
	PostMessage (m_hParent, WM_MSG_CODE_CHANGE, (WPARAM)m_szSelCode, (LPARAM)this);
	return true;
}

bool CWndStockBuy::OnMouseWheel (WPARAM wParam, LPARAM lParam)
{
	short	zDelta = HIWORD(wParam);
	if (zDelta > 0)
		OnKeyUp (VK_UP, NULL);
	else
		OnKeyUp (VK_DOWN, NULL);
	return true;
}

bool CWndStockBuy::OnKeyUp (WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_UP || wParam == VK_DOWN)
	{
		if (m_nSelIndex == -1)
		{
			m_nSelIndex = 1;
			InvalidateRect (m_hWnd, NULL, FALSE);
			m_pCodeList->SetCurSel (m_nSelIndex - 1);
			return true;
		}
		int	nItemCount = m_pCodeList->GetCodeCount ();
		int nIndex = m_nSelIndex;
		if (wParam == VK_UP && m_nSelIndex > 1)
			nIndex--;
		if (wParam == VK_DOWN && m_nSelIndex < nItemCount)
			nIndex++;
		if (nIndex != m_nSelIndex)
		{
			m_nSelIndex = nIndex;

			int nWndItemCount = m_rcWnd.bottom / m_nItemHeight - 1;
			if (m_nSelIndex - m_nStartItem >= nWndItemCount)
				m_nStartItem = m_nSelIndex - nWndItemCount;
			else if (m_nSelIndex - m_nStartItem < 1)
				m_nStartItem--;
			m_pCodeList->SetCurSel (m_nSelIndex - 1);
			InvalidateRect (m_hWnd, NULL, FALSE);
			return true;
		}
	}
	if (wParam == VK_RETURN)
	{
		if (m_nSelIndex > 0)
		{
			PostMessage (m_hParent, WM_MSG_CODE_CHANGE, (WPARAM)m_szSelCode, (LPARAM)this);
			return true;
		}
	}
	return false;
}

bool CWndStockBuy::CloneItemInfo (void)
{
	if (m_pRTInfoList == NULL)
		return false;

	qcStockRealTimeItem *	pPrev = NULL;
	qcStockRealTimeItem *	pItem = NULL;
	NODEPOS					posPrev = m_lstItem.GetHeadPosition ();
	NODEPOS					posList = m_pRTInfoList->m_lstItem.GetHeadPosition ();
	while (posList != NULL)
	{
		pItem = m_pRTInfoList->m_lstItem.GetNext (posList);
		if (posPrev == NULL)
		{
			pPrev = new qcStockRealTimeItem ();
			m_lstItem.AddTail (pPrev);
			memcpy (pPrev, pItem, sizeof (qcStockRealTimeItem));
			memset (pPrev->m_wzName, 0, sizeof (pPrev->m_wzName));
			MultiByteToWideChar(CP_UTF8, NULL, pPrev->m_szName, -1, pPrev->m_wzName, sizeof (pPrev->m_wzName));  
		}
		else
		{
			pPrev = m_lstItem.GetNext (posPrev);
			pPrev->m_dNowPrice = pItem->m_dNowPrice;
		}
	}
	return true;
}

void CWndStockBuy::AdjustOrder (void)
{
	if (m_pRTInfoList == NULL || m_pCodeList == NULL)
		return;

	CObjectList<qcStockRealTimeItem>	lstTemp;
	qcStockRealTimeItem *				pItem = NULL;
	pItem = m_pRTInfoList->m_lstItem.RemoveHead ();
	while (pItem != NULL)
	{
		lstTemp.AddTail (pItem);
		pItem = m_pRTInfoList->m_lstItem.RemoveHead ();
	}

	char *	pCode = NULL;
	int		nCodeCount = m_pCodeList->GetCodeCount ();
	NODEPOS	pos = NULL;	
	for (int i = 0; i < nCodeCount; i++)
	{
		pCode = m_pCodeList->GetCodeNum (i);
		pos = lstTemp.GetHeadPosition ();
		while (pos != NULL)
		{
			pItem = lstTemp.GetNext (pos);
			if (!strcmp (pItem->m_szCode, pCode))
			{
				lstTemp.Remove (pItem);
				m_pRTInfoList->m_lstItem.AddTail (pItem);
				break;
			}
		}
	}
	return;
}

void CWndStockBuy::SaveLastPrice (void)
{
	int						nIndex = 0;
	qcStockRealTimeItem *	pItem = NULL;
	NODEPOS					pos = m_pRTInfoList->m_lstItem.GetHeadPosition ();
	while (pos != NULL)
	{
		pItem = m_pRTInfoList->m_lstItem.GetNext (pos);
		m_aLastPrice[nIndex] = pItem->m_dNowPrice;
		m_aLastTrade[nIndex] = pItem->m_nTradeNum;
		nIndex++;
	}
}

bool CWndStockBuy::ReleaseItemData (void)
{
	qcStockRealTimeItem * pItem = m_lstItem.RemoveHead ();
	while (pItem != NULL)
	{
		delete pItem;
		pItem = m_lstItem.RemoveHead ();
	}
	return true;
}

void CWndStockBuy::InitParam (void)
{
	memset (&m_aLastPrice, 0, sizeof (m_aLastPrice));
	memset (&m_aLastTrade, 0, sizeof (m_aLastTrade));

	m_nItemColNum = 15;
	m_nItemHeight = m_nFntBigHeight + 8;
	m_aItemWidth[0] = 120;
	m_aItemWidth[1] = 120;	// Name
	m_aItemWidth[2] = 100;	// Now
	m_aItemWidth[3] = 110;	// Diff rate
	m_aItemWidth[4] = 100;
	m_aItemWidth[5] = 110;
	m_aItemWidth[6] = 100;
	m_aItemWidth[7] = 132;
	m_aItemWidth[8] = 100;
	m_aItemWidth[9] = 100;
	m_aItemWidth[10] = 100;
	m_aItemWidth[11] = 100;
	m_aItemWidth[12] = 100;
	m_aItemWidth[13] = 100;
	m_aItemWidth[14] = 100;
	m_aItemWidth[15] = 100;

	strcpy (m_aItemName[0], "股票代码");
	strcpy (m_aItemName[1], "名称");
	strcpy (m_aItemName[2], "最新");
	strcpy (m_aItemName[3], "涨幅");
	strcpy (m_aItemName[4], "涨速");
	strcpy (m_aItemName[5], "现手");
	strcpy (m_aItemName[6], "涨跌");
	strcpy (m_aItemName[7], "换手");
	strcpy (m_aItemName[8], "总手");
	strcpy (m_aItemName[9], "振幅");
	strcpy (m_aItemName[10], "开盘");
	strcpy (m_aItemName[11], "昨收");
	strcpy (m_aItemName[12], "最高");
	strcpy (m_aItemName[13], "最低");
	strcpy (m_aItemName[14], "委比");
	strcpy (m_aItemName[15], "涨幅");

}

bool CWndStockBuy::CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG)
{
	if (!CWndBase::CreateWnd (hParent, rcView, clrBG))
		return false;

	CBaseGraphics::OnCreateWnd (m_hWnd);

	m_nTimerFirst = SetTimer (m_hWnd, WM_TIMER_FIRST, 10, NULL);
	m_nTimerUpdate = SetTimer (m_hWnd, WM_TIMER_RTINFO, 10000, NULL);

	return true;
}

LRESULT CWndStockBuy::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
			SAFE_DEL_OBJ (m_hBmpBack);
		}
		m_nStartItem = 0;
		m_nSelIndex = -1;
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		UpdateView (hdc);
		EndPaint(hwnd, &ps);
	}
		break;

	case WM_KEYUP:
		OnKeyUp (wParam, lParam);
		break;

	case WM_LBUTTONUP:
		OnLButtonUp (wParam, lParam);
		break;

	case WM_RBUTTONUP:
		SendMessage (m_hParent, uMsg, wParam, lParam);
		break;

	case WM_LBUTTONDBLCLK:
		OnDoubleClick (wParam, lParam);
		break;

	case WM_MOUSEWHEEL:
		OnMouseWheel (wParam, lParam);
		return S_OK;

	case WM_ERASEBKGND:
		return S_FALSE;

	case WM_DESTROY:
		break;

	default:
		break;
	}

	return	CWndBase::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}

