/*******************************************************************************
	File:		CStockDayInfo.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockDayInfo_H__
#define __CStockDayInfo_H__

#include "qcStock.h"
#include "CStockBase.h"

class CStockDayInfo : public CStockBase
{
public:
    CStockDayInfo(void);
    virtual ~CStockDayInfo(void);

	virtual int		Update (void);

protected:
	virtual int		FillPath (void);
	virtual void	ReleaseData (void);

public:
	CObjectList<qcStockDayInfoItem>	m_lstItem;

	double							m_dClosePrice;
	int								m_nTotalVolume;
	double							m_dMaxPrice;
	double							m_dMinPrice;
	int								m_nMaxVolume;
};

#endif //__CStockDayInfo_H__

