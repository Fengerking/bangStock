/*******************************************************************************
	File:		CVideoRender.cpp

	Contains:	file info dialog implement code

	Written by:	Fenger King

	Change History (most recent first):
	2013-04-01		Fenger			Create file

*******************************************************************************/
#include "windows.h"
#include "commctrl.h"
#include "qcErr.h"
#include "qcStock.h"
#include "CFileIO.h"

#include "CDlgStockInfo.h"
#include "CStockFileHist.h"

#include "Resource.h"

#include "USystemFunc.h"

CDlgStockInfo::CDlgStockInfo(HINSTANCE hInst, HWND hParent)
	: CDlgBase (hInst, hParent)
{

}

CDlgStockInfo::~CDlgStockInfo(void)
{

}

int CDlgStockInfo::OpenDlg (char * pCode)
{
	strcpy (m_szCode, pCode);
	CDlgBase::OpenDlg ();
	return DialogBoxParam (m_hInst, MAKEINTRESOURCE(IDD_DIALOG_MYSTOCK), m_hParent, baseDlgProc, (LPARAM)this);
}

int CDlgStockInfo::OpenStockFile (void)
{
	char				szFile[256];
	DWORD				dwID = 0;
	OPENFILENAME		ofn;
	memset (szFile, 0, sizeof (szFile));
	memset( &(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = m_hDlg;
	ofn.lpstrFilter = TEXT("Stock File (*.txt)\0*.txt\0");	
	if (_tcsstr (szFile, _T(":/")) != NULL)
		_tcscpy (szFile, _T("*.txt"));
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = TEXT("Open Stock File");
	ofn.Flags = OFN_EXPLORER;
			
	if (!GetOpenFileName(&ofn))
		return QC_ERR_FAILED;	

	CStockFileHist			filKXT;
	qcStockKXTInfoItem *	pKXTItem = NULL;
	CStockFileBuy			filBuy;

	if (filBuy.Open (szFile) != QC_ERR_NONE)
		return QC_ERR_FAILED;

	SYSTEMTIME	tmNow;
	long long	llStartDate = 0;
	GetLocalTime (&tmNow);
	if (tmNow.wMonth > 1)
		tmNow.wMonth -= 1;
	else
	{
		tmNow.wYear -= 1;
		tmNow.wMonth = 12;
	}
	SystemTimeToFileTime (&tmNow, (LPFILETIME)&llStartDate);
	filKXT.SetStartEndDate (llStartDate, -1);

	double		dHandMoneyBuy = 0;
	double		dSellMoneyBuy = 0;
	double		dHandMoneySell = 0;
	double		dSellMoneySell = 0;
	double		dSellPrice = 0;
	char		szItem[64];
	char		szLine[1024];

	strcpy (szLine, "股票代码  买入日期   买入数量       买入价钱      卖出日期       卖出价钱      赢亏         百分比");
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_MYSTOCK), LB_RESETCONTENT, 0, 0);
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_MYSTOCK), LB_ADDSTRING, 0, (LPARAM)szLine);

	bsBuyInfo *	pBuyItem = NULL;
	NODEPOS		pos = filBuy.m_lstBuyInfo.GetHeadPosition ();
	while (pos != NULL)
	{
		pBuyItem = filBuy.m_lstBuyInfo.GetNext (pos);

		strcpy (szLine, pBuyItem->m_szCode);
		strcat (szLine, "    ");

		sprintf (szItem, "%d-%02d-%02d", pBuyItem->m_nBuyYear, pBuyItem->m_nBuyMonth, pBuyItem->m_nBuyDay);
		strcat (szLine, szItem);

		sprintf (szItem, "% 8d", pBuyItem->m_nBuyNum);
		strcat (szLine, szItem);

		strcat (szLine, "   ");
		FormatDouble (pBuyItem->m_dBuyPrice, szItem);
		strcat (szLine, szItem);

		if (pBuyItem->m_nSellYear > 0)
		{
			sprintf (szItem, "    %d-%02d-%02d   ", pBuyItem->m_nSellYear, pBuyItem->m_nSellMonth, pBuyItem->m_nSellDay);
			strcat (szLine, szItem);
			dSellPrice = pBuyItem->m_dSellPrice;
			dSellMoneyBuy = dSellMoneyBuy + pBuyItem->m_dBuyPrice * pBuyItem->m_nBuyNum;
			dSellMoneySell = dSellMoneySell + dSellPrice * pBuyItem->m_nBuyNum;
		}
		else
		{
			strcat (szLine, "                 ");
			filKXT.SetCode (pBuyItem->m_szCode);
			pKXTItem = filKXT.m_lstItem.GetTail ();
			dSellPrice = pKXTItem->m_dClose;
			dHandMoneyBuy = dHandMoneyBuy + pBuyItem->m_dBuyPrice * pBuyItem->m_nBuyNum;
			dHandMoneySell = dHandMoneySell + dSellPrice * pBuyItem->m_nBuyNum;
		}
		FormatDouble (dSellPrice, szItem);
		strcat (szLine, szItem);

		FormatDouble ((dSellPrice - pBuyItem->m_dBuyPrice) * pBuyItem->m_nBuyNum, szItem);
		strcat (szLine, szItem);

		FormatDouble ((dSellPrice - pBuyItem->m_dBuyPrice) * 100 / pBuyItem->m_dBuyPrice, szItem);
		strcat (szLine, szItem);
		strcat (szLine, "%");

		SendMessage (GetDlgItem (m_hDlg, IDC_LIST_MYSTOCK), LB_ADDSTRING, 0, (LPARAM)szLine);
	}
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_MYSTOCK), LB_ADDSTRING, 0, (LPARAM)"    ");
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_MYSTOCK), LB_ADDSTRING, 0, (LPARAM)"汇总:");
	strcpy (szLine, "卖掉股票：");
	FormatDouble (dSellMoneyBuy, szItem);
	strcat (szLine, szItem);
	FormatDouble (dSellMoneySell, szItem);
	strcat (szLine, szItem);
	FormatDouble (dSellMoneySell - dSellMoneyBuy, szItem);
	strcat (szLine, szItem);
	if (dSellMoneyBuy > 0)
	{
		FormatDouble ((dSellMoneySell - dSellMoneyBuy) * 100 / dSellMoneyBuy, szItem);
		strcat (szLine, szItem);
		strcat (szLine, "%");
	}
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_MYSTOCK), LB_ADDSTRING, 0, (LPARAM)szLine);
	strcpy (szLine, "持有股票：");
	FormatDouble (dHandMoneyBuy, szItem);
	strcat (szLine, szItem);
	FormatDouble (dHandMoneySell, szItem);
	strcat (szLine, szItem);
	FormatDouble (dHandMoneySell - dHandMoneyBuy, szItem);
	strcat (szLine, szItem);
	if (dHandMoneyBuy > 0)
	{
		FormatDouble ((dHandMoneySell - dHandMoneyBuy) * 100 / dHandMoneyBuy, szItem);
		strcat (szLine, szItem);
		strcat (szLine, "%");
	}
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_MYSTOCK), LB_ADDSTRING, 0, (LPARAM)szLine);

	strcpy (szLine, "    ");
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_MYSTOCK), LB_ADDSTRING, 0, (LPARAM)szLine);

	FormatDouble ((dHandMoneySell - dHandMoneyBuy) + (dSellMoneySell - dSellMoneyBuy), szItem);
	sprintf (szLine, "总盈亏： %s", szItem);
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_MYSTOCK), LB_ADDSTRING, 0, (LPARAM)szLine);

	return QC_ERR_NONE;
}


int CDlgStockInfo::OnInitDlg (void)
{
	char szLine[1024];
	strcpy (szLine, "股票代码  买入日期   买入数量       买入价钱     卖出日期        卖出价钱      赢亏        百分比");
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_MYSTOCK), LB_ADDSTRING, 0, (LPARAM)szLine);

	CenterDlg ();
	return QC_ERR_NONE;
}

INT_PTR CDlgStockInfo::OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		OnInitDlg ();
		break;

	case WM_COMMAND:
	{
		int wmId    = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case ID_FILE_OPEN:
			OpenStockFile ();
			break;

		case IDC_BUTTON_OK:
		case IDC_BUTTON_CANCEL:
			EndDialog (hDlg, wmId);
			break;

		default:
			break;
		}
		break;
	}
	default:
		break;
	}
	return FALSE;
}

void CDlgStockInfo::FormatDouble (double dNum, char * szText)
{
	dNum += 0.00001;
	if (dNum < -1000000)
		sprintf (szText, " %.2f", dNum);
	else if (dNum < -100000)
		sprintf (szText, "  %.2f", dNum);
	else if (dNum < -10000)
		sprintf (szText, "   %.2f", dNum);
	else if (dNum < -1000)
		sprintf (szText, "    %.2f", dNum);
	else if (dNum < -100)
		sprintf (szText, "     %.2f", dNum);
	else if (dNum < -10) 
		sprintf (szText, "      %.2f", dNum);
	else if (dNum < 0)
		sprintf (szText, "       %.2f", dNum);
	else if (dNum < 10)
		sprintf (szText, "        %.2f", dNum);
	else if (dNum < 100)
		sprintf (szText, "       %.2f", dNum);
	else if (dNum < 1000)
		sprintf (szText, "      %.2f", dNum);
	else if (dNum < 10000)
		sprintf (szText, "     %.2f", dNum);
	else if (dNum < 100000)
		sprintf (szText, "    %.2f", dNum);
	else if (dNum < 1000000)
		sprintf (szText, "   %.2f", dNum);
	else
		sprintf (szText, "  %.2f", dNum);
}
