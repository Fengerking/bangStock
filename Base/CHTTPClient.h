/*******************************************************************************
	File:		CHTTPClient.h

	Contains:	http client header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2017-01-06		Bangfei			Create file

*******************************************************************************/
#ifndef __CHTTPClient_H__
#define __CHTTPClient_H__
#ifdef __QC_OS_WIN32__
#include "pthread.h"
#else
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include "pthread.h"
#endif

#include "CBaseObject.h"
#include "CDNSCache.h"

#define MAXDOMAINNAME 256

static const int KMaxHostAddrLen		= 256;
static const int KMaxHostFileNameLen	= 2048;
static const int KMaxRequestLen			= 2048;
static const int KMaxLineLength			= 4096;

typedef struct _DNSParam{
    char			domainName[MAXDOMAINNAME];
    unsigned int	ip;
    int				errorcode;
    _DNSParam(){
        memset(domainName, 0, MAXDOMAINNAME);
        errorcode = -1;
        ip = 0;
    }
} DNSParam;

class CHTTPClient;

typedef struct sockaddr* QCIPAddr; //For IPv4 and IPv6
typedef int (CHTTPClient::*_pFunConnect)(const char* aUrl, long long aOffset);

class CHTTPClient : public CBaseObject
{
public:
    CHTTPClient(CDNSCache * pDNSCache);
    virtual ~CHTTPClient(void);

	virtual int				RequestData (char * pHost, char * pPath, char * pHead, char * pData, int nPort = 80);

	virtual int				Read(char* aDstBuffer, int aSize);
	virtual int				Recv(char* aDstBuffer, int aSize);
	virtual int				Send(const char* aSendBuffer, int aSize);
	virtual int				Connect(const char* aUrl, long long aOffset = 0);
    virtual int				ConnectViaProxy(const char* aUrl, long long aOffset = 0);
	virtual long long		ContentLength(void) { return m_llContentLength;};
	virtual void			Interrupt(void);
	virtual unsigned int	HostIP(void);

 	virtual int				Disconnect(void);

	virtual int				HttpStatus(void);
	virtual unsigned int	StatusCode(void);
	virtual void			SetStatusCode(unsigned int aCode);

    virtual bool			IsCancel(void);
    virtual void            SetSocketCheckForNetException(void);
	virtual bool			IsTtransferBlock(){return m_bTransferBlock;};
	virtual int				RequireContentLength();
	virtual int				ConvertToValue(char * aBuffer);
	virtual char*			GetRedirectUrl(void);
	virtual void			SetHostMetaData(char* aHostHead);

protected:
	virtual int		ConnectServer(const QCIPAddr aHostIP, int& nPortNum);
	virtual int 	ConnectServerIPV4Proxy(unsigned int nHostIP, int& nPortNum);
	virtual int		ResolveDNS(char* aHostAddr, QCIPAddr aHostIP);
    
	virtual int		SendRequestAndParseResponse(_pFunConnect pFunConnect, const char* aUrl, int aPort, long long aOffset);
	virtual int		SendRequest(int aPort, long long aOffset);
	virtual int		ParseResponseHeader(unsigned int& aStatusCode);
	virtual bool	IsRedirectStatusCode(unsigned int aStatusCode);
	virtual int		Redirect(_pFunConnect pFunConnect, long long aOffset);
	virtual int		ParseHeader(unsigned int& aStatusCode);

	virtual int		ParseContentLength(unsigned int aStatusCode);
	virtual int		GetHeaderValueByKey(const char* aKey, char* aBuffer, int aBufferLen);
	virtual int		ReceiveLine(char* aLine, int aSize);

	virtual int		Receive(int& aSocketHandle, timeval& aTimeOut, char* aDstBuffer, int aSize);
	virtual int		WaitSocketWriteBuffer(int& aSocketHandle, timeval& aTimeOut);
	virtual int		WaitSocketReadBuffer(int& aSocketHandle, timeval& aTimeOut);
	virtual int		SetSocketTimeOut(int& aSocketHandle, timeval aTimeOut);

	virtual void	SetSocketBlock(int& aSocketHandle);
	virtual void	SetSocketNonBlock(int& aSocketHandle);

protected:
	enum State {
	        DISCONNECTED,
	        CONNECTING,
	        CONNECTED
	    };

	State			m_sState;
	int				m_nSocketHandle;
	long long		m_llContentLength;
	int				m_nWSAStartup;

	char			m_szLineBuffer[KMaxLineLength];
	char			m_szHeaderValueBuffer[KMaxLineLength];
	pthread_t		m_hConnectionTid;

	char			m_szHostAddr[KMaxHostAddrLen];
	char			m_szHostFileName[KMaxHostFileNameLen];
	char			m_szRequset[KMaxRequestLen];
	char*			m_pHostMetaData;

	CDNSCache*		m_pDNSCache;
	QCIPAddr		m_sHostIP;
	unsigned int	m_nStatusCode;
    bool			m_bCancel;
    unsigned int    m_nHostIP;

	bool			m_bMediaType;
	bool			m_bTransferBlock;
	char			m_szRedirectUrl[KMaxLineLength];
};

#endif // __CHTTPClient_H__