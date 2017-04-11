/*******************************************************************************
	File:		CWndStockBuy.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CWndStockBuy_H__
#define __CWndStockBuy_H__

#include "CWndBase.h"
#include "CStockRTList.h"
#include "CStockFileBuy.h"

class CWndStockBuy : public CWndBase
{
public:
	CWndStockBuy(HINSTANCE hInst);
	virtual ~CWndStockBuy(void);

	virtual bool	CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual int		UpdateView (HDC hDC);

	CStockFileCode *	GetCodeList (void) {return m_pCodeList;}

protected:
	virtual int		DrawBackImage (HDC hDC);
	virtual void	ShowIndexInfo (void);
	virtual void	InitParam (void);
	virtual void	AdjustOrder (void);
	virtual void	SaveLastPrice (void);

	virtual bool	CloneItemInfo (void);
	virtual bool	ReleaseItemData (void);
	virtual bool	OnLButtonUp (WPARAM wParam, LPARAM lParam);
	virtual bool	OnDoubleClick (WPARAM wParam, LPARAM lParam);
	virtual bool	OnMouseWheel (WPARAM wParam, LPARAM lParam);
	virtual bool	OnKeyUp (WPARAM wParam, LPARAM lParam);

protected:
	CStockFileCode *	m_pCodeList;

	bool				m_bNeedUpdate;
	CStockRTList *		m_pRTInfoList;
	double				m_aLastPrice[1024];
	int					m_aLastTrade[1024];
	int					m_nSelIndex;
	int					m_nStartItem;
	char				m_szSelCode[32];

	int					m_nItemColNum;
	int					m_nItemHeight;
	int					m_aItemWidth[16];
	char				m_aItemName[16][16];

	CObjectList<qcStockRealTimeItem>	m_lstItem;
};
#endif //__CWndStockBuy_H__