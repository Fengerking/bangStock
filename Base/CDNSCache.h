/*******************************************************************************
	File:		CDNSCache.h

	Contains:	DNS cache header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2017-01-06		Bangfei			Create file

*******************************************************************************/
#ifndef __CDNSCache_H__
#define __CDNSCache_H__
#include "CBaseObject.h"

typedef struct DNSNode
{
    char*		pHostName;
    void *      pAddress; //适配IPv4 & IPv6地址，指向struct sockaddr
    DNSNode*	pNext;
    DNSNode()
    {
        pHostName = NULL;
        pAddress = 0;
        pNext = NULL;
    }
} DNSNode, *PDNSNode;

class CDNSCache : public CBaseObject
{
public:
	CDNSCache();
	virtual ~CDNSCache();

	virtual int		Add (char* pHostName, void * pAddress, unsigned int pAddressSize);
	virtual int		Del (char* pHostName);
	virtual void *	Get (char* pHostName);

protected:
	DNSNode *		m_pDNSList;
};

#endif // __CDNSCache_H__
