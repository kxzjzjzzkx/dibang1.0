#include <stdafx.h>
#include <WinInet.h>

#pragma comment(lib,"wininet.lib")

int main()
{
	//第一步，连接12306网站服务器
	LPCTSTR lpszAgent = L"WinInetGet/0.1";
	HINTERNET hInternet = InternetOpen( lpszAgent,
		INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	LPCTSTR lpszServerName = L"kyfw.12306.cn";
	INTERNET_PORT nServerPort = INTERNET_DEFAULT_HTTPS_PORT; // HTTPS端口443
	LPCTSTR lpszUserName = NULL; //无登录用户名
	LPCTSTR lpszPassword = NULL; //无登录密码
	DWORD dwConnectFlags = 0;
	DWORD dwConnectContext = 0;
	HINTERNET hConnect = InternetConnect(hInternet,
		lpszServerName, nServerPort,
		lpszUserName, lpszPassword,
		INTERNET_SERVICE_HTTP,
		dwConnectFlags, dwConnectContext);
	if(hConnect == NULL)
	{
		return false;
	}

	//第二步，访问12306网站登录验证码接口
	LPCTSTR lpszVerb = L"GET";
	LPCTSTR lpszObjectName = L"/otn/passcodeNew/getPassCodeNew.do?module=login&rand=sjrand";//L"/urchin.js";
	LPCTSTR lpszVersion = NULL;    // 默认.
	LPCTSTR lpszReferrer = L"";   // 没有引用页
	LPCTSTR *lplpszAcceptTypes = NULL; // Accpet所有类型.
	DWORD dwOpenRequestFlags =  INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP |
		INTERNET_FLAG_KEEP_CONNECTION |
		INTERNET_FLAG_NO_AUTH |
		INTERNET_FLAG_NO_COOKIES |
		INTERNET_FLAG_NO_UI | 
		INTERNET_FLAG_SECURE | //设置启用HTTPS
		INTERNET_FLAG_RELOAD;
	DWORD dwOpenRequestContext = 0;
	//初始化Request
	HINTERNET hRequest = HttpOpenRequest(hConnect, lpszVerb, lpszObjectName, lpszVersion,
		lpszReferrer, lplpszAcceptTypes,
		dwOpenRequestFlags, dwOpenRequestContext);

	//发送Request
	BOOL bResult = HttpSendRequest(hRequest, NULL, 0, NULL, 0);
	//失败
	if (!bResult) 
	{
		fprintf(stderr, "HttpSendRequest failed, error = %d (0x%x)\n",
			GetLastError(), GetLastError());
		return -2;
	}

	//第三步，获取返回结果
	//获得HTTP Response Header信息
	DWORD dwInfoLevel = HTTP_QUERY_RAW_HEADERS_CRLF;
	DWORD dwInfoBufferLength = 2048;
	BYTE *pInfoBuffer = (BYTE *)malloc(dwInfoBufferLength+2);
	while (!HttpQueryInfo(hRequest, dwInfoLevel, pInfoBuffer, &dwInfoBufferLength, NULL))
	{
		DWORD dwError = GetLastError();
		if (dwError == ERROR_INSUFFICIENT_BUFFER)
		{
			free(pInfoBuffer);
			pInfoBuffer = (BYTE *)malloc(dwInfoBufferLength + 2);
		}
		else
		{
			fprintf(stderr, "HttpQueryInfo failed, error = %d (0x%x)\n",
				GetLastError(), GetLastError());
			break;
		}
	}
	pInfoBuffer[dwInfoBufferLength] = '\0';
	pInfoBuffer[dwInfoBufferLength + 1] = '\0';
	printf("%s", pInfoBuffer); //很奇怪HttpQueryInfo保存的格式是wchar_t 和下面的InternetReadFile不一样
	free(pInfoBuffer);

	//获得HTTP Response 的 Body, 这里是png图片二进制数据流
	DeleteFile(L"verifycode.png");
	DWORD dwBytesAvailable;
	while (InternetQueryDataAvailable(hRequest, &dwBytesAvailable, 0, 0))
	{
		BYTE *pMessageBody = (BYTE *)malloc(dwBytesAvailable+1);
		DWORD dwBytesRead;
		BOOL bResult = InternetReadFile(hRequest, pMessageBody,
			dwBytesAvailable, &dwBytesRead);
		if (!bResult)
		{
			fprintf(stderr, "InternetReadFile failed, error = %d (0x%x)\n",
				GetLastError(), GetLastError());
			break;
		}
		if (dwBytesRead == 0)
			break; // End of File.
		pMessageBody[dwBytesRead] = '\0';
		printf("%s", pMessageBody); //InternetReadFile读出来的是普通的char. InternetReadFileEx 似乎是有宽字节版本的
		//MessageBox((LPCTSTR)(char*)pMessageBody);
		write_file("verifycode.png",(char*)pMessageBody,dwBytesRead);
		free(pMessageBody);
	}
	return 0;
}