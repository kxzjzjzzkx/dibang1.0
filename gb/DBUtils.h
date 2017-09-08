#pragma once
#include "stdafx.h"

#import "c:\Program Files\Common Files\System\ado\msado15.dll"  no_namespace rename("EOF","adoEOF")

class DBUtils
{
public:
	_ConnectionPtr HX_pConnection; // 数据库指针  
	_RecordsetPtr  HX_pRecordset; // 命令指针  
	_CommandPtr HX_pCommand; // 记录指针  

	BSTR _ErrorMessage;

	void OnInitADOConn();
	void ExitConnect();
};
