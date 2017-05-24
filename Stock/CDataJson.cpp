/*******************************************************************************
	File:		CDataJson.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CDataJson.h"


CDataJson::CDataJson(void)
	: CBaseObject ()
	, m_pJsonData (NULL)
	, m_nInIndex (0)
	, m_pFormatData (NULL)
	, m_nFormatSize (0)
	, m_nFormatUsed (0)
{
	SetObjectName ("CDataJson");
	m_nFormatSize = 1024 * 1024 * 2;
}

CDataJson::~CDataJson(void)
{
	Release ();
}

int CDataJson::ParseData (char * pData, int nSize)
{
	if (pData == NULL || *pData != '{')
		return QC_ERR_FAILED;

	Release ();

	m_pJsonData = new CJsonNode ();
	ParseNode (pData, m_pJsonData);

	return QC_ERR_NONE;
}

CJsonNode *	CDataJson::FindNode (char * pName)
{
	if (m_pJsonData == NULL)
		return NULL;
	return FindNode (m_pJsonData, pName);
}

CJsonNode *	CDataJson::FindNode (CJsonNode * pNode, char * pName)
{
	CJsonNode * pFindNode = NULL;
	CJsonItem * pItem = NULL;
	NODEPOS pos = pNode->m_lstItem.GetHeadPosition ();
	while (pos != NULL)
	{
		pItem = pNode->m_lstItem.GetNext (pos);
		if (!strcmp (pItem->m_szName, pName) && (pItem->m_pChild != NULL))
		{
			return pItem->m_pChild;
		}
		if (pItem->m_pChild != NULL)
		{
			pFindNode = FindNode (pItem->m_pChild, pName);
			if (pFindNode != NULL)
				return pFindNode;
		}
	}
	pos = pNode->m_lstNode.GetHeadPosition ();
	while (pos != NULL)
	{
		pFindNode = pNode->m_lstNode.GetNext (pos);
		pFindNode = FindNode (pFindNode, pName);
		if (pFindNode != NULL)
			return pFindNode;
	}
	return NULL;
}

int CDataJson::ParseNode (char * pData, CJsonNode * pNode)
{
	int		nUsed = 0;
	int		nLen = strlen (pData);
	char *	pPos = pData;
	char *	pNext = pData;
	int		nUsedSize = 0;

	if (pPos == NULL || *pPos != '{')
		return 0;
	while (pPos - pData < nLen)
	{
		while (*pPos != '"')
		{
			pPos++;
			if (pPos - pData >= nLen)
				return 0;
		}
		pPos++;
		pNext = pPos;
		while (*pNext != '"')
		{
			pNext++;
			if (pNext - pData >= nLen)
				return 0;
		}
		CJsonItem * pItem = new CJsonItem ();
		pNode->AddItem (pItem);
		if (pNext - pPos > 0)
			strncpy (pItem->m_szName, pPos, pNext - pPos);
		pPos = pNext + 1;
		if (*pPos != ':')
			return 0;
		while (*pPos++ == ' ');
		pNext = pPos + 1;
		if (*pPos == '{')
		{
			pItem->m_pChild = new CJsonNode ();
			nUsedSize =  ParseNode (pPos, pItem->m_pChild);
			pPos += nUsedSize;
		}
		else if (*pPos == '[')
		{
			pItem->m_pChild = new CJsonNode ();
			nUsedSize =  ParseList (pPos, pItem->m_pChild);
			pPos += nUsedSize;
		}
		else
		{
			if (*pPos == '"')
			{
				pPos++;
				pNext = pPos;
				while (*pNext != '"')
				{
					pNext++;
					if (pNext - pData >= nLen)
						return 0;
				}
			}
			else
			{
				while (*pNext != ',')
				{
					pNext++;
					if (pNext - pData >= nLen)
						return 0;
				}
			}
			pItem->m_pValue = new char[pNext - pPos + 2];
			memset (pItem->m_pValue, 0, (pNext - pPos) + 2);
			if (pNext - pPos > 0)
				strncpy (pItem->m_pValue, pPos, pNext - pPos);
			pPos = pNext+1;
		}
		if (*pPos == '}')
		{
			pPos++;
			return pPos - pData;
		}

	}
	return pPos - pData;
}

int CDataJson::ParseList (char * pData, CJsonNode * pNode)
{
	char *	pPos = pData;
	int		nLen = strlen (pData);
	int		nUsedSize = 0;
	if (*pPos != '[')
		return 0;
	pPos++;
	while (pPos - pData < nLen)
	{
		CJsonNode * pNewNode = new CJsonNode ();
		pNode->AddNode (pNewNode);
		nUsedSize = ParseNode (pPos, pNewNode);
		if (nUsedSize <= 0)
			break;
		pPos += nUsedSize;
		if (*pPos == ']')
		{
			pPos++;
			break;
		}
		if (*pPos == ',')
			pPos++;
	}
	return pPos - pData;
}

int CDataJson::FormatData (char ** ppFormatData)
{
	if (m_pFormatData == NULL)
		m_pFormatData = new char[m_nFormatSize];
	memset (m_pFormatData, 0, m_nFormatSize);
	m_nInIndex = 0;
	FormatNode (m_pJsonData, m_pFormatData);
	m_nFormatUsed = strlen (m_pFormatData);
	*ppFormatData = m_pFormatData;
	return m_nFormatUsed;
}

int CDataJson::FormatNode (CJsonNode * pNode, char * pFormat)
{
	if (pNode == NULL || pFormat == NULL)
		return 0;
	int			nUsedSize = 0;
	char *		pPos = pFormat;
	char		szSpace[128];
	memset (szSpace, 0, sizeof (szSpace));
	for (int i = 0; i < m_nInIndex; i++)
		memset (szSpace + i * 4, ' ', 4);
	strcat (pPos, szSpace);
	pPos += strlen (szSpace);
	strcat (pPos, "{\r\n");
	pPos += 3;

	m_nInIndex++;
	CJsonItem * pItem = NULL;
	NODEPOS pos = pNode->m_lstItem.GetHeadPosition ();
	while (pos != NULL)
	{
		pItem = pNode->m_lstItem.GetNext (pos);
		strcat (pPos, szSpace);
		pPos += strlen (szSpace);
		strcat (pPos, "    ");
		pPos += 4;
		strcat (pPos, pItem->m_szName);
		pPos += strlen (pItem->m_szName);
		*pPos++ = ':';
		if (pItem->m_pValue != NULL)
		{
			strcat (pPos, pItem->m_pValue);
			pPos += strlen (pItem->m_pValue);
			strcat (pPos, "\r\n");
			pPos += 2;
		}
		else
		{
			if (pItem->m_pChild->m_lstNode.GetCount () > 0)
			{
				strcat (pPos, " [\r\n");
				pPos += 4;
			}
			else
			{
				strcat (pPos, "\r\n");
				pPos += 2;
			}

			nUsedSize = FormatNode (pItem->m_pChild, pPos);
			pPos += nUsedSize;
		}
	}

	CJsonNode * pSubNode = NULL;
	pos = pNode->m_lstNode.GetHeadPosition ();
	while (pos != NULL)
	{
		pSubNode = pNode->m_lstNode.GetNext (pos);
		nUsedSize = FormatNode (pSubNode, pPos);
		pPos += nUsedSize;
	}

	strcat (pPos, szSpace);
	pPos += strlen (szSpace);
	strcat (pPos, "}\r\n");
	pPos += 3;
	if (pNode->m_lstNode.GetCount () > 0)
	{
		strcat (pPos, szSpace);
		pPos += strlen (szSpace);
		strcat (pPos, "]\r\n");
		pPos += 3;
	}

	m_nInIndex--;
	return pPos - pFormat;
}

int CDataJson::Release (void)
{
	QC_DEL_P (m_pJsonData);
	QC_DEL_A (m_pFormatData);
	return QC_ERR_NONE;
}

CJsonItem::CJsonItem(void)
	: m_pValue (NULL)
	, m_pChild (NULL)
{
	memset (m_szName, 0, sizeof (m_szName));
}

CJsonItem::~CJsonItem(void)
{
}

CJsonNode::CJsonNode(void)
{
	memset (m_szName, 0, sizeof (m_szName));
}

CJsonNode::~CJsonNode(void)
{
	CJsonItem * pItem = m_lstItem.RemoveHead ();
	while (pItem != NULL)
	{
		QC_DEL_A (pItem->m_pValue);
		QC_DEL_P (pItem->m_pChild);
		delete pItem;
		pItem = m_lstItem.RemoveHead ();
	}
	CJsonNode * pNode = m_lstNode.RemoveHead ();
	while (pNode != NULL)
	{
		delete pNode;
		pNode = m_lstNode.RemoveHead ();
	}
}
