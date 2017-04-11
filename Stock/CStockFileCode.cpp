/*******************************************************************************
	File:		CStockFileCode.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockFileCode.h"

#include "CFileIO.h"
#include "USystemFunc.h"

CStockFileCode::CStockFileCode(void)
	: CStockFileBase  ()
	, m_ppCodeNum (NULL)
	, m_nCodeCount (0)
	, m_pURLFormat (NULL)
	, m_nCurSel (-1)
{
	SetObjectName ("CStockFileCode");
}

CStockFileCode::~CStockFileCode(void)
{
	ReleaseCode ();
}

int CStockFileCode::Open (char * pFile)
{
	ReleaseCode ();

	qcGetAppPath (NULL, m_szFile, sizeof (m_szFile));
	if (pFile == NULL)
		sprintf (m_szFile, "%sdata\\%s", m_szFile, "codeList.txt");
	else
		sprintf (m_szFile, "%sdata\\%s", m_szFile, pFile);
	if (OpenFile (m_szFile) != QC_ERR_NONE)
		return QC_ERR_FAILED;

	m_nCodeCount = m_nFileSize / 8;
	m_ppCodeNum = new char *[m_nCodeCount];
	for (int i = 0; i < m_nCodeCount; i++)
	{
		m_ppCodeNum[i] = new char[8];
		memset (m_ppCodeNum[i], 0, 8);
		strncpy (m_ppCodeNum[i], m_pFilePos, 6);
		m_pFilePos += 8;
		if (m_pFilePos - m_pFileData > m_nFileSize)
			break;
	}
	return QC_ERR_NONE;
}

int CStockFileCode::GetCodeCount (void)
{
	return m_nCodeCount;
}

char * CStockFileCode::GetCodeNum (int nIndex)
{
	if (m_ppCodeNum == NULL)
		return NULL;
	if (nIndex < 0 || nIndex >= m_nCodeCount)
		return NULL;
	return m_ppCodeNum[nIndex];
}

char * CStockFileCode::GetCodeName (int nIndex)
{
	return NULL;
}

char *	CStockFileCode::GetURLFormat (void)
{
	if (m_nCodeCount <= 0)
		return NULL;

	int nCount = m_nCodeCount;
	if (nCount >= 100)
		nCount = 100;
	if (m_pURLFormat == NULL)
	{
		m_pURLFormat = new char[nCount * 16];
		strcpy (m_pURLFormat, "");
		char *			pCode = NULL;
		char			szCode[32];
		for (int i = 0; i < nCount; i++)
		{
			pCode = GetCodeNum (i);
			if (pCode[0] == '6' || pCode[0] == '9')
				sprintf (szCode, "sh%s", pCode);
			else
				sprintf (szCode, "sz%s", pCode);
			if (i > 0)
				strcat (m_pURLFormat, "%2C");
			strcat (m_pURLFormat, szCode);
		}
	}
	return m_pURLFormat;
}

int	CStockFileCode::SetCurSel (int nSel)
{
	if (nSel < 0 || nSel >= m_nCodeCount)
		return QC_ERR_ARG;
	m_nCurSel = nSel;
	return QC_ERR_NONE;
}

int CStockFileCode::GetCurSel (void)
{
	return m_nCurSel;
}

char * CStockFileCode::GetSelCode (void)
{
	if (m_ppCodeNum == NULL)
		return NULL;

	if (m_nCurSel < 0 || m_nCurSel >= m_nCodeCount)
		return m_ppCodeNum[0];
	return m_ppCodeNum[m_nCurSel];
}

void CStockFileCode::ReleaseCode (void)
{
	if (m_ppCodeNum != NULL)
	{
		for (int i = 0; i < m_nCodeCount; i++)
			delete []m_ppCodeNum[i];
		delete []m_ppCodeNum;
	}
	m_ppCodeNum = NULL;
	m_nCodeCount = 0;

	QC_DEL_A (m_pURLFormat);
}