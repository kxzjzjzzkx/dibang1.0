#include "stdafx.h"

#import "c:\Program Files\Common Files\System\ado\msado15.dll"  no_namespace rename("EOF","adoEOF")


class DBUtils
{
public:
	_ConnectionPtr HX_pConnection; // 数据库指针  
	_RecordsetPtr  HX_pRecordset; // 命令指针  
								  //_ConnectionPtr m_pConnection;
								  //_RecordsetPtr m_pRecordset;
	_CommandPtr HX_pCommand; // 记录指针  

	BSTR _ErrorMessage;

	void DBUtils::OnInitADOConn() {
		::CoInitialize(NULL);
		try
		{
			HX_pConnection.CreateInstance(__uuidof(Connection));
			_bstr_t strConnect = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=dibang.mdb;Persist Security Info=False ";
			HX_pConnection->Open(strConnect, "", "", adModeUnknown);
		}
		catch (_com_error e)
		{
			_ErrorMessage = e.Description();
			//		cout  << e.Description() << endl;
		}

	}
	void DBUtils::ExitConnect() {
		if (HX_pRecordset != NULL) {
			HX_pRecordset->Close();
		}
		HX_pConnection->Close();
		::CoUninitialize();
	}

	_RecordsetPtr getRecordRet(char* sql) {
		CoInitialize(NULL);         //初始化OLE/COM库环境
									//在引入ado并初始化成功com库后，就可以使用智能指针了
									//_ConnectionPtr m_pConnection;
									//_RecordsetPtr m_pRecordset;
		HRESULT hr;

		try
		{
			hr = HX_pConnection.CreateInstance("ADODB.Connection");///创建Connection对象
																   //     hr = m_pConnection.CreateInstance(__uuidof(Connection));//no_namespace的好处
			if (SUCCEEDED(hr))
			{
				hr = HX_pConnection->Open("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=dibang.mdb", "", "", adModeUnknown);          //access2003
																																	  //			hr = m_pConnection->Open("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=test.accdb","","",adModeUnknown);    //access2007
				printf("成功连接数据库\n");
			}
		}
		catch (_com_error e)///捕捉异常
		{
			MessageBox(NULL, TEXT(e.Description()), TEXT("消息"), 0);
		}
		HX_pRecordset.CreateInstance(__uuidof(Recordset)); //实例化结果集对象   
														   //执行sql语句   
		try
		{
			HX_pRecordset->Open(sql, HX_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		}
		catch (_com_error* e)
		{
			if (HX_pConnection->State)
			{
				HX_pConnection->Close();
				HX_pRecordset = NULL;
			}
			CoUninitialize();
		}
		return HX_pRecordset;
	}

	void addRecord(char* sql) {
		_variant_t RecordsAffected;
		CoInitialize(NULL);         //初始化OLE/COM库环境
		HRESULT hr;
		try
		{
			hr = HX_pConnection.CreateInstance("ADODB.Connection");///创建Connection对象
			if (SUCCEEDED(hr))
			{
				hr = HX_pConnection->Open("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=dibang.mdb", "", "", adModeUnknown);          //access2003
			}
		}
		catch (_com_error e)///捕捉异常
		{
		}
		HX_pRecordset.CreateInstance(__uuidof(Recordset)); //实例化结果集对象   
		try
		{
			HX_pConnection->Execute(sql, &RecordsAffected, adCmdText);
		}
		catch (_com_error* e)
		{
		}
		HX_pConnection->Close();
		::CoUninitialize();
	}
};