/*******************************************************************************
	File:		CStockFileFHSP.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockFileFHSP_H__
#define __CStockFileFHSP_H__
#include "qcStock.h"
#include "CStockFileBase.h"
#include "CNodeList.h"
#include "CHttpUtil.h"

class CStockFileFHSP : public CStockFileBase
{
public:
    CStockFileFHSP(void);
    virtual ~CStockFileFHSP(void);

	virtual int		Open (char * pCode, bool bUpdate);

protected:
	virtual int		Parser (bool bDownLoad);
	virtual int		DownLoad (void);

	virtual void	ReleaseItem (void);

public:
	CObjectList<qcStockFHSPInfoItem>	m_lstItem;

protected:
	CHttpUtil *		m_pHttpUtil;

};

#endif //__CStockFileFHSP_H__
