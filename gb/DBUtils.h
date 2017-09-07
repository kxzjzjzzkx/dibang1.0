#pragma once
#include "stdafx.h"


class DBUtils
{
public:
	_ConnectionPtr HX_pConnection; // 数据库指针  
	_RecordsetPtr  HX_pRecordset; // 命令指针  
	_CommandPtr HX_pCommand; // 记录指针  
	void OnInitADOConn();
	void ExitConnect();
};
