/*******************************************************************************
	File:		CDlgDataAnalyse.h

	Contains:	Open URL dialog header file

	Written by:	Fenger King

	Change History (most recent first):
	2013-10-21		Fenger			Create file

*******************************************************************************/
#ifndef __CDlgDataAnalyse_H__
#define __CDlgDataAnalyse_H__

#include "CStockFileCode.h"
#include "CBaseGraphics.h"
#include "CWndKXTInfo.h"

#define		WM_TIMER_PROCESS	301

class CDlgDataAnalyse : public CBaseGraphics
{
public:
	CDlgDataAnalyse (HINSTANCE hInst, HWND hParent);
	virtual ~CDlgDataAnalyse(void);

	int			CreateDlg (void);
	int			DestroyDlg (void);

	HWND		GetDlg (void) {return m_hDlg;}

protected:
	bool		CheckStockInfo (CStockKXTInfo * pStockInfo);
	LRESULT		OnTimer (WPARAM wParam, LPARAM lParam);

	LRESULT		OnSelItemChanged (void);

	LRESULT		OnResize (void);
	void		InitParam (void);

	void		OnButtonReset (void);
	void		OnButtonCopy (void);
	void		OnButtonSave (void);

protected:
	HINSTANCE				m_hInst;
	HWND					m_hParent;
	HWND					m_hDlg;
	int						m_nTimerProcess;

	CStockFileCode *		m_pCodeList;

	CWndKXTInfo *			m_pWndKXTInfo;
	CStockKXTInfo *			m_pStockKXTInfo;

public:
	static INT_PTR CALLBACK DataAnalyseDlgProc (HWND, UINT, WPARAM, LPARAM);


};
#endif //__CDlgDataAnalyse_H__

