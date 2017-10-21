#pragma once
#pragma once
#include "stdafx.h"
#include <string>
using namespace std;
//控制字定义
#define BLOCK0_EN  0x01
#define BLOCK1_EN  0x02
#define BLOCK2_EN  0x04
#define NEEDSERIAL 0x08
#define EXTERNKEY  0x10
#define NEEDHALT   0x20
#pragma hdrstop
#include <Mergemod.h>


/*
ic卡数据获取
*/
void getIC(char* str, char* tempStr) {
	int j = 0;
	;	for (size_t i = 0; i < strlen(tempStr); i++)
	{
		if (j >= 16)
		{
			break;
		}
		if (i % 2 == 0) {
			continue;
		}
		str[j] = tempStr[i];
		j++;
	}
	str[j] = '\0';
}

void convertUnCharToStr(char* str, unsigned char* UnChar, int ucLen)
{
	int i = 0;
	char tempStr[100];
	for (i = 0; i < ucLen; i++)
	{
		//格式化输str,每unsigned char 转换字符占两位置%x写输%X写输  
		sprintf(tempStr + i * 2, "%02x", UnChar[i]);
	}
	getIC(str, tempStr);
}

void convertStrToUnChar(char* str, unsigned char* UnChar)
{
	int i = strlen(str), j = 0, counter = 0;
	char c[2];
	unsigned int bytes[2];

	for (j = 0; j < i; j += 2)
	{
		if (0 == j % 2)
		{
			c[0] = str[j];
			c[1] = str[j + 1];
			sscanf(c, "%02x", &bytes[0]);
			UnChar[counter] = bytes[0];
			counter++;
		}
	}
	return;
}

void ReadICCard(char* result,bool* hasRead,char* errorMsg) {
	unsigned char status;//存放返回值
	unsigned char myareano;//区号
	unsigned char authmode;//密码类型，用A密码或B密码
	unsigned char myctrlword;//控制字
	unsigned char mypicckey[6];//密码
	unsigned char mypiccserial[4];//卡序列号
	unsigned char mypiccdata[48]; //卡数据缓冲
	unsigned char(__stdcall *piccreadex)(unsigned char ctrlword, unsigned char *serial, unsigned char area, unsigned char keyA1B0, unsigned char *picckey, unsigned char *piccdata0_2);
	//判断动态库是否存在
	char filePath[200];
	GetModuleFileName(NULL, filePath, 200);
	string FileName = filePath;
	FileName = FileName.replace(FileName.find("gb.exe"), 6, "");
	FileName += "OUR_MIFARE.dll";
	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFile(FileName.c_str(), &wfd);
	if (INVALID_HANDLE_VALUE == hFind)
	{
		sprintf(errorMsg,"%s","OUR_MIFARE.dll文件丢失");
		//MessageBox(NULL, TEXT("OUR_MIFARE.dll文件丢失"), TEXT("错误"), NULL);
		return ;
	}
	FindClose(hFind);
	//CloseHandle(hFind);
	//提取函数
	HINSTANCE hDll;
	hDll = LoadLibrary(FileName.c_str());
	piccreadex = (unsigned char(__stdcall *)(unsigned char, unsigned char *, unsigned char, unsigned char, unsigned char *, unsigned char *))GetProcAddress(hDll, "piccreadex");
	//控制字指定,控制字的含义请查看本公司网站提供的动态库说明
	myctrlword = BLOCK0_EN + BLOCK1_EN + BLOCK2_EN + EXTERNKEY;
	//指定区号
	myareano = 8;//指定为第8区
				 //批定密码模式
	authmode = 1;//大于0表示用A密码认证，推荐用A密码认证
				 //指定密码
	mypicckey[0] = 0xff;
	mypicckey[1] = 0xff;
	mypicckey[2] = 0xff;
	mypicckey[3] = 0xff;
	mypicckey[4] = 0xff;
	mypicckey[5] = 0xff;
	status = piccreadex(myctrlword, mypiccserial, myareano, authmode, mypicckey, mypiccdata);
	FreeLibrary(hDll);
	hasRead[0] = false;
	//在下面设定断点，然后查看mypiccserial、mypiccdata，
	//调用完 piccreadex函数可读出卡序列号到 mypiccserial，读出卡数据到mypiccdata，
	//开发人员根据自己的需要处理mypiccserial、mypiccdata 中的数据了。
	//处理返回函数
	switch (status)
	{
	case 0:
		hasRead[0] = true;
		convertUnCharToStr(result, mypiccdata, 48);
		break;
	case 8:
		sprintf(errorMsg, "%s", "请将卡放在感应区");
	//	MessageBox(hDlg, TEXT("请将卡放在感应区"), TEXT("提示"), NULL);
		break;
	case 23:
		sprintf(errorMsg, "%s", "异常，没有发现IC卡设备");
		//MessageBox(hDlg, TEXT("异常，没有发现IC卡设备"), TEXT("提示"), NULL);
		break;
	}
	//返回解释
	/*
	#define ERR_REQUEST 8//寻卡错误
	#define ERR_READSERIAL 9//读序列吗错误
	#define ERR_SELECTCARD 10//选卡错误
	#define ERR_LOADKEY 11//装载密码错误
	#define ERR_AUTHKEY 12//密码认证错误
	#define ERR_READ 13//读卡错误
	#define ERR_WRITE 14//写卡错误
	#define ERR_NONEDLL 21//没有动态库
	#define ERR_DRIVERORDLL 22//动态库或驱动程序异常
	#define ERR_DRIVERNULL 23//驱动程序错误或尚未安装
	#define ERR_TIMEOUT 24//操作超时，一般是动态库没有反映
	#define ERR_TXSIZE 25//发送字数不够
	#define ERR_TXCRC 26//发送的CRC错
	#define ERR_RXSIZE 27//接收的字数不够
	#define ERR_RXCRC 28//接收的CRC错
	*/
}