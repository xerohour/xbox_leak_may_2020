#include "std.h"
#include "xapp.h"
#include "Lerper.h"

CLerper* CLerper::c_pHead;

CLerper::CLerper(CObject* pObject, float* pValue, float nNewValue, float nInterval)
{
	ASSERT(pObject != NULL);
	ASSERT(pValue != NULL);
	ASSERT(nInterval != 0.0f);

	m_pObject = pObject;
	m_interval = nInterval;
	m_startTime = XAppGetNow();
	m_startValue = *pValue;
	m_endValue = nNewValue;
	m_pValue = pValue;

	m_pNext = c_pHead;
	c_pHead = this;
}

bool CLerper::Advance()
{
	float t = (float) (XAppGetNow() - m_startTime) / m_interval;

	if (t >= 1.0f)
	{
		*m_pValue = m_endValue;
		return false;
	}

	float t1 = 1.0f - t;

	*m_pValue = t1 * m_startValue + t * m_endValue;

	return true;
}

void CLerper::AdvanceAll()
{
	for (CLerper** ppLerper = &c_pHead; *ppLerper != NULL; )
	{
		CLerper* pLerper = *ppLerper;
		if (!pLerper->Advance())
		{
			*ppLerper = pLerper->m_pNext;
			delete pLerper;
		}
		else
		{
			ppLerper = &pLerper->m_pNext;
		}
	}
}

void CLerper::RemoveObject(CObject* pObject)
{
	for (CLerper** ppLerper = &c_pHead; *ppLerper != NULL; )
	{
		CLerper* pLerper = *ppLerper;
		if (pLerper->m_pObject == pObject)
		{
			*ppLerper = pLerper->m_pNext;
			delete pLerper;
		}
		else
		{
			ppLerper = &pLerper->m_pNext;
		}
	}
}
