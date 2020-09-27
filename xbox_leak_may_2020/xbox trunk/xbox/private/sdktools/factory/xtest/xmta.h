// xmta.h

#pragma once

class CHost;
class CError;
class CParameterTable;
class CDispatcher;
class CLoopManager;

class CAutoLeave
{
	CRITICAL_SECTION* m_pSection;
public:
	CAutoLeave (CRITICAL_SECTION* section) {
		m_pSection = section;
		EnterCriticalSection (m_pSection);
		}
	~CAutoLeave () {
		LeaveCriticalSection (m_pSection);
		}
};

class CAutoString
{
	LPTSTR m_str;

public:
	CAutoString (LPCTSTR format, ...);
	~CAutoString () {delete[] m_str;}
	operator LPCTSTR () {return m_str;}
};

/*
class CAutoSplit 
{
	_TCHAR m_first [128];
	LPCTSTR m_second;

public:
	CAutoSplit (LPCTSTR name, LPCTSTR separators = _T("."));
	~CAutoSplit () {}

	LPCTSTR GetFirst () {return m_first;}
	LPCTSTR GetSecond () {return m_second;}
};
CAutoSplit::CAutoSplit (LPCTSTR name, LPCTSTR separators)
{
	size_t dot = _tcscspn (name, separators);
	ASSERT (dot != -1);
	ASSERT (dot < _tcslen (name));

	m_second = &name [dot + 1];
	_tcsncpy (m_first, name, dot);
	m_first [dot] = 0;
	while (m_first [--dot] == _T(' '))
		m_first [dot] = 0;
}
*/

template<class T>
class CAutoDelete 
{
	T *p;
public:
	CAutoDelete (T *ptr = 0):p(ptr) {}
	~CAutoDelete () {delete p;}
	bool operator!() const {return (p != NULL);}
	T* get() const {return p;}
	void set (T* ptr) {p = ptr;}
};

class CAutoClose 
{
	HANDLE h;
public:
	CAutoClose (HANDLE handle = 0):h(handle) {}
	~CAutoClose () {CloseHandle (h);}
	bool IsValid () {return ((h != INVALID_HANDLE_VALUE) && (h != NULL));}
	bool operator!() const {return (h != NULL);}
	operator HANDLE () {return h;}
};

template <class T, int MAX>
class CArray
{
	T m_array [MAX];
	int m_count;

protected:
	T* GetArray () {return m_array;}

public:
	class BadIndexException {};

	CArray (int count, ...):m_count(count) 
	{
		va_list marker;
		va_start (marker, count);
		while (count-- && Append (va_arg (marker, T))) {}   
		va_end (marker );
	}

	CArray ():m_count(0) {}
	~CArray () {}

	bool Find (T t, int *i)
	{
		for (*i = 0; *i < m_count; (*i)++)
			if (array [*i] == t)
				return true;
		return false;
	}

	bool Append (T t)
	{
		if (m_count >= MAX) return false;
		m_array [m_count] = t;
		m_count++;
		return true;
	}

	bool Insert (T t, int i)
	{
		if (m_count >= MAX) return false;
		if ((i < 0) || (i > m_count)) throw BadIndexException();
		memcpy (&m_array[i+1], &m_array[i], (m_count-i)*sizeof(T));
		m_array [i] = t;
		m_count++;
		return true;
	}

	T RemoveAt (int i)
	{	
		T saved = m_array[i];
		m_count--;
		if (i < m_count) // closeup the hole
			memcpy (&m_array[i], &m_array[i+1], (m_count-i)*sizeof(T));
		return saved;
	}

	T& operator [] (int i) 
	{
		if ((i < 0) || (i >= m_count)) throw BadIndexException();
		return m_array[i];
	};

	int GetCount () {return m_count;}
	bool IsFull () {return (m_count >= MAX);}
	bool IsEmpty () {return (m_count <= 0);}
};

extern __int64 gi64CPUSpeed;
extern TCHAR g_tcXMTAVersionNumber[];
extern CError g_error;
extern CHost g_host;
extern CParameterTable g_parameters;
extern CLoopManager g_main;
extern CDispatcher g_dispatch;
extern DWORD g_tlsTestObj;
extern char *g_strHostName;
extern LPTSTR g_strSerialNumber;
void GetCPUSpeed();

#if defined( _XBOX ) 
extern LPDIRECT3D8			 g_pD3D;
extern LPDIRECT3DDEVICE8	 g_pd3dDevice;
extern D3DPRESENT_PARAMETERS g_d3dpp;
#endif
// Some constants for the app
//
const int MAX_NAME_LENGTH = 64;
const int MAX_THREAD_COUNT = 64; 
const int MAX_TIMEOUT = 30;
const int MAX_WAIT_HANDLES = 64; // Max size of WaitForMultipleObjects = 64






