#include "stdafx.h"

#include <stdio.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <list>

using namespace std;

class DBInfo{

	public:
		/*int getId(void);
		void setId(int _id);
		string getGmtCreated(void);
		void setGmtCreated(string _gmtCreated);
		int getCId();
		void setCId(int _cId);
		string getInNo();
		void setInNo(string _inNo);
		string getRecoverPerson();
		void setRecoverPerson(string _recoverPerson);
		string getRecoverGood();
		void setRecoverGood(string _recoverGood);
		float getQuantityTotal();
		void setQuantityTotal(float _quantityTotal);
		float getQuantityCar();
		void setQuantityCar(float _quantityCar);
		float getQuantityGood();
		void setQuantityGood(float _quantityGood);
		string getUnitPrice();
		void setUnitPrice(string _unitPrice);
		string getUnitQuantity();
		void setUnitQuantity(string _unitQuantity);
		float getPriceUnit();
		void setPriceUnit(float _priceUnit);
		float getPriceTotal();
		void setPriceTotal(float _priceTotal);
		string getPicAddr();
		void setPicAddr(string _picAddr);
		DBInfo(void);*/
		int DBInfo::getId(void){
			return id;
		}
		void DBInfo::setId(int _id){
			id = _id;
		}
		string DBInfo::getGmtCreated(void){
			return gmtCreated;
		}
		void DBInfo::setGmtCreated(string _gmtCreated){
			gmtCreated = _gmtCreated;
		}
		int DBInfo::getCId(){
			return cId;
		}
		void DBInfo::setCId(int _cId){
			cId = _cId;
		}
		string DBInfo::getInNo(){
			return inNo;
		}
		void DBInfo::setInNo(string _inNo){
			inNo = _inNo;
		}
		string DBInfo::getRecoverPerson(){
			return recoverPerson;
		}
		void DBInfo::setRecoverPerson(string _recoverPerson){
			recoverPerson = _recoverPerson;
		}
		string DBInfo::getRecoverGood(){
			return recoverGood;
		}
		void DBInfo::setRecoverGood(string _recoverGood){
			recoverGood = _recoverGood;
		}

		float DBInfo::getQuantityTotal(){
			return quantityTotal;
		}
		void DBInfo::setQuantityTotal(float _quantityTotal){
			quantityTotal = _quantityTotal;
		}

		float DBInfo::getQuantityCar(){
			return quantityCar;
		}

		void DBInfo::setQuantityCar(float _quantityCar){
			quantityCar = _quantityCar;
		}

		float DBInfo::getQuantityGood(){
			return quantityGood;
		}
		void DBInfo::setQuantityGood(float _quantityGood){
			quantityGood = _quantityGood;
		}

		string DBInfo::getUnitPrice(){
			return unitPrice;
		}
		void DBInfo::setUnitPrice(string _unitPrice){
			unitPrice = _unitPrice;
		}

		string DBInfo::getUnitQuantity(){
			return unitQuantity;
		}
		void DBInfo::setUnitQuantity(string _unitQuantity){
			unitQuantity = _unitQuantity;
		}

		float DBInfo::getPriceUnit(){
			return priceUnit;
		}
		void DBInfo::setPriceUnit(float _priceUnit){
			priceUnit = _priceUnit;
		}

		float DBInfo::getPriceTotal(){
			return priceTotal;
		}
		void DBInfo::setPriceTotal(float _priceTotal){
			priceTotal = _priceTotal;
		}

		string DBInfo::getPicAddr(){
			return picAddr;
		}
		void DBInfo::setPicAddr(string _picAddr){
			picAddr = _picAddr;
		}
		DBInfo(void){
		
		}
		~DBInfo(void){
		
		}


	private:
		int id;
		string gmtCreated;		//时间
		int cId;				// zz91公司id
		string inNo;			//"入库单号";
		string recoverPerson;	//"回收员";
		string recoverGood;     //"回收产品";
		float quantityTotal;	//"载货地磅重量";
		float quantityCar;     //"车辆地磅重量";
		float quantityGood;    //"货物重量";
		string unitPrice;		// 价格单位
		string unitQuantity;	// 质量单位	
		float priceUnit;		//"单价";
		float priceTotal;		//"总金额";
		string picAddr;			//"现场照片";
};