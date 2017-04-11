/*******************************************************************************
	File:		CDNSCache.cpp

	Contains:	DNS cache implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2017-01-06		Bangfei			Create file

*******************************************************************************/
#include "stdio.h"
#include "string.h"

#include "qcErr.h"

#include "CDNSCache.h"

CDNSCache::CDNSCache()
	: CBaseObject ()
	, m_pDNSList(NULL)
{
}

CDNSCache::~CDNSCache()
{
	if (m_pDNSList != NULL)
	{
		PDNSNode pNext = m_pDNSList;
		PDNSNode pCurr;
		while (pNext)
		{
			pCurr = pNext;
			pNext = pNext->pNext;
			delete[] pCurr->pHostName;
			delete pCurr;
		}
		m_pDNSList = NULL;
	}
}

void * CDNSCache::Get (char* pHostName)
{
	void *   nret	= NULL;
	PDNSNode ptr	= m_pDNSList;

	while (ptr)
	{
		if (ptr->pHostName && strcmp(ptr->pHostName, pHostName) == 0)
		{
			nret = ptr->pAddress;
			break;
		}
		else
			ptr = ptr->pNext;
	}
	return nret;
}

int CDNSCache::Add (char* pHostName, void * pAddress, unsigned int pAddressSize)
{
	PDNSNode ptr = m_pDNSList;
	PDNSNode ptrPrev = m_pDNSList;

	if (pHostName == NULL || strlen(pHostName) == 0)
		return QC_ERR_FAILED;

	while (ptr != NULL)
	{
		ptrPrev = ptr;
		ptr = ptr->pNext;
	}

	if(m_pDNSList == ptr)
	{
		m_pDNSList = new DNSNode;
		ptr = m_pDNSList;
	}
	else
	{
		ptrPrev->pNext = new DNSNode;
		ptr = ptrPrev->pNext;
	}

	ptr->pHostName = new char[strlen(pHostName) + 1];
	strcpy(ptr->pHostName, pHostName);
    ptr->pAddress = new char[pAddressSize];
    memcpy(ptr->pAddress, pAddress, pAddressSize);

	return QC_ERR_NONE;
}

int CDNSCache::Del (char* pHostName)
{
	PDNSNode ptr = m_pDNSList;
	PDNSNode ptrPrev = ptr;

	if (pHostName == NULL || strlen(pHostName) == 0 ||  ptr == NULL)
	{
		return QC_ERR_FAILED;
	}

	while (ptr != NULL)
	{ 
		if (ptr->pHostName && strcmp(ptr->pHostName, pHostName) == 0)
		{
			//handle the first item
			if (ptr == m_pDNSList)
				m_pDNSList = ptr->pNext;
			else
				ptrPrev->pNext = ptr->pNext;

			delete[] ptr->pHostName;
            delete[] ptr->pAddress;
			delete ptr;
			break;
		}

		ptrPrev = ptr;
		ptr = ptr->pNext;
	}

	return QC_ERR_NONE;
}

