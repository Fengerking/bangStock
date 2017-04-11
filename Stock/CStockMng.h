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
	char	szHangQing[256];	// ��Ʊ�������� needIndex ָ���� need_k_pic �Ƿ���Ҫ����k��ͼ��ַ
	strcpy (szHangQing, "http://stock.market.alicloudapi.com/real-stockinfo?code=600895&needIndex=0&need_k_pic=0");
	char	szZhiShu[256];		// ��Ʊָ��
	strcpy (szZhiShu, "http://stock.market.alicloudapi.com/stockIndex");
	char	szRealTime[256];	// ��Ʊʵʱ��ʱ�����ݡ� ���ض�����ķ�ʱ�����ݣ�1����ľ��ǵ��졣Ŀǰ֧��1��5�ķ�Χ��
	strcpy (szRealTime, "http://stock.market.alicloudapi.com/timeline?code=600895&day=1");
	char	szRealTimeK[256];	// ��ƱʵʱK��ͼ. 5 = 5��k��(Ĭ��) ��30 = 30��k�ߣ�60 = 60��k�ߣ�day = ��k�ߣ�week = ��k�ߣ�month = ��k�ߡ�ע��۹ɲ�֧��5�֡�30�ֺ�60��k�ߡ�
								// ��Ȩ��ʽ��֧�����ַ�ʽ �� bfq =����Ȩ(Ĭ�Ϸ�ʽ) qfq =ǰ��Ȩ����timeΪ[day,week,month]ʱ���ֶ���Ч
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