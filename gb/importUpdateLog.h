#pragma once
#include "stdafx.h"
#include "sysconfig.h"

#include "DBUtils.h"
#include "HttpUtils.h"
#include <time.h>

void insert(string sqlString) {
	DBUtils db;
	char sql[200];
	memset(sql, 0, 200);
	strcpy(sql, sqlString.c_str());
	db.addRecord(sql);
}

void updateUpdateLogByTable(string tablename) {
	string sqlString = SQL_UPDATE_LOG_MAXID_CHANGE;
	char sql[200];
	DBUtils db;
	char maxId[20];
	time_t tt = time(0);
	sprintf(maxId,"%d",tt);
	sqlString = sqlString.replace(sqlString.find("<MAXID>"), 7, maxId);
	sqlString = sqlString.replace(sqlString.find("<TABLENAME>"), 11, tablename);
	strcpy(sql, sqlString.c_str());
	db.addRecord(sql);
}

void insertForProducts(Json::Value value) {
	int nSize = value["list"].size();
	for (int i = 0; i < nSize; i++)
	{
		DBUtils db;
		string sqlString = SQL_PRODUCTS_SELECT_BY_SELFID;
		string selfid = value["list"][i]["selfid"].asString();	
		sqlString = sqlString.replace(sqlString.find("<SELFID>"), 8, selfid);
		char selectSql[200];
		strcpy(selectSql, sqlString.c_str());
		db.getRecordRet(selectSql);
		string hasExist;
		while(!db.HX_pRecordset->adoEOF){
			_variant_t var = db.HX_pRecordset->GetCollect("hasExist");
			hasExist = _com_util::ConvertBSTRToString((_bstr_t)var);
			db.HX_pRecordset->MoveNext();
		}
		if (hasExist!="0") {
			continue;
		}
		sqlString = SQL_PRODUCTS_INSERT;
		sqlString = sqlString.replace(sqlString.find("<SELFID>"), 8, selfid);

		string name = value["list"][i]["name"].asString();
		sqlString = sqlString.replace(sqlString.find("<NAME>"), 6, name);

		int companyId = value["list"][i]["company_id"].asInt();
		char companyIdChar[20];
		sprintf(companyIdChar,"%d" , companyId);
		sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"), 12, companyIdChar);

		string spec = value["list"][i]["spec"].asString();
		sqlString = sqlString.replace(sqlString.find("<SPEC>"), 6, spec);

		string categorySelfid = value["list"][i]["category_selfid"].asString();
		sqlString = sqlString.replace(sqlString.find("<CATEGORY_SELFID>"), 17, categorySelfid);

		string namePy = value["list"][i]["name_py"].asString();
		sqlString = sqlString.replace(sqlString.find("<NAME_PY>"), 9, namePy);

		string bz = value["list"][i]["bz"].asString();
		sqlString = sqlString.replace(sqlString.find("<BZ>"), 4, bz);

		int groupId = value["list"][i]["group_id"].asInt();
		char groupIdChar[20];
		sprintf(groupIdChar, "%d", groupId);
		sqlString = sqlString.replace(sqlString.find("<GROUP_ID>"), 10, groupIdChar);

		string unit = value["list"][i]["unit"].asString();
		sqlString = sqlString.replace(sqlString.find("<UNIT>"), 6, unit);

		int stock = value["list"][i]["stock"].asInt();
		char stockChar[20];
		sprintf(stockChar,"%d",stock);
		sqlString = sqlString.replace(sqlString.find("<STOCK>"), 7, stockChar);

		insert(sqlString);
	}
}

void insertForCategoryProducts(Json::Value value) {
	int nSize = value["list"].size();
	for (int i = 0; i < nSize; i++)
	{
		DBUtils db;
		//(<SELFID>,<SUB_SELFID>,<COMPANY_ID>,<NAME>)";
		string sqlString = SQL_CATEGORY_PRODUCTS_SELECT_BY_SELFID;
		string selfid = value["list"][i]["selfid"].asString();
		sqlString = sqlString.replace(sqlString.find("<SELFID>"), 8, selfid);
		char selectSql[200];
		strcpy(selectSql, sqlString.c_str());
		db.getRecordRet(selectSql);
		string hasExist;
		while (!db.HX_pRecordset->adoEOF) {
			_variant_t var = db.HX_pRecordset->GetCollect("hasExist");
			hasExist = _com_util::ConvertBSTRToString((_bstr_t)var);
			db.HX_pRecordset->MoveNext();
		}
		if (hasExist!="0") {
			continue;
		}
		sqlString = SQL_CATEGORY_PRODUCTS_INSERT;

		sqlString = sqlString.replace(sqlString.find("<SELFID>"), 8, selfid);

		string name = value["list"][i]["name"].asString();
		sqlString = sqlString.replace(sqlString.find("<NAME>"), 6, name);

		int companyId = value["list"][i]["company_id"].asInt();
		char companyIdChar[20];
		sprintf(companyIdChar, "%d", companyId);
		sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"), 12, companyIdChar);

		string subSelfid = value["list"][i]["sub_selfid"].asString();
		sqlString = sqlString.replace(sqlString.find("<SUB_SELFID>"), 12, subSelfid);

		insert(sqlString);
	}
}

void insertForSuppliers(Json::Value value) {
	int nSize = value["list"].size();
	for (int i = 0; i < nSize; i++)
	{
		DBUtils db;
		//(<SELFID>,<SUB_SELFID>,<COMPANY_ID>,<NAME>)";
		string sqlString = SQL_SUPPLIERS_BY_SELFID;
		string selfid = value["list"][i]["selfid"].asString();
		sqlString = sqlString.replace(sqlString.find("<SELFID>"), 8, selfid);
		char selectSql[200];
		strcpy(selectSql, sqlString.c_str());
		db.getRecordRet(selectSql);
		string hasExist;
		while (!db.HX_pRecordset->adoEOF) {
			_variant_t var = db.HX_pRecordset->GetCollect("hasExist");
			hasExist = _com_util::ConvertBSTRToString((_bstr_t)var);
			db.HX_pRecordset->MoveNext();
		}
		if (hasExist != "0") {
			continue;
		}
		//string SQL_SUPPLIERS_INSERT = "INSERT INTO `suppliers`(`selfid`,`iccode`,`name`,`contact`,`company_id`) VALUES('<SELFID>','<ICCODE>','<NAME>','<CONTACT>','<COMPANY_ID>')";
		sqlString = SQL_SUPPLIERS_INSERT;
		sqlString = sqlString.replace(sqlString.find("<SELFID>"), 8, selfid);
		string iccode = value["list"][i]["iccode"].asString();
		sqlString = sqlString.replace(sqlString.find("<ICCODE>"), 8, iccode);
		string name = value["list"][i]["name"].asString();
		sqlString = sqlString.replace(sqlString.find("<NAME>"), 6, name);
		string contact = value["list"][i]["contact"].asString();
		sqlString = sqlString.replace(sqlString.find("<CONTACT>"), 9, contact);
		int companyId = value["list"][i]["company_id"].asInt();
		char companyIdChar[20];
		sprintf(companyIdChar, "%d", companyId);
		sqlString = sqlString.replace(sqlString.find("<COMPANY_ID>"), 12, companyIdChar);

		insert(sqlString);
	}
}

/*
初始化系统数据 
下载products名表和category_products表
*/
void systemDataInit() {
	string sqlString = SQL_UPDATE_LOG;
	char sql[200];
	strcpy(sql, sqlString.c_str());
	DBUtils db;
	HttpUtils hu;
	char url[200];
	db.getRecordRet(sql);
	int count = 0;
	while (!db.HX_pRecordset->adoEOF)
	{
		//数据库数据获取
		_variant_t var = db.HX_pRecordset->GetCollect("tablename");
		string datatable = _com_util::ConvertBSTRToString((_bstr_t)var);
		var = db.HX_pRecordset->GetCollect("maxid");
		string maxid = _com_util::ConvertBSTRToString((_bstr_t)var);

		// 拼装url
		string urlString = URL_DATA_UPDATE_LOG;
		urlString = urlString.replace(urlString.find("<MAXID>"), 7, maxid);
		urlString = urlString.replace(urlString.find("<DATATABLE>"), 11, datatable);
		urlString = urlString.replace(urlString.find("<COMPANY_ID>"), 12, LOGIN_COMPANYID);
		strcpy(url, urlString.c_str());
		string httpResult = hu.httpGet(_T(url));
		Json::Reader reader;
		Json::Value value;
		//可以代表任意类型
		if (!reader.parse(httpResult, value))
		{
			db.HX_pRecordset->MoveNext();
			continue;
		}
		if (datatable=="products") {
			insertForProducts(value);
		}
		if (datatable=="category_products") {
			insertForCategoryProducts(value);
		}
		if (datatable == "suppliers") {
			insertForSuppliers(value);
		}
		int nSize = value["list"].size();
		if (nSize>0) {
			updateUpdateLogByTable(datatable);
		}
		db.HX_pRecordset->MoveNext();
	}

}

string bulidUploadColumn(char* column, DBUtils db, string result) {
	_variant_t var = db.HX_pRecordset->GetCollect(column);
	if (var.vt != VT_NULL) {
		result = result + "|" + _com_util::ConvertBSTRToString((_bstr_t)var);
	}
	else {
		result = result + "|";
	}
	return result;
}

string bulidUploadStorageData(DBUtils db) {
	string uploadData;
	while (!db.HX_pRecordset->adoEOF)
	{
		string strValue;
		// selfid group_id company_id code products_selfid suppliers_selfid
		_variant_t var = db.HX_pRecordset->GetCollect("selfid");
		if (var.vt != VT_NULL) {
			strValue = _com_util::ConvertBSTRToString((_bstr_t)var);
		}
		else {
			strValue = "0";
		}
		if (uploadData == "" || (*(uploadData.end() - 1) == '$')) {
			uploadData = uploadData + strValue;
		}
		else {
			uploadData = uploadData + "|" + strValue;
		}
		uploadData = bulidUploadColumn("group_id", db, uploadData);
		uploadData = bulidUploadColumn("company_id", db, uploadData);
		uploadData = bulidUploadColumn("code", db, uploadData);
		uploadData = bulidUploadColumn("products_selfid", db, uploadData);
		uploadData = bulidUploadColumn("suppliers_selfid", db, uploadData);
		uploadData = bulidUploadColumn("price", db, uploadData);
		uploadData = bulidUploadColumn("gw", db, uploadData);
		uploadData = bulidUploadColumn("nw", db, uploadData);
		uploadData = bulidUploadColumn("tare", db, uploadData);
		uploadData = bulidUploadColumn("tare_check", db, uploadData);
		uploadData = bulidUploadColumn("total", db, uploadData);
		uploadData = bulidUploadColumn("status", db, uploadData);
		uploadData = bulidUploadColumn("price_users_selfid", db, uploadData);
		uploadData = bulidUploadColumn("price_time", db, uploadData);
		uploadData = bulidUploadColumn("out_time", db, uploadData);
		uploadData = bulidUploadColumn("ispay", db, uploadData);
		uploadData = bulidUploadColumn("scorecheck", db, uploadData);
		uploadData = bulidUploadColumn("pay_time", db, uploadData);
		uploadData = bulidUploadColumn("pay_users_selfid", db, uploadData);
		uploadData = bulidUploadColumn("gmt_created", db, uploadData);
		uploadData = bulidUploadColumn("gmt_modified", db, uploadData);
		uploadData = uploadData + "$";
		db.HX_pRecordset->MoveNext();
	}
	return uploadData;
}