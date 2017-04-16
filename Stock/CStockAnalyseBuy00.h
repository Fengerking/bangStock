/*******************************************************************************
	File:		CStockAnalyseBuy00.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockAnalyseBuy00_H__
#define __CStockAnalyseBuy00_H__

#include "CStockAnalyseBase.h"


class CStockAnalyseBuy00 : public CStockAnalyseBase
{
public:
    CStockAnalyseBuy00(void);
    virtual ~CStockAnalyseBuy00(void);

	virtual int		Analyse (bool bNext);

protected:
	virtual bool	CheckDayLine5On10 (void);
	virtual bool	CheckLastTwoDays (void);

protected:
	int				m_nUp;
	int				m_nDown;
};

#endif //__CStockAnalyseBuy00_H__
