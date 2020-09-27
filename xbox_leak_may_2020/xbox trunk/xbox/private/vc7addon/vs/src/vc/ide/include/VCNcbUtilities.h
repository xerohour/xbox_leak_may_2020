#pragma once

template<typename T>
class CNCBArray
{
private :

	BscEx * m_pBscEx;
	T * m_rgT;

public :

	CNCBArray(BscEx * pBscEx) : m_pBscEx(pBscEx), m_rgT(NULL) { }
	~CNCBArray(void) { m_pBscEx->disposeArray(m_rgT); }

	operator T * (void) { return m_rgT; }
	T ** operator & (void) { VCASSERT(!m_rgT); return &m_rgT; }
	T & operator [] (INT iIndex) { VCASSERT(m_rgT); return m_rgT[iIndex]; }
	T & operator [] (UINT iIndex) { VCASSERT(m_rgT); return m_rgT[iIndex]; }
};

