/*******************************************************************************
	File:		CWndKXTFile.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CWndKXTFile_H__
#define __CWndKXTFile_H__

#include "CWndKXTInfo.h"
#include "CStockFileCode.h"

class CWndKXTFile : public CWndKXTInfo
{
public:
	CWndKXTFile(HINSTANCE hInst);
	virtual ~CWndKXTFile(void);

	virtual int		SetCode (char * pCode);
	virtual int		SetType (int nType);

	virtual bool	CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual int		UpdateView (HDC hDC, bool bNew);

protected:
	int					m_nType;

	CStockFileCode *	m_pStockCode;
	int					m_nCodeIndex;

};
#endif //__CWndKXTFile_H__