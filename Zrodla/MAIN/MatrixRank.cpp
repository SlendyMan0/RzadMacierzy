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
#include <fstream>
#include <string>

#if defined(UNICODE)
#define _tcout std::wcout
#else
#define _tcout std::cout
#endif

#define DLL_TYPE argv[1]
#define THREAD_COUNT _ttoi(argv[2])
#define DEBUG_MODE _ttoi(argv[3])

typedef struct {
	int * matrix;
	int deleteTemplateColumn, deleteTemplateRow;
	int rank;
}Testing;

typedef int(*MYPROC)(int *, int, int);
MYPROC dllFunction;

void __cdecl ThreadProc(void * Args) {
	Testing * args = (Testing *) Args;

	int det = (dllFunction)(args->matrix, args->deleteTemplateColumn, args->deleteTemplateRow);

	printf("TEMP: det = %i\n", det);



	free(args);

	_endthread();
}

//for now program takes three parameters, first is the name of Dll to use, second is number of threads, third is toggle of debug mode.
//for now the C++ function only returns matrix determinant (and due to some yet undisovered bug, only 2x2 matrix and below)
//program can be run either by normal debug or by batch file inside x64/Debug folder.
//TODO:	- look more into threads, as testing showed that they not always work, which is strange. 
//		- overhaul command arguments, define forces to provide value for DEBUG_MODE or debug cries foul.
//		- check the algorthm for calculating determinant to find error providing false answers to matrixes higher than 2x2
//		- clean up the code a bit, it's a mess
//		- learn how to access arrays in ASM :)

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 4) {
		return -1;
	}

	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	if (DEBUG_MODE == 1) {
		_tcout << _T("Debug mode activated.") << std::endl << std::endl;
		_tcout << _T("There are ") << argc << _T(" arguments:") << std::endl << std::endl;

		for (int i = 0; i < argc; i++) {
			_tcout << i << _T(" ") << argv[i] << std::endl;
		}

		wchar_t currentPath[MAX_PATH] = {};
		GetCurrentDirectory(MAX_PATH, currentPath);
		_tcout << std::endl << _T("Current directory: ") << currentPath << std::endl;		
	}

	std::ifstream ifile;
	ifile.open("example.txt");

	std::vector <int> itemp;
	std::string number;

	while (!ifile.eof()) {
		getline(ifile, number, ';');
		itemp.push_back(atoi(number.c_str()));
	}

	int * ifinal = new int[itemp.size() + 1];
	ifinal[0] = itemp.size() - 1;

	for (int i = 1; i < itemp.size(); i++) {
		_tcout << "I'm here " << i << " time. ";
		ifinal[i] = itemp[i - 1];
		_tcout << ifinal[i] << std::endl;
	}

	HINSTANCE hDll;
	hDll = LoadLibrary(DLL_TYPE);

	DWORD lastError = GetLastError();

	if (hDll != NULL) {
		if (DEBUG_MODE) {
			_tcout << std::endl << _T("Dll \"") << DLL_TYPE << _T("\" successfully loaded.") << std::endl;
		}

		dllFunction = (MYPROC)GetProcAddress(hDll, "rank");

		if (dllFunction != NULL) {
			_tcout << std::endl << "Preparing for " << THREAD_COUNT << " threads." << std::endl;

			std::vector < HANDLE > threads;

			for (int i = 0; i < THREAD_COUNT; i++) {
				Testing * args;
				args = (Testing *)malloc(sizeof(Testing));

				args->matrix = ifinal;
				args->deleteTemplateColumn = 0;
				args->deleteTemplateRow = 0;

				HANDLE hThread = (HANDLE)_beginthread(ThreadProc, 0, (void*)args);
				threads.push_back(hThread);
			}

			if (threads.size() > 0) {
				WaitForMultipleObjects(threads.size(), &threads[0], TRUE, 1000000);
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

	_tcout << "elapsed time: " << elapsed_seconds.count() << "s\n";
	std::system("Pause >nul");
	delete[] ifinal;
	itemp.clear();

	return 0;
}

