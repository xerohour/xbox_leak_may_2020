#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Runner.h"

START_NODE_FUN(CStrObject, CObject)
	NODE_FUN_IV(length)
	NODE_FUN_II(charCodeAt)
	NODE_FUN_SI(charAt)
	NODE_FUN_SS(concat)
	NODE_FUN(indexOf)
	NODE_FUN(lastIndexOf)
	NODE_FUN(substr)
	NODE_FUN(substring)
	NODE_FUN_SV(toLowerCase)
	NODE_FUN_SV(toUpperCase)
END_NODE_FUN()


CStrObject::CStrObject()
{
	m_obj = objString;
	m_nLength = 0;
	m_nAlloc = 0;
	m_sz = NULL;
}

CStrObject::CStrObject(const TCHAR* sz)
{
	if (sz == NULL)
		sz = _T("");

	m_obj = objString;
	m_nLength = _tcslen(sz);
	m_nAlloc = m_nLength + 1;
	m_sz = new TCHAR [m_nAlloc];
	CopyChars(m_sz, sz, m_nLength);
	m_sz[m_nLength] = '\0';
}

CStrObject::CStrObject(const TCHAR* pch, int cch)
{
	m_obj = objString;
	m_nLength = cch;
	m_nAlloc = cch + 1;
	m_sz = new TCHAR [m_nAlloc];
	CopyChars(m_sz, pch, cch);
	m_sz[cch] = '\0';
}

CStrObject::~CStrObject()
{
	delete [] m_sz;
}

CStrObject* CStrObject::ToStr()
{
	AddRef();
	return this;
}

CNumObject* CStrObject::ToNum()
{
	return new CNumObject(GetSz());
}

TCHAR* CStrObject::SetLength(int nLength)
{
	delete [] m_sz;

	m_nLength = nLength;
	m_nAlloc = nLength + 1;
	m_sz = new TCHAR [m_nAlloc];
	ZeroMemory(m_sz, m_nAlloc * sizeof (TCHAR));
	return m_sz;
}

void CStrObject::Append(const TCHAR* szAppend)
{
	int cchAppend = _tcslen(szAppend);

	if (m_nLength + cchAppend + 1 > m_nAlloc)
	{
		int nAllocNew = m_nLength + cchAppend + 1;
		TCHAR* szNew = new TCHAR [nAllocNew];
		m_nAlloc = nAllocNew;
		CopyChars(szNew, m_sz, m_nLength);
		delete [] m_sz;
		m_sz = szNew;
	}

	CopyChars(m_sz + m_nLength, szAppend, cchAppend);
	m_nLength += cchAppend;
	m_sz[m_nLength] = 0;
}

////////////////////////////////////////////////////////////////////////////
// Scriptable Methods...

int CStrObject::length()
{
	return m_nLength;
}

int CStrObject::charCodeAt(int index)
{
	if (index < 0 || index >= m_nLength)
		return -1; // REVIEW: Should be NaN!

	return m_sz[index];
}

CStrObject* CStrObject::charAt(int index)
{
	if (index < 0 || index >= m_nLength)
		return NULL; // REVIEW: Should be [undefined]!

	return new CStrObject(&m_sz[index], 1);
}

CStrObject* CStrObject::concat(const TCHAR* sz)
{
	int cch = _tcslen(sz);
	
	CStrObject* pNewStr = new CStrObject;
	pNewStr->m_nLength = m_nLength + cch;
	pNewStr->m_nAlloc = pNewStr->m_nLength + 1;
	pNewStr->m_sz = new TCHAR [pNewStr->m_nAlloc];
	CopyChars(pNewStr->m_sz, m_sz, m_nLength);
	CopyChars(pNewStr->m_sz + m_nLength, sz, cch);
	pNewStr->m_sz[pNewStr->m_nLength] = '\0';

	return pNewStr;
}

CObject* CStrObject::indexOf(CObject** rgparam, int nParam)
{
	if (nParam < 1 || nParam > 2)
	{
		g_pRunner->Error(_T("invalid number of parameters"));
		return NULL;
	}

	const TCHAR* szSubstring = rgparam[0]->ToStr()->GetSz();

	int nStartIndex = 0;
	if (nParam == 2)
	{
		nStartIndex = (int)rgparam[1]->ToNum()->m_nValue;

		if (nStartIndex < 0)
			nStartIndex = 0;
		else if (nStartIndex > m_nLength)
			nStartIndex = m_nLength;
	}

	int nRet = -1;

	if (nStartIndex < m_nLength)
	{
		const TCHAR* pch = _tcsstr(m_sz + nStartIndex, szSubstring);
		if (pch != NULL)
			nRet = nStartIndex + (int)(pch - m_sz);
	}

	return new CNumObject((float)nRet);
}

const TCHAR* strrstr(const TCHAR* sz, const TCHAR* szFind, int nStartIndex)
{
	int cchSz = _tcslen(sz);
	int cchFind = _tcslen(szFind);

	if (cchSz < cchFind)
		return NULL;

	const TCHAR* pch = sz + nStartIndex - cchFind;
	while (pch >= sz)
	{
		if (_tcsncmp(pch, szFind, cchFind) == 0)
			return pch;
		pch -= 1;
	}

	return pch;
}

CObject* CStrObject::lastIndexOf(CObject** rgparam, int nParam)
{
	if (nParam < 1 || nParam > 2)
	{
		g_pRunner->Error(_T("invalid number of parameters"));
		return NULL;
	}

	const TCHAR* szSubstring = rgparam[0]->ToStr()->GetSz();

	int nStartIndex = m_nLength;
	if (nParam == 2)
	{
		nStartIndex = (int)rgparam[1]->ToNum()->m_nValue;

		if (nStartIndex < 0)
			nStartIndex = 0;
		else if (nStartIndex > m_nLength)
			nStartIndex = m_nLength;
	}

	const TCHAR* pch = strrstr(m_sz, szSubstring, nStartIndex);
	int nRet = -1;
	if (pch != NULL)
		nRet = (int)(pch - m_sz);

	return new CNumObject((float)nRet);
}

CObject* CStrObject::substr(CObject** rgparam, int nParam)
{
	if (nParam < 1 || nParam > 2)
	{
		g_pRunner->Error(_T("invalid number of parameters"));
		return NULL;
	}

	int nStart = (int)rgparam[0]->ToNum()->m_nValue;

	int nLength = m_nLength - nStart;
	if (nParam == 2)
		nLength = (int)rgparam[1]->ToNum()->m_nValue;

	if (nStart > m_nLength)
		nStart = m_nLength;
	if (nLength <= 0)
		nLength = 0;
	else if (nStart + nLength > m_nLength)
		nLength = m_nLength - nStart;

	return new CStrObject(m_sz + nStart, nLength);
}

CObject* CStrObject::substring(CObject** rgparam, int nParam)
{
	if (nParam != 2)
	{
		g_pRunner->Error(_T("invalid number of parameters"));
		return NULL;
	}

	int nStart = (int)rgparam[0]->ToNum()->m_nValue;
	if (nStart < 0)
		nStart = 0;

	int nEnd = (int)rgparam[1]->ToNum()->m_nValue;
	if (nEnd < 0)
		nEnd = nStart;

	if (nStart > nEnd)
	{
		int t = nStart;
		nStart = nEnd;
		nEnd = t;
	}

	if (nStart > m_nLength)
		nStart = m_nLength;
	if (nEnd > m_nLength)
		nEnd = m_nLength;

	return new CStrObject(m_sz + nStart, nEnd - nStart);
}

CStrObject* CStrObject::toLowerCase()
{
	CStrObject* pStrObject = new CStrObject(m_sz, m_nLength);
#ifdef _WINDOWS
	CharLowerBuff(pStrObject->m_sz, pStrObject->m_nLength);
#else
	_tcslwr(pStrObject->m_sz);
#endif
	return pStrObject;
}

CStrObject* CStrObject::toUpperCase()
{
	CStrObject* pStrObject = new CStrObject(m_sz, m_nLength);
#ifdef _WINDOWS
	CharUpperBuff(pStrObject->m_sz, pStrObject->m_nLength);
#else
	_tcsupr(pStrObject->m_sz);
#endif
	return pStrObject;
}
