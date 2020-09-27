//
// CProjComponentMgr, CProjType
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop

#include "version.h"	// version info.
#include "schmztl.h"
#include "project.h"
#include "bldpack.h"
#include "bldrcvtr.h"

IMPLEMENT_DYNCREATE(CBldSysCmp, CSlob)
IMPLEMENT_DYNCREATE(CPlatform, CBldSysCmp)
IMPLEMENT_DYNCREATE(CProjType, CBldSysCmp)
IMPLEMENT_DYNAMIC(CProjTypeUnknown, CProjType)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// define the global tool option engine (parsing & generation)
COptionTable  *g_pPrjoptengine = NULL;

// define the global project configuration manager
CProjComponentMgr *g_pPrjcompmgr = NULL;

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

	VSASSERT(!strNewConfig.IsEmpty(), "Config name cannot be empty!");

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

				pProp = pPropBag->FindProp(P_ProjConfiguration);
				VSASSERT(pProp && pProp->m_nType == string, "P_ProjConfiguration missing or has wrong prop type!");

				if ( (pProp == NULL) || (((CStringProp *)pProp)->m_strVal == strNewConfig) )
					continue;	// Don't need to check just-created config

				
				if ((pProp = pPropBag->FindProp(P_OutDirs_Intermediate)) != NULL)
				{
					VSASSERT(pProp->m_nType == string, "P_OutDirs_Intermediate has wrong prop type!");
					if ( ((CStringProp *)pProp)->m_strVal == strResult )
					{
						bUnique = FALSE;
						break;
					}
				}

				if ( (pProp = pPropBag->FindProp(P_OutDirs_Target)) != NULL )
				{
					VSASSERT(pProp->m_nType == string, "P_OutDirs_Target has wrong prop type!");
					if ( ((CStringProp *)pProp)->m_strVal == strResult )
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
		strResult += (TCHAR)('0'+nTweak); // add a digit
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

BOOL CBldSysCmp::SynchronizeWithRegistry()
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
	if ((RegQueryValueEx(hKey, (LPTSTR)szVersion, NULL, &dwType,
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
	}

	// now write out our data for this component, base component data first
	CVCPtrList mapStack;	// map stack for remembering position in map
	CString strValue;	// name of value in registry
	CString strSubKey;	// name of possible sub-key

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
				if (RegGetKey(hKey, (LPCTSTR)pregmap->szRegSubKey, &hSubKey) != ERROR_SUCCESS)
					goto NextInRegMap;	// failed goto next one
			}

			// do we have this value resource?
			strValue = (LPCTSTR)pregmap->szRegField;

			LPBYTE lpbData = (LPBYTE)this + (DWORD_PTR)pregmap->pvData;
			
			// make sure we have valid data
			VSASSERT(pregmap->regType == CRegistryMap::MapFData || AfxIsValidAddress(lpbData, pregmap->cbData), "Invalid registry data");

			DWORD dwType, cbData;	// retrieved from registry using RegDataExists()
			if (RegDataExists(pregmap, hSubKey, &strValue, &dwType, &cbData))
			{
				BOOL bOK = LoadRegData(pregmap, hSubKey, &strValue, lpbData, cbData);
				VSASSERT(bOK, "Failed to load registry data!");
  			}

			// do we have a sub-key to close?
			if (pregmap->szRegSubKey != NO_SUB_KEY)
				RegCloseKey(hSubKey);
		}

		NextInRegMap:

		// next reg. map entry
		pregmap++;
	}

	// close the key
	RegCloseKey(hKey);

	return TRUE;		// ok
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
	if( pregmap )
	{
	    if (pregmap->szRegSubKey != NO_SUB_KEY)
	    {
		    // do we have this subkey resource?
		    CString strSubKey = pregmap->szRegSubKey;	// name of possible sub-key
    
		    fRet = RegGetKey(hKey, (LPCTSTR)(const TCHAR *)strSubKey, phkey, fCreate) == ERROR_SUCCESS;
		    
		    // always close the parent key
		    RegCloseKey(hKey);
	    }
	}
	else
	{
	    fRet = FALSE;	
	}

	return fRet;	// success
}

BOOL CBldSysCmp::LoadRegData(UINT nRegId)
{
	const CRegistryMap * pregmap = GetRegMapEntry(nRegId);
	VSASSERT(pregmap != (const CRegistryMap *)NULL, "Failed to find reg entry to load reg data from");

	HKEY hDataKey;
	// n.b don't create key if doesn't exist already
	if (!RegGetDataKey(pregmap, &hDataKey, FALSE))
		return FALSE;

	// do we have this value resource?
	CString strValue = pregmap->szRegField;	// name of value in registry

	LPBYTE lpbData = (LPBYTE)this + (DWORD_PTR)pregmap->pvData;
	
	// make sure we have valid data
	VSASSERT(pregmap->regType == CRegistryMap::MapFData || AfxIsValidAddress(lpbData, pregmap->cbData), "Invalid registry data");

	DWORD dwType, cbData;	// retrieved from registry using RegDataExists()
	if (RegDataExists(pregmap, hDataKey, &strValue, &dwType, &cbData))
		LoadRegData(pregmap, hDataKey, &strValue, lpbData, cbData);

	// close the data key
	RegCloseKey(hDataKey);

	return TRUE;	// success
}

const CRegistryMap * CBldSysCmp::GetRegMapEntry(UINT nRegId)
{
	CVCPtrList mapStack;	// map stack for remembering position in map
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
	VSASSERT(!fRet || *pdwType == MapRegType(pregmap->dwType), "Reg data type mismatch");
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
	VSASSERT(lpbData != (BYTE *)NULL, "forgot to implement 'special' override?");

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

const TCHAR * CBldSysCmp::GetCompRegKeyName(BOOL fInclAppKey)
{
	if (m_fCompRegKeyDirty)
	{
		DWORD bsc_id = m_dwId;
		CBldSysCmp * pcomp;
		CString str;
	 	TCHAR * pchType=NULL;

		m_strCompRegKey.Empty();
		do
		{
			// get our component name
			if (g_pPrjcompmgr->LookupBldSysComp(bsc_id, pcomp))
				str = *pcomp->GetName();			// component name
			else
				{VSASSERT(FALSE, "Failed to load bld system component"); return _TEXT("<Failed>");}		// failed!

			// get our component type
			if (pcomp->IsKindOf(RUNTIME_CLASS(CPlatform)))
				pchType = szPlatforms;

			else if (pcomp->IsKindOf(RUNTIME_CLASS(CProjType)))
				pchType = szTargets;

			else if (pcomp->IsKindOf(RUNTIME_CLASS(CBuildTool)))
				pchType = szTools;

			else if (pcomp->IsKindOf(RUNTIME_CLASS(COptionHandler)))
				VSASSERT(FALSE, "How did we get an option handler here?");						// shouldn't be here!

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
		m_strFullCompRegKey = _T("Software\\Microsoft\\Shared Tools\\"); //072199 ::GetRegistryKeyName();
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
	BOOL bOK = strMode.LoadString(nIDMode);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
	strConfig =  strName + _T(" - ") + strPlatformUIDescription + _T(' ') + strMode;
}

CProjComponentMgr::CProjComponentMgr()
{
	// init. enumerator info.
	m_posProjType = (VCPOSITION)NULL;			// no next projtype
	m_posTool = (VCPOSITION)NULL;				// no next build tool

	m_idOptPropBase = PROJ_BUILD_OPTION_FIRST;	// our next option prop id.
	m_nPropFirst = m_nPropLast = (UINT)-1;
	m_popthdlrCache = (COptionHandler *)NULL;

	// retrieve other information about the environment components might use
	CString strKeyName;
	HKEY hKey;

	// do we have ODBC installed?
	m_fODBCInstalled = TRUE;	// default is yes (error in favour of ODBC)

	// do we have MFC installed?
	m_fMFCInstalled = TRUE;		// default is yes (error in favour of MFC)

	strKeyName = _T("Software\\Microsoft\\Shared Tools\\"); //072199 GetRegistryKeyName();
	strKeyName += _TEXT("\\General");
	if (RegOpenKeyEx(HKEY_CURRENT_USER, strKeyName, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwType, dwValue, dwValueSize = sizeof(DWORD);
		if (RegQueryValueEx(hKey, _TEXT("MFCInstalled"), NULL, &dwType, (LPBYTE)&dwValue, &dwValueSize) == ERROR_SUCCESS)
		{
			// make sure we get the value we expect
			VSASSERT(dwType == REG_DWORD, "Unexpected reg data type");
			VSASSERT(dwValueSize == sizeof(DWORD), "Unexpected reg data size");
			m_fMFCInstalled = dwValue != 0;
		}
		if (RegQueryValueEx(hKey, _TEXT("ODBCInstalled"), NULL, &dwType, (LPBYTE)&dwValue, &dwValueSize) == ERROR_SUCCESS)
		{
			// make sure we get the value we expect
			VSASSERT(dwType == REG_DWORD, "Unexpected reg data type");
			VSASSERT(dwValueSize == sizeof(DWORD), "Unexpected reg data size");
			m_fODBCInstalled = dwValue != 0;
		}
		RegCloseKey(hKey); // close the key we just opened
	}
	m_fInit = FALSE;
}

CProjComponentMgr::~CProjComponentMgr()
{
 	Clean();
}

void CProjComponentMgr::Clean()
{
	// delete out unknown project objects
	VCPOSITION pos = m_lstUnkProjObjs.GetHeadPosition();
	CObject * pobj;
	while (pos != (VCPOSITION)NULL)
	{
		pobj = (CObject *)m_lstUnkProjObjs.GetNext(pos);
		ASSERT_VALID(pobj);
		delete pobj;	// delete the object
	}
	// make sure the list is now empty
	m_lstUnkProjObjs.RemoveAll();

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
	DWORD rkey;
	UINT iDTOROrder = 0;

	while (iDTOROrder < sizeof(rgDTOROrder) / sizeof(CRuntimeClass *))
	{
		CRuntimeClass * pRC = rgDTOROrder[iDTOROrder];

		pos = m_mapBldSysCmps.GetStartPosition();
		while (pos != (VCPOSITION)NULL)
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

extern "C" typedef BOOL(FAR PASCAL *INITPACKAGEPROC)(CObList *);
extern "C" BOOL PASCAL InitPackage(CObList *lstPackages);
CBuildPackage * g_pBuildPackage;

BOOL CProjComponentMgr::FInit(IVCProjConvert *pConverter)
{
	// first off, ask the packages to register their components
	// we'll need to arrange the list of packages ourselves

	CPackage *pBuildPackage;
	pConverter->GetPackage((long **)&pBuildPackage);
	VSASSERT(pBuildPackage, "No build package!  Deep trouble.");
	if (pBuildPackage)
	{
		lstArngdPackages.AddTail(pBuildPackage);
		g_pBuildPackage = (CBuildPackage *)pBuildPackage; // need this for Term();
	}

	InitPackage(&lstArngdPackages); // call CIntelx86AddOn's InitPackage...

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
	}

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
		}
	}

	// ask for any tools that may wish to be a part of target type
	CProjType * pProjType;
	InitProjTypeEnum();
	while (NextProjType(pProjType))
	{
		pos = lstArngdPackages.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CPackage * pPackage = (CPackage  *)lstArngdPackages.GetNext(pos);
			m_dwRegisterId = pProjType->CompId();	// for each target
			(void) pPackage->DoBldSysCompRegister(this, BLC_Tool, m_dwRegisterId);
		}
	}

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
				VSASSERT(!bFoundPrimary, "Found more than one primary platform!");
				bFoundPrimary = TRUE;
			}
		}
	}

	// create our hashed index into the project types
	CProjType* pprojtype;
 	InitProjTypeEnum();
	while (NextProjType(pprojtype))
	{
		m_mapProjTypes.SetAt(*(pprojtype->GetName()), pprojtype);
	}

	return S_OK;
}

void CProjComponentMgr::Term()
{
	POSITION vcpos;
	vcpos = lstArngdPackages.GetHeadPosition();
	while (vcpos)
	{
		CPackage * pPackage = (CPackage  *)lstArngdPackages.GetNext(vcpos);
		if (pPackage != (CPackage *)g_pBuildPackage)
			delete pPackage;
	}
	lstArngdPackages.RemoveAll();

	Clean();
}

WORD CProjComponentMgr::GenerateBldSysCompPackageId(const TCHAR * szPackageName)
{	
	// szAddOnGeneric is zero
	if (szPackageName == szAddOnGeneric)
		return 0;

	// do we have this package id already?
	INT_PTR iAddOn = m_rgstrAddOns.GetSize();
	while (iAddOn > 0)
	{
		iAddOn--;
		if (_tcscmp((TCHAR *)(const TCHAR *)m_rgstrAddOns[iAddOn], (TCHAR *)szPackageName) == 0)
			return (WORD)(iAddOn + 1);
	}

	// new package id
	return (WORD)(m_rgstrAddOns.Add(szPackageName) + 1);	// zero is reserved
}

int CProjComponentMgr::GetSupportedPlatformCount()
{
	int count = 0;
	VCPOSITION pos = m_lstPlatforms.GetHeadPosition();
	while (pos != NULL)
	{
		if (((CPlatform *)m_lstPlatforms.GetNext(pos))->IsSupported())
			count++;
	}
	return count;
}

BOOL CProjComponentMgr::RegisterBldSysComp(CBldSysCmp * pcomp, DWORD bsc_id)
{
	BOOL fInRegistry = TRUE;

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
	VSASSERT(!LookupBldSysComp(pcomp->CompId(), pcomp), "Build system component added more than once!");

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

		case BLC_Tool:
			break;

		case BLC_OptionHdlr:
			RegisterOptionHandler((COptionHandler *)pcomp);
			fInRegistry = FALSE;		// not in registry!
			break;

		default:
			VSASSERT(FALSE, "Unknown bldsys component");	// unknown bldsys component
			return FALSE;	// failure
	}

	// for quick retrieval use a map of id->component
	m_mapBldSysCmps.SetAt(pcomp->CompId(), pcomp);

	// make sure that the component and system registry are in sync.
	if (fInRegistry && !pcomp->SynchronizeWithRegistry())
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

	else if (pcomp->IsKindOf(RUNTIME_CLASS(CBuildTool)))
		type = BLC_Tool;

	else if (pcomp->IsKindOf(RUNTIME_CLASS(COptionHandler)))
		type = BLC_OptionHdlr;

	return type;	// return our type
}

// generic object removal
BOOL CProjComponentMgr::DeregisterComponent(CVCPtrList & lst, CObject * ptr)
{
	// remove it from our list
	VCPOSITION pos = lst.Find(ptr);
	if (pos != (VCPOSITION)NULL)
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
		InitBuildToolEnum();
		while (NextBuildTool(pTool))
		{
			const TCHAR * pc = pTool->m_strToolPrefix;
			int len = lstrlen(pc);
			if (len && !_tcsnicmp(pPrefix, pc, min(nPrefixLen, len)))
				return pTool;
		}
	}

	return (CBuildTool *)NULL;
}

// tool option management
void CProjComponentMgr::RegisterOptionHandler(COptionHandler * popthdlr)
{
	popthdlr->SetOptPropBase(m_idOptPropBase);
	m_idOptPropBase += popthdlr->GetOptPropRange();

	// make sure we don't fall out of our range
	VSASSERT(m_idOptPropBase < PROJ_BUILD_OPTION_PRIVATE_FIRST, "base prop out of range");
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
	VSASSERT(mode == MOB_Debug || mode == MOB_Release, "Illegal mode");
	CString strMode;
	BOOL bOK = strMode.LoadString(mode == MOB_Debug ? IDS_DEBUG_CONFIG : IDS_RELEASE_CONFIG);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");

	// retrieve the CProjType for the original project type for this project
	CProjType * pProjType;
	if (!GetProjTypefromProjItem((CProjItem *)pProject, pProjType))
		return FALSE;	// can't get the project type for this project item

	// preserve the option handler and current slob for 'option engine'
	COptionHandler * popthdlrOld = g_pPrjoptengine->GetOptionHandler();

	// enumerate through all of our known tools and then ask the project type
	// to provide the default option string
	// we'll then use the tools' option table to parse this string into
	// tool option properties
	CBuildTool * pBuildTool;
	const CVCPtrList *pList = pProjType->GetToolsList ();

	for (VCPOSITION pos = pList->GetHeadPosition (); pos != NULL;)	
	{
		pBuildTool = (CBuildTool *) pList->GetNext(pos);

		// ignore unknown tools
		if (!pBuildTool->IsSupported())
			continue;

		// ignore some tools, such as the pass through tool
		UINT bsc_id = pBuildTool->CompId();

		CString strOption; // don't move this line
		VSASSERT(strOption.IsEmpty(), "Brand-new string not empty?!?");

		// can the tool handle this?
		if (!pBuildTool->GetDefaultToolOptions(pProjType->CompId(), pProjType->GetAttributes(), mode, strOption))
			// no, so default to the target type - must be able to
			pProjType->GetDefaultToolOptions(bsc_id, strMode, strOption);

		// parse the default options into the property bag
		g_pPrjoptengine->SetOptionHandler(pBuildTool->GetOptionHandler());
		g_pPrjoptengine->SetPropertyBag(pProject);

		// clear before parse, behave as if we were reading from file ie.
		// we need "" to allow the setting of empty string props.
		BOOL bOK = g_pPrjoptengine->ParseString(strOption, OBClear | OBNeedEmptyString);
		VSASSERT(bOK, "Failed to parse option string!");
	}

	// reset current option handler
	g_pPrjoptengine->SetOptionHandler(popthdlrOld);

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
END_REGISTRY_MAP()
#undef theClass

CPlatform::CPlatform(PlatformInfo * pInit)
{
	m_bSupported = TRUE;
	m_bBuildable = TRUE;

	m_nIDUIDesc = pInit->nIDUIDesc;
	m_nUniqueId = pInit->nUniqueId;
	m_nIDUIShortDesc = pInit->nIDUIShortDesc;

	// our component name
	m_strCompName = pInit->szName;

	// m_strUIDesc will be set up in FInit() method
	// m_strUIShortDesc will be set up in FInit() method if defined

	// initializing ToolInfo
	m_rgToolInfo.strRCExtension = pInit->szRCExtension;

	// our attributes
	m_dwAttributes = pInit->dwPlatformAttrib;

	// use MFC default
	m_nUseMFCDefault = pInit->nUseMFCDefault;

	VSASSERT(m_nIDUIDesc, "No UI description ID!");
	VSASSERT(m_nUniqueId < 256, "Unique ID too big!");
	
	// load our platform description strings
	BOOL bOK = TRUE;
	if (m_nIDUIDesc)
	{
		bOK = m_strUICompName.LoadString(m_nIDUIDesc);
		VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
	}
	if (m_nIDUIShortDesc)
	{
		bOK = m_strUIShortDesc.LoadString(m_nIDUIShortDesc);
		VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
	}
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
	m_dwAttributes = 0;	

 	// initializing ToolInfo
	m_rgToolInfo.strRCExtension.Empty();

	VSASSERT(m_nUniqueId < 256, "Unique ID too big!");

	g_pPrjcompmgr->RegisterPlatform(this);
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
	return TRUE;
}

BOOL CPlatform::SupportsODBC() const
{
	return g_pPrjcompmgr->ODBCIsInstalled() && ((GetAttributes() & PIA_Allow_ODBC) != 0);
}

UINT CPlatform::GetUseMFCDefault()
{
	// FUTURE: remove this and just use the GetAttributes() & PIA_AllowMFCInDLL
	return g_pPrjcompmgr->MFCIsInstalled() ? m_nUseMFCDefault : NoUseMFC;
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
	g_pPrjcompmgr->DeregisterPlatform(this);
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
						_T(""),
						_T("Generic Project")
						};

CProjType::CProjType(const TCHAR * szPkg, WORD id,
				     UINT nIDType, UINT nUniqTypeId,
			  		 const TCHAR * szPkgPlatform, WORD idPlatform)
{
	this->CProjType::CProjType(szPkg, id, nIDType, nUniqTypeId,
						 GenerateComponentId(g_pPrjcompmgr->GenerateBldSysCompPackageId(szPkgPlatform), idPlatform));
}

CProjType::CProjType(const TCHAR * szPkg, WORD id,
					   UINT nIDType, UINT nUniqTypeId,
					   DWORD bsc_id)
{
	// set our id.
	SetId(GenerateComponentId(g_pPrjcompmgr->GenerateBldSysCompPackageId(szPkg), id));

	VSASSERT(nUniqTypeId < 256, "Unique ID too big!");

	// save the our name away
	m_nIDType = nIDType;
	VSASSERT(m_nIDType, "ID type not set!");
	m_nUniqueTypeId = nUniqTypeId & 255;
	VSASSERT(m_nUniqueTypeId < 11, "Unique type ID too big; bump array size and this assert to fix");	// bump array size if this fires

	if (!g_pPrjcompmgr->LookupBldSysComp(bsc_id, (CBldSysCmp *&)m_pPlatform))
		VSASSERT(FALSE, "Failed to find desired bldsys component!");	// failed

	// make sure we got a valid platform
	ASSERT_VALID(m_pPlatform);
	VSASSERT(m_pPlatform->IsKindOf(RUNTIME_CLASS(CPlatform)), "Invalid platform!");

	m_strType = TypeNameFromId[m_nUniqueTypeId];

	// our component name
	m_strCompName = MakeNameFromPlatformAndType(*(m_pPlatform->GetName()), m_strType);
}

// Useful for dynamic creation of CProjTypes
CProjType::CProjType(const _TCHAR * lpszTypeName, UINT nUniqueTypeId, CPlatform * pPlatform)
{
	VSASSERT(nUniqueTypeId < 256, "Unique ID too big!");
	
	m_nIDType = 0;
	m_strType = lpszTypeName;
	m_strUIType = lpszTypeName;
	m_nUniqueTypeId = nUniqueTypeId & 255;
	m_pPlatform = pPlatform;

	// our component name
	m_strCompName = MakeNameFromPlatformAndType(*(m_pPlatform->GetName()), m_strType);

	// register ourselves with the config. manager
	g_pPrjcompmgr->RegisterProjType(this);
}

BOOL CProjType::FInit()
{
	BOOL retval = m_nIDType ? m_strUIType.LoadString(m_nIDType) : TRUE;
	VSASSERT(m_strUIType.GetLength() > 0, "UI type name must not be blank!");
	if (retval)
		m_strUICompName = MakeNameFromPlatformAndType(*(m_pPlatform->GetUIDescription()), m_strUIType);
	if( m_strUIType != "Makefile")
	{
		AddTool(szAddOnGeneric, BCID_Tool_CustomBuild);   
		AddTool(szAddOnGeneric, BCID_Tool_SpecialBuild);   
	}

	return retval;
}

void CProjType::AddTool(const TCHAR * szPckg, WORD idTool)
{
	CBuildTool * pTool = NULL;
	if (!g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(g_pPrjcompmgr->GenerateBldSysCompPackageId((TCHAR *)szPckg), idTool),
									   (CBldSysCmp *&)pTool))
		VSASSERT(FALSE, "Tool not in list!" );	// failed

	// now add it
	AddTool(pTool);
}

void CProjType::AddTool(WORD pckg, WORD idTool)
{
	CBuildTool * pTool = NULL;
	if (!g_pPrjcompmgr->LookupBldSysComp(GenerateComponentId(pckg, idTool), (CBldSysCmp *&)pTool))
		VSASSERT(FALSE, "Tool not in list!");	// failed

	// now add it
	AddTool(pTool);
}

void CProjType::AddTool(CBuildTool * pTool)
{
	// make sure we got a valid tool
	ASSERT_VALID(pTool);
	VSASSERT(pTool->IsKindOf(RUNTIME_CLASS(CBuildTool)), "Tool isn't a CBuildTool!");

	// don't add if already there
	if (m_ToolsList.Find(pTool) != (VCPOSITION)NULL)
		return;

	// FUTURE: remove this ordering when it doesn't matter
	// ie. whe we have a dependency based build engine
	// for the old v2 build engine CSchmoozeTool's must be last

	if (pTool->IsKindOf(RUNTIME_CLASS(CSchmoozeTool)))
	{
		// Find location of CCustomBuildTool or CSpecialBuildTool
		VCPOSITION posSpecial = m_ToolsList.GetHeadPosition();
		while (NULL!=posSpecial)
		{
			// Save posLast which will be the position of pSpecailTool.
			VCPOSITION posLast = posSpecial;

			CBuildTool* pSpecialTool = (CBuildTool*)m_ToolsList.GetNext(posSpecial);
			VSASSERT(NULL!=pSpecialTool, "Failed to find special tool!");
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
	g_pPrjcompmgr->DeregisterProjType(this);
}

void CProjType::ClearUnknownTools()
{
	CBuildTool * pTool;
	// remove tools, deleting any dynamic ones
    VCPOSITION pos = m_ToolsList.GetHeadPosition();
    while (pos != NULL)
	{
		VCPOSITION posCurr = pos;
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
	pProject->SetIntProp(P_Proj_ComPlus, FALSE);

	return TRUE;
}

BOOL CProjType::SetDefaultReleaseTargetOptions(CProject* pProject, CTargetItem * pTarget, BOOL fUseMFC)
{
	return SetDefaultDebugTargetOptions(pProject, pTarget, fUseMFC);
}

CBuildTool * CProjType::GetToolFromCodePrefix(const TCHAR * pPrefix, int nPrefixLen)
{
	CBuildTool * pTool;
	VCPOSITION pos = m_ToolsList.GetHeadPosition();
	while (pos != (VCPOSITION)NULL)	
	{
		pTool = (CBuildTool *)m_ToolsList.GetNext(pos);

		const TCHAR * pc = pTool->m_strToolPrefix;
		int len = lstrlen(pc);
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

	const CVCPtrList * plistTools = GetToolsList();
	
	// preserve the option handler and current slob for 'option engine'
	COptionHandler * popthdlrOld = g_pPrjoptengine->GetOptionHandler();
	CSlob * pslobOld = g_pPrjoptengine->GetPropertyBag();

	g_pPrjoptengine->SetPropertyBag(pItem);

	// reset
	if( pslobOld )
		g_pPrjoptengine->SetPropertyBag(pslobOld);
	if( popthdlrOld )
		g_pPrjoptengine->SetOptionHandler(popthdlrOld);
}

CString CProjType::MakeNameFromPlatformAndType(const CString & platform, const CString & type)
{
	return platform + _TCHAR(' ') + type;
}

CString CProjType::MakeNameFromPlatformAndType(const _TCHAR * platform, const _TCHAR * type)
{
	return (CString)platform + _TCHAR(' ') + type;
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

	BOOL bOK = strOption.LoadString(nIDCommon);
	VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
	if (nIDMode != (UINT)-1)
	{
		bOK = strPart.LoadString(nIDMode);
		VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
		strOption += _T(' '); strOption += strPart;
	}
	if (nIDOption != (UINT)-1)
	{
		bOK = strPart.LoadString(nIDOption);
		VSASSERT(bOK, "Failed to load a string!  Are the resources initialized properly?");
		strOption += _T(' '); strOption += strPart;
	}
}
