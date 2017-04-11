/*******************************************************************************
	File:		CFileIO.cpp

	Contains:	local file io implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-05		Bangfei			Create file

*******************************************************************************/

#if defined __QC_OS_NDK__ || defined __QC_OS_IOS__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "qcErr.h"

#include "ULogFunc.h"

#include "CFileIO.h"

CFileIO::CFileIO(void)
	: CBaseIO ()
	, m_hFile (NULL)
	, m_nFD (-1)	
{
	SetObjectName ("CFileIO");
}

CFileIO::~CFileIO(void)
{
	Close ();
}

int CFileIO::Open (const char * pURL, long long llOffset, int nFlag)
{
	char * pNewURL = (char *)pURL;
	if (!strncmp (pURL, "file://", 7))
		pNewURL = (char *)pURL + 7;
#ifdef __QC_OS_WIN32__
	if (nFlag == QCIO_FLAG_READ)
		m_hFile = CreateFile(pNewURL, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, (DWORD) 0, NULL);
	else if (nFlag == QCIO_FLAG_WRITE) 
		m_hFile = CreateFile(pNewURL, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, (DWORD) 0, NULL);
	else
		m_hFile = CreateFile(pNewURL, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, (DWORD) 0, NULL);

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		m_hFile = NULL;
		//QCLOGE ("Open file %s failed!", pNewURL);
		return -1;
	}
	if (nFlag & QCIO_FLAG_READ)
	{
		DWORD dwHigh = 0;
		DWORD dwSize = GetFileSize (m_hFile, &dwHigh);
		m_llFileSize = dwHigh;
		m_llFileSize = m_llFileSize << 32;
		m_llFileSize += dwSize;
	}
#elif defined __QC_OS_NDK__ || defined __QC_OS_IOS__
	int nOpenFlag = O_RDONLY;
	if (nFlag & QCIO_FLAG_READ)
		nOpenFlag = O_RDONLY;
	else if (nFlag & QCIO_FLAG_WRITE) 
		nOpenFlag = O_RDWR | O_CREAT;
	else
		nOpenFlag = O_RDWR | O_CREAT;

	int nMode = 0640;
#ifdef _OS_LINUX_X86
	m_nFD = open64 (pNewURL, nOpenFlag, nMode);
#elif defined __QC_OS_NDK__ || defined __QC_OS_IOS__
	m_nFD = ::open (pNewURL, nOpenFlag, nMode);
#endif // _OS_LINUX_X86

	if (m_nFD > 0 && (nFlag & QCIO_FLAG_READ))
	{
		struct stat st;
		memset(&st, 0, sizeof(struct stat));
	    fstat(m_nFD, &st); 
		m_llFileSize = st.st_size;
	}
	else
	{
		if (nFlag & QCIO_FLAG_READ)
			m_hFile = fopen (pNewURL, "rb");
		else if (nFlag & QCIO_FLAG_WRITE) 
			m_hFile = fopen (pNewURL, "wb");
		else
			m_hFile = fopen (pNewURL, "a+b");
			
		if (m_hFile != NULL && (nFlag & QCIO_FLAG_READ))
		{
			fseeko (m_hFile, 0LL, SEEK_END);
			m_llFileSize = ftello (m_hFile);
			fseeko (m_hFile, 0, SEEK_SET);		
		}
	}

	if (m_hFile == NULL && m_nFD <= 0)
	{
        QCLOGE ("Open file %s failed!", pNewURL);
		return -1;
	}
#endif // __QC_OS_WIN32__

	m_llReadPos = llOffset;
	
	return QC_ERR_NONE;
}

int CFileIO::Reconnect (const char * pNewURL, long long llOffset)
{
	m_llReadPos = llOffset;
	return QC_ERR_NONE;
}

int CFileIO::Close (void)
{
	if (m_nFD > 0)
	{
#if defined __QC_OS_NDK__ || defined __QC_OS_IOS__
		::close (m_nFD);
#endif // _OS_LINUX
		m_nFD = -1;
	}
	
	if (m_hFile != NULL)
#ifdef __QC_OS_WIN32__
		CloseHandle (m_hFile);
#elif defined __QC_OS_NDK__ || defined __QC_OS_IOS__
		fclose(m_hFile);
#endif // __QC_OS_WIN32__
	m_hFile = NULL;

	return QC_ERR_NONE;
}

int CFileIO::Read (unsigned char * pBuff, int & nSize, bool bFull, int nFlag)
{
	if (m_nFD <= 0 && m_hFile == NULL)
		return QC_ERR_FAILED;
	if (m_llReadPos >= m_llFileSize)
		return QC_ERR_FINISH;

	int nRead = -1;
#ifdef __QC_OS_WIN32__
	DWORD	dwRead = 0;
	ReadFile (m_hFile, pBuff, nSize, &dwRead, NULL);
	if (dwRead == 0)
	{
		if (m_llReadPos < m_llFileSize)
			return QC_ERR_STATUS;
		return QC_ERR_FAILED;
	}
	m_llReadPos += dwRead;
	nSize = (int)dwRead;
#elif defined __QC_OS_NDK__ || defined __QC_OS_IOS__
	if (m_nFD > 0)
		nRead = ::read (m_nFD, pBuff, nSize);
	else	
		nRead = fread (pBuff, 1, nSize, m_hFile);
	if(nRead == -1)
	{
		QCLOGE ("It was error when Read file!");
		if (m_llReadPos < m_llFileSize)
//			g_bFileError = true;
		return QC_ERR_FAILED;
	}
	m_llReadPos += nRead;	
	if (nRead < nSize)
	{
		if (m_hFile != NULL)
		{
			if (feof(m_hFile) == 0)
			{
				QCLOGE ("It can't Read data from file enough! Read: % 8d, Size: % 8d, pos: % 8d", nRead, nSize, (int)m_llReadPos);			
				return QC_ERR_FAILED;
			}
		}
	}
	nSize = nRead;
#endif // __QC_OS_WIN32__

	return QC_ERR_NONE;
}

int	CFileIO::ReadAt (long long llPos, unsigned char * pBuff, int & nSize, bool bFull, int nFlag)
{
	if (llPos != m_llReadPos)
		SetPos (llPos, QCIO_SEEK_BEGIN);
	return Read (pBuff, nSize, bFull, nFlag);
}

int	CFileIO::Write (unsigned char * pBuff, int nSize)
{
	unsigned int uWrite = 0;

#ifdef __QC_OS_WIN32__
	WriteFile (m_hFile, pBuff, nSize, (DWORD *)&uWrite, NULL);
#elif defined __QC_OS_NDK__ || defined __QC_OS_IOS__
	if (m_nFD > 0)
		uWrite = ::write (m_nFD, pBuff, nSize);
	else
		uWrite = fwrite(pBuff,1, nSize, m_hFile);
#endif //__QC_OS_WIN32__

	return uWrite;
}

long long CFileIO::SetPos (long long llPos, int nFlag)
{
	if (m_nFD <= 0 && m_hFile == NULL)
		return -1;

#ifdef __QC_OS_WIN32__
	long		lPos = (long)llPos;
	long		lHigh = (long)(llPos >> 32);
	
	int sMove = FILE_BEGIN;
	if (nFlag == QCIO_SEEK_BEGIN)
	{
		sMove = FILE_BEGIN;
		m_llReadPos = llPos;

		lPos = (long)llPos;
		lHigh = (long)(llPos >> 32);
	}
	else if (nFlag == QCIO_SEEK_CUR)
	{
		sMove = FILE_CURRENT;
		m_llReadPos = m_llReadPos + llPos;
	}
	else if (nFlag == QCIO_SEEK_END)
	{
		sMove = FILE_END;
		m_llReadPos = m_llFileSize - llPos;
		lPos = (long)llPos;
		lHigh = (long)(llPos >> 32);
	}

	if (m_llReadPos > m_llFileSize)
		return QC_ERR_STATUS;

	DWORD dwRC = SetFilePointer (m_hFile, lPos, &lHigh, sMove);
	//modefied by Aiven,return the currect file pointer if finish the seek.
	if(dwRC == INVALID_SET_FILE_POINTER)
		return QC_ERR_FAILED;
	return llPos;

#elif defined __QC_OS_NDK__ || defined __QC_OS_IOS__
	if (nFlag == QCIO_SEEK_BEGIN)
		m_llReadPos = llPos;
	else if (nFlag == QCIO_SEEK_CUR)
		m_llReadPos = m_llReadPos + llPos;
	else if (nFlag == QCIO_SEEK_END)
		m_llReadPos = m_llFileSize - llPos;
	if (m_llReadPos > m_llFileSize)
		return QC_ERR_STATUS;
	
	nFlag = SEEK_SET;
	if (m_nFD > 0)
	{
#ifdef __QC_OS_IOS__
		if((llPos = lseek (m_nFD, m_llReadPos, nFlag)) < 0)
			return QC_ERR_FAILED;
#else
        if((llPos = lseek64 (m_nFD, m_llReadPos, nFlag)) < 0)
            return QC_ERR_FAILED;
#endif // __QC_OS_NDK__
	}
	else
	{	
		if (fseeko (m_hFile, m_llReadPos, nFlag) < 0)
		{
			QCLOGE("fseeko to  : %lld failed", (long long) llPos);
			return -1;
		}			
		llPos = ftello (m_hFile);
		if (llPos < 0)
		{
			QCLOGE("ftello the position failed");
			return -1;
		}	
	}
	return llPos;
#endif // __QC_OS_WIN32__
}

QCIOType CFileIO::GetType (void)
{
	return QC_IOTYPE_FILE;
}

int CFileIO::GetParam (int nID, void * pParam)
{
	return QC_ERR_IMPLEMENT;
}

int CFileIO::SetParam (int nID, void * pParam)
{
	return QC_ERR_IMPLEMENT;
}

