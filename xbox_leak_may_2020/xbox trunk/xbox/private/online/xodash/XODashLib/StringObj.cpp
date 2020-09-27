#include "std.h"
#include "StringObj.h"
#include "utilities.h"



CStringObj::CStringObj():m_pBuf (NULL)
{
}
CStringObj::CStringObj(const TCHAR* pBuf, int len)
{
	ASSERT(pBuf);
	m_pBuf = new TCHAR[len+1];
	ASSERT(m_pBuf);
	if (m_pBuf == NULL)
	{
		DbgPrint("CStringObj::CStringObj - OUTOFMEMORY");
	}
	_tcsncpy(m_pBuf,pBuf, len);
	m_pBuf[len] = '\0';
}

CStringObj::CStringObj(const TCHAR* pBuf)
{
	ASSERT(pBuf);
	m_pBuf = new TCHAR[_tcslen(pBuf)+1];
	
	if (m_pBuf)
	{
		_tcscpy(m_pBuf,pBuf);
	}
	
}

CStringObj::CStringObj(const char* pBuf, int nLen)
{
	ASSERT(pBuf);
	m_pBuf = new TCHAR[nLen+1];
	
	ASSERT(m_pBuf);
	if (m_pBuf == NULL)
	{
		DbgPrint("CStringObj::CStringObj - OUTOFMEMORY");
	}
	Unicode(m_pBuf,pBuf, nLen);
	m_pBuf[nLen] = '\0';
}

CStringObj::CStringObj(const char* pBuf)
{
	ASSERT(pBuf);
	int nLen = strlen(pBuf)+1;
	m_pBuf = new TCHAR[nLen];
	
	if (m_pBuf)
	{
		Unicode(m_pBuf,pBuf, nLen);
	}
	
}

CStringObj::CStringObj(const CStringObj& Obj)
{
	if(	Obj.m_pBuf)
	{
		m_pBuf = new TCHAR[_tcslen(Obj.m_pBuf)+1];
		if(m_pBuf)
		{
			_tcscpy(m_pBuf,Obj.m_pBuf);
		}
	}
}

const TCHAR* CStringObj::c_str()
{
	return m_pBuf;
}


void CStringObj::Cleanup()
{
	delete [] m_pBuf;
	m_pBuf = NULL;
}


CStringObj& CStringObj::operator = (const CStringObj& Obj)
{
	if(Obj.m_pBuf != this->m_pBuf)
	{
		Cleanup();
	}

	if(Obj.m_pBuf)
	{
		m_pBuf = new TCHAR[_tcslen(Obj.m_pBuf)+1];
		if(m_pBuf)
		{
			_tcscpy(m_pBuf, Obj.m_pBuf);
		}
	}
	return *this;
}

CStringObj::~CStringObj()
{
	Cleanup();
}

size_t CStringObj::length()
{
	if(m_pBuf)
	{
		return _tcslen(m_pBuf);
	}
	return 0;
}

bool CStringObj::empty()
{
	if(m_pBuf==NULL)
	{
		return true;
	}
	return false;
}

bool CStringObj::CompareNoCase(const CStringObj& Obj)
{
	return false;
}