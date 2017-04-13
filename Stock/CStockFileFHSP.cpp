/*******************************************************************************
	File:		CStockFileFHSP.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockFileFHSP.h"

#include "CFileIO.h"
#include "USystemFunc.h"
#include "ULogFunc.h"

CStockFileFHSP::CStockFileFHSP(void)
	: CStockFileBase  ()
	, m_pHttpUtil (NULL)
{
	SetObjectName ("CStockFileFHSP");
}

CStockFileFHSP::~CStockFileFHSP(void)
{
	ReleaseItem ();
	QC_DEL_P (m_pHttpUtil);
}

int CStockFileFHSP::Open (char * pCode, bool bUpdate)
{
	if (pCode == NULL)
		return QC_ERR_ARG;
	ReleaseItem ();

	strcpy (m_szCode, pCode);
	if (!bUpdate)
	{
		qcGetAppPath (NULL, m_szFile, sizeof (m_szFile));
		sprintf (m_szFile, "%sdata\\fhsp\\%s.txt", m_szFile, pCode);
		OpenFile (m_szFile);
	}
	if (m_nFileSize == 0)
		return Parser (bUpdate);

	char *		pBuff = m_pFileData;
	char		szLine[2048];
	char *		pPos = NULL;
	char *		pTxt = NULL;
	int			nLine = 0;
	qcStockFHSPInfoItem  * pItem = NULL;
	while (pBuff - m_pFileData < m_nFileSize)
	{
		nLine = ReadDataLine (pBuff, m_nFileSize - (pBuff - m_pFileData));
		if (nLine <= 0)
			break;
		memset (szLine, 0, sizeof (szLine));
		strncpy (szLine, pBuff, nLine);
		pBuff += nLine;

		pItem = new qcStockFHSPInfoItem ();
		memset (pItem, 0, sizeof (qcStockFHSPInfoItem));
		m_lstItem.AddTail (pItem);
		sscanf (szLine, "%d-%02d-%02d,", &pItem->m_nYear, &pItem->m_nMonth, &pItem->m_nDay);
		pTxt = strstr (szLine, ",") + 1;
		pPos = strstr (pTxt + 1, ",");
		*pPos = 0;
		pItem->m_dGive = atof (pTxt);

		pTxt = pPos + 1;
		pPos = strstr (pTxt + 1, ",");
		*pPos = 0;
		pItem->m_dGain = atof (pTxt);

		pTxt = pPos + 1;
		pItem->m_dRation = atof (pTxt);
	}

	return QC_ERR_NONE;
}

int CStockFileFHSP::Parser (bool bDownLoad)
{
	if (bDownLoad)
	{
		if (DownLoad () != QC_ERR_NONE)
			return QC_ERR_FAILED;
	}

	char szHtmFile[256];
	qcGetAppPath (NULL, szHtmFile, sizeof (szHtmFile));
	sprintf (szHtmFile, "%sdata\\fhsp\\sourcesina\\%s.htm", szHtmFile, m_szCode);
	CFileIO filIO;
	if (filIO.Open (szHtmFile, 0, QCIO_FLAG_READ) != QC_ERR_NONE)
	{
		if (DownLoad () != QC_ERR_NONE)
			return QC_ERR_FAILED;
		if (filIO.Open (szHtmFile, 0, QCIO_FLAG_READ) != QC_ERR_NONE)
			return QC_ERR_FAILED;
	}

	int nFileSize = (int)filIO.GetSize ();
	char * pFileData = new char[nFileSize];
	filIO.Read ((unsigned char *)pFileData, nFileSize, true, 0);

	int			nRest = nFileSize;
	char *		pBuff = pFileData;
	char		szLine[2048];
	char *		pPos = NULL;
	char *		pTxt = NULL;
	int			nLine = 0;
	int			nRC = 0;
	qcStockFHSPInfoItem  * pItem = NULL;
	while (pBuff - pFileData < nFileSize)
	{
		nLine = ReadDataLine (pBuff, nRest);
		if (nLine <= 0)
			break;
		memset (szLine, 0, sizeof (szLine));
		strncpy (szLine, pBuff, nLine);
		pBuff += nLine;
		if (strstr (szLine, "<tbody>") != NULL)
			break;
	}

	while (pBuff - pFileData < nFileSize)
	{
		nLine = ReadDataLine (pBuff, nRest);
		if (nLine <= 0)
			break;
		memset (szLine, 0, sizeof (szLine));
		strncpy (szLine, pBuff, nLine);
		pBuff += nLine;
		if (strstr (szLine, "</tbody>") != NULL)
			break;
		if (strstr (szLine, "<tr>") == NULL)
			continue;

		nLine = ReadDataLine (pBuff, nRest);
		if (nLine <= 0)
			break;
		memset (szLine, 0, sizeof (szLine));
		strncpy (szLine, pBuff, nLine);
		pBuff += nLine;
		pTxt = strstr (szLine, "<td>");
		if (pTxt == NULL)
			break;
		pTxt += 4;
		pPos = strstr (szLine, "</td>");
		if (pPos == NULL)
			break;
		*pPos = 0;

		pItem = new qcStockFHSPInfoItem ();
		memset (pItem, 0, sizeof (qcStockFHSPInfoItem));
		m_lstItem.AddTail (pItem);
		sscanf (pTxt, "%d-%d-%d", &pItem->m_nYear, &pItem->m_nMonth, &pItem->m_nDay);

		nLine = ReadDataLine (pBuff, nRest);
		memset (szLine, 0, sizeof (szLine));
		strncpy (szLine, pBuff, nLine);
		pBuff += nLine;
		pTxt = strstr (szLine, "<td>");
		pTxt += 4;
		pPos = strstr (szLine, "</td>");
		*pPos = 0;
		pItem->m_dGive = atof (pTxt) / 10;

		nLine = ReadDataLine (pBuff, nRest);
		memset (szLine, 0, sizeof (szLine));
		strncpy (szLine, pBuff, nLine);
		pBuff += nLine;
		pTxt = strstr (szLine, "<td>");
		pTxt += 4;
		pPos = strstr (szLine, "</td>");
		*pPos = 0;
		pItem->m_dGain = atof (pTxt) / 10;

		nLine = ReadDataLine (pBuff, nRest);
		memset (szLine, 0, sizeof (szLine));
		strncpy (szLine, pBuff, nLine);
		pBuff += nLine;
		pTxt = strstr (szLine, "<td>");
		pTxt += 4;
		pPos = strstr (szLine, "</td>");
		*pPos = 0;
		pItem->m_dRation = atof (pTxt) / 10;

		strcpy (szLine, "");
		while (strstr (szLine, "</td>") == NULL)
		{
			nLine = ReadDataLine (pBuff, nRest);
			memset (szLine, 0, sizeof (szLine));
			strncpy (szLine, pBuff, nLine);
			pBuff += nLine;
		}
		nLine = ReadDataLine (pBuff, nRest);
		memset (szLine, 0, sizeof (szLine));
		strncpy (szLine, pBuff, nLine);
		pBuff += nLine;
		pTxt = strstr (szLine, "<td>");
		pTxt += 4;
		pPos = strstr (szLine, "</td>");
		*pPos = 0;
		nRC = sscanf (pTxt, "%d-%d-%d", &pItem->m_nYear, &pItem->m_nMonth, &pItem->m_nDay);
		if (nRC == 0)
		{
			pItem->m_dGive = 0;
			pItem->m_dGain = 0;
			pItem->m_dDividend = 0;
			pItem->m_dRation = 0;
		}

		while (strstr (szLine, "</tr>") == NULL)
		{
			nLine = ReadDataLine (pBuff, nRest);
			memset (szLine, 0, sizeof (szLine));
			strncpy (szLine, pBuff, nLine);
			pBuff += nLine;
		}

	}

	delete []pFileData;

	qcGetAppPath (NULL, szHtmFile, sizeof (szHtmFile));
	sprintf (szHtmFile, "%sdata\\fhsp\\%s.txt", szHtmFile, m_szCode);
	if (filIO.Open (szHtmFile, 0, QCIO_FLAG_WRITE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	NODEPOS pos = m_lstItem.GetHeadPosition ();
	while (pos != NULL)
	{
		pItem = m_lstItem.GetNext (pos);
		sprintf (szLine, "%d-%02d-%02d,%.2f,%.2f,%.2f\r\n", pItem->m_nYear, pItem->m_nMonth, pItem->m_nDay, 
							pItem->m_dGive, pItem->m_dGain, pItem->m_dRation); 
		filIO.Write ((unsigned char *)szLine, strlen (szLine));
	}
	filIO.Close ();

	return QC_ERR_NONE;
}

int CStockFileFHSP::DownLoad (void)
{
	char szURL[1024];
	sprintf (szURL, "http://vip.stock.finance.sina.com.cn/corp/go.php/vISSUE_ShareBonus/stockid/%s.phtml", m_szCode);
	if (m_pHttpUtil == NULL)
		m_pHttpUtil = new CHttpUtil ();

	char * pData = NULL;
	int nRC = m_pHttpUtil->RequestData (szURL, &pData);
	if (nRC == QC_ERR_NONE)
	{
		char szFile[256];
		qcGetAppPath (NULL, szFile, sizeof (szFile));
		sprintf (szFile, "%sdata\\fhsp\\sourcesina\\%s.htm", szFile, m_szCode);

		CFileIO filIO;
		filIO.Open (szFile, 0, QCIO_FLAG_WRITE);
		filIO.Write ((unsigned char *)pData, strlen (pData));
		return QC_ERR_NONE;
	}
	return QC_ERR_FAILED;
}

void CStockFileFHSP::ReleaseItem (void)
{
	qcStockFHSPInfoItem * pItem = m_lstItem.RemoveHead ();
	while (pItem != NULL)
	{
		delete pItem;
		pItem = m_lstItem.RemoveHead ();
	}
}