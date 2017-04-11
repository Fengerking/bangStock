/*******************************************************************************
	File:		CStockRTList.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockRTList_H__
#define __CStockRTList_H__

#include "qcStock.h"

#include "CStockBase.h"
#include "CNodeList.h"

class CStockRTList : public CStockBase
{
public:
    CStockRTList(void);
    virtual ~CStockRTList(void);

	virtual void	SetNeedIndex (bool bNeedIndex) {m_bNeedIndex = bNeedIndex;}
	virtual int		SetCode (char * pCode);

	virtual int		Update (void);

protected:
	virtual int		FillPath (void);
	virtual void	ReleaseData (void);

public:
	bool								m_bNeedIndex;
	CObjectList<qcStockRealTimeItem>	m_lstItem;
	char								m_szCodeList[2048];
	CObjectList<qcStockIndexInfoItem>	m_lstIndexItem;

};

#endif // __CStockRTList_H__