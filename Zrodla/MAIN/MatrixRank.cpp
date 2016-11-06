// RzadMacierzy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <process.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <ctime> 
#include <chrono>

#if defined(UNICODE)
#define _tcout std::wcout
#else
#define _tcout std::cout
#endif

#define DLL_TYPE argv[1]
#define DEBUG_MODE _ttoi(argv[3])
#define THREAD_COUNT _ttoi(argv[2])

typedef struct {
	int firstNum;
	int secondNum;
}Testing;

int count = 0;

typedef int(*MYPROC)(int, int);
MYPROC dllFunction;

void __cdecl ThreadProc(void * Args) {
	Testing * args = (Testing *) Args;

	int sum = (dllFunction)(args->firstNum, args->secondNum);

	printf("Thread %i sum equals %i\n", args->firstNum, sum); 

	count++;

	free(args);

	_endthread();
}

//for now program takes two parameters, first is the name of Dll to use, second is number of threads, third is toggle of debug mode.
//temp function of dlls is to take two numbers  and add them together. For now those two numbers are number of thread and double number of thread.
//to run the program, run the rzadcpp.bat inside RzadMacierzy/x64 folder, as program will exit automatically if run by debugger without breakpoint in the end

int _tmain(int argc, _TCHAR* argv[])
{
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();


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

		dllFunction = (MYPROC)GetProcAddress(hDll, "adder");

		if (dllFunction != NULL) {
			_tcout << std::endl << "Preparing for " << THREAD_COUNT << " threads." << std::endl;

			std::vector < HANDLE > threads;

			for (int i = 0; i < THREAD_COUNT; i++) {
				Testing * args;
				args = (Testing *)malloc(sizeof(Testing));

				args->firstNum = i+1;
				args->secondNum = 2*(i+1);

				HANDLE hThread = (HANDLE)_beginthread(ThreadProc, 0, (void*)args);
				threads.push_back(hThread);
			}

			if (threads.size() > 0) {
				WaitForMultipleObjects(threads.size(), &threads[0], TRUE, 10000);
			}
				
			//WaitForSingleObject(hThread, INFINITE);
		}
		else {
			_tcout << std::endl << _T("Function failed to load. Exiting now.") << std::endl;
		}

	}
	else {
		_tcout << _T("Dll failed to load. Exiting now.") << std::endl;
	}

	end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;

	_tcout << "Thread count: " << count << ", elapsed time: " << elapsed_seconds.count() << "s\n";

	return 0;
}

