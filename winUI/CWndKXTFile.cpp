/*******************************************************************************
	File:		CWndKXTFile.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "CWndKXTFile.h"
#include "CStockFileHist.h"

CWndKXTFile::CWndKXTFile(HINSTANCE hInst)
	: CWndKXTInfo (hInst)
	, m_nType (1)
	, m_pStockCode (NULL)
	, m_nCodeIndex (0)
{
	m_dScale = 1.0;
	SetType (1);
}

CWndKXTFile::~CWndKXTFile(void)
{
	QC_DEL_P (m_pStockCode);
}

int CWndKXTFile::SetCode (char * pCode)
{
	strcpy (m_szCode, pCode);
	if (m_pKXTInfo == NULL)
		m_pKXTInfo = new CStockFileHist ();
	if (m_pKXTInfo->SetCode (m_szCode) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	m_nItemStart = -1;
	InvalidateRect (m_hWnd, NULL, FALSE);
	return QC_ERR_NONE;
}

int CWndKXTFile::SetType (int nType)
{
	m_nType = nType;
	if (nType == 1)
	{
		if (m_pStockCode == NULL)
		{
			m_pStockCode = new CStockFileCode ();
			m_pStockCode->Open ("codeList.txt");
			m_nCodeIndex = 0;
		}
		if (m_nTimerUpdate == 0)
			m_nTimerUpdate = SetTimer (m_hWnd, WM_TIMER_RTINFO, 2000, NULL);
	}
	return QC_ERR_NONE;
}

int CWndKXTFile::UpdateView (HDC hDC, bool bNew)
{
	if (m_pKXTInfo == NULL)
		m_pKXTInfo = new CStockFileHist ();
	if (m_pKXTInfo == NULL || hDC == NULL)
		return QC_ERR_STATUS;
	if (strcmp (m_szCode, m_pKXTInfo->m_szCode))
		bNew = true;
	if (bNew)
	{
		if (m_pKXTInfo->SetCode (m_szCode) != QC_ERR_NONE)
		{
			if (m_nTimerUpdate != 0)
				KillTimer (m_hWnd, m_nTimerUpdate);
			char szErr[256];
			sprintf (szErr, "Get %s data Failed!", m_szCode);
			MessageBox (m_hParent, szErr, "Error", MB_OK);
			return QC_ERR_FAILED;
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

	DrawInfoText (m_hMemDC);

	BitBlt (hDC, 0, 0, m_rcWnd.right, m_rcWnd.bottom, m_hMemDC, 0, 0, SRCCOPY);

	return QC_ERR_NONE;
}

bool CWndKXTFile::CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG)
{
	if (!CWndKXTInfo::CreateWnd (hParent, rcView, clrBG))
		return false;

	m_nTimerFirst = SetTimer (m_hWnd, WM_TIMER_FIRST, 10, NULL);
	if (m_nType == 1)
		m_nTimerUpdate = SetTimer (m_hWnd, WM_TIMER_RTINFO, 100, NULL);

	return true;
}

LRESULT CWndKXTFile::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_TIMER:
		if (wParam == WM_TIMER_FIRST)
			KillTimer (m_hWnd, m_nTimerFirst);
		if (wParam == WM_TIMER_RTINFO)
		{
			if (m_nCodeIndex >= m_pStockCode->GetCodeCount ())
				return S_OK;
			strcpy (m_szCode, m_pStockCode->GetCodeNum (m_nCodeIndex));
			SetWindowText (m_hParent, m_szCode);
			m_nCodeIndex++;
			::InvalidateRect (m_hWnd, NULL, FALSE);
		}
		return S_OK;

	default:
		break;
	}

	return	CWndKXTInfo::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}

