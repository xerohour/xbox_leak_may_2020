///////////////////////////////////////////////////////////////////////////////
//  COMEM.CPP
//
//  Created by :            Date :
//      MichMa                  1/13/94
//
//  Description :
//      Implementation of the COMemory class
//

#include "stdafx.h"
#include "comem.h"
#include "uidebug.h"
#include "..\SRC\cosource.h"
#include "..\SYM\qcqp.h"
#include "..\SHL\uwbframe.h"
#include <testxcpt.h>
#include <guiv1.h>

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#pragma warning (disable : 4069)

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::Enable(void)
// Description: Enable memory access in the debugger.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COMemory::Enable(void)
	{
	uimem = UIDebug::ShowDockWindow(IDW_MEMORY_WIN);
	return uimem.IsValid();
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::Disable(void)
// Description: Disable memory access in the debugger.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// END_HELP_COMMENT
BOOL COMemory::Disable(void)
	{
	uimem.Close();
	return TRUE;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(int address, char *data, MEM_FORMAT format, int count /* 1 */)
// Description: Get data from memory at the specified address.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: address An integer that contains the address in memory to retrieve data from.
// Param: data A pointer to a char buffer that will contain the data retrieved.
// Param: count An integer that contains the number of elements of data to retrieve from memory. (Default value is 1.)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(int address, char *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return GetMemoryData(symbol, data, format, count);

	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(int address, unsigned char *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(int address, unsigned char *data, MEM_FORMAT format, int count)
	{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return GetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(int address, short int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(int address, short int *data, MEM_FORMAT format, int count)
	{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return GetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(int address, unsigned short int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(int address, unsigned short int *data, MEM_FORMAT format, int count)
	{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return GetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(int address, int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(int address, int *data, MEM_FORMAT format, int count)
	{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return GetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(int address, unsigned int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(int address, unsigned int *data, MEM_FORMAT format, int count)
	{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return GetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(int address, long int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(int address, long int *data, MEM_FORMAT format, int count)
	{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return GetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(int address, unsigned long int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(int address, unsigned long int *data, MEM_FORMAT format, int count)
	{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return GetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(int address, float *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(int address, float *data, MEM_FORMAT format, int count)
	{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return GetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(int address, double *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(int address, double *data, MEM_FORMAT format, int count)
	{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return GetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(int address, long double *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(int address, long double *data, MEM_FORMAT format, int count)
	{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return GetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(int address, __int64 *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(int address, __int64 *data, MEM_FORMAT format, int count)
	{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return GetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(int address, unsigned __int64 *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(int address, unsigned __int64 *data, MEM_FORMAT format, int count)
	{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return GetMemoryData(symbol, data, format, count);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(LPCSTR symbol, char *data, MEM_FORMAT format,  int count /* 1 */)
// Description: Get data from memory for the specified symbol.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: symbol A string that contains the symbol in memory to retrieve data from.
// Param: data A pointer to a buffer that will contain the data retrieved.
// Param: count An integer that contains the number of elements of data to retrieve from memory. (Default value is 1.)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(LPCSTR symbol, char *data, MEM_FORMAT format, int count /* 1 */)
	{
	CString cstr = ExtractMemoryData(format, symbol, count);

	if(cstr == "")
		return FALSE;

	// In order to prevent GetAt(i) from failing we need to limit count
	if(count > cstr.GetLength())
		count = cstr.GetLength();

	for(int i = 0; i < count; i++)
		data[i] = cstr.GetAt(i);

	return TRUE;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(LPCSTR symbol, unsigned char *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(LPCSTR symbol, unsigned char *data, MEM_FORMAT format, int count /* 1 */)
	{
	CString cstr = ExtractMemoryData(format, symbol, count);

	if(cstr == "")
		return FALSE;

	// In order to prevent GetAt(i) from failing we need to limit count
	if(count > cstr.GetLength())
		count = cstr.GetLength();
	for(int i = 0; i < count; i++)
		data[i] = cstr.GetAt(i);

	return TRUE;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(LPCSTR symbol, short int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(LPCSTR symbol, short int *data, MEM_FORMAT format, int count /* 1 */)
	{
	CString cstr = ExtractMemoryData(format, symbol, count);

	if(cstr == "")
		return FALSE;

	for(int i = 0; i < count; i++)
		data[i] = atoi(cstr.Mid(uimem.GetFormatWidth() * i, uimem.GetFormatWidth()));

	return TRUE;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(LPCSTR symbol, unsigned short int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(LPCSTR symbol, unsigned short int *data, MEM_FORMAT format, int count /* 1 */)
	{
	CString cstr = ExtractMemoryData(format, symbol, count);

	if(cstr == "")
		return FALSE;

	for(int i = 0; i < count; i++)
		data[i] = atoi(cstr.Mid(uimem.GetFormatWidth() * i, uimem.GetFormatWidth()));

	return TRUE;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(LPCSTR symbol, int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(LPCSTR symbol, int *data, MEM_FORMAT format, int count /* 1 */)
	{
	CString cstr = ExtractMemoryData(format, symbol, count);

	if(cstr == "")
		return FALSE;

	for(int i = 0; i < count; i++)
		data[i] = atoi(cstr.Mid(uimem.GetFormatWidth() * i, uimem.GetFormatWidth()));

	return TRUE;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(LPCSTR symbol, unsigned int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(LPCSTR symbol, unsigned int *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	CString cstr = ExtractMemoryData(format, symbol, count);

	if(cstr == "")
		return FALSE;

	for(int i = 0; i < count; i++)
		data[i] = (unsigned)atoi(cstr.Mid(uimem.GetFormatWidth() * i, uimem.GetFormatWidth()));

	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(LPCSTR symbol, long int *data, MEM_FORMAT format, int count)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(LPCSTR symbol, long int *data, MEM_FORMAT format, int count)
	{
	CString cstr = ExtractMemoryData(format, symbol, count);

	if(cstr == "")
		return FALSE;

	for(int i = 0; i < count; i++)
		data[i] = atol(cstr.Mid(uimem.GetFormatWidth() * i, uimem.GetFormatWidth()));

	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(LPCSTR symbol, unsigned long int *data, MEM_FORMAT format, int count)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(LPCSTR symbol, unsigned long int *data, MEM_FORMAT format, int count)
	{
	CString cstr = ExtractMemoryData(format, symbol, count);

	if(cstr == "")
		return FALSE;

	for(int i = 0; i < count; i++)
		data[i] = (unsigned)atol(cstr.Mid(uimem.GetFormatWidth() * i, uimem.GetFormatWidth()));

	return TRUE;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(LPCSTR symbol, float *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(LPCSTR symbol, float *data, MEM_FORMAT format, int count /* 1 */)
	{
	CString cstr = ExtractMemoryData(format, symbol, count);

	if(cstr == "")
		return FALSE;

	for(int i = 0; i < count; i++)
		data[i] = (float)atof(cstr.Mid(uimem.GetFormatWidth() * i, uimem.GetFormatWidth()));

	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(LPCSTR symbol, double *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(LPCSTR symbol, double *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	CString cstr = ExtractMemoryData(format, symbol, count);

	if(cstr == "")
		return FALSE;

	for(int i = 0; i < count; i++)
		data[i] = atof(cstr.Mid(uimem.GetFormatWidth() * i, uimem.GetFormatWidth()));

	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(LPCSTR symbol, long double *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(LPCSTR symbol, long double *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	CString cstr = ExtractMemoryData(format, symbol, count);

	if(cstr == "")
		return FALSE;

	for(int i = 0; i < count; i++)
		data[i] = (long double)atof(cstr.Mid(uimem.GetFormatWidth() * i, uimem.GetFormatWidth()));

	return TRUE;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(LPCSTR symbol, __int64 *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(LPCSTR symbol, __int64 *data, MEM_FORMAT format, int count)
	{
	CString cstr = ExtractMemoryData(format, symbol, count);

	if(cstr == "")
		return FALSE;

	for(int i = 0; i < count; i++)
		data[i] = (__int64)_atoi64(cstr.Mid(uimem.GetFormatWidth() * i, uimem.GetFormatWidth()));

	return TRUE;
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::GetMemoryData(LPCSTR symbol, unsigned __int64 *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::GetMemoryData(LPCSTR symbol, unsigned __int64 *data, MEM_FORMAT format, int count)
	{
	CString cstr = ExtractMemoryData(format, symbol, count);

	if(cstr == "")
		return FALSE;

	for(int i = 0; i < count; i++)
		data[i] = (unsigned __int64)_atoi64(cstr.Mid(uimem.GetFormatWidth() * i, uimem.GetFormatWidth()));

	return TRUE;
	}

// TODO (michma): needs error logging.
CString COMemory::ExtractMemoryData(MEM_FORMAT format, int address, int count)	
	
	{ 

	char string[128];
	return ExtractMemoryData(format, _itoa(address, string, 10), count);

	}


// TODO (michma): needs error logging.
CString COMemory::ExtractMemoryData(MEM_FORMAT format, LPCSTR symbol, int count)	
	
	{ 
	
	if(!SetMemoryFormat(format)) return CString("");
	int MemIsActive = uimem.IsActive();

	if(!MemIsActive)
		if(!Enable()) return CString("");

	if (uimem.GoToAddress(symbol) != ERROR_SUCCESS)
		return CString("");

	CString data = uimem.GetCurrentData(count);
	
	if(!MemIsActive)
		if(!Disable()) return CString("");

	LOG->RecordInfo("Memory - extracted data = %s\n", data);
	return data;
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, char *data, MEM_FORMAT format /* Corresponding Format */, int count /* 1 */)
// Description: Set data at a specified memory location.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: address An integer that contains the address in memory to start setting data.
// Param: data A pointer to a buffer that contains the data to set.
// Param: format A MEM_FORMAT variable that will be used to set the memory format before setting data
// Param: count An integer that contains the number of elements of data to set in memory. (Default value is 1.)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, char *data, MEM_FORMAT format, int count /* 1 */)	//TODO: use big-endian for
	{																						// mac products.
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return SetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, short int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, short int *data, MEM_FORMAT format, int count /* 1 */)
	{																						// mac products.
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return SetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, unsigned short int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, unsigned short int *data, MEM_FORMAT format, int count /* 1 */)
	{																						// mac products.
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return SetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, int *data, MEM_FORMAT format, int count /* 1 */)
	{																						// mac products.
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return SetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, unsigned int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, unsigned int *data, MEM_FORMAT format, int count /* 1 */)
	{																						// mac products.
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return SetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, long int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, long int *data, MEM_FORMAT format, int count /* 1 */)
	{																						// mac products.
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return SetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, unsigned long int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, unsigned long int *data, MEM_FORMAT format, int count /* 1 */)
	{																						// mac products.
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return SetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, float *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, float *data, MEM_FORMAT format, int count /* 1 */)
	{																						// mac products.
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return SetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function:  BOOL COMemory::SetMemoryData(int address, double *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, double *data, MEM_FORMAT format, int count /* 1 */)
	{																						// mac products.
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return SetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, long double *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, long double *data, MEM_FORMAT format, int count /* 1 */)
	{																						// mac products.
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return SetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, __int64 *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, __int64 *data, MEM_FORMAT format, int count /* 1 */)
	{																						// mac products.
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return SetMemoryData(symbol, data, format, count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, unsigned __int64 *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, unsigned __int64 *data, MEM_FORMAT format, int count /* 1 */)
	{																						// mac products.
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return SetMemoryData(symbol, data, format, count);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, char data, MEM_FORMAT format /* Corresponding Format */)
// Description: Set data at a specified memory location.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: address An integer that contains the address in memory to start setting data.
// Param: data A variable that contains the data to set.
// Param: format A MEM_FORMAT variable that will be used to set the memory format before setting data
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, char data, MEM_FORMAT format)
	{
		//	modified by dverma Apr 2000 - must use string instead of &data
		char str[8];
		sprintf(str, "%c", data);

		return SetMemoryData(address, str, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, unsigned char data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, unsigned char data, MEM_FORMAT format)
	{
		//	modified by dverma Apr 2000 - must use string instead of &data
		char str[8];
		sprintf(str, "%c", data);
	
		return SetMemoryData(address, str, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, short int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, short int data, MEM_FORMAT format)
	{
	return SetMemoryData(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, unsigned short int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, unsigned short int data, MEM_FORMAT format)
	{
	return SetMemoryData(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, int data, MEM_FORMAT format)
	{
	return SetMemoryData(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, unsigned int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, unsigned int data, MEM_FORMAT format)
	{
	return SetMemoryData(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, long int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, long int data, MEM_FORMAT format)
	{
	return SetMemoryData(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, unsigned long int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, unsigned long int data, MEM_FORMAT format)
	{
	return SetMemoryData(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, float data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, float data, MEM_FORMAT format)
	{
	return SetMemoryData(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, double data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, double data, MEM_FORMAT format)
	{
	return SetMemoryData(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, long double data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, long double data, MEM_FORMAT format)
	{
	return SetMemoryData(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, __int64 data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, __int64 data, MEM_FORMAT format)
	{
	return SetMemoryData(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(int address, unsigned __int64 data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(int address, unsigned __int64 data, MEM_FORMAT format)
	{
	return SetMemoryData(address, &data, format);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HELP_COMMENT
// Function: int COMemory::SetMemoryData(LPCSTR symbol, char *data, MEM_FORMAT format /* Corresponding Format */, int count /* 1 */)
// Description: Set data at a specified memory location.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: symbol A pointer to a string that specifies a symbol where memory is to be set. The address of the symbol is used to as the address where memory changes will occur.
// Param: data A pointer to a buffer that contains the data to set.
// Param: format A MEM_FORMAT variable that will be used to set the memory format before setting data
// Param: count An integer that contains the number of elements of data to set in memory. (Default value is 1.)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, char *data, MEM_FORMAT format, int count /* 1 */)	//TODO: use big-endian for
																			// mac products.
	{
		return SetMemoryBytes(symbol, data, format);
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, short int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, short int *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	
	const int short nSymbolsInShort = 5;  // We asume that short int contains sign + 5 digits + '\0'
	char sOneShort[nSymbolsInShort];

	char *bytes = new char[count * (nSymbolsInShort + 2)];
	*bytes = '\0';

	for(int i = 0; i < count; i++)
	{
		if(data[i] > 0)
			strcat(bytes, " ");
		else
			strcat(bytes, "-");

		// Convert short to the string
		_itoa(abs(data[i]), sOneShort, 10);

		// Calculate length and add spaces
		for(short j = 0; j < nSymbolsInShort - (short) strlen(sOneShort); j++)
			strcat(bytes, " ");

		strcat(bytes, sOneShort);

	}
				
	return SetMemoryBytes(symbol, bytes, format);
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned short int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned short int *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	const int short nSymbolsInShort = 5;  // We asume that unsigned short int contains 5 digits + '\0'
	char sOneShort[nSymbolsInShort];

	char *bytes = new char[count * (nSymbolsInShort + 2)];
	*bytes = '\0';

	for(int i = 0; i < count; i++)
	{
		// Convert short to the string
		_itoa(data[i], sOneShort, 10);

		// Calculate length and add spaces
		for(short j = 0; j < nSymbolsInShort - (short) strlen(sOneShort); j++)
			strcat(bytes, " ");

		strcat(bytes, sOneShort);

	}
				
	return SetMemoryBytes(symbol, bytes, format);

	}


// BEGIN_HELP_COMMENT
// Function: int COMemory::SetMemoryData(LPCSTR symbol, int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, int *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	const int short nSymbolsInInt = 10;  // We asume that int contains sign + 10 digits + '\0'
	char sOneInteger[nSymbolsInInt];

	char *bytes = new char[count * (nSymbolsInInt + 2)];
	*bytes = '\0';

	for(int i = 0; i < count; i++)
	{
		if(data[i] > 0)
			strcat(bytes, " ");
		else
			strcat(bytes, "-");

		// Convert short to the string
		_itoa(abs(data[i]), sOneInteger, 10);

		// Calculate length and add spaces
		for(short j = 0; j < nSymbolsInInt - (short) strlen(sOneInteger); j++)
			strcat(bytes, " ");

		strcat(bytes, sOneInteger);

	}
				
	return SetMemoryBytes(symbol, bytes, format);	
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned int *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	const int short nSymbolsInInt = 10;  // We asume that unsigned int contains 10 digits + '\0'
	char sOneInteger[nSymbolsInInt];

	char *bytes = new char[count * (nSymbolsInInt + 2)];
	*bytes = '\0';

	for(int i = 0; i < count; i++)
	{
		// Convert short to the string
		_itoa(data[i], sOneInteger, 10);

		// Calculate length and add spaces
		for(short j = 0; j < nSymbolsInInt - (short) strlen(sOneInteger); j++)
			strcat(bytes, " ");

		strcat(bytes, sOneInteger);

	}
				
	return SetMemoryBytes(symbol, bytes, format);	
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, long int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, long int *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	const int short nSymbolsInLongInt = 10;  // We asume that long int contains sign + 10 digits + '\0'
	char sOneLongInteger[nSymbolsInLongInt];

	char *bytes = new char[count * (nSymbolsInLongInt + 2)];
	*bytes = '\0';

	for(int i = 0; i < count; i++)
	{
		if(data[i] > 0)
			strcat(bytes, " ");
		else
			strcat(bytes, "-");

		// Convert short to the string
		_itoa(abs(data[i]), sOneLongInteger, 10);

		// Calculate length and add spaces
		for(short j = 0; j < nSymbolsInLongInt - (short) strlen(sOneLongInteger); j++)
			strcat(bytes, " ");

		strcat(bytes, sOneLongInteger);

	}
				
	return SetMemoryBytes(symbol, bytes, format);	
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned long int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned long int *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	const int short nSymbolsInLongInt = 10;  // We asume that unsigned long int contains 10 digits + '\0'
	char sOneLongInteger[nSymbolsInLongInt];

	char *bytes = new char[count * (nSymbolsInLongInt + 2)];
	*bytes = '\0';

	for(int i = 0; i < count; i++)
	{
		// Convert short to the string
		_itoa(data[i], sOneLongInteger, 10);

		// Calculate length and add spaces
		for(short j = 0; j < nSymbolsInLongInt - (short) strlen(sOneLongInteger); j++)
			strcat(bytes, " ");

		strcat(bytes, sOneLongInteger);

	}
				
	return SetMemoryBytes(symbol, bytes, format);	
	}

// TODO: (dklem 08/12/98) Change the code for the next three functions when #4727 gets fixed.
//	Note: the code for these functions (for the float, double and long double) is NYI because memory window behavior when 
//	changing (typing) these types is not finally defined yet.

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, float *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, float *data, MEM_FORMAT format, int count /* 1 */)
	{
	const int short nSymbolsInReal = 18;  // We asume that real contains 14 digits + 4 '{'s + '\0'
	char sOneReal[nSymbolsInReal];
	char *ptr;

	char *bytes = new char[count * (nSymbolsInReal + 2)];
	*bytes = '\0';

	for(int i = 0; i < count; i++)
	{
		// We should add a sign first
		if(data[i] > 0)
			strcat(bytes, "{+}");

		// Convert real to the string
		sprintf(sOneReal, "%.5E", data[i]);

		// we should put {} around + at the 11th position
		if(ptr = strchr(sOneReal,'+'))
		{
			memcpy(ptr + 3, ptr + 1, 3);
			strncpy(ptr, "{+}", 3);
			*(ptr + 6) = '\0';
		}

		strcat(bytes, sOneReal);
	}
				
	return SetMemoryBytes(symbol, bytes, format);	
	
	}	

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, double *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, double *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	const int short nSymbolsInDouble = 27;  // We asume that double contains 23 digits + 4 '{'s + '\0'
	char sOneDouble[nSymbolsInDouble];
	char *ptr;

	char *bytes = new char[count * (nSymbolsInDouble + 2)];
	*bytes = '\0';

	for(int i = 0; i < count; i++)
	{
		// We should add a sign first
		if(data[i] > 0)
			strcat(bytes, "{+}");

		// Convert double to the string
		sprintf(sOneDouble, "%.15E",  data[i]);

		// we should put {} around + at the 11th position
		if(ptr = strchr(sOneDouble,'+'))
		{
			memcpy(ptr + 3, ptr + 1, 3);
			strncpy(ptr, "{+}", 3);
			*(ptr + 6) = '\0';
		}

		strcat(bytes, sOneDouble);
	}
				
	return SetMemoryBytes(symbol, bytes, format);	
}


// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, long double *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, long double *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	const int short nSymbolsInLongDouble = 27;  // We asume that long double contains 23 digits + 4 '{'s + '\0'
	char sOneLongDouble[nSymbolsInLongDouble];
	char *ptr;

	char *bytes = new char[count * (nSymbolsInLongDouble + 2)];
	*bytes = '\0';

	for(int i = 0; i < count; i++)
	{
		// We should add a sign first
		if(data[i] > 0)
			strcat(bytes, "{+}");

		// Convert long double to the string
		sprintf(sOneLongDouble, "%.14E", (double) data[i]);

		// we should put {} around + at the 11th position
		if(ptr = strchr(sOneLongDouble,'+'))
		{
			memcpy(ptr + 3, ptr + 1, 3);
			strncpy(ptr, "{+}", 3);
			*(ptr + 6) = '\0';
		}

		strcat(bytes, sOneLongDouble);
	}
				
	return SetMemoryBytes(symbol, bytes, format);	

	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, __int64 *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, __int64 *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	const int short nSymbolsInInt64 = 19;  // We asume that __int64 contains sign + 19 digits + '\0'
	char sOneInteger64[nSymbolsInInt64];

	char *bytes = new char[count * (nSymbolsInInt64 + 2)];
	*bytes = '\0';

	for(int i = 0; i < count; i++)
	{
		if(data[i] > 0)
			strcat(bytes, " ");
		else 
		{
			strcat(bytes, "-");
			// Since there is no abs for __int64, we do abs here manually
			data[i] -= (2*data[i]);
		}
		
		// Convert short to the string
		_i64toa(data[i], sOneInteger64, 10);

		// Calculate length and add spaces
		for(short j = 0; j < nSymbolsInInt64 - (short) strlen(sOneInteger64); j++)
			strcat(bytes, " ");

		strcat(bytes, sOneInteger64);

	}
				
	return SetMemoryBytes(symbol, bytes, format);	
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned __int64 *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned __int64 *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	const int short nSymbolsInInt64 = 19;  // We asume that unsigned __int64 contains 19 digits + '\0'
	char sOneInteger64[nSymbolsInInt64];

	char *bytes = new char[count * (nSymbolsInInt64 + 2)];
	*bytes = '\0';

	for(int i = 0; i < count; i++)
	{
		// Convert short to the string
		_ui64toa(data[i], sOneInteger64, 10);

		// Calculate length and add spaces
		for(short j = 0; j < nSymbolsInInt64 - (short) strlen(sOneInteger64); j++)
			strcat(bytes, " ");

		strcat(bytes, sOneInteger64);

	}
				
	return SetMemoryBytes(symbol, bytes, format);	
	}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, char data, MEM_FORMAT format /* Corresponding Format */)
// Description: Set data at a specified memory location.
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: symbol A pointer to a string that specifies a symbol where memory is to be set. The address of the symbol is used to as the address where memory changes will occur.
// Param: data A char that contains the data to set.
// Param: format A MEM_FORMAT variable that will be used to set the memory format before setting data
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, char data, MEM_FORMAT format)
	{
		//	modified by dverma Apr 2000 - must use string instead of &data
		char str[8];
		sprintf(str, "%c", data);

		return SetMemoryData(symbol, str, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned char data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned char data, MEM_FORMAT format)
	{
		//	modified by dverma Apr 2000 - must use string instead of &data
		char str[8];
		sprintf(str, "%c", data);

		return SetMemoryData(symbol, str, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, short int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, short int data, MEM_FORMAT format)
	{
	return SetMemoryData(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, int data, MEM_FORMAT format)
	{
	return SetMemoryData(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned short int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned short int data, MEM_FORMAT format)
	{
	return SetMemoryData(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned int data, MEM_FORMAT format)
	{
	return SetMemoryData(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, long int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, long int data, MEM_FORMAT format)
	{
	return SetMemoryData(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned long int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned long int data, MEM_FORMAT format)
	{
	return SetMemoryData(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, float data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, float data, MEM_FORMAT format)
	{
	return SetMemoryData(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, double data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, double data, MEM_FORMAT format)
	{
	return SetMemoryData(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, long double data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, long double data, MEM_FORMAT format)
	{
	return SetMemoryData(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, __int64 data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, __int64 data, MEM_FORMAT format)
	{
	return SetMemoryData(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned __int64 data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::SetMemoryData(LPCSTR symbol, unsigned __int64 data, MEM_FORMAT format)
	{
	return SetMemoryData(symbol, &data, format);
	}


BOOL COMemory::SetMemoryBytes(int address, LPCSTR bytes, MEM_FORMAT format)
	{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return SetMemoryBytes(symbol, bytes, format);
	}


// TODO (michma): needs error logging.
BOOL COMemory::SetMemoryBytes(LPCSTR symbol, LPCSTR bytes, MEM_FORMAT format)

	{
	if(!SetMemoryFormat(format)) return FALSE;
	int MemIsActive = uimem.IsActive();

	if(!MemIsActive)
		if(!Enable()) return FALSE;

	if(uimem.GoToAddress(symbol) != ERROR_SUCCESS) return FALSE;
	if(!uimem.SetCurrentData(bytes)) return FALSE;
	
	if(!MemIsActive)
		if(!Disable()) return FALSE;

	return TRUE;
	}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, char *data, MEM_FORMAT format, int count /* 1 */)
// Description: Determine if the data at the specified address is the same as the data given.
// Return: A Boolean value that indicates whether the data at the specified address is the same as the data given (TRUE) or not.
// Param: address An integer that contains the address in memory to compare with.
// Param: data A pointer to a buffer that contains the data to compare against.
// Param: count An integer that contains the number of elements of data to compare. (Default value is 1.)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, char *data, MEM_FORMAT format, int count /* 1 */)
{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return MemoryDataIs(symbol, data, format, count);
}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, unsigned char *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, unsigned char *data, MEM_FORMAT format, int count /* 1 */)
{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return MemoryDataIs(symbol, data, format, count);
}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, short int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, short int *data, MEM_FORMAT format, int count /* 1 */)
{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return MemoryDataIs(symbol, data, format, count);
}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, unsigned short int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, unsigned short int *data, MEM_FORMAT format, int count /* 1 */)
{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return MemoryDataIs(symbol, data, format, count);
}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, int *data, MEM_FORMAT format, int count /* 1 */)
{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return MemoryDataIs(symbol, data, format, count);
}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, unsigned int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, unsigned int *data, MEM_FORMAT format, int count /* 1 */)
{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return MemoryDataIs(symbol, data, format, count);
}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, long int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, long int *data, MEM_FORMAT format, int count /* 1 */)
{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return MemoryDataIs(symbol, data, format, count);
}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, unsigned long int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, unsigned long int *data, MEM_FORMAT format, int count /* 1 */)
{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return MemoryDataIs(symbol, data, format, count);
}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, float *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, float *data, MEM_FORMAT format, int count /* 1 */)
{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return MemoryDataIs(symbol, data, format, count);
}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, double *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, double *data, MEM_FORMAT format, int count /* 1 */)
{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return MemoryDataIs(symbol, data, format, count);
}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, long double *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, long double *data, MEM_FORMAT format, int count /* 1 */)
{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return MemoryDataIs(symbol, data, format, count);
}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, __int64 *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, __int64 *data, MEM_FORMAT format, int count /* 1 */)
{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return MemoryDataIs(symbol, data, format, count);
}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, unsigned __int64 *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, unsigned __int64 *data, MEM_FORMAT format, int count /* 1 */)
{
	char symbol[16];
	sprintf(symbol, "%08x", address);
	return MemoryDataIs(symbol, data, format, count);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, char *data, MEM_FORMAT format, int count /* 1 */)
// Description: Determine if the data at the specified address is the same as the data given.
// Return: A Boolean value that indicates whether the data at the specified address is the same as the data given (TRUE) or not.
// Param: symbol A pointer that contains the symbol in memory to compare with.
// Param: data A pointer to a buffer that contains the data to compare against.
// Param: count An integer that contains the number of elements of data to compare. (Default value is 1.)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, char *data, MEM_FORMAT format, int count /* 1 */)
	{
	char *memdata = new char[count];
	EXPECT(GetMemoryData(symbol, memdata, format, count));

	// If length of data is less than count we need to limit count
	if(count > (int) strlen(data))
		count = (int) strlen(data);

//	for (int i = 0; i < count; i++)
//		if(data[i] != memdata[i])
//			break;
	BOOL bRes = (0 == _strnicmp(data, memdata, count));

	delete[] memdata;
//	return (i == count);
	return bRes;
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned char *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned char *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	unsigned char *memdata = new unsigned char[count];
	EXPECT(GetMemoryData(symbol, memdata, format, count));

	// If length of data is less than count we need to limit count
	if(count > (int) strlen((char*) data))
		count = (int) strlen((char*) data);

	for (int i = 0; i < count; i++)
		if(data[i] != memdata[i])
			break;

	delete[] memdata;
	return (i == count);
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, short int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, short int *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	short int *memdata = new short int[count];
	EXPECT(GetMemoryData(symbol, memdata, format, count));

	for (int i = 0; i < count; i++)
		if(data[i] != memdata[i])
			break;

	delete[] memdata;
	return (i == count);
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned short int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned short int *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	unsigned short int *memdata = new unsigned short int[count];
	EXPECT(GetMemoryData(symbol, memdata, format, count));

	for (int i = 0; i < count; i++)
		if(data[i] != memdata[i])
			break;

	delete[] memdata;
	return (i == count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, int *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	int *memdata = new int[count];
	EXPECT(GetMemoryData(symbol, memdata, format, count));

	for (int i = 0; i < count; i++)
		if(data[i] != memdata[i])
			break;

	delete[] memdata;
	return (i == count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned int *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	unsigned int *memdata = new unsigned int[count];
	EXPECT(GetMemoryData(symbol, memdata, format, count));

	for (int i = 0; i < count; i++)
		if(data[i] != memdata[i])
			break;

	delete[] memdata;
	return (i == count);
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, long int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, long int *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	long int *memdata = new long int[count];
	EXPECT(GetMemoryData(symbol, memdata, format, count));

	for (int i = 0; i < count; i++)
		if(data[i] != memdata[i])
			break;

	delete[] memdata;
	return (i == count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned long int *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned long int *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	unsigned long int *memdata = new unsigned long int[count];
	EXPECT(GetMemoryData(symbol, memdata, format, count));

	for (int i = 0; i < count; i++)
		if(data[i] != memdata[i])
			break;

	delete[] memdata;
	return (i == count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, float *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, float *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	float *memdata = new float[count];
	EXPECT(GetMemoryData(symbol, memdata, format, count));

	char str1[128], str2[128];

	// Since comparing floats doesn't give correct results if one of them is a result of atof
	// we compare strings

	for (int i = 0; i < count; i++)
	{
		memset(str1, 0, sizeof(str1));
		memset(str2, 0, sizeof(str2));

		sprintf(str1, "%.5e", data[i]);
		sprintf(str2, "%.5e", memdata[i]);
		if(strcmp(str1,str2))
			break;
	}

	delete[] memdata;
	return (i == count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, double *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, double *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	double *memdata = new double[count];
	EXPECT(GetMemoryData(symbol, memdata, format, count));

	char str1[128], str2[128];

	// Since comparing doubles doesn't give correct results if one of them is a result of atof
	// we compare strings

	for (int i = 0; i < count; i++)
	{
		memset(str1, 0, sizeof(str1));
		memset(str2, 0, sizeof(str2));

		sprintf(str1, "%.14e", data[i]);
		sprintf(str2, "%.14e", memdata[i]);
		if(strcmp(str1,str2))
			break;
	}

	delete[] memdata;
	return (i == count);
	}


// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, long double *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, long double *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	long double *memdata = new long double[count];
	EXPECT(GetMemoryData(symbol, memdata, format, count));

	char str1[128], str2[128];

	// Since comparing long doubles doesn't give correct results if one of them is a result of atof
	// we compare strings

	for (int i = 0; i < count; i++)
	{
		memset(str1, 0, sizeof(str1));
		memset(str2, 0, sizeof(str2));

		sprintf(str1, "%.14e", data[i]);
		sprintf(str2, "%.14e", memdata[i]);
		if(strcmp(str1,str2))
			break;
	}

	delete[] memdata;
	return (i == count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, __int64 *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, __int64 *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	__int64 *memdata = new __int64[count];
	EXPECT(GetMemoryData(symbol, memdata, format, count));

	for (int i = 0; i < count; i++)
		if(data[i] != memdata[i])
			break;

	delete[] memdata;
	return (i == count);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned __int64 *data, MEM_FORMAT format, int count /* 1 */)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned __int64 *data, MEM_FORMAT format, int count /* 1 */)
	
	{
	unsigned __int64 *memdata = new unsigned __int64[count];
	EXPECT(GetMemoryData(symbol, memdata, format, count));

	for (int i = 0; i < count; i++)
		if(data[i] != memdata[i])
			break;

	delete[] memdata;
	return (i == count);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, char data, MEM_FORMAT format)
// Description: Determine if the data at the specified address is the same as the data given.
// Return: A Boolean value that indicates whether the data at the specified address is the same as the data given (TRUE) or not.
// Param: address An integer that contains the address in memory to compare with.
// Param: data A char that contains the data to compare against.
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, char data, MEM_FORMAT format)
	{
	return MemoryDataIs(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, unsigned char data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, unsigned char data, MEM_FORMAT format)
	{
	return MemoryDataIs(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, short int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, short int data, MEM_FORMAT format)
	{
	return MemoryDataIs(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, unsigned short int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, unsigned short int data, MEM_FORMAT format)
	{
	return MemoryDataIs(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, int data, MEM_FORMAT format)
	{
	return MemoryDataIs(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, unsigned int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, unsigned int data, MEM_FORMAT format)
	{
	return MemoryDataIs(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, long int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, long int data, MEM_FORMAT format)
	{
	return MemoryDataIs(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, unsigned long int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, unsigned long int data, MEM_FORMAT format)
	{
	return MemoryDataIs(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, float data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, float data, MEM_FORMAT format)
	{
	return MemoryDataIs(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, double data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, double data, MEM_FORMAT format)
	{
	return MemoryDataIs(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, long double data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, long double data, MEM_FORMAT format)
	{
	return MemoryDataIs(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, __int64 data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, __int64 data, MEM_FORMAT format)
	{
	return MemoryDataIs(address, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(int address, unsigned __int64 data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(int address, unsigned __int64 data, MEM_FORMAT format)
	{
	return MemoryDataIs(address, &data, format);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, char data, MEM_FORMAT format)
// Description: Determine if the data at the specified address is the same as the data given.
// Return: A Boolean value that indicates whether the data at the specified address is the same as the data given (TRUE) or not.
// Param: symbol A pointer that contains the symbol in memory to compare with.
// Param: data A char that contains the data to compare against.
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, char data, MEM_FORMAT format)
	{
	return MemoryDataIs(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned char data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned char data, MEM_FORMAT format)
	{
	return MemoryDataIs(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, short int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, short int data, MEM_FORMAT format)
	{
	return MemoryDataIs(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned short int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned short int data, MEM_FORMAT format)
	{
	return MemoryDataIs(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, int data, MEM_FORMAT format)
	{
	return MemoryDataIs(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned int data, MEM_FORMAT format)
	{
	return MemoryDataIs(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, long int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, long int data, MEM_FORMAT format)
	{
	return MemoryDataIs(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned long int data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned long int data, MEM_FORMAT format)
	{
	return MemoryDataIs(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, float data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, float data, MEM_FORMAT format)
	{
	return MemoryDataIs(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, double data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, double data, MEM_FORMAT format)
	{
	return MemoryDataIs(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, long double data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, long double data, MEM_FORMAT format)
	{
	return MemoryDataIs(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, __int64 data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, __int64 data, MEM_FORMAT format)
	{
	return MemoryDataIs(symbol, &data, format);
	}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned __int64 data, MEM_FORMAT format)
// END_HELP_COMMENT
BOOL COMemory::MemoryDataIs(LPCSTR symbol, unsigned __int64 data, MEM_FORMAT format)
	{
	return MemoryDataIs(symbol, &data, format);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetMemoryFormat(MEM_FORMAT format)	
// Description: Set the format of the memory window. 
// Return: A Boolean value that indicates success. TRUE if successful; FALSE otherwise.
// Param: format A MEM_FORMAT value specifying the format of the memory window: MEM_FORMAT_ASCII, MEM_FORMAT_WCHAR, 
//	MEM_FORMAT_BYTE, MEM_FORMAT_SHORT, MEM_FORMAT_SHORT_HEX, MEM_FORMAT_SHORT_UNSIGNED, MEM_FORMAT_LONG, MEM_FORMAT_LONG_HEX, 
//	MEM_FORMAT_LONG_UNSIGNED, MEM_FORMAT_REAL, MEM_FORMAT_REAL_LONG, MEM_FORMAT_INT64, MEM_FORMAT_INT64_HEX, MEM_FORMAT_INT64_UNSIGNED
// END_HELP_COMMENT

BOOL COMemory::SetMemoryFormat(MEM_FORMAT format)	
{

	//Activate memory window
	if (TRUE != uimem.Activate())
	{
		LOG->RecordInfo("ERROR. Can Not Activate Memory Window");
		return FALSE;
	}

	//Set format
	//uimem.SetMemoryFormat(format);
	if(!UIWB.SetMemoryFormat(format))
		return FALSE;
	else
	{
		uimem.m_MemoryFormat = format;
		return TRUE;
	}
}


// BEGIN_HELP_COMMENT
// Function: CString COMemory::GetColumns(void)
// Description: Gets number of columns in memory window
// Param: No Params
// Return: CString Number of columns
// END_HELP_COMMENT
CString COMemory::GetColumns(void)
{	
	//Activate memory window
	if (TRUE != uimem.Activate())
	{
		LOG->RecordInfo("ERROR. Can Not Activate Memory Window");
		return "";
	}

	return uimem.GetColumns();
}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::SetColumns(CString Columns)
// Description: Sets number of columns in memory window
// Param: Columns A number of columns or "<Auto>"
// Return: A Boolean value that indicates whether the set column operatin was sucessfull (TRUE) or not.
// END_HELP_COMMENT
BOOL COMemory::SetColumns(CString Columns)
{
	//Activate memory window
	if (TRUE != uimem.Activate())
	{
		LOG->RecordInfo("ERROR. Can Not Activate Memory Window");
		return FALSE;
	}

	return uimem.SetColumns(Columns);
}


// BEGIN_HELP_COMMENT
// Function: CString COMemory::GetAddress(CString symbol)
// Description: Returns the adddress of a symbol
// Param: symbol A symbol whoes address should be returned
// Return: CString The address of the symbol
// END_HELP_COMMENT
CString COMemory::GetAddress(CString symbol)
{
	//Activate memory window
	if (TRUE != uimem.Activate())
	{
		LOG->RecordInfo("ERROR. Can Not Activate Memory Window");
		return CString("");
	}
	if(ERROR_SUCCESS != uimem.GoToAddress(symbol))
	{
		LOG->RecordInfo("ERROR. Can Not Go To Address");
		return CString("");
	}

	return uimem.GetCurrentAddress();
}

// BEGIN_HELP_COMMENT
// Function: BOOL COMemory::CycleMemoryFormat(int count)
// Description: Cycles though memory formats
// Param: count A number of "Next Format" changes that will be done 
// Return: A Boolean value that indicates whether the cycle operatin was sucessfull (TRUE) or not..
// END_HELP_COMMENT

BOOL COMemory::CycleMemoryFormat(int count)
{
	return uimem.CycleMemoryFormat(count);
}


#pragma warning (default : 4069)




