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

typedef struct {
	int * matrix;
	int deleteTemplateColumn, deleteTemplateRow;
	int rank;
}Testing;

typedef int(*MYPROC)(int *, int, int);
MYPROC dllFunction;

int value;

void __cdecl ThreadProc(void * Args) {
	Testing * args = (Testing *) Args;

	int temp = (dllFunction)(args->matrix, args->deleteTemplateColumn, args->deleteTemplateRow);

	if (value < temp) {
		value = temp;
	}

	free(args);

	_endthread();
}

//parameters - first must be either -h/-help or dll name
//second either number of threads (leaving it results in using amount of threads equal amount of cores) 
//in range between 1-64 (higher values will be lowered to 64) or other parameter:
//-d enables debug mode (basically more info)
//-t enables test mode (to test run time of dll)
//-f must be followed by file name (f.e. -f filename), uses file other than example.txt

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2) {
		_tcout << _T("Error! Not provided with DLL name.") << std::endl;
		_tcout << _T("Consider using -h or -help to learn how to use this program.");
		std::system("Pause >nul");
		exit(-1);
	}

	_TCHAR* dllName;

	if ((_wcsicmp(argv[1], _T("-h")) == 0) || (_wcsicmp(argv[1], _T("-help")) == 0)) {
		_tcout << _T("Here should be stuff. But I'm gonna add it later.");
		std::system("Pause >nul");
		exit(0);
	}
	else {
		dllName = argv[1];
	}

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	int threadCount = sysinfo.dwNumberOfProcessors;
	bool debugMode = FALSE;
	std::wstring filename = L"example";
	std::chrono::time_point<std::chrono::system_clock> start, end;
	int argi = 2;

	if (argv[2][0] != '-') {
		int temp = _ttoi(argv[2]);
		if (temp > 0 && temp < 65) {
			threadCount = temp;
			argi = 3;
		}
		else if (temp > 64) {
			threadCount = 64;
		}
	}

	for (argi; argi < argc; argi++) {
		if ((_wcsicmp(argv[argi], _T("-d")) == 0)) {
			debugMode = TRUE;
		}
		else if ((_wcsicmp(argv[argi], _T("-t")) == 0)) {
			_tcout << _T("Testing mode activated.") << std::endl << std::endl;
		}
		else if ((_wcsicmp(argv[argi], _T("-f")) == 0)) {
			argi++;
			if (argi < argc && argv[argi][0] != '-') {
				filename = argv[argi];
			}
			else {
				_tcout << _T("Error! Not provided with the file name.") << std::endl;
				_tcout << _T("Consider using -h or -help to learn how to use this program.");
				std::system("Pause >nul");
				exit(-1);
			}
		}
		else {
			_tcout << _T("Error! Inputed ") << argi << _T(". parameter ")<< argv[argi] << _T(" not expected!") << std::endl;
			_tcout << _T("Consider using -h or -help to learn how to use this program.");
			std::system("Pause >nul");
			exit(-1);
		}
	}

	if (debugMode == 1) {
		_tcout << _T("Debug mode activated.") << std::endl << std::endl;
	}

	std::ifstream ifile;
	std::wstring pathname = L"./Dane testowe/" + filename + L".txt";
	ifile.open(pathname);

	if (debugMode == 1) {
		_tcout << _T("Loaded file form: ") << pathname << std::endl << std::endl;
	}

	std::vector <int> itemp;
	std::string number;

	while (!ifile.eof()) {
		getline(ifile, number, ';');
		itemp.push_back(atoi(number.c_str()));
	}

	int * ifinal = new int[itemp.size() + 1];
	ifinal[0] = sqrt(itemp.size() - 1);

	if (debugMode == 1) {
		_tcout << _T("Matrix size ") << ifinal[0] << _T("x") << ifinal[0] << _T(".") << std::endl << std::endl;
	}

	int counter = 0;
	for (int i = 1; i < itemp.size(); i++) {
		ifinal[i] = itemp[i - 1];
		if (debugMode == 1) {
			counter++;
			_tcout << ifinal[i] << " ";
			if (counter == ifinal[0]){
				_tcout << std::endl;
				counter = 0;
			}
		}

	}

	HINSTANCE hDll;
	hDll = LoadLibrary(dllName);

	DWORD lastError = GetLastError();

	if (hDll != NULL) {
		if (debugMode) {
			_tcout << std::endl << _T("Dll \"") << dllName << _T("\" successfully loaded.") << std::endl;
		}

		dllFunction = (MYPROC)GetProcAddress(hDll, "rank");

		if (dllFunction != NULL) {
			_tcout << std::endl << "Preparing for " << threadCount << " threads." << std::endl;

			start = std::chrono::system_clock::now();

			value = (dllFunction)(ifinal, 0, 0);

			if (value == -2) {
				std::vector < HANDLE > threads;
				std::vector < Testing * > amount;

				for (int i = 0; i < ifinal[0]; i++) {
					for (int j = 0; j < ifinal[0]; j++) {
						Testing * args;
						args = (Testing *)malloc(sizeof(Testing));

						args->matrix = ifinal;
						args->deleteTemplateColumn = pow(2, i);
						args->deleteTemplateRow = pow(2, j);

						amount.push_back(args);
					}
				}

				while (1) {
					for (int i = 0; i < threadCount; i++) {
						if (!amount.empty()) {
							HANDLE hThread = (HANDLE)_beginthread(ThreadProc, 0, (void*)amount.back());
							threads.push_back(hThread);
							amount.pop_back();
						}
					}

					if (threads.size() > 0) {
						WaitForMultipleObjects(threads.size(), &threads[0], TRUE, 1000000);
						threads.clear();
					}
					else {
						break;
					}
				}
			}
			
			printf("TEMP: det = %i\n", value);

			end = std::chrono::system_clock::now();
		}
		else {
			_tcout << std::endl << _T("Function failed to load. Exiting now.") << std::endl;
		}

	}
	else {
		_tcout << _T("Dll failed to load. Exiting now.") << std::endl;
	}

	std::chrono::duration<double> elapsed_seconds = end - start;

	_tcout << "elapsed time: " << elapsed_seconds.count() << "s\n";
	std::system("Pause >nul");
	delete[] ifinal;
	itemp.clear();

	return 0;
}

