/*******************************************************************************
	File:		CStockFileBase.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockFileBase.h"

#include "CFileIO.h"

CStockFileBase::CStockFileBase(void)
	: CBaseObject ()
	, m_pFileData (NULL)
	, m_pFilePos (NULL)
	, m_nFileSize (0)

{
	SetObjectName ("CStockFileBase");
	memset (m_szCode, 0, sizeof (m_szCode));
	memset (m_szFile, 0, sizeof (m_szFile));
}

CStockFileBase::~CStockFileBase(void)
{
	QC_DEL_A (m_pFileData);
}

int CStockFileBase::SetCode (char * pCode)
{
	if (pCode == NULL)
		return QC_ERR_ARG;

	strcpy (m_szCode, pCode);

	return QC_ERR_NONE;
}

int CStockFileBase::OpenFile (char * pFileName)
{
	if (pFileName == NULL)
		return QC_ERR_ARG;

	QC_DEL_A (m_pFileData);
	m_nFileSize = 0;

	strcpy (m_szFile, pFileName);
	CFileIO * pFile = new CFileIO ();
	if (pFile->Open (m_szFile, 0, QCIO_FLAG_READ) != QC_ERR_NONE)
	{
		delete pFile;
		return QC_ERR_FAILED;
	}
	m_nFileSize = (int)pFile->GetSize ();
	m_pFileData = new char[m_nFileSize];
	pFile->Read ((unsigned char *)m_pFileData, m_nFileSize, true, 0);
	delete pFile;
	m_pFilePos = m_pFileData;

	return QC_ERR_NONE;
}

int CStockFileBase::ReadDataLine (char * pData, int nSize)
{
	if (pData == NULL)
		return QC_ERR_ARG;

	char * pBuff = pData;
	while (pBuff - pData < nSize)
	{
		if (*pBuff == '\r' || *pBuff == '\n')
		{
			pBuff++;
			if (*(pBuff)== '\r' || *(pBuff) == '\n')
				pBuff++;
			break;
		}
		pBuff++;
	}
	return pBuff - pData;
}

int CStockFileBase::ReadTextLine (char * pData, int nSize, char * pLine, int nLine)
{
	if (pData == NULL)
		return 0;

	char * pBuff = pData;
	while (pBuff - pData < nSize)
	{
		if (*pBuff == '\r' || *pBuff == '\n')
		{
			pBuff++;
			if (*(pBuff)== '\r' || *(pBuff) == '\n')
				pBuff++;
			break;
		}
		pBuff++;
	}
	int nLineLen = pBuff - pData;
	if (nLine > nLineLen)
	{
		memset (pLine, 0, nLine);
		strncpy (pLine, pData, nLineLen);
	}
	return nLineLen;
}
