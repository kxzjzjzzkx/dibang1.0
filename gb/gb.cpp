// gb.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "gb.h"
#include "Resource.h"
#include "CommCtrl.h"
#include "HttpUtils.h"
#include "SerialPort.h"
#include <iostream>
/**/


#pragma comment ( lib,"comctl32.lib" )  

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

CSerialPort mySerialPort;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DialogStorage(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DialogLogin(HWND, UINT, WPARAM, LPARAM);


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
	LoadString(hInstance, IDC_GB, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GB));

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

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GB));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GB);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
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

   return TRUE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	//RECT rect;
	//LISTVIEW hListview;

	/*
		listview test code
		LVCOLUMN list1;
		LVITEM item1;
		HIMAGELIST imglist1;
		HWND hListview;
	*/

	switch (message)
	{
	case WM_CREATE:
        /*hInst = ((LPCREATESTRUCT)lParam) -> hInstance;
        hList = CreateWindowEx(NULL, TEXT("SysListView32"), NULL, LVS_REPORT | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, NULL, hInst, NULL);
        InsertColumn();
        SendMessage(hList, LVM_SETTEXTCOLOR, 0, RGB(255, 255, 255));
        SendMessage(hList, LVM_SETBKCOLOR, 0, RGB(100, 100, 100));
        SendMessage(hList, LVM_SETTEXTBKCOLOR, 0, RGB(0, 0, 0));
        Refresh();
		MessageBox(NULL,TEXT("窗口创建初始化"),TEXT("lololo"),0);
		*/
		DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_LOGIN), hWnd, DialogLogin);
        break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			//MessageBox(NULL,TEXT("关于"),TEXT("lololo"),0);
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
		//	MessageBox(NULL,TEXT("关闭咯！"),TEXT("lololo "),0);
			DestroyWindow(hWnd);
			break;
		case IDM_STORAGE:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_STORAGE), hWnd, DialogStorage);
			break;
		case IDM_LOGIN:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_LOGIN), hWnd, DialogLogin);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc=BeginPaint( hWnd,&ps ); //取得设备环境句柄
		//// TODO: Add any drawing code here...
		////GetClientRect(hWnd,&rect);
		////DrawText(hdc,TEXT("来吧！"),-1,&rect,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
  //      TextOut( hdc, 20, 10, "fei1", 4 );//输出文字
		//TextOut( hdc, 20, 30, "fei2", 4 );//输出文字
		////
	//	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_LOGIN), hWnd, DialogLogin);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:	
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

void initTableUser(HWND hDlg){
	HWND hListview;
	string httpResult;
	LVITEM lvItem;
	queue<DBInfo> list;
	int queueSize;
	int i;
	hListview = GetDlgItem(hDlg,IDC_LIST2);
	// 设置ListView的列
	LVCOLUMN vcl;
	vcl.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	vcl.pszText = "序号";//列标题
	vcl.cx = 40;//列宽
	vcl.iSubItem = 0;//子项索引，第一列无子项
	ListView_InsertColumn(hListview, 0, &vcl);
	vcl.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	vcl.pszText = "供货人编号";
	vcl.cx = 90;
	vcl.iSubItem = 1;//子项索引
	ListView_InsertColumn(hListview, 1, &vcl);
	vcl.pszText = "供货人";
	vcl.cx = 40;
	vcl.iSubItem = 2;
	ListView_InsertColumn(hListview, 2, &vcl);
	vcl.pszText = "毛重时间";
	vcl.cx = 60;
	vcl.iSubItem = 3;
	ListView_InsertColumn(hListview, 3, &vcl);
	vcl.pszText = "毛重";
	vcl.cx = 50;
	vcl.iSubItem = 4;
	ListView_InsertColumn(hListview, 4, &vcl);
	vcl.pszText = "皮重时间";
	vcl.cx = 60;
	vcl.iSubItem = 5;
	ListView_InsertColumn(hListview, 5, &vcl);
	vcl.pszText = "皮重";
	vcl.cx = 50;
	vcl.iSubItem = 6;
	ListView_InsertColumn(hListview, 6, &vcl);
	vcl.pszText = "品名";
	vcl.cx = 90;
	vcl.iSubItem = 7;
	ListView_InsertColumn(hListview, 7, &vcl);
	vcl.pszText = "单价";
	vcl.cx = 90;
	vcl.iSubItem = 8;
	ListView_InsertColumn(hListview, 8, &vcl);
	vcl.pszText = "净重";
	vcl.cx = 90;
	vcl.iSubItem = 9;
	ListView_InsertColumn(hListview, 9, &vcl);
	vcl.pszText = "总额";
	vcl.cx = 90;
	vcl.iSubItem = 10;
	ListView_InsertColumn(hListview, 10, &vcl);
	vcl.pszText = "状态";
	vcl.cx = 90;
	vcl.iSubItem = 11;
	ListView_InsertColumn(hListview, 11, &vcl);

	HttpUtils hu ;
	httpResult = hu.httpGet(_T("http://sym.zz91.com/fragment/esite/newsList.htm?cid=123516472&left2017md1"));		
	list = hu.ParseJsonInfoForList(httpResult);
	queueSize = list.size();
	lvItem.mask = LVIF_TEXT; 
	for(i=0;i<queueSize;i++){
		char str[100];
        lvItem.iItem = i;				// 行
        lvItem.iSubItem = 0;			// 列1
		lvItem.pszText = "abc";			// 内容
        ListView_InsertItem(hListview, (LPARAM)&lvItem);  
		// 列2
		lvItem.iSubItem = 1;			
		sprintf(str,"%d",list.front().getCId());	
		lvItem.pszText = str;
        ListView_SetItem( hListview, (LPARAM)&lvItem);
		// TODO

		list.pop();						// 下一个队列元素
	}
}


// Message handler for about box.
INT_PTR CALLBACK DialogStorage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	/* 初始化磅秤句柄  */
//	mySerialPort.setHDlg(hDlg);
	char str[100];
	HDC hdc;
	hdc = GetDC(hDlg);
	char test[9] = { "xilageba" };
	PAINTSTRUCT ps;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:

		

		initTableUser(hDlg); //用户表格初始化

		/*初始化线程实时同步磅秤信息*/
		
		if (!mySerialPort.InitPort(3, CBR_1200, 'N', 8, 1, EV_RXCHAR))
		{
			MessageBox(NULL, TEXT("设备异常，请检查后重试！"), TEXT("消息"), 0);
		}
		else
		{
			if (!mySerialPort.OpenListenThread())
			{
				MessageBox(NULL, TEXT("实时功能初始化异常，请重试！"), TEXT("消息"), 0);
			}
			else
			{
				MessageBox(NULL, TEXT("设备正常！"), TEXT("消息"), 0);
			}
		}
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_BUTTON_BT_COM)
		{
			mySerialPort.Read(str);
			TextOut(hdc, 35, 180, str, 10);
			return (INT_PTR)TRUE;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hDlg, &ps); //取得设备环境句柄
									 //// TODO: Add any drawing code here...
									 ////GetClientRect(hWnd,&rect);
									 ////DrawText(hdc,TEXT("来吧！"),-1,&rect,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
									 //      TextOut( hdc, 20, 10, "fei1", 4 );//输出文字
									 //TextOut( hdc, 20, 30, "fei2", 4 );//输出文字
									 ////
									 //	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_LOGIN), hWnd, DialogLogin);
		mySerialPort.Read(str);
		TextOut(hdc, 35, 80, str, 10);
		EndPaint(hDlg, &ps);
		break;
	}
	
	return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK DialogLogin(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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