#pragma once

#ifdef COMPILE_MATRIXRANK
#define MATRIXRANK_DLL __declspec(dllexport) 
#else
#define MATRIXRANK_DLL __declspec(dllimport) 
#endif

extern "C" {
	int MATRIXRANK_DLL adder(int * matrix, int deleteTemplate);
}
