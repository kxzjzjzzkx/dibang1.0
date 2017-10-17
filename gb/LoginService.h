#pragma once
#include "stdafx.h"
#include "sysconfig.h"

/*
获取总价以及计算净重

gw			毛重
tare		皮重
nw			净重
price		定价(单价)
totalPrice	总价(单价 乘 净重)
*/
string getCtypeByCid(string cid)
{
	string ctype="0",sqlString;
	char sql[200];
	DBUtils db;
	sqlString = SQL_COMPANY_CTYPE_BY_ID;
	sqlString = sqlString.replace(sqlString.find("<ID>"), 4, cid);
	strcpy(sql,sqlString.c_str());
	db.getRecordRet(sql);
	while (!db.HX_pRecordset->adoEOF) {
		_variant_t var = db.HX_pRecordset->GetCollect("ctype");
		if (var.vt != VT_NULL) {
			ctype = _com_util::ConvertBSTRToString((_bstr_t)var);
		}
		db.HX_pRecordset->MoveNext();
	}
	return ctype;
}