/*******************************************************************************
	File:		CWndDayInfo.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CWndDayInfo_H__
#define __CWndDayInfo_H__

#include "CWndBase.h"
#include "CStockDayInfo.h"

class CWndDayInfo : public CWndBase
{
public:
	CWndDayInfo(HINSTANCE hInst);
	virtual ~CWndDayInfo(void);

	virtual int		SetCode (char * pCode);

	virtual bool	CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual int		UpdateView (HDC hDC);

protected:
	virtual int		DrawBackImage (HDC hDC);
	LRESULT			OnResize (void);

protected:
	bool			m_bNeedUpdate;
	CStockDayInfo *	m_pDayInfo;

	RECT			m_rcMornPrice;
	RECT			m_rcAftnPrice;
	RECT			m_rcMornNum;
	RECT			m_rcAftnNum;
};
#endif //__CWndDayInfo_H__