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

#include "CDlgSellMsg.h"

#include "Resource.h"

CDlgSellMsg::CDlgSellMsg(HINSTANCE hInst, HWND hParent)
	: CDlgBase (hInst, hParent)
	, m_pBuyInfo (NULL)
{
}

CDlgSellMsg::~CDlgSellMsg(void)
{
}

void CDlgSellMsg::ResetMsg (void)
{
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_RESETCONTENT, 0, 0);

	char szLine[1024];
	strcpy (szLine, "股票代码  股票名称   买入日期   买入数量  买入价钱  成交价钱  五日均线  均线偏离  输  赢");
	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_ADDSTRING, 0, (LPARAM)szLine);
}

int	CDlgSellMsg::AddStockItem (qcStockRealTimeItem * pStockItem, double dLine5)
{
	if (m_pBuyInfo == NULL || pStockItem == NULL)
		return QC_ERR_FAILED;

	bsBuyInfo *	pBuyItem = NULL;
	NODEPOS pos = m_pBuyInfo->GetHeadPosition ();
	while (pos != NULL)
	{
		pBuyItem = m_pBuyInfo->GetNext (pos);
		if (!strcmp (pBuyItem->m_szCode, pStockItem->m_szCode))
			break;
	}

	char szItem[64];
	char szLine[1024];
	strcpy (szLine, pStockItem->m_szCode);
	strcat (szLine, "    ");
	memset (szItem, 0, sizeof (szItem));
	WideCharToMultiByte (CP_ACP, 0, pStockItem->m_wzName, -1, szItem, sizeof (szItem), NULL, NULL);
	strcat (szLine, szItem);
	strcat (szLine, "  ");

	sprintf (szItem, "%d-%02d-%02d", pBuyItem->m_nBuyYear, pBuyItem->m_nBuyMonth, pBuyItem->m_nBuyDay);
	strcat (szLine, szItem);

	sprintf (szItem, "% 8d", pBuyItem->m_nBuyNum);
	strcat (szLine, szItem);

	strcat (szLine, "   ");
	FormatDouble (pBuyItem->m_dBuyPrice, szItem);
	strcat (szLine, szItem);

	strcat (szLine, "    ");
	FormatDouble (pStockItem->m_dNowPrice, szItem);
	strcat (szLine, szItem);

	strcat (szLine, "   ");
	FormatDouble (dLine5, szItem);
	strcat (szLine, szItem);

	strcat (szLine, "   ");
	FormatDouble (pStockItem->m_dNowPrice - dLine5, szItem);
	strcat (szLine, szItem);

	strcat (szLine, " ");
	FormatDouble ((pStockItem->m_dNowPrice - pBuyItem->m_dBuyPrice) * 100 / pBuyItem->m_dBuyPrice, szItem);
	strcat (szLine, szItem);
	strcat (szLine, "%");

	SendMessage (GetDlgItem (m_hDlg, IDC_LIST_STOCK), LB_ADDSTRING, 0, (LPARAM)szLine);

	RECT rcDlg;
	GetWindowRect (m_hDlg, &rcDlg);
	if ((rcDlg.bottom - rcDlg.top) < 200)
		SendMessage (m_hDlg, WM_SYSCOMMAND, SC_RESTORE, 0);

	return QC_ERR_NONE;
}

int CDlgSellMsg::CreateDlg (void)
{
	CDlgBase::CreateDlg ();
	m_hDlg = CreateDialog (m_hInst, MAKEINTRESOURCE(IDD_DIALOG_SELLMSG), m_hParent, baseDlgProc);
	if (m_hDlg == NULL)
		return -1;
	SetWindowLong (m_hDlg, GWL_USERDATA, (LONG)this);

	CenterDlg ();
	PostMessage (m_hDlg, WM_SYSCOMMAND, SC_MINIMIZE, 0);

	return 0;
}

LRESULT CDlgSellMsg::OnResize (void)
{
	RECT rcWnd;
	GetWindowRect (m_hParent, &rcWnd);
	RECT rcDlg;
	GetWindowRect (m_hDlg, &rcDlg);
	if ((rcDlg.bottom - rcDlg.top) < 200)
		SetWindowPos (m_hDlg, NULL, rcWnd.right-(rcDlg.right-rcDlg.left), rcWnd.bottom- (rcDlg.bottom-rcDlg.top), 0, 0, SWP_NOSIZE);

	return S_OK;
}

INT_PTR CDlgSellMsg::OnReceiveMsg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SIZE:
		OnResize ();
		break;

	case WM_COMMAND:
	{
		int wmId    = LOWORD(wParam);
		int wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDOK:
		case IDCANCEL:
			SendMessage (hDlg, WM_SYSCOMMAND, SC_MINIMIZE, 0);
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
