/*******************************************************************************
	File:		CStockHistInfo.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockHistInfo_H__
#define __CStockHistInfo_H__

#include "qcStock.h"
#include "CStockBase.h"

class CStockHistInfo : public CStockBase
{
public:
    CStockHistInfo(void);
    virtual ~CStockHistInfo(void);

	virtual int		SetDate (char * pStart, char * pEnd);
	virtual int		Update (void);

protected:
	virtual int		FillPath (void);
	virtual void	ReleaseData (void);

public:
	CObjectList<qcStockKXTInfoItem>	m_lstItem;

	char							m_szStartDate[16];
	char							m_szEndDate[16];

};

#endif //__CStockHistInfo_H__

