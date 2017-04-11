// getStockFHSP.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "getStockFHSP.h"

#include "CHttpUtil.h"
#include "CStockFileCode.h"
#include "CStockFileFHSP.h"
#include "CStockFileFinance.h"
#include "CStockFileCompInfo.h"
#include "CStockFileHYGN.h"

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

//	CStockFileFHSP filFHSP;
//	filFHSP.Open ("600477", true);

//	CStockFileFinance	filFinance;
//	filFinance.Open ("600477", false);

//	CStockFileCompInfo	filCompInfo;
//	filCompInfo.Open ("600895", true);

//	CStockFileHYGN	filHYGN;
//	filHYGN.Open ("600895", false);

	SetTimer (hWnd, 101, 10, NULL);

//	char * pData = NULL;
//	int nRC = g_pHTTPUtil->RequestData ("http://vip.stock.finance.sina.com.cn/corp/go.php/vISSUE_ShareBonus/stockid/300316.phtml", &pData);

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
