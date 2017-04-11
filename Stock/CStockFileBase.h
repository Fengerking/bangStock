/*******************************************************************************
	File:		CStockFileBase.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockFileBase_H__
#define __CStockFileBase_H__

#include "CBaseObject.h"

class CStockFileBase : public CBaseObject
{
public:
    CStockFileBase(void);
    virtual ~CStockFileBase(void);

	virtual int		SetCode (char * pCode);
	virtual int		OpenFile (char * pFileName);

protected:
	virtual int		ReadDataLine (char * pData, int nSize);
	virtual int		ReadTextLine (char * pData, int nSize, char * pLine, int nLine);


public:
	char			m_szCode[32];
	char			m_szFile[256];
	char *			m_pFileData;
	char *			m_pFilePos;
	int				m_nFileSize;

};

#endif //__CStockFileBase_H__
