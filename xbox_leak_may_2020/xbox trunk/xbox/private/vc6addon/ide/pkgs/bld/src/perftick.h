// CPerfTicker
// for performance timings

#ifndef __PERFTICK_H__
#define __PERFTICK_H__

#ifdef DOPERFTICK
class CPerfTicker
{
public:
	CPerfTicker(const char * pchMsg)
	{
		VERIFY(QueryPerformanceCounter((LARGE_INTEGER*)&m_liStart));
		m_pchMsg = pchMsg;
	}

	~CPerfTicker()
	{
		VERIFY(QueryPerformanceCounter((LARGE_INTEGER*)&m_liEnd));
		m_liDiff = m_liEnd - m_liStart; //LargeIntegerSubtract(m_liEnd, m_liStart);
		TRACE("CPerfTicker: '%s' is %lu ticks\n", m_pchMsg, (unsigned long)m_liDiff);
	}

private:
	__int64 m_liStart, m_liEnd, m_liDiff;
	//LARGE_INTEGER m_liStart, m_liEnd, m_liDiff;
	const char * m_pchMsg;
};
#else
class CPerfTicker
{
public:
	CPerfTicker(const char * pchMsg)
	{
	}

	~CPerfTicker()
	{
	}
};

#endif

#endif // __PERFTICK_H__
