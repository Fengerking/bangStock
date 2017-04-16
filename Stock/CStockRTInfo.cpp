/*******************************************************************************
	File:		CStockRTInfo.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockRTInfo.h"

CStockRTInfo::CStockRTInfo(void)
	: CStockBase ()
{
	SetObjectName ("CStockRTInfo");
	memset (&m_rtItem, 0, sizeof (m_rtItem));
}

CStockRTInfo::~CStockRTInfo(void)
{
	ReleaseData ();
}

int CStockRTInfo::Update (void)
{
	int nRC = QC_ERR_NONE;
	nRC = CStockBase::Update ();
	if (nRC != NULL)
		return nRC;

	CJsonNode * pInfoNode = m_pDataJson->FindNode ("stockMarket");
	if (pInfoNode == NULL)
		return QC_ERR_STATUS;

	char * pCode = GetText (pInfoNode, "code");
	if (pCode != NULL)
		strcpy (m_rtItem.m_szCode, pCode);
	char * pName = GetText (pInfoNode, "name");
	if (pName != NULL)
	{
		strcpy (m_szName, pName);
		TranslateName (m_szName, m_wzName);
		strcpy (m_rtItem.m_szName, pName);
		wcscpy (m_rtItem.m_wzName, m_wzName);
	}
	if (GetText (pInfoNode, "date") != NULL)
		strcpy (m_szDate, GetText (pInfoNode, "date"));
	if (GetText (pInfoNode, "time") != NULL)
		strcpy (m_szTime, GetText (pInfoNode, "time"));

	m_rtItem.m_dNowPrice	= GetDblValue (pInfoNode, "nowPrice");
	m_rtItem.m_nTradeNum	= GetIntValue (pInfoNode, "tradeNum");
	m_rtItem.m_nTradeMoney	= GetIntValue (pInfoNode, "tradeAmount");

	m_rtItem.m_dMaxPrice	= GetDblValue (pInfoNode, "todayMax");
	m_rtItem.m_dMinPrice	= GetDblValue (pInfoNode, "todayMin");
	m_rtItem.m_dOpenPrice	= GetDblValue (pInfoNode, "openPrice");
	m_rtItem.m_dClosePrice	= GetDblValue (pInfoNode, "closePrice");

	m_rtItem.m_dSwing		= GetDblValue (pInfoNode, "swing");		
	m_rtItem.m_dDiffRate	= GetDblValue (pInfoNode, "diff_rate");
	m_rtItem.m_dDiffNum		= GetDblValue (pInfoNode, "diff_money");
	m_rtItem.m_dTurnOver		= GetDblValue (pInfoNode, "turnover");    

	m_rtItem.m_dHighLimit	= GetDblValue (pInfoNode, "highLimit");
	m_rtItem.m_dDownLimit	= GetDblValue (pInfoNode, "downLimit");

	m_rtItem.m_dBuyPrice1	= GetDblValue (pInfoNode, "buy1_m");
	m_rtItem.m_dBuyPrice2	= GetDblValue (pInfoNode, "buy2_m");
	m_rtItem.m_dBuyPrice3	= GetDblValue (pInfoNode, "buy3_m");
	m_rtItem.m_dBuyPrice4	= GetDblValue (pInfoNode, "buy4_m");
	m_rtItem.m_dBuyPrice5	= GetDblValue (pInfoNode, "buy5_m");
	m_rtItem.m_nBuyNum1		= GetIntValue (pInfoNode, "buy1_n");
	m_rtItem.m_nBuyNum2		= GetIntValue (pInfoNode, "buy2_n");
	m_rtItem.m_nBuyNum3		= GetIntValue (pInfoNode, "buy3_n");
	m_rtItem.m_nBuyNum4		= GetIntValue (pInfoNode, "buy4_n");
	m_rtItem.m_nBuyNum5		= GetIntValue (pInfoNode, "buy5_n");
	m_rtItem.m_dSellPrice1	= GetDblValue (pInfoNode, "sell1_m");
	m_rtItem.m_dSellPrice2	= GetDblValue (pInfoNode, "sell2_m");
	m_rtItem.m_dSellPrice3	= GetDblValue (pInfoNode, "sell3_m");
	m_rtItem.m_dSellPrice4	= GetDblValue (pInfoNode, "sell4_m");
	m_rtItem.m_dSellPrice5	= GetDblValue (pInfoNode, "sell5_m");
	m_rtItem.m_nSellNum1	= GetIntValue (pInfoNode, "sell1_n");
	m_rtItem.m_nSellNum2	= GetIntValue (pInfoNode, "sell2_n");
	m_rtItem.m_nSellNum3	= GetIntValue (pInfoNode, "sell3_n");
	m_rtItem.m_nSellNum4	= GetIntValue (pInfoNode, "sell4_n");
	m_rtItem.m_nSellNum5	= GetIntValue (pInfoNode, "sell5_n");

//	m_rtItem.m_nAllNum		= GetIntValue (pInfoNode, "nowPrice");
	m_rtItem.m_dAllMoney		= GetDblValue (pInfoNode, "all_value");

	ReleaseData ();
	CJsonNode * pIndexList = m_pDataJson->FindNode ("indexList");
	if (pIndexList == NULL)
		return QC_ERR_STATUS;
	char *					pText = NULL;
	qcStockIndexInfoItem *	pInfoItem = NULL;
	CJsonNode *				pIndexItem = NULL;
	NODEPOS pos = pIndexList->m_lstNode.GetHeadPosition ();
	while (pos != NULL)
	{
		pIndexItem = pIndexList->m_lstNode.GetNext (pos);
		pInfoItem = new qcStockIndexInfoItem ();
		m_lstIndexItem.AddTail (pInfoItem);
		pText = GetText (pIndexItem, "code");
		if (pText != NULL)
			strcpy (pInfoItem->m_szCode, pText);
		pText = GetText (pIndexItem, "name");
		if (pText != NULL)
			strcpy (pInfoItem->m_szName, pText);
		pText = GetText (pIndexItem, "time");
		if (pText != NULL)
			strcpy (pInfoItem->m_szTime, pText);
		pText = GetText (pIndexItem, "tradeNum");
		if (pText != NULL)
			strcpy (pInfoItem->m_szTradeNum, pText);
		pText = GetText (pIndexItem, "tradeAmount");
		if (pText != NULL)
			strcpy (pInfoItem->m_szTradeMoney, pText);
		pInfoItem->m_dClose = GetDblValue (pIndexItem, "yestodayClosePrice");
		pInfoItem->m_dOpen = GetDblValue (pIndexItem, "todayOpenPrice");
		pInfoItem->m_dNow = GetDblValue (pIndexItem, "nowPrice");
		pInfoItem->m_dMax = GetDblValue (pIndexItem, "maxPrice");
		pInfoItem->m_dMin = GetDblValue (pIndexItem, "minPrice");
		pInfoItem->m_dDiffRate = GetDblValue (pIndexItem, "diff_rate");
		pInfoItem->m_dDiffMoney = GetDblValue (pIndexItem, "diff_money");
	}
	return nRC;
}

int CStockRTInfo::FillPath (void)
{
#ifdef DATA_FROM_NETWORK
	char szLine[1024];
	strcpy (m_szPath, m_szHost);
	sprintf (szLine, "real-stockinfo?code=%s&needIndex=1&need_k_pic=0", m_szCode);
	strcat (m_szPath, szLine);
#else
	strcpy (m_szPath, "c:\\work\\Temp\\szHangQing.txt");
#endif // DATA_FROM_NETWORK
	return QC_ERR_NONE;
}

void CStockRTInfo::ReleaseData (void)
{
	qcStockIndexInfoItem * pIndexItem = m_lstIndexItem.RemoveHead ();
	while (pIndexItem != NULL)
	{
		delete pIndexItem;
		pIndexItem = m_lstIndexItem.RemoveHead ();
	}
}
