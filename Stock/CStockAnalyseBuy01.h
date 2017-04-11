/*******************************************************************************
	File:		CStockAnalyseBuy01.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockAnalyseBuy01_H__
#define __CStockAnalyseBuy01_H__

#include "CStockAnalyseBase.h"


class CStockAnalyseBuy01 : public CStockAnalyseBase
{
public:
    CStockAnalyseBuy01(void);
    virtual ~CStockAnalyseBuy01(void);

	virtual int		Analyse (bool bNext);

	virtual int		SetFirstParam (int nUpDays, double dMin, double dMax);
	virtual int		SetSecondParam (double dDown);
	virtual int		SetThirdParam (double dMaxDiff, int nMaxDays);

	virtual int		SetDayLine5 (bool bDayLine5);
	virtual int		SetMACDKingX (bool bMACDKingX);

protected:
	virtual bool	CheckFirstParam (void);
	virtual bool	CheckSecondParam (void);
	virtual bool	CheckThirdParam (void);


protected:
	int				m_nUpDays;
	double			m_dUpMin;
	double			m_dUpMax;

	double			m_dDownMax;

	double			m_dMaxDiff;
	int				m_nMaxDays;

	bool			m_bDayLine5;
	bool			m_bMACDKingX;
};

#endif //__CStockAnalyseBuy01_H__
