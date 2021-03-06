// gb.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "gb.h"
#include "Resource.h"
#include "CommCtrl.h"
//#include "HttpUtils.h"
#include "SerialPort.h"
//#include "DBUtils.cpp"
#include <stdlib.h>
#include "MD5.h"
#include <string>
#include <tchar.h>

// import class User 
#include "users.h"
#include "suppliers.h"

// import config		配置全局参数(sql语句，httpurl地址等等)
#include "sysconfig.h"

// import service		业务逻辑模块后续可以添加
#include "importUpdateLog.h"			// 初始化本地access的逻辑，必须有网络的情况下
#include "StorageService.h"				// 入库单逻辑
#include "LoginService.h"				// 登陆逻辑
#include "ICService.h"


// file io
#include <fstream>
#include <iostream>

// import StringUtils
#include "StringUtils.h"

// tab control
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
//控件库以及相关宏  
#include <windowsx.h>//有HANDLE_MSG宏+标准控件功能函数宏  
#include <commctrl.h>//高级控件函数宏  
#pragma comment ( lib,"comctl32.lib" )

// time.h
#include <time.h>

// http open 
#include <assert.h>
#include <shellapi.h>

//#pragma comment(lib, "SkinPPWTL.lib")
//#include "skin++/lib/SkinPPWTL.h"

#define MAX_LOADSTRING 100
#define ID_TIMER 1

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// dibang config
CSerialPort mySerialPort;
CSerialPort mySerialPort2;
int PORT_LV[12] = { 300,600,1200,2400,4800,9600,19200,38400,43000,56000,57600,115200 };
bool portThreadOpen=false;

// 自绘tab的 list数据填充
HWND Page[100];
int iPageIndex = 0;
LRESULT CALLBACK SearchProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK NoPayProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK PayedProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// choose iccode
string CHOOSE_ICCODE;

// set money data
int STORAGE_SET_MONEY_ID;
string STORAGE_SET_MONEY_NAME;

//  choose product
string CHOOSE_CATEGORY_SELFID;
string CHOOSE_SAVE_PRODUCTS_SELF_ID;


// 大磅 tab设置里所有控件
HWND tabBDlg[20];
// 小磅 tab设置里所有控件
HWND tabSDlg[20];

// 线程hwnd
HWND threadDisHwnd;

// 窗体hwnd
HWND hWndMain;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DialogStorage(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DialogLogin(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DialogSetMoney(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	DialogChooseProduct(HWND, UINT, WPARAM , LPARAM );
INT_PTR CALLBACK	DialogPortConfig(HWND, UINT, WPARAM, LPARAM);

//thread 线程 方法
DWORD WINAPI TimerSerialPortThread(LPVOID);			// 获取地磅重量信息
DWORD WINAPI SystemUploadThread(LPVOID);			// 推送本地最新的入库单和供应商数据给外网
DWORD WINAPI UpdateStorageForPriceThread(LPVOID);	// 5秒一次更新网上定价的信息


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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_MAIN));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_GB);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON_MAIN));

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
	// skin 皮肤装在，很丑暂时不用
	//skinppLoadSkin(_T("skin++/skin/spring.ssk"));
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   hWndMain = hWnd;
  
   if (!hWnd)
   {
      return FALSE;
   }

   //ShowWindow(hWnd, nCmdShow);
   ShowWindow(hWndMain, SW_HIDE);
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
// 

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;	

	DBUtils db;
	char sql[300];
	string sqlString;

	int count = 0;
	HWND hBn;
	HWND hDialog;

	HANDLE hThread;
	
	switch (message)
	{
	case WM_CREATE:
		//
		/*初始化线程实时同步磅秤信息*/
		//
		sqlString = SQL_EQSET_SELECT_ALL;
		strcpy(sql, sqlString.c_str());
		db.getRecordRet(sql);
		while (!db.HX_pRecordset->adoEOF)
		{
			_variant_t var = db.HX_pRecordset->GetCollect("eq_type");
			if (var.vt == VT_NULL) {
				continue;
			}
			string strType = _com_util::ConvertBSTRToString((_bstr_t)var);
			var = db.HX_pRecordset->GetCollect("eq_com");
			string strCom = "";
			if (var.vt != VT_NULL) {
				strCom = _com_util::ConvertBSTRToString((_bstr_t)var);
			}
			var = db.HX_pRecordset->GetCollect("eq_btl");
			string strBtl = "";
			if (var.vt != VT_NULL) {
				strBtl = _com_util::ConvertBSTRToString((_bstr_t)var);
			}
			char message[200];
			if (strType == "1") {
				strCom = strCom.replace(strCom.find("COM"), 3, "");
				int selcom = atoi(strCom.c_str());
				int btl = atoi(strBtl.c_str());
				if (!mySerialPort.InitPort(selcom, btl, 'N', 8, 1, NULL) || selcom < 1 || btl< 1)
				{
				}
				else
				{
					mySerialPort.isClose = false;
				}
			}
			else if (strType == "2") {
				strCom = strCom.replace(strCom.find("COM"), 3, "");
				int selcom = atoi(strCom.c_str());
				int btl = atoi(strBtl.c_str());
				if (!mySerialPort2.InitPort(selcom, btl, 'N', 8, 1, EV_RXCHAR) || selcom < 1 || btl< 1)
				{
				}
				else
				{
					mySerialPort2.isClose = false;
				}
			}
			db.HX_pRecordset->MoveNext();
		}
		db.ExitConnect();
		// 初始化结束

		hBn = GetDlgItem(hWnd, IDOK);
		SetWindowLong(hBn, GWL_STYLE, GetWindowLong(hBn, GWL_STYLE) | BS_OWNERDRAW);
		// //判定是否有client，如果有，则获取client下的用户登录
		// infile.open("client.config");
		// infile >> clientid;
		// sqlString = SQL_USERS_SELECT_BY_CLIENTID;
		// sqlString = sqlString.replace(sqlString.find("<CLIENTID>"),10,clientid);
		// strcpy(sql, sqlString.c_str());
		// db.getRecordRet(sql);
		
		// while (!db.HX_pRecordset->adoEOF) {
		// 	_variant_t var;
		// 	string strValue;
		// 	var = db.HX_pRecordset->GetCollect("id");
		// 	if (var.vt != VT_NULL) {
		// 		strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
		// 	}
		// 	count = atoi(strValue.c_str());
		// 	var = db.HX_pRecordset->GetCollect("selfid");
		// 	if (var.vt != VT_NULL) {
		// 		strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
		// 	}
		// 	strcpy(LOGIN_SELFID, strValue.c_str());
		// 	var = db.HX_pRecordset->GetCollect("username");
		// 	if (var.vt != VT_NULL) {
		// 		strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
		// 	}
		// 	strcpy(LOGIN_USERNAME, strValue.c_str());
		// 	var = db.HX_pRecordset->GetCollect("group_id");
		// 	if (var.vt != VT_NULL) {
		// 		strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
		// 	}
		// 	strcpy(LOGIN_GROUPID, strValue.c_str());
		// 	var = db.HX_pRecordset->GetCollect("company_id");
		// 	if (var.vt != VT_NULL) {
		// 		strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
		// 	}
		// 	strcpy(LOGIN_COMPANYID, strValue.c_str());
		// 	db.HX_pRecordset->MoveNext();
		// }
		// db.ExitConnect();
		// if (count>0)
		// {
		// 	// 更新外网数据
		// 	systemDataInit();
		// 	/* 启动上传入库单信息的线程或定时器 */
		// 	hThread = CreateThread(NULL, 0, SystemUploadThread, NULL, 0, NULL);
		// 	CloseHandle(hThread);
		// 	hThread = CreateThread(NULL, 0, UpdateStorageForPriceThread, NULL, 0, NULL);
		// 	CloseHandle(hThread);
		// 	//DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_STORAGE), hWnd, DialogStorage);
		// 	//
		// 	HWND hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_STORAGE), hWnd, DialogStorage);
		// 	ShowWindow(hDialog, SW_SHOW);
		// 	//ShowWindow(hWndMain, SW_HIDE);
		// }
		// else {
			hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_LOGIN), hWnd, DialogLogin);
			ShowWindow(hDialog, SW_SHOW);
			//DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_LOGIN), hWnd, DialogLogin);
		// }
		return (INT_PTR)TRUE;
        //break;
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
			// 未登录情况判断
			if (LOGIN_COMPANYID[0] == '\0') {
				MessageBox(hWnd,TEXT("请先登录(系统->登录)，再进行过磅操作"), TEXT("提示"),0);
				break;
			}
			hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_STORAGE), hWnd, DialogStorage);
			ShowWindow(hDialog, SW_SHOW);
			break;
		case IDM_PORT_CONFIG:
			// 未登录情况判断
			if (LOGIN_COMPANYID[0] == '\0') {
				MessageBox(hWnd, TEXT("请先登录(系统->登录)，再进行此操作"), TEXT("提示"), 0);
				break;
			}
			hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_PORT_CONFIG), hWnd, DialogPortConfig);
			ShowWindow(hDialog, SW_SHOW);
			break;
		case IDM_LOGIN:
			//DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_LOGIN), hWnd, DialogLogin);
			hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_LOGIN), hWnd, DialogLogin);
			ShowWindow(hDialog, SW_SHOW);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc=BeginPaint( hWnd,&ps ); //取得设备环境句柄
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		//skinppExitSkin();
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

void initHeadUser(HWND hListview) {
	// 设置ListView的列
	LVCOLUMN vcl;
	vcl.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	vcl.pszText = "序号";//列标题
	vcl.cx = 60;//列宽
	vcl.iSubItem = 0;//子项索引，第一列无子项
	ListView_InsertColumn(hListview, 0, &vcl);
	vcl.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	vcl.pszText = "供货人编号";
	vcl.cx = 120;
	vcl.iSubItem = 1;//子项索引
	ListView_InsertColumn(hListview, 1, &vcl);
	vcl.pszText = "供货人";
	vcl.cx = 80;
	vcl.iSubItem = 2;
	ListView_InsertColumn(hListview, 2, &vcl);
	vcl.pszText = "联系电话";
	vcl.cx = 100;
	vcl.iSubItem = 3;
	ListView_InsertColumn(hListview, 3, &vcl);
	vcl.pszText = "供应商类型";
	vcl.cx = 80;
	vcl.iSubItem = 4;
	ListView_InsertColumn(hListview, 4, &vcl);
	vcl.pszText = "地址";
	vcl.cx = 160;
	vcl.iSubItem = 5;
	ListView_InsertColumn(hListview, 5, &vcl);
}

void initHeadStorage(HWND hListview) {
	// 设置ListView的列
	LVCOLUMN vcl;
	vcl.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	vcl.pszText = "序号";//列标题
	vcl.cx = 60;//列宽
	vcl.iSubItem = 0;//子项索引，第一列无子项
	ListView_InsertColumn(hListview, 0, &vcl);
	vcl.pszText = "磅";
	vcl.cx = 60;
	vcl.iSubItem = 1;//子项索引
	ListView_InsertColumn(hListview, 1, &vcl);
	vcl.pszText = "供货人编号";
	vcl.cx = 90;
	vcl.iSubItem = 2;//子项索引
	ListView_InsertColumn(hListview, 2, &vcl);
	vcl.pszText = "供货人";
	vcl.cx = 60;
	vcl.iSubItem = 3;
	ListView_InsertColumn(hListview, 3, &vcl);
	vcl.pszText = "进场时间";
	vcl.cx = 120;
	vcl.iSubItem = 4;
	ListView_InsertColumn(hListview, 4, &vcl);
	vcl.pszText = "毛重";
	vcl.cx = 50;
	vcl.iSubItem = 5;
	ListView_InsertColumn(hListview, 5, &vcl);
	vcl.pszText = "皮重";
	vcl.cx = 50;
	vcl.iSubItem = 6;
	ListView_InsertColumn(hListview, 6, &vcl);
	vcl.pszText = "品名";
	vcl.cx = 60;
	vcl.iSubItem = 7;
	ListView_InsertColumn(hListview, 7, &vcl);
	vcl.pszText = "单价";
	vcl.cx = 60;
	vcl.iSubItem = 8;
	ListView_InsertColumn(hListview, 8, &vcl);
	vcl.pszText = "净重";
	vcl.cx = 80;
	vcl.iSubItem = 9;
	ListView_InsertColumn(hListview, 9, &vcl);
	vcl.pszText = "总额";
	vcl.cx = 80;
	vcl.iSubItem = 10;
	ListView_InsertColumn(hListview, 10, &vcl);
	vcl.pszText = "状态";
	vcl.cx = 80;
	vcl.iSubItem = 11;
	ListView_InsertColumn(hListview, 11, &vcl);
}

HWND CreatePageWndForListView(HWND hTab, WNDPROC PageWndProc)
{
	RECT rcTabDisplay;
	//获取Tab控件客户区的的大小（Tab窗口坐标系）  
	GetClientRect(hTab, &rcTabDisplay);//获取Tab窗口的大小（而不是在屏幕坐标系中）  
	TabCtrl_AdjustRect(hTab, FALSE, &rcTabDisplay);//通过窗口大小，获取客户区大小。  
												   //创建与Tab控件客户区大小的画布  
	HWND hPage = CreateWindowEx(
		WS_EX_WINDOWEDGE,       //window extend Style  
		WC_LISTVIEW,   // window class name  
		NULL,  // window caption  
		WS_CHILD|LVS_REPORT|WS_BORDER | WS_TABSTOP,//WS_TABSTOP |  |  WS_VISIBLE |      // window style  
		rcTabDisplay.left,            // initial x position  
		rcTabDisplay.top,            // initial y position  
		rcTabDisplay.right-1,            // initial x size  
		rcTabDisplay.bottom-20,            // initial y size  
		hTab,                     // parent window handle  
		NULL,                     // window menu handle  
		hInst,                // program instance handle  
		NULL);                    // creation parameters  
//	SetWindowLongPtr(hPage, GWLP_HINSTANCE, (LONG_PTR)PageWndProc);//子类化窗口
	// 扩展风格添加 start 
	//整行选中 网格线 添加checkbox 三项
	DWORD styles = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES;
	ListView_SetExtendedListViewStyleEx(hPage, styles,styles);
	// 扩展风格 end
	return hPage;
}


HWND CreatePageWnd(HWND hTab, WNDPROC PageWndProc)
{
	RECT rcTabDisplay;
	//获取Tab控件客户区的的大小（Tab窗口坐标系）  
	GetClientRect(hTab, &rcTabDisplay);//获取Tab窗口的大小（而不是在屏幕坐标系中）  
	TabCtrl_AdjustRect(hTab, FALSE, &rcTabDisplay);//通过窗口大小，获取客户区大小。  
												   //创建与Tab控件客户区大小的画布  
	HWND hPage = CreateWindowEx(
		NULL,       //window extend Style  
		WC_COMBOBOX,   // window class name  
		"",  // window caption  
		WS_CHILD| CBS_DROPDOWN | CBS_SORT | WS_VSCROLL | WS_TABSTOP,// | WS_BORDER | WS_TABSTOP|WS_VISIBLE,//WS_TABSTOP |  |  WS_VISIBLE |      // window style  
		50,            // initial x position  
		50,            // initial y position  
		200,            // initial x size  
		20,            // initial y size  
		hTab,                     // parent window handle  
		NULL,                     // window menu handle  
		hInst,                // program instance handle  
		NULL);                    // creation parameters  
	
	return hPage;
}


//功能:向Tab中添加标签  
int InsertTabItem(HWND hTab, LPTSTR pszText, int iid)
{
	TCITEM ti = { 0 };
	ti.mask = TCIF_TEXT;
	ti.pszText = pszText;
	ti.cchTextMax = strlen(pszText);
	return (int)SendMessage(hTab, TCM_INSERTITEM, iid, (LPARAM)&ti);
}

/*
功能：向Tab中添加页面
实现原理：一个窗口多个窗口处理函数，每个窗口处理函数负责一个页面的布局和处理
注意：该添加方式是顺序添加，即AddAfter，在当前页面后添加
*/
BOOL addPage(HWND hTab, LPTSTR strPageLabel, WNDPROC PageWndProc)
{
	InsertTabItem(hTab, strPageLabel, iPageIndex);
	HWND hPage = CreatePageWndForListView(hTab, PageWndProc);
	Page[iPageIndex++] = hPage;
	return TRUE;
}

/*
功能：向Tab中添加页面
实现原理：一个窗口多个窗口处理函数，每个窗口处理函数负责一个页面的布局和处理
注意：该添加方式是顺序添加，即AddAfter，在当前页面后添加
*/
BOOL addPageForPort(HWND hTab, LPTSTR strPageLabel, WNDPROC PageWndProc,HWND page[20])
{
	int bbc = TabCtrl_GetItemCount(hTab);
	InsertTabItem(hTab, strPageLabel, bbc);
	RECT rcTabDisplay;
	//获取Tab控件客户区的的大小（Tab窗口坐标系）  
	GetClientRect(hTab, &rcTabDisplay);//获取Tab窗口的大小（而不是在屏幕坐标系中）  
	TabCtrl_AdjustRect(hTab, FALSE, &rcTabDisplay);//通过窗口大小，获取客户区大小。  
												   //创建与Tab控件客户区大小的画布  
	HWND hCombo = CreateWindowEx(
		NULL,       //window extend Style  
		WC_COMBOBOX,   // window class name  
		"",  // window caption  
		WS_CHILD | CBS_DROPDOWN | CBS_SORT | WS_VSCROLL | WS_TABSTOP,// | WS_BORDER | WS_TABSTOP|WS_VISIBLE,//WS_TABSTOP |  |  WS_VISIBLE |      // window style  
		125,            // initial x position  
		45,            // initial y position  
		180,            // initial x size  
		80,            // initial y size  
		hTab,                     // parent window handle  
		NULL,                     // window menu handle  
		hInst,                // program instance handle  
		NULL);                    // creation parameters
	ShowWindow(hCombo,SW_SHOW);
	page[0] = hCombo;
	SendMessage(hCombo, CB_INSERTSTRING, 0, (LPARAM)"COM1");
	SendMessage(hCombo, CB_INSERTSTRING, 1, (LPARAM)"COM2");
	SendMessage(hCombo, CB_INSERTSTRING, 2, (LPARAM)"COM3");
	HWND hCombo2 = CreateWindowEx(
		NULL,       //window extend Style  
		WC_COMBOBOX,   // window class name  
		"",  // window caption  
		WS_CHILD | CBS_DROPDOWN | CBS_SORT | WS_VSCROLL | WS_TABSTOP,// | WS_BORDER | WS_TABSTOP|WS_VISIBLE,//WS_TABSTOP |  |  WS_VISIBLE |      // window style  
		125,            // initial x position  
		88,            // initial y position  
		180,            // initial x size  
		180,            // initial y size  
		hTab,                     // parent window handle  
		NULL,                     // window menu handle  
		hInst,                // program instance handle  
		NULL);                    // creation parameters
	ShowWindow(hCombo2, SW_SHOW);
	page[1] = hCombo2;
	char portStr[8];
	for (size_t i = 0; i < 12; i++)
	{
		sprintf(portStr, "%d", PORT_LV[i]);
		//itoa(abc[i], portStr,6);
		SendMessage(hCombo2, CB_INSERTSTRING, i, (LPARAM)portStr);
		memset(portStr, 0, 7);
	}
	return TRUE;
}

//Page自己的窗口类，通过SetWindowLongPtr（）可以子类化  
ATOM RegisterPageClass()
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = NULL;
	wcex.hIconSm = NULL;
	wcex.lpszMenuName = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszClassName = TEXT("Page");
	wcex.lpfnWndProc = DefWindowProc;
	return RegisterClassEx(&wcex);
}

/*功能：初始化所有页面*/
BOOL InitPages(HWND hTab)
{
	
	return TRUE;
}

void getSqlSetColumn(_variant_t var,char* result) {
	string strValue;
	if (var.vt != VT_NULL) {
		strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
		strcpy(result,strValue.c_str());
	}
}

void initUserData(DBUtils db,int pageIndex) {
	int i = 0;
	string iccodeSetStr;
	string iccodeTemp;
	while (!db.HX_pRecordset->adoEOF) {
		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = i;				// 行
		_variant_t var;

		char iccode[50];
		memset(iccode, 0, 50);
		var = db.HX_pRecordset->GetCollect("iccode");
		getSqlSetColumn(var, iccode);
		iccodeTemp = iccode;
		if (iccodeSetStr.find(iccodeTemp) != -1) {
			db.HX_pRecordset->MoveNext();
			continue;
		}
		iccodeSetStr = iccodeSetStr +"|"+ iccodeTemp;

		char id[50];
		memset(id, 0, 50);
		var = db.HX_pRecordset->GetCollect("id");
		getSqlSetColumn(var, id);
		lvItem.iSubItem = 0;			// 列1 序号
		lvItem.pszText = id;			// 内容
		ListView_InsertItem(Page[pageIndex], (LPARAM)&lvItem);

		lvItem.iSubItem = 1;				// 列2 iccode
		lvItem.pszText = iccode;
		ListView_SetItem(Page[pageIndex], (LPARAM)&lvItem);

		char name[50];
		memset(name, 0, 50);
		var = db.HX_pRecordset->GetCollect("name");
		getSqlSetColumn(var, name);
		lvItem.iSubItem = 2;				// 列3 公司名 
		lvItem.pszText = name;
		ListView_SetItem(Page[pageIndex], (LPARAM)&lvItem);

		char mobile[50];
		memset(mobile, 0, 50);
		var = db.HX_pRecordset->GetCollect("mobile");
		getSqlSetColumn(var, mobile);
		lvItem.iSubItem = 3;					// 列4 联系电话
		lvItem.pszText = mobile;
		ListView_SetItem(Page[pageIndex], (LPARAM)&lvItem);

		char ctype[50];
		var = db.HX_pRecordset->GetCollect("ctype");
		getSqlSetColumn(var, ctype);
		lvItem.iSubItem = 4;					// 列5 供应商类型1个人 2公司
		int ctypeInt = atoi(ctype);
		if (ctypeInt == 1) {
			lvItem.pszText = "个人";
		}
		else if (ctypeInt == 2) {
			lvItem.pszText = "公司";
		}
		else {
			lvItem.pszText = "";
		}
		ListView_SetItem(Page[pageIndex], (LPARAM)&lvItem);

		char address[500];
		memset(address, 0, 500);
		var = db.HX_pRecordset->GetCollect("address");
		getSqlSetColumn(var, address);
		lvItem.iSubItem = 5;					// 列4 联系电话
		lvItem.pszText = address;
		ListView_SetItem(Page[pageIndex], (LPARAM)&lvItem);

		db.HX_pRecordset->MoveNext();
		i++;
	}
}

#pragma endregion  
#pragma region TabControl  
HWND InitTabControlForUser(HWND hParentWnd, HWND hWndFocus, LPARAM lParam)
{
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccx.dwICC = ICC_TAB_CLASSES;
	if (!InitCommonControlsEx(&iccx))
		return FALSE;
	RECT rc;
	GetClientRect(hParentWnd, &rc);
	// Set the font of the tabs to a more typical system GUI font  
	SendMessage(hParentWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);
	//根据页面来初始化页面  
	RegisterPageClass();
	addPage(hParentWnd, TEXT("查询结果"), SearchProc);
	initHeadUser(Page[0]);// 初始化表格头
	addPage(hParentWnd, TEXT("未结算供应商"), NoPayProc);
	initHeadUser(Page[1]);// 初始化表格头
	addPage(hParentWnd, TEXT("已结算供应商"), PayedProc);
	initHeadUser(Page[2]);// 初始化表格头
	ShowWindow(Page[1], SW_SHOW);//显示第一个页面
	TabCtrl_SetCurSel(hParentWnd, 1);

	// 未结算供应商
	DBUtils db;
	char sql[500];
	string sqlstring = SQL_V_SUPPLIERS_FOR_NOPAY;
	sqlstring = sqlstring.replace(sqlstring.find("<COMPANY_ID>"),12,LOGIN_COMPANYID);
	strcpy(sql,sqlstring.c_str());
	db.getRecordRet(sql);
	initUserData(db, 1);

	// 结算供应商
	sqlstring = SQL_V_SUPPLIERS_FOR_PAY;
	sqlstring = sqlstring.replace(sqlstring.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
	strcpy(sql, sqlstring.c_str());
	db.getRecordRet(sql);
	initUserData(db, 2);
	db.ExitConnect();

	return hParentWnd;
}

#pragma endregion  
#pragma region TabControl  
HWND InitTabControlForStorage(HWND hParentWnd, HWND hWndFocus, LPARAM lParam)
{
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccx.dwICC = ICC_TAB_CLASSES;
	if (!InitCommonControlsEx(&iccx))
		return FALSE;
	RECT rc;
	GetClientRect(hParentWnd, &rc);
	SendMessage(hParentWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);
	RegisterPageClass();
	addPage(hParentWnd, TEXT("未结算磅单信息"), SearchProc);
	initHeadStorage(Page[3]);
	addPage(hParentWnd, TEXT("已结算磅单信息"), NoPayProc);
	initHeadStorage(Page[4]);
	ShowWindow(Page[3], SW_SHOW);//显示未结算磅单信息
	return hParentWnd;
}




#pragma endregion  
#pragma region TabControl  
HWND InitTabControlForPort(HWND hParentWnd, HWND hWndFocus, LPARAM lParam)
{
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccx.dwICC = ICC_TAB_CLASSES;
	if (!InitCommonControlsEx(&iccx))
		return FALSE;
	RECT rc;
	GetClientRect(hParentWnd, &rc);
	// Set the font of the tabs to a more typical system GUI font  
	SendMessage(hParentWnd, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0);
	//根据页面来初始化页面  
	RegisterPageClass();
	addPageForPort(hParentWnd, TEXT("大磅"), SearchProc, tabBDlg);
	addPageForPort(hParentWnd, TEXT("小磅"), SearchProc, tabSDlg);
	TabCtrl_SetCurSel(hParentWnd, 0);
	return hParentWnd;
}

double initClickStorage(DBUtils db,char* iccode) {
	_variant_t var;
	int i = 0;
	int i3 = 0;
	int i4 = 0;
	int flagIspay=-1;
	double totalMoneyDisplay=0;
	while (!db.HX_pRecordset->adoEOF) {
		int pageIndex = 3;

		char statusStr[10];
		char statusResult[10];
		int status = 0;
		memset(statusStr, 0, 10);
		memset(statusResult, 0, 10);
		var = db.HX_pRecordset->GetCollect("status");
		getSqlSetColumn(var, statusStr);
		status = atoi(statusStr);
		if (status == 99) {
			db.HX_pRecordset->MoveNext();
			continue;
		}

		char ispay[50];
		var = db.HX_pRecordset->GetCollect("ispay");

		if (var.vt != VT_NULL) {
			string strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
			flagIspay = atoi(strValue.c_str());
			strcpy(ispay, strValue.c_str());
		}
		if (flagIspay == 1) {
			i = i4;
			i4++;
			pageIndex = 4;
		}
		else if (flagIspay == 0) {
			i = i3;
			i3++;
			pageIndex = 3;
		}
		if (i>20) {
			db.HX_pRecordset->MoveNext();
			continue;
		}

		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = i;				// 行

		char id[50];
		memset(id, 0, 50);
		var = db.HX_pRecordset->GetCollect("id");
		getSqlSetColumn(var, id);
		lvItem.iSubItem = 0;			// 列0 序号
		lvItem.pszText = id;			// 内容
		ListView_InsertItem(Page[pageIndex], (LPARAM)&lvItem);

		char eqtype[10];
		var = db.HX_pRecordset->GetCollect("eqtype");
		getSqlSetColumn(var, eqtype);
		lvItem.iSubItem = 1;				// 列2 iccode
		lvItem.pszText = eqtype;
		ListView_SetItem(Page[pageIndex], (LPARAM)&lvItem);

		lvItem.iSubItem = 2;				// 列2 iccode
		lvItem.pszText = iccode;
		ListView_SetItem(Page[pageIndex], (LPARAM)&lvItem);

		char name[50];
		var = db.HX_pRecordset->GetCollect("name");
		getSqlSetColumn(var, name);
		lvItem.iSubItem = 3;				// 列3 供货人
		lvItem.pszText = name;
		ListView_SetItem(Page[pageIndex], (LPARAM)&lvItem);

		char gmtCreated[50];
		var = db.HX_pRecordset->GetCollect("gmt_created");
		getSqlSetColumn(var, gmtCreated);
		lvItem.iSubItem = 4;					// 列4 进场时间
		lvItem.pszText = gmtCreated;
		ListView_SetItem(Page[pageIndex], (LPARAM)&lvItem);

		char gw[20];
		memset(gw, 0, 20);
		var = db.HX_pRecordset->GetCollect("gw");
		getSqlSetColumn(var, gw);
		lvItem.iSubItem = 5;					// 列5 毛重
		if (gw[0] == '.') {
			string addZ = gw;
			addZ = "0" + addZ;
			memset(gw, 0, 20);
			strcpy(gw,addZ.c_str());
		}
		lvItem.pszText = gw;
		ListView_SetItem(Page[pageIndex], (LPARAM)&lvItem);

		char dbTare[50]="";
		var = db.HX_pRecordset->GetCollect("tare");
		getSqlSetColumn(var, dbTare);
		lvItem.iSubItem = 6;					// 列6 皮重
		if (dbTare[0] == '.') {
			string addZ = dbTare;
			addZ = "0" + addZ;
			memset(dbTare, 0, 20);
			strcpy(dbTare, addZ.c_str());
		}
		lvItem.pszText = dbTare;
		ListView_SetItem(Page[pageIndex], (LPARAM)&lvItem);

		char pname[50];
		memset(pname, 0, 50);
		var = db.HX_pRecordset->GetCollect("pname");
		getSqlSetColumn(var, pname);
		lvItem.iSubItem = 7;					//列7 品名
		lvItem.pszText = pname;
		ListView_SetItem(Page[pageIndex], (LPARAM)&lvItem);

		char price[50];
		memset(price, 0, 50);
		var = db.HX_pRecordset->GetCollect("price");
		getSqlSetColumn(var, price);
		lvItem.iSubItem = 8;					//列8 单价
		if (price[0] == '.') {
			string addZ = price;
			addZ = "0" + addZ;
			memset(price, 0, 20);
			strcpy(price, addZ.c_str());
		}
		lvItem.pszText = price;
		ListView_SetItem(Page[pageIndex], (LPARAM)&lvItem);

		char nw[50];
		memset(nw, 0, 50);
		var = db.HX_pRecordset->GetCollect("nw");
		getSqlSetColumn(var, nw);
		lvItem.iSubItem = 9;					//列9 净重
		if (nw[0] == '.') {
			string addZ = nw;
			addZ = "0" + addZ;
			memset(nw, 0, 20);
			strcpy(nw, addZ.c_str());
		}
		lvItem.pszText = nw;
		ListView_SetItem(Page[pageIndex], (LPARAM)&lvItem);
		char total[50];
		memset(total, 0, 50);
		var = db.HX_pRecordset->GetCollect("total");
		getSqlSetColumn(var, total);
		lvItem.iSubItem = 10;					//列10 总额
		if (total[0] == '.') {
			string addZ = total;
			addZ = "0" + addZ;
			memset(total, 0, 20);
			strcpy(total, addZ.c_str());
		}
		lvItem.pszText = total;
		ListView_SetItem(Page[pageIndex], (LPARAM)&lvItem);
		if (flagIspay != 1) {
			totalMoneyDisplay = totalMoneyDisplay + atof(total);
		}

	/*	char statusStr[10];
		char statusResult[10];
		int status = 0;
		memset(statusStr, 0, 10);
		memset(statusResult, 0, 10);
		var = db.HX_pRecordset->GetCollect("status");
		getSqlSetColumn(var, statusStr);
		status = atoi(statusStr);*/
		lvItem.iSubItem = 11;					//列11 状态
		if (status == 0) {
			sprintf(statusResult, "%s", "过磅中");
		}
		else if (status == 1) {
			sprintf(statusResult, "%s", "过磅完成");
		}
		else if (status == 2) {
			sprintf(statusResult, "%s", "定价完成");
		}
		else if (status == 3) {
			sprintf(statusResult, "%s", "完成皮重");
		}
		else if (status == 4) {
			sprintf(statusResult, "%s", "结算完成");
		}
		else if (status == 99) {
			sprintf(statusResult, "%s", "作废");
		}
		lvItem.pszText = statusResult;
		ListView_SetItem(Page[pageIndex], (LPARAM)&lvItem);
		db.HX_pRecordset->MoveNext();
		i++;
	}
	return totalMoneyDisplay;
}

void getSuppliersSelfidByIccode(char * iccode,char * selfid) {
	DBUtils db;
	char sql[200];
	string sqlString = SQL_SUPPLIERS_BY_ICCODE;
	sqlString = sqlString.replace(sqlString.find("<ICCODE>"), 8, iccode);
	strcpy(sql, sqlString.c_str());
	db.getRecordRet(sql);
	while (!db.HX_pRecordset->adoEOF)
	{
		_variant_t var = db.HX_pRecordset->GetCollect("selfid");
		string strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
		strcpy(selfid, strValue.c_str());
		db.HX_pRecordset->MoveNext();
	}
	db.ExitConnect();
}

void setTotalMoney(double totalMoneyDispaly,HDC hdc) {
	HFONT   hFont;
	LOGFONT   logfont;       //改变输出字体
	ZeroMemory(&logfont, sizeof(LOGFONT));
	logfont.lfCharSet = GB2312_CHARSET;
	logfont.lfHeight = -28;      //设置字体的大小
	hFont = CreateFontIndirect(&logfont);
	SetBkColor(hdc, RGB(240, 240, 240));
	////SetBkMode(hdc, TRANSPARENT); 透明样式，没用。适合固定展示，不适合经常变动的数据
	SelectObject(hdc, hFont);
	char zsBuffer[40];
	SetTextColor(hdc, RGB(255, 140, 102)); // 颜色
	char blank[12] = "          ";
	TextOut(hdc, 685, 600, blank, 10);
	TextOut(hdc, 685, 600, zsBuffer, sprintf(zsBuffer, TEXT("%.1f元"), totalMoneyDispaly));
}


/*
大磅 毛重按钮点击
1.根据上表格选中的供应商获得iccode
2.添加一条记录进入storage表
3.未结算磅单信息添加
*/ 
int addOneStorage(HWND hDlg,string eqtype) {
	HWND hTab = GetDlgItem(hDlg, IDC_TAB_USER);
	int sel = TabCtrl_GetCurSel(hTab);
	char iccode[50];
	string sqlString;
	char sql[500];
	MD5 md5;
	int i=0;
	// http
	HttpUtils hu;
	char domain[100];
	char url[100];
	char param[300];

	char suppliersSelfidInsert[200] = "";	//
	char gwInsert[200] = "";

	for (i = 0; i < ListView_GetItemCount(Page[sel]); i++)
	{
		if (ListView_GetItemState(Page[sel], i, LVIS_SELECTED) == LVIS_SELECTED || ListView_GetCheckState(Page[sel], i)) {
			LVITEM lvi;
			lvi.iItem = i;
			lvi.iSubItem = 1;
			lvi.mask = LVIF_TEXT;
			lvi.pszText = iccode;
			lvi.cchTextMax = 1024;
			ListView_GetItem(Page[sel], &lvi);
			sqlString = SQL_STORAGE_INSERT;
			time_t tt = time(NULL);//这句返回的只是一个时间cuo
			tm* t = localtime(&tt);
			char selfid[50];			// self_id 未加密
			int randInt = (rand() % 90) + 10;				// 10-99 的随机数
			sprintf(selfid, "%d%02d%02d%02d%02d%02d%d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, randInt); // 获取入库单selfid、
			string tempString = selfid;
			tempString = md5.EncodeForShort(tempString);
			sqlString.replace(sqlString.find("<SELFID>"), 8, tempString);

			string groupId = LOGIN_GROUPID;
			sqlString = sqlString.replace(sqlString.find("<GROUP_ID>"), 10, groupId);

			sqlString = sqlString.replace(sqlString.find("<EQTYPE>"), 8, eqtype);

			string companyId = LOGIN_COMPANYID;
			sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"), 12, companyId);

			char codeInsert[50];				// youle
			memset(codeInsert, 0, 50);
			strcpy(codeInsert, selfid);
			sqlString = sqlString.replace(sqlString.find("<CODE>"), 6, codeInsert);

			memset(suppliersSelfidInsert, 0, 50);
			getSuppliersSelfidByIccode(iccode, suppliersSelfidInsert);
			sqlString = sqlString.replace(sqlString.find("<SUPPLIERS_SELFID>"), 18, suppliersSelfidInsert);

			memset(gwInsert,0,50);
			
			if (eqtype=="小磅") {
				mySerialPort2.ReadOne(gwInsert);
			}
			else {
				mySerialPort.ReadOne(gwInsert);
			}
			sqlString = sqlString.replace(sqlString.find("<GW>"), 4, gwInsert);
			// 1过磅完成
			sqlString = sqlString.replace(sqlString.find("<STATUS>"), 8, "1");
			string priceUsersSelfid = LOGIN_SELFID;	//youle
			sqlString = sqlString.replace(sqlString.find("<PRICE_USERS_SELFID>"), 20, priceUsersSelfid);
			//	0未支付
			sqlString = sqlString.replace(sqlString.find("<ISPAY>"), 7, "0");
			memset(sql, 0, 500);
			strcpy(sql, sqlString.c_str());
			DBUtils db;
			db.addRecord(sql);

			// 上传至服务器
			string postData = URL_DATA_UPLOAD_STORAGE_DOMAIN;
			strcpy(domain,postData.c_str());
			postData = URL_DATA_UPLOAD_STORAGE_URL;
			strcpy(url, postData.c_str());
			postData = URL_DATA_UPLOAD_STORAGE_PARAM;
			sqlString = SQL_STORAGE_BY_SELFID;											// selfid 获取新建的入库单
			sqlString = sqlString.replace(sqlString.find("<SELFID>"),8, tempString);
			strcpy(sql,sqlString.c_str());
			db.getRecordRet(sql);
			string uploadData = bulidUploadStorageData(db);
			postData = postData.replace(postData.find("<DATA>"),6, uploadData);
			postData = postData.replace(postData.find("<COMPANY_ID>"), 12,LOGIN_COMPANYID);
			strcpy(param, postData.c_str());
			hu.httpPost(_T(domain), _T(url), _T(param));
			//

		}
	}
	DBUtils db;
	HWND storeHwnd = GetDlgItem(hDlg, IDC_TAB_STORAGE);
	SendMessage(Page[3], LVM_DELETEALLITEMS, 0, 0);// 清空
	sqlString = SQL_V_STORAGELIST_BY_ICCODE;
	sqlString = sqlString.replace(sqlString.find("<ICCODE>"), 8, iccode);
	sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
	strcpy(sql, sqlString.c_str());
	db.getRecordRet(sql);
	initClickStorage(db, iccode);			// 搜索数据填充下表
	db.ExitConnect();
	ShowWindow(Page[3], TRUE);
	ShowWindow(Page[4], FALSE);
	TabCtrl_SetCurSel(storeHwnd, 0);


	return i;
}

void changeNw(HWND hDlg,int eqtype,bool zeroFlag,HDC hdc) {
	HWND hTab = GetDlgItem(hDlg, IDC_TAB_STORAGE);
	DBUtils db;
	int sel = TabCtrl_GetCurSel(hTab);
	if (sel==1) {
		MessageBox(hDlg,TEXT("订单已经结算！无法进行此操作"),TEXT("提示"),0);
	}
	sel = sel + 3;

	string sqlString;
	char sql[500];
	MD5 md5;
	int i = 0;

	string iccodeString;
	
	double totalMoneyDispaly = 0;

	HttpUtils hu;
	char domain[100];
	char url[100];
	char param[300];

	for (i = 0; i < ListView_GetItemCount(Page[sel]); i++)
	{
		if (ListView_GetItemState(Page[sel], i, LVIS_SELECTED) == LVIS_SELECTED || ListView_GetCheckState(Page[sel], i)) {
			char status[20];
			ListView_GetItemText(Page[sel], i, 11, status, 20);
			string statusStr = status;
			sqlString = SQL_STORAGE_UPDATE_TO_WEIGHT_OVER;
			char id[20];
			ListView_GetItemText(Page[sel], i, 0, id, 20);
			sqlString = sqlString.replace(sqlString.find("<ID>"), 4, id);
			char iccode[20];
			ListView_GetItemText(Page[sel], i, 2, iccode, 20);
			iccodeString = iccode;
			char gw[20];
			ListView_GetItemText(Page[sel], i, 5, gw, 20);
			char price[20];
			ListView_GetItemText(Page[sel], i, 8, price, 20);
			char tare[100]="0.0";
			if (!zeroFlag) {
				if (eqtype == 1) {
					mySerialPort.ReadOne(tare);
				}
				else if (eqtype == 2) {
					mySerialPort2.ReadOne(tare);
				}
			}
			char nw[20];
			//string tareSql = tare;
			//double total = atof(gw);
			//double lost = atof(tare);
			//double last = total - lost;
			//sprintf(nw,"%f",last);
			//double priceMoney = atof(price);
			//double totalMoney = last * priceMoney;
			//totalMoney = round(totalMoney * 100) / 100;
			char totalPrice[20];
			//sprintf(totalPrice,"%f",totalMoney);
			getTotalPrice(gw,tare,nw,price,totalPrice);
			sqlString = sqlString.replace(sqlString.find("<NW>"),4,nw);
			sqlString = sqlString.replace(sqlString.find("<TARE>"), 6, tare);
			sqlString = sqlString.replace(sqlString.find("<TOTAL>"), 7, totalPrice);
			strcpy(sql, sqlString.c_str());
			db.addRecord(sql);

			HWND storeHwnd = GetDlgItem(hDlg, IDC_TAB_STORAGE);
			SendMessage(Page[3], LVM_DELETEALLITEMS, 0, 0);// 清空
			sqlString = SQL_V_STORAGELIST_BY_ICCODE;
			sqlString = sqlString.replace(sqlString.find("<ICCODE>"), 8, iccodeString);
			sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
			strcpy(sql, sqlString.c_str());
			db.getRecordRet(sql);
			char iccodeDB[20];
			strcpy(iccodeDB, iccodeString.c_str());
			totalMoneyDispaly = initClickStorage(db, iccodeDB);			// 搜索数据填充下表
			db.ExitConnect();
			ShowWindow(Page[3], TRUE);
			ShowWindow(Page[4], FALSE);
			TabCtrl_SetCurSel(storeHwnd, 0);

			// 上传至服务器
			string postData = URL_DATA_UPLOAD_STORAGE_DOMAIN;
			strcpy(domain, postData.c_str());
			postData = URL_DATA_UPLOAD_STORAGE_URL;
			strcpy(url, postData.c_str());
			postData = URL_DATA_UPLOAD_STORAGE_PARAM;
			sqlString = SQL_STORAGE_BY_ID;											// selfid 获取新建的入库单
			sqlString = sqlString.replace(sqlString.find("<ID>"), 4, id);
			strcpy(sql, sqlString.c_str());
			db.getRecordRet(sql);
			string uploadData = bulidUploadStorageData(db);
			postData = postData.replace(postData.find("<DATA>"), 6, uploadData);
			postData = postData.replace(postData.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
			strcpy(param, postData.c_str());
			hu.httpPost(_T(domain), _T(url), _T(param));

		}
	}

	setTotalMoney(totalMoneyDispaly,hdc);
	
}

/*
	通过http判定定价按钮和积分结算按钮
*/
void validAuthForWeb(bool* priceFlag,bool* scoreFlag) {
	HttpUtils hu;
	do {
		string urlString = URL_GET_AUTH;
		char url[200];
		urlString = urlString.replace(urlString.find("<USER_SELFID>"),13,LOGIN_SELFID);
		urlString = urlString.replace(urlString.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
		strcpy(url, urlString.c_str());
		string httpResult = hu.httpGet(url);
		Json::Value value;
		Json::Reader reader;
		if (!reader.parse(httpResult, value)) {
			break;
		}
		int isprice = value["isprice"].asInt();
		if (isprice > 0) {
			priceFlag[0] = true;
		}
		else {
			priceFlag[0] = false;
		}
		int isscore = value["isscore"].asInt();
		if (isscore > 0) {
			scoreFlag[0] = true;
		}
		else {
			scoreFlag[0] = false;
		}
	} while (false);
}

// Message handler for about box 过磅界面
INT_PTR CALLBACK DialogStorage(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	/* 初始化磅秤句柄  */
	char str[100];
	HDC hdc;
	hdc = GetDC(hDlg);
	PAINTSTRUCT ps;

	HWND hbn = (HWND)lParam;
	RECT rc;
	TCHAR text[64];
	HWND hBn;
	int nWidth,nHeight;
	//
	LPDRAWITEMSTRUCT pdis;
	HDC hMemDc;
	HFONT   hFont;
	HWND hTab;
	int sel;

	// search
	HWND searchHwnd;
	HWND abc;

	// sql common
	DBUtils db;
	string sqlString;
	char sql[500];

	// md5
	MD5 md5;

	// tab 选择的行和列
	int iItem;
	int iSubItem;

	// 通信设置句柄
	HWND portHwnd;

	double totalMoneyDispaly=0;

	bool treadFlag = true;

	// default size
	HWND hwndPay = GetDlgItem(hDlg, IDC_BUTTON_PAY);
	RECT rcDePay, rcDeWPay;

	// seaial port x and y
	int x1 = 150, y1 = 25, y2 = 340;       // 未连接的横纵坐标 

	//	
	HWND hcombo;
	char comboChar[20];

	// ic param
	char iccode[17];
	bool hasRead[1];
	char errorMsg[200];

	// 毛重参数
	int nwCount=0;
	int nwSel;


	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		if (!mySerialPort.isClose|| !mySerialPort2.isClose) {
			// 定时器 太卡。。取消
			//SetTimer(hDlg, ID_TIMER, 2000, TimerSerialPort);
			// 线程
			if (!portThreadOpen) {
				threadDisHwnd = hDlg;
				HANDLE hThread = CreateThread(NULL, 0, TimerSerialPortThread, NULL, 0, NULL);
				CloseHandle(hThread);
			}
		}

		// 按钮自定义样式
		hBn = GetDlgItem(hDlg, IDC_BUTTON_PAY);
		SetWindowLong(hBn, GWL_STYLE, GetWindowLong(hBn, GWL_STYLE) | BS_OWNERDRAW);
		hBn = GetDlgItem(hDlg, IDC_BUTTON_SET_MONEY);
		SetWindowLong(hBn, GWL_STYLE, GetWindowLong(hBn, GWL_STYLE) | BS_OWNERDRAW);

		// 显示tab标签
		iPageIndex = 0; // 下标数重置
		hTab = GetDlgItem(hDlg, IDC_TAB_USER); 
		InitTabControlForUser(hTab, NULL, lParam);//获取供应商 tab句柄，然后初始化list控件
		hTab = GetDlgItem(hDlg, IDC_TAB_STORAGE);
		InitTabControlForStorage(hTab, NULL, lParam);//获取磅单 tab句柄，然后初始化list控件

		CHOOSE_ICCODE = "";
	
		// 验证是否有 积分结算和本地定价的弹框按钮
		bool priceFlag[1],scoreFlag[1];
		validAuthForWeb(priceFlag, scoreFlag);
		if (scoreFlag[0]) {
			HWND hwndScore =  GetDlgItem(hDlg, IDC_BUTTON_ONLINE_SCORE);
			ShowWindow(hwndScore, SW_SHOW);
		}
		if (priceFlag[0]) {
			HWND hwndPrice = GetDlgItem(hDlg, IDC_BUTTON_SET_MONEY);
			ShowWindow(hwndPrice, SW_SHOW);
		}

		// 初始化IDC_COMBO_SELECT_TYPE ，检索类别 
		hcombo = GetDlgItem(hDlg, IDC_COMBO_SELECT_TYPE);
		sprintf(comboChar, "%s","供应商编号" );
		SendMessage(hcombo, CB_INSERTSTRING, 0, (LPARAM)comboChar);
		sprintf(comboChar, "%s", "手机号");
		SendMessage(hcombo, CB_INSERTSTRING, 1, (LPARAM)comboChar);
		sprintf(comboChar, "%s", "联系人");
		SendMessage(hcombo, CB_INSERTSTRING, 2, (LPARAM)comboChar);
		ComboBox_SetCurSel(hcombo, 0);


		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK ||LOWORD(wParam) == IDCANCEL)  
		{			
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;			
		}
		if (LOWORD(wParam) == IDM_ADMIN_URL)
		{
			// 进入后台数据管理
			HINSTANCE hRslt = ShellExecute(NULL, TEXT("open"), TEXT("http://dibang.zz91.com"), NULL, NULL, SW_SHOWNORMAL);
			assert(hRslt>(HINSTANCE)HINSTANCE_ERROR);
		}
		if (LOWORD(wParam) == IDM_SUPPLIERS_ADD)
		{
			// 新建供应商

		}
		if (LOWORD(wParam) == IDM_SUPPLIERS_MANAGE) 
		{
			// 管理供应商
		}
		if (LOWORD(wParam) == IDM_PORT_CONFIG) 
		{
			// 未登录情况判断
			if (LOGIN_COMPANYID[0] == '\0') {
				MessageBox(NULL, TEXT("请先登录，再进行此操作"), TEXT("提示"), 0);
				break;
			}
			HWND hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_PORT_CONFIG), hDlg, DialogPortConfig);
			ShowWindow(hDialog, SW_SHOW);
		}
		if (LOWORD(wParam) == IDM_EXIT)
		{
			// 未登录情况判断
			if (MessageBox(hDlg, TEXT("确定要退出系统？"), TEXT("提示"), MB_OKCANCEL) == IDOK)
			{
				EndDialog(hDlg, LOWORD(wParam));
				DestroyWindow(hWndMain);
			}
		}
		if (LOWORD(wParam) == IDC_BUTTON_PAY|| LOWORD(wParam) == IDC_BUTTON_ONLINE_SCORE)
		{
			do
			{
				HttpUtils hu;
				hTab = GetDlgItem(hDlg, IDC_TAB_STORAGE);
				int sel = TabCtrl_GetCurSel(hTab);
				char setMoneyId[10];
				if (sel != 0) {
					MessageBox(hDlg, TEXT("已经完成的订单不可再次结算"), TEXT("提示"), 0);
					break;
				}
				string key = "";
				if (LOWORD(wParam) == IDC_BUTTON_PAY) {
					key = "确定要现金结算吗？";
				}
				else if (LOWORD(wParam) == IDC_BUTTON_ONLINE_SCORE) {
					key= "确定要积分结算吗？";
				}
				if (MessageBox(hDlg, TEXT(key.c_str()), TEXT("提示"), MB_OKCANCEL) == IDCANCEL) {
					break;
				}
				sel = sel + 3;// 获取页面句柄下标 
				int i, suc=0;
				for (i = 0; i < ListView_GetItemCount(Page[sel]); i++)
				{
					char tare[20];
					ListView_GetItemText(Page[sel], i, 6, tare, 20);
					if (tare[0] == '\0') {
						MessageBox(hDlg, TEXT("还有磅单皮重还未确定，请选确定皮重,再结算"), TEXT("提示"), 0);
						return 0;
					}
					char price[20];
					ListView_GetItemText(Page[sel], i, 10, price, 20);
					float fprice = atof(price);
					if (fprice <= 0) {
						MessageBox(hDlg, TEXT("还有磅单未定价！请定价后再结算"), TEXT("提示"), 0);
						return  0;
					}
				}
				if (i == 0) {
					MessageBox(hDlg, TEXT("您没有待结算的订单！请新建订单后再结算"), TEXT("提示"), 0);
					return  0;
				}
				for (i = 0; i < ListView_GetItemCount(Page[sel]); i++)
				{
					// 现金结算，积分结算
					if (LOWORD(wParam) == IDC_BUTTON_PAY) {
						sqlString = SQL_STORAGE_UPDATE_TO_TRADE_OVER;
					}
					else if (LOWORD(wParam) == IDC_BUTTON_ONLINE_SCORE) {
						sqlString = SQL_STORAGE_UPDATE_TO_TRADE_OVER_ONLINE;
					}
					ListView_GetItemText(Page[sel], i, 0, setMoneyId, 10);
									
					sqlString = sqlString.replace(sqlString.find("<PAY_USERS_SELFID>"), 18, LOGIN_SELFID);
					sqlString = sqlString.replace(sqlString.find("<ID>"), 4, setMoneyId);
					strcpy(sql, sqlString.c_str());
					db.addRecord(sql);// 修改数据

					char domain[200], url[200], param[20480];
					string postData = URL_DATA_UPLOAD_STORAGE_DOMAIN;// url domain部分
					strcpy(domain, postData.c_str());
					postData = URL_DATA_UPLOAD_STORAGE_URL; // url后半段
					strcpy(url, postData.c_str());
					postData = URL_DATA_UPLOAD_STORAGE_PARAM;// 参数模板

					string selectString = SQL_STORAGE_BY_ID; // 根据id检索所有字段数据
					char selectChar[200];
					selectString = selectString.replace(selectString.find("<ID>"), 4, setMoneyId);
					strcpy(selectChar, selectString.c_str());
					db.getRecordRet(selectChar);// 结果集返回 db
					string uploadData = bulidUploadStorageData(db); // 使用db结果集构造url的参数
					postData = postData.replace(postData.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);	// 登陆公司id
					postData = postData.replace(postData.find("<DATA>"), 6, uploadData);				// 参数替换
					strcpy(param, postData.c_str());
					hu.httpPost(_T(domain), _T(url), _T(param));
					suc++;
				}

				if (suc>0)
				{
					MessageBox(hDlg, TEXT("订单结算成功！"), TEXT("提示"), 0);
					// 重新获取两个列表并将焦点切换至结算成功的标签 
					HWND storeHwnd = GetDlgItem(hDlg, IDC_TAB_STORAGE);
					SendMessage(Page[3], LVM_DELETEALLITEMS, 0, 0);// 清空
					SendMessage(Page[4], LVM_DELETEALLITEMS, 0, 0);// 清空
					sqlString = SQL_V_STORAGELIST_BY_ICCODE;
					sqlString = sqlString.replace(sqlString.find("<ICCODE>"), 8, CHOOSE_ICCODE);
					sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
					strcpy(sql, sqlString.c_str());
					db.getRecordRet(sql);
					char iccode[20];
					strcpy(iccode, CHOOSE_ICCODE.c_str());
					totalMoneyDispaly = initClickStorage(db, iccode);			// 搜索数据填充下表
					db.ExitConnect();
					ShowWindow(Page[3], FALSE);
					ShowWindow(Page[4], TRUE);
					TabCtrl_SetCurSel(storeHwnd, 1);
					setTotalMoney(totalMoneyDispaly, hdc);// 总金额重新计算显示
					
				}

			} while (false);
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_BUTTON_SET_MONEY) {
			// 定价弹框
			hTab = GetDlgItem(hDlg, IDC_TAB_STORAGE);
			int sel = TabCtrl_GetCurSel(hTab);
			char setMoneyId[10];
			char setMoneyStatus[20];
			char setMoneyContact[20];
			do
			{
				if (sel != 0) {
					MessageBox(hDlg, TEXT("已经结算的订单不可再次定价"), TEXT("提示"), 0);
					break;
				}
				sel = sel + 3;// 获取页面句柄下标 
				int i;
				for (i = 0; i < ListView_GetItemCount(Page[sel]); i++)
				{
					if (ListView_GetItemState(Page[sel], i, LVIS_SELECTED) == LVIS_SELECTED || ListView_GetCheckState(Page[sel], i)) {
						ListView_GetItemText(Page[sel], i, 0, setMoneyId, 10);
						ListView_GetItemText(Page[sel], i, 11, setMoneyStatus, 20);
						ListView_GetItemText(Page[sel], i, 3, setMoneyContact, 20);
						break;
					}
				}
				int id = atoi(setMoneyId);
				if (id ==0) {
					MessageBox(hDlg, TEXT("请选择要定价的信息，再进行定价"), TEXT("提示"), 0);
					break;
				}
				//string status = setMoneyStatus;
				/*if (status!="过磅完成") {
					MessageBox(hDlg, TEXT("信息未完成过磅或已定价完成，如需重新定价，请作废重来"), TEXT("提示"), 0);
					break;
				}*/
				STORAGE_SET_MONEY_ID = atoi(setMoneyId);
				STORAGE_SET_MONEY_NAME = setMoneyContact;
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_SET_MONEY), hDlg, DialogSetMoney);
			} while (false);
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_BUTTON_SEARCH)
		{
			// 搜索iccode
			searchHwnd = GetDlgItem(hDlg,IDC_EDIT_SEARCHKEY);
			char searchKey[50];
			GetWindowText(searchHwnd, searchKey, 50);
			if (searchKey[0]=='\0') {
				MessageBox(NULL, TEXT("搜索的内容不可为空"), TEXT("提示"), 0);
			}
			else {
				SendMessage(Page[0], LVM_DELETEALLITEMS, 0, 0);// 清空listview
				HWND typeHwnd = GetDlgItem(hDlg, IDC_COMBO_SELECT_TYPE);
				int type = ComboBox_GetCurSel(typeHwnd);
				
				if (type==0) {
					// 供应商编号
					sqlString = SQL_SUPPLIERS_BY_ICCODE;
					sqlString = sqlString.replace(sqlString.find("<ICCODE>"), 8, searchKey);
				}
				if (type == 1) 
				{
					// 手机号码 SQL_SUPPLIERS_BY_MOBILE = "select * from suppliers where mobile = '<MOBILE>' order by id desc";
					sqlString = SQL_SUPPLIERS_BY_MOBILE;
					sqlString = sqlString.replace(sqlString.find("<MOBILE>"), 8, searchKey);
				}
				if (type == 2)
				{
					// 联系人 SQL_SUPPLIERS_BY_CONTACT = "select * from suppliers where contact like '*<CONTACT>*' order by id desc";
					sqlString = SQL_SUPPLIERS_BY_CONTACT;
					sqlString = sqlString.replace(sqlString.find("<CONTACT>"), 9, searchKey);
				}
				// 进行检索 检索结果填充后，然后show tab第一页
				DBUtils db;
				strcpy(sql, sqlString.c_str());
				db.getRecordRet(sql);
				initUserData(db, 0);
				ShowWindow(Page[0], TRUE);
				ShowWindow(Page[1], FALSE);
				ShowWindow(Page[2], FALSE);
				hTab = GetDlgItem(hDlg, IDC_TAB_USER);
				TabCtrl_SetCurSel(hTab, 0);
				db.ExitConnect();
			}
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_BUTTON_READ_IC)
		{
			//读ic卡
			/*char iccode[17];
			bool hasRead[1];
			char errorMsg[200];*/
			ReadICCard(iccode, hasRead, errorMsg);
			if (hasRead[0]) {
				searchHwnd = GetDlgItem(hDlg, IDC_EDIT_SEARCHKEY);
				SetWindowText(searchHwnd, iccode);
			}
			else {
				MessageBox(hDlg, TEXT(errorMsg), TEXT("提示"), NULL);
			}
			
			return (INT_PTR)TRUE;
		}	
		if (LOWORD(wParam) == IDC_BUTTON_ADD_NEW_GUEST)
		{
			// 添加临时供应商
			HttpUtils hu;
			DBUtils db;
			sqlString = SQL_SUPPLIERS_FOR_TOP1;
			strcpy(sql, sqlString.c_str());
			db.getRecordRet(sql);
			int maxId=0;
			string strValue;
			while (!db.HX_pRecordset->adoEOF) {
				_variant_t var = db.HX_pRecordset->GetCollect("id");
				if (var.vt != VT_NULL) {
					strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
					char idChar[10];
					strcpy(idChar, strValue.c_str());
					maxId = atoi(idChar);
				}
				db.HX_pRecordset->MoveNext();
			}
			maxId++;
			char name[20];// = "供应商";
			char nameFix[15];
			itoa(maxId, nameFix, 15);
			string tempStr = string("供应商") + string(nameFix);
			strcpy(name, tempStr.c_str());
			time_t tt = time(NULL);//这句返回的只是一个时间cuo
			tm* t = localtime(&tt);
			char iccode[50];
			int randInt = (rand() % 90) + 10;
			sprintf(iccode,"%d%02d%02d%02d%02d%02d%d",
				t->tm_year + 1900,
				t->tm_mon + 1,
				t->tm_mday,
				t->tm_hour,
				t->tm_min,
				t->tm_sec,randInt);
			sqlString = SQL_SUPPLIERS_INSERT;
			MD5 md5;
			string suppliersSelfid = iccode;
			suppliersSelfid = md5.Encode(suppliersSelfid).substr(8, 16);
			sqlString = sqlString.replace(sqlString.find("<SELFID>"), 8, suppliersSelfid);
			sqlString = sqlString.replace(sqlString.find("<ICCODE>"), 8, iccode);
			sqlString = sqlString.replace(sqlString.find("<NAME>"),6,name);
			sqlString = sqlString.replace(sqlString.find("<CONTACT>"), 9, name);
			sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
			sqlString = sqlString.replace(sqlString.find("<GROUP_ID>"), 10, LOGIN_GROUPID);
			sqlString = sqlString.replace(sqlString.find("<CTYPE>"), 7, getCtypeByCid(LOGIN_COMPANYID));
			strcpy(sql, sqlString.c_str());
			db.addRecord(sql); // insert临时供应商进数据库
			ShowWindow(Page[0], TRUE);
			ShowWindow(Page[1], FALSE);
			ShowWindow(Page[2], FALSE);
			hTab = GetDlgItem(hDlg, IDC_TAB_USER);
			TabCtrl_SetCurSel(hTab,0);
			SendMessage(Page[0], LVM_DELETEALLITEMS, 0, 0);// 清空
			sqlString = SQL_SUPPLIERS_BY_ICCODE;
			sqlString = sqlString.replace(sqlString.find("<ICCODE>"), 8, iccode);
			strcpy(sql, sqlString.c_str());
			db.getRecordRet(sql);
			initUserData(db, 0);
			hTab = GetDlgItem(hDlg, IDC_TAB_USER);
			db.ExitConnect();
			// suppliers add  to  web
			sqlString = SQL_SUPPLIERS_FOR_TOP1_ALL;
			strcpy(sql, sqlString.c_str());
			db.getRecordRet(sql);
			string uploadData = bulidUploadSuppliersData(db);
			replace_all(uploadData, "/", "-");
			string urlString = URL_DATA_UPLOAD_SUPPLIERS_DOMAIN;
			char domain[200], url[200], data[20480];
			strcpy(domain, urlString.c_str());
			urlString = URL_DATA_UPLOAD_SUPPLIERS_URL;
			strcpy(url, urlString.c_str());
			urlString = URL_DATA_UPLOAD_SUPPLIERS_PARAM;
			urlString = urlString.replace(urlString.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
			urlString = urlString.replace(urlString.find("<DATA>"), 6, uploadData);
			urlString = hu.string_To_UTF8(urlString);
			strcpy(data, urlString.c_str());
			hu.httpPost(_T(domain), _T(url), _T(data));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_BUTTON_BT_COM) {
			// 添加磅1信息进入storage表
			hTab = GetDlgItem(hDlg, IDC_TAB_USER);
			nwSel = TabCtrl_GetCurSel(hTab);
			for (int i = 0; i < ListView_GetItemCount(Page[nwSel]); i++)
			{
				if (ListView_GetItemState(Page[nwSel], i, LVIS_SELECTED) == LVIS_SELECTED || ListView_GetCheckState(Page[nwSel], i)) {
					nwCount++;
				}
			}
			if (nwCount > 0) {
				addOneStorage(hDlg, "大磅");
			}
			else {
				MessageBox(hDlg,TEXT("请先选择供应商在点击 大磅-新建毛重"), TEXT("提示"),NULL);
			}
		}
		if (LOWORD(wParam) == IDC_BUTTON_BC_COM) {
			// 大磅 皮重
			changeNw(hDlg,1,false, hdc);
		}
		if (LOWORD(wParam) == IDC_BUTTON_ST_COM) {
			// 添加磅2信息进入storage表
			hTab = GetDlgItem(hDlg, IDC_TAB_USER);
			nwSel = TabCtrl_GetCurSel(hTab);
			for (int i = 0; i < ListView_GetItemCount(Page[nwSel]); i++)
			{
				if (ListView_GetItemState(Page[nwSel], i, LVIS_SELECTED) == LVIS_SELECTED || ListView_GetCheckState(Page[nwSel], i)) {
					nwCount++;
				}
			}
			if (nwCount > 0) {
				addOneStorage(hDlg, "小磅");
			}
			else {
				MessageBox(hDlg, TEXT("请先选择供应商在点击 小磅-新建毛重"), TEXT("提示"), NULL);
			}
		}
		if (LOWORD(wParam) == IDC_BUTTON_SC_COM) {
			// 小磅 皮重
			changeNw(hDlg, 2,false,hdc);
		}
		if (LOWORD(wParam) == IDC_BUTTON_SC_COM_ZERO) {
			// 小磅 0皮重
			changeNw(hDlg, 2,true, hdc);
		}
		//if (LOWORD(wParam) == IDC_BUTTON_OPEN_ADMIN) {
		//	// 数据管理后台
		//	HINSTANCE hRslt = ShellExecute(NULL, TEXT("open"), TEXT("http://dibang.zz91.com"), NULL, NULL, SW_SHOWNORMAL);
		//	assert(hRslt>(HINSTANCE) HINSTANCE_ERROR);
		//}
		if (LOWORD(wParam) == IDC_BUTTON_PORT_CONFIG) {
			// 通信设置
			portHwnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_PORT_CONFIG), hDlg, DialogPortConfig);
			ShowWindow(portHwnd, SW_SHOW);
		}
		if (LOWORD(wParam) == IDC_BUTTON_STORAGE_FAIL) {
			// 作废按钮
			hTab = GetDlgItem(hDlg, IDC_TAB_STORAGE);
			int sel = TabCtrl_GetCurSel(hTab);
			char setMoneyId[10];
			char setMoneyStatus[20];
			do
			{
				if (MessageBox(hDlg, TEXT("确定要将选择的订单作废？"), TEXT("提示"), MB_OKCANCEL) == IDCANCEL) {
					break;
				}
				if (sel != 0) {
					MessageBox(hDlg, TEXT("已经完成的订单不可作废"), TEXT("提示"), 0);
					break;
				}
				sel = sel + 3;// 获取页面句柄下标 
				int i;
				int delCount=0;
				for (i = 0; i < ListView_GetItemCount(Page[sel]); i++)
				{
					if (ListView_GetItemState(Page[sel], i, LVIS_SELECTED) == LVIS_SELECTED || ListView_GetCheckState(Page[sel], i)) {
						ListView_GetItemText(Page[sel], i, 0, setMoneyId, 10);
						ListView_GetItemText(Page[sel], i, 11, setMoneyStatus, 20);
						int id = atoi(setMoneyId);
						string status = setMoneyStatus;
						if (status == "结算完成") {
							continue;
						}
						sqlString = SQL_STORAGE_UPDATE_TO_FAIL;
						sqlString = sqlString.replace(sqlString.find("<ID>"), 4, setMoneyId);
						strcpy(sql, sqlString.c_str());
						db.addRecord(sql);
						delCount++;
					}
				}
				if (delCount == 0) {
					MessageBox(hDlg, TEXT("请先选择要作废的订单，再进行作废操作"), TEXT("提示"), 0);
					break;
				}
			} while (false);

			HWND storeHwnd = GetDlgItem(hDlg, IDC_TAB_STORAGE);

			SendMessage(Page[3], LVM_DELETEALLITEMS, 0, 0);// 清空
			SendMessage(Page[4], LVM_DELETEALLITEMS, 0, 0);// 清空
			sqlString = SQL_V_STORAGELIST_BY_ICCODE;
			sqlString = sqlString.replace(sqlString.find("<ICCODE>"), 8, CHOOSE_ICCODE);
			sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
			strcpy(sql, sqlString.c_str());
			db.getRecordRet(sql);
			char iccode[20];
			strcpy(iccode, CHOOSE_ICCODE.c_str());
			totalMoneyDispaly = initClickStorage(db, iccode);			// 搜索数据填充下表
			db.ExitConnect();
			ShowWindow(Page[3], TRUE);
			ShowWindow(Page[4], FALSE);
			TabCtrl_SetCurSel(storeHwnd, 0);
			setTotalMoney(totalMoneyDispaly, hdc);// 总金额显示
			return (INT_PTR)TRUE;
		}
		
		break;
	case WM_PAINT:
		hdc = BeginPaint(hDlg, &ps); //取得设备环境句柄
		// 结算金额初始化
		LOGFONT   logfont,portfont;       //改变输出字体
		ZeroMemory(&logfont, sizeof(LOGFONT));
		logfont.lfCharSet = GB2312_CHARSET;
		logfont.lfHeight = -28;      //设置字体的大小
		hFont = CreateFontIndirect(&logfont);
		SetBkColor(hdc, RGB(240, 240, 240));
		////SetBkMode(hdc, TRANSPARENT); 透明样式，没用。适合固定展示，不适合经常变动的数据
		SelectObject(hdc, hFont);
		char zsBuffer[40];

		TextOut(hdc, 550, 600, zsBuffer, sprintf(zsBuffer, TEXT("结算金额:")));
		SetTextColor(hdc, RGB(255, 140, 102)); // 颜色
		TextOut(hdc, 685, 600, zsBuffer, sprintf(zsBuffer, TEXT("0元"))); 
		ZeroMemory(&portfont, sizeof(LOGFONT));
		portfont.lfCharSet = GB2312_CHARSET;
		portfont.lfHeight = -14;      //设置字体的大小
		hFont = CreateFontIndirect(&portfont);
		SetBkColor(hdc, RGB(240, 240, 240));
		////SetBkMode(hdc, TRANSPARENT); 透明样式，没用。适合固定展示，不适合经常变动的数据
		SelectObject(hdc, hFont);
		if (mySerialPort.isClose) {
			SetTextColor(hdc, RGB(255, 0, 0)); // 红色
			TextOut(hdc, x1, y1, zsBuffer, sprintf(zsBuffer, TEXT("未连接")));
		}
		else {
			SetTextColor(hdc, RGB(17, 170, 51)); // 绿色
			TextOut(hdc, x1, y1, zsBuffer, sprintf(zsBuffer, TEXT("已连接")));
		}
		if (mySerialPort2.isClose) {
			SetTextColor(hdc, RGB(255, 0, 0)); // 红色
			TextOut(hdc, x1, y2, zsBuffer, sprintf(zsBuffer, TEXT("未连接")));
		}
		else {
			SetTextColor(hdc, RGB(17, 170, 51)); // 绿色
			TextOut(hdc, x1, y2, zsBuffer, sprintf(zsBuffer, TEXT("已连接")));
		}


		//int y = 297, x1 = 305, x2 = 600;			// 点击时候，供应商和供应商编号的横纵坐标
		SetTextColor(hdc, RGB(0, 136, 238)); // 颜色
		TextOut(hdc, 305, 297, zsBuffer, sprintf(zsBuffer, "%s","- - - - - -"));
		TextOut(hdc, 600, 297, zsBuffer, sprintf(zsBuffer, "%s","- - - - - -"));

		ReleaseDC(hDlg, hdc);
		DeleteObject(hFont);
		EndPaint(hDlg, &ps);
		break;
	case WM_CTLCOLORBTN://设置按钮的颜色
		if ((HWND)lParam == GetDlgItem(hDlg, IDC_BUTTON_PAY))
		{
			GetWindowText(hbn, text, 23);
			GetClientRect(hbn, &rc);
			SetTextColor(hdc, RGB(255, 255, 255));//设置按钮上文本的颜色
			SetBkMode(hdc, TRANSPARENT);
			rc.left = 220;
			rc.top = 608;
			rc.right = 312;
			rc.bottom = 655;
			MoveToEx(hdc,150,150,NULL);
			DrawText(hdc, text, _tcslen(text), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE|DT_BOTTOM|WS_EX_TRANSPARENT|WS_CHILD| BS_PUSHBUTTON);
			return (INT_PTR)CreateSolidBrush(RGB(255, 140, 102));//返回画刷设置按钮的背景色
		}
		if ((HWND)lParam == GetDlgItem(hDlg, IDC_BUTTON_SET_MONEY))
		{
			GetWindowText(hbn, text, 23);
			GetClientRect(hbn, &rc);
			SetTextColor(hdc, RGB(255, 255, 255));//设置按钮上文本的颜色
			SetBkMode(hdc, TRANSPARENT);
			rc.left = 965;
			rc.top = 608;
			rc.right = 1045;
			rc.bottom = 655;
			MoveToEx(hdc, 150, 150, NULL);
			DrawText(hdc, text, _tcslen(text), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_BOTTOM | WS_EX_TRANSPARENT | WS_CHILD | BS_PUSHBUTTON);
			return (INT_PTR)CreateSolidBrush(RGB(0, 180, 240));//返回画刷设置按钮的背景色
		}
		break;
	case WM_DRAWITEM:
		//获得绘制结构体，包含绘制的按钮DC和当前按钮状态等  
		if (LOWORD(wParam) == IDC_BUTTON_PAY) {
			pdis = (LPDRAWITEMSTRUCT)lParam;
			if (pdis->CtlType == ODT_BUTTON)//只绘制button类型  
			{
				hdc = pdis->hDC;
				RECT itemRect = pdis->rcItem;
				SaveDC(hdc);//保存DC,绘制完必须恢复默认  
							//绘制默认状态  
				hMemDc = CreateCompatibleDC(hdc);
				//SelectObject(hMemDc, pdis->CtlID == IDOK ? hBitmapOK_U : hBitmapCANCEL_U);
				//BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hMemDc, 0, 0, SRCCOPY);
				DeleteDC(hMemDc);
				//绘制获取焦点时状态  
				if (pdis->itemState & ODS_FOCUS)
				{
					HBRUSH br = CreateSolidBrush(RGB(255, 255, 255));
					FrameRect(hdc, &itemRect, br);
					InflateRect(&itemRect, -1, -1);
					DeleteObject(br);
				}
				//绘制下压状态  
				if (pdis->itemState & ODS_SELECTED)
				{
					HBRUSH brBtnShadow = CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
					FrameRect(hdc, &itemRect, brBtnShadow);
					DeleteObject(brBtnShadow);
				}
				else {
					UINT uState = DFCS_BUTTONPUSH |
						((pdis->itemState & ODS_SELECTED) ? DFCS_PUSHED : 0);
					HBRUSH br = CreateSolidBrush(RGB(194, 194, 194));
					FrameRect(hdc, &itemRect, br);
					InflateRect(&itemRect, -1, -1);
					DeleteObject(br);
				}
				RestoreDC(hdc, -1);
			}
		}
		if (LOWORD(wParam) == IDC_BUTTON_SET_MONEY) {
			pdis = (LPDRAWITEMSTRUCT)lParam;
			if (pdis->CtlType == ODT_BUTTON)//只绘制button类型  
			{
				hdc = pdis->hDC;
				RECT itemRect = pdis->rcItem;
				SaveDC(hdc);//保存DC,绘制完必须恢复默认  
							//绘制默认状态  
				hMemDc = CreateCompatibleDC(hdc);
				//SelectObject(hMemDc, pdis->CtlID == IDOK ? hBitmapOK_U : hBitmapCANCEL_U);
				//BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hMemDc, 0, 0, SRCCOPY);
				DeleteDC(hMemDc);
				//绘制获取焦点时状态  
				if (pdis->itemState & ODS_FOCUS)
				{
					HBRUSH br = CreateSolidBrush(RGB(255, 255, 255));
					FrameRect(hdc, &itemRect, br);
					InflateRect(&itemRect, -1, -1);
					DeleteObject(br);
				}
				//绘制下压状态  
				if (pdis->itemState & ODS_SELECTED)
				{
					HBRUSH brBtnShadow = CreateSolidBrush(GetSysColor(COLOR_BTNSHADOW));
					FrameRect(hdc, &itemRect, brBtnShadow);
					DeleteObject(brBtnShadow);
				}
				else {
					UINT uState = DFCS_BUTTONPUSH |
						((pdis->itemState & ODS_SELECTED) ? DFCS_PUSHED : 0);
					HBRUSH br = CreateSolidBrush(RGB(194, 194, 194));
					FrameRect(hdc, &itemRect, br);
					InflateRect(&itemRect, -1, -1);
					DeleteObject(br);
				}
				RestoreDC(hdc, -1);
			}
		}
		return (TRUE);
	case WM_SIZE:
		nWidth = LOWORD(lParam);
		nHeight = HIWORD(lParam);
		RECT rcDlg, rcWDlg;
		GetClientRect(hDlg, &rcDlg);	// 对话框自身参数 获取Tab窗口的大小（而不是在屏幕坐标系中）
		GetWindowRect(hDlg, &rcWDlg);	// window位置参数

		if (nWidth>0) {
			HWND hwndPay = GetDlgItem(hDlg, IDC_BUTTON_PAY);
			RECT rcPayTemp,rcWPayTemp;
			GetClientRect(hwndPay, &rcPayTemp);
			GetWindowRect(hwndPay, &rcWPayTemp);
			MoveWindow(hwndPay, 179, rcDlg.bottom-10-(rcPayTemp.bottom - rcPayTemp.top), rcPayTemp.right - rcPayTemp.left, rcPayTemp.bottom - rcPayTemp.top, true);
			HWND hwndSetMoney = GetDlgItem(hDlg, IDC_BUTTON_SET_MONEY);
			GetClientRect(hwndSetMoney, &rcPayTemp);
			MoveWindow(hwndSetMoney, rcDlg.right- (130+rcPayTemp.right - rcPayTemp.left ), rcDlg.bottom - 20 - (rcPayTemp.bottom - rcPayTemp.top), rcPayTemp.right - rcPayTemp.left, rcPayTemp.bottom - rcPayTemp.top, false);
			HWND hwndFail = GetDlgItem(hDlg, IDC_BUTTON_STORAGE_FAIL);
			GetClientRect(hwndFail, &rcPayTemp);
			MoveWindow(hwndFail, rcDlg.right - (10 + rcPayTemp.right - rcPayTemp.left), rcDlg.bottom - 20 - (rcPayTemp.bottom - rcPayTemp.top), rcPayTemp.right - rcPayTemp.left, rcPayTemp.bottom - rcPayTemp.top, false);
		}

		return (INT_PTR)FALSE;
	case WM_NOTIFY:
		// 捕获鼠标左键点击事件
		if (((LPNMHDR)lParam)->code == NM_CLICK) {
			iItem = ((LPNMITEMACTIVATE)lParam)->iItem;
			iSubItem = ((LPNMITEMACTIVATE)lParam)->iSubItem;
			hTab = GetDlgItem(hDlg, IDC_TAB_USER);
			int sel = TabCtrl_GetCurSel(hTab);
			HWND foceHwnd = GetFocus();						// 获取焦点句柄
			HWND foceParentHwnd = GetParent(foceHwnd);		// 获取焦点句柄的父句柄
			
			if (sel >= 0&& sel <= 2 && iItem>=0&& iSubItem>=0&& hTab == foceParentHwnd && iItem <= 100 && iSubItem <= 100) {
				HWND storeHwnd = GetDlgItem(hDlg, IDC_TAB_STORAGE);
				SendMessage(Page[3], LVM_DELETEALLITEMS, 0, 0);// 清空
				SendMessage(Page[4], LVM_DELETEALLITEMS, 0, 0);// 清空
				char iccode[50];
				ListView_GetItemText(Page[sel], iItem, 1, iccode, 50);
				CHOOSE_ICCODE = iccode;		//选择的iccode
				char suppliersName[50];
				ListView_GetItemText(Page[sel], iItem, 2, suppliersName, 50);
				DBUtils db;
				sqlString = SQL_V_STORAGELIST_BY_ICCODE;
				sqlString = sqlString.replace(sqlString.find("<ICCODE>"), 8, iccode);
				sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
				strcpy(sql, sqlString.c_str());
				db.getRecordRet(sql);
				totalMoneyDispaly = initClickStorage(db,iccode);			// 搜索数据填充下表
				setTotalMoney(totalMoneyDispaly, hdc);
				db.ExitConnect();
				if (sel == 1) {
					ShowWindow(Page[3], TRUE);
					ShowWindow(Page[4], FALSE);
					TabCtrl_SetCurSel(storeHwnd,0);
				}
				if (sel == 2) {
					ShowWindow(Page[4], TRUE);
					ShowWindow(Page[3], FALSE);
					TabCtrl_SetCurSel(storeHwnd, 1);
				}
				// 导航栏出现 供货人编号，供货人姓名
				UpdateWindow(hDlg);
				LOGFONT   logfont;       //改变输出字体
				ZeroMemory(&logfont, sizeof(LOGFONT));
				logfont.lfCharSet = GB2312_CHARSET;
				logfont.lfHeight = -16;      //设置字体的大小
				hFont = CreateFontIndirect(&logfont);
				SetBkColor(hdc, RGB(240, 240, 240));
				////SetBkMode(hdc, TRANSPARENT); 透明样式，没用。适合固定展示，不适合经常变动的数据
				SelectObject(hdc, hFont);
				char zsBuffer[40];
				SetTextColor(hdc, RGB(0, 180, 240)); // 颜色
				char blank[40]="                 ";
				int y=297,x1=305,x2=600;			// 点击时候，供应商和供应商编号的横纵坐标
				TextOut(hdc, x1, y, zsBuffer, sprintf(zsBuffer, blank));
				TextOut(hdc, x1, y, zsBuffer, sprintf(zsBuffer, iccode));
				TextOut(hdc, x2, y, zsBuffer, sprintf(zsBuffer, blank));
				TextOut(hdc, x2, y, zsBuffer, sprintf(zsBuffer, suppliersName));
				ReleaseDC(hDlg, hdc);
				DeleteObject(hFont);
			}
			hTab = GetDlgItem(hDlg, IDC_TAB_STORAGE);
			sel = TabCtrl_GetCurSel(hTab);
			if (sel == 0 && iItem >= 0 && iSubItem >= 0 && hTab == foceParentHwnd && iItem <= 100 && iSubItem <= 100) {
				char tare[10];
				ListView_GetItemText(Page[sel], iItem, 6, tare, 10);
				if (tare[0] == '\0') {
					
				}
				else {

				}
			}
		}
		if ((INT)wParam == IDC_TAB_USER)
		{
			hTab = GetDlgItem(hDlg, IDC_TAB_USER);
			if (((LPNMHDR)lParam)->code == TCN_SELCHANGE) //当TAB标签转换的时候发送TCN_SELCHANGE消息  
			{
				int sel = TabCtrl_GetCurSel(hTab);
				switch (sel)   //根据索引值查找相应的标签值，干相应的事情  
				{
					case 0: //TAB1标签时，我们要显示 tab1标签页面  
					{
						ShowWindow(Page[0], TRUE); //显示窗口用ShowWindow函数  
						ShowWindow(Page[1], FALSE);
						ShowWindow(Page[2], FALSE);
						break;
					}
					case 1://TAB2标签时，我们要显示 tab2标签页面  
					{
						ShowWindow(Page[0], FALSE);
						ShowWindow(Page[1], TRUE);
						ShowWindow(Page[2], FALSE);
						break;
					}
					case 2://TAB2标签时，我们要显示 tab3标签页面  
					{
						ShowWindow(Page[0], FALSE);
						ShowWindow(Page[1], FALSE);
						ShowWindow(Page[2], TRUE);
						break;
					}
				}
			}
		}
		if ((INT)wParam == IDC_TAB_STORAGE) //这里也可以用一个NMHDR *nm = (NMHDR *)lParam这个指针来获取 句柄和事件  
		{
			hTab = GetDlgItem(hDlg, IDC_TAB_STORAGE);
			if (((LPNMHDR)lParam)->code == TCN_SELCHANGE) //当TAB标签转换的时候发送TCN_SELCHANGE消息  
			{
				int sel = TabCtrl_GetCurSel(hTab);
				switch (sel)   //根据索引值查找相应的标签值，干相应的事情  
				{
					case 0: //TAB1标签时，我们要显示 tab1标签页面  
					{
						ShowWindow(Page[3], TRUE); //显示窗口用ShowWindow函数  
						ShowWindow(Page[4], FALSE);
						break;
					}
					case 1://TAB2标签时，我们要显示 tab2标签页面  
					{
						ShowWindow(Page[3], FALSE);
						ShowWindow(Page[4], TRUE);
						break;
					}
				}
			}
		}

		return (INT_PTR)FALSE;
	case WM_CLOSE:
		/*if (MessageBox(hDlg, TEXT("确定要退出系统？"), TEXT("提示"), MB_OKCANCEL) == IDOK)
		{*/
			DestroyWindow(hWndMain);
		/*}
		else 
		{
			HWND hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_STORAGE), NULL, DialogStorage);
			ShowWindow(hDialog, SW_SHOW);
			break;
		}*/
		return 0;
	}
	
	return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK DialogLogin(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HANDLE hThread;

	ifstream infile;
	string sqlString;
	char sql[200],clientid[50];
	DBUtils db;
	int count;

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:

		//判定是否有client，如果有，则获取client下的用户登录
		infile.open("client.config");
		infile >> clientid;
		sqlString = SQL_USERS_SELECT_BY_CLIENTID;
		sqlString = sqlString.replace(sqlString.find("<CLIENTID>"),10,clientid);
		strcpy(sql, sqlString.c_str());
		db.getRecordRet(sql);
		while (!db.HX_pRecordset->adoEOF) {
			_variant_t var;
			string strValue;
			var = db.HX_pRecordset->GetCollect("id");
			if (var.vt != VT_NULL) {
				strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
			}
			count = atoi(strValue.c_str());
			var = db.HX_pRecordset->GetCollect("selfid");
			if (var.vt != VT_NULL) {
				strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
			}
			strcpy(LOGIN_SELFID, strValue.c_str());
			var = db.HX_pRecordset->GetCollect("username");
			if (var.vt != VT_NULL) {
				strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
			}
			strcpy(LOGIN_USERNAME, strValue.c_str());
			var = db.HX_pRecordset->GetCollect("group_id");
			if (var.vt != VT_NULL) {
				strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
			}
			strcpy(LOGIN_GROUPID, strValue.c_str());
			var = db.HX_pRecordset->GetCollect("company_id");
			if (var.vt != VT_NULL) {
				strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
			}
			strcpy(LOGIN_COMPANYID, strValue.c_str());
			db.HX_pRecordset->MoveNext();
		}
		db.ExitConnect();
		if (count>0)
		{
			// 更新外网数据
			systemDataInit();
			/* 启动上传入库单信息的线程或定时器 */
			hThread = CreateThread(NULL, 0, SystemUploadThread, NULL, 0, NULL);
			CloseHandle(hThread);
			hThread = CreateThread(NULL, 0, UpdateStorageForPriceThread, NULL, 0, NULL);
			CloseHandle(hThread);
			HWND hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_STORAGE), NULL, DialogStorage);
			ShowWindow(hDialog, SW_SHOW);
			ShowWindow(hDlg,SW_HIDE);
			DestroyWindow(hDlg);
			//EndDialog(hDlg, LOWORD(wParam));
		}

		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			DestroyWindow(hWndMain);
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_DO_LOGIN )   // 登录逻辑
		{
			char clientid[50];		
			memset(clientid, 0, 50);
			HWND accounthDlg = GetDlgItem(hDlg, IDC_EDIT_ACCOUNT);
			HWND pwdDlg = GetDlgItem(hDlg, IDC_EDIT_PASSWORD);
			char username[50];					// 获取输入用户名
			char password[50];					// 获取密码
			char md5Pwd[50];					// md5 加密过后的密码 16位		
			string md5PwdString;
			string tempMd5;
			GetWindowText(accounthDlg, username, 50);
			GetWindowText(pwdDlg, password, 50);
			MD5 md5;
			md5PwdString = password;
			tempMd5 = md5.Encode(md5PwdString).substr(8,16);
			strcpy(md5Pwd, tempMd5.c_str());
			// 完整一次sql-----start
			DBUtils dbUtils;
			string sqlString = SQL_USERS_BY_UAP;
			sqlString = sqlString.replace(sqlString.find("<USERNAME>"),10,username);
			sqlString = sqlString.replace(sqlString.find("<PWD>"), 5, md5PwdString);
			char sql[300];
			memset(sql, 0, 300);
			strcpy(sql,sqlString.c_str());
			dbUtils.getRecordRet(sql);
			int count;
			while (!dbUtils.HX_pRecordset->adoEOF) {
				_variant_t var;
				string strValue;
				var  = dbUtils.HX_pRecordset->GetCollect("hasExist");
				if (var.vt != VT_NULL) {
					strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
					count = atoi(strValue.c_str()); 
				}
				dbUtils.HX_pRecordset->MoveNext();
			}
			dbUtils.ExitConnect();// 关闭连接
			// 完整一次sql----end
			// 本地不存在数据 走http外网
			// 添加inc文件 当前目录下client.config
			if (count <= 0) {
				// 当前时间获取
				time_t now = time(NULL);
				char* nowChar = asctime(gmtime(&now));
				string temp = nowChar;
				string clientidstring = temp;
				temp = username;
				clientidstring = clientidstring + temp;
				clientidstring = md5.Encode(clientidstring).substr(8,16);
				strcpy(clientid, clientidstring.c_str());
				// 打开文件
				ofstream outfile;
				outfile.open("client.config");
				outfile << clientid << endl;		//输入文本
				outfile.close();					// 文件操作结束
				char httpUrl[100];
				string url = URL_LOGIN;
				url = url.replace(url.find("<USERNAME>"),10,username);
				url = url.replace(url.find("<PASSWORD>"), 10, password);
				url = url.replace(url.find("<CLIENTID>"), 10, clientid);
				strcpy(httpUrl, url.c_str());
				HttpUtils hu;
				string httpResult = hu.httpGet(_T(httpUrl));
				Json::Reader reader;                                    //解析json用Json::Reader
				Json::Value value;
				//可以代表任意类型
				if (!reader.parse(httpResult, value))
				{
					// 检验失败，弹框返回返回
					MessageBox(NULL, TEXT("网络故障请重试！"), TEXT("提示"), 0);
				}
				else {
					char selfidChar[50];
					if (value["err"].asString()=="true") {
						char errMsg[50];
						string errMsgString = value["errtext"].asString();
						strcpy(errMsg,errMsgString.c_str());
						MessageBox(NULL, TEXT(errMsg), TEXT("提示"), 0);
					}
					else {
						// 检验成功，导入数据进本地数据库
						string selfid = value["result"]["selfid"].asString();
						strcpy(selfidChar, selfid.c_str());
						int group_id = value["result"]["group_id"].asInt();
						int company_id = value["result"]["company_id"].asInt();
						string sqlInsert = SQL_USERS_INSERT;
						sqlInsert.replace(sqlInsert.find("<SELFID>"),8, selfidChar);
						char groupArray[11];
						sqlInsert.replace(sqlInsert.find("<GROUP_ID>"), 10, itoa(group_id,groupArray,10));
						char companyArray[11];
						sqlInsert.replace(sqlInsert.find("<COMPANY_ID>"), 12, itoa(company_id, companyArray, 10));
						sqlInsert.replace(sqlInsert.find("<CLIENTID>"), 10, clientid);
						sqlInsert.replace(sqlInsert.find("<USERNAME>"),10,username);
						sqlInsert.replace(sqlInsert.find("<PWD>"),5, md5Pwd);
						char sql[1000];
						strcpy(sql,sqlInsert.c_str());
						dbUtils.addRecord(sql);
						// 记录登陆信息 初始化本地信息
						strcpy(LOGIN_SELFID, selfidChar);
						strcpy(LOGIN_USERNAME, username);
						strcpy(LOGIN_GROUPID, groupArray);
						strcpy(LOGIN_COMPANYID, companyArray);
						systemDataInit();
						/* 启动上传入库单信息的线程或定时器 */
						hThread = CreateThread(NULL, 0, SystemUploadThread, NULL, 0, NULL);
						CloseHandle(hThread);
						hThread = CreateThread(NULL, 0, UpdateStorageForPriceThread, NULL, 0, NULL);
						CloseHandle(hThread);
						EndDialog(hDlg, LOWORD(wParam));
						HWND hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_STORAGE), NULL, DialogStorage);
						ShowWindow(hDialog, SW_SHOW);
					}
				}
			}
			return (INT_PTR)TRUE;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWndMain);
		return 0;
	}
	return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK DialogSetMoney(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HWND parentHwnd;
	HDC hdc = GetDC(hDlg);
	char setMoneyName[20];
	HFONT hFont;

	char szBuffer[40];
	string sqlString;
	char id[10];
	DBUtils db;
	HWND hcombo;
	char sql[200];
	int i = 0;

	HWND hDialog;

	HWND storeHwnd;
	char iccode[50];

	RECT rcDlg, rc;
	HWND phwnd = GetParent(hDlg);

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		GetClientRect(hDlg, &rcDlg);//获取Tab窗口的大小（而不是在屏幕坐标系中）  
		GetWindowRect(phwnd, &rc);
		MoveWindow(hDlg, rc.left + 290, rc.top + 100, rcDlg.right - rcDlg.left - 5, rcDlg.bottom - rcDlg.top + 20, false);
		// 初始化定价弹框的下拉框。
		hcombo = GetDlgItem(hDlg, IDC_COMBO_PRODUCT_CODE);
		sqlString = SQL_CATEGORY_PRODUCTS_SELECT_BY_COMPANY_ID;
		sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"),12,LOGIN_COMPANYID);
		strcpy(sql,sqlString.c_str());
		db.getRecordRet(sql);
		while (!db.HX_pRecordset->adoEOF)
		{
			_variant_t var = db.HX_pRecordset->GetCollect("name");
			string strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
			char comboChar[20];
			strcpy(comboChar,strValue.c_str());
			SendMessage(hcombo, CB_INSERTSTRING, i, (LPARAM)comboChar);
			db.HX_pRecordset->MoveNext();
			i++;
		}
		db.ExitConnect();
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL|| LOWORD(wParam) == IDC_BUTTON_SET_CANCLE)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_BUTTON_PAY) {

		}
		if (LOWORD(wParam) == IDC_BUTTON_CHOOSE_CATEGORY)
		{
			char checkBox[20];
			hcombo = GetDlgItem(hDlg, IDC_COMBO_PRODUCT_CODE);
			GetWindowText(hcombo, checkBox, 20);
			sqlString = SQL_CATEGORY_PRODUCTS_SELECT_BY_NAME;
			sqlString = sqlString.replace(sqlString.find("<NAME>"),6,checkBox);
			strcpy(sql, sqlString.c_str());
			db.getRecordRet(sql);
			string categorySelfId;
			while (!db.HX_pRecordset->adoEOF)
			{
				_variant_t var = db.HX_pRecordset->GetCollect("selfid");
				categorySelfId = _com_util::ConvertBSTRToString((_bstr_t)var);
				CHOOSE_CATEGORY_SELFID = categorySelfId;
				db.HX_pRecordset->MoveNext();
			}
			//DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_CHOOSE_PRODUCT), hDlg, DialogChooseProduct);
			hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_CHOOSE_PRODUCT), hDlg, DialogChooseProduct);
			DWORD styles = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES;
			ListView_SetExtendedListViewStyleEx(hDialog, styles, styles);
			ShowWindow(hDialog, SW_SHOW);
			db.ExitConnect();
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_BUTTON_SET_SURE) {
			HttpUtils hu;
			char productName[20];
			GetDlgItemText(hDlg, IDC_EDIT_PRODUCT_NAME, productName, 20);
			if (productName[0] == '\0') {
				MessageBox(hDlg, TEXT("产品名必须填写"), TEXT("错误"), 0);
				break;
			}
			char price[20];
			GetDlgItemText(hDlg, IDC_EDIT_SET_MONEY, price, 20);
			if (price[0] == '\0') {
				MessageBox(hDlg, TEXT("单价必须填写"), TEXT("错误"), 0);
				break;
			}
			//set price=<PRICE>,price_users_selfid = <PRICE_USERS_SELFID> ,gmt_modified = now(),price_time = now(),status = 2 where id = <ID>"
			sqlString = SQL_STORAGE_UPDATE_TO_PRICE_OVER;
			sqlString = sqlString.replace(sqlString.find("<PRICE>"), 7, price);
			sqlString = sqlString.replace(sqlString.find("<PRICE_USERS_SELFID>"), 20, LOGIN_SELFID);
			sqlString = sqlString.replace(sqlString.find("<PRODUCTS_SELFID>"), 17, CHOOSE_SAVE_PRODUCTS_SELF_ID);
			sqlString = sqlString.replace(sqlString.find("<PRICE_TIME>"), 12, " now() ");

			//itoa(STORAGE_SET_MONEY_ID, id, 20);
			sprintf(id, "%d", STORAGE_SET_MONEY_ID);
			sqlString = sqlString.replace(sqlString.find("<ID>"), 4, id);

			string selectString = SQL_STORAGE_BY_ID;
			char selectChar[200];
			selectString = selectString.replace(selectString.find("<ID>"), 4, id);
			strcpy(selectChar, selectString.c_str());
			db.getRecordRet(selectChar);
			char gw[20], tare[20], nw[20], totalPrice[20];
			while (!db.HX_pRecordset->adoEOF) {
				_variant_t v_gw		= db.HX_pRecordset->GetCollect("gw");
				getSqlSetColumn(v_gw,gw);
				_variant_t v_tare	= db.HX_pRecordset->GetCollect("tare");
				getSqlSetColumn(v_tare, tare);
				_variant_t v_nw		= db.HX_pRecordset->GetCollect("nw");
				getSqlSetColumn(v_nw, nw);
				db.HX_pRecordset->MoveNext();
			}
			getTotalPrice(gw, tare, nw, price, totalPrice);
			sqlString = sqlString.replace(sqlString.find("<TOTAL>"), 7, totalPrice);

			strcpy(sql, sqlString.c_str());
			db.addRecord(sql);// update定价信息

			//上传数据至服务器
			char domain[200], url[200], param[20480];
			string postData = URL_DATA_UPLOAD_STORAGE_DOMAIN;
			strcpy(domain, postData.c_str());
			postData = URL_DATA_UPLOAD_STORAGE_URL;
			strcpy(url, postData.c_str());
			postData = URL_DATA_UPLOAD_STORAGE_PARAM;
			sqlString = SQL_STORAGE_BY_ID;											// selfid 获取新建的入库单
			sqlString = sqlString.replace(sqlString.find("<ID>"), 4, id);
			strcpy(sql, sqlString.c_str());
			db.getRecordRet(sql);
			string uploadData = bulidUploadStorageData(db);
			postData = postData.replace(postData.find("<DATA>"), 6, uploadData);
			postData = postData.replace(postData.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
			strcpy(param, postData.c_str());
			hu.httpPost(_T(domain), _T(url), _T(param));

			// 更新列表数据
			strcpy(iccode, CHOOSE_ICCODE.c_str());
			parentHwnd = GetParent(hDlg);
			HWND storeHwnd = GetDlgItem(parentHwnd, IDC_TAB_STORAGE);
			SendMessage(Page[3], LVM_DELETEALLITEMS, 0, 0);// 清空
			sqlString = SQL_V_STORAGELIST_BY_ICCODE;
			sqlString = sqlString.replace(sqlString.find("<ICCODE>"), 8, iccode);
			sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
			strcpy(sql, sqlString.c_str());
			db.getRecordRet(sql);
			initClickStorage(db, iccode);			// 搜索数据填充下表
			db.ExitConnect();
			ShowWindow(Page[3], TRUE);
			ShowWindow(Page[4], FALSE);
			TabCtrl_SetCurSel(storeHwnd, 0);

			CHOOSE_CATEGORY_SELFID = "";
			STORAGE_SET_MONEY_ID = 0;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hDlg, &ps);
		// 供应商姓名
		strcpy(setMoneyName, STORAGE_SET_MONEY_NAME.c_str());
		LOGFONT logfont;       //改变输出字体
		ZeroMemory(&logfont, sizeof(LOGFONT));
		logfont.lfCharSet = GB2312_CHARSET;
		logfont.lfHeight = -14;      //设置字体的大小
		hFont = CreateFontIndirect(&logfont);
		SetTextColor(hdc, RGB(0, 61, 245)); // 颜色
		SetBkMode(hdc, TRANSPARENT);
		SelectObject(hdc, hFont);
		TextOut(hdc, 182, 48, setMoneyName, strlen(setMoneyName));
		DBUtils db;
		sqlString = SQL_STORAGE_BY_ID;
		itoa(STORAGE_SET_MONEY_ID,id,10);
		sqlString = sqlString.replace(sqlString.find("<ID>"),4,id);
		char sql[200];
		strcpy(sql, sqlString.c_str());
		db.getRecordRet(sql);
		while (!db.HX_pRecordset->adoEOF) {
			_variant_t var = db.HX_pRecordset->GetCollect("gw");
			if (var.vt != VT_NULL) {
				string gw = _com_util::ConvertBSTRToString((_bstr_t)var);
				strcpy(szBuffer, gw.c_str());
				if (szBuffer[0] == '.') {
					string addZ = szBuffer;
					addZ = "0" + addZ;
					memset(szBuffer, 0, 20);
					strcpy(szBuffer, addZ.c_str());
				}
			}
			SetTextColor(hdc, RGB(230, 57, 0)); // 颜色// 毛重 重量
			TextOut(hdc, 182, 176, szBuffer, strlen(szBuffer));
			var = db.HX_pRecordset->GetCollect("price");
			if (var.vt != VT_NULL) {
				string price = _com_util::ConvertBSTRToString((_bstr_t)var);
				strcpy(szBuffer, price.c_str());
				if (szBuffer[0] == '.') {
					string addZ = szBuffer;
					addZ = "0" + addZ;
					memset(szBuffer, 0, 20);
					strcpy(szBuffer, addZ.c_str());
				}
			}
			else {
				memset(szBuffer, 0, strlen(szBuffer));
			}
			SetTextColor(hdc, RGB(0, 61, 245)); // 颜色// 最新定价 获取定价
			TextOut(hdc, 182, 213, szBuffer, strlen(szBuffer));
			db.HX_pRecordset->MoveNext();
		}
		db.ExitConnect();
		ReleaseDC(hDlg, hdc);
		DeleteObject(hFont);
		EndPaint(hDlg, &ps);
	}
	
	return (INT_PTR)FALSE;
}

void getSerialPortNumber(char* result,char* out) {
	int i = 0;
	for (; i < 100; i++) {
		if (result[i] == '\r'&&i>4) {
			break;
		}
	}
	
	memset(out, '\0', 100);
	if (i >= 5) {
		for (int j = 0, k = 0; j < i; j++) {
			if (result[j] == '\n' || result[j] == 'w' || result[j] == 'n') {
				continue;
			}
			out[k] = result[j];
			k++;
		}
	}
	int xi = 0;
	for (; xi < 12; xi++) {
		if (out[xi] == '.') {
			break;
		}
	}
	if (xi < 5) {
		char zero[5] = "";
		memset(out, '\0', 5);
		int xZero = 0;
		for (; xi <= 5 - xi; xi++) {
			zero[xZero] = '0';
			xZero++;
		}
		sprintf(out, "%s%s", zero, out);
	}
}


DWORD WINAPI TimerSerialPortThread(LPVOID pParam) {
	portThreadOpen = true;
	while (true)
	{
		Sleep(1);
		HDC hdc = GetDC(threadDisHwnd);
		char result[100] = "";
		string resultStr;
		mySerialPort.ReadOne(result);
		resultStr = result;
		LOGFONT   logfont;       //改变输出字体
		ZeroMemory(&logfont, sizeof(LOGFONT));
		logfont.lfCharSet = GB2312_CHARSET;
		logfont.lfHeight = -30;      //设置字体的大小
		HFONT   hFont = CreateFontIndirect(&logfont);
		SetTextColor(hdc, RGB(230, 57, 0)); // 颜色
		SetBkColor(hdc, RGB(240, 240, 240));
		SelectObject(hdc, hFont);
		float b = atof(resultStr.c_str());
		char out[20];
		sprintf(out, "%.1f", b);
		char blank[10] = "         ";
		if (b > 0) {
			TextOut(hdc, 28, 60, blank, 9);
			TextOut(hdc, 28, 60, out, strlen(out));
		}
		else {
			TextOut(hdc, 28, 60, "0.0", 3);
		}
		mySerialPort2.ReadOne(result);
		resultStr = result;
		SetTextColor(hdc, RGB(0, 102, 0)); // 颜色
		float s = atof(result);
		sprintf(out, "%.1f", s);
		if (s > 0) {
			TextOut(hdc, 28, 370, blank, 9);
			TextOut(hdc, 28, 370, out, strlen(out));
		}
		else {
			TextOut(hdc, 28, 370, "0.0", 3);
		}
		ReleaseDC(threadDisHwnd, hdc);
		DeleteObject(hFont);
	}
	return 0;
}

DWORD WINAPI SystemUploadThread(LPVOID pParam) {
	// sql
	string sqlString;
	char sql[200];
	DBUtils db;
	// http
	string urlString;
	char domain[200];
	char url[200];
	char data[20480];
	HttpUtils hu;
	string httpResult;
	int count = 0;
	while (true)
	{
		// 入库模块
		// 获取时间
		urlString = URL_DATA_GET_STORAGE_LASTTIME;
		urlString = urlString.replace(urlString.find("<COMPANY_ID>"),12,LOGIN_COMPANYID);
		strcpy(url,urlString.c_str());
		httpResult = hu.httpGet(_T(url));
		// 遍历数据库
		sqlString = SQL_STORAGE_FOR_UPLOAD;
		if (httpResult == "0"|| httpResult == "") {
			sqlString = sqlString.replace(sqlString.find("<GMT_MODIFIED>"), 14, "2017-1-1");
		}
		else {
			sqlString = sqlString.replace(sqlString.find("<GMT_MODIFIED>"), 14, httpResult);
		}
		sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
		strcpy(sql,sqlString.c_str());
		db.getRecordRet(sql);
		string uploadData= bulidUploadStorageData(db);
		
		// 有数据，则回传
		if (uploadData != "") {
			replace_all(uploadData,"/","-");
			urlString = URL_DATA_UPLOAD_STORAGE_DOMAIN;
			memset(domain, 0, 200);
			strcpy(domain, urlString.c_str());
			urlString = URL_DATA_UPLOAD_STORAGE_URL;
			memset(url, 0, 200);
			strcpy(url, urlString.c_str());

			urlString = URL_DATA_UPLOAD_STORAGE_PARAM;
			urlString = urlString.replace(urlString.find("<COMPANY_ID>"),12, LOGIN_COMPANYID);
			urlString = urlString.replace(urlString.find("<DATA>"),6, uploadData);
			memset(data, 0, 2048);
			strcpy(data, urlString.c_str());
			httpResult = hu.httpPost(_T(domain),_T(url),_T(data));
		}

		// 供应商模块
		// 获取时间
		urlString = URL_DATA_GET_SUPPLIERS_LASTTIME;
		urlString = urlString.replace(urlString.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
		strcpy(url, urlString.c_str());
		httpResult = hu.httpGet(_T(url));
		// 遍历数据库
		sqlString = SQL_SUPPLIERS_FOR_UPLOAD;
		if (httpResult == "0" || httpResult == "") {
			sqlString = sqlString.replace(sqlString.find("<GMT_MODIFIED>"), 14, "2017-1-1");
		}
		else {
			sqlString = sqlString.replace(sqlString.find("<GMT_MODIFIED>"), 14, httpResult);
		}
		sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
		strcpy(sql, sqlString.c_str());
		db.getRecordRet(sql);
		uploadData = bulidUploadSuppliersData(db);
		//while (!db.HX_pRecordset->adoEOF)
		//{
		//	string strValue;
		//	// selfid group_id company_id code products_selfid suppliers_selfid
		//	_variant_t var = db.HX_pRecordset->GetCollect("selfid");
		//	if (var.vt != VT_NULL) {
		//		strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
		//	}
		//	else {
		//		strValue = "";
		//	}
		//	if (uploadData == "" || (*(uploadData.end() - 1) == '$')) {
		//		uploadData = uploadData + strValue;
		//	}
		//	else {
		//		uploadData = uploadData + "|" + strValue;
		//	}
		//	uploadData = bulidUploadColumn("iccode", db, uploadData);
		//	uploadData = bulidUploadColumn("ctype", db, uploadData);
		//	uploadData = bulidUploadColumn("group_id", db, uploadData);
		//	uploadData = bulidUploadColumn("company_id", db, uploadData);
		//	uploadData = bulidUploadColumn("name", db, uploadData);
		//	uploadData = bulidUploadColumn("htype", db, uploadData);
		//	uploadData = bulidUploadColumn("contact", db, uploadData);
		//	uploadData = bulidUploadColumn("mobile", db, uploadData);
		//	uploadData = bulidUploadColumn("pwd", db, uploadData);
		//	uploadData = bulidUploadColumn("address", db, uploadData);
		//	uploadData = bulidUploadColumn("bz", db, uploadData);
		//	uploadData = bulidUploadColumn("gmt_created", db, uploadData);
		//	uploadData = bulidUploadColumn("gmt_modified", db, uploadData);
		//	uploadData = uploadData + "$";
		//	db.HX_pRecordset->MoveNext();
		//}

		db.ExitConnect();
		// 有数据，则回传
		if (uploadData != "") {
			replace_all(uploadData, "/", "-");
			urlString = URL_DATA_UPLOAD_SUPPLIERS_DOMAIN;
			memset(domain, 0, 200);
			strcpy(domain, urlString.c_str());
			urlString = URL_DATA_UPLOAD_SUPPLIERS_URL;
			memset(url, 0, 200);
			strcpy(url, urlString.c_str());
			urlString = URL_DATA_UPLOAD_SUPPLIERS_PARAM;
			urlString = urlString.replace(urlString.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
			urlString = urlString.replace(urlString.find("<DATA>"), 6, uploadData);
			memset(data, 0, 2048);
			urlString = hu.string_To_UTF8(urlString);
			strcpy(data, urlString.c_str());
			httpResult = hu.httpPost(_T(domain), _T(url), _T(data));
		}
		Sleep(1000*60*5);
		//Sleep(1000 * 5);
		/*char print[20];
		sprintf(print, "%s%d", "数据上传次数：", count);
		MessageBox(NULL, print, "提示", NULL);
		count++;*/
	}
	return 0;
}

DWORD WINAPI UpdateStorageForPriceThread(LPVOID pParam) {
	// sql
	string sqlString;
	char sql[200];
	DBUtils db;
	// http
	string urlString;
	char domain[200];
	char url[200];
	char data[20480];
	HttpUtils hu;
	string httpResult;

	string tablename = "storage_price";
	while (true)
	{
		// 登陆下。才会更新
		if (LOGIN_COMPANYID=="") {
			Sleep(1000 * 5);
			continue;
		}
		// URL_DATA_UPDATE_LOG = "http://dibang.zz91.com/api/datatable_update.html?maxid=<MAXID>&datatable=<DATATABLE>&company_id=<COMPANY_ID>";
		urlString = URL_DATA_UPDATE_LOG;
		//string URL_DATA_UPDATE_LOG = "http://dibang.zz91.com/api/datatable_update.html?maxid=<MAXID>&datatable=<DATATABLE>&company_id=<COMPANY_ID>";
		urlString = urlString.replace(urlString.find("<COMPANY_ID>"),12 , LOGIN_COMPANYID);
		urlString = urlString.replace(urlString.find("<DATATABLE>"), 11, "storage");

		//
		string sqlString = SQL_UPDATE_LOG;
		char sql[200];
		strcpy(sql, sqlString.c_str());
		DBUtils db;
		HttpUtils hu;
		char url[200];
		db.getRecordRet(sql);
		int count = 0;
		string maxid = "";
		while (!db.HX_pRecordset->adoEOF)
		{
			//数据库数据获取
			_variant_t var = db.HX_pRecordset->GetCollect("tablename");
			string datatable = _com_util::ConvertBSTRToString((_bstr_t)var);
			if (datatable != tablename) {
				db.HX_pRecordset->MoveNext();
				continue;
			}
			var = db.HX_pRecordset->GetCollect("maxid");
			maxid = _com_util::ConvertBSTRToString((_bstr_t)var);
			db.HX_pRecordset->MoveNext();
		}
		if(maxid==""){
			urlString = urlString.replace(urlString.find("<MAXID>"), 7, "0");
		}
		else {
			urlString = urlString.replace(urlString.find("<MAXID>"), 7, maxid);
		}
		strcpy(url,urlString.c_str());
		string httpResult = hu.httpGet(_T(url));
		
		Json::Reader reader;                                    //解析json用Json::Reader
		Json::Value value;
		if (!reader.parse(httpResult,value)) {
			Sleep(1000 * 5);
			continue;
		}
		else {
			if (value["err"].asString()=="false") {
				int count = value["list"].size();
				char sqlMaxId[20];
				if (count > 0) {
					string maxGmtModified = value["list"][count-1]["gmt_modified"].asString();
					cnDatetoMaxid(sqlMaxId,maxGmtModified);					// 获取maxid
					// 然后更新
					DBUtils dbMaxId;
					strcpy(sql, sqlString.c_str());
					if (maxid == "") {
						// insert
						//string SQL_UPDATE_LOG_INSERT = "INSERT INTO  update_log (maxid,bz,tablename) values (<MAXID>,'<BZ>','<TABLENAME>')";
						sqlString = SQL_UPDATE_LOG_INSERT;
						sqlString = sqlString.replace(sqlString.find("<BZ>"), 4, "");
					}
					else {
						// update
						//string SQL_UPDATE_LOG_MAXID_CHANGE = "update update_log set maxid = <MAXID> where tablename = '<TABLENAME>'";
						sqlString = SQL_UPDATE_LOG_MAXID_CHANGE;
					}
					sqlString = sqlString.replace(sqlString.find("<MAXID>"), 7, sqlMaxId);
					sqlString = sqlString.replace(sqlString.find("<TABLENAME>"), 11, tablename);
					strcpy(sql,sqlString.c_str());
					dbMaxId.addRecord(sql);
				}
				for (int i = 0; i < count;i++) {
					/*
						{"gw": 1.5, "code": "2017091818", "gmt_modified": "2017-09-21 11:46:22",
						"selfid": "8b1e3150f6e1b742", "company_id": 17, "pay_users_selfid": "", 
						"pay_time": "", "total": 0.0, "id": 112, "gmt_created": "2017-09-18 18:09:25",
						"out_time": "", "products_selfid": "12c578986584c5cc", "tare": 0.2, "ispay": 0,
						"tare_check": 1, "nw": 1.3, "status": 2, "price": 155.6, "scorecheck": 0, "price_users_selfid": "dfgdfgsdfg",
						"price_time": "2017-09-21 11:46:22", "suppliers_selfid": "808fba5d1a37a875", "group_id": 23}, 
					*/
					string selfid = value["list"][i]["selfid"].asString();
					sqlString = SQL_STORAGE_BY_SELFID;
					sqlString = sqlString.replace(sqlString.find("<SELFID>"), 8, selfid);
					strcpy(sql, sqlString.c_str());
					db.getRecordRet(sql);
					//db.HX_pRecordset->MoveFirst();
					if (!db.HX_pRecordset->adoEOF) {
						_variant_t var = db.HX_pRecordset->GetCollect("id");			// 获取状态
						if (var.vt!=VT_NULL) {
							DBUtils updateDb;
							sqlString = SQL_STORAGE_UPDATE_TO_PRICE_OVER;
							float price = value["list"][i]["price"].asDouble();
							char priceChar[20];
							sprintf(priceChar,"%f", price);
							//update `storage` set gmt_modified = now(),status = 2,price_time = ,price=1.520000,products_selfid = '',price_users_selfid = '' ,total=<TOTAL> where id = 24
							//{L"update `storage` set gmt_modified = now(),status = 2,price_time = ,price=2.540000,products_selfid = '',price_users_selfid = '' ,total=0.000000 where id = 25" (1)}
							sqlString = sqlString.replace(sqlString.find("<PRICE>"), 7, priceChar);
							sqlString = sqlString.replace(sqlString.find("<PRICE_USERS_SELFID>"), 20, value["list"][i]["pay_user_selfid"].asString());
							sqlString = sqlString.replace(sqlString.find("<PRODUCTS_SELFID>"), 17, value["list"][i]["products_selfid"].asString());
							string price_time = value["list"][i]["price_time"].asString();
							if (price_time == "") {
								price_time = "now()";
							}
							else {
								price_time = "'" + price_time + "'";
							}
							sqlString = sqlString.replace(sqlString.find("<PRICE_TIME>"), 12, price_time);
							float total = value["list"][i]["total"].asDouble();
							char totalChar[20];
							sprintf(totalChar, "%f", total);
							sqlString = sqlString.replace(sqlString.find("<TOTAL>"), 7, totalChar);
							sqlString = sqlString.replace(sqlString.find("<ID>"), 4, _com_util::ConvertBSTRToString((_bstr_t)db.HX_pRecordset->GetCollect("id")));
							strcpy(sql, sqlString.c_str());
							updateDb.addRecord(sql);
						}
					}
				}
			}
		}

		Sleep(1000 * 5);
	}
	return 0;
}

LRESULT CALLBACK SearchProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC          hdc;
	PAINTSTRUCT  ps;
	HWND hDialog;
	switch (message)
	{
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

LRESULT CALLBACK NoPayProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC          hdc;
	PAINTSTRUCT  ps;
	HWND hDialog;
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_NOTIFY:
		MessageBox(NULL, TEXT("dianji"), TEXT("提示"), 0);
		return 0;
	case NM_CLICK:
		MessageBox(NULL, TEXT("dianji"), TEXT("提示"), 0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}


LRESULT CALLBACK PayedProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC          hdc;
	PAINTSTRUCT  ps;
	switch (message)
	{
	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

// Message handler for about box.
INT_PTR CALLBACK DialogChooseProduct(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hlist;

	PAINTSTRUCT ps;
	HDC hdc = GetDC(hDlg);

	char szBuffer[40];
	string sqlString;
	char id[10];
	DBUtils db;
	char sql[200];
	int i = 0;

	// 父窗口
	HWND parentHwnd;	
	HWND parentProductHwnd;

	// select
	char selectText[20];

	LOGFONT   logfont;       //改变输出字体
	HFONT hFont;

	RECT rcDlg, rc;
	HWND phwnd = GetParent(hDlg);

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		GetClientRect(hDlg, &rcDlg);//获取Tab窗口的大小（而不是在屏幕坐标系中）  
		GetWindowRect(phwnd, &rc);
		MoveWindow(hDlg, rc.left + 190, rc.top + 100, rcDlg.right - rcDlg.left - 5, rcDlg.bottom - rcDlg.top + 20, false);
		
		hlist = GetDlgItem(hDlg, IDC_LIST_PRODUCTS);
		// 设置ListView的列
		LVCOLUMN vcl;
		vcl.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM| LVCFMT_CENTER;
		vcl.pszText = "类别名称";//列标题
		vcl.cx = 104;//列宽
		vcl.iSubItem = 0;//子项索引，第一列无子项
		ListView_InsertColumn(hlist, 0, &vcl);

		vcl.pszText = "序号";
		vcl.cx = 120;
		vcl.iSubItem = 1;//子项索引
		ListView_InsertColumn(hlist, 1, &vcl);

		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = i;				// 行
		sqlString = SQL_PRODUCTS_SELECT_BY_COMPANY_ID;
		if (CHOOSE_CATEGORY_SELFID=="") {
			sqlString = sqlString.replace(sqlString.find("<CAULSE>"),8,"");
		}
		else {
			sqlString = sqlString.replace(sqlString.find("<CAULSE>"), 8, " and category_selfid = '" + CHOOSE_CATEGORY_SELFID+"' ");
		}
		sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"),12,LOGIN_COMPANYID );
		strcpy(sql, sqlString.c_str());
		db.getRecordRet(sql);
		while (!db.HX_pRecordset->adoEOF)
		{
			_variant_t var;
			char szBuffer[20];
			var = db.HX_pRecordset->GetCollect("name");
			getSqlSetColumn(var, szBuffer);
			lvItem.iSubItem = 0;			// 列1
			lvItem.pszText = szBuffer;
			ListView_InsertItem(hlist, (LPARAM)&lvItem);

			var = db.HX_pRecordset->GetCollect("selfid");
			getSqlSetColumn(var, szBuffer);
			lvItem.iSubItem = 1;				// 列2 iccode
			lvItem.pszText = szBuffer;
			ListView_SetItem(hlist, (LPARAM)&lvItem);;

			
			db.HX_pRecordset->MoveNext();
		}
		db.ExitConnect();
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDC_BUTTON_CHOOSE)
		{
			CHOOSE_CATEGORY_SELFID = "";
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hDlg, &ps);
		
		EndPaint(hDlg, &ps);
	case WM_NOTIFY:
		switch (wParam)
		{
		case IDC_LIST_PRODUCTS:
			LPNMITEMACTIVATE now;
			now = (LPNMITEMACTIVATE)lParam;
			//char a[50];
			//_itoa(now->iItem,a,10);
			if (now->iItem < 0) {
				break;
			}
			switch (now->hdr.code)
			{
			case NM_CLICK:
				hlist = GetDlgItem(hDlg, IDC_LIST_PRODUCTS);
				ListView_GetItemText(hlist, now->iItem, 0, selectText, 20);
				parentHwnd = GetParent(hDlg);
				parentProductHwnd = GetDlgItem(parentHwnd, IDC_EDIT_PRODUCT_NAME);
				SetWindowText(parentProductHwnd, selectText);
				ListView_GetItemText(hlist, now->iItem, 1, selectText, 20);
				CHOOSE_SAVE_PRODUCTS_SELF_ID = selectText;

				// 获取最新定价
				sqlString = SQL_STORAGE_FOR_LAST_PRICE;
				sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"),12,LOGIN_COMPANYID);
				sqlString = sqlString.replace(sqlString.find("<PRODUCTS_SELFID>"), 17, CHOOSE_SAVE_PRODUCTS_SELF_ID);
				strcpy(sql,sqlString.c_str());
				db.getRecordRet(sql);
				while (!db.HX_pRecordset->adoEOF)
				{
					_variant_t var = db.HX_pRecordset->GetCollect("price");
					string strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
					char price[20] = "          ";
					strcpy(price,strValue.c_str());

					//设置最新定价
					hdc = GetDC(parentHwnd);
					ZeroMemory(&logfont, sizeof(LOGFONT));
					logfont.lfCharSet = GB2312_CHARSET;
					logfont.lfHeight = -16;      //设置字体的大小
					hFont = CreateFontIndirect(&logfont);
					SetTextColor(hdc, RGB(230, 57, 0)); // 颜色
					SetBkColor(hdc, RGB(240, 240, 240));
					SelectObject(hdc, hFont);
					TextOut(hdc, 181, 210, price, strlen(price));
					ReleaseDC(parentHwnd, hdc);
					DeleteObject(hFont);
					db.HX_pRecordset->MoveNext();
				}
				
				db.ExitConnect();
				break;
			case NM_DBLCLK:
				hlist = GetDlgItem(hDlg, IDC_LIST_PRODUCTS);
				ListView_GetItemText(hlist, now->iItem, 0, selectText, 20);
				parentHwnd = GetParent(hDlg);
				parentProductHwnd = GetDlgItem(parentHwnd, IDC_EDIT_PRODUCT_NAME);
				SetWindowText(parentProductHwnd, selectText);
				ListView_GetItemText(hlist, now->iItem, 1, selectText, 20);
				CHOOSE_SAVE_PRODUCTS_SELF_ID = selectText;

				// 获取最新定价
				sqlString = SQL_STORAGE_FOR_LAST_PRICE;
				sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
				sqlString = sqlString.replace(sqlString.find("<PRODUCTS_SELFID>"), 17, CHOOSE_SAVE_PRODUCTS_SELF_ID);
				strcpy(sql, sqlString.c_str());
				db.getRecordRet(sql);
				while (!db.HX_pRecordset->adoEOF)
				{
					_variant_t var = db.HX_pRecordset->GetCollect("price");
					string strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
					char price[20] = "          ";
					strcpy(price, strValue.c_str());

					//设置最新定价
					hdc = GetDC(parentHwnd);
					ZeroMemory(&logfont, sizeof(LOGFONT));
					logfont.lfCharSet = GB2312_CHARSET;
					logfont.lfHeight = -16;      //设置字体的大小
					hFont = CreateFontIndirect(&logfont);
					SetTextColor(hdc, RGB(230, 57, 0)); // 颜色
					SetBkColor(hdc, RGB(240, 240, 240));
					SelectObject(hdc, hFont);
					TextOut(hdc, 181, 210, price, strlen(price));
					ReleaseDC(parentHwnd, hdc);
					DeleteObject(hFont);
					db.HX_pRecordset->MoveNext();
				}

				db.ExitConnect();
				CHOOSE_CATEGORY_SELFID = "";
				EndDialog(hDlg, LOWORD(wParam));
				break;
			case NM_RCLICK:
				break;
			}
		}
	}
	return (INT_PTR)FALSE;
}

int getComboPortLvSel(string portlv) {
	int compareLv = atoi(portlv.c_str());
	int i = 0;
	for (; i < 12;i++) {
		if (compareLv == PORT_LV[i]) {
			break;
		}
	}
	return i;
}

// Message handler for about box.
INT_PTR CALLBACK DialogPortConfig(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	DBUtils db;
	string sqlString;
	char sql[200];

	HWND hTab = GetDlgItem(hDlg, IDC_TAB_PORT_CONFIG);;
	HWND hPage;

	char com[20];		// com口
	char portlv[20];	// 波特率

	HWND phwnd = GetParent(hDlg);
	RECT rcDlg;			// 对话框自身参数
	RECT rc;			// 对话框位置相对于屏幕参数

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	//case WM_PAINT:
	case WM_INITDIALOG:

		GetClientRect(hDlg, &rcDlg);//获取Tab窗口的大小（而不是在屏幕坐标系中）  
		GetWindowRect(phwnd, &rc);
		MoveWindow(hDlg, rc.left+290, rc.top+100, rcDlg.right - rcDlg.left - 5, rcDlg.bottom - rcDlg.top + 20, false);

		InitTabControlForPort(hTab, NULL, lParam);
		sqlString = SQL_EQSET_SELECT_ALL;
		strcpy(sql,sqlString.c_str());
		db.getRecordRet(sql);
		while (!db.HX_pRecordset->adoEOF)
		{
			_variant_t var = db.HX_pRecordset->GetCollect("eq_type");
			if (var.vt == VT_NULL) {
				continue;
			}
			string strType = _com_util::ConvertBSTRToString((_bstr_t)var);
			var = db.HX_pRecordset->GetCollect("eq_com");
			string strCom = "";
			if (var.vt != VT_NULL) {
				strCom = _com_util::ConvertBSTRToString((_bstr_t)var);
			}
			var = db.HX_pRecordset->GetCollect("eq_btl");
			string strBtl = "";
			if (var.vt != VT_NULL) {
				strBtl = _com_util::ConvertBSTRToString((_bstr_t)var);
			}
			if (strType =="1") {
				strCom = strCom.replace(strCom.find("COM"), 3, "");
				int selcom = atoi(strCom.c_str());
				ComboBox_SetCurSel(tabBDlg[0], selcom-1);
				ComboBox_SetCurSel(tabBDlg[1], getComboPortLvSel(strBtl));
			}
			else if (strType == "2") {
				strCom = strCom.replace(strCom.find("COM"), 3, "");
				int selcom = atoi(strCom.c_str());
				ComboBox_SetCurSel(tabSDlg[0], selcom - 1);
				ComboBox_SetCurSel(tabSDlg[1], getComboPortLvSel(strBtl));
			}
			db.HX_pRecordset->MoveNext();
		}
		db.ExitConnect();
		ShowWindow(tabBDlg[0], TRUE);
		ShowWindow(tabBDlg[1], TRUE);
		ShowWindow(tabSDlg[0], FALSE);
		ShowWindow(tabSDlg[1], FALSE);
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL )
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_BUTTON_PORT_CONFIG_SAVE)
		{
			int count;
			string eq_type;
			string eq_name;
			sqlString = SQL_EQSET_SELECT_HASEXIST;
			// 更新设备的信息
			int sel = TabCtrl_GetCurSel(hTab);
			if (sel == 0) {
				GetWindowText(tabBDlg[0], com, 20);
				GetWindowText(tabBDlg[1], portlv, 20);
				eq_type = "1";
				eq_name = "大磅";
			}else if (sel == 1) {
				GetWindowText(tabSDlg[0], com, 20);
				GetWindowText(tabSDlg[1], portlv, 20);
				eq_type = "2";
				eq_name = "小磅";
			}
			if (com[0] == '\0') {
				MessageBox(hDlg, TEXT("com数据必须选择"), TEXT("错误"), 0);
				break;
			}
			if (portlv[0] == '\0') {
				MessageBox(hDlg, TEXT("请选择波特率"), TEXT("错误"), 0);
				break;
			}
			sqlString = sqlString.replace(sqlString.find("<EQ_TYPE>"), 9, eq_type);
			strcpy(sql, sqlString.c_str());
			db.getRecordRet(sql);
			while (!db.HX_pRecordset->adoEOF) {
				_variant_t var = db.HX_pRecordset->GetCollect("hasExist");
				string strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
				count = atoi(strValue.c_str());
				db.HX_pRecordset->MoveNext();
			}
			if (count > 0) {
				sqlString = SQL_EQSET_UPDATE;
			}
			else {
				sqlString = SQL_EQSET_INSERT;
				sqlString = sqlString.replace(sqlString.find("<EQ_SJW>"), 8, "8");
				sqlString = sqlString.replace(sqlString.find("<EQ_JYW>"), 8, "N");
				sqlString = sqlString.replace(sqlString.find("<EQ_TZW>"), 8, " ");
				sqlString = sqlString.replace(sqlString.find("<EQ_NAME>"), 9, eq_name);
			}
			sqlString = sqlString.replace(sqlString.find("<EQ_COM>"), 8, com);
			sqlString = sqlString.replace(sqlString.find("<EQ_BTL>"), 8, portlv);
			sqlString = sqlString.replace(sqlString.find("<EQ_TYPE>"), 9, eq_type);
			strcpy(sql, sqlString.c_str());
			db.addRecord(sql);
			MessageBox(hDlg, TEXT("设置成功，请重启程序测试端口"), TEXT("提示"), 0);

			// 初始化port
			sqlString = SQL_EQSET_SELECT_ALL;
			strcpy(sql, sqlString.c_str());
			db.getRecordRet(sql);
			while (!db.HX_pRecordset->adoEOF)
			{
				_variant_t var = db.HX_pRecordset->GetCollect("eq_type");
				if (var.vt == VT_NULL) {
					continue;
				}
				string strType = _com_util::ConvertBSTRToString((_bstr_t)var);
				var = db.HX_pRecordset->GetCollect("eq_com");
				string strCom = "";
				if (var.vt != VT_NULL) {
					strCom = _com_util::ConvertBSTRToString((_bstr_t)var);
				}
				var = db.HX_pRecordset->GetCollect("eq_btl");
				string strBtl = "";
				if (var.vt != VT_NULL) {
					strBtl = _com_util::ConvertBSTRToString((_bstr_t)var);
				}
				char message[200];
				if (strType == "1") {
					strCom = strCom.replace(strCom.find("COM"), 3, "");
					int selcom = atoi(strCom.c_str());
					int btl = atoi(strBtl.c_str());
					if (!mySerialPort.InitPort(selcom, btl, 'N', 8, 1, NULL) || selcom < 1 || btl< 1)
					{
					}
					else
					{
						mySerialPort.isClose = false;
					}
				}
				else if (strType == "2") {
					strCom = strCom.replace(strCom.find("COM"), 3, "");
					int selcom = atoi(strCom.c_str());
					int btl = atoi(strBtl.c_str());
					if (!mySerialPort2.InitPort(selcom, btl, 'N', 8, 1, EV_RXCHAR) || selcom < 1 || btl< 1)
					{
					}
					else
					{
						mySerialPort2.isClose = false;
					}
				}
				db.HX_pRecordset->MoveNext();
			}
			db.ExitConnect();

			//initPort(mySerialPort, mySerialPort2);

			//EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	case WM_NOTIFY:
		if ((INT)wParam == IDC_TAB_PORT_CONFIG)
		{
			hTab = GetDlgItem(hDlg, IDC_TAB_PORT_CONFIG);
			if (((LPNMHDR)lParam)->code == TCN_SELCHANGE) //当TAB标签转换的时候发送TCN_SELCHANGE消息  
			{
				int sel = TabCtrl_GetCurSel(hTab);
				if (sel == 0) {
					ShowWindow(tabBDlg[0], TRUE);
					ShowWindow(tabBDlg[1], TRUE);
					ShowWindow(tabSDlg[0], FALSE);
					ShowWindow(tabSDlg[1], FALSE);
				}
				else if (sel == 1) {
					ShowWindow(tabBDlg[0], FALSE);
					ShowWindow(tabBDlg[1], FALSE);
					ShowWindow(tabSDlg[0], TRUE);
					ShowWindow(tabSDlg[1], TRUE);
				}
			}
		}
		return (INT_PTR)TRUE;
	}
	return (INT_PTR)FALSE;
}