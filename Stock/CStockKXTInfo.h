/*******************************************************************************
	File:		CStockKXTInfo.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockKXTInfo_H__
#define __CStockKXTInfo_H__

#include "qcStock.h"
#include "CStockBase.h"

class CStockKXTInfo : public CStockBase
{
public:
    CStockKXTInfo(void);
    virtual ~CStockKXTInfo(void);

	virtual int		Update (void);
	// The date format should 20170316 for download history data
	virtual int		SetDate (char * pStart, char * pEnd);
	virtual int		SetStartEndDate (long long llStartDate, long long llEndDate);
	virtual int		UpdateTodayItem (qcStockRealTimeItem * pRTItem);
	virtual int		SetDaysNum (int nDay5, int nDay10, int nDay20, int nDay30, int nDay60, int nDay120);

protected:
	virtual int		FillPath (void);
	virtual void	UpdateWithFHSP (void);
	virtual void	UpdateMaxMinPrice (void);
	virtual int		CreateDayLineMACD (void);
	virtual int		UpdateTodayLineMACD (void);
	virtual void	ReleaseData (void);

	virtual void	DumpToFile (void);

public:
	CObjectList<qcStockKXTInfoItem>	m_lstItem;

	double							m_dMaxPrice;
	double							m_dMinPrice;
	int								m_nMaxVolume;
	double							m_dMaxMacdBar;
	double							m_dMinMacdBar;

	char							m_szStartDate[16];
	char							m_szEndDate[16];

	long long						m_llStartDate;
	long long						m_llEndDate;

	int								m_nDay5;
	int								m_nDay10;
	int								m_nDay20;
	int								m_nDay30;
	int								m_nDay60;
	int								m_nDay120;
};

#endif //__CStockKXTInfo_H__

