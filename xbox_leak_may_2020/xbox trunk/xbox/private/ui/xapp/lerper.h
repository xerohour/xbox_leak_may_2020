#pragma once

class CLerper
{
public:
	CLerper(CObject* pObject, float* pValue, float nNewValue, float nInterval);

	float m_interval;
	XTIME m_startTime;
	float m_startValue;
	float m_endValue;
	float* m_pValue;
	CObject* m_pObject;

	bool Advance();

	static void RemoveObject(CObject* pObject);
	static void AdvanceAll();
	static CLerper* c_pHead;
	CLerper* m_pNext;
};
