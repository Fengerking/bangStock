/*******************************************************************************
	File:		CWndKXTSelect.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "CWndKXTSelect.h"
#include "CStockFileHist.h"

#include "CRegMng.h"

CWndKXTSelect::CWndKXTSelect(HINSTANCE hInst)
	: CWndKXTInfo (hInst)
	, m_nTimerClear (0)
	, m_nTimerSetCode (0)
	, m_nTimerFirstShow (0)
	, m_pDlgSel (NULL)
{
	_tcscpy (m_szClassName, _T("bangKXTViewWindow"));
	_tcscpy (m_szWindowName, _T("banKXTViewWindow"));

	strcpy (m_szCode, CRegMng::g_pRegMng->GetTextValue ("LastCode"));
	memset (m_szKeyCode, 0, sizeof (m_szKeyCode));
}

CWndKXTSelect::~CWndKXTSelect(void)
{
	QC_DEL_P (m_pDlgSel);
}

int CWndKXTSelect::UpdateView (HDC hDC, bool bNew)
{
	int nRC = CWndKXTInfo::UpdateView (hDC, bNew);

	SetBkMode (hDC, TRANSPARENT);
	DrawStrText (hDC, m_szKeyCode, m_hFntXLL, m_rcKXT.right / 2, m_rcKXT.bottom, MSC_WHITE, 2);

	return nRC;
}

bool CWndKXTSelect::OnTimer (WPARAM wParam, LPARAM lParam)
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
	}
	return true;
}

bool CWndKXTSelect::OnKeyUp (WPARAM wParam, LPARAM lParam)
{
	if (m_pDlgSel != NULL && (wParam == VK_PRIOR || wParam == VK_NEXT))
	{
		char * pCode = NULL;
		if (wParam == VK_PRIOR)
			pCode = m_pDlgSel->GetStockCode (false);
		else
			pCode = m_pDlgSel->GetStockCode (true);
		if (pCode != NULL)
			SetCode (pCode);
		return true;
	}
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

LRESULT CWndKXTSelect::OnResize (void)
{
	LRESULT lRC = CWndKXTInfo::OnResize ();

	if (m_pDlgSel != NULL)
		m_pDlgSel->OnResize ();

	return lRC;
}

bool CWndKXTSelect::CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG)
{
	if (!CWndKXTInfo::CreateWnd (hParent, rcView, clrBG))
		return false;

	m_pDlgSel = new CDlgStockSelect (m_hInst, m_hWnd);
	m_pDlgSel->CreateDlg ();
	m_pDlgSel->SetWndKXT (this);

	OnResize ();

	m_nTimerFirstShow = SetTimer (m_hWnd, WM_TIMER_FIRST_SHOW, 100, NULL);

	return true;
}

LRESULT CWndKXTSelect::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

