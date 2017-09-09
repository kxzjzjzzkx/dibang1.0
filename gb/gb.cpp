// gb.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "gb.h"
#include "Resource.h"
#include "CommCtrl.h"
#include "HttpUtils.h"
#include "SerialPort.h"
#include "DBUtils.cpp"
#include <stdlib.h>
#include "MD5.h"


// import json cpp
#pragma comment(lib, "json1.lib")
#include "json/json.h"
//

// import class User 
#include "users.h"

// file io
#include <fstream>
#include <iostream>

// import StringUtils
#include "StringUtils.h"

#pragma comment ( lib,"comctl32.lib" )  

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// dibang config
CSerialPort mySerialPort;
// http config
string URL_LOGIN = "http://dibang.zz91.com/api/loginsave.html?username=<USERNAME>&pwd=<PASSWORD>";
string URL_GET_SELLER = "http://dibang.zz91.com/api/searchsuppliers.html?iccode=<iccode>";

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

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
  // hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG_STORAGE), GetDesktopWindow(), (DLGPROC)DialogStorage);

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
	char clientid[50];
	ifstream infile;
	DBUtils db;
	char sql[300];
	string sqlString;
	int count = 0;
	switch (message)
	{
	case WM_CREATE:
		//判定是否有client，如果有，则获取client下的用户登录
		infile.open("client.config");
		infile >> clientid;
		sqlString = "select id from users where clientid = '<CLIENTID>'";
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
			db.HX_pRecordset->MoveNext();
		}
		db.ExitConnect();
		if (count>0)
		{
			//MessageBox(NULL, TEXT("登陆成功"), TEXT("消息"), 0);
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_STORAGE), hWnd, DialogStorage);
		}
		else {
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_LOGIN), hWnd, DialogLogin);
		}
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
	mySerialPort.setHDlg(hDlg);
	char str[100];
	HDC hdc;
	hdc = GetDC(hDlg);
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
		if (LOWORD(wParam) == IDC_DO_LOGIN )   // 登录逻辑
		{
			char clientid[50];		
			memset(clientid, 0, 50);
			HWND accounthDlg = GetDlgItem(hDlg, IDC_ACCOUNT);
			HWND pwdDlg = GetDlgItem(hDlg, IDC_PASSWORD);
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
			string sqlString = "select count(*) as hasExist from users where username='<USERNAME>' and pwd='<PWD>' ";
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
				if (var.vt != VT_NULL)
				strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
				count = atoi(strValue.c_str()); 
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
				strcpy(httpUrl, url.c_str());
				HttpUtils hu;
				string httpResult = hu.httpGet(_T(httpUrl));
				Json::Reader reader;                                    //解析json用Json::Reader
				Json::Value value;
				//可以代表任意类型
				if (!reader.parse(httpResult, value))
				{
					// 检验失败，弹框返回返回
					MessageBox(NULL, TEXT("网络故障请重试！"), TEXT("消息"), 0);
				}
				else {
					char selfidChar[50];
					if (value["err"].asString()=="true") {
						char errMsg[50];
						string errMsgString = value["errtext"].asString();
						strcpy(errMsg,errMsgString.c_str());
						MessageBox(NULL, TEXT(errMsg), TEXT("消息"), 0);
					}
					else {
						// 检验成功，导入数据进本地数据库
						string selfid = value["result"]["selfid"].asString();
						strcpy(selfidChar, selfid.c_str());
						int group_id = value["result"]["group_id"].asInt();
						int company_id = value["result"]["company_id"].asInt();
						string sqlInsert = "Insert INTO users (selfid,group_id,company_id,clientid,username,pwd) VALUES ('<SELFID>',<GROUP_ID>,<COMPANY_ID>,'<CLIENTID>','<USERNAME>','<PWD>')";
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
						EndDialog(hDlg, LOWORD(wParam));
					}
				}
			}

			return (INT_PTR)TRUE;
		}
		
		break;
	}
	return (INT_PTR)FALSE;
}