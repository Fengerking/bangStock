/*******************************************************************************
	File:		CStockHistInfo.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockHistInfo.h"

#include "USystemFunc.h"
#include "ULogFunc.h"

CStockHistInfo::CStockHistInfo(void)
	: CStockBase ()
{
	SetObjectName ("CStockHistInfo");
	memset (m_szStartDate, 0, sizeof (m_szStartDate));
	memset (m_szEndDate, 0, sizeof (m_szEndDate));
	strcpy (m_szStartDate, "2017-02-21");
	SYSTEMTIME tmSys;
	GetLocalTime (&tmSys);
	sprintf (m_szEndDate, "%d-%02d-%02d", tmSys.wYear, tmSys.wMonth, tmSys.wDay);
}

CStockHistInfo::~CStockHistInfo(void)
{
	ReleaseData ();
}

int CStockHistInfo::SetDate (char * pStart, char * pEnd)
{
	if (pStart != NULL)
		strcpy (m_szStartDate, pStart);
	if (pEnd != NULL)
		strcpy (m_szEndDate, pEnd);
	return QC_ERR_NONE;
}

int CStockHistInfo::Update (void)
{
	int nRC = QC_ERR_NONE;
	nRC = CStockBase::Update ();
	if (nRC != NULL)
	{
		QCLOGW ("CStockBase::Update error!");
		return nRC;
	}

	ReleaseData ();

	CJsonNode * pInfoNode = m_pDataJson->FindNode ("list");
	if (pInfoNode == NULL)
	{
		QCLOGW ("FindNode list error!");
		return QC_ERR_STATUS;
	}

	char *					pDate = NULL;
	qcStockKXTInfoItem *	pItem = NULL;
	CJsonNode *				pItemNode = NULL;
	NODEPOS					pos = pInfoNode->m_lstNode.GetTailPosition ();
	while (pos != NULL)
	{
		pItemNode = pInfoNode->m_lstNode.GetPrev (pos);
		pDate = GetText (pItemNode, "date");
		if (pDate == NULL)
			break;

		pItem = new qcStockKXTInfoItem ();
		pItem->m_pDayLine = NULL;
		pItem->m_pMACD = NULL;
		m_lstItem.AddTail (pItem);
		sscanf (pDate, "%d-%d-%d", &pItem->m_nYear, &pItem->m_nMonth, &pItem->m_nDay);
		pItem->m_dOpen = GetDblValue (pItemNode, "open_price");
		pItem->m_dMax = GetDblValue (pItemNode, "max_price");
		pItem->m_dMin = GetDblValue (pItemNode, "min_price");
		pItem->m_dClose = GetDblValue (pItemNode, "close_price");
		pItem->m_nVolume = GetIntValue (pItemNode, "trade_num");
		pItem->m_nMoney = GetIntValue (pItemNode, "trade_money");
		pItem->m_dDiffNum = GetDblValue (pItemNode, "diff_money");
		pItem->m_dDiffRate = GetDblValue (pItemNode, "diff_rate");
		pItem->m_dSwing = GetDblValue (pItemNode, "swing");
		pItem->m_dExchange = GetDblValue (pItemNode, "turnover");
	}

	//日期,开盘,收盘,最高,最低,交易量(手),交易金额(元),涨跌额,涨跌幅(%),振幅(%),换手率(%)
	//2012-05-11,35.00,34.88,36.00,33.33,158937,548030000,1.88,5.70,8.09,59.42

	return nRC;
}

int CStockHistInfo::FillPath (void)
{
#ifdef DATA_FROM_NETWORK
	char szLine[1024];
	strcpy (m_szPath, m_szHost);
	// sz-sh-stock-history?begin=2015-09-01&code=600004&end=2015-09-02'  -H 'Authorization:APPCODE 你自己的AppCode'
	sprintf (szLine, "sz-sh-stock-history?begin=%s&code=%s&end=%s", m_szStartDate, m_szCode, m_szEndDate);	
	// batch-real-stockinfo?needIndex=0&stocks=sh601006%2Csh601007%2Csh601008%2Csh601009%2Csz000018%2Chk00941'  -H 'Authorization:APPCODE 你自己的AppCode'
	//strcpy (szLine, "batch-real-stockinfo?needIndex=0&stocks=sh601006%2Csh601007%2Csh601008%2Csh601009%2Csz000018%2Chk00941");	

	strcat (m_szPath, szLine);
#else
	strcpy (m_szPath, "c:\\work\\Temp\\stockHistInfo.txt");
#endif // DATA_FROM_NETWORK
	return QC_ERR_NONE;
}

void CStockHistInfo::ReleaseData (void)
{
	qcStockKXTInfoItem *	pItem = m_lstItem.RemoveHead ();
	while (pItem != NULL)
	{
		delete pItem;
		pItem = m_lstItem.RemoveHead ();
	}
}