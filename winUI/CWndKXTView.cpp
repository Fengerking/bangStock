/*******************************************************************************
	File:		CWndKXTView.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "CWndKXTView.h"
#include "CStockFileHist.h"

#include "CRegMng.h"

CWndKXTView::CWndKXTView(HINSTANCE hInst)
	: CWndKXTInfo (hInst)
	, m_nTimerClear (0)
	, m_nTimerSetCode (0)
	, m_pDlgBuy01 (NULL)
{
	_tcscpy (m_szClassName, _T("bangKXTViewWindow"));
	_tcscpy (m_szWindowName, _T("banKXTViewWindow"));

	strcpy (m_szCode, CRegMng::g_pRegMng->GetTextValue ("LastCode"));
	memset (m_szKeyCode, 0, sizeof (m_szKeyCode));
}

CWndKXTView::~CWndKXTView(void)
{
	QC_DEL_P (m_pDlgBuy01);
}

int CWndKXTView::UpdateView (HDC hDC, bool bNew)
{
	int nRC = CWndKXTInfo::UpdateView (hDC, bNew);

	SetBkMode (hDC, TRANSPARENT);
	DrawStrText (hDC, m_szKeyCode, m_hFntXLL, m_rcKXT.right / 2, m_rcKXT.bottom, MSC_WHITE, 2);

	return nRC;
}

bool CWndKXTView::OnTimer (WPARAM wParam, LPARAM lParam)
{
	if (wParam == WM_TIMER_CLEAR_TEXT)
	{
		strcpy (m_szKeyCode, "");
		KillTimer (m_hWnd, m_nTimerClear);
		m_nTimerClear = 0;
		InvalidateRect (m_hWnd, NULL, FALSE);
	}
	else if (wParam == WM_TIMER_SET_CODE)
	{
		KillTimer (m_hWnd, m_nTimerSetCode);
		m_nTimerSetCode = 0;
		SetCode (m_szKeyCode);
		m_szKeyCode[0] = 0;
	}
	else if (wParam == WM_TIMER_FIRST_SHOW)
	{
		KillTimer (m_hWnd, m_nTimerFirstShow);
//		if (m_pDlgBuy01 != NULL)
//			SendMessage (m_pDlgBuy01->GetDlg (), WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
	return true;
}

bool CWndKXTView::OnKeyUp (WPARAM wParam, LPARAM lParam)
{
	long	nLen = strlen (m_szKeyCode);
	if ((wParam == VK_DELETE || wParam == VK_BACK) && nLen > 0)
	{
		m_szKeyCode[nLen-1] = 0;
		return true;	
	}

	char cKey = '0';
	if (wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9)
		cKey = wParam - VK_NUMPAD0 + '0';
	else if (wParam >= '0' && wParam <= '9')
		cKey = (char)wParam;
	else
		return false;

	m_szKeyCode[nLen] = cKey;
	m_szKeyCode[nLen+1] = 0;
	InvalidateRect (m_hWnd, NULL, FALSE);
	nLen = strlen (m_szKeyCode);
	if (nLen >= 6)
	{
		m_nTimerSetCode = SetTimer (m_hWnd, WM_TIMER_SET_CODE, 100, NULL);
	}
	else
	{
		if (m_nTimerClear != 0)
			KillTimer (m_hWnd, m_nTimerClear);
		m_nTimerClear = SetTimer (m_hWnd, WM_TIMER_CLEAR_TEXT, 2000, NULL);
	}

	return true;
}

LRESULT CWndKXTView::OnResize (void)
{
	LRESULT lRC = CWndKXTInfo::OnResize ();
	if (m_pDlgBuy01 != NULL)
		m_pDlgBuy01->OnResize ();
	return lRC;
}

bool CWndKXTView::CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG)
{
	if (!CWndKXTInfo::CreateWnd (hParent, rcView, clrBG))
		return false;

	m_pDlgBuy01 = new CDlgBuy01 (m_hInst, m_hWnd);
	m_pDlgBuy01->CreateDlg ();
	m_pDlgBuy01->SetWndKXT (this);

	m_nTimerFirstShow = SetTimer (m_hWnd, WM_TIMER_FIRST_SHOW, 100, NULL);

	return true;
}

LRESULT CWndKXTView::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_TIMER:
		OnTimer (wParam, lParam);
		break;

	case WM_KEYUP:
		OnKeyUp (wParam, lParam);
		break;

	default:
		break;
	}

	return	CWndKXTInfo::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}

