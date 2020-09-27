// timer.h - generic timing algorithm

class CTimeable
{
public:
	virtual void OnTimer () = 0;
};

class CTimer
{
	CTimeable *m_targetObj;
	HANDLE m_timerAbort;
	HANDLE m_timerThread;
	int m_milliseconds;
	__int64 m_i64ExpireTSCValue;

	static unsigned __stdcall TimerEntry (LPVOID pParam);
	void TimerMain ();

public:
	CTimer (CTimeable *obj, size_t seconds);
	~CTimer ();
};
