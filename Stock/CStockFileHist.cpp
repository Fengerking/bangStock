/*******************************************************************************
	File:		CStockFileHist.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockFileHist.h"

#include "CFileIO.h"
#include "USystemFunc.h"

CStockFileHist::CStockFileHist(void)
	: CStockKXTInfo ()
{
	SetObjectName ("CStockFileHist");
}

CStockFileHist::~CStockFileHist(void)
{
}

int CStockFileHist::SetCode (char * pCode)
{
	if (pCode == NULL)
		return QC_ERR_ARG;
	ReleaseData ();

	strcpy (m_szCode, pCode);
	qcGetAppPath (NULL, m_szPath, sizeof (m_szPath));
	if (pCode[0] == '6' || pCode[0] == '9')
		sprintf (m_szPath, "%sdata\\history\\sh%s.txt", m_szPath, pCode);
	else if (pCode[0] == '3' || pCode[0] == '0' || pCode[0] == '2')
		sprintf (m_szPath, "%sdata\\history\\sz%s.txt", m_szPath, pCode);
	else 
		sprintf (m_szPath, "%sdata\\history\\%s.txt", m_szPath, pCode);

	CFileIO * pFile = new CFileIO ();
	if (pFile->Open (m_szPath, 0, QCIO_FLAG_READ) != QC_ERR_NONE)
	{
		delete pFile;
		return QC_ERR_FAILED;
	}
	int nSize = (int)pFile->GetSize ();
	char * pData = new char[nSize];
	pFile->Read ((unsigned char *)pData, nSize, true, 0);

	long long				tmFile;
	SYSTEMTIME				tmDate;
	memset (&tmDate, 0, sizeof (SYSTEMTIME));

	qcStockKXTInfoItem *	pItem = NULL;
	char *					pNum = NULL;
	char					szLine[256];
	int						nRest = nSize;
	char *					pBuff = pData;
	int						nLine = ReadDataLine (pBuff, nRest);
	pBuff +=  nLine;
	nRest -= nLine;
	while (nRest > 0)
	{
		nLine = ReadDataLine (pBuff, nRest);
		if (nLine <= 0)
			break;
		memset (szLine, 0, sizeof (szLine));
		strncpy (szLine, pBuff, nLine);
		pBuff +=  nLine;
		nRest -= nLine;
		
		if (szLine[nLine-1] == '\r' || szLine[nLine-1] == '\n')
			szLine[nLine-1] = 0;
		if (szLine[nLine-2] == '\r' || szLine[nLine-2] == '\n')
			szLine[nLine-2] = 0;

		sscanf (szLine, "%d-%d-%d", &tmDate.wYear, &tmDate.wMonth, &tmDate.wDay);
		SystemTimeToFileTime (&tmDate, (LPFILETIME)&tmFile);
		if (tmFile < m_llStartDate)
			continue;
		if (m_llEndDate > 0 && tmFile > m_llEndDate)
			break;

		pItem = new qcStockKXTInfoItem ();
		pItem->m_pDayLine = NULL;
		pItem->m_pMACD = NULL;
		m_lstItem.AddTail (pItem);
		pItem->m_nYear = tmDate.wYear; pItem->m_nMonth = tmDate.wMonth; pItem->m_nDay = tmDate.wDay; 
		pNum = szLine;
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_dOpen = atof (pNum);	
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_dClose = atof (pNum);		
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_dMax = atof (pNum);		
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_dMin = atof (pNum);		
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_nVolume = atoi (pNum);		
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_nMoney = atoi (pNum);		
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_dDiffNum = atof (pNum);		
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_dDiffRate = atof (pNum);		
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_dSwing = atof (pNum);
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_dExchange = atof (pNum);	

		//日期,开盘,收盘,最高,最低,交易量(手),交易金额(元),涨跌额,涨跌幅(%),振幅(%),换手率(%)
		//2012-05-11,35.00,34.88,36.00,33.33,158937,548030000,1.88,5.70,8.09,59.42
	}

	delete []pData;
	delete pFile;

	UpdateWithFHSP ();
	
	UpdateMaxMinPrice ();
	CreateDayLineMACD ();

	return QC_ERR_NONE;
}

int CStockFileHist::SetStartEndDate (long long llStartDate, long long llEndDate)
{
	m_llStartDate = llStartDate;
	m_llEndDate = llEndDate;

//	SetCode (m_szCode);

	return QC_ERR_NONE;
}