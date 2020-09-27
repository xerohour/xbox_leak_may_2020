//
// Performance Counter classes to snapshot memory
//
// Winthrop Chan (winthc)
// March 22, 2001
//
#include <pdh.h>

class PerfCounter
{
private:
	HQUERY			m_hQuery;
	HCOUNTER		m_hCounter;
	bool			m_firsttime;

	unsigned long	m_ticks;
	long			m_minCounter;
	long			m_maxCounter;
	long			m_curCounter;
	ULONGLONG		m_totalCounter;
	ULONGLONG		m_avgCounter;

public:
	PerfCounter(char *strCounter = "\\process(msdev)\\Working Set")
	{
		PDH_STATUS	pdhStatus;

		m_ticks        = 0;
		m_firsttime    = true;
		m_minCounter   = 0;
		m_curCounter   = 0;
		m_maxCounter   = 0;
		m_totalCounter = 0;
		m_avgCounter   = 0;

		pdhStatus = PdhOpenQuery (0, 0, &m_hQuery);

		pdhStatus = PdhAddCounter (m_hQuery, strCounter, 0, &m_hCounter);
	}

	~PerfCounter()
	{
		PdhCloseQuery (m_hQuery);
	}

	void LogData()
	{
		PDH_STATUS				pdhStatus;
		DWORD					ctype;
		PDH_FMT_COUNTERVALUE	fmtValue;

		pdhStatus = PdhCollectQueryData (m_hQuery);
		pdhStatus = PdhGetFormattedCounterValue(
			m_hCounter, PDH_FMT_LONG, // PDH_FMT_DOUBLE, 
			&ctype, 
			&fmtValue); 

		m_ticks++;
		if (m_firsttime)
		{
			m_minCounter    = fmtValue.longValue;
			m_maxCounter    = fmtValue.longValue;
			m_firsttime     = false;
		}
		else
		{
			m_minCounter    = min(m_minCounter, fmtValue.longValue);
			m_maxCounter    = max(m_maxCounter, fmtValue.longValue);
		}


		m_curCounter    = fmtValue.longValue;
		m_totalCounter += fmtValue.longValue;
		m_avgCounter    = m_totalCounter / m_ticks;
	}

	long GetMin()
	{
		return m_minCounter;
	}

	long GetMax()
	{
		return m_maxCounter;
	}

	long GetCur()
	{
		return m_curCounter;
	}

	ULONGLONG GetAvg()
	{
		return m_avgCounter;
	}

	unsigned long GetTicks()
	{
		return m_ticks;
	}

//	pdhStatus = PdhAddCounter (hQuery, _T("\\process(msdev)\\private bytes"), 0, &hCounter[0]);
//	pdhStatus = PdhAddCounter (hQuery, _T("\\process(msdev)\\Working Set"), 0, &hCounter[1]);
//	pdhStatus = PdhAddCounter (hQuery, _T("\\process(msdev)\\page file bytes"), 0, &hCounter);
//	pdhStatus = PdhAddCounter (hQuery, _T("\\process(msdev)\\pool Paged bytes"), 0, &hCounter);
//	pdhStatus = PdhAddCounter (hQuery, _T("\\process(msdev)\\pool Nonpaged bytes"), 0, &hCounter);
};