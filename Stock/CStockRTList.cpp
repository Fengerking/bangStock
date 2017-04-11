/*******************************************************************************
	File:		CStockRTList.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockRTList.h"

#include "ULogFunc.h"
#include "USystemfunc.h"

CStockRTList::CStockRTList(void)
	: CStockBase ()
	, m_bNeedIndex (false)
{
	SetObjectName ("CStockRTList");
	memset (m_szCodeList, 0, sizeof (m_szCodeList));
}

CStockRTList::~CStockRTList(void)
{
	ReleaseData ();
}

int CStockRTList::SetCode (char * pCode)
{
	if (pCode == NULL)
		return QC_ERR_ARG;

	strcpy (m_szCodeList, pCode);

	int nRC = FillPath ();
	if (nRC != QC_ERR_NONE)
		return nRC;

	nRC = Update ();
	if (nRC != QC_ERR_NONE)
		return nRC;

	return QC_ERR_NONE;
}

int CStockRTList::Update (void)
{
	int nRC = QC_ERR_NONE;
	nRC = CStockBase::Update ();
	if (nRC != NULL)
		return nRC;

	CJsonNode * pListNode = m_pDataJson->FindNode ("list");
	if (pListNode == NULL)
	{
		QCLOGW ("FindNode list error!");
		return QC_ERR_STATUS;
	}
	ReleaseData ();

	char *					pCodeNum = NULL;
	char *					pDate = NULL;
	qcStockRealTimeItem *	pItem = NULL;
	CJsonNode *				pItemNode = NULL;
	NODEPOS					pos = pListNode->m_lstNode.GetTailPosition ();
	while (pos != NULL)
	{
		pItemNode = pListNode->m_lstNode.GetPrev (pos);
		pCodeNum = GetText (pItemNode, "code");
		if (pCodeNum == NULL)
			continue;
		pItem = new qcStockRealTimeItem ();
		memset (pItem, 0, sizeof (qcStockRealTimeItem));
		m_lstItem.AddTail (pItem);
	
		strcpy (pItem->m_szCode, pCodeNum);
		pDate = GetText (pItemNode, "date");
		if (pDate != NULL)
			strcpy (pItem->m_szDate, pDate);

		char * pName = GetText (pItemNode, "name");
		if (pName != NULL)
		{
			strcpy (pItem->m_szName, pName);
			TranslateName (pItem->m_szName, pItem->m_wzName);
		}

		pItem->m_dNowPrice		= GetDblValue (pItemNode, "nowPrice");
		pItem->m_nTradeNum		= GetIntValue (pItemNode, "tradeNum");
		pItem->m_nTradeMoney	= GetIntValue (pItemNode, "tradeAmount");

		pItem->m_dMaxPrice		= GetDblValue (pItemNode, "todayMax");
		pItem->m_dMinPrice		= GetDblValue (pItemNode, "todayMin");
		pItem->m_dOpenPrice		= GetDblValue (pItemNode, "openPrice");
		pItem->m_dClosePrice	= GetDblValue (pItemNode, "closePrice");

		pItem->m_dSwing			= GetDblValue (pItemNode, "swing");		
		pItem->m_dDiffRate		= GetDblValue (pItemNode, "diff_rate");
		pItem->m_dDiffNum		= GetDblValue (pItemNode, "diff_money");
		pItem->m_dTurnOver		= GetDblValue (pItemNode, "turnover");    
	}

	if (!m_bNeedIndex)
		return QC_ERR_NONE;

	CJsonNode * pIndexList = m_pDataJson->FindNode ("indexList");
	if (pIndexList == NULL)
		return QC_ERR_STATUS;
	char *					pText = NULL;
	qcStockIndexInfoItem *	pInfoItem = NULL;
	CJsonNode *				pIndexItem = NULL;
	pos = pIndexList->m_lstNode.GetHeadPosition ();
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
		pInfoItem->m_szTradeNum[32];
		pInfoItem->m_szTradeMoney[32];
	}

	return QC_ERR_NONE;
}

int CStockRTList::FillPath (void)
{
#ifdef DATA_FROM_NETWORK
	strcpy (m_szPath, m_szHost);
	if (m_bNeedIndex)
		strcat (m_szPath, "batch-real-stockinfo?needIndex=1&stocks=");
	else
		strcat (m_szPath, "batch-real-stockinfo?needIndex=0&stocks=");
	strcat (m_szPath, m_szCodeList);
	//strcpy (szLine, "batch-real-stockinfo?needIndex=0&stocks=sh600000%2Csh601007%2Csh601008%2Csh601009%2Csz000018%2Chk00941");	
#else
	strcpy (m_szPath, "c:\\work\\Temp\\szHangQing.txt");
#endif // DATA_FROM_NETWORK
	return QC_ERR_NONE;
}

void CStockRTList::ReleaseData (void)
{
	qcStockRealTimeItem *	pItem = m_lstItem.RemoveHead ();
	while (pItem != NULL)
	{
		delete pItem;
		pItem = m_lstItem.RemoveHead ();
	}

	qcStockIndexInfoItem * pIndexItem = m_lstIndexItem.RemoveHead ();
	while (pIndexItem != NULL)
	{
		delete pIndexItem;
		pIndexItem = m_lstIndexItem.RemoveHead ();
	}
}