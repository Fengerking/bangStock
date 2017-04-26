// getStockFHSP.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "math.h"

#include "getStockFHSP.h"

#include "CHttpUtil.h"
#include "CStockKXTInfo.h"

#include "CStockFileCode.h"
#include "CStockFileFHSP.h"
#include "CStockFileFinance.h"
#include "CStockFileCompInfo.h"
#include "CStockFileHYGN.h"

#include "USystemFunc.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

CHttpUtil *			g_pHTTPUtil = NULL;
CFileIO *			g_pFIO = NULL;
CStockFileCode *	g_pCodeList = NULL;

char				g_szURL[1024];
char				g_szFile[1024];

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int					ParserIndexHistData (void);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_GETSTOCKFHSP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GETSTOCKFHSP));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GETSTOCKFHSP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GETSTOCKFHSP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

	g_pCodeList = new CStockFileCode ();
	g_pCodeList->Open ("codeList.txt");

//	ParserIndexHistData ();		

//	CStockFileFHSP filFHSP;
//	filFHSP.Open ("300400", true);

//	CStockFileFinance	filFinance;
//	filFinance.Open ("600477", false);

//	CStockFileCompInfo	filCompInfo;
//	filCompInfo.Open ("600895", true);

//	CStockFileHYGN	filHYGN;
//	filHYGN.Open ("600895", false);

//	SetTimer (hWnd, 101, 10, NULL);

	char * pData = NULL;
	g_pHTTPUtil = new CHttpUtil ();
	int nRC = g_pHTTPUtil->RequestData ("https://www.baidu.com/index.html", &pData);

//	filIO.Open ("c:\\work\\Temp\\300316.txt", 0, QCIO_FLAG_WRITE);
//	filIO.Write ((unsigned char *)pData, strlen (pData));


   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_TIMER:
	{
		int		nCurSel = g_pCodeList->GetCurSel ();
		char *	pCode = g_pCodeList->GetSelCode ();

		//CStockFileFHSP filFHSP;
		//filFHSP.Open (pCode, true);
		//CStockFileFinance	filFinance;
		//filFinance.Open (pCode, true);
		CStockFileHYGN	filHYGN;
		filHYGN.Open (pCode, false);


		SetWindowText (hWnd, pCode);
		if (nCurSel+1 == g_pCodeList->GetCodeCount ())
		{
			KillTimer (hWnd, 101);
			MessageBox (hWnd, "It finished download!", "Information", MB_OK);
		}
		g_pCodeList->SetCurSel (nCurSel+1);
	}
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		delete g_pHTTPUtil;
		delete g_pFIO;
		delete g_pCodeList;

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

int ParserIndexHistData (void)
{
	char m_szPath[256];

	strcpy (m_szPath, "c:\\work\\temp\\399006.csv");

	CFileIO * pFile = new CFileIO ();
	if (pFile->Open (m_szPath, 0, QCIO_FLAG_READ) != QC_ERR_NONE)
	{
		delete pFile;
		return QC_ERR_FAILED;
	}
	int nSize = (int)pFile->GetSize ();
	char * pData = new char[nSize];
	pFile->Read ((unsigned char *)pData, nSize, true, 0);

	SYSTEMTIME				tmDate;
	memset (&tmDate, 0, sizeof (SYSTEMTIME));

	CObjectList <qcStockKXTInfoItem> m_lstItem;

	qcStockKXTInfoItem *	pItem = NULL;
	char *					pNum = NULL;
	char *					pE = NULL;
	double					dE = 0;
	int						nE = 0;
	char					szLine[256];
	int						nRest = nSize;
	char *					pBuff = pData;
	int						nLine = qcReadTextLine (pBuff, nRest, szLine, sizeof (szLine));
	pBuff +=  nLine;
	nRest -= nLine;
	while (nRest > 0)
	{
		nLine = qcReadTextLine (pBuff, nRest, szLine, sizeof (szLine));		
		if (nLine <= 0)
			break;
		pBuff +=  nLine;
		nRest -= nLine;

		sscanf (szLine, "%d-%d-%d", &tmDate.wYear, &tmDate.wMonth, &tmDate.wDay);
		if (tmDate.wYear < 2000)
			break;
		pItem = new qcStockKXTInfoItem ();
		memset (pItem, 0, sizeof (qcStockKXTInfoItem));
		m_lstItem.AddTail (pItem);
		pItem->m_nYear = tmDate.wYear; pItem->m_nMonth = tmDate.wMonth; pItem->m_nDay = tmDate.wDay; 

		pNum = szLine;
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_dClose = atof (pNum);	
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_dMax = atof (pNum);	
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_dMin = atof (pNum);	
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_dOpen = atof (pNum);	
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_dDiffNum = atof (pNum);		
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_dDiffRate = atof (pNum);		
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pItem->m_nVolume = (int)(atof (pNum) / 100);		
		while (*pNum != ',') {pNum++;if (pNum-szLine > nLine) return QC_ERR_FAILED;}pNum++;
		pE = strchr (pNum, 'e');
		if (pE == NULL)
			pItem->m_nMoney = atoi (pNum);
		else
		{
			*pE = 0;
			dE = atof (pNum);
			pE++; pE++;
			nE = atoi (pE);
			pItem->m_nMoney = (int)(dE * pow (10.0, (nE - 4)));
		}
	}

	qcGetAppPath (NULL, m_szPath, sizeof (m_szPath));
	sprintf (m_szPath, "%sdata\\history\\%s.txt", m_szPath, "sz399006");

	CFileIO filIO;
	if (filIO.Open (m_szPath, 0, QCIO_FLAG_WRITE) != QC_ERR_NONE)
		return -1;

	strcpy (szLine, "日期,开盘,收盘,最高,最低,交易量(手),交易金额(元),涨跌额,涨跌幅(%),振幅(%),换手率(%)\r\n");
	filIO.Write ((unsigned char *)szLine, strlen (szLine));

	NODEPOS					pos = m_lstItem.GetTailPosition ();
	while (pos != NULL)
	{
		pItem = m_lstItem.GetPrev (pos);
		sprintf (szLine, "%d-%02d-%02d,%.2f,%.2f,%.2f,%.2f,%d,%d,%.2f,%.2f,%.2f,%.2f\r\n",
						 pItem->m_nYear, pItem->m_nMonth, pItem->m_nDay, pItem->m_dOpen, pItem->m_dClose, pItem->m_dMax, pItem->m_dMin,
						 pItem->m_nVolume, pItem->m_nMoney, pItem->m_dDiffNum, pItem->m_dDiffRate, pItem->m_dSwing, pItem->m_dExchange);
		filIO.Write ((unsigned char *)szLine, strlen (szLine));
	}
	filIO.Close ();

	pItem = m_lstItem.RemoveHead ();
	while (pItem != NULL)
	{
		delete pItem;
		pItem = m_lstItem.RemoveHead ();
	}
	return 0;
}