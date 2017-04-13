/*******************************************************************************
	File:		CDlgStockSelect.h

	Contains:	Open URL dialog header file

	Written by:	Fenger King

	Change History (most recent first):
	2013-10-21		Fenger			Create file

*******************************************************************************/
#ifndef __CDlgStockSelect_H__
#define __CDlgStockSelect_H__

#include "CDlgBase.h"

#include "CWndKXTInfo.h"
#include "CStockFileCode.h"
#include "CStockAnalyseBuy01.h"

class CDlgStockSelect : public CDlgBase
{
public:
	CDlgStockSelect (HINSTANCE hInst, HWND hParent);
	virtual ~CDlgStockSelect(void);

	virtual int		CreateDlg (void);
	LRESULT			OnResize (void);

	virtual void	SetWndKXT (CWndKXTInfo * pWndKXT) {m_pWndKXT = pWndKXT;}
	char *			GetStockCode (bool bNext);

protected:

	virtual int			OpenSelectFile (char * pFile);
	virtual int			OnInitDlg (void);

	bool				OnSelectStock1 (void);
	bool				OnTimer (WPARAM wParam, LPARAM lParam);
	bool				UpdateKXTView (void);
	void				InitParam (void);

	virtual INT_PTR		OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	CStockFileCode *		m_pCodeList;
	char					m_szCode[32];

	int						m_nTimerAnalyse;
	CStockAnalyseBuy01 *	m_pBuy01;


	CWndKXTInfo *			m_pWndKXT;

};
#endif //__CDlgStockSelect_H__

