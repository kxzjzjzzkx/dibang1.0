#include "stdafx.h"
//#pragma once
//#include "config.h"
//
//#include "DBUtils.cpp"
//#include "Resource.h"
//
///*
//	初始化combo下拉框数据 数据来源 category_products
//*/
//void getCombo(HWND hwnd,string companyId) {
//	DBUtils db;
//	HWND hcombo = GetDlgItem(hwnd, IDC_COMBO_PRODUCT_CODE);
//	string sqlString = SQL_CATEGORY_PRODUCTS_SELECT_BY_COMPANY_ID;
//	sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"),12,companyId);
//	char sql[200];
//	strcpy(sql,sqlString.c_str());
//	db.getRecordRet(sql);
//	while (!db.HX_pRecordset->adoEOF)
//	{
//		_variant_t var = db.HX_pRecordset->GetCollect("name");
//		string strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
//		char comboChar[20];
//		strcpy(comboChar,strValue.c_str());
//		SendMessage(hcombo, CB_INSERTSTRING, 0, (LPARAM)comboChar);
//		db.HX_pRecordset->MoveNext();
//	}
//	db.ExitConnect();
//}