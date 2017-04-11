/*******************************************************************************
	File:		CMsgMng.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CMsgMng_H__
#define __CMsgMng_H__

#include "CBaseObject.h"
#include "CThreadWork.h"
#include "CNodeList.h"
#include "CMutexLock.h"

#include "UMsgMng.H"

class CMsgMng : public CBaseObject, public CThreadFunc
{
public:
    CMsgMng(void);
    virtual ~CMsgMng(void);

	virtual int		RegNotify (CMsgReceiver * pReceiver);
	virtual int		RemNotify (CMsgReceiver * pReceiver);

	virtual int		Notify (int nMsg, int nValue, long long llValue);
	virtual int		Notify (int nMsg, int nValue, long long llValue, const char * pValue);
	virtual int		Notify (int nMsg, int nValue, long long llValue, const char * pValue, void * pInfo);

	virtual int		Send (int nMsg, int nValue, long long llValue);
	virtual int		Send (int nMsg, int nValue, long long llValue, const char * pValue);
	virtual int		Send (int nMsg, int nValue, long long llValue, const char * pValue, void * pInfo);

protected:
	virtual	int		ReleaseItem (void);

	virtual int		OnWorkItem (void);

	virtual int		NotifyItem (CMsgItem * pItem);

protected:
	CThreadWork *				m_pThreadWork;

	CMutexLock					m_lckRecv;
	CObjectList<CMsgReceiver>	m_lstReceiver;

	CMutexLock					m_lckItem;
	CObjectList<CMsgItem>		m_lstItemFull;
	CObjectList<CMsgItem>		m_lstItemFree;

	CMutexLock					m_lckNotify;

};

#endif //__CMsgMng_H__
