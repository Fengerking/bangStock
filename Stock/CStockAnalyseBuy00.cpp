/*******************************************************************************
	File:		CStockAnalyseBuy00.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockAnalyseBuy00.h"

#include "CFileIO.h"
#include "ULOgFunc.h"
#include "USystemFunc.h"

CStockAnalyseBuy00::CStockAnalyseBuy00(void)
	: CStockAnalyseBase ()
	, m_nUp (0)
	, m_nDown (0)
{
	SetObjectName ("CStockAnalyseBuy00");
}

CStockAnalyseBuy00::~CStockAnalyseBuy00(void)
{
}

int	CStockAnalyseBuy00::Analyse (bool bNext)
{
	if (m_pCodeList == NULL)
		return QC_ERR_STATUS;

	int nCurSel = m_pCodeList->GetCurSel ();
	if (nCurSel >= m_pCodeList->GetCodeCount () - 1)
		return QC_ERR_FINISH;

	int nRC = m_pKXTInfo->SetCode (m_pCodeList->GetCodeNum (nCurSel));
	m_pCodeList->SetCurSel (nCurSel+1);
	if (nRC != QC_ERR_NONE)
		return nRC;

//	if (!CheckDayLine5On10 ())
	if (!CheckLastTwoDays ())
		return QC_ERR_FAILED;

	if (m_nResultCount < m_pCodeList->GetCodeCount ())
		strcpy (m_szResult[m_nResultCount], m_pCodeList->GetCodeNum (nCurSel));
	m_nResultCount++;

	return QC_ERR_NONE;
}

bool CStockAnalyseBuy00::CheckDayLine5On10 (void)
{
	qcStockKXTInfoItem *	pItem = NULL;
	NODEPOS					pos = m_pKXTInfo->m_lstItem.GetTailPosition ();
	int						nIndex = 0;
	double					dPrevLine5 = 0;
	double					dPrevClose = 0;

	if (m_pKXTInfo->m_lstItem.GetCount () < 10)
		return false;

	while (pos != NULL)
	{
		if (pItem != NULL)
		{
			dPrevLine5 = pItem->m_pDayLine->m_dLine5;
			dPrevClose = pItem->m_dClose;
		}
		pItem = m_pKXTInfo->m_lstItem.GetPrev (pos);
		if (pItem->m_pDayLine == NULL)
			return false;
		nIndex++;
		if (nIndex <= 2 )
			continue;
		if (nIndex >= 5)
			break;
		if (pItem->m_pDayLine->m_dLine5 > dPrevLine5)
			return false;
		if (pItem->m_dClose > dPrevClose)
			return false;
		if (pItem->m_dClose < pItem->m_dOpen)
			return false;
		if (pItem->m_dClose < pItem->m_pDayLine->m_dLine10)
			return false;
		if (pItem->m_pDayLine->m_dLine10 > pItem->m_pDayLine->m_dLine5)
			return false;
	}

	qcStockKXTInfoItem *	pLast = NULL;
	pos = m_pKXTInfo->m_lstItem.GetTailPosition ();
	pLast = m_pKXTInfo->m_lstItem.GetPrev (pos);
	pItem = m_pKXTInfo->m_lstItem.GetPrev (pos);
	pItem = m_pKXTInfo->m_lstItem.GetPrev (pos);
	if (pLast->m_dClose > pItem->m_dClose)
		m_nUp++;
	else
		m_nDown++;
	return true;
}

bool CStockAnalyseBuy00::CheckLastTwoDays (void)
{
	if (m_pKXTInfo->m_lstItem.GetCount () < 10)
		return false;
	NODEPOS					pos = m_pKXTInfo->m_lstItem.GetTailPosition ();
	qcStockKXTInfoItem *	pDay0= m_pKXTInfo->m_lstItem.GetPrev (pos);
	qcStockKXTInfoItem *	pDay1= m_pKXTInfo->m_lstItem.GetPrev (pos);
	qcStockKXTInfoItem *	pDay2= m_pKXTInfo->m_lstItem.GetPrev (pos);

	if (pDay0->m_nDay != 14)
		return false;

	if (pDay0->m_dClose < pDay1->m_dClose)
		return false;
	if (pDay1->m_dClose < pDay2->m_dClose)
		return false;

	return true;
}