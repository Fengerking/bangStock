/*******************************************************************************
	File:		CStockFileHist.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockFileHist_H__
#define __CStockFileHist_H__

#include "CStockKXTInfo.h"

class CStockFileHist : public CStockKXTInfo
{
public:
    CStockFileHist(void);
    virtual ~CStockFileHist(void);

	virtual int		SetCode (char * pCode);
	virtual int		SetStartEndDate (long long llStartDate, long long llEndDate);

public:

};

#endif //__CStockFileHist_H__
