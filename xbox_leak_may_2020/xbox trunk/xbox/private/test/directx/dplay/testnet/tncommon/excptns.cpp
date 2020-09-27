//==================================================================================
// Includes
//==================================================================================
#include <windows.h>

#include "main.h"
#include "debugprint.h"
#include "cppobjhelp.h"
#include "linklist.h"
#ifdef TNCOMMON_EXPORTS
#include "symbols.h"
#endif // TNCOMMON_EXPORTS
#include "excptns.h"





//==================================================================================
// Exported Globals
//==================================================================================
CTNExceptionsList		g_TNExceptions;






#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNException::CTNException"
//==================================================================================
// CTNException constructor
//----------------------------------------------------------------------------------
//
// Description: Creates the exception object with the specified properties.
//
// Arguments:
//	LPCONTEXT lpContext					?
//	DWORD dwExceptionCode				?
//	DWORD dwExceptionFlags				?
//	PVOID pvExceptionAddress			?
//	DWORD dwNumberParameters			?
//	DWORD* padwExceptionInformation		?
//
// Returns: None (just the object).
//==================================================================================
CTNException::CTNException(LPCONTEXT lpContext,
					DWORD dwExceptionCode,
					DWORD dwExceptionFlags,
					PVOID pvExceptionAddress,
					DWORD dwNumberParameters,
					DWORD_PTR* padwExceptionInformation) :
		m_dwExceptionCode(dwExceptionCode),
		m_dwExceptionFlags(dwExceptionFlags),
		m_pvExceptionAddress(pvExceptionAddress),
		m_dwNumberParameters(dwNumberParameters)
{
	CopyMemory(&(this->m_context), lpContext, sizeof (CONTEXT));

	ZeroMemory(this->m_adwExceptionInformation,
				(EXCEPTION_MAXIMUM_PARAMETERS * sizeof (DWORD)));
	CopyMemory(this->m_adwExceptionInformation,
				padwExceptionInformation,
				dwNumberParameters * sizeof (DWORD));
} // CTNException::CTNException
#undef DEBUG_SECTION
#define DEBUG_SECTION	""



/*
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNException::GetDescription"
//==================================================================================
// CTNException::GetDescription
//----------------------------------------------------------------------------------
//
// Description: Destroys the exception object.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNException::GetDescription(void)
{
	return (S_OK);
} // CTNException::GetDescription
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
*/



#undef DEBUG_SECTION
#define DEBUG_SECTION	"CTNException::PrintSelf"
//==================================================================================
// CTNException::PrintSelf
//----------------------------------------------------------------------------------
//
// Description: Prints this object's properties to the DPL macro at level 0.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CTNException::PrintSelf(void)
{
	DPL(0, "%X", 1, this->m_pvExceptionAddress);

	switch (this->m_dwExceptionCode)
	{
		case EXCEPTION_ACCESS_VIOLATION:
			DPL(0, "Thread attempted to %s the address %X!",
					2, ((this->m_adwExceptionInformation[0] == 0) ? "read from" : "write to"),
					this->m_adwExceptionInformation[1]);
		  break;

		default:
			DWORD	dwTemp;


			for(dwTemp = 0; dwTemp < this->m_dwNumberParameters; dwTemp++)
			{
				DPL(0, "adwExceptionInformation[%u] = %X",
						2, dwTemp, this->m_adwExceptionInformation[dwTemp]);
			} // end for (each parameter)
		  break;
	} // end switch (on exception code)

#ifdef _X86_
	DPL(0, "eip = %X", 1, this->m_context.Eip);
	DPL(0, "ebp = %X", 1, this->m_context.Ebp);
	DPL(0, "esp = %X", 1, this->m_context.Esp);
	DPL(0, "eax = %X", 1, this->m_context.Eax);
	DPL(0, "ebx = %X", 1, this->m_context.Ebx);
	DPL(0, "ecx = %X", 1, this->m_context.Ecx);
	DPL(0, "edx = %X", 1, this->m_context.Edx);
#endif // _X86_

	return (S_OK);
} // CTNException::PrintSelf
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNExcptnConvertToClassFilter()"
//==================================================================================
// TNExcptnConvertToClassFilter
//----------------------------------------------------------------------------------
//
// Description: Filter to convert exceptions into a CTNException object.
//
// Arguments:
//	PEXCEPTION_POINTERS pExceptionInfo	Pointer to exception record(s).
//	PTNEXCEPTIONSLIST pExceptionsList	Pointer to list to add records to.
//
// Returns: Always returns EXCEPTION_EXECUTE_HANDLER.
//==================================================================================
int TNExcptnConvertToClassFilter(PEXCEPTION_POINTERS pExceptionInfo,
								PTNEXCEPTIONSLIST pExceptionsList)
{
	HRESULT				hr;
	PEXCEPTION_RECORD	pExceptionRecord = pExceptionInfo->ExceptionRecord;
	PTNEXCEPTION		pException = NULL;


	while (pExceptionRecord != NULL)
	{
		pException = new (CTNException)(pExceptionInfo->ContextRecord,
										pExceptionRecord->ExceptionCode,
										pExceptionRecord->ExceptionFlags,
										pExceptionRecord->ExceptionAddress,
										pExceptionRecord->NumberParameters,
										pExceptionRecord->ExceptionInformation);

		// Add the new exception to the front of the list
		hr = pExceptionsList->AddAfter(pException, NULL);
		if (hr != S_OK)
		{
			DPL(0, "Couldn't add new exception to list!  %e", 1, hr);
			delete (pException);
			pException = NULL;
		} // end if (couldn't add item to list)

		pExceptionRecord = pExceptionRecord->ExceptionRecord;
	} // end while (there are more exceptions to add)

	return (EXCEPTION_EXECUTE_HANDLER);
} // TNConvertExceptionsFilter
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
