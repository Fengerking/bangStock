/*******************************************************************************
	File:		CWndCompInfo.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CWndCompInfo_H__
#define __CWndCompInfo_H__

#include "CWndBase.h"
#include "CStockFileFinance.h"

class CWndCompInfo : public CWndBase
{
public:
	CWndCompInfo(HINSTANCE hInst);
	virtual ~CWndCompInfo(void);

	virtual bool	CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual int		UpdateView (HDC hDC, bool bNew);
	virtual int		SetCode (char * pCode);

protected:
	virtual int		DrawCompInfo (HDC hDC);
	virtual int		DrawLineText (HDC hDC, int nXPos, int nYPos, char * pLine);

protected:
	char *				m_pCompInfoData;
	int					m_nCompInfoSize;
	CStockFileFinance *	m_pStockFinance;

	int					m_nBmpWidth;
	int					m_nBmpHeight;
	int					m_nBmpYPos;
	int					m_nBmpYMax;

};
#endif //__CWndCompInfo_H__