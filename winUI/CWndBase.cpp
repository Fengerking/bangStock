/*******************************************************************************
	File:		CWndBase.cpp

	Contains:	Window base implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "CWndBase.h"

LRESULT CALLBACK CWndBase::ViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWndBase * pViewWindow = (CWndBase *)GetWindowLong (hwnd, GWL_USERDATA);
	if (pViewWindow == NULL)
		return(DefWindowProc(hwnd, uMsg, wParam, lParam));
	else
		return pViewWindow->OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}

CWndBase::CWndBase(HINSTANCE hInst)
	: m_hInst (hInst)
	, m_hWnd (NULL)
	, m_hParent (NULL)
	, m_hBKBrush (NULL)
	, m_nClrFont (RGB(255,255,255))
	, m_nTimerUpdate (0)
	, m_nTimerFirst (0)
	, m_bLButtonClick (false)
	, m_nTimerLBClick (0)
{
	_tcscpy (m_szClassName, _T("bangViewWidnow"));
	_tcscpy (m_szWindowName, _T("bangViewWidnow"));

	_tcscpy (m_szText, _T(""));
}

CWndBase::~CWndBase(void)
{
	if (m_hWnd != NULL)
		Close ();
}

bool CWndBase::CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG)
{
	m_hParent = hParent;
	if (m_hWnd != NULL)
	{
		SetParent (m_hWnd, hParent);
		return true;
	}
	HINSTANCE hInst = (HINSTANCE )GetWindowLong (hParent, GWL_HINSTANCE);
	WNDCLASS wcex;
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)ViewWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)::CreateSolidBrush (clrBG);
	wcex.lpszMenuName	= (LPCTSTR)NULL;
	wcex.lpszClassName	= m_szClassName;

	RegisterClass(&wcex);

	if (m_hParent != NULL)
		m_hWnd = CreateWindow(m_szClassName, m_szWindowName, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hParent, NULL, hInst, NULL);
	else
		m_hWnd = CreateWindow(m_szClassName, m_szWindowName, WS_VISIBLE | WS_BORDER | WS_POPUP,
								rcView.left, rcView.top, rcView.right, rcView.bottom, hParent, NULL, hInst, NULL);

	if (m_hWnd == NULL)
		return false;

	if (rcView.bottom == 0 || rcView.right == 0)
	{
		if (hParent != NULL)
			GetClientRect (hParent, &rcView);
	}

	SetWindowPos (m_hWnd, NULL, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
	LONG lRC = SetWindowLong (m_hWnd, GWL_USERDATA, (LONG)this);

	m_hBKBrush = CreateSolidBrush (clrBG);
	GetClientRect (m_hWnd, &m_rcWnd);

	return true;
}

void CWndBase::Close (void)
{
	SendMessage (m_hWnd, WM_CLOSE, 0, 0);
	m_hWnd = NULL;
}

void CWndBase::SetBGColor (COLORREF nColor)
{
	if (m_hBKBrush != NULL)
		DeleteObject (m_hBKBrush);

	m_hBKBrush = CreateSolidBrush (nColor);
	::InvalidateRect (m_hWnd, NULL, TRUE);
}

void CWndBase::SetText (TCHAR * pText)
{
	if (pText == NULL)
		return;

	_tcscpy (m_szText, pText);
	::InvalidateRect (m_hWnd, NULL, TRUE);
}

LRESULT CWndBase::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		PostMessage (m_hParent, uMsg, wParam, lParam);
		if (m_bLButtonClick)
		{
			PostMessage (m_hWnd, WM_LBUTTONDBLCLK, wParam, lParam);
			m_bLButtonClick = false;
		}
		else
		{
			m_bLButtonClick = true;
			m_nTimerLBClick = SetTimer (m_hWnd, WM_TIMER_LBUTTONCLICK, 300, NULL);
		}
		break;

	case WM_MOUSEMOVE:
		PostMessage (m_hParent, uMsg, wParam, lParam);
		break;

	case WM_TIMER:
		if (wParam == WM_TIMER_LBUTTONCLICK)
		{
			KillTimer (m_hWnd, m_nTimerLBClick);
			m_nTimerLBClick = 0;
			if (m_bLButtonClick)
				m_bLButtonClick = false;
		}
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		if (_tcslen (m_szText) > 0)
		{
			RECT rcView;
			GetClientRect (m_hWnd, &rcView);
			SetTextColor (hdc, m_nClrFont);
			::SetBkMode (hdc, TRANSPARENT);
			DrawText (hdc, m_szText, _tcslen (m_szText), &rcView, DT_CENTER | DT_VCENTER);
		}

		EndPaint(hwnd, &ps);
	}
		break;

	case WM_ERASEBKGND:
	{
		HDC hdc = (HDC)wParam;
		RECT rcView;
		GetClientRect (m_hWnd, &rcView);
		FillRect (hdc, &rcView, m_hBKBrush);
		return S_OK;
	}
		break;

	case WM_DESTROY:
		DeleteObject (m_hBKBrush);
		UnregisterClass (m_szClassName, NULL);
		break;

	default:
		break;
	}

	return	DefWindowProc(hwnd, uMsg, wParam, lParam);
}


