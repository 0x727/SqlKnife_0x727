#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <typeinfo>
#include <comdef.h>
#include <fstream>

enum ExecMethod
{
	NONE,
	XPCMD,
	OACREATE,
	CLRCMD,
	CLRDEXEC,
	DBUP,
	DBUP2
};
