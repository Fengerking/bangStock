/*******************************************************************************
	File:		CStockFileCompInfo.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockFileCompInfo.h"

#include "CFileIO.h"
#include "USystemFunc.h"
#include "ULogFunc.h"

CStockFileCompInfo::CStockFileCompInfo(void)
	: CStockFileBase  ()
	, m_pHttpUtil (NULL)
{
	SetObjectName ("CStockFileCompInfo");
}

CStockFileCompInfo::~CStockFileCompInfo(void)
{
	QC_DEL_P (m_pHttpUtil);
}

int CStockFileCompInfo::Open (char * pCode, bool bUpdate)
{
	if (pCode == NULL)
		return QC_ERR_ARG;

	InitParam ();
	strcpy (m_szCode, pCode);
	if (!bUpdate)
	{
		qcGetAppPath (NULL, m_szFile, sizeof (m_szFile));
		sprintf (m_szFile, "%sdata\\compinfo\\%s.txt", m_szFile, pCode);
		OpenFile (m_szFile);
	}
	if (m_nFileSize == 0)
		return Parser (bUpdate);



	return QC_ERR_NONE;
}

int CStockFileCompInfo::Parser (bool bDownLoad)
{
	if (bDownLoad)
	{
		if (DownLoad () != QC_ERR_NONE)
			return QC_ERR_FAILED;
	}

	char szHtmFile[256];
	qcGetAppPath (NULL, szHtmFile, sizeof (szHtmFile));
	sprintf (szHtmFile, "%sdata\\compinfo\\sourcesina\\%s.htm", szHtmFile, m_szCode);
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
	char		szLine[8192];
	char *		pPos = NULL;
	char *		pTxt = NULL;
	int			nLine = 0;
	while (pBuff - pFileData < nFileSize)
	{
		nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
		pBuff += nLine;
		if (strstr (szLine, "公司名称：") != NULL)
		{
			nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
			pBuff += nLine;
			pTxt = strstr (szLine, ">");
			pTxt += 1;
			pPos = strstr (pTxt, "<");
			if (pPos == NULL)
				break;
			strncpy (m_szName, pTxt, pPos - pTxt);
			break;
		}
	}

	while (pBuff - pFileData < nFileSize)
	{
		nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
		pBuff += nLine;
		if (strstr (szLine, "上市日期：") != NULL)
		{
			nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
			pBuff += nLine;
			pTxt = strstr (szLine, "_blank\">");
			pTxt += 8;
			pPos = strstr (pTxt, "<");
			if (pPos == NULL)
				break;
			strncpy (m_szStockDate, pTxt, pPos - pTxt);
			break;
		}
	}

	while (pBuff - pFileData < nFileSize)
	{
		nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
		pBuff += nLine;
		if (strstr (szLine, "发行价格：") != NULL)
		{
			nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
			pBuff += nLine;
			pTxt = strstr (szLine, ">");
			pTxt += 1;
			pPos = strstr (pTxt, "<");
			if (pPos == NULL)
				break;
			*pPos = 0;
			m_dFirstPrice = atof (pTxt);
			break;
		}
	}

	while (pBuff - pFileData < nFileSize)
	{
		nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
		pBuff += nLine;
		if (strstr (szLine, "成立日期：") != NULL)
		{
			nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
			pBuff += nLine;
			pTxt = strstr (szLine, "_blank\">");
			pTxt += 8;
			pPos = strstr (pTxt, "<");
			if (pPos == NULL)
				break;
			strncpy (m_szBuildDate, pTxt, pPos - pTxt);
			break;
		}
	}

	while (pBuff - pFileData < nFileSize)
	{
		nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
		pBuff += nLine;
		if (strstr (szLine, "办公地址：") != NULL)
		{
			nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
			pBuff += nLine;
			pTxt = strstr (szLine, ">");
			pTxt += 1;
			pPos = strstr (pTxt, "<");
			if (pPos == NULL)
				break;
			strncpy (m_szAddress, pTxt, pPos - pTxt);
			break;
		}
	}

	while (pBuff - pFileData < nFileSize)
	{
		nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
		pBuff += nLine;
		if (strstr (szLine, "公司简介：") != NULL)
		{
			nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
			pBuff += nLine;
			pTxt = strstr (szLine, "公司");
			if (pTxt == NULL)
			{
				pTxt = strstr (szLine, ">");
				pTxt += 1;
				pPos = strstr (pTxt, "&nbsp");
				while (pPos != NULL)
				{
					pTxt = pPos + 5;
					pPos = strstr (pTxt, "&nbsp");
				}
				pTxt += 1;
			}
			pPos = strstr (pTxt, "<");
			if (pPos == NULL)
				break;
			strncpy (m_szInfo, pTxt, pPos - pTxt);
			break;
		}
	}

	while (pBuff - pFileData < nFileSize)
	{
		nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
		pBuff += nLine;
		if (strstr (szLine, "经营范围：") != NULL)
		{
			nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
			pBuff += nLine;
			pTxt = strstr (szLine, ">");
			pTxt += 1;
			pPos = strstr (pTxt, "<");
			if (pPos == NULL)
				break;
			strncpy (m_szWork, pTxt, pPos - pTxt);
			break;
		}
	}
	delete []pFileData;

	qcGetAppPath (NULL, szHtmFile, sizeof (szHtmFile));
	sprintf (szHtmFile, "%sdata\\compinfo\\%s.txt", szHtmFile, m_szCode);
	if (filIO.Open (szHtmFile, 0, QCIO_FLAG_WRITE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	sprintf (szLine, "%s %s\r\n", "公司名称：", m_szName);
	filIO.Write ((unsigned char*)szLine, strlen (szLine));
	sprintf (szLine, "%s %s\r\n", "上市日期：", m_szStockDate);
	filIO.Write ((unsigned char*)szLine, strlen (szLine));
	sprintf (szLine, "%s %s\r\n", "创立日期：", m_szBuildDate);
	filIO.Write ((unsigned char*)szLine, strlen (szLine));
	sprintf (szLine, "%s %.2f\r\n", "发行价格：", m_dFirstPrice);
	filIO.Write ((unsigned char*)szLine, strlen (szLine));
	sprintf (szLine, "%s %s\r\n", "公司简介：", m_szInfo);
	filIO.Write ((unsigned char*)szLine, strlen (szLine));
	sprintf (szLine, "%s %s\r\n", "经营范围：", m_szWork);
	filIO.Write ((unsigned char*)szLine, strlen (szLine));

	filIO.Close ();

	return QC_ERR_NONE;
}

int CStockFileCompInfo::DownLoad (void)
{
	char szURL[1024];
	sprintf (szURL, "http://vip.stock.finance.sina.com.cn/corp/go.php/vCI_CorpInfo/stockid/%s.phtml", m_szCode);
	if (m_pHttpUtil == NULL)
		m_pHttpUtil = new CHttpUtil ();

	char * pData = NULL;
	int nRC = m_pHttpUtil->RequestData (szURL, &pData);
	if (nRC == QC_ERR_NONE)
	{
		char szFile[256];
		qcGetAppPath (NULL, szFile, sizeof (szFile));
		sprintf (szFile, "%sdata\\compinfo\\sourcesina\\%s.htm", szFile, m_szCode);

		CFileIO filIO;
		filIO.Open (szFile, 0, QCIO_FLAG_WRITE);
		filIO.Write ((unsigned char *)pData, strlen (pData));
		return QC_ERR_NONE;
	}
	return QC_ERR_FAILED;
}

void CStockFileCompInfo::InitParam (void)
{
	memset (m_szName, 0, sizeof (m_szName));
	memset (m_szStockDate, 0, sizeof (m_szStockDate));
	memset (m_szBuildDate, 0, sizeof (m_szBuildDate));
	memset (m_szAddress, 0, sizeof (m_szAddress));
	memset (m_szInfo, 0, sizeof (m_szInfo));
	memset (m_szWork, 0, sizeof (m_szWork));
	m_dFirstPrice = 0;;
}