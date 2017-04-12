/*******************************************************************************
	File:		CWndKXTSelect.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CWndKXTSelect_H__
#define __CWndKXTSelect_H__

#include "CWndKXTInfo.h"
#include "CStockFileCode.h"

#include "CDlgStockSelect.h"

#define		WM_TIMER_SET_CODE		501
#define		WM_TIMER_CLEAR_TEXT		502
#define		WM_TIMER_FIRST_SHOW		503

class CWndKXTSelect : public CWndKXTInfo
{
public:
	CWndKXTSelect(HINSTANCE hInst);
	virtual ~CWndKXTSelect(void);

	virtual bool	CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual int		UpdateView (HDC hDC, bool bNew);

protected:
	virtual bool	OnKeyUp (WPARAM wParam, LPARAM lParam);
	virtual bool	OnTimer (WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnResize (void);

protected:
	char				m_szKeyCode[32];
	int					m_nTimerClear;
	int					m_nTimerSetCode;
	int					m_nTimerFirstShow;

	CDlgStockSelect *	m_pDlgSel;
};
#endif //__CWndKXTSelect_H__