// ale.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>

#pragma comment(lib,"wininet.lib")

#include <iostream>
#include <cstdio>
#include <WinInet.h>
#define X "\r\n"
#define URL "http://www.zz91.com/doLogin.htm"  //请求地址
#define _COM "www.zz91.com" //网站地址
#define _COM_URL "/doLogin.htm"      //请求页面
#define PORT 0x50   //端口号80
#define _POST "account=kongsj&password=123456" //POST请求数据
#define MAXSIZE 1024
using namespace std;

int main()
{
	////原始协议头
 //   char _HTTP_ARAC[] = "Accept: */*\r\n"\
 //                       "Referer: "\
 //                       "URL"\
 //                       "X"\
 //                       "Accept-Language: zh-cn\r\n"\
 //                       "Content-Type: application/x-www-form-urlencoded\r\n\r\n";
 //   char _HTTP_POST[] = _POST;
 //   char _HTTP_File[1024] ={0};
 //   cout << "提交HTTP协议头：" << _HTTP_ARAC << endl;
 //   cout << "提交POST数据：" << _POST << endl;
 //   HINTERNET Inte = InternetOpenA ("Mozilla/4.0 (compatible; MSIE 9.0; Windows NT 6.1)", 1, "" ,"" ,0);    //初始化Wininet并创建一个HTTP连接
 //   if(0 == Inte){
 //       InternetCloseHandle(Inte);
 //       cout << "error InternetOpen" << endl;
 //   }
 //   HINTERNET Connect = InternetConnectA (Inte, _COM,PORT, "","" , 3, 0, 0);    //请求与网站连接
 //     if(0 == Connect){
 //       InternetCloseHandle(Inte);
 //       InternetCloseHandle(Connect);
 //       cout << "error InternetConnect" << endl;
 //   }

	HINTERNET hSession = InternetOpen(_T("UrlTest"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(hSession != NULL){
		HINTERNET hHttp = InternetOpenUrl(hSession, _T("http://m.taozaisheng.com"), NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
        if (hHttp != NULL)
        {
            wprintf_s(_T("%s\n"), _T("http://www.zz91.com"));
            BYTE Temp[MAXSIZE];
            ULONG Number = 1;
            while (Number > 0)
            {
                InternetReadFile(hHttp, Temp, MAXSIZE - 1, &Number);
                Temp[Number] = '\0';
                printf("%s", Temp);
            }
            InternetCloseHandle(hHttp);
            hHttp = NULL;
        }
        InternetCloseHandle(hSession);
        hSession = NULL;
	}



    //HINTERNET HttpOpen = HttpOpenRequestA (Connect, "POST", _COM_URL, "HTTP/1.1", NULL,NULL,1, 0);  //向网站服务器发送请求页面
    //  if(0 == HttpOpen){
    //    InternetCloseHandle(Inte);
    //    InternetCloseHandle(Connect);
    //    InternetCloseHandle(HttpOpen);
    //    cout << "error HttpOpenRequest" << endl;
    //}

    //BOOL bo = HttpSendRequestA(HttpOpen, _HTTP_ARAC, strlen(_HTTP_ARAC), _HTTP_POST, strlen (_HTTP_POST));  //向网站服务器发送请求HTTP协议和POST请求数据
    //if(bo == false){
    //    InternetCloseHandle(Inte);
    //    InternetCloseHandle(Connect);
    //    InternetCloseHandle(HttpOpen);
    //    cout << "error HttpSendRequest" << endl;
    //}
    //DWORD y = 0;
    // if(!InternetReadFile (HttpOpen, _HTTP_File, 1024, &y)){    //获取HTTP响应消息
    //    InternetCloseHandle(Inte);
    //    InternetCloseHandle(Connect);
    //    InternetCloseHandle(HttpOpen);
    //    cout << "error InternetReadFile" << endl;
    //}else{
    //    printf("%s",_HTTP_File);    //打印得到的响应消息
    //}
    getchar();
    return 0;
}
