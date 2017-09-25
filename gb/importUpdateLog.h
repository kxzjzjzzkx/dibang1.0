#pragma once
#include "sysconfig.h"

#include "DBUtils.cpp"
#include "HttpUtils.h"
#include <time.h>

#pragma comment(lib, "json1.lib")
#include "json/json.h"

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
		if (!hasExist._Equal("0")) {
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
		if (!hasExist._Equal("0")) {
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

/*
初始化系统数据
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
		if (datatable._Equal("products")) {
			insertForProducts(value);
		}
		if (datatable._Equal("category_products")) {
			insertForCategoryProducts(value);
		}
		int nSize = value["list"].size();
		if (nSize>0) {
			updateUpdateLogByTable(datatable);
		}
		db.HX_pRecordset->MoveNext();
	}

}