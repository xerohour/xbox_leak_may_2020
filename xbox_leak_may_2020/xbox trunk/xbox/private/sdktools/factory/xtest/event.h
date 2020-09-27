// uutmsg.h

class CStore; // declared in store.h

class CUutMsg
{
public:
	int Type;
	CUutMsg (int t):Type(t) {}
	virtual CStore& Store (CStore& s) {return s;} // << Type;}
	virtual CStore& Load (CStore& s) {return s;} // Type already read
};

struct CMsg
{
	struct TestingStarted: public CUutMsg
	{
		TestingStarted ():CUutMsg(MID_UUT_STARTING) {}
		TestingStarted (CStore &s):CUutMsg(MID_UUT_STARTING) {Load (s);}
	};

	struct TestingEnded: public CUutMsg
	{
		TestingEnded ():CUutMsg(MID_UUT_ENDING) {}
		TestingEnded (CStore &s):CUutMsg(MID_UUT_ENDING) {Load (s);}
	};

	struct TestingActive: public CUutMsg
	{
		TestingActive ():CUutMsg(MID_UUT_ACTIVE) {}
		TestingActive (CStore &s):CUutMsg(MID_UUT_ACTIVE) {Load (s);}
	};

	struct TestStarted: public CUutMsg
	{
		LPCTSTR Name;

		TestStarted ():CUutMsg(MID_UUT_TESTSTART) {}
		TestStarted (CStore &s):CUutMsg(MID_UUT_TESTSTART) {Load (s);}
		TestStarted (LPCTSTR n):CUutMsg(MID_UUT_TESTSTART),Name(n) {}
		CStore& Store (CStore& s) {return CUutMsg::Store(s) << Name;}
		CStore& Load  (CStore& s) {return CUutMsg::Load(s) >> &Name;}
	};

	struct TestEnded: public CUutMsg
	{
		LPCWSTR Name;
		DWORD Elapsed;
		BOOL Result;

		TestEnded ():CUutMsg(MID_UUT_TESTEND) {}
		TestEnded (CStore &s):CUutMsg(MID_UUT_TESTEND) {Load (s);}
		TestEnded (LPCWSTR n, DWORD e, BOOL r):CUutMsg(MID_UUT_TESTEND),
			Name(n), Elapsed(e), Result(r) {}
		CStore& Store (CStore& s) {return CUutMsg::Store(s) << Name << Elapsed << Result;}
		CStore& Load  (CStore& s) {return CUutMsg::Load(s) >> &Name >> Elapsed >> Result;}
	};

	struct Error: public CUutMsg
	{
		LPCWSTR Name;
		DWORD ErrorCode;
		LPCWSTR Message;

		Error ():CUutMsg(MID_UUT_ERROR) {}
		Error (CStore &s):CUutMsg(MID_UUT_ERROR) {Load (s);}
		Error (LPCWSTR n, DWORD e, LPCWSTR m):CUutMsg(MID_UUT_ERROR),
			Name(n), ErrorCode(e), Message(m) {}
		CStore& Store (CStore& s) {return CUutMsg::Store(s) << Name << ErrorCode << Message;}
		CStore& Load  (CStore& s) {return CUutMsg::Load(s) >> &Name >> ErrorCode >> &Message;}
	};

	struct Warning: public CUutMsg
	{
		LPCWSTR Name;
		LPCWSTR Message;

		Warning ():CUutMsg(MID_UUT_WARNING)  {}
		Warning (CStore &s):CUutMsg(MID_UUT_WARNING) {Load (s);}
		Warning (LPCWSTR n, LPCWSTR m):CUutMsg(MID_UUT_WARNING),
			Name(n), Message(m) {}
		CStore& Store (CStore& s) {return CUutMsg::Store(s) << Name << Message;}
		CStore& Load  (CStore& s) {return CUutMsg::Load(s) >> &Name >> &Message;}
	};

	struct Debug: public CUutMsg
	{
		LPCWSTR Name;
		LPCWSTR Message;

		Debug ():CUutMsg(MID_UUT_DEBUG) {}
		Debug (CStore &s):CUutMsg(MID_UUT_DEBUG) {Load (s);}
		Debug (LPCWSTR n, LPCWSTR m):CUutMsg(MID_UUT_DEBUG),
			Name(n), Message(m) {}
		CStore& Store (CStore& s) {return CUutMsg::Store(s) << Name << Message;}
		CStore& Load  (CStore& s) {return CUutMsg::Load(s) >> &Name >> &Message;}
	};

	struct Statistic: public CUutMsg
	{
		LPCWSTR Name;
		LPCWSTR Key;
		LPCWSTR Message;

		Statistic ():CUutMsg(MID_UUT_STAT) {}
		Statistic (CStore &s):CUutMsg(MID_UUT_STAT) {Load (s);}
		Statistic (LPCWSTR n, LPCWSTR k, LPCWSTR m):CUutMsg(MID_UUT_STAT),
			Name(n), Key(k), Message(m) {}
		CStore& Store (CStore& s) {return CUutMsg::Store(s) << Name << Key << Message;}
		CStore& Load  (CStore& s) {return CUutMsg::Load(s) >> &Name >> &Key >> &Message;}
	};

	static CUutMsg *CreateMsg (DWORD mid, CStore &store)
	{
		switch (mid)
		{
		case MID_UUT_STARTING:	return new TestingStarted (store);
		case MID_UUT_ACTIVE:	return new TestingActive (store);
		case MID_UUT_ENDING:		return new TestingEnded (store);
		case MID_UUT_TESTSTART:		return new TestStarted (store);
		case MID_UUT_TESTEND:		return new TestEnded (store);
		case MID_UUT_ERROR:			return new Error (store);
		case MID_UUT_WARNING:			return new Warning (store);
		case MID_UUT_DEBUG:				return new Debug (store);
		case MID_UUT_STAT:				return new Statistic (store);
		}

		return NULL;
	}

};
