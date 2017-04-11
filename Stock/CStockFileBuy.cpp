/*******************************************************************************
	File:		CStockFileBuy.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockFileBuy.h"

#include "CFileIO.h"
#include "USystemFunc.h"

CStockFileBuy::CStockFileBuy(void)
	: CStockFileCode  ()
{
	SetObjectName ("CStockFileBuy");
}

CStockFileBuy::~CStockFileBuy(void)
{
	ReleaseCode ();
}

int CStockFileBuy::Open (char * pFile)
{
	ReleaseCode ();

	qcGetAppPath (NULL, m_szFile, sizeof (m_szFile));
	if (pFile == NULL)
		sprintf (m_szFile, "%sdata\\%s", m_szFile, "codeList.txt");
	else
	{
		if (strlen (pFile) > 16)
			strcpy (m_szFile, pFile);
		else
			sprintf (m_szFile, "%sdata\\%s", m_szFile, pFile);
	}
	if (OpenFile (m_szFile) != QC_ERR_NONE)
		return QC_ERR_FAILED;

	bsBuyInfo *	pItem = NULL;
	int			nRest = m_nFileSize;
	char *		pBuff = m_pFileData;
	char		szLine[256];
	char *		pPos = NULL;
	char *		pTxt = NULL;
	int			nLine = ReadDataLine (pBuff, nRest);
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

		pItem = new bsBuyInfo ();
		memset (pItem, 0, sizeof (bsBuyInfo));
		m_lstBuyInfo.AddTail (pItem);

		pTxt = szLine;
		pPos = szLine;
		while (*pPos != ',') {pPos++;if (pPos-szLine > nLine) return QC_ERR_FAILED;}*pPos = 0;
		strcpy (pItem->m_szCode, pTxt);
		pTxt = pPos + 1;
		while (*pPos != ',') {pPos++;if (pPos-szLine > nLine) return QC_ERR_FAILED;}*pPos = 0;
		sscanf (pTxt, "%d-%d-%d", &pItem->m_nBuyYear, &pItem->m_nBuyMonth, &pItem->m_nBuyDay);
		pTxt = pPos + 1;
		while (*pPos != ',') {pPos++;if (pPos-szLine > nLine) return QC_ERR_FAILED;}*pPos = 0;
		pItem->m_dBuyPrice = atof (pTxt);
		pTxt = pPos + 1;
		while (*pPos != ',') 
		{
			pPos++;
			if (*pPos == 0)
			{
				pItem->m_nBuyNum = atoi (pTxt);
				break;
			}
			if (pPos-szLine > nLine)
				return QC_ERR_FAILED;
		}
		if (*pPos == 0)
			continue;
		*pPos = 0;
		pItem->m_nBuyNum = atoi (pTxt);

		pTxt = pPos + 1;
		if (*pTxt == 0)
			continue;
		while (*pPos != ',') {pPos++;if (pPos-szLine > nLine) return QC_ERR_FAILED;}*pPos = 0;
		sscanf (pTxt, "%d-%d-%d", &pItem->m_nSellYear, &pItem->m_nSellMonth, &pItem->m_nSellDay);
		pTxt = pPos + 1;
		while (*pPos != ',') {pPos++;if (pPos-szLine > nLine) return QC_ERR_FAILED;}*pPos = 0;
		pItem->m_dSellPrice = atof (pTxt);
		pTxt = pPos + 1;
		while (*pPos != ',') 
		{
			pPos++;
			if (*pPos == 0)
			{
				pItem->m_nSellNum = atoi (pTxt);
				break;
			}
			if (pPos-szLine > nLine)
				return QC_ERR_FAILED;
		}
		if (*pPos == 0)
			continue;
		*pPos = 0;
		pItem->m_nBuyNum = atoi (pTxt);
	}

	m_nCodeCount = m_lstBuyInfo.GetCount ();
	m_ppCodeNum = new char *[m_nCodeCount];
	m_nCurSel = 0;
	NODEPOS pos = m_lstBuyInfo.GetHeadPosition ();
	while (pos != NULL)
	{
		pItem = m_lstBuyInfo.GetNext (pos);
		m_ppCodeNum[m_nCurSel] = new char[8];
		strcpy (m_ppCodeNum[m_nCurSel], pItem->m_szCode);
		m_nCurSel++;
	}
	m_nCurSel = 0;
	return QC_ERR_NONE;
}


void CStockFileBuy::ReleaseCode (void)
{
	CStockFileCode::ReleaseCode ();

	bsBuyInfo * pBuyItem = m_lstBuyInfo.RemoveHead ();
	while (pBuyItem != NULL)
	{
		delete pBuyItem;
		pBuyItem = m_lstBuyInfo.RemoveHead ();
	}
}