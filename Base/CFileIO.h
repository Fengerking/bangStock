/*******************************************************************************
	File:		CFileIO.h

	Contains:	local file io header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-05		Bangfei			Create file

*******************************************************************************/
#ifndef __CFileIO_H__
#define __CFileIO_H__

#include "CBaseIO.h"

class CFileIO : public CBaseIO
{
public:
    CFileIO(void);
    virtual ~CFileIO(void);

	virtual int 		Open (const char * pURL, long long llOffset, int nFlag);
	virtual int 		Reconnect (const char * pNewURL, long long llOffset);
	virtual int 		Close (void);

	virtual int			Read (unsigned char * pBuff, int & nSize, bool bFull, int nFlag);
	virtual int			ReadAt (long long llPos, unsigned char * pBuff, int & nSize, bool bFull, int nFlag);
	virtual int			Write (unsigned char * pBuff, int nSize);
	virtual long long 	SetPos (long long llPos, int nFlag);

	virtual QCIOType	GetType (void);

	virtual int 		GetParam (int nID, void * pParam);
	virtual int 		SetParam (int nID, void * pParam);

protected:
#ifdef __QC_OS_WIN32__
	HANDLE				m_hFile;
#else
	FILE *				m_hFile;
#endif // _OS_WIN32
	int					m_nFD;	
};

#endif // __CFileIO_H__