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


	return QC_ERR_NONE;
}

int CStockFileHYGN::Parser (bool bDownLoad)
{
	if (bDownLoad)
	{
		if (DownLoad () != QC_ERR_NONE)
			return QC_ERR_FAILED;
	}

	return QC_ERR_NONE;
}

int CStockFileHYGN::DownLoad (void)
{
	char szURL[1024];
	strcpy (szURL, "http://vip.stock.finance.sina.com.cn/mkt/#hangye_ZC4");
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
