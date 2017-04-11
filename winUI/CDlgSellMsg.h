/*******************************************************************************
	File:		CDlgSellMsg.h

	Contains:	Open URL dialog header file

	Written by:	Fenger King

	Change History (most recent first):
	2013-10-21		Fenger			Create file

*******************************************************************************/
#ifndef __CDlgSellMsg_H__
#define __CDlgSellMsg_H__

#include "CDlgBase.h"

#include "CStockFileBuy.h"

class CDlgSellMsg : public CDlgBase
{
public:
	CDlgSellMsg (HINSTANCE hInst, HWND hParent);
	virtual ~CDlgSellMsg(void);

	virtual int		CreateDlg (void);
	LRESULT			OnResize (void);

	virtual void	SetBuyInfo (CObjectList<bsBuyInfo> * pBuyInfo) {m_pBuyInfo = pBuyInfo;}

	virtual void	ResetMsg (void);
	virtual int		AddStockItem (qcStockRealTimeItem * pStockItem, double dLine5);

protected:		
	virtual INT_PTR	OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	CObjectList<bsBuyInfo> *	m_pBuyInfo;

};
#endif //__CDlgSellMsg_H__

