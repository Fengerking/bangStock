/*******************************************************************************
	File:		CStockMng.cpp

	Contains:	message manager class implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#include "qcErr.h"
#include "CStockMng.h"

#include "CStockRTInfo.h"

#include "CFileIO.h"

CStockMng::CStockMng(void)
	: CBaseObject ()
	, m_hMainWnd (NULL)
	, m_pBuyInfo (NULL)
{
	SetObjectName ("CStockMng");
}

CStockMng::~CStockMng(void)
{
	QC_DEL_P (m_pBuyInfo);
}

int CStockMng::Init (HWND hWnd)
{
	m_hMainWnd = hWnd;

	m_pBuyInfo = new CStockFileBuy ();
	m_pBuyInfo->Open ("codeBuy.txt");

	return QC_ERR_NONE;

}

