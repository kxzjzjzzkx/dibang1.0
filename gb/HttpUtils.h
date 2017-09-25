// CppConsole.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
//#include <winhttp.h>
//#pragma comment(lib, "winhttp")

// ale.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#pragma comment(lib,"wininet.lib")

#include <iostream>
#include <cstdio>
#include <WinInet.h>

#include <list>
#include <queue> 

using namespace std;

class HttpUtils{
	public:
		
		~HttpUtils(void);
		
		string httpGet(_TCHAR* url);
		
		int httpPost(_TCHAR* domain,_TCHAR* url);
		
		// 解析卡口Json数据 [3/14/2017/shike]
		//DBInfo ParseJsonInfo(const string &strJsonInfo);
		// 获取list 数据
		//list<DBInfo> HttpUtils::ParseJsonInfoForList(const string &strJsonInfo);
		//queue<DBInfo> HttpUtils::ParseJsonInfoForList(const string &strJsonInfo);
	protected:
		

		// 关闭句柄 [3/14/2017/shike]
		void Release();

		// 释放句柄 [3/14/2017/shike]
		//void ReleaseHandle( HINTERNET& hInternet );

		// 解析URL地址 [3/14/2017/shike]
		//void ParseURLWeb( std::string strUrl, std::string& strHostName, std::string& strPageName, WORD& sPort);

		// UTF-8转为GBK2312 [3/14/2017/shike]
		//char* UtfToGbk(const char* utf8);
	private:
		/*HINTERNET            m_hSession;
		HINTERNET            m_hConnect;
		HINTERNET            m_hRequest;
		HttpInterfaceError    m_error;*/
};
