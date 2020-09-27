#ifndef __DLLMAP_H__
#define __DLLMAP_H__

#define BEGIN_DLLNAME_MAP(var)		inline CFuncInfo * GetFuncInfo##var(void) { static CFuncInfo s_arrFuncInfo[] = {
#define FUNCTION_ITEM(name)	{ name, 0},
#define END_DLLNAME_MAP()		{0,0}}; return s_arrFuncInfo; }

#define FUNC_PROLOGUE()			if (!DllInit()) return FALSE;

#define INIT_WRAPPER(name)			protected:\
										name() {};\
									public:\
										name (const _TCHAR * szDll) :\
											CDllWrapper (szDll, GetFuncInfo##name())\
										{};

#define DECLARE_DLLWRAPPER(var, cls, dll)	cls var (dll);

struct CFuncInfo
{
	TCHAR *	m_szRoutine;
	FARPROC			m_pfn;
};

// wrapper class for Dll Loading
class CDllWrapper 
{
protected:
	CDllWrapper() {}; // private default ctor
	CFuncInfo * m_arrFuncInfo;	// array of functions loaded
private:
	BOOL m_fDllInit;		// check if we have tried to load or not
	HINSTANCE	m_hInst;	// handle to dll
	_TCHAR		m_szDll[_MAX_PATH]; // dll name
public:
	CDllWrapper (const _TCHAR * szDll, CFuncInfo * arrFuncInfo)
	{
		m_arrFuncInfo = arrFuncInfo;
		m_fDllInit = FALSE;
		m_hInst = 0;
		_tcscpy (m_szDll, szDll);
	}

	~CDllWrapper ()
	{
		if (m_hInst)
			FreeLibrary (m_hInst);
		m_hInst = 0;
		m_fDllInit = FALSE;
	}

	// initialiaze dll only if we have never tried to load
	BOOL DllInit ()
	{
		if (m_hInst)
			return TRUE;
		else if (m_fDllInit)
			return FALSE;
		else 
		{
			HINSTANCE hInst = LoadLibrary (m_szDll);
			m_fDllInit = TRUE;
			if (hInst)
			{
				m_hInst = hInst;
				CFuncInfo * arrFuncInfo;
				BOOL fRoutineOK = TRUE;

				for (arrFuncInfo = m_arrFuncInfo; arrFuncInfo->m_szRoutine; arrFuncInfo++)
				{
					fRoutineOK = fRoutineOK &&
						(arrFuncInfo->m_pfn = GetProcAddress (hInst, arrFuncInfo->m_szRoutine));
					if (!fRoutineOK)
					{
						char szBuf[2048];
						sprintf (szBuf, "Cannot load library: %s because of GetProcAddress (%s) fails\n", m_szDll, arrFuncInfo->m_szRoutine); 
						_ASSERT (szBuf);
						break;
					}
				}

				if (!fRoutineOK)
				{
					FreeLibrary (hInst);
					m_hInst = 0;
					return FALSE;
				}
			}
			else
				return FALSE;
		}
		return TRUE;
	}

	BOOL FInit () const 
	{
		return m_fDllInit;
	}

	BOOL FLoaded () const
	{
		return m_hInst != 0;
	}

	FARPROC FuncMap( UINT iRoutine ) 
	{
		return m_arrFuncInfo[iRoutine].m_pfn;
	}

};

#endif