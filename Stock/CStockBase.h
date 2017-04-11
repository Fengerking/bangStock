/*******************************************************************************
	File:		CStockBase.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockBase_H__
#define __CStockBase_H__

#include "CBaseObject.h"
#include "CHttpUtil.h"
#include "CDataJson.h"

#define DATA_FROM_NETWORK	 1

class CStockBase : public CBaseObject
{
public:
    CStockBase(void);
    virtual ~CStockBase(void);

	virtual int		SetCode (char * pCode);
	virtual int		Update (void);

protected:
	virtual int		FillPath (void);
	virtual char *	GetText (CJsonNode * pNode, char * pName);
	virtual double	GetDblValue (CJsonNode * pNode, char * pName);
	virtual int		GetIntValue (CJsonNode * pNode, char * pName);
	CJsonItem *		FindItem (CJsonNode * pNode, char * pName);

	virtual int		TranslateName (char * pStrName, wchar_t * pWcsName);

	virtual int		ReadDataLine (char * pData, int nSize);

protected:
	CHttpUtil *		m_pHttpUtil;
	CDataJson *		m_pDataJson;

	char			m_szHost[128];
	char			m_szPath[2048];

public:
	char			m_szCode[32];
	char			m_szName[32];
	char			m_szDate[32];
	char			m_szTime[32];

	wchar_t			m_wzName[32];
};

#endif //__CStockBase_H__
