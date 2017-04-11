/*******************************************************************************
	File:		CStockMng.h

	Contains:	the message manager class header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-01		Bangfei			Create file

*******************************************************************************/
#ifndef __CStockMng_H__
#define __CStockMng_H__

#include "CBaseObject.h"
#include "CStockFileBuy.h"

class CStockMng : public CBaseObject
{
public:
    CStockMng(void);
    virtual ~CStockMng(void);

	virtual int			Init (HWND hWnd);

protected:
	HWND				m_hMainWnd;
	CStockFileBuy *		m_pBuyInfo;


};

#endif //__CStockMng_H__




/*
	if (m_pHttpUtil == NULL)
		m_pHttpUtil = new CHttpUtil ();

	char * pData = NULL;
	char	szHangQing[256];	// 股票行情数据 needIndex 指数， need_k_pic 是否需要返回k线图地址
	strcpy (szHangQing, "http://stock.market.alicloudapi.com/real-stockinfo?code=600895&needIndex=0&need_k_pic=0");
	char	szZhiShu[256];		// 股票指数
	strcpy (szZhiShu, "http://stock.market.alicloudapi.com/stockIndex");
	char	szRealTime[256];	// 股票实时分时线数据。 返回多少天的分时线数据，1代表的就是当天。目前支持1至5的范围。
	strcpy (szRealTime, "http://stock.market.alicloudapi.com/timeline?code=600895&day=1");
	char	szRealTimeK[256];	// 股票实时K线图. 5 = 5分k线(默认) ，30 = 30分k线，60 = 60分k线，day = 日k线，week = 周k线，month = 月k线。注意港股不支持5分、30分和60分k线。
								// 复权方式，支持两种方式 。 bfq =不复权(默认方式) qfq =前复权。当time为[day,week,month]时此字段有效
	strcpy (szRealTimeK, "http://stock.market.alicloudapi.com/realtime-k?beginDay=20161101&code=600895&time=day&type=bfq");

	m_pHttpUtil->RequestData (szHangQing, &pData);

	CFileIO * pFile = new CFileIO ();
	pFile->Open ("c:\\work\\Temp\\szHangQing.txt", 0, QCIO_FLAG_WRITE);
	pFile->Write ((unsigned char *)pData, strlen (pData));
	pFile->Close ();
	delete pFile;
*/


/*
	pFile->Open ("c:\\work\\Temp\\RealTimeK_00.txt", 0, QCIO_FLAG_WRITE);
	char * pFormat = NULL;
	m_pDataJson->FormatData (&pFormat);
	pFile->Write ((unsigned char *)pFormat, strlen (pFormat));
	pFile->Close ();
*/