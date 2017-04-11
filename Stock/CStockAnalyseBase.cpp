/*******************************************************************************
	File:		CStockAnalyseBase.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockAnalyseBase.h"

#include "CFileIO.h"
#include "ULOgFunc.h"
#include "USystemFunc.h"

CStockAnalyseBase::CStockAnalyseBase(void)
	: CBaseObject ()
	, m_pCodeList (NULL)
	, m_pKXTInfo (NULL)
	, m_nResultCount (0)
	, m_nEndDate2000 (0)
{
	SetObjectName ("CStockAnalyseBase");

	memset (&m_tmStart, 0, sizeof (m_tmStart));
	memset (&m_tmEnd, 0, sizeof (m_tmStart));
}

CStockAnalyseBase::~CStockAnalyseBase(void)
{
	QC_DEL_P (m_pCodeList);
	QC_DEL_P (m_pKXTInfo);
}

int	CStockAnalyseBase::Init (char * pCodeListFile)
{
	if (m_pCodeList == NULL)
	{
		m_pCodeList = new CStockFileCode ();
		m_pCodeList->Open (pCodeListFile);
	}
	m_pCodeList->SetCurSel (0);

	if (m_pKXTInfo == NULL)
		m_pKXTInfo = new CStockFileHist ();

	long long llStart, llEnd;
	SystemTimeToFileTime (&m_tmStart, (LPFILETIME)&llStart);
	SystemTimeToFileTime (&m_tmEnd, (LPFILETIME)&llEnd);
	m_pKXTInfo->SetStartEndDate (llStart, llEnd);

	m_nResultCount = 0;

	return QC_ERR_NONE;
}

int CStockAnalyseBase::GetTotalNum (void)
{
	if (m_pCodeList == NULL)
		return 0;
	return m_pCodeList->GetCodeCount ();
}

int	CStockAnalyseBase::GetCurIndex (void)
{
	if (m_pCodeList == NULL)
		return -1;
	return m_pCodeList->GetCurSel ();
}

int CStockAnalyseBase::SetStartEndDate (SYSTEMTIME * ptmStart, SYSTEMTIME * ptmEnd)
{
	if (ptmStart == NULL || ptmEnd == NULL)
		return QC_ERR_ARG;
	memcpy (&m_tmStart, ptmStart, sizeof (SYSTEMTIME));
	memcpy (&m_tmEnd, ptmEnd, sizeof (SYSTEMTIME));
	m_nEndDate2000 = qcGetDaysFrom2000 (m_tmEnd.wYear, m_tmEnd.wMonth, m_tmEnd.wDay);
	return QC_ERR_NONE;
}

int CStockAnalyseBase::Analyse (bool bNext)
{
	return QC_ERR_FAILED;
}

int	CStockAnalyseBase::DumpResult (void)
{
	return QC_ERR_FAILED;
}

//	qsort(ppPriceItems, nItemNum, sizeof(qcStockKXTInfoItem *), compareClosePrice);
int __cdecl CStockAnalyseBase::compareClosePrice (const void *arg1, const void *arg2)
{
	qcStockKXTInfoItem * pItem1 = *(qcStockKXTInfoItem **)arg1;
	qcStockKXTInfoItem * pItem2 = *(qcStockKXTInfoItem **)arg2;
	return (int)(pItem1->m_dClose * 100 - pItem2->m_dClose * 100);
}
