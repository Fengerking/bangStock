/*******************************************************************************
	File:		CWndMng.h

	Contains:	the base class of all objects.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CWndMng_H__
#define __CWndMng_H__

#include "windows.h"
#include "string.h"

#include "CRegMng.h"
#include "CWndRTInfo.h"
#include "CWndDayInfo.h"
#include "CWndKXTInfo.h"
#include "CWndSelection.h"

#include "CDlgDataAnalyse.h"
#include "CDlgKXTStudy.h"
#include "CWndKXTView.h"
#include "CWndCompInfo.h"

#include "CStockMng.h"

#define	WND_STOCK_FST				1
#define	WND_STOCK_KXT				2
#define	WND_STOCK_DAY				3
#define	WND_STOCK_FST_KXT			4

#define	WND_STOCK_DATA_ANALYSE		10
#define	WND_STOCK_KXT_STUDY			11
#define	WND_STOCK_KXT_RESEARCH		12

#define	WND_STOCK_SEL_LIKE			50
#define	WND_STOCK_SEL_BUY			51

class CWndMng
{
public:
	CWndMng(HINSTANCE hInst);
	virtual ~CWndMng(void);

	virtual int			OnCreateWnd (HWND hWnd);
	virtual LRESULT		OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual LRESULT		OnResize (void);
	virtual LRESULT		OnKeyUp (UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnRButtonUp (UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void		ShowStockWnd (void);


protected:
	HINSTANCE			m_hInst;
	HWND				m_hMainWnd;
	CRegMng *			m_pRegMng;

	//CStockMng *		m_pStockMng;
	CWndRTInfo *		m_pWndRTInfo;
	CWndDayInfo *		m_pWndDayInfo;
	CWndKXTInfo *		m_pWndKXTInfo;
	CWndSelection *		m_pWndSelect;

	CDlgDataAnalyse *	m_pDlgDataAnalyse;
	CDlgKXTStudy *		m_pDlgKXTStudy;
	CWndKXTView *		m_pWndKXTView;

	CWndDayInfo *		m_pWndDayInfo1;
	CWndDayInfo *		m_pWndDayInfo2;
	CWndDayInfo *		m_pWndDayInfo3;
	CWndDayInfo *		m_pWndDayInfo4;

	CWndCompInfo *		m_pWndCompInfo;

	int					m_nShowWnd;


	CStockMng *			m_pStockMng;

};

#endif // __CWndMng_H__