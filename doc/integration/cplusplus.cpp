#include "stdafx.h"
#include <iostream>
#include <string>
#include "windows.h"
#include <shellapi.h>
using namespace std;

int main()
{	
	string uri = "ph://ph.ini";
	cout << "Opening: \"" << uri << "\"\n";
	ShellExecuteA(NULL, "open", uri.c_str(), NULL, NULL, SW_SHOWNORMAL);
    return 0;
}

