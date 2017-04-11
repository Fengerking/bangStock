/*******************************************************************************
	File:		CStockFileHYGN.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockFileHYGN_H__
#define __CStockFileHYGN_H__
#include "qcStock.h"
#include "CStockFileBase.h"
#include "CNodeList.h"
#include "CHttpUtil.h"

class CStockFileHYGN : public CStockFileBase
{
public:
    CStockFileHYGN(void);
    virtual ~CStockFileHYGN(void);

	virtual int		Open (char * pCode, bool bUpdate);

protected:
	virtual int		Parser (bool bDownLoad);
	virtual int		DownLoad (void);

public:

protected:
	CHttpUtil *		m_pHttpUtil;

};

#endif //__CStockFileHYGN_H__
