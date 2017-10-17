#pragma once
#include "stdafx.h"

/*
	获取总价以及计算净重

	gw			毛重
	tare		皮重
	nw			净重
	price		定价(单价)
	totalPrice	总价(单价 乘 净重)
*/
void getTotalPrice(char* gw, char* tare,char* nw,char* price,char* totalPrice)
{
	double total = atof(gw);
	double lost = atof(tare);
	double last = total - lost;
	sprintf(nw, "%f", last);
	double priceMoney = atof(price);
	double totalMoney = last * priceMoney;
	totalMoney = round(totalMoney * 100) / 100;
	sprintf(totalPrice, "%f", totalMoney);
}
//
///*
//	初始化 com端口数据的读取准备
//
//*/
//void initPort(CSerialPort mySerialPort, CSerialPort mySerialPort2) {
//	//
//	/*初始化线程实时同步磅秤信息*/
//	string sqlString = SQL_EQSET_SELECT_ALL;
//	char sql[200];
//	DBUtils db;
//	strcpy(sql, sqlString.c_str());
//	db.getRecordRet(sql);
//	while (!db.HX_pRecordset->adoEOF)
//	{
//		_variant_t var = db.HX_pRecordset->GetCollect("eq_type");
//		if (var.vt == VT_NULL) {
//			continue;
//		}
//		string strType = _com_util::ConvertBSTRToString((_bstr_t)var);
//		var = db.HX_pRecordset->GetCollect("eq_com");
//		string strCom = "";
//		if (var.vt != VT_NULL) {
//			strCom = _com_util::ConvertBSTRToString((_bstr_t)var);
//		}
//		var = db.HX_pRecordset->GetCollect("eq_btl");
//		string strBtl = "";
//		if (var.vt != VT_NULL) {
//			strBtl = _com_util::ConvertBSTRToString((_bstr_t)var);
//		}
//		char message[200];
//		if (strType == "1") {
//			strCom = strCom.replace(strCom.find("COM"), 3, "");
//			int selcom = atoi(strCom.c_str());
//			int btl = atoi(strBtl.c_str());
//			if (!mySerialPort.InitPort(selcom, btl, 'N', 8, 1, NULL) || selcom < 1 || btl< 1)
//			{
//				/*sprintf(message, "磅1(com%d口)异常，请检查后重试！", selcom);
//				MessageBoxA(NULL, message, "消息", 0);*/
//			}
//			else
//			{
//				mySerialPort.isClose = false;
//				/*sprintf(message, "磅1(com%d口)启动正常！", selcom);
//				MessageBoxA(NULL, message, "消息", 0);*/
//			}
//		}
//		else if (strType == "2") {
//			strCom = strCom.replace(strCom.find("COM"), 3, "");
//			int selcom = atoi(strCom.c_str());
//			int btl = atoi(strBtl.c_str());
//			if (!mySerialPort2.InitPort(selcom, btl, 'N', 8, 1, EV_RXCHAR) || selcom < 1 || btl< 1)
//			{
//				/*sprintf(message, "磅2(com%d口)异常，请检查后重试！", selcom);
//				MessageBox(NULL, message, TEXT("消息"), 0);*/
//			}
//			else
//			{
//				mySerialPort2.isClose = false;
//				/*sprintf(message, "磅2(com%d口)启动正常！", selcom);
//				MessageBox(NULL, message, TEXT("消息"), 0);*/
//			}
//		}
//		db.HX_pRecordset->MoveNext();
//	}
//	db.ExitConnect();
//	//
//}
//
