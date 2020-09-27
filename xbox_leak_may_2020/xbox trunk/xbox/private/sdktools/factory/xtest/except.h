// except.h

#pragma once

typedef unsigned int WIN32_EXCEPTION;
typedef void * INSTRUCTION_ADDRESS;

class CStructuredException
{
private:
	CStructuredException () {}
	WIN32_EXCEPTION m_exceptionCode;
	_EXCEPTION_POINTERS* m_pEP;

public:
	CStructuredException (WIN32_EXCEPTION ecode):m_exceptionCode(ecode) {}
	CStructuredException (WIN32_EXCEPTION ecode, _EXCEPTION_POINTERS* pEP)
		:m_exceptionCode(ecode), m_pEP(pEP) {}
	~CStructuredException () {}

	WIN32_EXCEPTION GetExceptionCode () {return m_exceptionCode;}
	LPCTSTR GetExceptionString ();
	INSTRUCTION_ADDRESS GetExceptionAddress ();
	LPCTSTR GetExceptionStack ();
};

void CustomExceptionTranslator (WIN32_EXCEPTION, _EXCEPTION_POINTERS*);

class CXmtaException
{
private:
	WORD m_error;
	LPCTSTR m_message;
	_TCHAR m_buffer [1024];
public:
	CXmtaException (WORD error, LPCTSTR fomat, ...);
	CXmtaException (const CXmtaException &x);
	CXmtaException ();
	~CXmtaException () {}

	WORD GetErrorCode () {return m_error;}
	LPCTSTR GetMessage () {return m_message;}
};

