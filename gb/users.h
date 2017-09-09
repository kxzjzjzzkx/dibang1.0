#pragma once
#include<time.h>
#include<string.h>

class Users {
public :
	Users(int _id,char* _selfid,int _groupId,int _companyId,char* _clientid,char* _utype,char* _username,char* _pwd,char* _contact,char* _sex,char* _mobile,char* _bz, time_t gmtCreated,time_t gmtModified);
	Users() {
	}
	~Users() {
	}

	void setId(int _id);
	int getId();

	void setSelfid(char* _selfid);
	char* getSelfid();

	void setGroupId(int _groupId);
	int getGroupId();

	void setCompanyId(int _companyId);
	int getCompanyId();

	void setClientid(char* _clientid);
	char* getClientid();

	void setUtype(char* _utype);
	char* getUtype();

	void setUsername(char* _username);
	char* getUsername();

	void setPwd(char* _pwd);
	char* getPwd();

	void setContact(char* _contact);
	char* getContact();

	void setSex(char* _sex);
	char* getSex();

	void setmobile(char* _mobile);
	char* getmobile();

	void setBz(char* _bz);
	char* getBz();

	void setGmtCreated(time_t _gmtCreated);
	time_t getGmtCreated();

	void setGmtModified(time_t _gmtCreated);
	time_t getGmtModified();


private:
	int id;
	char selfid[16];
	int groupId;
	int companyId;
	char clientid[16];
	char utype[20];
	char username[20];
	char pwd[16];
	char contact[30];
	char sex[10];
	char mobile[30];
	char bz[1000];
	time_t gmtCreated;
	time_t gmtModified;
};

Users::Users(int _id, char* _selfid, int _groupId, int _companyId, char* _clientid, char* _utype, char* _username, char* _pwd, char* _contact, char* _sex, char* _mobile, char* _bz, time_t _gmtCreated, time_t _gmtModified){
	id = _id;
	memset(selfid, 0, strlen(selfid));
	strcpy(selfid, _selfid);
	groupId = _groupId;
	companyId = _companyId;
	memset(clientid, 0, strlen(clientid));
	strcpy(clientid, _clientid);
	memset(utype, 0, strlen(utype));
	strcpy(utype, _utype);
	memset(username, 0, strlen(username));
	strcpy(username, _username);
	memset(pwd, 0, strlen(pwd));
	strcpy(pwd, _pwd);
	memset(contact, 0, strlen(contact));
	strcpy(contact, _contact);
	memset(sex, 0, strlen(sex));
	strcpy(sex, _sex);
	memset(mobile, 0, strlen(mobile));
	strcpy(mobile, _mobile);
	memset(bz, 0, strlen(bz));
	strcpy(bz, _bz);
	gmtCreated = _gmtCreated;
	gmtModified = _gmtModified;
}

void Users::setId(int _id) {
	id = _id;
}
int Users::getId() {
	return id;
}

void Users::setGroupId(int _groupId) {
	groupId = _groupId;
}
int Users::getGroupId() {
	return groupId;
}

void Users::setSelfid(char* _selfid) {
	memset(selfid, 0, strlen(selfid));
	strcpy(selfid, _selfid);
}
char* Users::getSelfid() {
	return selfid;
}

void Users::setCompanyId(int _companyId) {
	companyId = _companyId;
}
int Users::getCompanyId() {
	return companyId;
}

void Users::setClientid(char* _clientid) {
	memset(clientid, 0, strlen(clientid));
	strcpy(clientid, _clientid);
}
char* Users::getClientid() {
	return clientid;
}

void Users::setUtype(char* _utype){
	memset(utype, 0, strlen(utype));
	strcpy(utype, _utype);
}
char* Users::getUtype() {
	return utype;
}

void Users::setUsername(char* _username) {
	memset(username, 0, strlen(username));
	strcpy(username, _username);
}
char* Users::getUsername() {
	return username;
}

void Users::setPwd(char* _pwd) {
	memset(pwd, 0, strlen(pwd));
	strcpy(pwd, _pwd);
}
char* Users::getPwd() {
	return pwd;
}

void Users::setContact(char* _contact) {
	memset(contact, 0, strlen(contact));
	strcpy(contact, _contact);
}
char* Users::getContact() {
	return contact;
}

void Users::setSex(char* _sex) {
	memset(sex, 0, strlen(sex));
	strcpy(sex, _sex);
}
char* Users::getSex() {
	return sex;
}

void Users::setmobile(char* _mobile) {
	memset(mobile, 0, strlen(mobile));
	strcpy(mobile, _mobile);
}
char* Users::getmobile() {
	return mobile;
}

void Users::setBz(char* _bz) {
	memset(bz, 0, strlen(bz));
	strcpy(bz, _bz);
}
char* Users::getBz() {
	return bz;
}


void Users::setGmtCreated(time_t _gmtCreated) {
	gmtCreated = _gmtCreated;
}
time_t Users::getGmtCreated() {
	return gmtCreated;
}

void Users::setGmtModified(time_t _gmtModified) {
	gmtModified = _gmtModified;
}
time_t Users::getGmtModified() {
	return gmtModified;
}