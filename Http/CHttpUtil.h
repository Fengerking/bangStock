/*******************************************************************************
	File:		CHttpUtil.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CHttpUtil_H__
#define __CHttpUtil_H__
#include <string>  
#include <iostream>  

#include "CBaseObject.h"
#include "curl.h"

#include "CFileIO.h"

class CHttpUtil : public CBaseObject
{
public:
    CHttpUtil(void);
    virtual ~CHttpUtil(void);

	virtual int		RequestData (const char * pURL, char ** ppData);

protected:
	CURL *			m_pCURL;
	curl_slist		m_lstHeader;

    std::string		m_szBufferData;  
    std::string		m_szBufferHead;  
	double			m_dDownSize;
	double			m_dDownTime;
	double			m_dDownSpeed;

public:
	static size_t	CURL_CallWriteHead (const char * pData, size_t nBlock, size_t nSize, std::string * strStream);
	static size_t	CURL_CallWriteData (const char * pData, size_t nBlock, size_t nSize, std::string * strStream);
};

#endif //__CHttpUtil_H__
