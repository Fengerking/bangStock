/*******************************************************************************
	File:		CMsgMng.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CMsgMng.h"

#include "USystemFunc.h"

CMsgMng::CMsgMng(void)
	: CBaseObject ()
	, m_pThreadWork (NULL)
{
	SetObjectName ("CMsgMng");
}

CMsgMng::~CMsgMng(void)
{
	if (m_pThreadWork != NULL)
	{
		m_pThreadWork->Stop ();
		QC_DEL_P (m_pThreadWork);
	}
	ReleaseItem ();
	CAutoLock lock (&m_lckRecv);
	m_lstReceiver.RemoveAll ();
}

int	CMsgMng::RegNotify (CMsgReceiver * pReceiver)
{
	if (m_pThreadWork == NULL)
	{
		m_pThreadWork = new CThreadWork ();
		m_pThreadWork->SetOwner (m_szObjName);
		m_pThreadWork->SetWorkProc (this, &CThreadFunc::OnWork);
		m_pThreadWork->Start ();
	}
	CAutoLock lock (&m_lckRecv);
	m_lstReceiver.AddTail (pReceiver);
	return QC_ERR_NONE;
}

int	CMsgMng::RemNotify (CMsgReceiver * pReceiver)
{
	CAutoLock lock (&m_lckRecv);
	NODEPOS	pPos = m_lstReceiver.GetHeadPosition ();
	CMsgReceiver * pRecv = m_lstReceiver.GetNext (pPos);
	while (pRecv != NULL)
	{
		if (pRecv == pReceiver)
		{
			m_lstReceiver.Remove (pRecv);
			return QC_ERR_NONE;
		}
		pRecv = m_lstReceiver.GetNext (pPos);
	}
	return QC_ERR_FAILED;
}

int	CMsgMng::Notify (int nMsg, int nValue, long long llValue)
{
	CAutoLock lock (&m_lckItem);
	CMsgItem * pItem = m_lstItemFree.RemoveHead ();
	if (pItem == NULL)
		pItem = new CMsgItem (nMsg, nValue, llValue);
	else
		pItem->SetValue (nMsg, nValue, llValue);
	m_lstItemFull.AddTail (pItem);
	return QC_ERR_NONE;
}

int	CMsgMng::Notify (int nMsg, int nValue, long long llValue, const char * pValue)
{
	CAutoLock lock (&m_lckItem);
	CMsgItem * pItem = m_lstItemFree.RemoveHead ();
	if (pItem == NULL)
		pItem = new CMsgItem (nMsg, nValue, llValue, pValue);
	else
		pItem->SetValue (nMsg, nValue, llValue, pValue);
	m_lstItemFull.AddTail (pItem);
	return QC_ERR_NONE;
}

int	CMsgMng::Notify (int nMsg, int nValue, long long llValue, const char * pValue, void * pInfo)
{
	CAutoLock lock (&m_lckItem);
	CMsgItem * pItem = m_lstItemFree.RemoveHead ();
	if (pItem == NULL)
		pItem = new CMsgItem (nMsg, nValue, llValue, pValue, pInfo);
	else
		pItem->SetValue (nMsg, nValue, llValue, pValue, pInfo);
	m_lstItemFull.AddTail (pItem);
	return QC_ERR_NONE;
}

int	CMsgMng::Send (int nMsg, int nValue, long long llValue)
{
	CMsgItem * pItem = NULL;
	if (pItem == NULL)
	{
		CAutoLock lock (&m_lckItem);
		pItem = m_lstItemFree.RemoveHead ();
		if (pItem == NULL)
			pItem = new CMsgItem (nMsg, nValue, llValue);
		else
			pItem->SetValue (nMsg, nValue, llValue);
	}
	return NotifyItem (pItem);
}

int	CMsgMng::Send (int nMsg, int nValue, long long llValue, const char * pValue)
{
	CMsgItem * pItem = NULL;
	if (pItem == NULL)
	{
		CAutoLock lock (&m_lckItem);
		pItem = m_lstItemFree.RemoveHead ();
		if (pItem == NULL)
			pItem = new CMsgItem (nMsg, nValue, llValue, pValue);
		else
			pItem->SetValue (nMsg, nValue, llValue, pValue);
	}
	return NotifyItem (pItem);
}

int	CMsgMng::Send (int nMsg, int nValue, long long llValue, const char * pValue, void * pInfo)
{
	CMsgItem * pItem = NULL;
	if (pItem == NULL)
	{
		CAutoLock lock (&m_lckItem);
		CMsgItem * pItem = m_lstItemFree.RemoveHead ();
		if (pItem == NULL)
			pItem = new CMsgItem (nMsg, nValue, llValue, pValue, pInfo);
		else
			pItem->SetValue (nMsg, nValue, llValue, pValue, pInfo);
	}
	return NotifyItem (pItem);
}

int	CMsgMng::NotifyItem (CMsgItem * pItem)
{
	CAutoLock lockNotify (&m_lckNotify);
	CMsgReceiver *	pRecv = NULL;
	NODEPOS			pPos = m_lstReceiver.GetHeadPosition ();
	while (pPos != NULL)
	{
		if (pRecv == NULL)
		{
			CAutoLock lockRecv (&m_lckRecv);
			pRecv = m_lstReceiver.GetNext (pPos);
			if (pRecv == NULL)
				break;
		}
		pRecv->ReceiveMsg (pItem);
		pRecv = NULL;
	}
	CAutoLock lock (&m_lckItem);
	m_lstItemFree.AddTail (pItem);
	return QC_ERR_NONE;
}

int	CMsgMng::ReleaseItem (void)
{
	CAutoLock lock (&m_lckItem);
	CMsgItem * pItem = m_lstItemFree.RemoveHead ();
	while (pItem != NULL)
	{
		delete pItem;
		pItem = m_lstItemFree.RemoveHead ();
	}
	pItem = m_lstItemFull.RemoveHead ();
	while (pItem != NULL)
	{
		delete pItem;
		pItem = m_lstItemFree.RemoveHead ();
	}
	return QC_ERR_NONE;
}

int CMsgMng::OnWorkItem (void)
{
	if (m_lstItemFull.GetCount () <= 0)
	{
		qcSleep (5000);
		return QC_ERR_NONE;
	}

	CMsgItem * pItem = NULL;
	while (true)
	{
		if (pItem == NULL)
		{
			CAutoLock lock (&m_lckItem);
			pItem = m_lstItemFull.RemoveHead ();
			if (pItem == NULL)
				break;
		}
		NotifyItem (pItem);
		pItem = NULL;
	}
	return QC_ERR_NONE;
}