/*******************************************************************************
	File:		CDlgStockInfo.h

	Contains:	Open URL dialog header file

	Written by:	Fenger King

	Change History (most recent first):
	2013-10-21		Fenger			Create file

*******************************************************************************/
#ifndef __CDlgStockInfo_H__
#define __CDlgStockInfo_H__

#include "CDlgBase.h"

#include "CStockFileBuy.h"

class CDlgStockInfo : public CDlgBase
{
public:
	CDlgStockInfo (HINSTANCE hInst, HWND hParent);
	virtual ~CDlgStockInfo(void);

	virtual int			OpenDlg (char * pCode);

protected:
	virtual int			OpenStockFile (void);
	virtual int			OnInitDlg (void);
	virtual void		FormatDouble (double dNum, char * szText);

	virtual INT_PTR		OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	char				m_szCode[16];


};
#endif //__CDlgStockInfo_H__

