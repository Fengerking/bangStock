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

class CDlgStockSelect : public CDlgBase
{
public:
	CDlgStockSelect (HINSTANCE hInst, HWND hParent);
	virtual ~CDlgStockSelect(void);

	virtual int		CreateDlg (void);
	LRESULT			OnResize (void);

	int				GetWidth (void) {return m_nWidth;}

	virtual void	SetWndKXT (CWndKXTInfo * pWndKXT) {m_pWndKXT = pWndKXT;}

protected:
	virtual int			OpenSelectFile (void);
	virtual int			OnInitDlg (void);

	virtual INT_PTR		OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	CWndKXTInfo *		m_pWndKXT;
	int					m_nWidth;

};
#endif //__CDlgStockSelect_H__

