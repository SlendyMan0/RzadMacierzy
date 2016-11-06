// RzadMacierzy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <cstdlib>
#include <iostream>

#if defined(UNICODE)
#define _tcout std::wcout
#else
#define _tcout std::cout
#endif

#define DLL_TYPE argv[1]
#define DEBUG_MODE argv[2]

//for now program takes two parameters, first is the name of Dll to use, second is toggle of debug mode.
//temp function of dlls is to take two numbers inputed by user in command line and add them together.
//to run the program, run the rzadcpp.bat inside RzadMacierzy/x64 folder, as program will exit automatically if run by debugger without breakpoint in the end

int _tmain(int argc, _TCHAR* argv[])
{
	
	if (DEBUG_MODE) {
		_tcout << _T("Debug mode activated.") << std::endl << std::endl;
		_tcout << _T("There are ") << argc << _T(" arguments:") << std::endl << std::endl;

		for (int i = 0; i < argc; i++) {
			_tcout << i << _T(" ") << argv[i] << std::endl;
		}

		wchar_t currentPath[MAX_PATH] = {};
		GetCurrentDirectory(MAX_PATH, currentPath);
		_tcout << std::endl << _T("Current directory: ") << currentPath << std::endl;		
	}

	HINSTANCE hDll;
	hDll = LoadLibrary(DLL_TYPE);

	DWORD lastError = GetLastError();

	if (hDll != NULL) {
		if (DEBUG_MODE) {
			_tcout << std::endl << _T("Dll \"") << DLL_TYPE << _T("\" successfully loaded.") << std::endl;
		}

		typedef int(*MYPROC)(int, int);
		MYPROC dllFunction;

		dllFunction = (MYPROC)GetProcAddress(hDll, "adder");

		if (dllFunction != NULL) {
			int firstNum, secondNum;

			_tcout << std::endl << _T("Input first number: ");
			std::cin >> firstNum;
			_tcout  << _T("Input second number: ");
			std::cin >> secondNum;

			int sum = (dllFunction)(firstNum, secondNum);

			_tcout << std::endl << _T("Sum equals: ") << sum << std::endl;
		}
		else {
			_tcout << std::endl << _T("Function failed to load. Exiting now.") << std::endl;
		}

	}
	else {
		_tcout << _T("Dll failed to load. Exiting now.") << std::endl;
	}

	return 0;
}

