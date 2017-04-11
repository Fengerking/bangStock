/*******************************************************************************
	File:		CHTTPClient.cpp

	Contains:	http client implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2017-01-06		Bangfei			Create file

*******************************************************************************/
#ifdef __QC_OS_WIN32__
#include <winsock2.h>
#include "Ws2tcpip.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <signal.h> 
#include <ctype.h>
#include "GKMacrodef.h"
#include <exception>
#include <typeinfo>
#include <pthread.h>
#include <sys/types.h>
#include <fcntl.h>
#include<signal.h>
#endif // __QC_OS_WIN32__

#include "qcErr.h"
#include "CHTTPClient.h"

#include "UMsgMng.h"
#include "UUrlParser.h"
#include "USystemFunc.h"
#include "ULogFunc.h"

#define  CONNECTION_TIMEOUT_IN_SECOND			30	//unit: second
#define  HTTP_HEADER_RECV_TIMEOUT_IN_SECOND		30 //unit: second
#define  HTTP_HEADER_RECV_MAXTIME				10000 //

#define  CONNECT_ERROR_BASE		600		//connect error base value
#define  REQUEST_ERROR_BASE		1000	//request error base value
#define  RESPONSE_ERROR_BASE	1300	//response error base value
#define  DNS_ERROR_BASE			2000	//dns resolve error base value

#define HTTPRESPONSE_INTERUPTER 1304
#define ERROR_CODE_TEST         902
#define INET_ADDR_EXCEPTION     16
#define DNS_TIME_OUT            17
#define DNS_UNKNOWN             18

#define TIME_KEEP_ALIVE  1  // 打开探测
#define TIME_KEEP_IDLE   10  // 开始探测前的空闲等待时长10s
#define TIME_KEEP_INTVL  2  // 发送探测分节的时间间隔 2s
#define TIME_KEEP_CNT    3  // 发送探测分节的次数 3 times

static const int KInvalidSocketHandler = -1;

static const int KInvalidContentLength = -1;

static const char KContentType[] = {'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'T', 'y', 'p', 'e', '\0'};
static const char KContentRangeKey[] = {'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'R', 'a', 'n', 'g', 'e', '\0'};
static const char KContentLengthKey[] = {'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'L', 'e', 'n', 'g', 't', 'h', '\0'};
static const char KLocationKey[] = {'L', 'o', 'c', 'a', 't', 'i', 'o', 'n', '\0'};
static const char KTransferEncodingKey[] = {'T', 'r', 'a', 'n', 's', 'f', 'e', 'r', '-', 'E', 'n', 'c', 'o', 'd',  'i', 'n', 'g','\0'};

static const int STATUS_OK = 0;
static const int Status_Error_ServerConnectTimeOut = 905;
static const int Status_Error_HttpResponseTimeOut = 1556;
static const int Status_Error_HttpResponseBadDescriptor = 1557;
static const int Status_Error_HttpResponseArgumentError= 1558;
static const int Status_Error_NoUsefulSocket = 1559;

#define	_ETIMEDOUT	60

unsigned int	g_ProxyHostIP = 0;
QCIPAddr		g_ProxyHostIPV6 = NULL;
int				g_ProxyHostPort = 0;
char*			g_AutherKey = NULL;
char*			g_Domain = NULL;

#define ONESECOND			1000
#define WAIT_DNS_INTERNAL	50
#define WAIT_DNS_MAX_TIME	600

void HTTP_SignalHandle(int avalue)
{
  //  gCancle = true;
}

CHTTPClient::CHTTPClient(CDNSCache * pDNSCache)
	: CBaseObject ()
	, m_sState (DISCONNECTED)
	, m_nSocketHandle (0)
	, m_llContentLength (0)
	, m_nWSAStartup (0)
	, m_pHostMetaData (NULL)
	, m_pDNSCache (pDNSCache)
	, m_sHostIP (NULL)
	, m_nStatusCode (0)
	, m_bCancel (false)
	, m_nHostIP (0)
	, m_bMediaType (false)
	, m_bTransferBlock (false)
{
	SetObjectName ("CHTTPClient");
#ifndef __QC_OS_WIN32__
   m_hConnectionTid = 0;
#endif

#ifdef __QC_OS_WIN32__
   	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD( 2, 2 );
	m_nWSAStartup = WSAStartup( wVersionRequested, &wsaData );
#else
	struct sigaction act, oldact;
	act.sa_handler = HTTP_SignalHandle;
	act.sa_flags = SA_NODEFER; 
	//sigaddset(&act.sa_mask, SIGALRM);
 	sigaction(SIGALRM, &act, &oldact);
	//signal(SIGPIPE, SIG_IGN);
#endif
	memset(&m_hConnectionTid, 0, sizeof(m_hConnectionTid));
	memset(m_szRedirectUrl,0,sizeof(m_szRedirectUrl));
}

CHTTPClient::~CHTTPClient(void)
{
	if (m_sState == CONNECTED)
		Disconnect();
#ifdef __QC_OS_WIN32__
	WSACleanup();
#endif
    
    if (m_nHostIP != NULL) 
	{
        struct sockaddr_storage* tmp = (struct sockaddr_storage*)m_nHostIP;
        //b need free?  free(tmp);
        m_nHostIP = NULL;
    }

	QC_FREE_P (m_pHostMetaData);
}

int CHTTPClient::RequestData (char * pHost, char * pPath, char * pHead, char * pData, int nPort)
{
	if(m_nWSAStartup)
		return QC_ERR_STATUS;

	m_nStatusCode = STATUS_OK;
	m_bCancel = false;
	m_bTransferBlock = false;
	m_bMediaType = false;
    if (m_sHostIP == NULL)
        m_sHostIP = (QCIPAddr)malloc(sizeof(struct sockaddr_storage));
    else
        memset(m_sHostIP, 0, sizeof(struct sockaddr_storage));
    m_nHostIP = 0;
    
	int nErr = ResolveDNS(pHost, m_sHostIP);
	if( nErr != QC_ERR_NONE)
		return NULL;

	nErr = ConnectServer(m_sHostIP, nPort); 
	if( nErr != QC_ERR_NONE)
		return nErr;

	char	szRequest[4096];
	char	szLine[1024];
	sprintf (szRequest, "GET %s HTTP/1.1\r\n", pPath);
	sprintf (szLine, "%s\r\n\r\n", pHead);
	strcat (szRequest, szLine);
	if (strlen (pData) > 0)
	{
		sprintf (szLine, "%s\r\n", pData);
		strcat (szRequest, szLine);
	}

/*

	if(nPort != 80) {
		sprintf(m_szRequset, "GET /%s HTTP/1.1\r\n%s:%d\r\nConnection: keep-alive\r\n\r\n", m_szHostFileName, m_pHostMetaData, aPort);
	 else {
		sprintf(m_szRequset, "GET /%s HTTP/1.1\r\n%s\r\nConnection: keep-alive\r\n\r\n", m_szHostFileName, m_pHostMetaData);
	}
*/
	nErr = Send(szRequest, strlen(szRequest));
	
    if ((nErr != QC_ERR_NONE) && (m_sState == CONNECTED))
	{
        nErr = QC_ERR_CANNOT_CONNECT;
		QCLOGE("connect failed. Connection is going to be closed");
		Disconnect();
	}
	
	return nErr;
}

int CHTTPClient::Read(char* aDstBuffer, int aSize)
{
	if(m_sState == DISCONNECTED)
		return QC_ERR_Disconnected;

	struct timeval tTimeout = {0, 500000};
	return Receive(m_nSocketHandle, tTimeout, aDstBuffer, aSize);
}

int CHTTPClient::Recv(char* aDstBuffer, int aSize)
{
	struct timeval tTimeout = {0, 20000};//{HTTP_HEADER_RECV_TIMEOUT_IN_SECOND, 0};
    int nErr;
   // int retryCnt = HTTP_HEADER_RECV_MAXTIMES;// 6*5s = 30s, total wait time is 30s;
	long long nStartTime = qcGetSysTime();
	long long nOffset = 0;
	do{
        nErr = Receive(m_nSocketHandle, tTimeout, aDstBuffer, aSize);
        nOffset = qcGetSysTime() - nStartTime;
        if (nOffset > HTTP_HEADER_RECV_MAXTIME || m_bCancel) {
            break;
        }
    }while (nErr == 0);
    return nErr;
}

int CHTTPClient::Send(const char* aSendBuffer, int aSize)
{
	if(m_sState == DISCONNECTED)
		return QC_ERR_Disconnected;

	int nSend = 0;
	int nTotalsend = 0;
	while(nTotalsend < aSize)
	{
#ifdef __QC_OS_WIN32__
		nSend = send(m_nSocketHandle , aSendBuffer + nTotalsend, aSize - nTotalsend , 0 );
#else
		nSend = write(m_nSocketHandle, aSendBuffer + nTotalsend, aSize - nTotalsend);
		if (nSend < 0 && errno == EINTR) {
              nSend = 0;        /* and call write() again */
		} 
#endif
		if(nSend < 0)
		{
			SetStatusCode(errno + REQUEST_ERROR_BASE);
			QCLOGE("send error!%s/n", strerror(errno));
		    return QC_ERR_CANNOT_CONNECT;
		}

		nTotalsend += nSend;
	}

	return QC_ERR_NONE;
}

int CHTTPClient::Connect(const char* aUrl, long long aOffset)
{
	if(m_nWSAStartup)
		return QC_ERR_CANNOT_CONNECT;

	QCMSG_Notify (QC_MSG_HTTP_CONNECT_START, 0, 0, aUrl);

	int nPort;
	qcUrlParseUrl(aUrl, m_szHostAddr, m_szHostFileName, nPort);

	m_nStatusCode = STATUS_OK;
	m_bCancel = false;
	m_bTransferBlock = false;
	m_bMediaType = false;
	m_llContentLength = KInvalidContentLength;
#ifdef __QC_OS_WIN32__
	m_hConnectionTid.p = NULL;
	m_hConnectionTid.x = 0;
#else
	m_hConnectionTid = pthread_self();
#endif //  __QC_OS_WIN32__
    if (m_sHostIP == NULL) {
        m_sHostIP = (QCIPAddr)malloc(sizeof(struct sockaddr_storage));
    }
    else{
        memset(m_sHostIP, 0, sizeof(struct sockaddr_storage));
    }
    m_nHostIP = 0;
    
	int nErr = ResolveDNS(m_szHostAddr, m_sHostIP);
	if( nErr != QC_ERR_NONE)
		return nErr;

	nErr = ConnectServer(m_sHostIP, nPort); 
	if( nErr != QC_ERR_NONE)
		return nErr;

	return SendRequestAndParseResponse(&CHTTPClient::Connect, aUrl, nPort, aOffset);
}

int CHTTPClient::ConnectViaProxy(const char* aUrl, long long aOffset)
{
	if(m_nWSAStartup)
		return QC_ERR_CANNOT_CONNECT;
 
	QCMSG_Notify (QC_MSG_HTTP_CONNECT_START, 0, 0, aUrl);

    char	tLine[3] = {0};
    int		nPort;
    int		nErr;

	m_nStatusCode = STATUS_OK;
	m_bCancel = false;
	m_bTransferBlock = false;
	m_bMediaType = false;
	m_llContentLength = KInvalidContentLength;
#ifdef __QC_OS_WIN32__
	m_hConnectionTid.p = NULL;
	m_hConnectionTid.x = 0;
#else
	m_hConnectionTid = pthread_self();
#endif //  __QC_OS_WIN32__
    if (g_Domain != NULL) {
        //memset(&g_ProxyHostIP, 0, sizeof(QCIPAddr));
        if (g_ProxyHostIPV6 == NULL) {
            g_ProxyHostIPV6 = (QCIPAddr)malloc(sizeof(struct sockaddr_storage));
        }
        else{
            memset(g_ProxyHostIPV6, 0, sizeof(struct sockaddr_storage));
        }
        
        int nErr = ResolveDNS(g_Domain, g_ProxyHostIPV6);
        if( nErr != QC_ERR_NONE)
        {
            return nErr;
        }
        
        nErr = ConnectServer(g_ProxyHostIPV6, g_ProxyHostPort);
    }
    else
        nErr = ConnectServerIPV4Proxy(g_ProxyHostIP, g_ProxyHostPort);
    
    if( nErr != QC_ERR_NONE)
		return nErr;
    
    qcUrlParseUrl(aUrl, m_szHostAddr, m_szHostFileName, nPort);
    
	m_nStatusCode = STATUS_OK;
    char strRequest[2048] = {0};
    
    // sprintf(strRequest, "CONNECT %s:%d HTTP/1.1\r\nHost: %s:%d\r\nProxy-Connection: Keep-Alive\r\nContent-Length: 0\r\nProxy-Authorization: Basic MzAwMDAwNDU1MDpCREFBQUQ5QjczOUQzQjNG\r\n\r\n",g_pHostAddr, nPort, g_pHostAddr, nPort);MzAwMDAwNDU1MDpCREFBQUQ5QjczOUQzQjNG
    sprintf(strRequest, "CONNECT %s:%d HTTP/1.1\r\nProxy-Authorization: Basic %s\r\n\r\n",m_szHostAddr, nPort, g_AutherKey);
    
    //send proxyserver connect request
	nErr = Send(strRequest, strlen(strRequest));
    if (nErr != QC_ERR_NONE)
        return nErr;
    
    unsigned int nStatusCode;
    //wait for proxyserver connect response
    //response: HTTP/1.1 200 Connection established\r\n
    nErr = ParseResponseHeader(nStatusCode);
	if (nStatusCode != 200)
        return nErr;
    
    //read \r\n
    Recv(tLine, 2);
    
    return SendRequestAndParseResponse(&CHTTPClient::ConnectViaProxy, aUrl, nPort, aOffset);
}

void  CHTTPClient::Interrupt()
{
#ifndef __QC_OS_WIN32__
	if (m_hConnectionTid > 0 && !pthread_equal(m_hConnectionTid, pthread_self()))
	{
		int pthread_kill_err = pthread_kill(m_hConnectionTid, 0);
		if((pthread_kill_err != ESRCH) && (pthread_kill_err != EINVAL))
		{
			pthread_kill(m_hConnectionTid, SIGALRM);
			QCLOGI("sent interrupt signal");
		}
	}
#endif
	
	m_bCancel = true;
}

unsigned int CHTTPClient::HostIP()
{
    return  m_nHostIP;
}

void  CHTTPClient::SetSocketCheckForNetException(void)
{
#ifdef __QC_OS_IOS__
    int keepalive = TIME_KEEP_ALIVE;
    int keepidle = TIME_KEEP_IDLE;
    int keepintvl = TIME_KEEP_INTVL;
    int keepcnt = TIME_KEEP_CNT;
    if(m_nSocketHandle != KInvalidSocketHandler){
        setsockopt(m_nSocketHandle, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof (keepalive));
        setsockopt(m_nSocketHandle, IPPROTO_TCP, TCP_KEEPALIVE, (void *) &keepidle, sizeof (keepidle));
        setsockopt(m_nSocketHandle, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&keepintvl, sizeof (keepintvl));
        setsockopt(m_nSocketHandle, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepcnt, sizeof (keepcnt));
    }
#endif
}

int CHTTPClient::RequireContentLength()
{
	int nErr = QC_ERR_ARG;

	if(!m_bTransferBlock)
		return nErr;
	while (true)
	{
		nErr = ReceiveLine(m_szLineBuffer, sizeof(char) * KMaxLineLength);
		if (nErr != QC_ERR_NONE)
		{
			QCLOGE("CHTTPClient RecHeader Error:%d", nErr);
			break;
		}
		if (m_szLineBuffer[0] == '\0')
			continue;
		
		int a= ConvertToValue(m_szLineBuffer);
		return a;
	}
	return nErr;
}

int CHTTPClient::ConvertToValue(char * aBuffer)
{
	int size = strlen(aBuffer);
	 int i=0;
	 int value = 0;
	 while(i<size)
	 {
		 if(aBuffer[i] >= '0' && aBuffer[i] <= '9')
		 {
			 value = value* 16 +(aBuffer[i]-'0');
		 }
		 else  if(aBuffer[i] >= 'a' && aBuffer[i] <= 'f')
		 {
			 value = value* 16 +(aBuffer[i]-'a' + 10);
		 }
		 else  if(aBuffer[i] >= 'A' && aBuffer[i] <= 'F')
		 {
			 value = value* 16 +(aBuffer[i]-'A' + 10);
		 }
		 else
			 return -1;

		 i++;
	  }
	 return value;
}

char* CHTTPClient::GetRedirectUrl(void)
{
	if(strlen(m_szRedirectUrl) == 0)
		return NULL;
	else
		return m_szRedirectUrl;
}

void CHTTPClient::SetHostMetaData(char* aHostHead)
{
	QC_FREE_P (m_pHostMetaData);
	if(aHostHead)
	{
		m_pHostMetaData = (char*)malloc(strlen(aHostHead) + 1);
		strcpy(m_pHostMetaData, aHostHead);
	}
}

int CHTTPClient::Disconnect()
{
	QCMSG_Notify (QC_MSG_HTTP_DISCONNECT_START, 0, 0);

	if ((m_sState == CONNECTED || m_sState == CONNECTING) && (m_nSocketHandle != KInvalidSocketHandler))
	{
#ifdef __QC_OS_WIN32__
		closesocket(m_nSocketHandle);
#else
		close(m_nSocketHandle);
#endif
		m_nSocketHandle = KInvalidSocketHandler;
		m_sState = DISCONNECTED;
	}
	m_bTransferBlock = false;
	m_bMediaType = false;
	memset(m_szRedirectUrl,0,sizeof(m_szRedirectUrl));
#ifndef __QC_OS_WIN32__
	m_hConnectionTid = 0;
#else
	memset(&m_hConnectionTid, 0, sizeof(m_hConnectionTid));
#endif
	m_bCancel = false;

	QCMSG_Notify (QC_MSG_HTTP_DISCONNECT_DONE, 0, 0);

	return 0;
}

bool CHTTPClient::IsCancel()
{
    return m_bCancel;
}

int CHTTPClient::HttpStatus(void)
{
	return (int)m_sState;
}

unsigned int CHTTPClient::StatusCode(void)
{
	return m_nStatusCode;
}

void CHTTPClient::SetStatusCode(unsigned int aCode)
{
    //just for test, will delete later!
    if (aCode == ERROR_CODE_TEST || aCode == ERROR_CODE_TEST + 2) 
        aCode = aCode<<1;
	m_nStatusCode = aCode;
}

int CHTTPClient::ConnectServer(const QCIPAddr aHostIP, int& nPortNum)
{
	if((m_nSocketHandle = socket(aHostIP->sa_family, SOCK_STREAM, 0)) == KInvalidSocketHandler)//
	{
		QCLOGE("socket return error");
		m_nStatusCode = Status_Error_NoUsefulSocket;
		return QC_ERR_CANNOT_CONNECT;
	}
	m_sState = CONNECTING;

	SetSocketNonBlock(m_nSocketHandle); //aIP = ((struct sockaddr_in*)(res->ai_addr))->sin_addr;
    
    if (aHostIP->sa_family==AF_INET6) {
        ((struct sockaddr_in6*)aHostIP)->sin6_port = htons(nPortNum);
    } else {
        ((struct sockaddr_in*)aHostIP)->sin_port   = htons(nPortNum);
    }
#ifdef __TT_OS_IOS__
	int nErr = connect(m_nSocketHandle, aHostIP, aHostIP->sa_len);
#else
    int nErr = connect(m_nSocketHandle, aHostIP, sizeof(struct sockaddr));
#endif
    
	if (nErr < 0)
	{
		m_nStatusCode = errno + CONNECT_ERROR_BASE;
#ifdef __QC_OS_WIN32__
		if(nErr == -1)
#else
		if (errno == EINPROGRESS)
#endif
		{
			timeval timeout = {CONNECTION_TIMEOUT_IN_SECOND, 0};
			nErr = WaitSocketWriteBuffer(m_nSocketHandle, timeout);
		}

		if (nErr < 0)
		{
			if (nErr == QC_ERR_TIMEOUT)
			{
				m_nStatusCode = Status_Error_ServerConnectTimeOut;
				if (m_pDNSCache != NULL)
					m_pDNSCache->Del(m_szHostAddr); 
			}

			QCLOGE("connect error. nErr: %d, errorno: %d", nErr, errno);
			Disconnect();
			SetSocketBlock(m_nSocketHandle);
			return QC_ERR_CANNOT_CONNECT;
		}
	}	

	SetSocketBlock(m_nSocketHandle);

	m_sState = CONNECTED;

	QCMSG_Notify (QC_MSG_HTTP_CONNECT_SUCESS, 0, 0);

	return QC_ERR_NONE;
}

int CHTTPClient::ConnectServerIPV4Proxy(unsigned int nHostIP, int& nPortNum)
{
    if((m_nSocketHandle = socket(AF_INET, SOCK_STREAM, 0)) == KInvalidSocketHandler)
    {
        QCLOGE("socket return error");
        m_nStatusCode = Status_Error_NoUsefulSocket;
        return QC_ERR_CANNOT_CONNECT;
    }
    m_sState = CONNECTING;
    
    SetSocketNonBlock(m_nSocketHandle);
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(nPortNum);
    server_addr.sin_addr = *(struct in_addr *)(&nHostIP);
    int nErr = connect(m_nSocketHandle, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr));
    if (nErr < 0)
    {
        m_nStatusCode = errno + CONNECT_ERROR_BASE;
#ifdef __QC_OS_WIN32__
        if(nErr == -1)
#else
            if (errno == EINPROGRESS)
#endif
            {
                timeval timeout = {CONNECTION_TIMEOUT_IN_SECOND, 0};
                nErr = WaitSocketWriteBuffer(m_nSocketHandle, timeout);
            }
        
        if (nErr < 0)
        {
            if (nErr == QC_ERR_TIMEOUT)
            {
                m_nStatusCode = Status_Error_ServerConnectTimeOut;
				if (m_pDNSCache != NULL)
				   m_pDNSCache->Del(m_szHostAddr);
            }
            
            QCLOGE("connect error. nErr: %d, errorno: %d", nErr, errno);
            Disconnect();
            SetSocketBlock(m_nSocketHandle);
            return QC_ERR_CANNOT_CONNECT;
        }
    }	
    
    SetSocketBlock(m_nSocketHandle);
    
    m_sState = CONNECTED;
    
    return QC_ERR_NONE;
}

int CHTTPClient::ResolveDNS(char* aHostAddr, QCIPAddr aHostIP)
{
    QCIPAddr *	cachedHostIp = NULL;
    int			loopWaitCnt = 0;
    bool		parseRet;
  
	QCMSG_Notify (QC_MSG_HTTP_DNS_START, 0, 0, aHostAddr);

 	if (m_pDNSCache != NULL)
		cachedHostIp = (QCIPAddr*)m_pDNSCache->Get(aHostAddr);
	if (cachedHostIp != NULL)
    {
        memcpy((void *)aHostIP, cachedHostIp, sizeof(struct sockaddr_storage));
		QCMSG_Notify (QC_MSG_HTTP_DNS_GET_CACHE, 0, 0);
        return QC_ERR_NONE;
    }
    if (m_bCancel)
        return QC_ERR_CANNOT_CONNECT;

    parseRet = false;
    struct addrinfo hints;
    struct addrinfo *res;
    int ret;
    char pHostIP[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; /* Allow IPv4 & IPv6 */
    hints.ai_socktype = SOCK_STREAM;
    ret = getaddrinfo(aHostAddr, NULL,&hints,&res);
    if (ret == 0 && res != NULL) {
        memcpy(aHostIP, res->ai_addr, res->ai_addrlen);
        freeaddrinfo(res);
        parseRet = true;
    } else {
         m_nStatusCode = DNS_ERROR_BASE + ret;
		 QCLOGE ("getaddrinfo return err: %d", ret);
    }
        
    if (parseRet) 
	{
        void *numericAddress = NULL;
        if (aHostIP->sa_family == AF_INET6) {
            m_nHostIP = 0xffffffff;
            //numericAddress = (void*)&(((struct sockaddr_in6*)&aHostIP)->sin6_addr);
        } else {
            numericAddress = (void*)&(((struct sockaddr_in*)aHostIP)->sin_addr);
            if (inet_ntop(aHostIP->sa_family, numericAddress, pHostIP, INET6_ADDRSTRLEN) != NULL){
                m_nHostIP = inet_addr(pHostIP);
                if (strcmp(pHostIP, aHostAddr) != 0)
                {
					if (m_pDNSCache != NULL)
					  m_pDNSCache->Add (aHostAddr, (void *)aHostIP, sizeof(struct sockaddr_storage));
                }
            }
        }
		QCMSG_Notify (QC_MSG_HTTP_DNS_GET_IPADDR, 0, 0);
        return QC_ERR_NONE;
    }       
    //parse fail
    return QC_ERR_CANNOT_CONNECT;
}

int CHTTPClient::SendRequestAndParseResponse(_pFunConnect pFunConnect, const char* aUrl, int aPort, long long aOffset)
{
	//send get file request
    int nErr = SendRequest(aPort, aOffset);
    if (nErr == QC_ERR_NONE)
    {
		unsigned int nStatusCode = STATUS_OK;
        nErr = ParseResponseHeader(nStatusCode);
        if (nErr == QC_ERR_NONE)
        {
            if (IsRedirectStatusCode(nStatusCode))
            {
                return Redirect(pFunConnect, aOffset);
            }
            else if (nStatusCode == 200 || nStatusCode == 206)
            {
                nErr = ParseContentLength(nStatusCode);
            }
            else
			{
				m_nStatusCode = nStatusCode;
				nErr = QC_ERR_CANNOT_CONNECT;
			}
		}
	}
    
    if ((nErr != QC_ERR_NONE) && (m_sState == CONNECTED))
	{
        nErr = QC_ERR_CANNOT_CONNECT;
		QCLOGE("connect failed. Connection is going to be closed");
		Disconnect();
	}
	
	struct timeval nRecvDataTimeOut = {0, 500000};
	SetSocketTimeOut(m_nSocketHandle, nRecvDataTimeOut);

	return nErr;
}

int CHTTPClient::SendRequest(int aPort, long long aOffset)
{
	memset(m_szRequset, 0, sizeof(m_szRequset));
	if (m_pHostMetaData)
	{
		if(NULL == strstr(m_pHostMetaData,"Host:"))
		{
			if (aOffset > 0)
			{
				if(aPort != 80) {
					sprintf(m_szRequset, "GET /%s HTTP/1.1\r\n%sHost: %s:%d\r\nRange: bytes=%ld-\r\nConnection: keep-alive\r\n\r\n", m_szHostFileName, m_pHostMetaData, m_szHostAddr, aPort, aOffset);

				} else {
					sprintf(m_szRequset, "GET /%s HTTP/1.1\r\n%sHost: %s\r\nRange: bytes=%ld-\r\nConnection: keep-alive\r\n\r\n", m_szHostFileName, m_pHostMetaData, m_szHostAddr, aOffset);
				}
			}
			else
			{
				if(aPort != 80) {
					sprintf(m_szRequset, "GET /%s HTTP/1.1\r\n%sHost: %s:%d\r\nConnection: keep-alive\r\n\r\n", m_szHostFileName, m_pHostMetaData, m_szHostAddr, aPort);
				} else {
					sprintf(m_szRequset, "GET /%s HTTP/1.1\r\n%sHost: %s\r\nConnection: keep-alive\r\n\r\n", m_szHostFileName, m_pHostMetaData, m_szHostAddr);
				}
			}
		}
		else{
			if (aOffset > 0)
			{
				if(aPort != 80) {
					sprintf(m_szRequset, "GET /%s HTTP/1.1\r\n%s:%d\r\nRange: bytes=%ld-\r\nConnection: keep-alive\r\n\r\n", m_szHostFileName, m_pHostMetaData, aPort, aOffset);
				} else {
					sprintf(m_szRequset, "GET /%s HTTP/1.1\r\n%s\r\nRange: bytes=%ld-\r\nConnection: keep-alive\r\n\r\n", m_szHostFileName, m_pHostMetaData, aOffset);
				}
			}
			else
			{
				if(aPort != 80) {
					sprintf(m_szRequset, "GET /%s HTTP/1.1\r\n%s:%d\r\nConnection: keep-alive\r\n\r\n", m_szHostFileName, m_pHostMetaData, aPort);
				} else {
					sprintf(m_szRequset, "GET /%s HTTP/1.1\r\n%s\r\nConnection: keep-alive\r\n\r\n", m_szHostFileName, m_pHostMetaData);
				}
			}
		}
	}
	else{
		if (aOffset > 0) 
		{
			if(aPort != 80) {
				sprintf(m_szRequset, "GET /%s HTTP/1.1\r\nHost: %s:%d\r\nRange: bytes=%ld-\r\nConnection: keep-alive\r\n\r\n", m_szHostFileName, m_szHostAddr, aPort, aOffset);
			} else {
				sprintf(m_szRequset, "GET /%s HTTP/1.1\r\nHost: %s\r\nRange: bytes=%ld-\r\nConnection: keep-alive\r\n\r\n", m_szHostFileName, m_szHostAddr, aOffset);
			}
		}
		else
		{
			if(aPort != 80) {
				sprintf(m_szRequset, "GET /%s HTTP/1.1\r\nHost: %s:%d\r\nConnection: keep-alive\r\n\r\n", m_szHostFileName, m_szHostAddr, aPort);
			} else {
				sprintf(m_szRequset, "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\n\r\n", m_szHostFileName, m_szHostAddr);
			}
		}
	}

	return Send(m_szRequset, strlen(m_szRequset));
}

int CHTTPClient::ParseResponseHeader(unsigned int& aStatusCode)
{
	int nErr = ParseHeader(aStatusCode);
	if(nErr == QC_ERR_BadDescriptor)
	{
		m_nStatusCode = Status_Error_HttpResponseBadDescriptor;
		QCLOGW("ParseResponseHeader return %d, %u", nErr, aStatusCode);
	}
	return nErr;
}

int CHTTPClient::ParseHeader(unsigned int& aStatusCode)
{
	char tLine[KMaxLineLength];

	int nErr = ReceiveLine(tLine, sizeof(tLine));
	if (nErr != QC_ERR_NONE)
	{
		QCLOGE("Receive Response Error!");
		return nErr;
	}

	char* pSpaceStart = strchr(tLine, ' ');
	if (pSpaceStart == NULL) 
	{
		QCLOGE("Receive Response content Error!");
		return QC_ERR_BadDescriptor;
	}

	char* pResponseStatusStart = pSpaceStart + 1;
	char* pResponseStatusEnd = pResponseStatusStart;
	while (isdigit(*pResponseStatusEnd)) 
	{
		++pResponseStatusEnd;
	}

	if (pResponseStatusStart == pResponseStatusEnd) 
	{
		return QC_ERR_BadDescriptor;
	}

	memmove(tLine, pResponseStatusStart, pResponseStatusEnd - pResponseStatusStart);
	tLine[pResponseStatusEnd - pResponseStatusStart] = '\0';

	unsigned int nResponseNum = strtol(tLine, NULL, 10);
	if ((nResponseNum < 0) || (nResponseNum > 999))
	{
		QCLOGE("Receive Invalid ResponseNum!");
		return QC_ERR_BadDescriptor;
	}

	aStatusCode = nResponseNum;

	QCMSG_Notify (QC_MSG_HTTP_GET_HEADDATA, 0, 0, tLine);

	return QC_ERR_NONE;
}

bool CHTTPClient::IsRedirectStatusCode(unsigned int aStatusCode)
{
	return aStatusCode == 301 || aStatusCode == 302
		|| aStatusCode == 303 || aStatusCode == 307;
}

int CHTTPClient::Redirect(_pFunConnect pFunConnect, long long aOffset)
{
	int nErr = GetHeaderValueByKey(KLocationKey, m_szHeaderValueBuffer, sizeof(char)*KMaxLineLength);
	Disconnect();
	if (QC_ERR_NONE == nErr)
	{
		 memcpy(m_szRedirectUrl,m_szHeaderValueBuffer,sizeof(m_szRedirectUrl));
		 QCMSG_Notify (QC_MSG_HTTP_REDIRECT, 0, 0, m_szRedirectUrl);
		 return (this->*pFunConnect)(m_szHeaderValueBuffer, aOffset);
	}
	else
	{
		nErr = QC_ERR_CANNOT_CONNECT;
	}
	return nErr;
}

int CHTTPClient::ParseContentLength(unsigned int aStatusCode)
{
	const char* pKey = (aStatusCode == 206) ? KContentRangeKey : KContentLengthKey;
	int nErr = GetHeaderValueByKey(pKey, m_szHeaderValueBuffer, sizeof(char)*KMaxLineLength);
	if(m_bTransferBlock) 
		return QC_ERR_NONE;

	if(QC_ERR_FINISH == nErr && m_bMediaType) 
	{
		m_llContentLength = 0;
		return QC_ERR_NONE;
	}

	if (QC_ERR_NONE == nErr)
	{
		char *pStart = (aStatusCode == 206) ? strchr(m_szHeaderValueBuffer, '/') + 1 : m_szHeaderValueBuffer;
		char* pEnd = NULL;
		long nContentLen = strtol(pStart, &pEnd, 10);

		if ((pEnd == m_szHeaderValueBuffer) || (*pEnd != '\0'))
		{
			QCLOGE("CHTTPClient Get ContentLength Error!");
			m_nStatusCode = Status_Error_HttpResponseArgumentError;
			nErr = QC_ERR_ARG;
		}
		else
		{
			m_llContentLength = nContentLen;
			QCMSG_Notify (QC_MSG_HTTP_CONTENT_LEN, 0, m_llContentLength);
		}
	}
	return nErr;
}

int CHTTPClient::GetHeaderValueByKey(const char* aKey, char* aBuffer, int aBufferLen)
{
	int nErr = QC_ERR_ARG;
    bool bIsKeyFound = false;
	bool bIsKeyChange = false;

	if(0 == strcmp(aKey, KContentLengthKey))
		bIsKeyChange = true;

	while (true)
	{
		nErr = ReceiveLine(m_szLineBuffer, sizeof(char) * KMaxLineLength);

		if (nErr != QC_ERR_NONE)
		{
			QCLOGE("CHTTPClient RecHeader Error:%d", nErr);
			break;
		}

		if(m_bTransferBlock)
		{
			if (m_szLineBuffer[0] == '\0')
			{
				nErr = QC_ERR_NONE;
				break;
			}
			else
				continue;
		}

		if (m_szLineBuffer[0] == '\0')
		{
			nErr = bIsKeyFound ? QC_ERR_NONE : QC_ERR_FINISH;
			break;
		}

		char* pColonStart = strchr(m_szLineBuffer, ':');
		if (pColonStart == NULL) 
		{
			continue;			
		} 

		char* pEndofkey = pColonStart;

		while ((pEndofkey > m_szLineBuffer) && isspace(pEndofkey[-1])) 
		{
			--pEndofkey;
		}

		char* pStartofValue = pColonStart + 1;
		while (isspace(*pStartofValue)) 
		{
			++pStartofValue;
		}

		*pEndofkey = '\0';

		if (strncmp(m_szLineBuffer, aKey, strlen(aKey)) != 0)
		{
			if(bIsKeyChange){
				if (strncmp(m_szLineBuffer, KTransferEncodingKey, strlen(KTransferEncodingKey)) == 0)
				{
					 m_bTransferBlock = true;
					 m_llContentLength = 0;
				}

				if (strncmp(m_szLineBuffer, KContentType, strlen(KContentType)) == 0)
				{
					char* pSrc = m_szLineBuffer + strlen(KContentType) + 1;
					if(strstr(pSrc, "audio") != NULL || strstr(pSrc, "video") != NULL)  {
						m_bMediaType = true;
					}
				}
			}
			
			continue;
		}		

		if (aBufferLen > (int)strlen(pStartofValue))
		{
            bIsKeyFound = true;
			strcpy(aBuffer, pStartofValue);
		}		
	}

	return nErr;
}

int CHTTPClient::ReceiveLine(char* aLine, int aSize)
{
	if (m_sState != CONNECTED) 
		return QC_ERR_RETRY;

	bool bSawCR = false;
	int nLength = 0;
	char log[2048];
	memset(log, 0, sizeof(char) * 2048);

	while (true) 
	{
		char c;
		int n = Recv(&c, 1);
		if (n <= 0) 
		{
			strncpy(log, aLine, nLength);
			if (n == 0)
			{
				m_nStatusCode = Status_Error_HttpResponseTimeOut;
				return QC_ERR_TIMEOUT;
			}
			else
			{
				return QC_ERR_CANNOT_CONNECT;
			}
		} 

		if (bSawCR && c == '\n') 
		{
			aLine[nLength - 1] = '\0';
			//strncpy(log, aLine, nLength);
			//LOGI("log: %s, logLength: %d", log, nLength);
			return QC_ERR_NONE;
		}

		bSawCR = (c == '\r');

		if (nLength + 1 >= aSize) 
		{
			return QC_ERR_Overflow;
		}

		aLine[nLength++] = c;
	}

	return QC_ERR_NONE;
}

int CHTTPClient::Receive(int& aSocketHandle, timeval& aTimeOut, char* aDstBuffer, int aSize)
{
	int nErr = WaitSocketReadBuffer(aSocketHandle, aTimeOut);
	if (nErr > 0)
	{
		nErr = recv(aSocketHandle, aDstBuffer, aSize, 0);
		if (nErr == 0)
		{
			//server close socket
			nErr = QC_ERR_ServerTerminated;
			QCLOGW ("server closed socket!");
		}
        if (nErr == -1 && errno == _ETIMEDOUT) {
            //network abnormal disconnected
            nErr = QC_ERR_NTAbnormallDisconneted;
 			QCLOGW ("network abnormal disconnected!");
       }
	}
	return nErr;
}

int CHTTPClient::WaitSocketWriteBuffer(int& aSocketHandle, timeval& aTimeOut)
{
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(aSocketHandle, &fds);

	int ret = select(aSocketHandle + 1, NULL, &fds, NULL, &aTimeOut);
	int err = 0;
	int errLength = sizeof(err);

	if (ret > 0 && FD_ISSET(aSocketHandle, &fds))
	{
        getsockopt(aSocketHandle, SOL_SOCKET, SO_ERROR, (char *)&err, (socklen_t*)&errLength);
		if (err != 0)
		{
			SetStatusCode(err + CONNECT_ERROR_BASE);
			ret = -1;
		}
	}
	else if(ret < 0)
	{
		SetStatusCode(errno + CONNECT_ERROR_BASE);
	}

	return (ret > 0) ? QC_ERR_NONE : ((ret == 0) ? QC_ERR_TIMEOUT : QC_ERR_CANNOT_CONNECT);
}

int CHTTPClient::WaitSocketReadBuffer(int& aSocketHandle, timeval& aTimeOut)
{
	fd_set fds;
    int nRet;
    int tryCnt =0;
retry:
	FD_ZERO(&fds);
	FD_SET(aSocketHandle, &fds);

	//select : if error happens, select return -1, detail errorcode is in error
    SetStatusCode(0);
    nRet = select(aSocketHandle + 1, &fds, NULL, NULL, &aTimeOut);
	if (nRet > 0 && !FD_ISSET(aSocketHandle, &fds))
	{
		nRet = 0;
	}
	else if(nRet < 0)
	{
		SetStatusCode(errno + RESPONSE_ERROR_BASE);
        if (StatusCode() == HTTPRESPONSE_INTERUPTER && tryCnt == 0 && IsCancel() == false)
        {
            tryCnt++;
            goto retry;
        }
	}
    
	return nRet;
}

int CHTTPClient::SetSocketTimeOut(int& aSocketHandle, timeval aTimeOut)
{	
	return setsockopt(aSocketHandle, SOL_SOCKET, SO_RCVTIMEO, (char *)&aTimeOut, sizeof(struct timeval));
}

void CHTTPClient::SetSocketBlock(int& aSocketHandle)
{
#ifdef __QC_OS_WIN32__
	u_long non_blk = 0;
	ioctlsocket(aSocketHandle, FIONBIO, &non_blk);
#else
	int flags = fcntl(aSocketHandle, F_GETFL, 0);
	flags &= (~O_NONBLOCK);
	fcntl(aSocketHandle, F_SETFL, flags);
#endif
}

void CHTTPClient::SetSocketNonBlock(int& aSocketHandle)
{
#ifdef __QC_OS_WIN32__
	u_long non_blk = 1;
	ioctlsocket(aSocketHandle, FIONBIO, &non_blk);
#else
	int flags = fcntl(aSocketHandle, F_GETFL, 0);
	fcntl(aSocketHandle, F_SETFL, flags | O_NONBLOCK);
#endif
}