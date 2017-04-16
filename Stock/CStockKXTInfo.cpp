/*******************************************************************************
	File:		CStockKXTInfo.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockKXTInfo.h"

#include "CStockFileFHSP.h"
#include "CRegMng.H"
#include "USystemFunc.h"

CStockKXTInfo::CStockKXTInfo(void)
	: CStockBase ()
	, m_dMaxPrice (0)
	, m_dMinPrice (0)
	, m_nMaxVolume (0)
	, m_nDay5 (5)
	, m_nDay10 (10)
	, m_nDay20 (20)
	, m_nDay30 (30)
	, m_nDay60 (250)
	, m_nDay120 (120)
	, m_dMaxMacdBar (0)
	, m_dMinMacdBar (0)
{
	SetObjectName ("CStockKXTInfo");
	memset (m_szStartDate, 0, sizeof (m_szStartDate));
	memset (m_szEndDate, 0, sizeof (m_szEndDate));
	strcpy (m_szStartDate, "20000101");

	m_llStartDate = 0;
	SYSTEMTIME tmNow;
	GetLocalTime (&tmNow);
	SystemTimeToFileTime (&tmNow, (LPFILETIME)&m_llEndDate);
}

CStockKXTInfo::~CStockKXTInfo(void)
{
	ReleaseData ();
}

int CStockKXTInfo::SetDate (char * pStart, char * pEnd)
{
	if (pStart != NULL)
		strcpy (m_szStartDate, pStart);
	if (pEnd != NULL)
		strcpy (m_szEndDate, pEnd);
	return QC_ERR_NONE;
}

int CStockKXTInfo::SetStartEndDate (long long llStartDate, long long llEndDate)
{
	m_llStartDate = llStartDate;
	m_llEndDate = llEndDate;
	return QC_ERR_NONE;
}

int CStockKXTInfo::Update (void)
{
	int nRC = QC_ERR_NONE;
	nRC = CStockBase::Update ();
	if (nRC != NULL)
		return nRC;

	ReleaseData ();

	CJsonNode * pInfoNode = m_pDataJson->FindNode ("dataList");
	if (pInfoNode == NULL)
		return QC_ERR_STATUS;

	qcStockKXTInfoItem *	pItem = NULL;
	CJsonNode *				pItemNode = NULL;
	NODEPOS					pos = pInfoNode->m_lstNode.GetTailPosition ();
	while (pos != NULL)
	{
		pItemNode = pInfoNode->m_lstNode.GetPrev (pos);
		pItem = new qcStockKXTInfoItem ();
		pItem->m_pDayLine = NULL;
		pItem->m_pMACD = NULL;
		m_lstItem.AddTail (pItem);
		pItem->m_nYear = GetIntValue (pItemNode, "time");
		pItem->m_nMonth = (pItem->m_nYear % 10000) / 100;
		pItem->m_nDay = pItem->m_nYear % 100;
		pItem->m_nYear = pItem->m_nYear / 10000;
		pItem->m_dOpen = GetDblValue (pItemNode, "open");
		pItem->m_dMax = GetDblValue (pItemNode, "max");
		pItem->m_dMin = GetDblValue (pItemNode, "min");
		pItem->m_dClose = GetDblValue (pItemNode, "close");
		pItem->m_nVolume = GetIntValue (pItemNode, "volumn");

		pItem->m_nMoney = 0;		
		pItem->m_dDiffNum = 0;		
		pItem->m_dDiffRate = 0;		
		pItem->m_dSwing = 0;
		pItem->m_dExchange = 0;	
	}
	UpdateWithFHSP ();
	UpdateMaxMinPrice ();
	CreateDayLineMACD ();	

	return nRC;
}

int CStockKXTInfo::UpdateTodayItem (qcStockRealTimeItem * pRTItem)
{
	qcStockKXTInfoItem * pLastItem = m_lstItem.GetTail ();
	if (pLastItem == NULL)
		return QC_ERR_STATUS;
	SYSTEMTIME	tmSystem;
	GetLocalTime (&tmSystem);
	if (pLastItem->m_nDay != tmSystem.wDay)
	{
		pLastItem = new qcStockKXTInfoItem ();
		pLastItem->m_pDayLine = NULL;
		pLastItem->m_pMACD = NULL;
		m_lstItem.AddTail (pLastItem);
		pLastItem->m_nYear = tmSystem.wYear;
		pLastItem->m_nMonth = tmSystem.wMonth;
		pLastItem->m_nDay = tmSystem.wDay;
	}
	pLastItem->m_dMin = pRTItem->m_dMinPrice;
	pLastItem->m_dMax = pRTItem->m_dMaxPrice;
	pLastItem->m_dOpen = pRTItem->m_dOpenPrice;
	pLastItem->m_dClose = pRTItem->m_dNowPrice;
	pLastItem->m_nVolume = pRTItem->m_nTradeNum / 100;
	pLastItem->m_nMoney = pRTItem->m_nTradeMoney;
	pLastItem->m_dDiffNum = pRTItem->m_dDiffNum;
	pLastItem->m_dDiffRate = pRTItem->m_dDiffRate;
	pLastItem->m_dSwing = pRTItem->m_dSwing;
	pLastItem->m_dExchange = pRTItem->m_dTurnOver;

	UpdateTodayLineMACD ();

	return QC_ERR_NONE;
}

int CStockKXTInfo::SetDaysNum (int nDay5, int nDay10, int nDay20, int nDay30, int nDay60, int nDay120)
{
	if (nDay5 <= 0 || nDay10 <= 0 || nDay20 <= 0 || nDay30 <= 0 || nDay60 <= 0 || nDay10 <= 120)
		return QC_ERR_ARG;

	m_nDay5 = nDay5;
	m_nDay10 = nDay10;
	m_nDay20 = nDay20;
	m_nDay30 = nDay30;
	m_nDay60 = nDay60;
	m_nDay120 = nDay120;
	return QC_ERR_NONE;
}

void CStockKXTInfo::UpdateWithFHSP (void)
{
	int nChuFuQuan = 1;
	if (CRegMng::g_pRegMng != NULL)
		nChuFuQuan = CRegMng::g_pRegMng->GetIntValue ("ChuFuQuan", 1);
	if (nChuFuQuan == 0)
		return;

	CStockFileFHSP stkFHSP;
	if (stkFHSP.Open (m_szCode, false) != QC_ERR_NONE)
		return;

	double	dDiffPrice = 0;
	double	dScale = 1.0;
	int		nDaysFSHP = 0;
	int		nDaysKXT = 0;

	NODEPOS					posFHSP = NULL;
	qcStockFHSPInfoItem *	pItemFHSP = NULL;
	qcStockKXTInfoItem *	pItemKXT = NULL;
	NODEPOS					posKXT = NULL;
	if (nChuFuQuan == 1)
	{
		posFHSP = stkFHSP.m_lstItem.GetHeadPosition ();
		if (posFHSP == NULL)
			return;
		pItemFHSP = stkFHSP.m_lstItem.GetNext (posFHSP);
		nDaysFSHP = qcGetDaysFrom2000 (pItemFHSP->m_nYear, pItemFHSP->m_nMonth, pItemFHSP->m_nDay);
		posKXT = m_lstItem.GetTailPosition ();
		while (posKXT != NULL)
		{
			pItemKXT = m_lstItem.GetPrev (posKXT);
			pItemKXT->m_dClose	= (pItemKXT->m_dClose - dDiffPrice) / dScale;
			pItemKXT->m_dOpen	= (pItemKXT->m_dOpen - dDiffPrice) / dScale;
			pItemKXT->m_dMin	= (pItemKXT->m_dMin - dDiffPrice) / dScale;
			pItemKXT->m_dMax	= (pItemKXT->m_dMax - dDiffPrice) / dScale;
			pItemKXT->m_nVolume = (int)(pItemKXT->m_nVolume  * dScale);
	
			nDaysKXT = qcGetDaysFrom2000 (pItemKXT->m_nYear, pItemKXT->m_nMonth, pItemKXT->m_nDay);
			if (nDaysKXT <= nDaysFSHP)
			{
				dDiffPrice = dDiffPrice + pItemFHSP->m_dRation;
				dScale = dScale * (1 + pItemFHSP->m_dGive + pItemFHSP->m_dGain);
				if (posFHSP != NULL)
				{
					pItemFHSP = stkFHSP.m_lstItem.GetNext (posFHSP);
					nDaysFSHP = qcGetDaysFrom2000 (pItemFHSP->m_nYear, pItemFHSP->m_nMonth, pItemFHSP->m_nDay);
				}
			}
		}
	}
	else
	{
		posFHSP = stkFHSP.m_lstItem.GetTailPosition ();
		if (posFHSP == NULL)
			return;
		pItemFHSP = stkFHSP.m_lstItem.GetPrev (posFHSP);
		posKXT = m_lstItem.GetHeadPosition ();
		while (posKXT != NULL)
		{
			pItemKXT = m_lstItem.GetNext (posKXT);
			if (pItemKXT->m_nYear == pItemFHSP->m_nYear && pItemKXT->m_nMonth == pItemFHSP->m_nMonth && pItemKXT->m_nDay == pItemFHSP->m_nDay)
			{
				dDiffPrice = dDiffPrice + pItemFHSP->m_dRation;
				dScale = dScale * (1 + pItemFHSP->m_dGive + pItemFHSP->m_dGain);
				if (posFHSP != NULL)
					pItemFHSP = stkFHSP.m_lstItem.GetPrev (posFHSP);
			}
			pItemKXT->m_dClose	= (pItemKXT->m_dClose + dDiffPrice) * dScale;
			pItemKXT->m_dOpen	= (pItemKXT->m_dOpen + dDiffPrice) * dScale;
			pItemKXT->m_dMin	= (pItemKXT->m_dMin + dDiffPrice) * dScale;
			pItemKXT->m_dMax	= (pItemKXT->m_dMax + dDiffPrice) * dScale;
			pItemKXT->m_nVolume = (int)(pItemKXT->m_nVolume  * dScale);
		}	
	}
}

void CStockKXTInfo::UpdateMaxMinPrice (void)
{
	qcStockKXTInfoItem *	pItem = NULL;
	m_dMinPrice = 10000.00;
	m_dMaxPrice = 0;
	m_nMaxVolume = 0;
	NODEPOS		pos = m_lstItem.GetHeadPosition ();
	while (pos != NULL)
	{
		pItem = m_lstItem.GetNext (pos);
		if (m_dMaxPrice < pItem->m_dMax)
			m_dMaxPrice = pItem->m_dMax;
		if (m_dMinPrice > pItem->m_dMin)
			m_dMinPrice = pItem->m_dMin;
		if (m_nMaxVolume < pItem->m_nVolume)
			m_nMaxVolume = pItem->m_nVolume;
	}
}

int CStockKXTInfo::CreateDayLineMACD (void)
{
	int			nIndex = 0;
	double		dLine5 = 0;
	double *	pLine5 = new double[m_nDay5];
	double		dLine10 = 0;
	double *	pLine10 = new double[m_nDay10];
	double		dLine20 = 0;
	double *	pLine20 = new double[m_nDay20];
	double		dLine30 = 0;
	double *	pLine30 = new double[m_nDay30];
	double		dLine60 = 0;
	double *	pLine60 = new double[m_nDay60];
	double		dLine120 = 0;
	double *	pLine120 = new double[m_nDay120];

	double		dPrevEMA12 = 0;
	double		dPrevEMA26 = 0;
	double		dPrevDEA = 0;

	qcStockKXTInfoItem *	pItem = NULL;
	NODEPOS					pos = m_lstItem.GetHeadPosition ();
	while (pos != NULL)
	{
		pItem = m_lstItem.GetNext (pos);
		pItem->m_pDayLine = new qcDayLine ();
		pItem->m_pMACD = new qcMACD ();

		dLine5 += pItem->m_dClose;
		pLine5[nIndex%m_nDay5] = pItem->m_dClose;
		if (nIndex < m_nDay5 - 1)
		{
			pItem->m_pDayLine->m_dLine5 = -1;
		}
		else
		{
			pItem->m_pDayLine->m_dLine5 = dLine5 / m_nDay5;
			dLine5 = dLine5 - pLine5[(nIndex+1)%m_nDay5];
		}

		dLine10 += pItem->m_dClose;
		pLine10[nIndex%m_nDay10] = pItem->m_dClose;
		if (nIndex < m_nDay10 - 1)
		{
			pItem->m_pDayLine->m_dLine10 = -1;
		}
		else
		{
			pItem->m_pDayLine->m_dLine10 = dLine10 / m_nDay10;
			dLine10 = dLine10 - pLine10[(nIndex+1)%m_nDay10];
		}

		dLine20 += pItem->m_dClose;
		pLine20[nIndex%m_nDay20] = pItem->m_dClose;
		if (nIndex < m_nDay20 - 1)
		{
			pItem->m_pDayLine->m_dLine20 = -1;
		}
		else
		{
			pItem->m_pDayLine->m_dLine20 = dLine20 / m_nDay20;
			dLine20 = dLine20 - pLine20[(nIndex+1)%m_nDay20];
		}

		dLine30 += pItem->m_dClose;
		pLine30[nIndex%m_nDay30] = pItem->m_dClose;
		if (nIndex < m_nDay30 - 1)
		{
			pItem->m_pDayLine->m_dLine30 = -1;
		}
		else
		{
			pItem->m_pDayLine->m_dLine30 = dLine30 / m_nDay30;
			dLine30 = dLine30 - pLine30[(nIndex+1)%m_nDay30];
		}

		dLine60 += pItem->m_dClose;
		pLine60[nIndex%m_nDay60] = pItem->m_dClose;
		if (nIndex < m_nDay60 - 1)
		{
			pItem->m_pDayLine->m_dLine60 = -1;
		}
		else
		{
			pItem->m_pDayLine->m_dLine60 = dLine60 / m_nDay60;
			dLine60 = dLine60 - pLine60[(nIndex+1)%m_nDay60];
		}

		dLine120 += pItem->m_dClose;
		pLine120[nIndex%m_nDay120] = pItem->m_dClose;
		if (nIndex < m_nDay120 - 1)
		{
			pItem->m_pDayLine->m_dLine120 = -1;
		}
		else
		{
			pItem->m_pDayLine->m_dLine120 = dLine120 / m_nDay120;
			dLine120 = dLine120 - pLine120[(nIndex+1)%m_nDay120];
		}

		// circulate MACD data
		if (nIndex == 0)
		{
			pItem->m_pMACD->m_dEMA12 = 0;
			pItem->m_pMACD->m_dEMA26 = 0;
			pItem->m_pMACD->m_dDIFF = 0;
			pItem->m_pMACD->m_dDEA = 0;
			pItem->m_pMACD->m_dBAR = 0;
		}
		else
		{
			pItem->m_pMACD->m_dEMA12 = (2 * pItem->m_dClose + 11 * dPrevEMA12) / 13;
			pItem->m_pMACD->m_dEMA26 = (2 * pItem->m_dClose + 25 * dPrevEMA26) / 27;;
			pItem->m_pMACD->m_dDIFF = pItem->m_pMACD->m_dEMA12 - pItem->m_pMACD->m_dEMA26;
			pItem->m_pMACD->m_dDEA = dPrevDEA * 8 / 10 + pItem->m_pMACD->m_dDIFF * 2 / 10;
			pItem->m_pMACD->m_dBAR = 2 * (pItem->m_pMACD->m_dDIFF - pItem->m_pMACD->m_dDEA);
		}
		dPrevEMA12 = pItem->m_pMACD->m_dEMA12;
		dPrevEMA26 = pItem->m_pMACD->m_dEMA26;
		dPrevDEA = pItem->m_pMACD->m_dDEA;

		nIndex++;
	}

	delete []pLine5;
	delete []pLine10;
	delete []pLine20;
	delete []pLine30;
	delete []pLine60;
	delete []pLine120;

	return QC_ERR_NONE;
}

int CStockKXTInfo::UpdateTodayLineMACD (void)
{
	int		nIndex = 0;
	double	dLine5 = 0;
	double	dLine10 = 0;
	double	dLine20 = 0;
	double	dLine30 = 0;
	double	dLine60 = 0;
	double	dLine120 = 0;

	int		nMaxDays = 0;
	if (nMaxDays < m_nDay5)
		nMaxDays = m_nDay5;
	if (nMaxDays < m_nDay10)
		nMaxDays = m_nDay10;
	if (nMaxDays < m_nDay20)
		nMaxDays = m_nDay20;
	if (nMaxDays<  m_nDay30)
		nMaxDays = m_nDay30;
	if (nMaxDays < m_nDay60)
		nMaxDays = m_nDay60;
	if (nMaxDays < m_nDay120)
		nMaxDays = m_nDay120;

	qcStockKXTInfoItem *	pItem = NULL;
	NODEPOS					pos = m_lstItem.GetTailPosition ();
	while (pos != NULL)
	{
		pItem = m_lstItem.GetPrev (pos);
		if (nIndex < m_nDay5)
			dLine5 += pItem->m_dClose;
		if (nIndex < m_nDay10)
			dLine10 += pItem->m_dClose;
		if (nIndex < m_nDay20)
			dLine20 += pItem->m_dClose;
		if (nIndex < m_nDay30)
			dLine30 += pItem->m_dClose;
		if (nIndex < m_nDay60)
			dLine60 += pItem->m_dClose;
		if (nIndex < m_nDay120)
			dLine120 += pItem->m_dClose;

		nIndex++;
		if (nIndex >= nMaxDays)
			break;
	}

	pItem = m_lstItem.GetTail ();
	if (pItem->m_pDayLine == NULL)
		pItem->m_pDayLine = new qcDayLine ();
	pItem->m_pDayLine->m_dLine5 = dLine5 / m_nDay5;
	pItem->m_pDayLine->m_dLine10 = dLine10 / m_nDay10;
	pItem->m_pDayLine->m_dLine20 = dLine20 / m_nDay20;
	pItem->m_pDayLine->m_dLine30 = dLine30 / m_nDay30;
	pItem->m_pDayLine->m_dLine60 = dLine60 / m_nDay60;
	pItem->m_pDayLine->m_dLine120 = dLine120 / m_nDay120;

	pos = m_lstItem.GetTailPosition ();
	if (pos != NULL)
		m_lstItem.GetPrev (pos);
	if (pos != NULL)
	{
		pItem = m_lstItem.GetPrev (pos);
		qcMACD * pPrevMACD = pItem->m_pMACD;
		pItem = m_lstItem.GetTail ();

		if (pItem->m_pMACD == NULL)
			pItem->m_pMACD = new qcMACD ();
		pItem->m_pMACD->m_dEMA12 = (2 * pItem->m_dClose + 11 * pPrevMACD->m_dEMA12) / 13;
		pItem->m_pMACD->m_dEMA26 = (2 * pItem->m_dClose + 25 * pPrevMACD->m_dEMA26) / 27;;
		pItem->m_pMACD->m_dDIFF = pItem->m_pMACD->m_dEMA12 - pItem->m_pMACD->m_dEMA26;
		pItem->m_pMACD->m_dDEA = pPrevMACD->m_dDEA * 8 / 10 + pItem->m_pMACD->m_dDIFF * 2 / 10;
		pItem->m_pMACD->m_dBAR = 2 * (pItem->m_pMACD->m_dDIFF - pItem->m_pMACD->m_dDEA);
	}

	return QC_ERR_NONE;
}

int CStockKXTInfo::FillPath (void)
{
#ifdef DATA_FROM_NETWORK
	char szLine[1024];
	strcpy (m_szPath, m_szHost);
	sprintf (szLine, "realtime-k?beginDay=%s&code=%s&time=day&type=bfq",m_szStartDate, m_szCode);	
	strcat (m_szPath, szLine);
#else
	strcpy (m_szPath, "c:\\work\\Temp\\RealTimeK.txt");
#endif // DATA_FROM_NETWORK
	return QC_ERR_NONE;
}

void CStockKXTInfo::ReleaseData (void)
{
	qcStockKXTInfoItem * pItem = m_lstItem.RemoveHead ();
	while (pItem != NULL)
	{
		QC_DEL_P (pItem->m_pDayLine);
		QC_DEL_P (pItem->m_pMACD);
		delete pItem;
		pItem = m_lstItem.RemoveHead ();
	}
}

void CStockKXTInfo::DumpToFile (void)
{
	qcGetAppPath (NULL, m_szPath, sizeof (m_szPath));
	if (m_szCode[0] == '6' || m_szCode[0] == '9')
		sprintf (m_szPath, "%sdata\\history\\sh%s.txt", m_szPath, m_szCode);
	else if (m_szCode[0] == '3' || m_szCode[0] == '0' || m_szCode[0] == '2')
		sprintf (m_szPath, "%sdata\\history\\sz%s.txt", m_szPath, m_szCode);
	else 
		sprintf (m_szPath, "%sdata\\history\\%s.txt", m_szPath, m_szCode);

	CFileIO filIO;
	if (filIO.Open (m_szPath, 0, QCIO_FLAG_WRITE) != QC_ERR_NONE)
		return;
	char					szLine[1024];
	qcStockKXTInfoItem *	pItem = NULL;
	NODEPOS					pos = m_lstItem.GetHeadPosition ();
	while (pos != NULL)
	{
		pItem = m_lstItem.GetNext (pos);
		sprintf (szLine, "%d-%02d-%02d,%.2f,%.2f,%.2f,%.2f,%d,%d,%.2f,%.2f,%.2f,%.2f\r\n",
						 pItem->m_nYear, pItem->m_nMonth, pItem->m_nDay, pItem->m_dOpen, pItem->m_dClose, pItem->m_dMax, pItem->m_dMin,
						 pItem->m_nVolume, pItem->m_nMoney, pItem->m_dDiffNum, pItem->m_dDiffRate, pItem->m_dSwing, pItem->m_dExchange);
		filIO.Write ((unsigned char *)szLine, strlen (szLine));
	}
	filIO.Close ();
}