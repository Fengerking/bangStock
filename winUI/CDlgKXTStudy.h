/*******************************************************************************
	File:		CDlgKXTStudy.h

	Contains:	Open URL dialog header file

	Written by:	Fenger King

	Change History (most recent first):
	2013-10-21		Fenger			Create file

*******************************************************************************/
#ifndef __CDlgKXTStudy_H__
#define __CDlgKXTStudy_H__

#include "CStockFileCode.h"
#include "CBaseGraphics.h"
#include "CWndKXTInfo.h"

class CDlgKXTStudy : public CBaseGraphics
{
public:
	CDlgKXTStudy (HINSTANCE hInst, HWND hParent);
	virtual ~CDlgKXTStudy(void);

	int			CreateDlg (void);
	int			DestroyDlg (void);

	HWND		GetDlg (void) {return m_hDlg;}

protected:		
	bool		OnDateChange (WPARAM wParam, LPARAM lParam);
	bool		OnKeyUp (WPARAM wParam, LPARAM lParam);
	LRESULT		OnResize (void);
	void		InitParam (void);


protected:
	HINSTANCE				m_hInst;
	HWND					m_hParent;
	HWND					m_hDlg;

	CWndKXTInfo *			m_pWndKXTInfo1;
	CWndKXTInfo *			m_pWndKXTInfo2;

	char					m_szKeyCode[32];

public:
	static INT_PTR CALLBACK KXTStudyDlgProc (HWND, UINT, WPARAM, LPARAM);


};
#endif //__CDlgKXTStudy_H__

