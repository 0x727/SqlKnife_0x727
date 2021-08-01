#pragma once

#import "c:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF") rename("BOF","adoBOF")
#include "stdfax.h"

using namespace std;

class DataSourceHandle
{

public:
	//static DataSourceHandle* getInstance();
	bool basequery(string sql);
	void builddsn(string host, string port, string dbname);
	bool connect(string username,string password);
	//void printresult();
	void justquery(string sql);
	DataSourceHandle();
	~DataSourceHandle();

private:
	
	
	string connstr;
	string conndsn;
	static DataSourceHandle* m_DataSourceHandle;
	_ConnectionPtr m_pConn;
	_CommandPtr m_pCommand;
	_RecordsetPtr m_pRecordset;
	

};

