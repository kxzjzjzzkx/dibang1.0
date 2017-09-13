#pragma once

#include <string>

class Suppliers {
public:
	int getId() {
		return id;
	}
	void setId(int _id) {
		id = _id;
	}
	char* getIccode() {
		return iccode;
	}
	
	void setIccode(char* _iccode) {
		memset(iccode, 0, strlen(iccode));
		strcpy(iccode,_iccode);
	}

	char* getName() {
		return name;
	}
	void setName(char* _name) {
		memset(name, 0, strlen(name));
		strcpy(name, _name);
	}

	char* getContact() {
		return contact;
	}
	void setContact(char* _contact) {
		memset(contact, 0, strlen(contact));
		strcpy(contact, _contact);
	}
	char* getMobile() {
		return mobile;
	}
	void setMobile(char* _mobile) {
		memset(mobile, 0, strlen(mobile));
		strcpy(mobile, _mobile);
	}
	int getCtype() {
		return ctype;
	}
	void setCtype(int _ctype) {
		ctype = _ctype;
	}
	char* getAddress() {
		return address;
	}
	void setAddress(char* _address) {
		memset(address,0,strlen(address));
		strcpy(address, _address);
	}
	int getIspay() {
		return ispay;
	}
	void setIspay(int _ispay) {
		ispay = _ispay;
	}
	char* getSelfid(char* selfid) {
		return selfid;
	}
	void setSelfid(char* _selfid) {
		memset(selfid,0,strlen(selfid));
	}
private:
	int id;
	char iccode[500];
	char name[50];
	char contact[50];
	char mobile[50];
	int ctype;
	char address[50];
	int ispay;
	char selfid[50];
};