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
	int totalInt = (int)(totalMoney * 100 + 0.5);
	totalMoney = totalInt / 100;
	sprintf(totalPrice, "%f", totalMoney);
}
