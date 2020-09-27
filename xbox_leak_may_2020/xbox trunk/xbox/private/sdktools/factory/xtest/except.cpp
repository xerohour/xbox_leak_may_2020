// except.cpp

#include "stdafx.h"

CXmtaException::CXmtaException ()
{
	m_error = 0;
	m_message = NULL;
}

CXmtaException::CXmtaException (WORD error, LPCTSTR format, ...)
{
	m_error = error;

	va_list argptr;
	va_start (argptr, format);

	if (format) {
		_vsntprintf (m_buffer, 1024, format, argptr);
		m_message = m_buffer;
	}
	else {
		m_message = NULL;
	}
}

CXmtaException::CXmtaException (const CXmtaException &x)
{
	m_error = x.m_error;
	m_message = x.m_message;
	_tcscpy (m_buffer, x.m_buffer);
}



// Custom translation function, set by calling "_set_se_translator"
//
void CustomExceptionTranslator (WIN32_EXCEPTION n, _EXCEPTION_POINTERS* p)
{
	throw CStructuredException (n, p);
}

INSTRUCTION_ADDRESS CStructuredException::GetExceptionAddress ()
{
	return m_pEP->ExceptionRecord->ExceptionAddress;
}

LPCTSTR CStructuredException::GetExceptionString ()
{
#define EXCEPTION( x ) case EXCEPTION_##x: return _T(#x);
	
	switch (m_exceptionCode)
	{
		EXCEPTION( DATATYPE_MISALIGNMENT )
		EXCEPTION( BREAKPOINT )
		EXCEPTION( SINGLE_STEP )
		EXCEPTION( ARRAY_BOUNDS_EXCEEDED )
		EXCEPTION( FLT_DENORMAL_OPERAND )
		EXCEPTION( FLT_DIVIDE_BY_ZERO )
		EXCEPTION( FLT_INEXACT_RESULT )
		EXCEPTION( FLT_INVALID_OPERATION )
		EXCEPTION( FLT_OVERFLOW )
		EXCEPTION( FLT_STACK_CHECK )
		EXCEPTION( FLT_UNDERFLOW )
		EXCEPTION( INT_DIVIDE_BY_ZERO )
		EXCEPTION( INT_OVERFLOW )
		EXCEPTION( PRIV_INSTRUCTION )
		EXCEPTION( IN_PAGE_ERROR )
		EXCEPTION( ILLEGAL_INSTRUCTION )
		EXCEPTION( NONCONTINUABLE_EXCEPTION )
		EXCEPTION( STACK_OVERFLOW )
		EXCEPTION( INVALID_DISPOSITION )
		EXCEPTION( GUARD_PAGE )
		EXCEPTION( INVALID_HANDLE )
	}

	static _TCHAR szBuffer[512] = _T("Unknown Structured Exception");
	
	// ACCESS_VIOLATION is the only exception with additional information
	//
	if (m_exceptionCode == EXCEPTION_ACCESS_VIOLATION)
	{
		_stprintf (szBuffer,
			_T("ACCESS_VIOLATION %sing virtual address 0x%08lX"),
			m_pEP->ExceptionRecord->ExceptionInformation[0]? _T("read"):_T("writ"),
			m_pEP->ExceptionRecord->ExceptionInformation[1]);
	}
		
	return szBuffer;
}

/*  Test carefully before useing ...
void CStructuredException::GetCallStack (PCONTEXT pContext, LPTSTR buffer)
{
	_tcscat (buffer, _T("\nCall stack:"));
	
	DWORD pc = pContext->Eip;
	PDWORD pFrame, pPrevFrame;
	pFrame = (PDWORD) pContext->Ebp;
	
	buffer = _tcschr (buffer, 0);
	int done = FALSE;
	do
	{
		_stprintf (buffer, _T(" %08X"), pc);
		buffer = _tcschr (buffer, 0);
		
		pc = pFrame[1];
		
		pPrevFrame = pFrame;
		
		pFrame = (PDWORD)pFrame[0]; // precede to next higher frame on stack
		
		if ((DWORD)pFrame & 3)    // Frame pointer must be aligned on a
			break;                  // DWORD boundary.  Bail if not so.
		
		if (pFrame <= pPrevFrame)
			break;
		
		// Can two DWORDs be read from the supposed frame address?          
		if (IsBadWritePtr(pFrame, sizeof(PVOID)*2))
			break;
		
	} while (!done);
}
*/
