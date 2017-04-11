/*******************************************************************************
	File:		CStockFileFinance.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockFileFinance_H__
#define __CStockFileFinance_H__
#include "qcStock.h"
#include "CStockFileBase.h"
#include "CNodeList.h"
#include "CHttpUtil.h"

class CStockFileFinance : public CStockFileBase
{
public:
    CStockFileFinance(void);
    virtual ~CStockFileFinance(void);

	virtual int		Open (char * pCode, bool bUpdate);

protected:
	virtual int		Parser (bool bDownLoad);
	virtual int		DownLoad (void);

	virtual void	ReleaseItem (void);

public:
	CObjectList<qcStockFinanceInfoItem>	m_lstItem;

protected:
	CHttpUtil *		m_pHttpUtil;

};

#endif //__CStockFileFinance_H__
