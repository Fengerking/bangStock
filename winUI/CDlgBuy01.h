/*******************************************************************************
	File:		CDlgBuy01.h

	Contains:	Open URL dialog header file

	Written by:	Fenger King

	Change History (most recent first):
	2013-10-21		Fenger			Create file

*******************************************************************************/
#ifndef __CDlgBuy01_H__
#define __CDlgBuy01_H__

#include "CDlgBase.h"

#include "CWndKXTInfo.h"
#include "CStockAnalyseBuy01.h"

struct sStockBuySellInfo {
	double		dBuyPrice;
	SYSTEMTIME	tmBuy;
	double		dSellPrice;
	SYSTEMTIME	tmSell;
};

class CDlgBuy01 : public CDlgBase
{
public:
	CDlgBuy01 (HINSTANCE hInst, HWND hParent);
	virtual ~CDlgBuy01(void);

	virtual int		CreateDlg (void);
	LRESULT			OnResize (void);

	virtual void	SetWndKXT (CWndKXTInfo * pWndKXT) {m_pWndKXT = pWndKXT;}

protected:		
	bool			OnSelectStock (void);

	bool			OnKeyUp (WPARAM wParam, LPARAM lParam);
	bool			OnTimer (WPARAM wParam, LPARAM lParam);

	bool			UpdateKXTView (void);
	void			OnUpdateDaysCtrl (void);

	void			UpdateEndCheckDate (void);

	void			InitParam (void);

	int				CheckWithMaxMin (void);
	int				TryToSell (void);


	virtual INT_PTR	OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	CStockAnalyseBuy01 *	m_pBuy01;
	int						m_nTimerAnalyse;

	CWndKXTInfo *			m_pWndKXT;
	long long				m_llStartDate;
	long long				m_llEndDate;
	SYSTEMTIME				m_tmStart;
	SYSTEMTIME				m_tmEnd;
	SYSTEMTIME				m_tmEndCheck;
	long long				m_llEndCheck;

	sStockBuySellInfo *		m_pBuySellInfo;
};
#endif //__CDlgBuy01_H__

