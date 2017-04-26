/*******************************************************************************
	File:		CDataJson.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CDataJson_H__
#define __CDataJson_H__

#include "CBaseObject.h"
#include "CNodeList.h"

class CJsonNode;

class CJsonItem
{
public:
    CJsonItem(void);
    virtual ~CJsonItem(void);

public:
	char			m_szName[128];
	char *			m_pValue;
	CJsonNode *		m_pChild;
};

class CJsonNode
{
public:
    CJsonNode(void);
    virtual ~CJsonNode(void);

	virtual void	AddItem (CJsonItem * pItem) {m_lstItem.AddTail (pItem);}
	virtual void	AddNode (CJsonNode * pNode) {m_lstNode.AddTail (pNode);}

public:
	char					m_szName[128];
	CObjectList<CJsonItem>	m_lstItem;
	CObjectList<CJsonNode>	m_lstNode;	
};

class CDataJson : public CBaseObject
{
public:
    CDataJson(void);
    virtual ~CDataJson(void);

	virtual	int		ParseData (char * pData, int nSize);
	virtual int		FormatData (char ** ppFormatData);

	CJsonNode *		FindNode (char * pName);
	CJsonNode *		GetRootNode (void) {return m_pJsonData;}

protected:
	virtual int		ParseNode (char * pData, CJsonNode * pNode);
	virtual int		ParseList (char * pData, CJsonNode * pNode);
	virtual int		Release (void);

	CJsonNode *		FindNode (CJsonNode * pNode, char * pName);

	virtual int		FormatNode (CJsonNode * pNode, char * pFormat);



protected:
	CJsonNode *		m_pJsonData;

	int				m_nInIndex;
	char *			m_pFormatData;
	int				m_nFormatSize;
	int				m_nFormatUsed;


};

#endif //__CDataJson_H__
