/*******************************************************************************
	File:		CWndRTInfo.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CWndRTInfo_H__
#define __CWndRTInfo_H__

#include "CWndBase.h"
#include "CStockRTInfo.h"
#include "CNodeList.h"

#define	 MAX_DOUBLE_VALUE	0XFFFFFFFF

struct sTradeHistory
{
	SYSTEMTIME	sTime;
	double		dPrice;
	int			nNumber;
};

class CWndRTInfo : public CWndBase
{
public:
	CWndRTInfo(HINSTANCE hInst);
	virtual ~CWndRTInfo(void);

	virtual int		SetCode (char * pCode);
	virtual char *	GetCode (void) {return m_szCode;}

	virtual int		GetWndWidth (void) {return m_nWndWidth;}
	virtual int		UpdateView (HDC hDC);

	virtual bool	CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual bool	OnKeyUp (WPARAM wParam, LPARAM lParam);
	virtual bool	OnEditCodeChanged (void);

	virtual void	ShowIndexInfo (void);

	virtual bool	ReleaseHistory (void);

protected:
	int							m_nWndWidth;
	HWND						m_hEdtCode;
	bool						m_bNeedUpdate;
	CStockRTInfo *				m_pRTInfo;
	CObjectList <sTradeHistory>	m_lstHistory;
	int							m_nLastValume;

	char						m_szKeyCode[32];
};
#endif //__CWndRTInfo_H__