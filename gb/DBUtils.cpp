#include "DBUtils.h"

void DBUtils::OnInitADOConn() {
	::CoInitialize(NULL);
	try
	{ 
		HX_pConnection.CreateInstance(__uuidof(Connection));//等价于 //HX_pConnection.CreateInstance("ADODB.Connection");//此句包含定位你数据库的所需的访问信息  
		_bstr_t strConnect = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=Database1.accdb;Persist Security Info=False ";
		//测试//_bstr_t strConnect = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=Database1.accdb;Persist Security Info=False";//此句包含定位你数据库的所需的访问信息  
		//测试//_bstr_t strConnect = "Provider=Microsoft.ACE.OLEDB.12.0;Driver = { Microsoft Access Driver(*.mdb, *.accdb) }; DBQ =Database1.accdb;Persist Security Info=False ";  
		//此句包含定位你数据库的所需的访问信息  
		//连接master数据库，无密码。  
		HX_pConnection->Open(strConnect, "", "", adModeUnknown);//连接数据库 
																//cout << "连接成功,并获得智能指针" << endl;
	}
	catch (_com_error e)
	{
		//cout  << e.Description() << endl;
	}

}
void DBUtils::ExitConnect() {
	if (HX_pRecordset != NULL)
		HX_pRecordset->Close();
	HX_pConnection->Close();
	::CoUninitialize();
}