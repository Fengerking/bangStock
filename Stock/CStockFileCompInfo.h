/*******************************************************************************
	File:		CStockFileCompInfo.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockFileCompInfo_H__
#define __CStockFileCompInfo_H__
#include "qcStock.h"
#include "CStockFileBase.h"
#include "CNodeList.h"
#include "CHttpUtil.h"

class CStockFileCompInfo : public CStockFileBase
{
public:
    CStockFileCompInfo(void);
    virtual ~CStockFileCompInfo(void);

	virtual int		Open (char * pCode, bool bUpdate);

protected:
	virtual int		Parser (bool bDownLoad);
	virtual int		DownLoad (void);

	virtual void	InitParam (void);

protected:
	CHttpUtil *		m_pHttpUtil;

public:
	char			m_szName[64];
	char			m_szStockDate[32];
	char			m_szBuildDate[32];
	char			m_szAddress[128];
	char			m_szInfo[8192];
	char			m_szWork[8192];
	double			m_dFirstPrice;

};

#endif //__CStockFileCompInfo_H__
