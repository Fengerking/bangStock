/*******************************************************************************
	File:		CKXTLineInfo.h

	Contains:	the base class of all objects.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-11-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CKXTLineInfo_H__
#define __CKXTLineInfo_H__

#include "CBaseGraphics.h"
#include "CStockKXTInfo.h"

struct stockMACD
{
	double	dEMA12;
	double	dEMA26;
	double	dDIFF;
	double	dDEA;
	double	dBAR;
};

class CKXTLineInfo : public CBaseGraphics
{
public:
	CKXTLineInfo(void);
	virtual ~CKXTLineInfo(void);

	virtual bool	OnCreateWnd (HWND hWnd);
	virtual bool	SetStockKXTInfo (CStockKXTInfo * pKXTInfo);

	virtual bool	UpdateLastItem (CStockKXTInfo * pKXTInfo, bool bNew);

protected:
	virtual bool	ReleaseLineInfo (void);

protected:
	HWND			m_hWnd;
	CStockKXTInfo *	m_pStockKXTInfo;

public:
	double *		m_pLineInfo5;
	double *		m_pLineInfo10;
	double *		m_pLineInfo20;
	double *		m_pLineInfo30;
	double *		m_pLineInfo60;

	stockMACD *		m_pMACD;

};

#endif // __CKXTLineInfo_H__
