#pragma once
#include<time.h>

class Users {
public :
	Users(int id,int groupId,char clientid[50],char utype[20],char username[20],char pwd[16],char contact[30],char sex[10],char mobile[30],char bz[1000],tm gmtCreated,tm gmtModified);
	Users();
	~Users();

	void setId(int _id);
	int getId();

	void setGroupId(int _groupId);
	int getGroupId();

	void setUtype(char* _utype);
	char* getUtype();

	void setUsername(char* _username);
	char* getUsername();

	void setPwd(char* _pwd);
	char* getPwd();

	void setUtype(char* _utype);
	char* getUtype();

	void setUtype(char* _utype);
	char* getUtype();

	void setUtype(char* _utype);
	char* getUtype();

	void setUtype(char* _utype);
	char* getUtype();

	void setUtype(char* _utype);
	char* getUtype();

	void setGmtCreated(tm _gmtCreated);
	tm getGmtCreated();

	void setGmtModified(tm _gmtCreated);
	tm getGmtModified();


private:
	int id;
	int groupId;
	char clientid[50];
	char utype[20];
	char username[20];
	char pwd[16];
	char contact[30];
	char sex[10];
	char mobile[30];
	char bz[1000];
	tm gmtCreated;
	tm gmtModified;

};