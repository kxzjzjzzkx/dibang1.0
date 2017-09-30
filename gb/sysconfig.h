#pragma once

#include <string>
using namespace std;

// http get
string URL_LOGIN = "http://dibang.zz91.com/api/loginsave.html?username=<USERNAME>&pwd=<PASSWORD>&clientid=<CLIENTID>";
string URL_GET_SELLER = "http://dibang.zz91.com/api/searchsuppliers.html?iccode=<iccode>";
string URL_DATA_UPDATE_LOG = "http://dibang.zz91.com/api/datatable_update.html?maxid=<MAXID>&datatable=<DATATABLE>&company_id=<COMPANY_ID>";

// http post
string URL_DATA_GET_STORAGE_LASTTIME = "http://dibang.zz91.com/api/getstoragedate.html?company_id=<COMPANY_ID>";
string URL_DATA_UPLOAD_STORAGE_DOMAIN = "dibang.zz91.com";
string URL_DATA_UPLOAD_STORAGE_URL = "/api/updatestorage.html";							// 批量上传入库单
string URL_DATA_UPLOAD_STORAGE_PARAM = "company_id=<COMPANY_ID>&storelist=<DATA>";
string URL_DATA_GET_SUPPLIERS_LASTTIME = "http://dibang.zz91.com/api/getsuppliersdate.html?company_id=<COMPANY_ID>";
string URL_DATA_UPLOAD_SUPPLIERS_DOMAIN = "dibang.zz91.com";
string URL_DATA_UPLOAD_SUPPLIERS_URL = "/api/updatesuppliers.html";						// 批量上传本地供应商
string URL_DATA_UPLOAD_SUPPLIERS_PARAM = "company_id=<COMPANY_ID>&supplierlist=<DATA>";

// local properties
char LOGIN_USERNAME[50];				// 登录用户名
char LOGIN_SELFID[50];					// 登录的selfid信息
char LOGIN_GROUPID[50];
char LOGIN_COMPANYID[50];

// sql 语句统一堆放处
/*
select
*/
string SQL_USERS_SELECT_BY_CLIENTID		= "select top 1 id,selfid,username,group_id,company_id from users where clientid = '<CLIENTID>'";
string SQL_USERS_BY_UAP					= "select count(*) as hasExist from users where username='<USERNAME>' and pwd='<PWD>' ";

string SQL_V_SUPPLIERS_FOR_NOPAY	= "select * from v_suppliers where ispay = 0 order by id desc";
string SQL_V_SUPPLIERS_FOR_PAY		= "select * from v_suppliers where ispay > 0 order by id desc";

string SQL_SUPPLIERS_BY_ICCODE	= "select top 1 * from suppliers where iccode = '<ICCODE>' order by id desc";
string SQL_SUPPLIERS_FOR_TOP1	= "select top 1 id from suppliers order by id desc";
string SQL_SUPPLIERS_FOR_UPLOAD = "select * from suppliers where gmt_modified >#<GMT_MODIFIED># and company_id=<COMPANY_ID>";
string SQL_SUPPLIERS_BY_SELFID  = "select  count(0) as hasExist  from suppliers where selfid = '<SELFID>'";;

string SQL_STORAGE_FOR_UPLOAD = "select * from storage where gmt_modified >#<GMT_MODIFIED># and company_id=<COMPANY_ID>";

string SQL_V_STORAGELIST_BY_ICCODE = "select * from v_storagelist where iccode = '<ICCODE>' order by id desc";

string SQL_STORAGE_BY_ID			= "select * from storage where id = <ID>";
string SQL_STORAGE_BY_SELFID = "select * from storage where selfid = '<SELFID>'";
string SQL_STORAGE_FOR_LAST_PRICE	= "select * from storage where company_id = <COMPANY_ID> and products_selfid = '<PRODUCTS_SELFID>' and price > 0 order by id desc";

string SQL_UPDATE_LOG = "select * from update_log";

string SQL_PRODUCTS_SELECT_BY_SELFID = "select count(0) as hasExist from products where selfid = '<SELFID>'";

string SQL_CATEGORY_PRODUCTS_SELECT_BY_SELFID		= "select count(0) as hasExist from category_products where selfid = '<SELFID>'";
string SQL_CATEGORY_PRODUCTS_SELECT_BY_COMPANY_ID	= "select * from category_products where company_id = <COMPANY_ID> order by id desc";
string SQL_CATEGORY_PRODUCTS_SELECT_BY_NAME			= "select * from category_products where name = '<NAME>' order by id desc";

string SQL_PRODUCTS_SELECT_BY_COMPANY_ID = "select top 1 * from products where company_id = <COMPANY_ID> <CAULSE> order by id desc";

string SQL_EQSET_SELECT_ALL					= "select * from eqset";
string SQL_EQSET_SELECT_HASEXIST			= "select count(0) as hasExist from eqset where eq_type = '<EQ_TYPE>' ";

/*
insert
*/
string SQL_STORAGE_INSERT			= "INSERT INTO `storage` (`selfid`,`group_id`,`company_id`,`code`,`suppliers_selfid`,`gw`,`status`,`price_users_selfid`,`ispay`,`eqtype`) VALUES ('<SELFID>',<GROUP_ID>,<COMPANY_ID>,'<CODE>','<SUPPLIERS_SELFID>',<GW>,<STATUS>,'<PRICE_USERS_SELFID>',<ISPAY>,'<EQTYPE>')";
string SQL_SUPPLIERS_INSERT			= "INSERT INTO `suppliers`(`selfid`,`iccode`,`name`,`contact`,`company_id`) VALUES('<SELFID>','<ICCODE>','<NAME>','<CONTACT>','<COMPANY_ID>')";
string SQL_USERS_INSERT				= "INSERT INTO  users (selfid,group_id,company_id,clientid,username,pwd) VALUES ('<SELFID>',<GROUP_ID>,<COMPANY_ID>,'<CLIENTID>','<USERNAME>','<PWD>')";
string SQL_PRODUCTS_INSERT			= "INSERT INTO `products`(`selfid`,`group_id`,`company_id`,`name`,`name_py`,`category_selfid`,`spec`,`unit`,`stock`,`bz`)VALUES ('<SELFID>',<GROUP_ID>,<COMPANY_ID>,'<NAME>','<NAME_PY>','<CATEGORY_SELFID>','<SPEC>','<UNIT>',<STOCK>,'<BZ>')";
string SQL_CATEGORY_PRODUCTS_INSERT = "INSERT INTO `category_products`(`selfid`,`sub_selfid`,`company_id`,`name`) VALUES ('<SELFID>','<SUB_SELFID>',<COMPANY_ID>,'<NAME>')";
string SQL_EQSET_INSERT				= "INSERT INTO  eqset (eq_name,eq_type,eq_com,eq_btl,eq_sjw,eq_jyw,eq_tzw) values ('<EQ_NAME>','<EQ_TYPE>','<EQ_COM>','<EQ_BTL>','<EQ_SJW>','<EQ_JYW>','<EQ_TZW>')";
string SQL_UPDATE_LOG_INSERT		= "INSERT INTO  update_log (maxid,bz,tablename) values (<MAXID>,'<BZ>','<TABLENAME>')";

/*
update
*/
string SQL_STORAGE_UPDATE_TO_PRICE_OVER =			"update `storage` set gmt_modified = now(),status = 2,price_time = <PRICE_TIME>,price=<PRICE>,products_selfid = '<PRODUCTS_SELFID>',price_users_selfid = '<PRICE_USERS_SELFID>' where id = <ID>";
string SQL_STORAGE_UPDATE_TO_WEIGHT_OVER =			"update `storage` set gmt_modified = now(),status = 3,tare_check=1,nw=<NW>,total=<TOTAL>,tare=<TARE> where id = <ID>";
string SQL_STORAGE_UPDATE_TO_TRADE_OVER =			"update `storage` set gmt_modified = now(),status = 4,pay_time = now(),ispay=1 , pay_users_selfid = '<PAY_USERS_SELFID>' where id = <ID>";
string SQL_STORAGE_UPDATE_TO_TRADE_OVER_ONLINE =	"update `storage` set gmt_modified = now(),status = 4,pay_time = now(),ispay=2 , pay_users_selfid = '<PAY_USERS_SELFID>' where id = <ID>";
string SQL_STORAGE_UPDATE_TO_FAIL =					"update `storage` set gmt_modified = now(),status = 99 where id = <ID>";
string SQL_EQSET_UPDATE =							"update eqset set eq_com = '<EQ_COM>' , eq_btl = '<EQ_BTL>' where eq_type = '<EQ_TYPE>' ";
string SQL_UPDATE_LOG_MAXID_CHANGE =				"update update_log set maxid = <MAXID> where tablename = '<TABLENAME>'";
