/*******************************************************************************
	File:		CVideoRender.cpp

	Contains:	file info dialog implement code

	Written by:	Fenger King

	Change History (most recent first):
	2013-04-01		Fenger			Create file

*******************************************************************************/
#include "windows.h"
#include "commctrl.h"
#include "qcStock.h"

#include "CStockRTList.h"
#include "CStockHistInfo.h"

#include "CDlgDownLoad.h"
#include "Resource.h"

#include "USystemFunc.h"

CDlgDownLoad::CDlgDownLoad(HINSTANCE hInst, HWND hParent)
	: m_hInst (hInst)
	, m_hParent (hParent)
	, m_hDlg (NULL)
	, m_hEdtInfo (NULL)
	, m_hEdtResult (NULL)
	, m_hProgress (NULL)
	, m_nTimerDownLoad (0)
	, m_pCodeList (NULL)
	, m_nCodeIndex (0)
	, m_nCodeCount (0)
	, m_nCodeStep (88)
	, m_nOffsetHour (8)
	, m_pHistInfo (NULL) 
	, m_pRTInfoList (NULL)
{
	m_pResultErr = new char[1024 * 512];
	m_pResultLog = new char[1024 * 1024];
	SYSTEMTIME tmSys;
	SYSTEMTIME tmLcl;
	GetSystemTime (&tmSys);
	GetLocalTime (&tmLcl);
	m_nOffsetHour = tmLcl.wDay * 24 + tmLcl.wHour - (tmSys.wDay * 24 + tmSys.wHour);

	strcpy (m_szStartDate, "2017-02-21");
}

CDlgDownLoad::~CDlgDownLoad(void)
{
	QC_DEL_P (m_pCodeList);
	QC_DEL_A (m_pResultErr);
	QC_DEL_A (m_pResultLog);
	QC_DEL_P (m_pRTInfoList);
	QC_DEL_P (m_pHistInfo);
}

int CDlgDownLoad::OpenDlg (void)
{
	int nRC = DialogBoxParam (m_hInst, MAKEINTRESOURCE(IDD_DIALOG_DOWNLOAD), m_hParent, DownLoadDlgProc, (LPARAM)this);
	return nRC;
}

int CDlgDownLoad::StartDownLoad (int nType)
{
	if (nType == 1)
	{
		memset (m_szStartDate, 0, sizeof (m_szStartDate));
		GetWindowText (GetDlgItem (m_hDlg, IDC_EDIT_STARTDATE), m_szStartDate, sizeof (m_szStartDate));
		if (strlen (m_szStartDate) != 10)
		{
			MessageBox (m_hDlg, "Please set the correct start date first!   Like 2010-10-10.", "Error", MB_OK);
			return 0;
		}
	}
	char		szFile[256];
	FILETIME	tmFile;
	SYSTEMTIME	tmSys;
	SYSTEMTIME	tmNow;
	HANDLE		hFile = INVALID_HANDLE_VALUE;
	qcGetAppPath (NULL, szFile, sizeof (szFile));
	strcat (szFile, "data\\history\\sh600000.txt");
	hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, (DWORD) 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		GetFileTime (hFile, NULL, NULL, &tmFile);
		CloseHandle (hFile);
		FileTimeToSystemTime (&tmFile, &tmSys);
		tmSys.wHour += m_nOffsetHour;
		GetLocalTime (&tmNow);
		if (tmSys.wDay == tmNow.wDay)
		{
			if (tmSys.wHour > 15)
			{
				if (MessageBox (m_hDlg, "It looks that had downloaded today. Continue?", "Information", MB_YESNO) != IDYES)
					return -1;
			}
		}
	}

	if (m_pCodeList == NULL)
	{
		m_pCodeList = new CStockFileCode ();
		m_pCodeList->Open ("codeList.txt");
		m_nCodeIndex = 0;
		m_nCodeCount = m_pCodeList->GetCodeCount ();

		SendMessage (m_hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, m_nCodeCount)); 
		SendMessage (m_hProgress, PBM_SETSTEP, (WPARAM) 1, 0); 
		SendMessage (m_hProgress, PBM_SETPOS, (WPARAM) 0, 0); 
	}
	strcpy (m_pResultErr, "");
	strcpy (m_pResultLog, "");
	EnableWindow (GetDlgItem (m_hDlg, IDC_BUTTON_TODAY), FALSE);
	EnableWindow (GetDlgItem (m_hDlg, IDC_BUTTON_HISTORY), FALSE);
	if (nType == 0)
	{
		if (m_pRTInfoList == NULL)
			m_pRTInfoList = new CStockRTList ();
		m_nTimerDownLoad = SetTimer (m_hDlg, WM_TIMER_DOWNLOAD_TODAY, 10, NULL);
	}
	else
	{
		if (m_pHistInfo == NULL)
			m_pHistInfo = new CStockHistInfo ();
		m_nTimerDownLoad = SetTimer (m_hDlg, WM_TIMER_DOWNLOAD_HISTORY, 10, NULL);
	}
	
	return 0;
}

int CDlgDownLoad::ProcessDownLoadToday (void)
{
	char *			pCode = NULL;
	char			szCode[32];
	memset (m_szCodeList, 0, sizeof (m_szCodeList));
	memset (m_szCodeInfo, 0, sizeof (m_szCodeInfo));
	for (int i = m_nCodeIndex; i < m_nCodeIndex + m_nCodeStep; i++)
	{
		if (i >= m_nCodeCount)
			break;
		pCode = m_pCodeList->GetCodeNum (i);
		if (pCode[0] == '6' || pCode[0] == '9')
			sprintf (szCode, "sh%s", pCode);
		else
			sprintf (szCode, "sz%s", pCode);
		if (i > m_nCodeIndex)
			strcat (m_szCodeList, "%2C");
		strcat (m_szCodeList, szCode);

		strcat (m_szCodeInfo, pCode);
		strcat (m_szCodeInfo, "  ");
		if (((i - m_nCodeIndex + 1) % 8) == 0)
			strcat (m_szCodeInfo, "\r\n");

	}
	m_nCodeIndex += m_nCodeStep;
	SetWindowText (m_hEdtInfo, m_szCodeInfo);

	int nRC = m_pRTInfoList->SetCode (m_szCodeList);
	if (nRC != QC_ERR_NONE)
		return nRC;

	char		szLine[256];
	char		szFile[256];
	DWORD		dwWrite = 0;
	FILETIME	tmFile;
	SYSTEMTIME	tmSys;
	SYSTEMTIME	tmNow;
	HANDLE		hFile = INVALID_HANDLE_VALUE;
	GetLocalTime (&tmNow);

	qcStockRealTimeItem *	pItem = NULL;
	NODEPOS					pos = m_pRTInfoList->m_lstItem.GetTailPosition ();
	while (pos != NULL)
	{
		pItem = m_pRTInfoList->m_lstItem.GetPrev (pos);
		if (pItem->m_dOpenPrice == 0)
		{
			strcat (m_pResultErr, pItem->m_szCode);
			strcat (m_pResultErr, "  Stop \r\n");
			strcat (m_pResultLog, pItem->m_szCode);
			strcat (m_pResultLog, "  Stop \r\n");
			continue;
		}

		qcGetAppPath (NULL, szFile, sizeof (szFile));
		if (pItem->m_szCode[0] == '6' || pItem->m_szCode[0] == '9')
			sprintf (szFile, "%sdata\\history\\sh%s.txt", szFile, pItem->m_szCode);
		else 
			sprintf (szFile, "%sdata\\history\\sz%s.txt", szFile, pItem->m_szCode);

		hFile = CreateFile(szFile, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, (DWORD) 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			strcat (m_pResultErr, pItem->m_szCode);
			strcat (m_pResultErr, "  Open file failed. \r\n");
			strcat (m_pResultLog, pItem->m_szCode);
			strcat (m_pResultLog, "  Open file failed. \r\n");
			continue;
		}
		GetFileTime (hFile, NULL, NULL, &tmFile);
		FileTimeToSystemTime (&tmFile, &tmSys);
		tmSys.wHour += m_nOffsetHour;
		if (tmSys.wDay == tmNow.wDay)
		{
			if (tmSys.wHour > 15)
			{
				CloseHandle (hFile);
				strcat (m_pResultErr, pItem->m_szCode);
				strcat (m_pResultErr, "  had updated today. \r\n");
				strcat (m_pResultLog, pItem->m_szCode);
				strcat (m_pResultLog, "  had updated today. \r\n");
				continue;
			}
		}

		sprintf (szLine, "%s,%.2f,%.2f,%.2f,%.2f,%d,%d,%.2f,%.2f,%.2f,%.2f\r\n",
						 pItem->m_szDate, pItem->m_dOpenPrice, pItem->m_dNowPrice, pItem->m_dMaxPrice, pItem->m_dMinPrice,
						 pItem->m_nTradeNum/100, pItem->m_nTradeMoney, pItem->m_dDiffNum, pItem->m_dDiffRate, pItem->m_dSwing, pItem->m_dTurnOver);
		dwWrite = strlen (szLine);
		SetFilePointer (hFile, 0, NULL, FILE_END);
		WriteFile (hFile, szLine, strlen (szLine), &dwWrite, NULL);
		CloseHandle (hFile);
		if (dwWrite != strlen (szLine))
		{
			strcat (m_pResultErr, pItem->m_szCode);
			strcat (m_pResultErr, "  write file failed. \r\n");
			strcat (m_pResultLog, pItem->m_szCode);
			strcat (m_pResultLog, "  write file failed. \r\n");
		}
		else
		{
			strcat (m_pResultLog, pItem->m_szCode);
			strcat (m_pResultLog, "  OK.\r\n");
		}
	}

	SetWindowText (m_hEdtResult, m_pResultErr);
	SendMessage (m_hProgress, PBM_SETPOS, (WPARAM) m_nCodeIndex, 0); 

	if (m_nCodeIndex >= m_nCodeCount)
		DownLoadFinish ();
	else
		m_nTimerDownLoad = SetTimer (m_hDlg, WM_TIMER_DOWNLOAD_TODAY, 1000, NULL);
	
	return QC_ERR_NONE;
}

int CDlgDownLoad::ProcessDownLoadHistory (void)
{
	char		szCode[32];
	char		szInfo[32];
	char		szLine[256];
	char		szFile[256];
	DWORD		dwWrite = 0;
	FILETIME	tmFile;
	SYSTEMTIME	tmSys;
	SYSTEMTIME	tmNow;
	HANDLE		hFile = INVALID_HANDLE_VALUE;

	strcpy (szCode, m_pCodeList->GetCodeNum (m_nCodeIndex)); 
	sprintf (szInfo, "Code = %s, Index = % 8d", szCode, m_nCodeIndex);
	SetWindowText (m_hEdtInfo, szInfo);

	qcGetAppPath (NULL, szFile, sizeof (szFile));
	if (szCode[0] == '6' || szCode[0] == '9')
		sprintf (szFile, "%sdata\\history\\sh%s.txt", szFile, szCode);
	else 
		sprintf (szFile, "%sdata\\history\\sz%s.txt", szFile, szCode);
	hFile = CreateFile(szFile, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, (DWORD) 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		strcpy (m_pResultErr, szCode);
		strcat (m_pResultErr, "  Open file failed. \r\n");
		strcat (m_pResultLog, szCode);
		strcat (m_pResultLog, "  Open file failed. \r\n");
		return -1;
	}
	GetFileTime (hFile, NULL, NULL, &tmFile);
	FileTimeToSystemTime (&tmFile, &tmSys);
	tmSys.wHour += m_nOffsetHour;
	GetLocalTime (&tmNow);
	if (tmSys.wDay == tmNow.wDay)
	{
		if (tmSys.wHour > 15)
		{
			CloseHandle (hFile);
			strcpy (m_pResultErr, szCode);
			strcat (m_pResultErr, "  had updated today. \r\n");
			strcat (m_pResultLog, szCode);
			strcat (m_pResultLog, "  had updated today. \r\n");
			return -1;
		}
	}
	SetFilePointer (hFile, 0, NULL, FILE_END);

	m_pHistInfo->SetDate (m_szStartDate, NULL);
	if (m_pHistInfo->SetCode (szCode) != QC_ERR_NONE)
	{
		CloseHandle (hFile);
		strcat (m_pResultLog, szCode);
		strcat (m_pResultLog, "  SetCode Failed. \r\n");
		return 0;
	}
	if (m_pHistInfo->m_lstItem.GetCount () <= 0)
	{
		CloseHandle (hFile);
		strcat (m_pResultLog, szCode);
		strcat (m_pResultLog, "  List Empty. \r\n");
		return 0;
	}

	qcStockKXTInfoItem * pItem = NULL;
	NODEPOS pos = m_pHistInfo->m_lstItem.GetHeadPosition ();
	while (pos != NULL)
	{
		pItem = m_pHistInfo->m_lstItem.GetNext (pos);
		sprintf (szLine, "%d-%02d-%02d,%.2f,%.2f,%.2f,%.2f,%d,%d,%.2f,%.2f,%.2f,%.2f\r\n",
						 pItem->m_nYear, pItem->m_nMonth, pItem->m_nDay, pItem->m_dOpen, pItem->m_dClose, pItem->m_dMax, pItem->m_dMin,
						 pItem->m_nVolume, pItem->m_nMoney, pItem->m_dDiffNum, pItem->m_dDiffRate, pItem->m_dSwing, pItem->m_dExchange);
		WriteFile (hFile, szLine, strlen (szLine), &dwWrite, NULL);
	}
	CloseHandle (hFile);
	strcat (m_pResultLog, szCode);
	strcat (m_pResultLog, "  OK. \r\n");
	strcpy (m_pResultErr, szCode);
	strcat (m_pResultErr, "  OK. \r\n");

	SetWindowText (m_hEdtResult, m_pResultLog);
	SendMessage (m_hProgress, PBM_SETPOS, (WPARAM) m_nCodeIndex, 0); 

	return QC_ERR_NONE;
}

int CDlgDownLoad::DownLoadFinish (void)
{
	EnableWindow (GetDlgItem (m_hDlg, IDC_BUTTON_TODAY), TRUE);
	EnableWindow (GetDlgItem (m_hDlg, IDC_BUTTON_HISTORY), TRUE);

	char		szFile[256];
	SYSTEMTIME	tmNow;
	HANDLE		hFile = INVALID_HANDLE_VALUE;
	DWORD		dwWrite = 0;
	GetLocalTime (&tmNow);	
	qcGetAppPath (NULL, szFile, sizeof (szFile));
	sprintf (szFile, "%sdata\\log\\dl_%d_%02d_%02d_%02d_%02d_%02d.txt", szFile, 
						tmNow.wYear, tmNow.wMonth, tmNow.wDay, tmNow.wHour, tmNow.wMinute, tmNow.wSecond);
	hFile = CreateFile(szFile, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, (DWORD) 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		WriteFile (hFile, m_pResultLog, strlen (m_pResultLog), &dwWrite, NULL);
		CloseHandle (hFile);
	}
	MessageBox (m_hDlg, "Download finished!", "Information...", MB_OK);

	return 0;
}

INT_PTR CALLBACK CDlgDownLoad::DownLoadDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int				wmId, wmEvent;
	RECT			rcDlg;
	CDlgDownLoad *	pDlgDown = NULL;

	if (hDlg != NULL)
	{
		GetClientRect (hDlg, &rcDlg);
		pDlgDown = (CDlgDownLoad *)GetWindowLong (hDlg, GWL_USERDATA);
	}

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		SetWindowLong (hDlg, GWL_USERDATA, lParam);
		pDlgDown = (CDlgDownLoad *)lParam;
		pDlgDown->m_hDlg = hDlg;
		pDlgDown->m_hEdtInfo = GetDlgItem (hDlg, IDC_EDIT_INFO);
		pDlgDown->m_hEdtResult = GetDlgItem (hDlg, IDC_EDIT_RESULT);
		pDlgDown->m_hProgress = GetDlgItem (hDlg, IDC_PROGRESS_DOWNLOAD);

		SetWindowText (GetDlgItem (hDlg, IDC_EDIT_STARTDATE), pDlgDown->m_szStartDate);

		int	nScreenX = GetSystemMetrics (SM_CXSCREEN);
		int	nScreenY = GetSystemMetrics (SM_CYSCREEN);
		SetWindowPos (hDlg, HWND_BOTTOM, (nScreenX - rcDlg.right) / 2, (nScreenY - rcDlg.bottom) / 2 - 100 , rcDlg.right, rcDlg.bottom, 0);

		return (INT_PTR)TRUE;
	}

	case WM_TIMER:
		if (pDlgDown != NULL)
		{
			if (pDlgDown->m_nTimerDownLoad != 0)
				KillTimer (pDlgDown->m_hDlg, pDlgDown->m_nTimerDownLoad);
			pDlgDown->m_nTimerDownLoad = 0;

			if (wParam == WM_TIMER_DOWNLOAD_TODAY)
			{
				pDlgDown->ProcessDownLoadToday ();
			}
			else
			{
				int nRC = pDlgDown->ProcessDownLoadHistory ();
				pDlgDown->m_nCodeIndex++;
				if (pDlgDown->m_nCodeIndex >= pDlgDown->m_nCodeCount)
					pDlgDown->DownLoadFinish ();
				else
				{
					SetWindowText (pDlgDown->m_hEdtInfo, pDlgDown->m_pResultErr);
					if (nRC < 0)
						pDlgDown->m_nTimerDownLoad = SetTimer (hDlg, WM_TIMER_DOWNLOAD_HISTORY, 5, NULL);
					else
						pDlgDown->m_nTimerDownLoad = SetTimer (hDlg, WM_TIMER_DOWNLOAD_HISTORY, 2000, NULL);
				}
			}
		}
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		// Parse the menu selections:
		switch (wmId)
		{
		case IDC_BUTTON_TODAY:
			pDlgDown->StartDownLoad (0);
			break;
		case IDC_BUTTON_HISTORY:
			pDlgDown->StartDownLoad (1);
			break;

		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}

	return (INT_PTR)FALSE;
}
