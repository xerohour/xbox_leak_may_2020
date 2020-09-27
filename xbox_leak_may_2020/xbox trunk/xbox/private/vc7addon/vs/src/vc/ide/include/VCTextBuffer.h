#pragma once

#include <VCAssert.h>

// CVCTextBufferLock
//

class CVCTextBufferLock
{
private :

	CComPtr<IVsTextBuffer> m_spIVsTextBuffer;
	BufferLockFlags m_blf;
	bool m_bLocked;

public :

	CVCTextBufferLock(IVsTextBuffer * pIVsTextBuffer, BufferLockFlags blf = BLF_READ) 

	: m_spIVsTextBuffer(pIVsTextBuffer), 
	  m_blf(blf),
	  m_bLocked(false)

	{
		HRESULT hr(S_OK);

		if (SUCCEEDED(hr = m_spIVsTextBuffer->LockBufferEx(blf)))
		{
			m_bLocked = true;
		}

		VCASSERT(SUCCEEDED(hr));
	}

	~CVCTextBufferLock(void)
	{
		if (m_bLocked)
		{
			HRESULT hr(S_OK);
			hr = m_spIVsTextBuffer->UnlockBufferEx(m_blf);
			VCASSERT(SUCCEEDED(hr));
		}
	}
};

