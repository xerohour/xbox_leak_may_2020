#pragma once


class CStringObj
{
public:
	virtual ~CStringObj();
	explicit CStringObj();
/*	CStringObj(TCHAR* pBuf, int len);
	CStringObj(TCHAR* pBuf);
	CStringObj(char* pBuf);
	CStringObj(char* pBuf, int len);
*/
	CStringObj(const TCHAR* pBuf, int len);
	CStringObj(const TCHAR* pBuf);
	CStringObj(const char* pBuf);
	CStringObj(const char* pBuf, int len);


	CStringObj(const CStringObj& Obj);

	CStringObj& operator = (const CStringObj& Obj);
	
	friend bool operator < (const CStringObj& Obj1,const CStringObj& Obj2)
	{
		if(!Obj1.m_pBuf && !Obj2.m_pBuf)
			return false;
		if(!Obj1.m_pBuf && Obj2.m_pBuf)
			return true;
		if(Obj1.m_pBuf && !Obj2.m_pBuf)
			return false;

		if(_tcsicmp(Obj1.m_pBuf, Obj2.m_pBuf) >= 0)
			return false;
		if(_tcsicmp(Obj1.m_pBuf, Obj2.m_pBuf) < 0)
			return true;
		return false;

	}
	
	friend bool operator >= (const CStringObj& Obj1,const CStringObj& Obj2)
	{
		return !(Obj1 < Obj2);
		
	}
	size_t length();
	bool empty();
	bool CompareNoCase(const CStringObj& Obj);
	const TCHAR* c_str();
	void Cleanup();
private:
	TCHAR* m_pBuf;
	
};



