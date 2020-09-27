// store.cpp

#include "stdafx.h"

CStore::CStore (int size)
{
	m_next = m_buffer = new BYTE [size];
	m_size = size;
	m_delete = true;
}

CStore::CStore (LPCBYTE buffer, int size)
{
	m_next = m_buffer = const_cast<LPBYTE> (buffer);
	m_size = size;
	m_delete = false;
}
	
void CStore::Put (const void* bp, int ln) 
{
	if ((m_buffer + m_size) < (m_next + ln))
		throw m_size;

//	ASSERT (!m_delete);
	memcpy (m_next, bp, ln);
	m_next += ln;
}

void CStore::Get (void *bp, int ln)
{
	if ((m_buffer + m_size) < (m_next + ln))
		throw m_size;

	memcpy (bp, m_next, ln);
	m_next += ln;
}

LPCTSTR CStore::GetStringPtr (void)
{
	LPCTSTR str = (LPCTSTR) m_next;
	while (m_next < (m_buffer + m_size))
	{
		_TCHAR current = *(_TCHAR*)m_next;
		m_next += sizeof (_TCHAR);
		if (current == 0)
			return str;
	}
	throw m_size;
}



