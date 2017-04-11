/*******************************************************************************
	File:		CStockDayInfo.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockDayInfo.h"

CStockDayInfo::CStockDayInfo(void)
	: CStockBase ()
	, m_dClosePrice (0)
	, m_nTotalVolume (0)
	, m_dMaxPrice (0)
	, m_dMinPrice (0)
	, m_nMaxVolume (0)
{
	SetObjectName ("CStockDayInfo");
}

CStockDayInfo::~CStockDayInfo(void)
{
	ReleaseData ();
}

int CStockDayInfo::Update (void)
{
	int nRC = QC_ERR_NONE;
	nRC = CStockBase::Update ();
	if (nRC != NULL)
		return nRC;

	ReleaseData ();

	CJsonNode * pRootNode = m_pDataJson->GetRootNode ();
	if (pRootNode == NULL)
		return QC_ERR_STATUS;
	m_dClosePrice = GetDblValue (pRootNode, "yestclose");
	m_nTotalVolume = GetIntValue (pRootNode, "lastVolume");

	CJsonNode * pInfoNode = m_pDataJson->FindNode ("minuteList");
	if (pInfoNode == NULL)
		return QC_ERR_STATUS;

	qcStockDayInfoItem *	pItem = NULL;
	CJsonNode *				pItemNode = NULL;
	NODEPOS					pos = pInfoNode->m_lstNode.GetHeadPosition ();
	while (pos != NULL)
	{
		pItemNode = pInfoNode->m_lstNode.GetNext (pos);
		pItem = new qcStockDayInfoItem ();
		m_lstItem.AddTail (pItem);
		pItem->m_nHour = GetIntValue (pItemNode, "time");
		pItem->m_nMinute = pItem->m_nHour % 100;
		pItem->m_nHour = pItem->m_nHour / 100;
		pItem->m_dNowPrice = GetDblValue (pItemNode, "nowPrice");
		pItem->m_dAvgPrice = GetDblValue (pItemNode, "avgPrice");
		pItem->m_nVolume = GetIntValue (pItemNode, "volume");
	}

	m_dMinPrice = 10000.00;
	m_dMaxPrice = 0;
	m_nMaxVolume = 0;
	pos = m_lstItem.GetHeadPosition ();
	while (pos != NULL)
	{
		pItem = m_lstItem.GetNext (pos);
		if (m_dMaxPrice < pItem->m_dNowPrice)
			m_dMaxPrice = pItem->m_dNowPrice;
		if (m_dMinPrice > pItem->m_dNowPrice)
			m_dMinPrice = pItem->m_dNowPrice;
		if (m_nMaxVolume < pItem->m_nVolume)
			m_nMaxVolume = pItem->m_nVolume;
	}
	if (m_dMaxPrice >  m_dClosePrice && m_dMinPrice >= m_dClosePrice)
	{
		m_dMinPrice = m_dClosePrice * 2 - m_dMaxPrice;
	}
	else if (m_dMaxPrice >  m_dClosePrice && m_dMinPrice < m_dClosePrice)
	{
		if (m_dMaxPrice - m_dClosePrice > m_dClosePrice - m_dMinPrice)
			m_dMinPrice = m_dClosePrice * 2 - m_dMaxPrice;
		else
			m_dMaxPrice = m_dClosePrice * 2 - m_dMinPrice;
	}
	else
	{
		m_dMaxPrice = m_dClosePrice * 2 - m_dMinPrice;
	}

	return nRC;
}

int CStockDayInfo::FillPath (void)
{
#ifdef DATA_FROM_NETWORK
	char szLine[1024];
	strcpy (m_szPath, m_szHost);
	sprintf (szLine, "timeline?code=%s&day=1", m_szCode);	
	strcat (m_szPath, szLine);
#else
	strcpy (m_szPath, "c:\\work\\Temp\\RealTime.txt");
#endif // DATA_FROM_NETWORK
	return QC_ERR_NONE;
}

void CStockDayInfo::ReleaseData (void)
{
	qcStockDayInfoItem *	pItem = m_lstItem.RemoveHead ();
	while (pItem != NULL)
	{
		delete pItem;
		pItem = m_lstItem.RemoveHead ();
	}
}