/*******************************************************************************
	File:		CStockFileFinance.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockFileFinance.h"

#include "CFileIO.h"
#include "USystemFunc.h"
#include "ULogFunc.h"

CStockFileFinance::CStockFileFinance(void)
	: CStockFileBase  ()
	, m_pHttpUtil (NULL)
{
	SetObjectName ("CStockFileFinance");
}

CStockFileFinance::~CStockFileFinance(void)
{
	ReleaseItem ();
	QC_DEL_P (m_pHttpUtil);
}

int CStockFileFinance::Open (char * pCode, bool bUpdate)
{
	if (pCode == NULL)
		return QC_ERR_ARG;
	ReleaseItem ();

	strcpy (m_szCode, pCode);
	if (!bUpdate)
	{
		qcGetAppPath (NULL, m_szFile, sizeof (m_szFile));
		sprintf (m_szFile, "%sdata\\finance\\%s.txt", m_szFile, pCode);
		OpenFile (m_szFile);
	}
	if (m_nFileSize == 0)
		return Parser (bUpdate);

	char *		pBuff = m_pFileData;
	char		szLine[2048];
	char *		pPos = NULL;
	char *		pTxt = NULL;
	int			nLine = 0;
	qcStockFinanceInfoItem  * pItem = NULL;
	while (pBuff - m_pFileData < m_nFileSize)
	{
		nLine = ReadDataLine (pBuff, m_nFileSize - (pBuff - m_pFileData));
		if (nLine <= 0)
			break;
		memset (szLine, 0, sizeof (szLine));
		strncpy (szLine, pBuff, nLine);
		pBuff += nLine;

		pItem = new qcStockFinanceInfoItem ();
		memset (pItem, 0, sizeof (qcStockFHSPInfoItem));
		m_lstItem.AddTail (pItem);
		sscanf (szLine, "%d-%02d-%02d,", &pItem->m_nYear, &pItem->m_nMonth, &pItem->m_nDay);
		pTxt = strstr (szLine, ",") + 1;
		pPos = strstr (pTxt + 1, ",");
		*pPos = 0;
		pItem->m_dValue = atof (pTxt);

		pTxt = pPos + 1;
		pPos = strstr (pTxt + 1, ",");
		*pPos = 0;
		pItem->m_dIncome = atof (pTxt);

		pTxt = pPos + 1;
		pPos = strstr (pTxt + 1, ",");
		*pPos = 0;
		pItem->m_dCash = atof (pTxt);

		pTxt = pPos + 1;
		pItem->m_dFund = atof (pTxt);
	}

	return QC_ERR_NONE;
}

int CStockFileFinance::Parser (bool bDownLoad)
{
	if (bDownLoad)
	{
		if (DownLoad () != QC_ERR_NONE)
			return QC_ERR_FAILED;
	}

	char szHtmFile[256];
	qcGetAppPath (NULL, szHtmFile, sizeof (szHtmFile));
	sprintf (szHtmFile, "%sdata\\finance\\sourcesina\\%s.htm", szHtmFile, m_szCode);
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
	while (pBuff - pFileData < nFileSize)
	{
		nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
		pBuff += nLine;
		if (strstr (szLine, "项目：财务摘要") != NULL)
			break;
	}

	qcStockFinanceInfoItem * pItem = NULL;
	while (pBuff - pFileData < nFileSize)
	{
		while (pBuff - pFileData < nFileSize)
		{
			nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
			pBuff += nLine;
			if (strstr (szLine, "截止日期") != NULL)
			{
				pTxt = strstr (szLine, "name=") + 6;

				pItem = new qcStockFinanceInfoItem ();
				memset (pItem, 0, sizeof (qcStockFinanceInfoItem));
				m_lstItem.AddTail (pItem);

				sscanf (pTxt, "%d-%02d-%02d,", &pItem->m_nYear, &pItem->m_nMonth, &pItem->m_nDay);
				break;
			}
		}

		while (pBuff - pFileData < nFileSize)
		{
			nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
			pBuff += nLine;
			if (strstr (szLine, "每股净资产") != NULL)
			{
				nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
				pBuff += nLine;
				pTxt = strstr (szLine, ">");
				while (*(pTxt + 1) == '<')
					pTxt = strstr (pTxt + 1, ">");
				pTxt += 1;
				pPos = strstr (pTxt, "元");
				if (pPos == NULL)
					break;
				*pPos = 0;
				pItem->m_dValue = atof (pTxt);
				break;
			}
		}

		while (pBuff - pFileData < nFileSize)
		{
			nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
			pBuff += nLine;
			if (strstr (szLine, "每股收益") != NULL)
			{
				nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
				pBuff += nLine;
				pTxt = strstr (szLine, ">");
				while (*(pTxt + 1) == '<')
					pTxt = strstr (pTxt + 1, ">");
				pTxt += 1;
				pPos = strstr (pTxt, "元");
				if (pPos == NULL)
					break;
				*pPos = 0;
				pItem->m_dIncome = atof (pTxt);
				break;
			}
		}

		while (pBuff - pFileData < nFileSize)
		{
			nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
			pBuff += nLine;
			if (strstr (szLine, "每股现金含量") != NULL)
			{
				nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
				pBuff += nLine;
				pTxt = strstr (szLine, ">");
				while (*(pTxt + 1) == '<')
					pTxt = strstr (pTxt + 1, ">");
				pTxt += 1;
				pPos = strstr (pTxt, "元");
				if (pPos == NULL)
					break;
				*pPos = 0;
				pItem->m_dCash = atof (pTxt);
				break;
			}
		}

		while (pBuff - pFileData < nFileSize)
		{
			nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
			pBuff += nLine;
			if (strstr (szLine, "每股资本公积金") != NULL)
			{
				nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
				pBuff += nLine;
				pTxt = strstr (szLine, ">");
				while (*(pTxt + 1) == '<')
					pTxt = strstr (pTxt + 1, ">");
				pTxt += 1;
				pPos = strstr (pTxt, "元");
				if (pPos == NULL)
					break;
				*pPos = 0;
				pItem->m_dFund = atof (pTxt);
				break;
			}
		}
	}
	delete []pFileData;

	qcGetAppPath (NULL, szHtmFile, sizeof (szHtmFile));
	sprintf (szHtmFile, "%sdata\\finance\\%s.txt", szHtmFile, m_szCode);
	if (filIO.Open (szHtmFile, 0, QCIO_FLAG_WRITE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	NODEPOS pos = m_lstItem.GetHeadPosition ();
	while (pos != NULL)
	{
		pItem = m_lstItem.GetNext (pos);
		sprintf (szLine, "%d-%02d-%02d,%.2f,%.2f,%.2f,%.2f\r\n", pItem->m_nYear, pItem->m_nMonth, pItem->m_nDay, 
															pItem->m_dValue, pItem->m_dIncome, pItem->m_dCash, pItem->m_dFund); 
		filIO.Write ((unsigned char *)szLine, strlen (szLine));
	}
	filIO.Close ();

	return QC_ERR_NONE;
}

int CStockFileFinance::DownLoad (void)
{
	char szURL[1024];
	sprintf (szURL, "http://vip.stock.finance.sina.com.cn/corp/go.php/vFD_FinanceSummary/stockid/%s/displaytype/4.phtml", m_szCode);
	if (m_pHttpUtil == NULL)
		m_pHttpUtil = new CHttpUtil ();

	char * pData = NULL;
	int nRC = m_pHttpUtil->RequestData (szURL, &pData);
	if (nRC == QC_ERR_NONE)
	{
		char szFile[256];
		qcGetAppPath (NULL, szFile, sizeof (szFile));
		sprintf (szFile, "%sdata\\finance\\sourcesina\\%s.htm", szFile, m_szCode);

		CFileIO filIO;
		filIO.Open (szFile, 0, QCIO_FLAG_WRITE);
		filIO.Write ((unsigned char *)pData, strlen (pData));
		return QC_ERR_NONE;
	}
	return QC_ERR_FAILED;
}

void CStockFileFinance::ReleaseItem (void)
{
	qcStockFinanceInfoItem * pItem = m_lstItem.RemoveHead ();
	while (pItem != NULL)
	{
		delete pItem;
		pItem = m_lstItem.RemoveHead ();
	}
}