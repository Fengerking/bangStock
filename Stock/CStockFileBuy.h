/*******************************************************************************
	File:		CStockFileBuy.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockFileBuy_H__
#define __CStockFileBuy_H__

#include "CStockFileCode.h"
#include "CNodeList.h"

struct bsBuyInfo
{
	char		m_szCode[16];
	int			m_nBuyYear;
	int			m_nBuyMonth;
	int			m_nBuyDay;
	double		m_dBuyPrice;
	int			m_nBuyNum;
	int			m_nSellYear;
	int			m_nSellMonth;
	int			m_nSellDay;
	double		m_dSellPrice;
	int			m_nSellNum;
};

class CStockFileBuy : public CStockFileCode
{
public:
    CStockFileBuy(void);
    virtual ~CStockFileBuy(void);

	virtual int		Open (char * pFile);

protected:
	virtual void	ReleaseCode (void);

public:
	CObjectList<bsBuyInfo>	m_lstBuyInfo;
};

#endif //__CStockFileBuy_H__
