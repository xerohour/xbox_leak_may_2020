#include "std.h"
#include "xapp.h"
#include "Node.h"

////////////////////////////////////////////////////////////////////////////

CIntArray::CIntArray()
{
	m_nAlloc = 0;
	m_nSize = 0;
	m_value = NULL;
}

CIntArray::~CIntArray()
{
	delete [] m_value;
}

void CIntArray::SetSize(int nSize)
{
	if (nSize > m_nAlloc)
	{
		int* newValue = new int [nSize];

		if (m_value != NULL)
		{
			CopyMemory(newValue, m_value, sizeof (int) * m_nSize);
			delete [] m_value;
		}

		m_value = newValue;
		m_nAlloc = nSize;
	}

	m_nSize = nSize;
}

////////////////////////////////////////////////////////////////////////////

CNumArray::CNumArray()
{
	m_nAlloc = 0;
	m_nSize = 0;
	m_value = NULL;
}

CNumArray::~CNumArray()
{
	delete [] m_value;
}

void CNumArray::SetSize(int nSize)
{
	if (nSize > m_nAlloc)
	{
		float* newValue = new float [nSize];

		if (m_value != NULL)
		{
			CopyMemory(newValue, m_value, sizeof (float) * m_nSize);
			delete [] m_value;
		}

		m_value = newValue;
		m_nAlloc = nSize;
	}

	m_nSize = nSize;
}

////////////////////////////////////////////////////////////////////////////

CVec2Array::CVec2Array()
{
	m_nAlloc = 0;
	m_nSize = 0;
	m_value = NULL;
}

CVec2Array::~CVec2Array()
{
	delete [] m_value;
}

void CVec2Array::SetSize(int nSize)
{
	if (nSize > m_nAlloc)
	{
		D3DXVECTOR2* newValue = new D3DXVECTOR2 [nSize];

		if (m_value != NULL)
		{
			CopyMemory(newValue, m_value, sizeof (D3DXVECTOR2) * m_nSize);
			delete [] m_value;
		}

		m_value = newValue;
		m_nAlloc = nSize;
	}

	m_nSize = nSize;
}

////////////////////////////////////////////////////////////////////////////

CVec3Array::CVec3Array()
{
	m_nAlloc = 0;
	m_nSize = 0;
	m_value = NULL;
}

CVec3Array::~CVec3Array()
{
	delete [] m_value;
}

void CVec3Array::SetSize(int nSize)
{
	if (nSize > m_nAlloc)
	{
		D3DXVECTOR3* newValue = new D3DXVECTOR3 [nSize];

		if (m_value != NULL)
		{
			CopyMemory(newValue, m_value, sizeof (D3DXVECTOR3) * m_nSize);
			delete [] m_value;
		}

		m_value = newValue;
		m_nAlloc = nSize;
	}

	m_nSize = nSize;
}

////////////////////////////////////////////////////////////////////////////

CVec4Array::CVec4Array()
{
	m_nAlloc = 0;
	m_nSize = 0;
	m_value = NULL;
}

CVec4Array::~CVec4Array()
{
	delete [] m_value;
}

void CVec4Array::SetSize(int nSize)
{
	if (nSize > m_nAlloc)
	{
		D3DXVECTOR4* newValue = new D3DXVECTOR4 [nSize];

		if (m_value != NULL)
		{
			CopyMemory(newValue, m_value, sizeof (D3DXVECTOR4) * m_nSize);
			delete [] m_value;
		}

		m_value = newValue;
		m_nAlloc = nSize;
	}

	m_nSize = nSize;
}

