// CppConsole.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"

#pragma comment(lib, "json1.lib")

#include "HttpUtils.h"
#include <fstream>
#include <cassert>
#include "json/json.h"
#include <atlconv.h>




#define X "\r\n"
#define URL "http://www.zz91.com/doStatusLogin.htm"  //请求地址
#define _COM "www.zz91.com" //网站地址
#define _COM_URL "/doStatusLogin.htm"      //请求页面
#define PORT 0x50   //端口号80
#define _POST "username=kongsj&password=123456" //POST请求数据
#define MAXSIZE 4096

//每次读取的字节数
#define READ_BUFFER_SIZE        4096

HttpUtils::~HttpUtils(void)
{
    
}

string HttpUtils::httpGet(_TCHAR* url){
	string result;
	HINTERNET hSession = InternetOpen(_T("UrlTest"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	wchar_t *pwText = NULL;
	if(hSession != NULL){
		HINTERNET hHttp = InternetOpenUrl(hSession, url, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0);
        if (hHttp != NULL)
        {
            //wprintf_s(_T("%s\n"), _T("http://www.zz91.com"));
			BYTE Temp[MAXSIZE] ={0};
			char szBuffer[READ_BUFFER_SIZE + 1] = {0};
            ULONG Number = 1;
            while (Number > 0)
            {
                InternetReadFile(hHttp, szBuffer, MAXSIZE - 1, &Number);
                szBuffer[Number] = '\0';

				//
				DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, szBuffer, -1, NULL, 0);    //返回原始ASCII码的字符数目
				pwText = new wchar_t[dwNum];                                                //根据ASCII码的字符数分配UTF8的空间
				MultiByteToWideChar(CP_UTF8, 0, szBuffer, -1, pwText, dwNum);           //将ASCII码转换成UTF8
				//
                //printf("%s", szBuffer);
				//result = Temp;
				//string temp;
				USES_CONVERSION;
				char *psText = W2A(pwText);
				//sprintf(psText, "%s", pwText);
				//psText = new char[dwNum];
				//WideCharToMultiByte(CP_OEMCP,NULL,pwText,-1,psText,dwNum,NULL,FALSE);
				result.append(psText);
            }
			InternetCloseHandle(hHttp);
            hHttp = NULL;
        }
        InternetCloseHandle(hSession);
        hSession = NULL;
	}
	return result;
}

string HttpUtils::string_To_UTF8(const string & str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

int HttpUtils::httpPost(_TCHAR* domain,_TCHAR* url,_TCHAR* data){
	//原始协议头
    char _HTTP_ARAC[] = "Accept: */*\r\n"\
                        "Referer: "\
                        "URL"\
                        "X"\
                        "Accept-Language: zh-cn\r\n"\
                        "Content-Type: application/x-www-form-urlencoded\r\n\r\n";
    char _HTTP_POST[] = _POST;
    char _HTTP_File[1024] ={0};
    //cout << "提交HTTP协议头：" << _HTTP_ARAC << endl;
    //cout << "提交POST数据：" << _POST << endl;
    HINTERNET Inte = InternetOpenA ("Mozilla/4.0 (compatible; MSIE 9.0; Windows NT 6.1)", 1, "" ,"" ,0);    //初始化Wininet并创建一个HTTP连接
    if(0 == Inte){
        InternetCloseHandle(Inte);
        //cout << "error InternetOpen" << endl;
    }
    HINTERNET Connect = InternetConnectA (Inte, domain,PORT, "","" , 3, 0, 0);    //请求与网站连接
      if(0 == Connect){
        InternetCloseHandle(Inte);
        InternetCloseHandle(Connect);
        //cout << "error InternetConnect" << endl;
    }
	HINTERNET HttpOpen = HttpOpenRequestA (Connect, "POST", url, "HTTP/1.1", NULL,NULL,1, 0);  //向网站服务器发送请求页面
      if(0 == HttpOpen){
        InternetCloseHandle(Inte);
        InternetCloseHandle(Connect);
        InternetCloseHandle(HttpOpen);
        //cout << "error HttpOpenRequest" << endl;
    }

    BOOL bo = HttpSendRequestA(HttpOpen, _HTTP_ARAC, strlen(_HTTP_ARAC), data, strlen (data));  //向网站服务器发送请求HTTP协议和POST请求数据
    if(bo == false){
        InternetCloseHandle(Inte);
        InternetCloseHandle(Connect);
        InternetCloseHandle(HttpOpen);
        //cout << "error HttpSendRequest" << endl;
    }
    DWORD y = 0;
     if(!InternetReadFile (HttpOpen, _HTTP_File, 1024, &y)){    //获取HTTP响应消息
        InternetCloseHandle(Inte);
        InternetCloseHandle(Connect);
        InternetCloseHandle(HttpOpen);
        //cout << "error InternetReadFile" << endl;
    }else{
        //printf("%s",_HTTP_File);    //打印得到的响应消息
    }
	return 0;
}