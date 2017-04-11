/*******************************************************************************
	File:		CStockBase.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockBase.h"

#include "CFileIO.h"
#include "ULOgFunc.h"

CStockBase::CStockBase(void)
	: CBaseObject ()
	, m_pHttpUtil (NULL)
	, m_pDataJson (NULL)
{
	SetObjectName ("CStockBase");
	strcpy (m_szHost, "http://stock.market.alicloudapi.com/");
	strcpy (m_szPath, "");
	memset (m_szCode, 0, sizeof (m_szCode));
	memset (m_szName, 0, sizeof (m_szName));
	memset (m_wzName, 0, sizeof (m_wzName));
}

CStockBase::~CStockBase(void)
{
	QC_DEL_P (m_pHttpUtil);
	QC_DEL_P (m_pDataJson);
}

int CStockBase::SetCode (char * pCode)
{
	if (pCode == NULL)
		return QC_ERR_ARG;

	strcpy (m_szCode, pCode);
	int nRC = FillPath ();
	if (nRC != QC_ERR_NONE)
		return nRC;

	return Update ();
}

int CStockBase::Update (void)
{
	int		nRC = QC_ERR_NONE;
	char *	pData = NULL;
	int		nSize = 0;
#ifdef DATA_FROM_NETWORK
	if (m_pHttpUtil == NULL)
		m_pHttpUtil = new CHttpUtil ();
	nRC = m_pHttpUtil->RequestData (m_szPath, &pData);
	if (nRC != QC_ERR_NONE)
	{
		QCLOGW ("m_pHttpUtil->RequestData Error!");
		return nRC;
	}
#else
	CFileIO * pFile = new CFileIO ();
	pFile->Open (m_szPath, 0, QCIO_FLAG_READ);
	nSize = (int)pFile->GetSize ();
	pData = new char[nSize + 4];
	pFile->Read ((unsigned char *)pData, nSize, true, 0);
	*(pData + nSize) = 0;
	pFile->Close ();
	delete pFile;
#endif // DATA_FROM_NETWORK

	if (m_pDataJson == NULL)
		m_pDataJson = new CDataJson ();
	nRC = m_pDataJson->ParseData (pData, nSize);
	if (nRC != QC_ERR_NONE)
	{
		QCLOGW ("m_pDataJson->ParseData Error!");
	}


#ifndef DATA_FROM_NETWORK
	delete []pData;
#endif // DATA_FROM_NETWORK

	return nRC;
}

int CStockBase::FillPath (void)
{
	return QC_ERR_FAILED;
}

char * CStockBase::GetText (CJsonNode * pNode, char * pName)
{
	if (pNode == NULL || pName == NULL)
		return NULL;
	CJsonItem * pItem = FindItem (pNode, pName);
	if (pItem == NULL)
		return NULL;
	return pItem->m_pValue;
}

double CStockBase::GetDblValue (CJsonNode * pNode, char * pName)
{
	if (pNode == NULL || pName == NULL)
		return 0;
	CJsonItem * pItem = FindItem (pNode, pName);
	if (pItem == NULL || pItem->m_pValue == NULL)
		return 0;
	double dValue = atof (pItem->m_pValue);
	return dValue;
}

int CStockBase::GetIntValue (CJsonNode * pNode, char * pName)
{
	if (pNode == NULL || pName == NULL)
		return 0;
	CJsonItem * pItem = FindItem (pNode, pName);
	if (pItem == NULL || pItem->m_pValue == NULL)
		return 0;
	int nValue = atoi (pItem->m_pValue);
	return nValue;
}

CJsonItem *	CStockBase::FindItem (CJsonNode * pNode, char * pName)
{
	CJsonItem * pItem = NULL;
	NODEPOS pos = pNode->m_lstItem.GetHeadPosition ();
	while (pos != NULL)
	{
		pItem = pNode->m_lstItem.GetNext (pos);
		if (!strcmp (pItem->m_szName, pName))
			return pItem;
		if (pItem->m_pChild != NULL)
		{
			pItem = FindItem (pItem->m_pChild, pName);
			if (pItem != NULL)
				return pItem;
		}
	}
	
	CJsonNode * pChildNode = NULL;
	pos = pNode->m_lstNode.GetHeadPosition ();
	while (pos != NULL)
	{
		pChildNode = pNode->m_lstNode.GetNext (pos);
		pItem = FindItem (pChildNode, pName);
		if (pItem != NULL)
			return pItem;
	}
	return NULL;
}

int CStockBase::TranslateName (char * pStrName, wchar_t * pWcsName)
{
	if (pStrName == NULL || pStrName == NULL)
		return QC_ERR_ARG;

	unsigned int len = ::MultiByteToWideChar(CP_UTF8, NULL, pStrName, -1, NULL, NULL);  
	if (len == 0)  
		return 0;  
	::MultiByteToWideChar(CP_UTF8, NULL, pStrName, -1, pWcsName, len);  

    return len;
}

int CStockBase::ReadDataLine (char * pData, int nSize)
{
	if (pData == NULL)
		return QC_ERR_ARG;

	char * pBuff = pData;
	while (pBuff - pData < nSize)
	{
		if (*pBuff == '\r' || *pBuff == '\n')
		{
			pBuff++;
			if (*(pBuff)== '\r' || *(pBuff) == '\n')
				pBuff++;
			break;
		}
		pBuff++;
	}
	return pBuff - pData;
}