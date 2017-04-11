/*******************************************************************************
	File:		CDlgKXTSet.h

	Contains:	Open URL dialog header file

	Written by:	Fenger King

	Change History (most recent first):
	2013-10-21		Fenger			Create file

*******************************************************************************/
#ifndef __CDlgKXTSet_H__
#define __CDlgKXTSet_H__

#include "CDlgBase.h"

#include "CWndKXTInfo.h"
#include "CStockAnalyseBuy01.h"

class CDlgKXTSet : public CDlgBase
{
public:
	CDlgKXTSet (HINSTANCE hInst, HWND hParent);
	virtual ~CDlgKXTSet(void);

	virtual int		CreateDlg (void);
	LRESULT			OnResize (void);

protected:		
	bool			OnKeyUp (WPARAM wParam, LPARAM lParam);
	void			InitParam (void);

	virtual INT_PTR	OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	CStockAnalyseBuy01 *		m_pBuy01;

};
#endif //__CDlgKXTSet_H__

