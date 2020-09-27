//
// CProjComponentMgr, CProjType
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "prjoptn.h"	// our project options

#include "project.h"	// CProject class
#include "exttarg.h"	// The external target type

#include "resource.h"	// our Windows resource ids

#include "version.h"	// version info.

#ifdef PROJECT_CONFIGURE
IMPLEMENT_DYNAMIC(CProjectConfigureDialog, CDialog)	// C3dDialog is not dynamic.
#endif // PROJECT_CONFIGURE

IMPLEMENT_DYNCREATE(CBldSysCmp, CSlob)

IMPLEMENT_DYNCREATE(CPlatform, CBldSysCmp)

IMPLEMENT_DYNCREATE(CProjType, CBldSysCmp)

IMPLEMENT_DYNAMIC(COLEProjType, CProjType)

IMPLEMENT_DYNAMIC(CProjTypeUnknown, CProjType)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// define the global tool option engine (parsing & generation)
COptionTable AFX_DATA_EXPORT g_prjoptengine;

// define the global project configuration manager
CProjComponentMgr g_prjcompmgr;

// This helper routine attempts to create a unique name for an output
// directory.  This is called when a new configuration is created in
// order to establish unique output directories for that new configuration.
// This routine only checks for collisions with other configurations at
// the CProject level -- if a group in some configuration uses an output
// directory with the same name we propose, this routine doesn't catch that.
// But checking the project level should work in 90%+ of cases since most
// users won't override at the group or file level anyway.
//
// This routine should probably be in some PROJUTIL.CPP file...
void GetNewOutputDirName(CProject * pProject, const CString & strNewConfig, CString & strResult)
{
			int			nNumCfgs, nCfg, nTweak;
			BOOL		bUnique, bDirValid;
	const	CPath *		pProjPath;
			CPath		path;
			CDir		dirProj;
			CProp *		pProp;
			CPropBag *	pPropBag;

	ASSERT(!strNewConfig.IsEmpty());

	strResult = strNewConfig;
	// Replace any non-alphanumeric characters with '_'.
	for (int i=0;i<strResult.GetLength();i++)
	{
		#ifdef _MBCS
		if (_ismbblead(strResult[i]))
			i++;	// ignore/allow DBCS characters
		else
		#endif
		if (!IsCharAlphaNumeric(strResult[i]))
		{
			strResult.SetAt(i, _T('_'));
		}
	}

	// Now check to see if the CProject node has this same output directory
	// name (either intermediate or final) for any other configuration.
	nNumCfgs = pProject->GetPropBagCount();
	nTweak = 0;
	pProjPath = pProject->GetFilePath();
	bDirValid = TRUE;
	dirProj = pProject->GetProjDir();

	for (;;)
	{
		bUnique = TRUE;		// Assume

		// First check to see if the name exists on disk -- if so, it's
		// a poor choice for an output directory name.
		if	(
			(bDirValid)
			&&
			(path.CreateFromDirAndFilename(dirProj, strResult))
			&&
			(path.ExistsOnDisk())
			)
		{
			bUnique = FALSE;
		}
		else
		{
			for (nCfg=0 ; nCfg<nNumCfgs ; ++nCfg)
			{
				pPropBag = pProject->GetPropBag(nCfg);

				VERIFY((pProp = pPropBag->FindProp(P_ProjConfiguration)) != NULL);
				ASSERT(pProp->m_nType == string);

				if (((CStringProp *)pProp)->m_strVal == strNewConfig)
					continue;	// Don't need to check just-created config

				if ((pProp = pPropBag->FindProp(P_OutDirs_Intermediate)) != NULL)
				{
					ASSERT(pProp->m_nType == string);
					if (((CStringProp *)pProp)->m_strVal == strResult)
					{
						bUnique = FALSE;
						break;
					}
				}

				if ((pProp = pPropBag->FindProp(P_OutDirs_Target)) != NULL)
				{
					ASSERT(pProp->m_nType == string);
					if (((CStringProp *)pProp)->m_strVal == strResult)
					{
						bUnique = FALSE;
						break;
					}
				}
			}
		}

		if (bUnique)
			break;

		// Tweak the last character of the name to see if that helps.
		// if our tweak digit is 9.
		if (nTweak == 10)
		{
			break;			// Ran out of tweak digits; the user gets what we've got!
		}
		if(nTweak != 0){
			// strip off the last charater. NOTE that it is a digit and so not mb
			strResult=strResult.Left(strResult.GetLength()-1);
		}
		strResult += _T('0'+nTweak); // add a digit
		nTweak++;
	}
}

// helper
LONG RegGetKey
(
	HKEY hKey,
	LPCTSTR lpctstr,
	PHKEY phkey,
	BOOL fCreate
)
{
	// either create or just try to open the key
	if (fCreate)
	{
		DWORD dwDisp;
		return RegCreateKeyEx(hKey, lpctstr, 0,
					   	   	   "", REG_OPTION_NON_VOLATILE,
					       	   KEY_ALL_ACCESS, NULL, phkey,
					       	   &dwDisp);
	}
	else
	{
		return RegOpenKeyEx(hKey, lpctstr, 0,
							 KEY_ALL_ACCESS, phkey);
	}
}

// implementation of the CBldSysCmp class

static TCHAR szRegKey[]			= _TEXT("Build System\\Components\\");
static TCHAR szPlatforms[]		= _TEXT("Platforms\\");
static TCHAR szTargets[]		= _TEXT("Targets\\");
static TCHAR szTools[]			= _TEXT("Tools\\");
static TCHAR szVersion[]		= _TEXT("Version");
static TCHAR szCustomList[]		= _TEXT("Custom");
static TCHAR szCustomSection[]	= _TEXT("Custom Tools");

const CString CBldSysCmp::g_strRegKey(szRegKey);

#define theClass CBldSysCmp
BEGIN_SLOBPROP_MAP(CBldSysCmp, CSlob)
	STR_PROP(CompName)
END_SLOBPROP_MAP()
#undef theClass

#define theClass CBldSysCmp
BEGIN_REGISTRY_MAP_P(CBldSysCmp)
END_REGISTRY_MAP()
#undef theClass

CBldSysCmp::CBldSysCmp()
{
	// restore out component's data from the registry
	// this is TRUE by default unless the "Version"
	// value shows an invalid component
	m_fRegRestore = TRUE;
	
	m_dwId = m_dwRelatedId = 0;
}

void CBldSysCmp::SetId(DWORD dwId)
{
	m_dwId = dwId;

	// create our 'default' name?
	if (m_strCompName.IsEmpty())
	{
		sprintf(m_strCompName.GetBuffer(32), "<Component 0x%x>", dwId);
		m_strCompName.ReleaseBuffer();
	}

	m_fCompRegKeyDirty = TRUE;	// our 'cached' key is now dirty
}

void CBldSysCmp::SetRelatedId(DWORD dwRelatedId)
{
	m_dwRelatedId = dwRelatedId;

	m_fCompRegKeyDirty = TRUE;	// our 'cached' key is now dirty
}

CSlob * CBldSysCmp::Clone()
{
	// clone CSlob part first
	CBldSysCmp * pcomp = (CBldSysCmp *)CSlob::Clone();
	if (pcomp != (CBldSysCmp *)NULL)
	{
		// clone the component ids
		pcomp->SetId(m_dwId);
		pcomp->SetRelatedId(m_dwRelatedId);
	}

	return pcomp;
}

BOOL CBldSysCmp::SynchronizeWithRegistry(BOOL fSaveBeforeLoad /* = TRUE */)
{
	const TCHAR * pchKey = GetCompRegKeyName();

	// make sure this key exists
	HKEY hKey;
	if (RegGetKey(HKEY_CURRENT_USER, pchKey, &hKey) != ERROR_SUCCESS)
		return FALSE;	// failed

	// check the version info.
	// major, minor and update version info.
	int iMaj = 0, iMin = 0, iUpd = 0;
	TCHAR * szVerFormat =
#ifndef _SHIP
		_TEXT("%1d.%02d.%04d");	// eg. '3.00.1234'
#else
		_TEXT("%1d.%02d");		// eg. '3.00'
#endif // _SHIP

	DWORD dwType = 0;		// type must be REG_SZ
	TCHAR szVerValue[10];
	DWORD cbData = sizeof(szVerValue) / sizeof(TCHAR);
	if (m_fRegRestore &&				// want to try to restore?
		(RegQueryValueEx(hKey, (LPTSTR)szVersion, NULL, &dwType,
				  		(LPBYTE)szVerValue, &cbData) == ERROR_SUCCESS) &&
		(dwType == REG_SZ) &&			// must be an REG_SZ
		(cbData == 5 || cbData == 10)	// must be '0.00' or '0.00.0000'
	   )
	{
		// got version info. so

#ifndef _SHIP
		// pad with an update version?
		if (cbData == 5)
			_tcscat(szVerValue, _TEXT(".0000"));
#endif

		// and then crack the args
		_stscanf(szVerValue, szVerFormat, &iMaj, &iMin, &iUpd);

		// version ok?
		// ok to restore from registry
		m_fRegRestore = TRUE;
	}

	// write out version info.?
	if (!m_fRegRestore)
	{
		// format the arg
		_stprintf(szVerValue, szVerFormat, rmj, rmm, rup);

		// write out this registry key
		(void) RegSetValueEx(hKey, (LPTSTR)szVersion, NULL, REG_SZ,
							 (LPBYTE)szVerValue, _tcslen(szVerValue) + 1);
	}

	// now write out our data for this component, base component data first
	CPtrList mapStack;	// map stack for remembering position in map
	CString strValue;	// name of value in registry
	CString strSubKey;	// name of possible sub-key

	BOOL fSaving = fSaveBeforeLoad;

	// two-pass, do all saves, then all loads
	DoSyncPass:

	const CRegistryMap * pregmap = GetRegistryMap();
	BOOL fEndOfMap = FALSE;
	while (!fEndOfMap)
	{
		// base map?
		if (pregmap->regType == CRegistryMap::MapBegin)
		{
			if (pregmap->pvData != (const void *)NULL)
			{
				mapStack.AddTail((void *)pregmap);
				pregmap = (const CRegistryMap *)pregmap->pvData;
				continue;	// run down this one
			}
		}
		else
		// end of map?
		if (pregmap->regType == CRegistryMap::MapEnd)
		{
			if (mapStack.IsEmpty())
				fEndOfMap = TRUE;
			else
				pregmap = (const CRegistryMap *)mapStack.RemoveTail();
		}
		else
		{
			HKEY hSubKey = hKey;	// initially subkey is this key!

			// do we have a sub-key?
			if (pregmap->szRegSubKey != NO_SUB_KEY)
			{
				// do we have this subkey resource?
				// VERIFY(strSubKey.LoadString(pregmap->nRegSubKeyId));

				//if (RegGetKey(hKey, (LPCTSTR)(const TCHAR *)strSubKey, &hSubKey) != ERROR_SUCCESS)
				if (RegGetKey(hKey, (LPCTSTR)pregmap->szRegSubKey, &hSubKey) != ERROR_SUCCESS)
					goto NextInRegMap;	// failed goto next one
			}

			// do we have this value resource?
			// VERIFY(strValue.LoadString(pregmap->nRegId));
			strValue = (LPCTSTR)pregmap->szRegField;

			LPBYTE lpbData = (LPBYTE)this + (DWORD)pregmap->pvData;
			
			// make sure we have valid data
			ASSERT(pregmap->regType == CRegistryMap::MapFData || AfxIsValidAddress(lpbData, pregmap->cbData));

			DWORD dwType, cbData;	// retrieved from registry using RegDataExists()
			if (m_fRegRestore && RegDataExists(pregmap, hSubKey, &strValue, &dwType, &cbData))
			{
				// yes, so load it?
				if (!fSaving)
				{
					VERIFY(LoadRegData(pregmap, hSubKey, &strValue, lpbData, cbData));
				}
  			}
			else
			{
				// no, so save it?
				if (fSaving)
				{
					cbData = pregmap->cbData; // size of the data arena to fill
					VERIFY(SaveRegData(pregmap, hSubKey, &strValue, lpbData, cbData));
				}
			}

			// do we have a sub-key to close?
			if (pregmap->szRegSubKey != NO_SUB_KEY)
				RegCloseKey(hSubKey);
		}

		NextInRegMap:

		// next reg. map entry
		pregmap++;
	}

	// do the loading pass?
	if (m_fRegRestore)
	{
		if (!(fSaving = !fSaving))
			goto DoSyncPass;
	}

	// close the key
	RegCloseKey(hKey);

	return TRUE;		// ok
}

BOOL CBldSysCmp::Serialize(TCHAR * pch, UINT & cch)
{
	CPtrList mapStack;	// map stack for remembering position in map
	const CRegistryMap * pregmap = GetRegistryMap();

	UINT cchBuf = cch;
	UINT cchLen;

	// write out the component name
	const CString * pstrName = GetName();
	cchLen = pstrName->GetLength();
	if (cchBuf < (cchLen + 1))	// incl. zero term.
		return FALSE;	// need more memory

	_tcscpy(pch, (const TCHAR *)*pstrName);
	pch += cchLen; cchBuf -= cchLen;
	*pch++ = _T('\0'); cchBuf--;

	BOOL fEndOfMap = FALSE;
	while (!fEndOfMap)
	{
		// base map?
		if (pregmap->regType == CRegistryMap::MapBegin)
		{
			if (pregmap->pvData != (const void *)NULL)
			{
				mapStack.AddTail((void *)pregmap);
				pregmap = (const CRegistryMap *)pregmap->pvData;
				continue;	// run down this one
			}
		}
		else
		// end of map?
		if (pregmap->regType == CRegistryMap::MapEnd)
		{
			if (mapStack.IsEmpty())
				fEndOfMap = TRUE;
			else
				pregmap = (const CRegistryMap *)mapStack.RemoveTail();
		}
		else
		{
			// sub-keys are not supported!
			ASSERT(pregmap->szRegSubKey == NO_SUB_KEY);
			
			// do we have this value resource?
			// VERIFY(strValue.LoadString(pregmap->nRegId));
			CString strValue = pregmap->szRegField;

			// buffer has room for this value name + space sep.?
			cchLen = strValue.GetLength();
			if (cchBuf < (cchLen + 1))	// incl. space sep.
				return FALSE;	// need more memory

			_tcscpy(pch, (const TCHAR *)strValue);
			pch += cchLen; cchBuf -= cchLen;
			*pch++ = _T(' '); cchBuf--;

			// data to write-out
			LPBYTE lpbData = (LPBYTE)this + (DWORD)pregmap->pvData;
			
			// make sure we have valid data
			ASSERT(pregmap->regType != CRegistryMap::MapFData && AfxIsValidAddress(lpbData, pregmap->cbData));

			// what kind of data?
			switch (pregmap->dwType)
			{
				case REG_STRING:
				{
					// if we are using REG_STRING then we need to do some CString buffer magic
					// and fix up lpbData and cbData
					cchLen = ((CString *)lpbData)->GetLength();
					if (cchBuf < cchLen)
						return FALSE;	// need more memory

					// copy string value
					_tcscpy(pch, (const TCHAR *)*((CString *)lpbData));
					pch += cchLen; cchBuf -= cchLen;
					break;
				}

				case REG_DWORD:
				{
					TCHAR pchVal[10 + 1];
					cchLen = _tcslen(_itoa((unsigned long)(DWORD *)*lpbData, pchVal, 16));
					if (cchBuf < (cchLen + 2))	// allow for '0x'
						return FALSE;	// need more memory

					// copy hex. prefix
					_tcscpy(pch, "0x");
					pch += 2; cchBuf -= 2;

					// copy number value
					_tcscpy(pch, pchVal);
					pch += cchLen; cchBuf -= cchLen;
					break;
				}
			}

			// buffer has space for zero. term.?
			if (cchBuf < 1)
				return FALSE;	// need more memory

			*pch++ = _T('\0'); cchBuf--;
		}

		// next reg. map entry
		pregmap++;
	}

	// return chars. copied
	cch = cch - cchBuf;

	return TRUE;	// success
}

BOOL CBldSysCmp::Deserialize(TCHAR * pch, UINT cch)
{
	// FUTURE:
	return FALSE;
}

BOOL CBldSysCmp::RegGetDataKey(const CRegistryMap * pregmap, PHKEY phkey, BOOL fCreate)
{
	const TCHAR * pchKey = GetCompRegKeyName();

	// make sure the parent key exists
	HKEY hKey;
	if (RegGetKey(HKEY_CURRENT_USER, pchKey, &hKey, fCreate) != ERROR_SUCCESS)
		return FALSE;	// failed

	BOOL fRet = TRUE;	// success, unless get subkey fails
	*phkey = hKey;		// unless we have a subkey, this is the one we want

	// do we have a sub-key?
	if (pregmap->szRegSubKey != NO_SUB_KEY)
	{
		// do we have this subkey resource?
		// VERIFY(strSubKey.LoadString(pregmap->nRegSubKeyId));
		CString strSubKey = pregmap->szRegSubKey;	// name of possible sub-key

		fRet = RegGetKey(hKey, (LPCTSTR)(const TCHAR *)strSubKey, phkey, fCreate) == ERROR_SUCCESS;
		
		// always close the parent key
		RegCloseKey(hKey);
	}

	return fRet;	// success
}

BOOL CBldSysCmp::LoadRegData(UINT nRegId)
{
	const CRegistryMap * pregmap = GetRegMapEntry(nRegId);
	ASSERT(pregmap != (const CRegistryMap *)NULL);

	HKEY hDataKey;
	// n.b don't create key if doesn't exist already
	if (!RegGetDataKey(pregmap, &hDataKey, FALSE))
		return FALSE;

	// do we have this value resource?
	// VERIFY(strValue.LoadString(pregmap->nRegId));
	CString strValue = pregmap->szRegField;	// name of value in registry

	LPBYTE lpbData = (LPBYTE)this + (DWORD)pregmap->pvData;
	
	// make sure we have valid data
	ASSERT(pregmap->regType == CRegistryMap::MapFData || AfxIsValidAddress(lpbData, pregmap->cbData));

	DWORD dwType, cbData;	// retrieved from registry using RegDataExists()
	if (RegDataExists(pregmap, hDataKey, &strValue, &dwType, &cbData))
		VERIFY(LoadRegData(pregmap, hDataKey, &strValue, lpbData, cbData));

	// close the data key
	RegCloseKey(hDataKey);

	return TRUE;	// success
}

BOOL CBldSysCmp::SaveRegData(UINT nRegId)
{
	const CRegistryMap * pregmap = GetRegMapEntry(nRegId);
	ASSERT(pregmap != (const CRegistryMap *)NULL);

	HKEY hDataKey;
	// n.b. always create key
	if (!RegGetDataKey(pregmap, &hDataKey, TRUE))
		return FALSE;

	// do we have this value resource?
	// VERIFY(strValue.LoadString(pregmap->nRegId));
	CString strValue = pregmap->szRegField;	// name of value in registry

	LPBYTE lpbData = (LPBYTE)this + (DWORD)pregmap->pvData;
	
	// make sure we have valid data
	ASSERT(pregmap->regType == CRegistryMap::MapFData || AfxIsValidAddress(lpbData, pregmap->cbData));

	DWORD cbData;	// retrieved from registry using RegDataExists()
	cbData = pregmap->cbData; // size of the data arena to fill
	VERIFY(SaveRegData(pregmap, hDataKey, &strValue, lpbData, cbData));

	// close the data key
	RegCloseKey(hDataKey);

	return TRUE;	// success
}

const CRegistryMap * CBldSysCmp::GetRegMapEntry(UINT nRegId)
{
	CPtrList mapStack;	// map stack for remembering position in map
	const CRegistryMap * pregmap = GetRegistryMap();
	BOOL fEndOfMap = FALSE;
	while (!fEndOfMap)
	{
		// base map?
		if (pregmap->regType == CRegistryMap::MapBegin)
		{
			if (pregmap->pvData != (const void *)NULL)
			{
				mapStack.AddTail((void *)pregmap);
				pregmap = (const CRegistryMap *)pregmap->pvData;
				continue;	// run down this one
			}
		}
		else
		// end of map?
		if (pregmap->regType == CRegistryMap::MapEnd)
		{
			if (mapStack.IsEmpty())
				fEndOfMap = TRUE;
			else
				pregmap = (const CRegistryMap *)mapStack.RemoveTail();
		}
		else
		// found the registry entry we want?
		if (pregmap->nRegId == nRegId)
			//
			// FOUND registry entry
			return pregmap;

		// next reg. map entry
		pregmap++;
	}

	//
	// NO registry entry
	return (const CRegistryMap *)NULL;
}

BOOL CBldSysCmp::RegDataExists
(
	const CRegistryMap * pregmap,
	HKEY hKey,
	CString * pstrValue,
	LPDWORD pdwType, LPDWORD pcbData
)
{
	// data in registry?
	BOOL fRet = RegQueryValueEx(hKey, (LPTSTR)(const TCHAR *)*pstrValue, NULL, pdwType,
		   				        (LPBYTE)NULL, pcbData) == ERROR_SUCCESS;

	// make sure we have matching types if we succeeded
	ASSERT(!fRet || *pdwType == MapRegType(pregmap->dwType));
	return fRet;
}

BOOL CBldSysCmp::LoadRegData
(
	const CRegistryMap * pregmap,
	HKEY hKey,
	CString * pstrValue,
	LPBYTE lpbData, DWORD cbData
)
{
	ASSERT(lpbData != (BYTE *)NULL);	// forgot to implement 'special' override?

	DWORD dwType = pregmap->dwType;

	// if we are using REG_STRING then we need to do some CString buffer magic
	// and expand the buffer to the req. size
	CString * pstr = (CString *) (dwType == REG_STRING ? lpbData : NULL);
	if (pstr != (CString *)NULL)
	{
		(void) pstr->GetBuffer(cbData);
		lpbData = (LPBYTE)(const TCHAR *)*pstr;

		dwType = MapRegType(dwType);	// re-map to be a regualar system reg. type
	}

	// load the data
	if (RegQueryValueEx(hKey, (LPTSTR)(const TCHAR *)*pstrValue, NULL, &dwType,
				  		(LPBYTE)lpbData, &cbData) != ERROR_SUCCESS)
		return FALSE;	// failed

	// ensure that we adjust for the correct length (null-terminated string)
	if (pstr != (CString *)NULL)
		(void) pstr->ReleaseBuffer();

	return TRUE;	// ok
}

BOOL CBldSysCmp::SaveRegData
(
	const CRegistryMap * pregmap,
	HKEY hKey,
	CString * pstrValue,
	LPBYTE lpbData,
	DWORD cbData
)
{
	ASSERT(lpbData != (BYTE *)NULL);	// forgot to implement 'special' override?

	DWORD dwType = pregmap->dwType;

	// if we are using REG_STRING then we need to do some CString buffer magic
	// and fix up lpbData and cbData
	CString * pstr = (CString *) (dwType == REG_STRING ? lpbData : NULL);
	if (pstr != (CString *)NULL)
	{
		cbData = (DWORD)pstr->GetLength() + 1;
		lpbData = (LPBYTE)(const TCHAR *)*pstr;

		dwType = MapRegType(dwType);	// re-map to be a regualar system reg. type
	}

	// save the data
	if (RegSetValueEx(hKey, (LPTSTR)(const TCHAR *)*pstrValue, NULL, dwType,
			  			  lpbData, cbData) != ERROR_SUCCESS)
		return FALSE;	// failed

	return TRUE;	//ok
}

const TCHAR * CBldSysCmp::GetCompRegKeyName(BOOL fInclAppKey)
{
	if (m_fCompRegKeyDirty)
	{
		DWORD bsc_id = m_dwId;
		CBldSysCmp * pcomp;
		CString str;
	 	TCHAR * pchType;

		m_strCompRegKey.Empty();
		do
		{
			// get our component name
			if (g_prjcompmgr.LookupBldSysComp(bsc_id, pcomp))
				str = *pcomp->GetName();			// component name
			else
				{ASSERT(FALSE); return _TEXT("<Failed>");}		// failed!

			// get our component type
			if (pcomp->IsKindOf(RUNTIME_CLASS(CPlatform)))
				pchType = szPlatforms;

			else if (pcomp->IsKindOf(RUNTIME_CLASS(CProjType)))
				pchType = szTargets;

			else if (pcomp->IsKindOf(RUNTIME_CLASS(CBuildTool)))
				pchType = szTools;

			else if (pcomp->IsKindOf(RUNTIME_CLASS(COptionHandler)))
				ASSERT(FALSE);						// shouldn't be here!

			// concat. our component name to *front* of current key
			str += _TEXT("\\");
			str += m_strCompRegKey;

			// concat. our component type to *front* of component name
			m_strCompRegKey = pchType;
			m_strCompRegKey += str;

		} while ((bsc_id = pcomp->RelatedCompId()) != idAddOnGeneric);
		// 'break' out on bsc_id == idAddOnGeneric

		// append the BldSys key ro *front*
		m_strCompRegKey = *GetRegKeyName() + m_strCompRegKey;

		// append the Visual C++ key to *front*
		m_strFullCompRegKey = ::GetRegistryKeyName();
		m_strFullCompRegKey += _TEXT("\\");
		m_strFullCompRegKey += m_strCompRegKey;

		m_fCompRegKeyDirty = FALSE;	// not dirty anymore.
	}

 	return fInclAppKey ? (const TCHAR *)m_strFullCompRegKey : (const TCHAR *)m_strCompRegKey;
}

// implementation of the CProjComponentMgr class
void DefaultConfig(CProject * pProject, const CString & strPlatformUIDescription, UINT nIDMode, CString & strConfig)
{
	CString strName = pProject->GetFilePath()->GetFileName();
	int iDot = strName.ReverseFind(_T('.'));
	if (iDot > 0)
		strName = strName.Left(iDot);

	CString strMode;
	VERIFY(strMode.LoadString(nIDMode));
	strConfig =  strName + _T(" - ") + strPlatformUIDescription + _T(' ') + strMode;
}

CProjComponentMgr::CProjComponentMgr()
{
	// init. enumerator info.
	m_posProjType = (POSITION)NULL;				// no next projtype
	m_posTool = (POSITION)NULL;					// no next build tool

	m_idOptPropBase = PROJ_BUILD_OPTION_FIRST;	// our next option prop id.
	m_dwCustomBase = BCID_Custom_Component;		// our next custom component

	// FUTURE: remove this
	m_cSchmoozeTools = 0;						// no Schmooze tools initially registered

	m_nPropFirst = m_nPropLast = (UINT)-1;
	m_popthdlrCache = (COptionHandler *)NULL;

	// retrieve other information about the environment components might use
	CString strKeyName;
	HKEY hKey;

	// do we have ODBC installed?
	m_fODBCInstalled = TRUE;	// default is yes (error in favour of ODBC)

	// do we have MFC installed?
	m_fMFCInstalled = TRUE;		// default is yes (error in favour of MFC)

	strKeyName = GetRegistryKeyName();
	strKeyName += _TEXT("\\General");
	if (RegOpenKeyEx(HKEY_CURRENT_USER, strKeyName, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwType, dwValue, dwValueSize = sizeof(DWORD);
		if (RegQueryValueEx(hKey, _TEXT("MFCInstalled"), NULL, &dwType, (LPBYTE)&dwValue, &dwValueSize) == ERROR_SUCCESS)
		{
			// make sure we get the value we expect
			ASSERT(dwType == REG_DWORD);
			ASSERT(dwValueSize == sizeof(DWORD));
			m_fMFCInstalled = dwValue != 0;
		}
		if (RegQueryValueEx(hKey, _TEXT("ODBCInstalled"), NULL, &dwType, (LPBYTE)&dwValue, &dwValueSize) == ERROR_SUCCESS)
		{
			// make sure we get the value we expect
			ASSERT(dwType == REG_DWORD);
			ASSERT(dwValueSize == sizeof(DWORD));
			m_fODBCInstalled = dwValue != 0;
		}
		RegCloseKey(hKey); // close the key we just opened
	}
	m_fInit = FALSE;
}

CProjComponentMgr::~CProjComponentMgr()
{
	// delete out unknown project objects
	POSITION pos = m_lstUnkProjObjs.GetHeadPosition();
	CObject * pobj;
	while (pos != (POSITION)NULL)
	{
		pobj = (CObject *)m_lstUnkProjObjs.GetNext(pos);
		ASSERT_VALID(pobj);
		delete pobj;	// delete the object
	}

	// delete our build system components for the packages

	// delete in this order
	CRuntimeClass * rgDTOROrder[] =
	{
		// project types next
		RUNTIME_CLASS(CProjType),

		// then everything else
		RUNTIME_CLASS(CObject)
	};

	CBldSysCmp * pcomp;
	void * rkey;
	UINT iDTOROrder = 0;

	while (iDTOROrder < sizeof(rgDTOROrder) / sizeof(CRuntimeClass *))
	{
		CRuntimeClass * pRC = rgDTOROrder[iDTOROrder];

		pos = m_mapBldSysCmps.GetStartPosition();
		while (pos != (POSITION)NULL)
		{
			m_mapBldSysCmps.GetNextAssoc(pos, rkey, (void *&)pcomp);
			if (pcomp->IsKindOf(pRC))
			{
				delete (CBldSysCmp *)pcomp; // delete the component
				m_mapBldSysCmps.RemoveKey(rkey); // remove this one
			}
		}

		iDTOROrder++;
	}
}

BOOL CProjComponentMgr::FInit()
{
	// first off, ask the packages to register their components
	// we'll need to arrange the list of packages ourselves
	CObList lstArngdPackages;
	lstArngdPackages.AddTail(&(theApp.m_packages));

	// Now create the OLE servers for NON package based ones
	POSITION pos;
	pos = lstArngdPackages.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		CPackage * pPackage = (CPackage  *)lstArngdPackages.GetNext(pos);
		(void) pPackage->DoBldSysCompRegister(this, BLC_OleAddOn, m_dwRegisterId);
	}

	// first ask for generic components
	// o platforms
	// o tools not platform specific
	// o builder converters not product specific
	pos = lstArngdPackages.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		POSITION posCurr = pos;
		CPackage * pPackage = (CPackage  *)lstArngdPackages.GetNext(pos);

		m_dwRegisterId = idAddOnGeneric;	// general

		m_fRegPrimaryPlatform = FALSE;	// use this to check if registered a primary platform
		(void) pPackage->DoBldSysCompRegister(this, BLC_Platform, m_dwRegisterId);

		// registered a primary platform?
		if (m_fRegPrimaryPlatform)
		{
			// move package to head of list
			lstArngdPackages.RemoveAt(posCurr);
			lstArngdPackages.AddHead(pPackage);
		}

		(void) pPackage->DoBldSysCompRegister(this, BLC_Tool, m_dwRegisterId);
		(void) pPackage->DoBldSysCompRegister(this, BLC_OptionHdlr, m_dwRegisterId);
		(void) pPackage->DoBldSysCompRegister(this, BLC_BldrCnvtr, m_dwRegisterId);		
	}

	// Now register tools and platforms from the Ole Servers
	pos = m_AddOns.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		IEnumBuildTools *pToolEnum = NULL;

		LPBUILDSYSTEMADDON pAddOn = (LPBUILDSYSTEMADDON)m_AddOns.GetNext(pos);

		// Get the tools
		LPBUILDTOOL pTool = NULL;
		pAddOn->EnumBuildTools( &pToolEnum, &pTool );
		if( pTool ){
			UINT tool_id,pkg_id,bsc_id;
			pTool->GetID(&tool_id);
			pAddOn->GetID(&pkg_id);
			bsc_id = GenerateComponentId(pkg_id, tool_id);
			RegisterOleTool( pTool, bsc_id );
		} else if( pToolEnum ) { 
			LPBUILDTOOL pTools[10];
			ULONG nGot = 10;
			UINT pkg_id;
			pAddOn->GetID(&pkg_id);
			while( nGot == 10 ){
				pToolEnum->Next( 10, pTools, &nGot );
				ULONG i=0;
				while ( i < nGot ){
					// note that I am not releasing here
					UINT tool_id,bsc_id;
					pTool->GetID(&tool_id);
					bsc_id = GenerateComponentId(pkg_id, tool_id);
					RegisterOleTool( pTools[i], bsc_id );
					i++;
				}
			}
			pToolEnum->Release();
		}

		// Get the paltforms
		IPlatform *pPlat = NULL;
		IEnumPlatforms *pPlatEnum = NULL;
		pAddOn->EnumPlatforms( &pPlatEnum, &pPlat );
		if( pPlat != NULL ) {
			RegisterOlePlatform( pPlat, NULL/* bsc_id */ );
		} else if ( pPlatEnum != NULL ) { 
			LPPLATFORM pPlats[10];
			ULONG nGot = 10;
			while( nGot == 10 ){
				pPlatEnum->Next( 10, pPlats, &nGot );
				ULONG i=0;
				while ( i < nGot ){
					// note that I am not releasing here
					RegisterOlePlatform( pPlats[i], NULL/* bsc_id */ );
					i++;
				}
			}
		}

		// option handlers and bld converters here ?
	}


/*
R.I.P. for v4.0 with VBA?
	// retrieve our custom components
	if (!RetrieveGlobalCustomTool())
	{
		ASSERT(FALSE);	// FUTURE: error msg. goes here...
	}
*/

	// now ask each for any other components for each of these platforms
	// o tools
	// o target types (target types will bind to tools)
	// o builder converters
	CPlatform * pPlatform;
	InitPlatformEnum();
	while (NextPlatform(pPlatform))
	{
 		pos = lstArngdPackages.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CPackage * pPackage = (CPackage  *)lstArngdPackages.GetNext(pos);

			m_dwRegisterId = pPlatform->CompId();	// for each platform
			(void) pPackage->DoBldSysCompRegister(this, BLC_Tool, m_dwRegisterId);
			(void) pPackage->DoBldSysCompRegister(this, BLC_TargetType, m_dwRegisterId);
			(void) pPackage->DoBldSysCompRegister(this, BLC_BldrCnvtr, m_dwRegisterId);
		}
	}

	// Now register target types from the Ole Servers
	pos = m_AddOns.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		IEnumProjectTypes *pIEnum = NULL;
		IProjectType *pProj = NULL;
		LPBUILDSYSTEMADDON pAddOn = (LPBUILDSYSTEMADDON)m_AddOns.GetNext(pos);
		pAddOn->EnumProjectTypes( &pIEnum, &pProj );
		if( pProj != NULL ){
			 RegisterOleProjType( pProj, NULL/* bsc_id */ );
		} else if ( pIEnum != NULL ){
			LPPROJECTTYPE pProjs[10];
			ULONG nGot = 10;
			while( nGot == 10 ){
				pIEnum->Next( 10, pProjs, &nGot );
				ULONG i=0;
				while ( i < nGot ){
					// note that I am not releasing here
					RegisterOleProjType( pProjs[i], NULL/* bsc_id */ );
					i++;
				}
			}
		}
	}

	// ask for any tools that may wish to be a part of target type
	CProjType * pProjType;
	InitProjTypeEnum();
	while (NextProjType(pProjType))
	{
		// not for external targets!
 		if (pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)))
			continue;

		pos = lstArngdPackages.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CPackage * pPackage = (CPackage  *)lstArngdPackages.GetNext(pos);
			m_dwRegisterId = pProjType->CompId();	// for each target
			(void) pPackage->DoBldSysCompRegister(this, BLC_Tool, m_dwRegisterId);
		}
	}

	// Now we inform each add on that it can add additional tools to project types
	LPBUILDSYSTEM pInterface;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_IBuildSystem, (LPVOID FAR*)&pInterface)));
	pos = m_AddOns.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		LPENUMPROJECTTYPES pIEnum = NULL;
		LPPROJECTTYPE pProj = NULL;

		// each add on gets their own copy and MUST release it.
		LPBUILDSYSTEMADDON pAddOn = (LPBUILDSYSTEMADDON)m_AddOns.GetNext(pos);
		pInterface->EnumProjectTypes( &pIEnum, &pProj );
		pAddOn->ModifyProjectTypes( pIEnum );

	}
	pInterface->Release();


	// now ask for the option handlers for each platform (option handlers will bind to tools)
 	InitPlatformEnum();
	while (NextPlatform(pPlatform))
	{
 		pos = lstArngdPackages.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CPackage * pPackage = (CPackage  *)lstArngdPackages.GetNext(pos);

			m_dwRegisterId = pPlatform->CompId();	// will bind to a tool
			(void) pPackage->DoBldSysCompRegister(this, BLC_OptionHdlr, m_dwRegisterId);
		}
	}

	int			nCountPlatforms = 0;
	BOOL		bFoundPrimary = FALSE;

	// create our hashed index into the platforms
	// and at the same time FInit() the platforms
	InitPlatformEnum();
	while (NextPlatform(pPlatform))
	{
		if (pPlatform->FInit())
		{
			m_mapPlatforms.SetAt(*(pPlatform->GetName()), pPlatform);

			// count many platforms
			++nCountPlatforms;

			// ensure only one primary platform
			if (pPlatform->IsPrimaryPlatform())
			{
				ASSERT(!bFoundPrimary);
				bFoundPrimary = TRUE;
			}
		}
	}

 	// make sure we have at least one supported platform
	//if ((nCountPlatforms == 0) || !bFoundPrimary)
	if (nCountPlatforms == 0)
	{
		// this is v.bad, the build system is disabled
		ErrorBox(IDS_ERROR_NO_PLATFORMS);
	}

	// HACK ALERT
	// FUTURE (colint)
	// This code rearranges our projtype list so that all the
	// external target types are at the end of the list. This
	// is so that whenever we display a list of the projtypes
	// the external target type is always at the end. This code
	// should be replaced by a general mechanism for ordering
	// the projtype list for UI purposes.
	CProjType* pprojtype;
	CPtrList lstExtTargets;
	POSITION posCurr;
	
	// First we cycle through the projtypes building up a list
	// of all the external target types, and removing them from
	// the master list
	pos = m_lstProjTypes.GetHeadPosition();
	while (pos)
	{
		posCurr = pos;
		pprojtype = (CProjType*) m_lstProjTypes.GetNext(pos);
		if (pprojtype->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)))
		{
			m_lstProjTypes.RemoveAt(posCurr);
			lstExtTargets.AddTail(pprojtype);
		}
	}

	// Next we add back the external target types at the end of
	// the master list.
	pos = lstExtTargets.GetHeadPosition();
	while (pos)
	{
		pprojtype = (CProjType*) lstExtTargets.GetNext(pos);
		m_lstProjTypes.AddTail(pprojtype);
	}
	// END OF HACK ALERT

	// create our hashed index into the project types
 	InitProjTypeEnum();
	while (NextProjType(pprojtype))
	{
		m_mapProjTypes.SetAt(*(pprojtype->GetName()), pprojtype);
	}

/*
R.I.P. for v4.0 with VBA?
	// register our builder file section to read/write custom tools
	m_bldsection.m_strName = szCustomSection;
	m_bldsection.m_pfn = &SerializeProjectCustomTool;
	VERIFY(GetBldSysIFace()->RegisterSection(&m_bldsection));
*/

	return TRUE;
}

void CProjComponentMgr::Term()
{
/*
R.I.P. for v4.0 with VBA?
	// store our custom components
	if (!StoreGlobalCustomTool())
	{
		ASSERT(FALSE);	// FUTURE: error msg. goes here...
	}

	// de-register our builder file section
	VERIFY(GetBldSysIFace()->DeregisterSection(&m_bldsection));
*/
}

WORD CProjComponentMgr::GenerateBldSysCompPackageId(const TCHAR * szPackageName)
{	
	// szAddOnGeneric is zero
	if (szPackageName == szAddOnGeneric)
		return 0;

	// do we have this package id already?
	int iAddOn = m_rgstrAddOns.GetSize();
	while (iAddOn > 0)
	{
		iAddOn--;
		if (_tcscmp((TCHAR *)(const TCHAR *)m_rgstrAddOns[iAddOn], (TCHAR *)szPackageName) == 0)
			return (WORD)iAddOn + 1;
	}

	// new package id
	return (WORD)m_rgstrAddOns.Add(szPackageName) + 1;	// zero is reserved
}

int CProjComponentMgr::GetSupportedPlatformCount()
{
	int count = 0;
	POSITION pos = m_lstPlatforms.GetHeadPosition();
	while (pos != NULL)
	{
		if (((CPlatform *)m_lstPlatforms.GetNext(pos))->IsSupported())
			count++;
	}
	return count;
}

BOOL CProjComponentMgr::RegisterBldSysComp(CBldSysCmp * pcomp, DWORD bsc_id)
{
	BOOL fInRegistry = TRUE, fSaveBeforeLoad = TRUE;

 	// set the id.?
	if (!pcomp->CompId() && bsc_id != (DWORD)-1)
		pcomp->SetId(bsc_id);

	DWORD type = BldSysCompType(pcomp);

	// set the related id.
	if (!pcomp->RelatedCompId())
	{
		// custom tools are always generic
		if (type == BLC_CustomTool)
			pcomp->SetRelatedId(idAddOnGeneric);
		else
			pcomp->SetRelatedId(m_dwRegisterId);
	}

	// make sure we don't already have this
	ASSERT(!LookupBldSysComp(pcomp->CompId(), pcomp));

	// depending on the type of component this is,
	// we'll register in different lists
	switch (type)
	{
		case BLC_Platform:
			RegisterPlatform((CPlatform *)pcomp);
			break;

		case BLC_TargetType:
			RegisterProjType((CProjType *)pcomp);
			break;

/*
R.I.P. for v4.0 with VBA?
		case BLC_CustomTool:
			RegisterCustomTool((CCustomTool *)pcomp);
			fSaveBeforeLoad = FALSE;	// only load from registry!
			break;
*/

		case BLC_Tool:
			RegisterBuildTool((CBuildTool *)pcomp);
			break;

		case BLC_BldrCnvtr:
			RegisterBuilderConverter((CBuilderConverter *)pcomp);
			fInRegistry = FALSE;		// not in registry!
			break;

		case BLC_OptionHdlr:
			RegisterOptionHandler((COptionHandler *)pcomp);
			fInRegistry = FALSE;		// not in registry!
			break;

		default:
			ASSERT(FALSE);	// unknown bldsys component
			return FALSE;	// failure
	}

	// for quick retrieval use a map of id->component
	m_mapBldSysCmps.SetAt((void *)pcomp->CompId(), pcomp);

	// make sure that the component and system registry are in sync.
	if (fInRegistry && !pcomp->SynchronizeWithRegistry(fSaveBeforeLoad))
		return FALSE;	// failed to sync. with registry

	return TRUE;	// success
}

// get a component type for a component with id, 'bsc_id', returns 'BLC_ERROR' if not found
DWORD CProjComponentMgr::BldSysCompType(DWORD bsc_id)
{
	CBldSysCmp * pcomp;
	// does this component exist in the registry?
	if (!LookupBldSysComp(bsc_id, pcomp))
		return BLC_ERROR;

	return BldSysCompType(pcomp);
}

// get a component type for a component with id, 'bsc_id', returns 'BLC_ERROR' if not found
DWORD CProjComponentMgr::BldSysCompType(CBldSysCmp * pcomp)
{
	// our component type
	DWORD type = BLC_ERROR;

	// depending on the type of component this is,
	// we'll register in different lists
	if (pcomp->IsKindOf(RUNTIME_CLASS(CPlatform)))
		type = BLC_Platform;

	else if (pcomp->IsKindOf(RUNTIME_CLASS(CProjType)))
		type = BLC_TargetType;

/*
R.I.P. for v4.0 with VBA?
	else if (pcomp->IsKindOf(RUNTIME_CLASS(CCustomTool)))
		type = BLC_CustomTool;
*/

	else if (pcomp->IsKindOf(RUNTIME_CLASS(CBuildTool)))
		type = BLC_Tool;

	else if (pcomp->IsKindOf(RUNTIME_CLASS(COptionHandler)))
		type = BLC_OptionHdlr;

	else if (pcomp->IsKindOf(RUNTIME_CLASS(CBuilderConverter)))
		type = BLC_BldrCnvtr;

	return type;	// return our type
}

// generic object removal
BOOL CProjComponentMgr::DeregisterComponent(CPtrList & lst, CObject * ptr)
{
	// remove it from our list
	POSITION pos = lst.Find(ptr);
	if (pos != (POSITION)NULL)
	{
		// remove
		lst.RemoveAt(pos);

		return TRUE;	// ok
	}

	return FALSE; // didn't find it
}

// platform management
void CProjComponentMgr::RegisterPlatform(CPlatform * pPlatform)
{
	ASSERT_VALID(pPlatform);
	if (!pPlatform->FInit())
		return;

	m_mapPlatforms.SetAt(*(pPlatform->GetName()), (void *)pPlatform);

	if (pPlatform->IsPrimaryPlatform())
	{
		m_lstPlatforms.AddHead(pPlatform);
		m_fRegPrimaryPlatform = TRUE;
	}
	else
	{
		m_lstPlatforms.AddTail(pPlatform);
	}
}

BOOL CProjComponentMgr::DeregisterPlatform(CPlatform * pPlatform)
{
	m_mapPlatforms.RemoveKey(*(pPlatform->GetName()));

	return DeregisterComponent(m_lstPlatforms, pPlatform);
}

BOOL CProjComponentMgr::LookupPlatformByOfficialName(const TCHAR * szOfficialName, CPlatform * & pPlatform)
{
	return m_mapPlatforms.Lookup(szOfficialName, (void * &)pPlatform);
}

BOOL CProjComponentMgr::LookupPlatformByUIDescription(const TCHAR * szUIDescription, CPlatform * & pPlatform)
{
	CPlatform *	pPlatformEnum;

	InitPlatformEnum();

	while (NextPlatform(pPlatformEnum))
	{
		if (_tcscmp((const TCHAR *)*(pPlatformEnum->GetUIDescription()), szUIDescription) == 0)
		{
			pPlatform = pPlatformEnum;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CProjComponentMgr::LookupPlatformByUniqueId(UINT nUniqueId, CPlatform * & pPlatform)
{
	pPlatform = (CPlatform *)NULL;	// not found yet

	if (nUniqueId)	// don't match 'unknown' platforms
	{
		CPlatform *	pPlatformEnum;
		InitPlatformEnum();

		while (NextPlatform(pPlatformEnum))
		{
			// have we found a platform with our unique id?
			if (pPlatformEnum->GetUniqueId() == nUniqueId &&
				// do we have one yet, or can this override one 'not-named'
				// (created by unknown exe targets)
				(!pPlatform || (pPlatform->GetName()->CompareNoCase("<Unknown>") == 0))
			)
			{
				pPlatform = pPlatformEnum;
			}
		}
	}

	return pPlatform != (CPlatform *)NULL;
}

// [jimsc 7-16-96] Added the bAllowNonPrimary flag, which defaults to TRUE, to signal
// that if no platform explicitly marked "primary" is found, the first supported
// platform will be returned.  If FALSE, and no primary platforms are found, this
// will return NULL.

CPlatform * CProjComponentMgr::GetPrimaryPlatform(BOOL bAllowNonPrimary /* = TRUE */)
{
	CPlatform *	pPlatform;
	CPlatform *	pFirstSupported = NULL;

	InitPlatformEnum();

	while (NextPlatform(pPlatform))
	{
		if ((pFirstSupported == NULL) && (pPlatform->IsSupported()))
			pFirstSupported = pPlatform;
			
		if (pPlatform->IsPrimaryPlatform())
			return pPlatform;
	}

	// No primary guys were found, so return the first supported platform unless told not to
	return bAllowNonPrimary ? pFirstSupported : NULL;
}


UINT CProjComponentMgr::GetIdFromMachineImage(WORD wMachineImage)
{
	// This is a mapping function that maps from a NT image file
	// signature to our internal unique platform id's
	
	switch (wMachineImage)
	{
		case IMAGE_FILE_MACHINE_I386:
			return win32x86;

		case IMAGE_FILE_MACHINE_M68K:
			return mac68k;

		case IMAGE_FILE_MACHINE_ALPHA:
			return win32alpha;

		case IMAGE_FILE_MACHINE_MPPC_601:
			return macppc;
			
		case IMAGE_FILE_MACHINE_UNKNOWN:
		default:
			break;
	}

	// In case we didn't recognise the platform.
	return unknown_platform;
}

// project type management
void CProjComponentMgr::RegisterProjType(CProjType * pprojtype)
{
	ASSERT_VALID(pprojtype);

	if (!pprojtype->FInit())
		return;

	m_mapProjTypes.SetAt(*(pprojtype->GetName()), pprojtype);
	m_lstProjTypes.AddTail(pprojtype);
}

BOOL CProjComponentMgr::DeregisterProjType(CProjType * pprojtype)
{
	m_mapProjTypes.RemoveKey(*(pprojtype->GetName()));

	return DeregisterComponent(m_lstProjTypes, pprojtype);
}

BOOL CProjComponentMgr::GetProjTypefromProjItem(CProjItem * pProjItem, CProjType * & pProjType)
{
	CString strProjName;

	if (pProjItem->GetStrProp(P_ProjOriginalType, strProjName) != valid)
		return FALSE;	// can't find project type

	if (!LookupProjTypeByName(strProjName, pProjType))
		return FALSE;	// unrecognised project type

	return TRUE;	// ok
}

//
// build tool management
//
void CProjComponentMgr::RegisterBuildTool(CBuildTool * ptool)
{
	ASSERT_VALID(ptool);

	// initialise
	(void) ptool->FInit();

	// Put all the source tools at the head of the tool registry
	// Put all the schmooze tools at the tail of the tool registry
	if (ptool->IsKindOf(RUNTIME_CLASS(CSchmoozeTool)))
	{
		m_lstTools.AddTail(ptool);

		((CSchmoozeTool *) ptool)->SetMenuID
				(
					min(IDM_PROJECT_TOOL_CMD_LAST, (IDM_PROJECT_TOOL_CMD_FIRST + m_cSchmoozeTools))
				);
		m_cSchmoozeTools++;
	}
	else if (ptool->IsKindOf(RUNTIME_CLASS(CUnknownTool)))
	{
		// FUTURE: find a better way to handle this
		m_lstTools.AddTail(ptool);
	}
	else
		m_lstTools.AddHead(ptool);
}

//
// build tool management
//
void CProjComponentMgr::RegisterOleTool(IBuildTool * ptool, DWORD bsc_id )
{
	// Put all the source tools at the head of the tool registry
	// Put all the schmooze tools at the tail of the tool registry
	if ( ptool->IsCollectorTool() == S_OK )
	{

		// add the non schmooze tool to the front of the list.
		COLEBuildTool *pProxy = new COLEBuildTool( (LPUNKNOWN)ptool, 0 );
		m_lstTools.AddTail(pProxy);
		m_cSchmoozeTools++;
/*
		m_lstTools.AddTail(ptool);

		((CSchmoozeTool *) ptool)->SetMenuID
				(
					min(IDM_PROJECT_TOOL_CMD_LAST, (IDM_PROJECT_TOOL_CMD_FIRST + m_cSchmoozeTools))
				);
 */
	}
	else 
	{
		// add the non schmooze tool to the front of the list.
		COLEBuildTool *pProxy = new COLEBuildTool( (LPUNKNOWN)ptool, 0 );
		
		RegisterBldSysComp( pProxy, bsc_id );
	}
}

//
// build tool management
//
void CProjComponentMgr::RegisterOleAddOn( LPBUILDSYSTEMADDON pAddOn, DWORD bsc_id )
{
	m_AddOns.AddTail( pAddOn );

	LPBUILDSYSTEM pInterface;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_IBuildSystem, (LPVOID FAR*)&pInterface)));
	pAddOn->SetClient(pInterface);
}

//
// build tool management
//
void CProjComponentMgr::RegisterOleProjType(IProjectType * pProj, DWORD bsc_id )
{
	CString name;
	UINT type;
	pProj->GetName(&name);
	pProj->GetType(&type);
	// add the non schmooze tool to the front of the list.
	COLEProjType *pProxy = new COLEProjType( pProj, name, type );
	RegisterBldSysComp( pProxy, bsc_id );

}

//
// build tool management
//
void CProjComponentMgr::RegisterOlePlatform(IPlatform * pPlat, DWORD bsc_id )
{
	// add the non schmooze tool to the front of the list.
	COLEPlatform *pProxy = new COLEPlatform( pPlat);

	RegisterBldSysComp( pProxy, bsc_id );
}

/*
R.I.P. for v4.0 with VBA?
BOOL CProjComponentMgr::RegisterCustomTool(CCustomTool * ptool)
{
 	// generate a new custom tool id.
	// leave our related id. as zero (default)
	ptool->SetId(GenerateComponentId(idAddOnGeneric, m_dwCustomBase++));

	// initialise
	(void) ptool->FInit();

	// add to our list
	m_lstCustTools.AddTail(ptool);

	return TRUE;
}

#define CCH_ALLOC_BLOCK		256
BOOL CProjComponentMgr::SerializeProjectCustomTool(CBldrSection * pbldsect, UINT op)
{
	// read or write?
	switch (op)
	{
		case SECT_READ:
			// FUTURE:
			break;

		case SECT_WRITE:
		{
			TCHAR * pch;
			UINT cchAlloc = CCH_ALLOC_BLOCK;
			pch = (TCHAR *)calloc(cchAlloc + 1, sizeof(TCHAR));	// alloc. one for zero-term.
			if (pch == (TCHAR *)NULL)
				return FALSE;

			pbldsect->m_pch = pch;
			pbldsect->m_cch = 0;

			CCustomTool * pTool;
			g_prjcompmgr.InitCustomToolEnum();
			while (g_prjcompmgr.NextCustomTool(pTool))
			{
				// only write out project custom tools...
				//if (!pTool->m_bToolPerProject)
				//	continue;

			TryAgain:

				UINT cch = cchAlloc - pbldsect->m_cch;
				if (pTool->Serialize(pbldsect->m_pch + pbldsect->m_cch, cch))
				{
					// advance by actual bytes. copied
					pbldsect->m_cch += cch;
				}
				else
				{
					// re-alloc
					cchAlloc += CCH_ALLOC_BLOCK;
					pch = (TCHAR *)realloc(pbldsect->m_pch, sizeof(TCHAR) * (cchAlloc + 1));		// alloc. one for zero-term.
					if (pch == (TCHAR *)NULL)
					{
						free(pbldsect->m_pch);
						return FALSE;
					}

 					pbldsect->m_pch = pch;

					goto TryAgain;
				}
			}

			// zero-terminate
			*(pbldsect->m_pch + pbldsect->m_cch) = _T('\0');
			break;
		}

		case SECT_END:
		{
			// free our memory
			free(pbldsect->m_pch);
			break;
		}
	}

	return TRUE;	// ok
}

BOOL CProjComponentMgr::RetrieveGlobalCustomTool()
{
	// our custom tool list
	CString strKey = ::GetRegistryKeyName();
	strKey += _TEXT("\\");
	strKey += szRegKey;
	strKey += szTools;

	// make sure this key exists
	HKEY hKey;
	if (RegGetKey(HKEY_CURRENT_USER, (const TCHAR *)strKey, &hKey) != ERROR_SUCCESS)
		return TRUE;	// no tools, no parent key!

	DWORD dwType = 0;		// type must be REG_MULTI_SZ
	TCHAR * pszValue;
	DWORD cbData;
	// get the size of buffer we need
	if (RegQueryValueEx(hKey, (LPTSTR)szCustomList, NULL, &dwType,
				  		(LPBYTE)NULL, &cbData) != ERROR_SUCCESS)
		return TRUE;	// no tools, no key value

	if (dwType != REG_MULTI_SZ && dwType != REG_BINARY)
		return FALSE;	// no tools, wrong key value type -> error

	// get the data
	TCHAR * pch = new TCHAR[cbData];
	pszValue = pch;

	BOOL fRetVal = TRUE;

 	if (RegQueryValueEx(hKey, (LPTSTR)szCustomList, NULL, &dwType,
				  		(LPBYTE)pszValue, &cbData) == ERROR_SUCCESS)
	{
		// end of string block>
		while (*pszValue != _T('\0'))
		{
			// for each tool name in the tool list
			// register a build tool with the name and synchronize
			if (!RegisterBldSysComp(new CCustomTool(pszValue)))
			{
				fRetVal = FALSE;	// cannot create custom tools -> error
				break;
			}

			// move to the next name
			pszValue += _tcslen(pszValue);
			pszValue++;	// skip zero-terminator
		}
	}
	else
	{
		fRetVal = FALSE;
	}

	delete [] pch;

	return fRetVal;	// success
}

BOOL CProjComponentMgr::StoreGlobalCustomTool()
{
	// our custom tool list
	CString strKey = ::GetRegistryKeyName();
	strKey += _TEXT("\\");
	strKey += szRegKey;
	strKey += szTools;

	// make sure this key exists
	HKEY hKey;
	if (RegGetKey(HKEY_CURRENT_USER, (const TCHAR *)strKey, &hKey) != ERROR_SUCCESS)
		return TRUE;	// no tools, no parent key!

	// enumerate our list of custom tools
	// and write each out to the registry
	CStringList strlstValue;	// our 'Custom' value string list
	DWORD cbData = 1;			// size of our value string list (if empty we end with single '\0')

	CCustomTool * pTool;
	InitCustomToolEnum();
	while (NextCustomTool(pTool))
	{
		// don't write out per-project custom tools...
		if (pTool->m_bToolPerProject)
			continue;

		pTool->m_fRegRestore = FALSE;	// don't restore, just save
		if (pTool->SynchronizeWithRegistry())
		{
			// remember this tool
			const CString * pstrName = pTool->GetName();
			strlstValue.AddTail(*pstrName);
			cbData += pstrName->GetLength() + 1;	// incl. nul term.
		}
	}

	// allocate our registry 'Custom' value REG_MULTI_SZ
	TCHAR * pszValue = new TCHAR[cbData];
	TCHAR * psz = pszValue;

	POSITION pos = strlstValue.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		// copy into this buffer our name
		_tcscpy(psz, (const TCHAR *)strlstValue.GetNext(pos));
		psz += _tcslen(psz);

		// append with a '\0'
		*psz++ = _T('\0');
	}

	// finish with a '\0'

	*psz = _T('\0');

	// write out this registry key
	(void) RegSetValueEx(hKey, (LPTSTR)szCustomList, NULL, REG_MULTI_SZ,
						 (LPBYTE)pszValue, cbData);

	// free up our memory
	delete [] pszValue;

	return TRUE;
}
*/

// Find the tool whose prefix matches the pPrefix. nPrefixLen is the maximum
// number of characters to match up to.  pprojitem can be used to retrieve
// only the tools for the current configuration.
CBuildTool * CProjComponentMgr::GetToolFromCodePrefix(CProjItem * pItem, const TCHAR * pPrefix, int nPrefixLen)
{
	// Project type tool?
	if (pItem != (CProjItem *)NULL)
	{
		CProjType * pprojtype;
		if (!GetProjTypefromProjItem(pItem, pprojtype))
			return FALSE;

		return pprojtype->GetToolFromCodePrefix(pPrefix, nPrefixLen);
	}
	// In master-list or custom-tool list?
	else
	{
		CBuildTool * pTool;
/*
R.I.P. for v4.0 with VBA?
		CCustomTool * pCustTool;

		InitCustomToolEnum();
		while (NextCustomTool(pCustTool))
		{
			const TCHAR * pc = pCustTool->m_strToolPrefix;
			int len = _tcslen(pc);
			if (len && !_tcsnicmp(pPrefix, pc, min(nPrefixLen, len)))
				return pCustTool;
		}
*/

		InitBuildToolEnum();
		while (NextBuildTool(pTool))
		{
			const TCHAR * pc = pTool->m_strToolPrefix;
			int len = _tcslen(pc);
			if (len && !_tcsnicmp(pPrefix, pc, min(nPrefixLen, len)))
				return pTool;
		}
	}

	return (CBuildTool *)NULL;
}

// tool option management
void CProjComponentMgr::RegisterBuilderConverter(CBuilderConverter * pbldrcnvtr)
{
	m_lstBldrCnvtrs.AddTail(pbldrcnvtr);
}

BOOL CProjComponentMgr::DeregisterBuilderConverter(CBuilderConverter * pbldrcnvtr)
{
	return DeregisterComponent(m_lstBldrCnvtrs, pbldrcnvtr);
}

// tool option management
void CProjComponentMgr::RegisterOptionHandler(COptionHandler * popthdlr)
{
	popthdlr->SetOptPropBase(m_idOptPropBase);
	m_idOptPropBase += popthdlr->GetOptPropRange();

	// make sure we don't fall out of our range
	ASSERT(m_idOptPropBase < PROJ_BUILD_OPTION_PRIVATE_FIRST);
	m_lstOptionHandlers.AddTail(popthdlr);

	// initialise this option handler
	popthdlr->Initialise();
}

BOOL CProjComponentMgr::DeregisterOptionHandler(COptionHandler * popthdlr)
{
	return DeregisterComponent(m_lstOptionHandlers, popthdlr);
}

BOOL CProjComponentMgr::SetDefaultToolOptions(DWORD mode, CProject * pProject)
{
	// retrieve the string rep. of this proj. mode
	ASSERT(mode == MOB_Debug || mode == MOB_Release);
	CString strMode;
	VERIFY(strMode.LoadString(mode == MOB_Debug ? IDS_DEBUG_CONFIG : IDS_RELEASE_CONFIG));

	// retrieve the CProjType for the original project type for this project
	CProjType * pProjType;
	if (!GetProjTypefromProjItem((CProjItem *)pProject, pProjType))
		return FALSE;	// can't get the project type for this project item

	// preserve the option handler and current slob for 'option engine'
	COptionHandler * popthdlrOld = g_prjoptengine.GetOptionHandler();

	// enumerate through all of our known tools and then ask the project type
	// to provide the default option string
	// we'll then use the tools' option table to parse this string into
	// tool option properties
	CBuildTool * pBuildTool;
	POSITION pos;
	const CPtrList *pList = pProjType->GetToolsList ();

	for (pos = pList->GetHeadPosition (); pos != NULL;)	
	{
		pBuildTool = (CBuildTool *) pList->GetNext(pos);

		// ignore unknown tools
		if (!pBuildTool->IsSupported())
			continue;

		// ignore some tools, such as the pass through tool
		UINT bsc_id = pBuildTool->CompId();

		CString strOption; // don't move this line
		ASSERT(strOption.IsEmpty());

		// can the tool handle this?
		if (!pBuildTool->GetDefaultToolOptions(pProjType->CompId(), pProjType->GetAttributes(), mode, strOption))
			// no, so default to the target type - must be able to
			VERIFY(pProjType->GetDefaultToolOptions(bsc_id, strMode, strOption));

		// parse the default options into the property bag
		g_prjoptengine.SetOptionHandler(pBuildTool->GetOptionHandler());
		g_prjoptengine.SetPropertyBag(pProject);

		// clear before parse, behave as if we were reading from file ie.
		// we need "" to allow the setting of empty string props.
		VERIFY(g_prjoptengine.ParseString(strOption, OBClear | OBNeedEmptyString));
	}

	// reset current option handler
	g_prjoptengine.SetOptionHandler(popthdlrOld);

	return TRUE;	// success	
}

BOOL CProjComponentMgr::LookupProjTypeByUniqueId(UINT nUniqueId, CProjType * & pProjType)
{
	InitProjTypeEnum();
	while (NextProjType(pProjType))
	{
		if (nUniqueId==pProjType->GetUniqueId()) 
			return TRUE;
	}

	return FALSE;
}

BOOL CProjComponentMgr::LookupProjTypeByUIDescription(const TCHAR * pszUIDesc, CProjType * & pprojtype)
{
	// FUTURE: implement fast version of this?
	InitProjTypeEnum();
	while (NextProjType(pprojtype))	
	{
		if (!_tcscmp(pszUIDesc,LPCTSTR(*(pprojtype->GetUIDescription()))))
			return TRUE;
	}

	return FALSE;
}

CObList * CProjComponentMgr::GetMatchingProjTypes(const _TCHAR * pszPlatform, const _TCHAR * pszType, BOOL bUIDescription /* = FALSE */)
{
	CProjType * pprojtype;
	m_tmplstProjTypes.RemoveAll();

	if ((pszPlatform) && (pszType))	 // looking for specific match
	{
		if (bUIDescription)
		{
			if (!LookupProjTypeByName(pprojtype->MakeNameFromPlatformAndType(pszPlatform, pszType), pprojtype))
				return NULL;
		}
		else
		{
			if (!LookupProjTypeByUIDescription(pprojtype->MakeNameFromPlatformAndType(pszPlatform, pszType), pprojtype))
				return NULL;
		}
		if (!pprojtype->IsSupported())
			return NULL;
		m_tmplstProjTypes.AddTail(pprojtype);
	}
	else
	{
		LPCTSTR pszPlatformKey;
		LPCTSTR pszTypeKey;
		InitProjTypeEnum();
		while (NextProjType(pprojtype))
		{
			if (bUIDescription)
			{
				pszPlatformKey = LPCTSTR(*(pprojtype->GetPlatformUIDescription()));
				pszTypeKey     = LPCTSTR(*(pprojtype->GetTypeUIDescription()));
			}
			else
			{
				pszPlatformKey = LPCTSTR(*(pprojtype->GetPlatformName()));
				pszTypeKey     = LPCTSTR(*(pprojtype->GetTypeName()));
			}

			if (((!pszPlatform) || (!_tcscmp(pszPlatform, pszPlatformKey)))
				&&
				((!pszType) || (!_tcscmp(pszType, pszTypeKey))))
			{
				if (pprojtype->IsSupported())
					m_tmplstProjTypes.AddTail(pprojtype);
			}
		
		}
	}	

	return &m_tmplstProjTypes;
}

const CStringList * CProjComponentMgr::GetListOfProjTypes(const _TCHAR * pszPlatform /* = NULL */ , const _TCHAR * pszType /* = NULL */, BOOL bUIDescription /* = FALSE */)
{
	CProjType * pprojtype;
	CStringList * pList = (bUIDescription) ? &m_tmpUIDescList : &m_tmpNameList;
	pList->RemoveAll();

	if ((pszPlatform) && (pszType))	 // looking for specific match
	{
		if (bUIDescription)
		{
			if ((!LookupProjTypeByUIDescription(pprojtype->MakeNameFromPlatformAndType(pszPlatform, pszType), pprojtype))
				|| (!pprojtype->IsSupported()))
				return NULL;
			pList->AddTail(*(pprojtype->GetUIDescription()));
		}
		else
		{
			if ((!LookupProjTypeByName(pprojtype->MakeNameFromPlatformAndType(pszPlatform, pszType), pprojtype))
				|| (!pprojtype->IsSupported()))
				return NULL;
			pList->AddTail(*(pprojtype->GetName()));
		}
	}
	else
	{
		LPCTSTR pszPlatformKey;
		LPCTSTR pszTypeKey;
		InitProjTypeEnum();
		while (NextProjType(pprojtype))
		{	
		   	if (bUIDescription)
			{
				pszPlatformKey = LPCTSTR(*(pprojtype->GetPlatformUIDescription()));
				pszTypeKey     = LPCTSTR(*(pprojtype->GetTypeUIDescription()));
			}
			else
			{
				pszPlatformKey = LPCTSTR(*(pprojtype->GetPlatformName()));
				pszTypeKey     = LPCTSTR(*(pprojtype->GetTypeName()));
			}
			if (((!pszPlatform) || (!_tcscmp(pszPlatform, pszPlatformKey)))
				&&
				((!pszType) || (!_tcscmp(pszType, pszTypeKey))))
			{
				if (pprojtype->IsSupported())
					pList->AddTail((bUIDescription) ? *(pprojtype->GetUIDescription()) : *(pprojtype->GetName()));
			}
		}
	}	
	return pList;
}

extern BOOL g_bEnableAllPlatforms;

const CStringList * CProjComponentMgr::GetListOfPlatforms(const _TCHAR * pszType /* = NULL */, BOOL bUIDescription /* = FALSE */ )
{
	CProjType * pprojtype;
	CMapStringToPtr UniqueMap;
	const CString * pstrName;
	void * pTmp;
	LPCTSTR pszPlatformKey;
	LPCTSTR pszTypeKey;
	CStringList * pList = (bUIDescription) ? &m_tmpUIDescList : &m_tmpNameList;
	pList->RemoveAll();

	InitProjTypeEnum();
	while (NextProjType(pprojtype))
	{
		if (pprojtype->IsSupported() )
 		{
			if( pprojtype->GetPlatform()->GetBuildable() || g_bEnableAllPlatforms ){
				if (bUIDescription)
					pszTypeKey = LPCTSTR(*(pprojtype->GetTypeUIDescription()));
				else
					pszTypeKey = LPCTSTR(*(pprojtype->GetTypeName()));
		
				if ((!pszType) || (!_tcscmp(pszType, pszTypeKey)))
				{
					if (bUIDescription)
						pstrName = pprojtype->GetPlatformUIDescription();
					else
						pstrName = pprojtype->GetPlatformName();

					pszPlatformKey = LPCTSTR(*pstrName);
					if (!UniqueMap.Lookup(pszPlatformKey, pTmp)) // make sure platform not already in list	
					{
						// add platform name to uniqueness map and output list
						UniqueMap.SetAt(pszPlatformKey, this); /* value doesn't matter */
						pList->AddTail(*pstrName);
					}
				}
			}
		}
	}
	return pList;
}

const CStringList * CProjComponentMgr::GetListOfTypes(const _TCHAR * pszPlatform /* = NULL */, BOOL bUIDescription /* = FALSE */ )
{
	CProjType * pprojtype;
	CMapStringToPtr UniqueMap;
	const CString * pstrName;
	void * pTmp;
	LPCTSTR pszPlatformKey;
	LPCTSTR pszTypeKey;
	CStringList * pList = (bUIDescription) ? &m_tmpUIDescList : &m_tmpNameList;
	pList->RemoveAll();

	InitProjTypeEnum();
	while (NextProjType(pprojtype))
	{
		if (pprojtype->IsSupported())
 		{
			if (bUIDescription)
				pszPlatformKey = LPCTSTR(*(pprojtype->GetPlatformUIDescription()));
			else
				pszPlatformKey = LPCTSTR(*(pprojtype->GetPlatformName()));

			if ((!pszPlatform) || (!_tcscmp(pszPlatform, pszPlatformKey)))
			{
				if (bUIDescription)
					pstrName = pprojtype->GetTypeUIDescription();
				else
					pstrName = pprojtype->GetTypeName();

				pszTypeKey = LPCTSTR(*pstrName);
				if (!UniqueMap.Lookup(pszTypeKey, pTmp)) // make sure type not already in list	
				{
					// add type name to uniqueness map and output list
					UniqueMap.SetAt(pszTypeKey, this); /* value doesn't mater */
					pList->AddTail(*pstrName);
				}
			}
		}
	}
	return pList;
}

COptionHandler * CProjComponentMgr::FindOptionHandler(UINT idProp)
{
	// quick check to see if could be a tool option!
	if (idProp < PROJ_BUILD_OPTION_FIRST || idProp > PROJ_BUILD_OPTION_LAST)
		return (COptionHandler *)NULL;

	// try our cache first
	if (m_nPropFirst <= idProp && idProp <= m_nPropLast)
		return m_popthdlrCache;

	// look through our list of option handlers
	COptionHandler * popthdlr;	
	UINT nPropLast;

	InitOptHdlrEnum();
	while (NextOptHdlr(popthdlr))
	{
		// N.B. we know that these options are in an ordered list
		// such that they are in prop. id. block' order
		if (idProp <= (nPropLast = popthdlr->LastOptProp()))
		{
			// cache this
			m_nPropFirst = popthdlr->FirstOptProp();
			m_nPropLast = nPropLast;
			m_popthdlrCache = popthdlr;

			return popthdlr;
		}
	}

	return (COptionHandler *)NULL;	// no option handler found, not a tool option
}

#define theClass CPlatform
BEGIN_REGISTRY_MAP(CPlatform, CBldSysCmp)	
	// Symbol Handlers
	REGISTRY_DATA_ENTRY(IDS_REG_PLATCURRENTSH, IDS_REG_PLATSH,"Current SH","Symbol Handlers", REG_STRING, m_strSHName)

	// Expression Evaluators
	REGISTRY_DATA_ENTRY(IDS_REG_PLATCURRENTEE, IDS_REG_PLATEE,"Current EE","Expression Evaluators", REG_STRING, m_strEEName)

	// Execution Models
	REGISTRY_DATA_ENTRY(IDS_REG_PLATCURRENTEM, IDS_REG_PLATEM,"Current EM", "Execution Models", REG_STRING, m_strEMName)

	// Non-native Execution Models
	REGISTRY_FAKE_DATA_ENTRY(IDS_REG_PLATATTRIBNM, IDS_REG_PLATNM,"NM Attributes","Execution Models (non-native)", REG_DWORD)

	// Transports
	REGISTRY_FAKE_DATA_ENTRY(IDS_REG_PLATATTRIBTL, IDS_REG_PLATTL, "TL Attributes", "Transports", REG_DWORD)
	REGISTRY_FAKE_DATA_ENTRY(IDS_REG_PLATDESCTL, IDS_REG_PLATTL, "TL Description", "Transports", REG_STRING)

	REGISTRY_FAKE_DATA_ENTRY(IDS_REG_PLATCURRENTTL, IDS_REG_PLATTL, "Current TL", "Transports", REG_STRING)
END_REGISTRY_MAP()
#undef theClass

CPlatform::CPlatform(PlatformInfo * pInit)
{
	UINT	i;
	
	m_bSupported = TRUE;
	m_bBuildable = TRUE;

	m_nIDUIDesc = pInit->nIDUIDesc;
	m_nUniqueId = pInit->nUniqueId;
	m_nIDUIShortDesc = pInit->nIDUIShortDesc;
	m_nCountTLs = pInit->nCountTLs;
	m_nCountNMs = pInit->nCountNMs;

	m_rgIDTLDescs = new UINT[m_nCountTLs];
	m_rgTLFlags = new UINT[m_nCountTLs];

	// our component name
	m_strCompName = pInit->szName;

	// m_strUIDesc will be set up in FInit() method
	// m_strUIShortDesc will be set up in FInit() method if defined

	m_strEMName = pInit->szEMName;
	m_strSHName = pInit->szSHName;
	m_strEEName = pInit->szEEName;

	m_rgStrTLNames = new CString[m_nCountTLs];
	m_rgStrTLDescs = new CString[m_nCountTLs];

	for (i=0 ; i<m_nCountTLs ; ++i)
	{
		m_rgIDTLDescs[i] = pInit->rgTLInfo[i].nIDDesc;
		m_rgTLFlags[i] = pInit->rgTLInfo[i].nFlags;
		m_rgStrTLNames[i] = pInit->rgTLInfo[i].szName;
		VERIFY(m_rgStrTLDescs[i].LoadString(m_rgIDTLDescs[i]));
	}

	// default is the local TL
	if ((m_nCurrentTL = GetLocalTLIndex()) == -1)	m_nCurrentTL = 0;
	
	if (m_nCountNMs == 0)
	{
		m_rgStrNMNames = NULL;
		m_rgNMFlags = NULL;
	}
	else
	{
		m_rgStrNMNames = new CString[m_nCountNMs];
		m_rgNMFlags = new UINT[m_nCountNMs];
	
		for (i = 0; i < m_nCountNMs; i += 1)
		{
			m_rgStrNMNames[i] = pInit->rgNMInfo[i].szName;
			m_rgNMFlags[i] = pInit->rgNMInfo[i].nFlags;
		}
	}

	// initializing ToolInfo
	m_rgToolInfo.strRCExtension = pInit->szRCExtension;
	if (pInit->szPath)
		m_rgToolInfo.strPath = pInit->szPath;
	else
		m_rgToolInfo.strPath.Empty() ;
	if (pInit->szIncludePath)
		m_rgToolInfo.strIncludePath = pInit->szIncludePath;
	else
		m_rgToolInfo.strIncludePath.Empty() ;
	if (pInit->szLibPath)
		m_rgToolInfo.strLibPath = pInit->szLibPath;
	else
		m_rgToolInfo.strLibPath.Empty() ;
	if (pInit->szSourcePath)
		m_rgToolInfo.strSourcePath = pInit->szSourcePath;
	else
		m_rgToolInfo.strIncludePath.Empty() ;

	// our attributes
	m_dwAttributes = pInit->dwPlatformAttrib;

	// use MFC default
	m_nUseMFCDefault = pInit->nUseMFCDefault;

	ASSERT(m_nIDUIDesc);
	ASSERT(m_nUniqueId < 256);
	
	// load our platform description strings
	if (m_nIDUIDesc)
		VERIFY (m_strUICompName.LoadString(m_nIDUIDesc));

	if (m_nIDUIShortDesc)
		VERIFY (m_strUIShortDesc.LoadString(m_nIDUIShortDesc));
	else
		m_strUIShortDesc = m_strCompName.Left(5); // default

	// set up our default output directory
	if (pInit->szOutDirPrefix == (TCHAR *)NULL)
		m_strDefOutDirPrefix = m_strUICompName.Left(3);	// use TLA for default
	else
		m_strDefOutDirPrefix = pInit->szOutDirPrefix;
}

// Special constructor used for generating 'unknown' platforms dynamically
CPlatform::CPlatform(const _TCHAR * lpszName, UINT nUniqueId /* = unknown_platform */)
{
	// our component name
 	m_strCompName = lpszName;

	m_strUIShortDesc = m_strCompName.Left(5); // default
	// all other strings are "" for generic platforms

	m_nUniqueId = nUniqueId;
	// all other values are set to defaults (disabled)
	m_bSupported = FALSE;
	m_nIDUIDesc = 0;
	m_nIDUIShortDesc = 0;
	m_nCountTLs = 0;
	m_nCurrentTL = 0;
	m_rgIDTLDescs = NULL;
	m_rgTLFlags = NULL;
	m_rgStrTLNames = NULL;
	m_rgStrTLDescs = NULL;
	m_dwAttributes = 0;	
	m_nCountNMs = 0;
	m_rgStrNMNames = NULL;
	m_rgNMFlags = NULL;

 	// initializing ToolInfo
	m_rgToolInfo.strRCExtension.Empty() ;
	m_rgToolInfo.strPath.Empty() ;
	m_rgToolInfo.strIncludePath.Empty() ;
	m_rgToolInfo.strLibPath.Empty() ;
	m_rgToolInfo.strSourcePath.Empty() ;

	ASSERT(m_nUniqueId < 256);

	g_prjcompmgr.RegisterPlatform(this);
}

// Perform platform 'special' registry loading/saving
BOOL CPlatform::RegDataExists
(
	const CRegistryMap * pregmap,
	HKEY hKey,
	CString * pstrValue,
	LPDWORD pdwType, LPDWORD pcbData
)
{
	switch (pregmap->nRegId)
	{
		// Transport Layer values
		case IDS_REG_PLATDESCTL:
		case IDS_REG_PLATATTRIBTL:

		// Non-native Execution Models values
		case IDS_REG_PLATATTRIBNM:
		{
			UINT cDbgComp;
			FILETIME ft;
			DWORD dw;
			TCHAR rgchClass[1];	// we know the class to be "" ie. of length 1 byte
			DWORD cchClass = sizeof(rgchClass);
			// assume we need to fill in with defaults if this fails
			if (RegQueryInfoKey(hKey,
								rgchClass, &cchClass, NULL,
								(LPDWORD)&cDbgComp,
								&dw, &dw, &dw, &dw, &dw, &dw, // dummy values
								&ft) != ERROR_SUCCESS) cDbgComp = 0;

			// HACK: force the use of the const object's cbData for our data size ;-)
			((CRegistryMap *)pregmap)->cbData = cDbgComp;
			
			// HACK: toggle load/save
			BOOL fSave = !!((CRegistryMap *)pregmap)->pvData;
			((CRegistryMap *)pregmap)->pvData = (void *)!fSave;	// next time we load

			return fSave;
		}

		default:
			break;
	}

	// do the base-class thing
	return CBldSysCmp::RegDataExists(pregmap, hKey, pstrValue, pdwType, pcbData);
}

BOOL CPlatform::LoadRegData
(
	const CRegistryMap * pregmap,
	HKEY hKey,
	CString * pstrValue,
	LPBYTE lpbData, DWORD cbData
)
{
	BOOL fRet = TRUE;

	CString strFakedValue;
	DWORD dwFakedValue;
	switch (pregmap->nRegId)
	{
		case IDS_REG_PLATCURRENTTL:
			lpbData = (LPBYTE)&strFakedValue;
			break;

		// Non-native Execution Models values
		case IDS_REG_PLATATTRIBNM:
		{
			// we should have a NM count in data size(RegDataExists() did this for us!)
			UINT cNM = pregmap->cbData;

			// do the re-alloc.?
			if (cNM != m_nCountNMs)
			{
				delete [] m_rgStrNMNames;
			  	delete [] m_rgNMFlags;

				m_nCountNMs = cNM;
				m_rgStrNMNames = new CString[m_nCountNMs];
				m_rgNMFlags = new UINT[m_nCountNMs];
			}

			if (cNM == 0)
				return TRUE;	// nothing to load

			lpbData = (LPBYTE)&dwFakedValue;

			FILETIME ft;
			cNM = 0;	// count from zero->m_nCountNMs-1
			while (cNM < m_nCountNMs)
			{
				TCHAR rgchNMName[MAX_PATH+1];
				DWORD cch = sizeof(rgchNMName);
				if (RegEnumKeyEx(hKey,
								 (DWORD)cNM,
								 (LPTSTR)rgchNMName,
								 (LPDWORD)&cch,
								 NULL, NULL, NULL,
								 &ft) == ERROR_SUCCESS)
				{
					// make sure we have the name of this NM
					m_rgStrNMNames[cNM] = rgchNMName;

					// load into our internal representation the NM data
					HKEY hNMKey = (HKEY)NULL;
					BOOL fValueOk = FALSE;

					// open our subkey for the NM name
					if (RegGetKey(hKey, (LPCTSTR)rgchNMName, &hNMKey, FALSE) == ERROR_SUCCESS)
					{
						// load value
						DWORD dwType;
						if (CBldSysCmp::RegDataExists(pregmap, hNMKey, pstrValue, &dwType, &cbData))
							fValueOk = CBldSysCmp::LoadRegData(pregmap, hNMKey, pstrValue, lpbData, cbData);
					}

					m_rgNMFlags[cNM] = fValueOk ? dwFakedValue : 0;

					fRet = fRet && fValueOk;

					// close out sub-key?
					if (hNMKey)
						RegCloseKey(hNMKey);
				}

				cNM++;
			}

			return fRet;	// don't do base-class thing!
		}

		// Transport Layer values
		case IDS_REG_PLATDESCTL:
		case IDS_REG_PLATATTRIBTL:
		{
			// we should have a TL count in data size(RegDataExists() did this for us!)
			UINT cTL = pregmap->cbData;

			// need to re-map current index if reading new keys
			CString strTrackTL;
			if (m_nCurrentTL != (UINT)-1)
				strTrackTL = GetTLName(m_nCurrentTL);

			// do the re-alloc.?
			if (cTL != m_nCountTLs)
			{
				delete [] m_rgStrTLNames;
				delete [] m_rgStrTLDescs;
 			  	delete [] m_rgTLFlags;

				m_nCountTLs = cTL;
				m_rgStrTLNames = new CString[m_nCountTLs];
				m_rgStrTLDescs = new CString[m_nCountTLs];
 			  	m_rgTLFlags = new UINT[m_nCountTLs];
			}

			if (cTL == 0)
				return TRUE;	// nothing to load

			if (pregmap->nRegId == IDS_REG_PLATDESCTL)
				lpbData = (LPBYTE)&strFakedValue;
			else
				lpbData = (LPBYTE)&dwFakedValue;

			FILETIME ft;
			cTL = 0;	// count from zero->m_nCountTLs-1
			while (cTL < m_nCountTLs)
			{
				TCHAR rgchTLName[MAX_PATH+1];
				DWORD cch = sizeof(rgchTLName);
				if (RegEnumKeyEx(hKey,
								 (DWORD)cTL,
								 (LPTSTR)rgchTLName,
								 (LPDWORD)&cch,
								 NULL, NULL, NULL,
								 &ft) == ERROR_SUCCESS)
				{
					// make sure we have the name of this TL
					m_rgStrTLNames[cTL] = rgchTLName;

					// do we need to track?
					// is this the current TL we wish to track?
					if (strTrackTL.CompareNoCase(rgchTLName) == 0)
						m_nCurrentTL = cTL;

					// load into our internal representation the TL data
					HKEY hTLKey = (HKEY)NULL;
					BOOL fValueOk = FALSE;

					// open our subkey for the TL name
					if (RegGetKey(hKey, (LPCTSTR)rgchTLName, &hTLKey, FALSE) == ERROR_SUCCESS)
					{
						// load value
						DWORD dwType;
						if (CBldSysCmp::RegDataExists(pregmap, hTLKey, pstrValue, &dwType, &cbData))
							fValueOk = CBldSysCmp::LoadRegData(pregmap, hTLKey, pstrValue, lpbData, cbData);
					}

					if (pregmap->nRegId == IDS_REG_PLATDESCTL)
						m_rgStrTLDescs[cTL] = fValueOk ? strFakedValue : "<Error>";
					else
						m_rgTLFlags[cTL] = fValueOk ? dwFakedValue : 0;

					fRet = fRet && fValueOk;

					// close out sub-key?
					if (hTLKey)
						RegCloseKey(hTLKey);
				}

				cTL++;
			}

			return fRet;	// don't do base-class thing!
		}

		default:
			break;
	}

	fRet = CBldSysCmp::LoadRegData(pregmap, hKey, pstrValue, lpbData, cbData);

	switch (pregmap->nRegId)
	{
		case IDS_REG_PLATCURRENTTL:
			// set the name of our current TL
			m_nCurrentTL = GetTLIndex(strFakedValue, 0);
			break;

		default:
			break;
	}
	return fRet;	// ok?
}

BOOL CPlatform::SaveRegData
(
	const CRegistryMap * pregmap,
	HKEY hKey,
	CString * pstrValue,
	LPBYTE lpbData,
	DWORD cbData
)
{
	BOOL fRet = TRUE;

	CString strFakedValue;
	DWORD dwFakedValue;

	switch (pregmap->nRegId)
	{
		case IDS_REG_PLATCURRENTTL:
			// get the name of our current TL
			strFakedValue = GetTLName(GetCurrentTLIndex());
			lpbData = (LPBYTE)&strFakedValue;
			break;

		// Non-native Execution Models values
		case IDS_REG_PLATATTRIBNM:
		{
			HKEY hNMKey;

			BOOL fRegNMExists = !!pregmap->cbData;

			// we want to enumerate through our NM's and write out the description
			UINT cNM = 0;	// count from zero->m_nCountNMs-1
			while (cNM < m_nCountNMs)
			{
				// base our subkey name on the NM name
				// only try to create this if we have no NM subkeys
				if (RegGetKey(hKey, (LPCTSTR)GetNMName(cNM), &hNMKey, !fRegNMExists) != ERROR_SUCCESS)
					goto NoNMKeyOpen;	// failed goto next one

				// create this?	so we now have one
				if (!fRegNMExists)
					((CRegistryMap *)pregmap)->cbData++;

				// get our NM value
				dwFakedValue = GetNMFlags(cNM);

				// save it out (but only if we are filling in a hole!)
				DWORD dwType;
				if (!CBldSysCmp::RegDataExists(pregmap, hNMKey, pstrValue, &dwType, &cbData))
				{
					lpbData = (LPBYTE)&dwFakedValue;
					cbData = sizeof(DWORD);

					fRet = fRet && CBldSysCmp::SaveRegData(pregmap, hNMKey, pstrValue, lpbData, cbData);
				}

				// close out sub-key
				RegCloseKey(hNMKey);

				NoNMKeyOpen:

				cNM++;
			}

			return fRet;	// don't do base-class thing!
		}

		// Transport Layer values
		case IDS_REG_PLATDESCTL:
		case IDS_REG_PLATATTRIBTL:
		{
			HKEY hTLKey;

			BOOL fRegTLExists = !!pregmap->cbData;

			// we want to enumerate through our TL's and write out the description/attributes
			UINT cTL = 0;	// count from zero->m_nCountTLs-1
			while (cTL < m_nCountTLs)
			{
				// base our subkey name on the TL name
				// only try to create this if we have no TL subkeys
				if (RegGetKey(hKey, (LPCTSTR)GetTLName(cTL), &hTLKey, !fRegTLExists) != ERROR_SUCCESS)
					goto NoTLKeyOpen;	// failed goto next one

				// create this?	so we now have one
				if (!fRegTLExists)
					((CRegistryMap *)pregmap)->cbData++;

				// get our TL value
				if (pregmap->nRegId == IDS_REG_PLATDESCTL)
					strFakedValue = GetTLDescription(cTL);
				else
					dwFakedValue = GetTLFlags(cTL);

				// save it out (but only if we are filling in a hole!)
				DWORD dwType;
				if (!CBldSysCmp::RegDataExists(pregmap, hTLKey, pstrValue, &dwType, &cbData))
				{
					if (pregmap->nRegId == IDS_REG_PLATDESCTL)
						lpbData = (LPBYTE)&strFakedValue;
					else
					{
						lpbData = (LPBYTE)&dwFakedValue;
						cbData = sizeof(DWORD);
					}

					fRet = fRet && CBldSysCmp::SaveRegData(pregmap, hTLKey, pstrValue, lpbData, cbData);
				}

				// close out sub-key
				RegCloseKey(hTLKey);

				NoTLKeyOpen:

				cTL++;
			}

			return fRet;	// don't do base-class thing!
		}

		default:
			break;
	}

	fRet = CBldSysCmp::SaveRegData(pregmap, hKey, pstrValue, lpbData, cbData);

	return fRet;	// ok?
}

const UINT CPlatform::GetTLIndex(const TCHAR * pTLName, UINT nDefault)
{
	for (UINT i=0; i<m_nCountTLs ; i++)
	{
		if (m_rgStrTLNames[i].CompareNoCase (pTLName) == 0)
			return i;
	}
	return nDefault;
}

BOOL CPlatform::SupportsODBC() const
{
	return g_prjcompmgr.ODBCIsInstalled() && ((GetAttributes() & PIA_Allow_ODBC) != 0);
}

UINT CPlatform::GetUseMFCDefault()
{
	// FUTURE: remove this and just use the GetAttributes() & PIA_AllowMFCInDLL
	return g_prjcompmgr.MFCIsInstalled() ? m_nUseMFCDefault : NoUseMFC;
}

UINT CPlatform::GetLocalTLIndex() const
{
	for (UINT i=0; i<m_nCountTLs ; i++)
	{
		if (GetTLFlags(i) & TFL_LOCAL == TFL_LOCAL)
			return i;
	}

	return (const UINT)-1;
}

void CPlatform::SetCurrentTLIndex(UINT index)
{
	// remember the index of the current TL
	m_nCurrentTL = index;

	// save this state in our registry
	VERIFY (CBldSysCmp::SaveRegData(IDS_REG_PLATCURRENTTL));
}

BOOL CPlatform::FInit()
{
	if (!m_bSupported) // nothing to do for 'unknown' platforms
		return TRUE;

	// post-ctor initialise the platform here

	return TRUE;
}

CPlatform::~CPlatform()
{
	g_prjcompmgr.DeregisterPlatform(this);

	delete [] m_rgStrTLNames;
	delete [] m_rgStrTLDescs;
	delete [] m_rgIDTLDescs;
	delete [] m_rgTLFlags;
	delete [] m_rgStrNMNames;
	delete [] m_rgNMFlags;
}

const _TCHAR * CProjType::TypeNameFromId[] = {	_T("<unknown type>"),
						_T("Application"),
						_T("Dynamic-Link Library"),
						_T("Console Application"),
						_T("Static Library"),
						_T("ASLM Shared Library"),
						_T("External Target"),
						_T("QuickWin Application"),
						_T("Standard Graphics Application"),
						_T("Java Project"),
						_T("Generic Project")
						};

CProjType::CProjType(const TCHAR * szPkg, WORD id,
				     UINT nIDType, UINT nUniqTypeId,
			  		 const TCHAR * szPkgPlatform, WORD idPlatform)
{
	this->CProjType::CProjType(szPkg, id, nIDType, nUniqTypeId,
						 GenerateComponentId(g_prjcompmgr.GenerateBldSysCompPackageId(szPkgPlatform), idPlatform));
}

CProjType::CProjType(const TCHAR * szPkg, WORD id,
					   UINT nIDType, UINT nUniqTypeId,
					   DWORD bsc_id)
{
	// set our id.
	SetId(GenerateComponentId(g_prjcompmgr.GenerateBldSysCompPackageId(szPkg), id));

	ASSERT(nUniqTypeId < 256);

	// save the our name away
	VERIFY(m_nIDType = nIDType);
	m_nUniqueTypeId = nUniqTypeId & 255;
	ASSERT(m_nUniqueTypeId < 11);	// bump array size if this fires

	if (!g_prjcompmgr.LookupBldSysComp(bsc_id, (CBldSysCmp *&)m_pPlatform))
		ASSERT(FALSE);	// failed

	// make sure we got a valid platform
	ASSERT_VALID(m_pPlatform);
	ASSERT(m_pPlatform->IsKindOf(RUNTIME_CLASS(CPlatform)));

	m_strType = TypeNameFromId[m_nUniqueTypeId];

	// our component name
	m_strCompName = MakeNameFromPlatformAndType(*(m_pPlatform->GetName()), m_strType);
	m_pIProjType = NULL;
}

BEGIN_INTERFACE_MAP(CProjTypeInterface, CCmdTarget)
	INTERFACE_PART(CProjTypeInterface, IID_IProjectType, ProjType)
END_INTERFACE_MAP()

////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CProjTypeInterface::XProjType::AddRef()
{
	// Delegate to our exported AddRef.
	METHOD_PROLOGUE_EX(CProjTypeInterface, ProjType)
	return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CProjTypeInterface::XProjType::Release()
{
	// Delegate to our exported Release.
	METHOD_PROLOGUE_EX(CProjTypeInterface, ProjType)
	return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CProjTypeInterface::XProjType::QueryInterface(REFIID iid, LPVOID far * ppvObj)
{
	// Delegate to our exported QueryInterface.
	METHOD_PROLOGUE_EX(CProjTypeInterface, ProjType)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}


STDMETHODIMP CProjTypeInterface::XProjType::GetName( CString *pStr )
{
	METHOD_PROLOGUE_EX(CProjTypeInterface, ProjType );
	const CString *strName = pThis->m_pProjectType->GetTypeName();
	*pStr = *strName;
	return NOERROR;
}

STDMETHODIMP CProjTypeInterface::XProjType::GetType( UINT *pTypeID )
{
	METHOD_PROLOGUE_EX(CProjTypeInterface, ProjType );
	*pTypeID = pThis->m_pProjectType->GetUniqueId();
	return NOERROR;
}

STDMETHODIMP CProjTypeInterface::XProjType::GetPlatform( LPPLATFORM *pPlatfrom )
{
	METHOD_PROLOGUE_EX(CProjTypeInterface, ProjType );
	return NOERROR;
}

STDMETHODIMP CProjTypeInterface::XProjType::IsSupported( void )
{
	METHOD_PROLOGUE_EX(CProjTypeInterface, ProjType );
	
	BOOL bSupported = pThis->m_pProjectType->IsSupported();
	
	if( bSupported ){
		return S_OK;
	}
	else {
		return S_FALSE;
	}
}

STDMETHODIMP CProjTypeInterface::XProjType::PickTool( LPBUILDFILE pFile, LPBUILDTOOL *pBuildTool )
{
	METHOD_PROLOGUE_EX(CProjTypeInterface, ProjType );
	// CBuildTool * pTool;
	// pTool = pThis->PickTool((LPBUILDFILE)pFile );
	// convert from pTool to LPBUILDTOOL (impossible at the momment)
	return E_NOTIMPL;
}

STDMETHODIMP CProjTypeInterface::XProjType::AddTool( LPBUILDTOOL pBuildTool )
{
	METHOD_PROLOGUE_EX(CProjTypeInterface, ProjType );

	UINT idTool,idPkg,bsc_id;
	pBuildTool->GetID(&idTool);
	pBuildTool->GetPkgID(&idPkg);
	CBuildTool * pTool;
	bsc_id = GenerateComponentId( idPkg,idTool);
	if (!g_prjcompmgr.LookupBldSysComp(bsc_id, (CBldSysCmp *&)pTool))
		ASSERT(FALSE);	// failed

	// now add it
	pThis->m_pProjectType->AddTool(pTool);

	return NOERROR;
}

STDMETHODIMP CProjTypeInterface::XProjType::EnumBuildTools( LPENUMBUILDTOOLS *, LPBUILDTOOL * )
{
	METHOD_PROLOGUE_EX(CProjTypeInterface, ProjType );
	return  E_NOTIMPL;
}

void CProjType::ReleaseInterface( void ){
	if( m_pIProjType != NULL ) {
		pCIProjType->SetThis( NULL );
		m_pIProjType->Release();
		m_pIProjType = NULL;
	}
};

LPPROJECTTYPE CProjType::GetInterface(void){
	if( m_pIProjType ) {
		m_pIProjType->AddRef();
	} else {
		pCIProjType = new CProjTypeInterface;
		m_pIProjType = pCIProjType->GetInterface();
		pCIProjType->SetThis(this);
	}
	return m_pIProjType;
}


// Useful for dynamic creation of CProjTypes
CProjType::CProjType(const _TCHAR * lpszTypeName, UINT nUniqueTypeId, CPlatform * pPlatform)
{
	ASSERT(nUniqueTypeId < 256);
	
	m_nIDType = 0;
	m_strType = lpszTypeName;
	m_strUIType = lpszTypeName;
	m_nUniqueTypeId = nUniqueTypeId & 255;
	m_pPlatform = pPlatform;
	m_pIProjType = NULL;


	// our component name
	m_strCompName = MakeNameFromPlatformAndType(*(m_pPlatform->GetName()), m_strType);

	// register ourselves with the config. manager
	g_prjcompmgr.RegisterProjType(this);
}

BOOL CProjType::FInit()
{
	BOOL retval = m_nIDType ? m_strUIType.LoadString(m_nIDType) : TRUE;
	if (retval)
		m_strUICompName = MakeNameFromPlatformAndType(*(m_pPlatform->GetUIDescription()), m_strUIType);

    AddTool(szAddOnGeneric, BCID_Tool_CustomBuild);   
    AddTool(szAddOnGeneric, BCID_Tool_SpecialBuild);   

	return retval;
}

void CProjType::AddTool(const TCHAR * szPckg, WORD idTool)
{
	CBuildTool * pTool;
	if (!g_prjcompmgr.LookupBldSysComp(GenerateComponentId(g_prjcompmgr.GenerateBldSysCompPackageId((TCHAR *)szPckg), idTool),
									   (CBldSysCmp *&)pTool))
		ASSERT(FALSE);	// failed

	// now add it
	AddTool(pTool);
}

void CProjType::AddTool(WORD pckg, WORD idTool)
{
	CBuildTool * pTool;
	if (!g_prjcompmgr.LookupBldSysComp(GenerateComponentId(pckg, idTool), (CBldSysCmp *&)pTool))
		ASSERT(FALSE);	// failed

	// now add it
	AddTool(pTool);
}

void CProjType::AddTool(CBuildTool * pTool)
{
	// make sure we got a valid tool
	ASSERT_VALID(pTool);
	ASSERT(pTool->IsKindOf(RUNTIME_CLASS(CBuildTool)));

	// don't add if already there
	if (m_ToolsList.Find(pTool) != (POSITION)NULL)
		return;

	// FUTURE: remove this ordering when it doesn't matter
	// ie. whe we have a dependency based build engine
	// for the old v2 build engine CSchmoozeTool's must be last

	if (pTool->IsKindOf(RUNTIME_CLASS(CSchmoozeTool)))
	{
		// Find location of CCustomBuildTool or CSpecialBuildTool
		POSITION posSpecial = m_ToolsList.GetHeadPosition();
		while (NULL!=posSpecial)
		{
			// Save posLast which will be the position of pSpecailTool.
			POSITION posLast = posSpecial;

			CBuildTool* pSpecialTool = (CBuildTool*)m_ToolsList.GetNext(posSpecial);
			ASSERT(NULL!=pSpecialTool);
			if (pSpecialTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool)) ||
				pSpecialTool->IsKindOf(RUNTIME_CLASS(CSpecialBuildTool)) )
			{
				// Set posSpecial to position of pSpecialTool and exit loop.
				posSpecial = posLast;
				break;
			}
		}

		if (NULL!=posSpecial)
			m_ToolsList.InsertBefore(posSpecial,pTool);
		else
			m_ToolsList.AddTail(pTool);
	}
	else if (pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool)))
	{
		m_ToolsList.AddTail(pTool);
	}
	else if (pTool->IsKindOf(RUNTIME_CLASS(CSpecialBuildTool)))
	{
		m_ToolsList.AddTail(pTool);
	}
	else
		m_ToolsList.AddHead((CObject *)pTool);
	// FUTURE:
}

CProjType::~CProjType()
{
	CBuildTool * pTool;
	// remove tools, deleting any dynamic ones
	while (!m_ToolsList.IsEmpty())
	{
		pTool = (CBuildTool *)m_ToolsList.RemoveHead();
		if (pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)))
			delete pTool;
	}

	// de-register ourselves with the config. manager
	g_prjcompmgr.DeregisterProjType(this);
	ReleaseInterface();
}

void CProjType::ClearUnknownTools()
{
	CBuildTool * pTool;
	// remove tools, deleting any dynamic ones
    POSITION posCurr;
    POSITION pos = m_ToolsList.GetHeadPosition();
    while (pos != NULL)
	{
        posCurr = pos;
        pTool = (CBuildTool *)m_ToolsList.GetNext(pos);
		if (pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)))
        {
			delete pTool;
            m_ToolsList.RemoveAt(posCurr);
        }
	}
}

BOOL CProjType::SetDefaultDebugTargetOptions(CProject* pProject, CTargetItem * pTarget, BOOL fUseMFC)
{
	pProject->SetIntProp(P_ProjAppWizUseMFC, FALSE);

	// use MFC by default, except console application
	// NOTE: must do this after set default tool option !
	int iUseMFC = fUseMFC ? GetUseMFCDefault() : NoUseMFC;
	pProject->SetIntProp(P_ProjUseMFC, iUseMFC);

	return TRUE;
}

BOOL CProjType::SetDefaultReleaseTargetOptions(CProject* pProject, CTargetItem * pTarget, BOOL fUseMFC)
{
	return SetDefaultDebugTargetOptions(pProject, pTarget, fUseMFC);
}

//	Get through the tools one at a time, looking for a match.  This is
//	slow if there are many tools, but there ain't.
CBuildTool * CProjType::PickTool(FileRegHandle frh)
{
	if (frh == (FileRegHandle)NULL)
		return NULL;

	CBuildTool * pTool;
	POSITION pos = m_ToolsList.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		pTool = (CBuildTool *)m_ToolsList.GetNext (pos);

		if (pTool->m_fTargetTool)
			break;

		if (pTool->AttachToFile(frh))
			return pTool;
	}
	return NULL;
}


CBuildTool * CProjType::GetToolFromCodePrefix(const TCHAR * pPrefix, int nPrefixLen)
{
	CBuildTool * pTool;
/*
R.I.P. for v4.0 with VBA?
	CCustomTool * pCustTool;

	g_prjcompmgr.InitCustomToolEnum();
	while (g_prjcompmgr.NextCustomTool(pCustTool))
	{
		const TCHAR * pc = pCustTool->m_strToolPrefix;
		int len = _tcslen(pc);
		if (len && !_tcsnicmp(pPrefix, pc, min(nPrefixLen, len)))
			return pCustTool;
	}
*/

	POSITION pos = m_ToolsList.GetHeadPosition();
	while (pos != (POSITION)NULL)	
	{
		pTool = (CBuildTool *)m_ToolsList.GetNext(pos);

		const TCHAR * pc = pTool->m_strToolPrefix;
		int len = (int)_tcslen(pc);
		if (len && !_tcsnicmp(pPrefix, pc, min(nPrefixLen, len)))
			return pTool;
	}

	return (CBuildTool *)NULL;
}

void CProjType::PerformSettingsWizard(CProjItem * pItem, BOOL fUsingMFC)
{
	// ensure our configs are ok, ie. inform each tool
	// of this MFC state change

	// what is the config. mode?
	BOOL fDebug;

	pItem->GetIntProp(P_UseDebugLibs, fDebug);

	const CPtrList * plistTools = GetToolsList();
	
	// preserve the option handler and current slob for 'option engine'
	COptionHandler * popthdlrOld = g_prjoptengine.GetOptionHandler();
	CSlob * pslobOld = g_prjoptengine.GetPropertyBag();

	g_prjoptengine.SetPropertyBag(pItem);

	CBuildTool * pTool;
	for (POSITION pos = plistTools->GetHeadPosition(); pos != NULL;)
	{
		// tell each tool that whether or not we're using MFC
		pTool = (CBuildTool *)plistTools->GetNext(pos);
		COptionHandler * popthdlr = pTool->GetOptionHandler();
		if (popthdlr != (COptionHandler *)NULL)
			g_prjoptengine.SetOptionHandler(popthdlr);
		pTool->PerformSettingsWizard(pItem, fDebug, (BOOL)fUsingMFC);
	}		

	// reset
	g_prjoptengine.SetPropertyBag(pslobOld);
	g_prjoptengine.SetOptionHandler(popthdlrOld);
}

CString CProjType::MakeNameFromPlatformAndType(const CString & platform, const CString & type)
{
	return platform + _TCHAR(' ') + type;
}

CString CProjType::MakeNameFromPlatformAndType(const _TCHAR * platform, const _TCHAR * type)
{
	return (CString)platform + _TCHAR(' ') + type;
}

// COLEProjType
// ************

CPlatform * COLEProjType::GetPlatform() const {
	return m_pPlatform;
}

// Get Type name Get the project type name.
const CString * COLEProjType::GetTypeName(){
	ASSERT( m_pProj );
	if( m_pProj ){
		// TODO : change to LPOLESTR
		CString str;
		m_pProj->GetName( &str );
		m_strType = str;
	}
	return &m_strType;		// our target type name
}

// IsSupported - Add On should true if all components needed for 
//				 this project type are available.
BOOL COLEProjType::IsSupported() const {
	ASSERT( m_pProj );
	if( m_pProj ){
		return m_pProj->IsSupported();
	}
	return FALSE;
}

CBuildTool *COLEProjType::PickTool(FileRegHandle){
	ASSERT( m_pProj );
	if( m_pProj ){
		// Get the file handle from the pItem
		// m_pProj->PickTool( file handle, tool );
		// convert the ole tool to a CBuid tool
		// return pTool;
	}
	return NULL;
}

CPtrList * COLEProjType::GetToolsList() {
	return &m_ToolsList;
}

// CProjTypeUnknown
// ****************

CProjTypeUnknown::~CProjTypeUnknown()
{
}

BOOL CProjTypeUnknown::FInit()
{
	// Tools that we can use.  Tools are added later dynamically

	return CProjType::FInit();
}

// our tool option helper functions
void VPROJDeriveDefaultOptions(CString & strOption, UINT nIDCommon, UINT nIDMode, UINT nIDOption)
{
	CString	strPart;

	VERIFY(strOption.LoadString(nIDCommon));
	if (nIDMode != (UINT)-1)
	{
		VERIFY(strPart.LoadString(nIDMode));
		strOption += _T(' '); strOption += strPart;
	}
	if (nIDOption != (UINT)-1)
	{
		VERIFY(strPart.LoadString(nIDOption));
		strOption += _T(' '); strOption += strPart;
	}
}

////////////////////////////////////////////////////////////
// CConfigEnum

CConfigEnum g_theConfigEnum;	// The one and only CConfigEnum

CEnumerator * CConfigEnum::GetList()
{
	// FUTURE: this could be made a whole lot more efficient!

	int			nCfg, nNumCfgs, nCurCfg, nTotalCfgs;
	CProp *		pProp;

	// Free the last list returned, if any.
	if (m_prevListReturned != NULL)
		delete [] m_prevListReturned;

	nTotalCfgs = nCurCfg = 0;
	// Now form the new list and return it.
	CString strProject;
	CProject * pProject;
	CProject::InitProjectEnum();
	while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, TRUE)) != NULL)
	{
		nTotalCfgs += pProject->GetPropBagCount();
	}
	m_prevListReturned = new CEnumerator[nTotalCfgs + 1];

	CProject::InitProjectEnum();
	while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, TRUE)) != NULL)
	{
		nNumCfgs = pProject->GetPropBagCount();
		for (nCfg = 0 ; nCfg<nNumCfgs ; ++nCfg, ++nCurCfg)
		{
			CPropBag * pBag = pProject->GetPropBag(nCfg);
			VERIFY((pProp = pBag->FindProp(P_ProjConfiguration)) != (CProp *)NULL);
			ASSERT(pProp->m_nType == string);

			m_prevListReturned[nCurCfg].szId = (const char *)((CStringProp *)pProp)->m_strVal;

			// is this a 'native' supported config.?
			BOOL fSupported = TRUE;	// default is TRUE
		
			// FUTURE: Do we need this for exe projects at all!
			if (!pProject->m_bProjIsExe)	// exe projects *are* supported!
			{
				VERIFY((pProp = pBag->FindProp(P_ProjOriginalType)) != (CProp *)NULL);
				ASSERT (pProp->m_nType == string);

				CProjType * pprojtype;
				if (g_prjcompmgr.LookupProjTypeByName(((CStringProp *)pProp)->m_strVal, pprojtype))
					fSupported = pprojtype->IsSupported();
			}
			m_prevListReturned[nCurCfg].val = fSupported;	// supported?
		}
	}

	m_prevListReturned[nCurCfg].szId = NULL;	// terminate list
	m_prevListReturned[nCurCfg].val = 0;		// terminate list

	return m_prevListReturned;
}

////////////////////////////////////////////////////////////
// CConfigEnum

CProjectEnum g_theProjectEnum;	// The one and only CConfigEnum

CEnumerator * CProjectEnum::GetList()
{
	// FUTURE: this could be made a whole lot more efficient!

	// Free the last list returned, if any.
	if (m_prevListReturned != NULL) {
		delete [] m_prevListReturned;
		delete [] m_prevStrListReturned;
	}

	// Now form the new list and return it.
	CString strProject;
	CProject * pProject;
	int nTotalProjects=0;
	CProject::InitProjectEnum();
	while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, TRUE)) != NULL)
	{
		nTotalProjects++;
	}
	m_prevListReturned = new CEnumerator[nTotalProjects + 1];
	m_prevStrListReturned = new CString[nTotalProjects];

	int	nCurProj = 0;
	CProject::InitProjectEnum();
	while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, TRUE)) != NULL)
	{
		int nNumCfgs = pProject->GetPropBagCount();
		if( nNumCfgs ){
			CString strProjectName;
			VERIFY(pProject->GetStrProp(P_ProjItemName, strProjectName));

			int iDot = strProjectName.ReverseFind(_T('.'));
			if (iDot > 0)
				strProjectName = strProjectName.Left(iDot);

			m_prevStrListReturned[nCurProj] = (const char *)strProjectName;
			m_prevListReturned[nCurProj].szId = (const char *)m_prevStrListReturned[nCurProj];
			m_prevListReturned[nCurProj].val = TRUE;	// supported?
			nCurProj++;
		}
	}

	m_prevListReturned[nCurProj].szId = NULL;	// terminate list
	m_prevListReturned[nCurProj].val = 0;		// terminate list

	return m_prevListReturned;
}

CEnumerator * CProjConfigEnum::GetList()
{
	// FUTURE: this could be made a whole lot more efficient!

	int			nCfg, nNumCfgs, nCurCfg, nTotalCfgs;
	CProp *		pProp;

	// Free the last list returned, if any.
	if (m_prevListReturned != NULL){
		delete [] m_prevListReturned;
		delete [] m_prevStrListReturned;
	}

	nTotalCfgs = nCurCfg = 0;
	// Now form the new list and return it.

	CString strProject;
 	CProject * pProject = g_pActiveProject;
	nTotalCfgs += pProject->GetPropBagCount();
	nNumCfgs = pProject->GetPropBagCount();
	m_prevListReturned = new CEnumerator[nTotalCfgs + 1];
	m_prevStrListReturned = new CString[nTotalCfgs];


	for (nCfg = 0 ; nCfg<nNumCfgs ; ++nCfg, ++nCurCfg)
	{
		CPropBag * pBag = pProject->GetPropBag(nCfg);
		VERIFY((pProp = pBag->FindProp(P_ProjConfiguration)) != (CProp *)NULL);
		ASSERT(pProp->m_nType == string);

		CString strDescription = (const char *)((CStringProp *)pProp)->m_strVal;
		int nSep = strDescription.Find(_T(" - "));
		if( nSep != -1 )
			strDescription = strDescription.Right(strDescription.GetLength() - nSep - 3);

		m_prevStrListReturned[nCfg] = (const char *)strDescription;
		m_prevListReturned[nCfg].szId = (const char *)m_prevStrListReturned[nCfg];

		// is this a 'native' supported config.?
		BOOL fSupported = TRUE;	// default is TRUE 
	
		// FUTURE: Do we need this for exe projects at all!
		if (!pProject->m_bProjIsExe)	// exe projects *are* supported!
		{
			VERIFY((pProp = pBag->FindProp(P_ProjOriginalType)) != (CProp *)NULL);
			ASSERT (pProp->m_nType == string);

			CProjType * pprojtype;
			if (g_prjcompmgr.LookupProjTypeByName(((CStringProp *)pProp)->m_strVal, pprojtype))
				fSupported = pprojtype->IsSupported() && pprojtype->GetPlatform()->GetBuildable();
		}
		m_prevListReturned[nCurCfg].val = fSupported;	// supported?
	}
	

	m_prevListReturned[nCurCfg].szId = NULL;	// terminate list
	m_prevListReturned[nCurCfg].val = 0;		// terminate list

	return m_prevListReturned;
}

////////////////////////////////////////////////////////////
// CPlatformEnum

CPlatformEnum g_thePlatformEnum;	// The one and only CPlatformEnum

CEnumerator * CPlatformEnum::GetList()
{
	int			nPlatform, nCountPlatforms;
	CPlatform *	pPlatform;

	// Free the last list returned, if any.
	if (m_prevListReturned != NULL)
		delete [] m_prevListReturned;

	// Now form the new list and return it.
	nCountPlatforms = g_prjcompmgr.GetPlatformCount();
	m_prevListReturned = new CEnumerator[nCountPlatforms + 1];
	nPlatform = 0;

	g_prjcompmgr.InitPlatformEnum();
	while (g_prjcompmgr.NextPlatform(pPlatform))
	{
		if (!pPlatform->IsSupported()) continue;
		m_prevListReturned[nPlatform].szId = (const TCHAR *)*pPlatform->GetUIDescription();
		m_prevListReturned[nPlatform].val = nPlatform;

		++nPlatform;
	}
	m_prevListReturned[nPlatform].szId = NULL;	// terminate list
	m_prevListReturned[nPlatform].val = 0;		// terminate list

	return m_prevListReturned;
}
