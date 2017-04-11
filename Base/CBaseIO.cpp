/*******************************************************************************
	File:		CBaseIO.cpp

	Contains:	base io implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CBaseIO.h"

CBaseIO::CBaseIO(void)
	: CBaseObject ()
	, m_sStatus (QCIO_Init)
	, m_llFileSize (0)
	, m_llReadPos (-1)
	, m_llDownPos (-1)
{
	SetObjectName ("CBaseIO");
}

CBaseIO::~CBaseIO(void)
{
	Close ();
}

int CBaseIO::Open (const char * pURL, long long llOffset, int nFlag)
{
	return QC_ERR_IMPLEMENT;
}

int CBaseIO::Reconnect (const char * pNewURL, long long llOffset)
{
	return QC_ERR_IMPLEMENT;
}

int CBaseIO::Close (void)
{
	m_llFileSize = 0;
	m_llReadPos = -1;
	m_llDownPos = -1;
	return QC_ERR_IMPLEMENT;
}

int CBaseIO::Run (void)
{
	m_sStatus = QCIO_Run;
	return QC_ERR_NONE;
}

int CBaseIO::Pause (void)
{
	m_sStatus = QCIO_Pause;
	return QC_ERR_NONE;
}

int CBaseIO::Stop (void)
{
	m_sStatus = QCIO_Stop;
	return QC_ERR_NONE;
}

long long CBaseIO::GetSize (void)
{
	return m_llFileSize;
}

int CBaseIO::Read (unsigned char * pBuff, int & nSize, bool bFull, int nFlag)
{
	return QC_ERR_IMPLEMENT;
}

int	CBaseIO::ReadAt (long long llPos, unsigned char * pBuff, int & nSize, bool bFull, int nFlag)
{
	return QC_ERR_IMPLEMENT;
}

int	CBaseIO::ReadSync (long long llPos, unsigned char * pBuff, int nSize, int nFlag)
{
	int nRead = nSize;
	int nRC = ReadAt (llPos, pBuff, nRead, true, nFlag);
	if (nRC == QC_ERR_NONE)
		return nRead;
	else
		return 0;
}

int	CBaseIO::Write (unsigned char * pBuff, int & nSize)
{
	return QC_ERR_IMPLEMENT;
}

long long CBaseIO::SetPos (long long llPos, int nFlag)
{
	return -1;
}

long long CBaseIO::GetDownPos (void)
{
	return m_llDownPos;
}

long long CBaseIO::GetReadPos (void)
{
	return m_llReadPos;
}

int CBaseIO::GetSpeed (int nLastSecs)
{
	return 1024 * 1024 * 8;
}

QCIOType CBaseIO::GetType (void)
{
	return QC_IOTYPE_NONE;
}

int CBaseIO::GetParam (int nID, void * pParam)
{
	return QC_ERR_IMPLEMENT;
}

int CBaseIO::SetParam (int nID, void * pParam)
{
	return QC_ERR_IMPLEMENT;
}

