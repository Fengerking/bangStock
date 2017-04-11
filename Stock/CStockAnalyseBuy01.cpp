/*******************************************************************************
	File:		CStockAnalyseBuy01.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockAnalyseBuy01.h"

#include "CFileIO.h"
#include "ULOgFunc.h"
#include "USystemFunc.h"

CStockAnalyseBuy01::CStockAnalyseBuy01(void)
	: CStockAnalyseBase ()
	, m_nUpDays (5)
	, m_dUpMin (1.0)
	, m_dUpMax (5.0)
	, m_dDownMax (30)
	, m_dMaxDiff (50)
	, m_nMaxDays (365)
	, m_bDayLine5 (false)
	, m_bMACDKingX (false)
{
	SetObjectName ("CStockAnalyseBuy01");
}

CStockAnalyseBuy01::~CStockAnalyseBuy01(void)
{
}

int CStockAnalyseBuy01::SetFirstParam (int nUpDays, double dMin, double dMax)
{
	m_nUpDays = nUpDays;
	m_dUpMin = dMin;
	m_dUpMax = dMax;

	return QC_ERR_NONE;
}

int	CStockAnalyseBuy01::SetSecondParam (double dDown)
{
	m_dDownMax = dDown;

	return QC_ERR_NONE;
}

int CStockAnalyseBuy01::SetThirdParam (double dMaxDiff, int nMaxDays)
{
	m_dMaxDiff = dMaxDiff;
	m_nMaxDays = nMaxDays;
	return QC_ERR_NONE;
}

int CStockAnalyseBuy01::SetDayLine5 (bool bDayLine5)
{
	m_bDayLine5 = bDayLine5;
	return QC_ERR_NONE;
}

int CStockAnalyseBuy01::SetMACDKingX (bool bMACDKingX)
{
	m_bMACDKingX = bMACDKingX;
	return QC_ERR_NONE;
}

int	CStockAnalyseBuy01::Analyse (bool bNext)
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

	if (!CheckFirstParam ())
		return QC_ERR_FAILED;

	if (!CheckSecondParam ())
		return QC_ERR_FAILED;

	if (!CheckThirdParam ())
		return QC_ERR_FAILED;

	if (m_nResultCount < m_pCodeList->GetCodeCount ())
		strcpy (m_szResult[m_nResultCount], m_pCodeList->GetCodeNum (nCurSel));
	m_nResultCount++;

	return QC_ERR_NONE;
}

bool CStockAnalyseBuy01::CheckFirstParam (void)
{
	if (m_pKXTInfo->m_lstItem.GetCount () <= m_nUpDays)
		return false;

	double *				pPrice = new double[m_nUpDays];
	double					dMaxPrice = 0;
	int						nMaxIndex = 0;
	double					dMinPrice = 10000;
	int						nMinIndex = 0;
	int						nIndex = 0;
	long long				llLastDate = 0;
	qcStockKXTInfoItem *	pItem = m_pKXTInfo->m_lstItem.GetTail ();
	if (m_nEndDate2000 > qcGetDaysFrom2000 (pItem->m_nYear, pItem->m_nMonth, pItem->m_nDay))
		return false;

	NODEPOS pos = m_pKXTInfo->m_lstItem.GetTailPosition ();
	while (pos != NULL)
	{
		pItem = m_pKXTInfo->m_lstItem.GetPrev (pos);
		if (dMaxPrice < pItem->m_dClose)
		{
			dMaxPrice = pItem->m_dClose;
			nMaxIndex = nIndex;
		}
		if (dMinPrice > pItem->m_dClose)
		{
			dMinPrice = pItem->m_dClose;
			nMinIndex = nIndex;
		}

		pPrice[nIndex] = pItem->m_dClose;
		nIndex++;
		if (nIndex >= m_nUpDays)
			break;
	}
	delete []pPrice;
	if (nMaxIndex > nMinIndex)
		return false;
	if (nMaxIndex > m_nUpDays / 2)
		return false;
	if (nMinIndex < m_nUpDays / 2)
		return false;

	if ((dMaxPrice - dMinPrice) * 100 / dMinPrice < m_dUpMin )
		return false;
	if ((dMaxPrice - dMinPrice) * 100 / dMinPrice > m_dUpMax )
		return false;

	pItem = m_pKXTInfo->m_lstItem.GetTail ();
	// 最后一日五日均线要在收盘价以上
	if (m_bDayLine5)
	{
		if (pItem->m_dClose < pItem->m_pDayLine->m_dLine5)
			return false;
	}

	// 最后一日的MACD金叉
	if (m_bMACDKingX)
	{
		int nBar = (int)(pItem->m_pMACD->m_dBAR * 10);
		if (abs (nBar) > 2)
			return false;

		bool	bUP = false;
		double	dPrevBar = 0;
		nIndex = 0;
		pos = m_pKXTInfo->m_lstItem.GetTailPosition ();
		while (pos != NULL)
		{
			pItem = m_pKXTInfo->m_lstItem.GetPrev (pos);
			nIndex++;
			if (nIndex == 1)
			{
				dPrevBar = pItem->m_pMACD->m_dBAR;
				continue;
			}
			if (pItem->m_pMACD->m_dBAR > dPrevBar)
				break;
			dPrevBar = pItem->m_pMACD->m_dBAR;
			if (nIndex >= 3)
			{
				bUP = true;
				break;
			}
		}
		if (!bUP)
			return false;
	}

	return true;
}

bool CStockAnalyseBuy01::CheckSecondParam (void)
{
	double					dMaxPrice = 0;
	double					dStartPrice = 0;
	double					dCurPrice = 0;
	double					dPrevPrice = 0;
	int						nDownNum = 0;
	int						nIndex = 0;
	qcStockKXTInfoItem *	pItem = NULL;
	NODEPOS pos = m_pKXTInfo->m_lstItem.GetTailPosition ();
	while (pos != NULL)
	{
		pItem = m_pKXTInfo->m_lstItem.GetPrev (pos);
		if (dPrevPrice == 0)
			dPrevPrice = pItem->m_dClose;
		if (pItem->m_dClose / dPrevPrice > 1.2)
			return false;
		dPrevPrice = pItem->m_dClose;
		nIndex++;
		if (nIndex >= m_nUpDays)
			break;
	}
	if (pItem == NULL)
		return false;

	dStartPrice = pItem->m_dClose;
	dCurPrice = pItem->m_dClose;
	dPrevPrice = pItem->m_dClose;
	nIndex = 0;
	while (pos != NULL)
	{
		pItem = m_pKXTInfo->m_lstItem.GetPrev (pos);
		if (pItem->m_dClose / dPrevPrice > 1.2)
			return false;
		dPrevPrice = pItem->m_dClose;
		if (dMaxPrice < pItem->m_dClose)
			dMaxPrice = pItem->m_dClose;
		nIndex++;
		if (dMaxPrice > dStartPrice * (m_dDownMax + 100) / 100)
			break;
		if (nIndex >= 60)
			break;
	}

	if (dMaxPrice > dStartPrice * (m_dDownMax + 100) / 100)
		return true;
	else
		return false;
}

bool CStockAnalyseBuy01::CheckThirdParam (void)
{
	double					dMinPrice = 10000;
	double					dStartPrice = 0;
	int						nIndex = 0;
	qcStockKXTInfoItem *	pItem = m_pKXTInfo->m_lstItem.GetTail ();
	dStartPrice = pItem->m_dClose;
	NODEPOS pos = m_pKXTInfo->m_lstItem.GetTailPosition ();
	while (pos != NULL)
	{
		pItem = m_pKXTInfo->m_lstItem.GetPrev (pos);
		if (dMinPrice > pItem->m_dClose)
			dMinPrice = pItem->m_dClose;
		nIndex++;
		if (nIndex >= m_nMaxDays)
			break;
	}

	if (dStartPrice > dMinPrice * (m_dMaxDiff + 100) / 100)
		return false;
	else
		return true;
}

