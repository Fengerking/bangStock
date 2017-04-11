/*******************************************************************************
	File:		CStockFileHYGN.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockFileHYGN.h"

#include "CFileIO.h"
#include "USystemFunc.h"
#include "ULogFunc.h"

CStockFileHYGN::CStockFileHYGN(void)
	: CStockFileBase  ()
	, m_pHttpUtil (NULL)
{
	SetObjectName ("CStockFileHYGN");
	strcpy (m_szHangYe, "");
	strcpy (m_szGaiNian, "");
}

CStockFileHYGN::~CStockFileHYGN(void)
{
	QC_DEL_P (m_pHttpUtil);
}

int CStockFileHYGN::Open (char * pCode, bool bUpdate)
{
	if (pCode == NULL)
		return QC_ERR_ARG;

	strcpy (m_szCode, pCode);
	if (!bUpdate)
	{
		qcGetAppPath (NULL, m_szFile, sizeof (m_szFile));
		sprintf (m_szFile, "%sdata\\hygn\\%s.txt", m_szFile, pCode);
		OpenFile (m_szFile);
	}
	if (m_nFileSize == 0)
		return Parser (bUpdate);

	char *	pBuff = m_pFileData;
	int nLine = ReadTextLine (pBuff, m_nFileSize, m_szHangYe, sizeof (m_szHangYe));
	pBuff += nLine;

	nLine = ReadTextLine (pBuff, m_nFileSize, m_szGaiNian, sizeof (m_szGaiNian));
	pBuff += nLine;

	return QC_ERR_NONE;
}

int CStockFileHYGN::Parser (bool bDownLoad)
{
	if (bDownLoad)
	{
		if (DownLoad () != QC_ERR_NONE)
			return QC_ERR_FAILED;
	}

	char szHtmFile[256];
	qcGetAppPath (NULL, szHtmFile, sizeof (szHtmFile));
	sprintf (szHtmFile, "%sdata\\hygn\\sourcesina\\%s.htm", szHtmFile, m_szCode);
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
	char		szHangYe[256];
	char		szGaiNian[256];
	memset (szHangYe, 0, sizeof (szHangYe));
	memset (szGaiNian, 0, sizeof (szGaiNian));
	while (pBuff - pFileData < nFileSize)
	{
		nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
		pBuff += nLine;
		if (strstr (szLine, "同行业个股") != NULL)
		{
			while (pBuff - pFileData < nFileSize)
			{
				nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
				pBuff += nLine;
				if (strstr (szLine, "center") != NULL)
				{
					pTxt = strstr (szLine, ">");
					pTxt += 1;
					pPos = strstr (pTxt, "<");
					if (pPos == NULL)
						break;
					if (pPos == pTxt)
						strcpy (szHangYe, "    ");
					else
					{
						*pPos = 0;
						strcpy (szHangYe, pTxt);
					}
					break;
				}
			}
			if (strlen (szHangYe) > 0)
				break;
		}
	}

	while (pBuff - pFileData < nFileSize)
	{
		nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
		pBuff += nLine;
		if (strstr (szLine, "同概念个股") != NULL)
		{
			while (pBuff - pFileData < nFileSize)
			{
				nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
				pBuff += nLine;
				if (strstr (szLine, "</table>") != NULL)
					break;
				if (strstr (szLine, "center") != NULL)
				{
					pTxt = strstr (szLine, ">");
					pTxt += 1;
					pPos = strstr (pTxt, "<");
					if (pPos == NULL)
						break;
					if (pPos == pTxt + 1)
						strcat (szGaiNian, "    ");
					else
					{
						*pPos = 0;
						strcat (szGaiNian, pTxt);
						strcat (szGaiNian, " ");
					}
					nLine = ReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
					pBuff += nLine;
				}
			}
			if (strlen (szHangYe) > 0)
				break;
		}
	}
	delete []pFileData;

	qcGetAppPath (NULL, szHtmFile, sizeof (szHtmFile));
	sprintf (szHtmFile, "%sdata\\hygn\\%s.txt", szHtmFile, m_szCode);
	if (filIO.Open (szHtmFile, 0, QCIO_FLAG_WRITE) != QC_ERR_NONE)
		return QC_ERR_FAILED;
	sprintf (m_szHangYe, "行业：%s\r\n", szHangYe);
	filIO.Write ((unsigned char*)m_szHangYe, strlen (m_szHangYe));
	sprintf (m_szGaiNian, "概念：%s\r\n", szGaiNian);
	filIO.Write ((unsigned char*)m_szGaiNian, strlen (m_szGaiNian));
	filIO.Close ();

	return QC_ERR_NONE;
}

int CStockFileHYGN::DownLoad (void)
{
	char szURL[1024];
	sprintf (szURL, "http://vip.stock.finance.sina.com.cn/corp/go.php/vCI_CorpOtherInfo/stockid/%s/menu_num/2.phtml", m_szCode);
	if (m_pHttpUtil == NULL)
		m_pHttpUtil = new CHttpUtil ();

	char * pData = NULL;
	int nRC = m_pHttpUtil->RequestData (szURL, &pData);
	if (nRC == QC_ERR_NONE)
	{
		char szFile[256];
		qcGetAppPath (NULL, szFile, sizeof (szFile));
		sprintf (szFile, "%sdata\\hygn\\sourcesina\\%s.htm", szFile, m_szCode);

		CFileIO filIO;
		filIO.Open (szFile, 0, QCIO_FLAG_WRITE);
		filIO.Write ((unsigned char *)pData, strlen (pData));
		return QC_ERR_NONE;
	}
	return QC_ERR_FAILED;
}
