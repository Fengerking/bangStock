/*******************************************************************************
	File:		CStockAnalyseBase.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockAnalyseBase_H__
#define __CStockAnalyseBase_H__

#include "CStockFileCode.h"
#include "CStockFileHist.h"

#include "CBaseObject.h"

class CStockAnalyseBase : public CBaseObject
{
public:
    CStockAnalyseBase(void);
    virtual ~CStockAnalyseBase(void);

	virtual int		SetStartEndDate (SYSTEMTIME * ptmStart, SYSTEMTIME * ptmEnd);
	CStockKXTInfo *	GetCurStockKXTInfo (void) {return m_pKXTInfo;}

	virtual int		Init (char * pCodeListFile);

	virtual int		GetTotalNum (void);
	virtual int		GetCurIndex (void);

	virtual int		Analyse (bool bNext);

	virtual int		GetResultCount (void) {return m_nResultCount;}
	virtual char *	GetResultCode (int nIndex) {return m_szResult[nIndex];}

	virtual int		DumpResult (void);

	
protected:
	CStockFileCode *	m_pCodeList;
	CStockKXTInfo *		m_pKXTInfo;

	SYSTEMTIME			m_tmStart;
	SYSTEMTIME			m_tmEnd;
	int					m_nEndDate2000;

	char 				m_szResult[3600][8];
	int					m_nResultCount;

protected:
	static int __cdecl compareClosePrice (const void *arg1, const void *arg2);

};

#endif //__CStockAnalyseBase_H__
