/*******************************************************************************
	File:		CDlgDownLoad.h

	Contains:	Open URL dialog header file

	Written by:	Fenger King

	Change History (most recent first):
	2013-10-21		Fenger			Create file

*******************************************************************************/
#ifndef __CDlgDownLoad_H__
#define __CDlgDownLoad_H__

#include "CStockFileCode.h"

#include "CStockFileFHSP.h"
#include "CStockFileFinance.h"
#include "CStockFileCompInfo.h"
#include "CStockFileHYGN.h"

#define		WM_TIMER_DOWNLOAD_TODAY		201
#define		WM_TIMER_DOWNLOAD_HISTORY	202

#define		WM_TIMER_UPDATE_COMPINFO	205

#define		BS_UPDATE_FHSP				1
#define		BS_UPDATE_COMPINFO			2
#define		BS_UPDATE_FINANCE			3
#define		BS_UPDATE_HYGN				4

class CDlgDownLoad
{
public:
	CDlgDownLoad (HINSTANCE hInst, HWND hParent);
	virtual ~CDlgDownLoad(void);

	int			OpenDlg (void);

protected:
	int			StartUpdate (int nType);
	int			OnTimerUpdate (void);

	int			StartDownLoad (int nType);

	int			ProcessDownLoadToday (void);
	int			ProcessDownLoadHistory (void);

	int			DownLoadFinish (void);
	int			Prepare (void);

protected:
	HINSTANCE				m_hInst;
	HWND					m_hParent;
	HWND					m_hDlg;

	HWND					m_hEdtInfo;
	HWND					m_hEdtResult;
	HWND					m_hProgress;
	char					m_szStartDate[32];

	int						m_nTimerDownLoad;

	CStockFileCode *		m_pCodeList;
	int						m_nCodeIndex;
	int						m_nCodeCount;
	int						m_nCodeStep;
	char					m_szCodeList[2048];
	char					m_szCodeInfo[2048];

	char *					m_pResultErr;
	char *					m_pResultLog;
	int						m_nOffsetHour;

	CStockRTList *			m_pRTInfoList;
	CStockHistInfo *		m_pHistInfo;

	CStockFileFHSP *		m_pStockFHSP;
	CStockFileFinance *		m_pStockFinance;
	CStockFileCompInfo *	m_pStockCompInfo;
	CStockFileHYGN *		m_pStockHYGN;
	int						m_nUpdateType;

public:
	static INT_PTR CALLBACK DownLoadDlgProc (HWND, UINT, WPARAM, LPARAM);
};
#endif //__CDlgDownLoad_H__

