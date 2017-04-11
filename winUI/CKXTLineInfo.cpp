/*******************************************************************************
	File:		CKXTLineInfo.cpp

	Contains:	base object implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#include "stdio.h"
#include "string.h"

#include "qcDef.h"
#include "CKXTLineInfo.h"

CKXTLineInfo::CKXTLineInfo(void)
	: CBaseGraphics ()
	, m_hWnd (NULL)
	, m_pStockKXTInfo (NULL)
	, m_pLineInfo5 (NULL)
	, m_pLineInfo10 (NULL)
	, m_pLineInfo20 (NULL)
	, m_pLineInfo30 (NULL)
	, m_pLineInfo60 (NULL)
{
}

CKXTLineInfo::~CKXTLineInfo(void)
{
	ReleaseLineInfo ();
}

bool CKXTLineInfo::SetStockKXTInfo (CStockKXTInfo * pKXTInfo)
{
	if (pKXTInfo == NULL || pKXTInfo->m_lstItem.GetCount () <= 0)
		return false;

	ReleaseLineInfo ();
	m_pStockKXTInfo = pKXTInfo;
	int nItemCount = pKXTInfo->m_lstItem.GetCount ()+5;
	m_pLineInfo5 = new double [nItemCount];
	memset (m_pLineInfo5, 0, nItemCount * sizeof (double));
	m_pLineInfo10 = new double [nItemCount];
	memset (m_pLineInfo10, 0, nItemCount * sizeof (double));
	m_pLineInfo20 = new double [nItemCount];
	memset (m_pLineInfo20, 0, nItemCount * sizeof (double));
	m_pLineInfo30 = new double [nItemCount];
	memset (m_pLineInfo30, 0, nItemCount * sizeof (double));

	m_pMACD = new stockMACD[nItemCount];
	memset (m_pMACD, 0, nItemCount * sizeof (stockMACD));

	int		nIndex = 0;
	double	dLine5 = 0;
	double	aLine5[5];

	double	dLine10 = 0;
	double	aLine10[10];

	double	dLine20 = 0;
	double	aLine20[20];

	double	dLine30 = 0;
	double	aLine30[30];

	qcStockKXTInfoItem *	pItem = NULL;
	NODEPOS					pos = m_pStockKXTInfo->m_lstItem.GetHeadPosition ();
	while (pos != NULL)
	{
		pItem = m_pStockKXTInfo->m_lstItem.GetNext (pos);
		dLine5 += pItem->m_dClose;
		aLine5[nIndex%5] = pItem->m_dClose;
		if (nIndex < 4)
		{
			m_pLineInfo5[nIndex] = -1;
		}
		else
		{
			m_pLineInfo5[nIndex] = dLine5 / 5;
			dLine5 = dLine5 - aLine5[(nIndex+1)%5];
		}

		dLine10 += pItem->m_dClose;
		aLine10[nIndex%10] = pItem->m_dClose;
		if (nIndex < 9)
		{
			m_pLineInfo10[nIndex] = -1;
		}
		else
		{
			m_pLineInfo10[nIndex] = dLine10 / 10;
			dLine10 = dLine10 - aLine10[(nIndex+1)%10];
		}

		dLine20 += pItem->m_dClose;
		aLine20[nIndex%20] = pItem->m_dClose;
		if (nIndex < 19)
		{
			m_pLineInfo20[nIndex] = -1;
		}
		else
		{
			m_pLineInfo20[nIndex] = dLine20 / 20;
			dLine20 = dLine20 - aLine20[(nIndex+1)%20];
		}

		dLine30 += pItem->m_dClose;
		aLine30[nIndex%30] = pItem->m_dClose;
		if (nIndex < 29)
		{
			m_pLineInfo30[nIndex] = -1;
		}
		else
		{
			m_pLineInfo30[nIndex] = dLine30 / 30;
			dLine30 = dLine30 - aLine30[(nIndex+1)%30];
		}

		// circulate MACD data
		if (nIndex == 0)
		{
			m_pMACD[nIndex].dEMA12 = 0;
			m_pMACD[nIndex].dEMA26 = 0;
			m_pMACD[nIndex].dDIFF = 0;
			m_pMACD[nIndex].dDEA = 0;
			m_pMACD[nIndex].dBAR = 0;
		}
		else
		{
			m_pMACD[nIndex].dEMA12 = (2 * pItem->m_dClose + 11 * m_pMACD[nIndex - 1].dEMA12) / 13;
			m_pMACD[nIndex].dEMA26 = (2 * pItem->m_dClose + 25 * m_pMACD[nIndex - 1].dEMA26) / 27;;
			m_pMACD[nIndex].dDIFF = m_pMACD[nIndex].dEMA12 - m_pMACD[nIndex].dEMA26;
			m_pMACD[nIndex].dDEA = m_pMACD[nIndex - 1].dDEA * 8 / 10 + m_pMACD[nIndex].dDIFF * 2 / 10;
			m_pMACD[nIndex].dBAR = 2 * (m_pMACD[nIndex].dDIFF - m_pMACD[nIndex].dDEA);
		}

		nIndex++;
	}

	return true;
}

bool CKXTLineInfo::UpdateLastItem (CStockKXTInfo * pKXTInfo, bool bNew)
{
	if (m_pStockKXTInfo != pKXTInfo)
		return false;
	if (m_pLineInfo5 == NULL || m_pMACD == NULL)
		return false;

	int nItemCount = pKXTInfo->m_lstItem.GetCount ();

	int		nIndex = 0;
	double	dLine5 = 0;
	double	dLine10 = 0;
	double	dLine20 = 0;
	double	dLine30 = 0;

	qcStockKXTInfoItem *	pItem = NULL;
	NODEPOS					pos = m_pStockKXTInfo->m_lstItem.GetTailPosition ();
	while (pos != NULL)
	{
		pItem = m_pStockKXTInfo->m_lstItem.GetPrev (pos);
		if (nIndex < 5)
			dLine5 = dLine5 + pItem->m_dClose;
		if (nIndex < 10)
			dLine10 = dLine10 + pItem->m_dClose;
		if (nIndex < 20)
			dLine20 = dLine20 + pItem->m_dClose;
		if (nIndex < 30)
			dLine30 = dLine30 + pItem->m_dClose;
		nIndex++;
		if (nIndex >= 30)
			break;
	}
	nIndex = nItemCount - 1;
	if (nIndex < 1)
		return false;

	m_pLineInfo5[nIndex] = dLine5 / 5;
	m_pLineInfo10[nIndex] = dLine10 / 10;
	m_pLineInfo20[nIndex] = dLine20 / 20;
	m_pLineInfo30[nIndex] = dLine30 / 30;

	if (nItemCount > 1)
	{
		pItem = m_pStockKXTInfo->m_lstItem.GetTail ();	
		
		m_pMACD[nIndex].dEMA12 = (2 * pItem->m_dClose + 11 * m_pMACD[nIndex - 1].dEMA12) / 13;
		m_pMACD[nIndex].dEMA26 = (2 * pItem->m_dClose + 25 * m_pMACD[nIndex - 1].dEMA26) / 27;;
		m_pMACD[nIndex].dDIFF = m_pMACD[nIndex].dEMA12 - m_pMACD[nIndex].dEMA26;
		m_pMACD[nIndex].dDEA = m_pMACD[nIndex - 1].dDEA * 8 / 10 + m_pMACD[nIndex].dDIFF * 2 / 10;
		m_pMACD[nIndex].dBAR = 2 * (m_pMACD[nIndex].dDIFF - m_pMACD[nIndex].dDEA);
	}

	return true;
}

bool CKXTLineInfo::OnCreateWnd (HWND hWnd)
{
	m_hWnd = hWnd;
	return true;
}

bool CKXTLineInfo::ReleaseLineInfo (void)
{
	QC_DEL_A (m_pLineInfo5);
	QC_DEL_A (m_pLineInfo10);
	QC_DEL_A (m_pLineInfo20);
	QC_DEL_A (m_pLineInfo30);
	QC_DEL_A (m_pLineInfo60);

	QC_DEL_A (m_pMACD);

	return true;
}