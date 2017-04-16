/*******************************************************************************
	File:		CWndKXTInfo.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CWndKXTInfo_H__
#define __CWndKXTInfo_H__

#include "CWndBase.h"
#include "CStockKXTInfo.h"
#include "CKXTLineInfo.h"

#define	KXT_ITEM_MAX_WIDTH	96
#define	KXT_ITEM_STD_WIDTH	12

#define	KXT_SOURCE_FILE		1
#define	KXT_SOURCE_NETWORK	2

class CWndKXTInfo : public CWndBase
{
public:
	CWndKXTInfo(HINSTANCE hInst);
	virtual ~CWndKXTInfo(void);

	virtual int		SetSource (int nSource, char * pCode);
	virtual int		SetCode (char * pCode);
	virtual char *	GetCode (void) {return m_szCode;}
	virtual int		SetStartEndDate (long long llStartDate, long long llEndDate);

	virtual bool	CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual int		UpdateView (HDC hDC, bool bNew);
	virtual int		OnNewPrice (WPARAM wParam, LPARAM lParam);

	virtual int		UpdateDayLineSetting (void);
	CStockKXTInfo *	GetKXTInfo (void) {return m_pKXTInfo;}

	virtual void	SetBuyTime (SYSTEMTIME tmBuy) {m_tmBuyTime = tmBuy;}
	virtual void	SetSellTime (SYSTEMTIME tmSell) {m_tmSellTime = tmSell;}

protected:
	virtual int		DrawKXImage (HDC hDC);
	virtual int		DrawDayLine (HDC hDC);
	virtual int		DrawDayLine (HDC hDC, int nDays);
	virtual int		DrawMACDPic (HDC hDC);
	virtual int		DrawDayInfoPic (HDC hDC);

	virtual int		DrawInfoText (HDC hDC);
	virtual int		DrawBackImage (HDC hDC);

	virtual int		MoveToStartItem (void);
	virtual int		AdjustItemSize (void);

	virtual LRESULT	OnKeyUp (UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnResize (void);

protected:
	int				m_nSourceType;
	CStockKXTInfo *	m_pKXTInfo;
	double			m_dMaxPrice;
	double			m_dMinPrice;
	long long		m_llMaxVolume;
	double			m_dMaxExchange;
	double			m_dMaxMacdBar;
	double			m_dMinMacdBar;

	int				m_nItemStart;
	int				m_nItemCount;
	NODEPOS			m_hItemPos;
	int				m_nItemWidth;
	int				m_nItemGap;
	double			m_dItemWidth;
	double			m_dScale;

	RECT			m_rcKXT;
	RECT			m_rcNUM;
	RECT			m_rcJST;

	bool			m_bMouseDown;
	int				m_nXPos;
	int				m_nYPos;

	int				m_nShowDayLine;

	SYSTEMTIME		m_tmBuyTime;
	SYSTEMTIME		m_tmSellTime;
};
#endif //__CWndKXTInfo_H__