///////////////////////////////////////////////////////////////////////////////
//  COMEM.H
//
//  Created by :            Date :
//      MichMa                  1/13/94
//
//  Description :
//      Declaration of the COMemory class
//

#ifndef __COMEM_H__
#define __COMEM_H__

#include "dbgxprt.h"
#include "uimem.h"
#include "..\SHL\uwbframe.h"


#pragma warning (disable : 4069)

#define ERROR_ERROR -1

///////////////////////////////////////////////////////////////////////////////
//  COMemory class

// BEGIN_CLASS_HELP
// ClassName: COMemory
// BaseClass: none
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS COMemory

	{
	// Data
	private:
		UIMemory uimem;

	// Utilities
	public:

        BOOL Enable(void);
        BOOL Disable(void);

        BOOL GetMemoryData(int address, char *data, MEM_FORMAT format = MEM_FORMAT_ASCII, int count = 1);
        BOOL GetMemoryData(int address, unsigned char *data, MEM_FORMAT format = MEM_FORMAT_ASCII, int count = 1);
        BOOL GetMemoryData(int address, short int *data, MEM_FORMAT format = MEM_FORMAT_SHORT, int count = 1);
        BOOL GetMemoryData(int address, unsigned short int *data, MEM_FORMAT format = MEM_FORMAT_SHORT_UNSIGNED, int count = 1);
        BOOL GetMemoryData(int address, int *data, MEM_FORMAT format = MEM_FORMAT_LONG, int count = 1);
        BOOL GetMemoryData(int address, unsigned int *data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED, int count = 1);
        BOOL GetMemoryData(int address, long int *data, MEM_FORMAT format = MEM_FORMAT_LONG, int count = 1);
        BOOL GetMemoryData(int address, unsigned long int *data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED, int count = 1);
        BOOL GetMemoryData(int address, float *data, MEM_FORMAT format = MEM_FORMAT_REAL, int count = 1);
        BOOL GetMemoryData(int address, double *data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG, int count = 1);
        BOOL GetMemoryData(int address, long double *data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG, int count = 1);
		BOOL GetMemoryData(int address, __int64 *data, MEM_FORMAT format = MEM_FORMAT_INT64, int count = 1);
		BOOL GetMemoryData(int address, unsigned __int64 *data, MEM_FORMAT format = MEM_FORMAT_INT64_UNSIGNED, int count = 1);

		BOOL GetMemoryData(LPCSTR symbol, char *data, MEM_FORMAT format = MEM_FORMAT_ASCII, int count = 1);
		BOOL GetMemoryData(LPCSTR symbol, unsigned char *data, MEM_FORMAT format = MEM_FORMAT_ASCII, int count = 1);
		BOOL GetMemoryData(LPCSTR symbol, short int *data, MEM_FORMAT format = MEM_FORMAT_SHORT, int count = 1);
		BOOL GetMemoryData(LPCSTR symbol, unsigned short int *data, MEM_FORMAT format = MEM_FORMAT_SHORT_UNSIGNED, int count = 1);
        BOOL GetMemoryData(LPCSTR symbol, int *data, MEM_FORMAT format = MEM_FORMAT_LONG, int count = 1);
		BOOL GetMemoryData(LPCSTR symbol, unsigned int *data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED, int count = 1);
		BOOL GetMemoryData(LPCSTR symbol, long int *data, MEM_FORMAT format = MEM_FORMAT_LONG, int count = 1);
		BOOL GetMemoryData(LPCSTR symbol, unsigned long int *data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED, int count = 1);
		BOOL GetMemoryData(LPCSTR symbol, float *data, MEM_FORMAT format = MEM_FORMAT_REAL, int count = 1);
		BOOL GetMemoryData(LPCSTR symbol, double *data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG, int count = 1);
		BOOL GetMemoryData(LPCSTR symbol, long double *data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG, int count = 1);
		BOOL GetMemoryData(LPCSTR symbol, __int64 *data, MEM_FORMAT format = MEM_FORMAT_INT64, int count = 1);
		BOOL GetMemoryData(LPCSTR symbol, unsigned __int64 *data, MEM_FORMAT format = MEM_FORMAT_INT64_UNSIGNED, int count = 1);

        BOOL SetMemoryData(int address, char *data, MEM_FORMAT format = MEM_FORMAT_ASCII, int count = 1);
        BOOL SetMemoryData(int address, short int *data, MEM_FORMAT format = MEM_FORMAT_SHORT, int count = 1);
        BOOL SetMemoryData(int address, unsigned short int *data, MEM_FORMAT format = MEM_FORMAT_SHORT_UNSIGNED, int count = 1);
        BOOL SetMemoryData(int address, int *data, MEM_FORMAT format = MEM_FORMAT_LONG, int count = 1);
        BOOL SetMemoryData(int address, unsigned int *data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED, int count = 1);
        BOOL SetMemoryData(int address, long int *data, MEM_FORMAT format = MEM_FORMAT_LONG, int count = 1);
        BOOL SetMemoryData(int address, unsigned long int *data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED, int count = 1);
        BOOL SetMemoryData(int address, float *data, MEM_FORMAT format = MEM_FORMAT_REAL, int count = 1);
        BOOL SetMemoryData(int address, double *data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG, int count = 1);
        BOOL SetMemoryData(int address, long double *data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG, int count = 1);
        BOOL SetMemoryData(int address, __int64 *data, MEM_FORMAT format = MEM_FORMAT_INT64, int count = 1);
        BOOL SetMemoryData(int address, unsigned __int64 *data, MEM_FORMAT format = MEM_FORMAT_INT64_UNSIGNED, int count = 1);

        BOOL SetMemoryData(int address, char data, MEM_FORMAT format = MEM_FORMAT_ASCII);
        BOOL SetMemoryData(int address, unsigned char data, MEM_FORMAT format = MEM_FORMAT_ASCII);
        BOOL SetMemoryData(int address, short int data, MEM_FORMAT format = MEM_FORMAT_SHORT);
        BOOL SetMemoryData(int address, unsigned short int data, MEM_FORMAT format = MEM_FORMAT_SHORT_UNSIGNED);
        BOOL SetMemoryData(int address, int data, MEM_FORMAT format = MEM_FORMAT_LONG);
        BOOL SetMemoryData(int address, unsigned int data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED);
        BOOL SetMemoryData(int address, long int data, MEM_FORMAT format = MEM_FORMAT_LONG);
        BOOL SetMemoryData(int address, unsigned long int data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED);
        BOOL SetMemoryData(int address, float data, MEM_FORMAT format = MEM_FORMAT_REAL);
        BOOL SetMemoryData(int address, double data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG);
        BOOL SetMemoryData(int address, long double data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG);
        BOOL SetMemoryData(int address, __int64 data, MEM_FORMAT format = MEM_FORMAT_INT64);
		BOOL SetMemoryData(int address, unsigned __int64 data, MEM_FORMAT format = MEM_FORMAT_INT64_UNSIGNED);

		BOOL SetMemoryData(LPCSTR symbol, char *data, MEM_FORMAT format = MEM_FORMAT_ASCII, int count = 1);
		BOOL SetMemoryData(LPCSTR symbol, short int *data, MEM_FORMAT format = MEM_FORMAT_SHORT, int count = 1);
		BOOL SetMemoryData(LPCSTR symbol, unsigned short int *data, MEM_FORMAT format = MEM_FORMAT_SHORT_UNSIGNED, int count = 1);
        BOOL SetMemoryData(LPCSTR symbol, int *data, MEM_FORMAT format = MEM_FORMAT_LONG, int count = 1);
		BOOL SetMemoryData(LPCSTR symbol, unsigned int *data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED, int count = 1);
		BOOL SetMemoryData(LPCSTR symbol, long int *data, MEM_FORMAT format = MEM_FORMAT_LONG, int count = 1);
		BOOL SetMemoryData(LPCSTR symbol, unsigned long int *data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED, int count = 1);
		BOOL SetMemoryData(LPCSTR symbol, float *data, MEM_FORMAT format = MEM_FORMAT_REAL, int count = 1);
		BOOL SetMemoryData(LPCSTR symbol, double *data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG, int count = 1);
		BOOL SetMemoryData(LPCSTR symbol, long double *data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG, int count = 1);
		BOOL SetMemoryData(LPCSTR symbol, __int64 *data, MEM_FORMAT format = MEM_FORMAT_INT64, int count = 1);
		BOOL SetMemoryData(LPCSTR symbol, unsigned __int64 *data, MEM_FORMAT format = MEM_FORMAT_INT64_UNSIGNED, int count = 1);

        BOOL SetMemoryData(LPCSTR symbol, char data, MEM_FORMAT format = MEM_FORMAT_ASCII);		
		BOOL SetMemoryData(LPCSTR symbol, unsigned char data, MEM_FORMAT format = MEM_FORMAT_ASCII);
		BOOL SetMemoryData(LPCSTR symbol, short int data, MEM_FORMAT format = MEM_FORMAT_SHORT);		
		BOOL SetMemoryData(LPCSTR symbol, unsigned short int data, MEM_FORMAT format = MEM_FORMAT_SHORT_UNSIGNED);
		BOOL SetMemoryData(LPCSTR symbol, int data, MEM_FORMAT format = MEM_FORMAT_LONG);
		BOOL SetMemoryData(LPCSTR symbol, unsigned int data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED);
		BOOL SetMemoryData(LPCSTR symbol, long int data, MEM_FORMAT format = MEM_FORMAT_LONG);
		BOOL SetMemoryData(LPCSTR symbol, unsigned long int data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED);
		BOOL SetMemoryData(LPCSTR symbol, float data, MEM_FORMAT format = MEM_FORMAT_REAL);
		BOOL SetMemoryData(LPCSTR symbol, double data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG);
		BOOL SetMemoryData(LPCSTR symbol, long double data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG);
		BOOL SetMemoryData(LPCSTR symbol, __int64 data, MEM_FORMAT format = MEM_FORMAT_INT64);
		BOOL SetMemoryData(LPCSTR symbol, unsigned __int64 data, MEM_FORMAT format = MEM_FORMAT_INT64_UNSIGNED);

        BOOL MemoryDataIs(int address, char *data, MEM_FORMAT format = MEM_FORMAT_ASCII, int count = 1);
        BOOL MemoryDataIs(int address, unsigned char *data, MEM_FORMAT format = MEM_FORMAT_ASCII, int count = 1);
        BOOL MemoryDataIs(int address, short int *data, MEM_FORMAT format = MEM_FORMAT_SHORT, int count = 1);
        BOOL MemoryDataIs(int address, unsigned short int *data, MEM_FORMAT format = MEM_FORMAT_SHORT_UNSIGNED, int count = 1);
        BOOL MemoryDataIs(int address, int *data, MEM_FORMAT format = MEM_FORMAT_LONG, int count = 1);
        BOOL MemoryDataIs(int address, unsigned int *data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED, int count = 1);
        BOOL MemoryDataIs(int address, long int *data, MEM_FORMAT format = MEM_FORMAT_LONG, int count = 1);
        BOOL MemoryDataIs(int address, unsigned long int *data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED, int count = 1);
        BOOL MemoryDataIs(int address, float *data, MEM_FORMAT format = MEM_FORMAT_REAL, int count = 1);
        BOOL MemoryDataIs(int address, double *data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG, int count = 1);
        BOOL MemoryDataIs(int address, long double *data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG, int count = 1);
        BOOL MemoryDataIs(int address, __int64 *data, MEM_FORMAT format = MEM_FORMAT_INT64, int count = 1);
        BOOL MemoryDataIs(int address, unsigned __int64 *data, MEM_FORMAT format = MEM_FORMAT_INT64_UNSIGNED, int count = 1);

		BOOL MemoryDataIs(LPCSTR symbol, char *data, MEM_FORMAT format = MEM_FORMAT_ASCII, int count = 1);
		BOOL MemoryDataIs(LPCSTR symbol, unsigned char *data, MEM_FORMAT format = MEM_FORMAT_ASCII, int count = 1);
		BOOL MemoryDataIs(LPCSTR symbol, short int *data, MEM_FORMAT format = MEM_FORMAT_SHORT, int count = 1);
		BOOL MemoryDataIs(LPCSTR symbol, unsigned short int *data, MEM_FORMAT format = MEM_FORMAT_SHORT_UNSIGNED, int count = 1);
        BOOL MemoryDataIs(LPCSTR symbol, int *data, MEM_FORMAT format = MEM_FORMAT_LONG, int count = 1);
		BOOL MemoryDataIs(LPCSTR symbol, unsigned int *data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED, int count = 1);
		BOOL MemoryDataIs(LPCSTR symbol, long int *data, MEM_FORMAT format = MEM_FORMAT_LONG, int count = 1);
		BOOL MemoryDataIs(LPCSTR symbol, unsigned long int *data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED, int count = 1);
		BOOL MemoryDataIs(LPCSTR symbol, float *data, MEM_FORMAT format = MEM_FORMAT_REAL, int count = 1);
		BOOL MemoryDataIs(LPCSTR symbol, double *data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG, int count = 1);
		BOOL MemoryDataIs(LPCSTR symbol, long double *data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG, int count = 1);
        BOOL MemoryDataIs(LPCSTR symbol, __int64 *data, MEM_FORMAT format = MEM_FORMAT_INT64, int count = 1);
        BOOL MemoryDataIs(LPCSTR symbol, unsigned __int64 *data, MEM_FORMAT format = MEM_FORMAT_INT64_UNSIGNED, int count = 1);

        BOOL MemoryDataIs(int address, char data, MEM_FORMAT format = MEM_FORMAT_ASCII);
        BOOL MemoryDataIs(int address, unsigned char data, MEM_FORMAT format = MEM_FORMAT_ASCII);
        BOOL MemoryDataIs(int address, short int data, MEM_FORMAT format = MEM_FORMAT_SHORT);
        BOOL MemoryDataIs(int address, unsigned short int data, MEM_FORMAT format = MEM_FORMAT_SHORT_UNSIGNED);
        BOOL MemoryDataIs(int address, int data, MEM_FORMAT format = MEM_FORMAT_LONG);
        BOOL MemoryDataIs(int address, unsigned int data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED);
        BOOL MemoryDataIs(int address, long int data, MEM_FORMAT format = MEM_FORMAT_LONG);
        BOOL MemoryDataIs(int address, unsigned long int data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED);
        BOOL MemoryDataIs(int address, float data, MEM_FORMAT format = MEM_FORMAT_REAL);
        BOOL MemoryDataIs(int address, double data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG);
        BOOL MemoryDataIs(int address, long double data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG);
        BOOL MemoryDataIs(int address, __int64 data, MEM_FORMAT format = MEM_FORMAT_INT64);
        BOOL MemoryDataIs(int address, unsigned __int64 data, MEM_FORMAT format = MEM_FORMAT_INT64_UNSIGNED);

        BOOL MemoryDataIs(LPCSTR symbol, char data, MEM_FORMAT format = MEM_FORMAT_ASCII);
        BOOL MemoryDataIs(LPCSTR symbol, unsigned char data, MEM_FORMAT format = MEM_FORMAT_ASCII);
        BOOL MemoryDataIs(LPCSTR symbol, short int data, MEM_FORMAT format = MEM_FORMAT_SHORT);
        BOOL MemoryDataIs(LPCSTR symbol, unsigned short int data, MEM_FORMAT format = MEM_FORMAT_SHORT_UNSIGNED);
        BOOL MemoryDataIs(LPCSTR symbol, int data, MEM_FORMAT format = MEM_FORMAT_LONG);
        BOOL MemoryDataIs(LPCSTR symbol, unsigned int data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED);
        BOOL MemoryDataIs(LPCSTR symbol, long int data, MEM_FORMAT format = MEM_FORMAT_LONG );
        BOOL MemoryDataIs(LPCSTR symbol, unsigned long int data, MEM_FORMAT format = MEM_FORMAT_LONG_UNSIGNED);
        BOOL MemoryDataIs(LPCSTR symbol, float data, MEM_FORMAT format = MEM_FORMAT_REAL);
        BOOL MemoryDataIs(LPCSTR symbol, double data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG);
        BOOL MemoryDataIs(LPCSTR symbol, long double data, MEM_FORMAT format = MEM_FORMAT_REAL_LONG);
        BOOL MemoryDataIs(LPCSTR symbol, __int64 data, MEM_FORMAT format = MEM_FORMAT_INT64);
        BOOL MemoryDataIs(LPCSTR symbol, unsigned __int64 data, MEM_FORMAT format = MEM_FORMAT_INT64_UNSIGNED);

		BOOL SetMemoryFormat(MEM_FORMAT format);
		CString GetColumns(void);
		BOOL SetColumns(CString Columns);
		CString COMemory::GetAddress(CString symbol);
		BOOL CycleMemoryFormat(int count);

		CString ExtractMemoryData(MEM_FORMAT format, int address, int count);
		CString ExtractMemoryData(MEM_FORMAT format, LPCSTR symbol, int count);
		BOOL SetMemoryBytes(int address, LPCSTR bytes, MEM_FORMAT format = MEM_FORMAT_BYTE);
		BOOL SetMemoryBytes(LPCSTR symbol, LPCSTR bytes, MEM_FORMAT format = MEM_FORMAT_BYTE);
	};

#pragma warning (default : 4069)

#endif // __COMEM_H__
