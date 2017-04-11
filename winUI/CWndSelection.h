/*******************************************************************************
	File:		CWndSelection.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CWndSelection_H__
#define __CWndSelection_H__

#include "CWndBase.h"
#include "CStockRTList.h"
#include "CStockFileCode.h"
#include "CDlgSellMsg.h"

#define	WND_SEL_TYPE_LIKE			50
#define	WND_SEL_TYPE_BUY			51

class CWndSelection : public CWndBase
{
public:
	CWndSelection(HINSTANCE hInst);
	virtual ~CWndSelection(void);

	virtual bool	SetSelectType (int nSelType);

	virtual bool	CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual int		UpdateView (HDC hDC);

	CStockFileCode *	GetCodeList (void) {return m_pCodeList;}

protected:
	virtual int		DrawLikeStock (HDC hDC);
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

	virtual bool	CheckSellChance (void);

protected:
	CStockFileCode *	m_pCodeList;
	int					m_nSelType;

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

	CDlgSellMsg *		m_pDlgSellMsg;
};
#endif //__CWndSelection_H__