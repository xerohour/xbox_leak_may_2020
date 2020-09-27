//	PROJITEM.CPP
//
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop

#include "projcomp.h"
#include "resource.h"
#include "cursor.h"
#include "mmsystem.h"
#include "exttarg.h"
#include "bldrfile.h"	// our CBuildFile
#include "bldslob.h"
#include "bldnode.h"
#include "optndlg.h"    // for build.settings tree ctl
#include "toolcplr.h"
#include "toolrc.h"
#include "util.h"
#include "oleref.h"

#include "image.h"

#include <prjapi.h>
#include <prjguid.h>
#include <srcguid.h>
#include <utilauto.h>	// For Automation CAutoApp

IMPLEMENT_SERIAL (CProjItem, CProjSlob, SLOB_SCHEMA_VERSION)
IMPLEMENT_SERIAL (CFileItem, CProjItem, SLOB_SCHEMA_VERSION)
IMPLEMENT_SERIAL (CDependencyFile, CProjItem, SLOB_SCHEMA_VERSION)
IMPLEMENT_DYNAMIC (ConfigurationRecord, CObject)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


#include "icapexp.h"

bool
IsCrashDumpEnabled(
	);

// Cache used to speed up reading of dependency files from the makefile.
// See code in CProjItem::SuckMacro().
// FUTURE (karlsi): this should be allocated dynamically just for makefile reading
CMapStringToPtr	*g_pMakReadDepCache;

// Since INCLUDE dep paths are per platform, we need to maintain a seperate
// cache per platform here.
CMapStringToPtr *g_pMakReadIncludeDepCache;

// this variable is part of a MAJOR HACK involving the BuildCmds Macro and
// unsupported platforms in makefiles. find a betterr work around for this problem ASAP
// olympus bug #1103 for details.
CBuildTool *g_pBuildTool=NULL;

// 04-10-96 jimsc This is used by SuckMacro, but ReadMakeFile has to set it in project.cpp
CBuildTool *g_pLastTool = NULL;

// HACK: used to preserve formated name of current source file during writing
static CString strSrc;
extern BOOL g_bHTMLLog;

#ifdef _DEBUG

// These variables keep track of the value of the P_D_ properties used
// to implement the "projinfo" property page.
int			g_D_nPropIdCur = -1;
int			g_D_nPropBag = 0;
CString		g_D_strTargetCur;

BOOL AFX_DATA_EXPORT bDoFileTrace   	= FALSE;
BOOL AFX_DATA_EXPORT bDoConfigTrace 	= FALSE;
BOOL AFX_DATA_EXPORT bDoInformTrace 	= FALSE;
BOOL AFX_DATA_EXPORT bDoScanTrace   	= FALSE;

// Our own debug output 'cause MFC sucks (crashes when line is too long)

#define DBG_BUFSIZE	1024
#define MAX_OUTPUT  249		// Max chars debugger can show in output window
void CDECL PrjTrace(LPCSTR pszFormat, ...)
{
	int nBuf;
	TCHAR szBuffer[DBG_BUFSIZE];
	const TCHAR* pszLocalFormat;

	pszLocalFormat = pszFormat;

	va_list args;
	va_start(args, pszFormat);

	nBuf = _vsnprintf(szBuffer, DBG_BUFSIZE, pszLocalFormat, args);
	szBuffer[DBG_BUFSIZE-1] = 0;
	ASSERT(nBuf < DBG_BUFSIZE);

    // The following code is not DBC aware.  Don't TRACE
    // with mbcs strings!

    char szLine[MAX_OUTPUT];
    char *pszTo, *pszFrom;

    pszFrom = szBuffer;
    pszTo = szLine;
    while( *pszFrom )
    {
        if( (szLine + sizeof(szLine) - pszTo) < 3 )
        {
            *pszTo = '0';
            OutputDebugString(szLine);
            OutputDebugString("\r\n");
            pszTo = szLine;
        }
        *pszTo++ = *pszFrom++;
        if( *(pszFrom-1) == '\n' )
            *pszTo++ = '\r'; 
    }
    if( pszTo != szLine )
    {
        *pszTo = '\0';
        OutputDebugString(szLine);
    }

	va_end(args);
}

#endif

// Remember to free() the return value!
TCHAR *StripQuotes(LPCTSTR szFilePath)
{
	const TCHAR * pchLookup = szFilePath;
	TCHAR *pchSet = new TCHAR[lstrlen(szFilePath) + 1];
	TCHAR *pchRet = pchSet;

	ASSERT(pchSet != NULL);

	while (*pchLookup != _T('\0'))
	{
		if (*pchLookup != _T('"'))
 		{
			_tccpy(pchSet, pchLookup);
			pchSet = _tcsinc(pchSet);
 		}
		pchLookup = _tcsinc(pchLookup);
	}

    *pchSet = _T('\0');
	return pchRet;
}

///////////////////////////////////////////////////////////////////////////////
// Dependency cache management.

// These caches are used so we don't go around asking for fullpaths for filenames
// every time we read new dependency information.

VOID InitDependencyCaches( )
{
	ASSERT(g_pMakReadDepCache == NULL);
	ASSERT(g_pMakReadIncludeDepCache == NULL);

	g_pMakReadDepCache = new CMapStringToPtr;
	g_pMakReadDepCache->InitHashTable(199);

	CDirMgr *pDirMgr = GetDirMgr();
	int cToolset = pDirMgr->GetNumberOfToolsets();
	ASSERT(cToolset > 0);
	
	g_pMakReadIncludeDepCache = new CMapStringToPtr[cToolset];

	for ( int i = 0; i < cToolset ; i++ )
		g_pMakReadIncludeDepCache[i].InitHashTable(97);
}


VOID FreeDependencyCaches( )
{
#ifdef REFCOUNT_WORK
	POSITION pos = NULL;
	CString strKey;
	CDirMgr *pDirMgr = GetDirMgr();
	int cToolset = pDirMgr->GetNumberOfToolsets();
	ASSERT(cToolset > 0);

	if (NULL!=g_pMakReadIncludeDepCache)
	{
		for ( int i = 0; i < cToolset ; i++ )
		{
			pos = g_pMakReadIncludeDepCache[i].GetStartPosition();
			while (NULL!=pos)
			{
				FileRegHandle frh = NULL;
				g_pMakReadIncludeDepCache[i].GetNextAssoc( pos, strKey, (void*&)frh );
				ASSERT(NULL!=frh);
				frh->ReleaseFRHRef();
			}
		}
	}

	pos = (NULL==g_pMakReadDepCache)?NULL:g_pMakReadDepCache->GetStartPosition();
	while (NULL!=pos)
	{
		FileRegHandle frh = NULL;
		g_pMakReadDepCache->GetNextAssoc( pos, strKey, (void*&)frh );
		ASSERT(NULL!=frh);
		frh->ReleaseFRHRef();
	}
#endif
	delete g_pMakReadDepCache;
	delete [] g_pMakReadIncludeDepCache;

	g_pMakReadDepCache = NULL;
	g_pMakReadIncludeDepCache = NULL;
}

//
// Helper function that test for a container being non-NULL or that the
// undo slob is recording(which mean that the undo slob could be the container)
//
static __inline BOOL CouldBeContainer(CSlob* pContainer)
{
	return (NULL!=pContainer || theUndoSlob.IsRecording());
}

// CStack Tiny Stack 
void CStack::Push( void * data ) { 
	SNode *tmp = new SNode; 
	tmp->next = head; 
	tmp->data = data;
	head = tmp;
}



///////////////////////////////////////////////////////////////////////////////
//
// ------------------------- ConfigurationRecord ------------------------------
//
///////////////////////////////////////////////////////////////////////////////
DWORD ConfigurationRecord::nSignature = 0;
int ConfigurationRecord::nCongfigCacheCount = 0;

ConfigurationRecord::ConfigurationRecord
(
	const ConfigurationRecord * pBaseRecord, 
	CProjItem * pOwner
) :
  m_parryTargetFiles (NULL), m_pTargetFiles (NULL),m_pPathExtTrg(NULL),
// increase the size of the hash table for project-level,
// these bags have the most props, to a more efficient value
// than the default of 17 and reduce for CfileItems and CDepItems
#if 1
  m_BasePropBag(pOwner->GetBagHashSize()),
  m_CurrPropBag(pOwner->GetBagHashSize()),
  m_ClonePropBag(pOwner->GetBagHashSize())
#else
  m_BasePropBag(53),
  m_CurrPropBag(53),
  m_ClonePropBag(53)
#endif
{
	// Owner project item
	m_pOwner = pOwner;

	// Record linkage
	m_pBaseRecord = pBaseRecord;
	#ifdef _DEBUG
	if (pBaseRecord) ASSERT_VALID (pBaseRecord);
	#endif
	// default is *NO* attributes which are dirty
	m_iAttrib = 0;
	m_fAttribDirty = TRUE;

	// Config rec is not valid for the owning item by default
	m_fValid = FALSE;

	// Actions list is allocated on demand..
	m_pActions = (CActionSlobList *)NULL;
	m_LastUpdate = 0;
}

ConfigurationRecord::~ConfigurationRecord()
{
	// empty our target file set
#if 0	// m_setTargetFiles removed from ConfigurationRecord
	m_setTargetFiles.EmptyContent();
#endif
	delete m_pPathExtTrg;

	// delete our actions and actions list
	if (m_pActions)
	{
		POSITION pos = m_pActions->GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CActionSlob * pAction = (CActionSlob *)m_pActions->GetNext(pos);

			// must not be bound, unless it's an unknown tool
			// we *just* directly added these, so we can *just* directly
			// delete them, no CActionSlob::UnAssign or CActionSlob::Assign used!
			ASSERT(pAction->m_pTool == (CBuildTool *)NULL ||
				   pAction->m_pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)));
			delete pAction;
		}
		delete m_pActions;
	}

	// FUTURE: remove this
	FreeOutputFiles();

	// This is already done by FreeOutputFiles().
	delete m_parryTargetFiles;
	delete m_pTargetFiles;
}

CActionSlobList * ConfigurationRecord::GetActionList()
{
	if (m_pActions == (CActionSlobList *)NULL)
		m_pActions = new CActionSlobList;
	return m_pActions;
}

#if 0
CPropBag * ConfigurationRecord::GetPropBag(UINT idBag)
 {
	switch( idBag ){
		case BaseBag:
			return &m_BasePropBag;
		case CloneBag:
			return &m_ClonePropBag;
		default:
			return &m_CurrPropBag;
	}
}

void ConfigurationRecord::EmptyPropBag(UINT idBag)
 {
	switch( idBag ){
		case BaseBag:
			m_BasePropBag.Empty();
		case CloneBag:
			m_ClonePropBag.Empty();
		default:
			m_CurrPropBag.Empty();
	}
}
#endif

BOOL ConfigurationRecord::IsSupported()
{
	CProjType * pprojtype;
	
	if (!g_prjcompmgr.LookupProjTypeByName(GetOriginalTypeName(), pprojtype))
		return FALSE;

	return pprojtype->IsSupported();
}

BOOL ConfigurationRecord::IsBuildable()
{
	CProjType * pprojtype;
	
	if (!g_prjcompmgr.LookupProjTypeByName(GetOriginalTypeName(), pprojtype))
		return FALSE;

	return pprojtype->GetPlatform()->GetBuildable();
}

void ConfigurationRecord::FreeOutputFiles ()
{
	if (m_parryTargetFiles)
	{
		// Maybe only a CProject???
		ASSERT( m_pOwner->IsKindOf(RUNTIME_CLASS(CProject)) );

		delete m_parryTargetFiles;
		m_parryTargetFiles = NULL;
	}

	if (m_pTargetFiles)
	{
		// Must be project.
		ASSERT( m_pOwner->IsKindOf(RUNTIME_CLASS(CProject)) );

		m_pTargetFiles->EmptyContent();
		delete m_pTargetFiles;
		m_pTargetFiles = NULL;
	}

#ifdef REFCOUNT_WORK
	for (int i=0; i < m_arryOutputFiles.GetSize(); i++)
	{
		m_arryOutputFiles[i]->ReleaseFRHRef();
	}
#endif

	// Size = 0, GrowBy = 2. Assume that there are usually 2 pointers added.
	m_arryOutputFiles.SetSize( 0, 2 );
}

CString &ConfigurationRecord::GetConfigurationName () const
{
	ASSERT (m_pBaseRecord);
	const CPropBag * pBag = ((ConfigurationRecord *)m_pBaseRecord)->GetPropBag(CurrBag);

	CStringProp *pProp = (CStringProp *)pBag->FindProp (P_ProjConfiguration);
	ASSERT (pProp);
	ASSERT (pProp->m_nType == string);
	return pProp->m_strVal;
}

void ConfigurationRecord::GetProjectName (CString &strProject) const
{
	ASSERT (m_pBaseRecord);
	CProject * pProject = (CProject *)m_pBaseRecord->m_pOwner;
	ASSERT(pProject);
	if (pProject->m_bProjIsExe)
	{
		strProject = pProject->GetFilePath()->GetFileName();
	}
	else
	{
		const CPropBag * pBag = ((ConfigurationRecord *)m_pBaseRecord)->GetPropBag(CurrBag);

		CStringProp *pProp = (CStringProp *)pBag->FindProp (P_ProjConfiguration);
		ASSERT (pProp);
		ASSERT (pProp->m_nType == string);

	    strProject = (const char *)((CStringProp *)pProp)->m_strVal;
  	  int iDash = strProject.Find(_T(" - "));
  	  if (iDash > 0)
  	      strProject = strProject.Left(iDash);
	}

	return ;
}

void ConfigurationRecord::GetConfigurationDescription ( CString &strDescription ) const
{
	ASSERT (m_pBaseRecord);
	const CPropBag * pBag = ((ConfigurationRecord *)m_pBaseRecord)->GetPropBag(CurrBag);

	CStringProp *pProp = (CStringProp *)pBag->FindProp (P_ProjConfiguration);
	ASSERT (pProp);
	ASSERT (pProp->m_nType == string);
	strDescription = pProp->m_strVal;

    strDescription = (const char *)((CStringProp *)pProp)->m_strVal;
	int nSep = strDescription.Find(_T(" - "));
	strDescription = strDescription.Right(strDescription.GetLength() - nSep - 3 );

	return;
}



CString &ConfigurationRecord::GetOriginalTypeName() const
{
	ASSERT (m_pBaseRecord);
	const CPropBag * pBag = ((ConfigurationRecord *)m_pBaseRecord)->GetPropBag(CurrBag);

	// format is '<Platform> <ProjType>'
	CStringProp *pProp = (CStringProp *)pBag->FindProp (P_ProjOriginalType);
	ASSERT (pProp);
 	ASSERT (pProp->m_nType == string);

	return pProp->m_strVal;
}

void ConfigurationRecord::BeginConfigCache()
{
	if (nCongfigCacheCount == 0) nSignature++;
	nCongfigCacheCount++;

}
void ConfigurationRecord::EndConfigCache()
{
	nCongfigCacheCount--;
}
	  
CBuildTool * ConfigurationRecord::GetBuildTool()
{
	// don't return a build tool for non-source tools
	if (!m_pOwner->UsesSourceTool())
		return (CBuildTool *)NULL;

	// FUTURE: fix the use of this function
	// given customizable build tools we may have
	// multiple sources tools per item
	// for now we know we have one, return that
	CActionSlobList * plstActions = GetActionList();
	int c = plstActions->GetCount();
	ASSERT(c <= 1);	// must re-write this function!

	if (c == 0)
		return (CBuildTool *)NULL;	// no tools

	// return the first
	return ((CActionSlob *)plstActions->GetHead())->m_pTool;
}

int ConfigurationRecord::GetTargetAttributes()
{
	BOOL			fForce = FALSE;

	// do we need to force ourselves into the same config.?
	if (m_pOwner->GetActiveConfig() != this)
	{
		m_pOwner->ForceConfigActive(this);
 		fForce = TRUE;
	}

	BOOL fIsExternal =
		m_pOwner->IsKindOf(RUNTIME_CLASS(CProject))	&& 
		((CProject *)m_pOwner)->m_bProjIsExe;

	BOOL fIsExternalTarget = m_pOwner->IsKindOf(RUNTIME_CLASS(CProject)) &&
		((CProject *)m_pOwner)->IsExternalTarget();

	// if we're an exe project create the current target
	// and check it against the one we last got attributes for
	// if they are different then get the new set of attributes
	if (fIsExternal || fIsExternalTarget)
	{
		CPath path;
		CString strTarget;

		VERIFY(m_pOwner->GetStrProp(fIsExternalTarget ? P_Proj_Targ : P_ExtOpts_Targ, strTarget) == valid);

		path.CreateFromDirAndFilename(((CProject*)m_pOwner)->GetWorkspaceDir(), strTarget);

		// if this target is different to our current we are dirty
		if( m_pPathExtTrg == NULL )
		{
			m_pPathExtTrg = new CPath( path );
			m_fAttribDirty = TRUE;
		} 
		else if (path != *m_pPathExtTrg)
		{
			*m_pPathExtTrg = path;
			m_fAttribDirty = TRUE;
		}
	}

	// are our target attributes dirty?
	if (m_fAttribDirty)
	{
		m_iAttrib = 0;

		// for an external makefile (.EXE 'pseudo' makefile)
		// refresh the target attributes only if the target has changed
		if (fIsExternal || fIsExternalTarget)
		{
			// we need to figure out as much of the attributes as
			// we can from the PE header and if that doesn't exist
			// use the extension 

			//
			// is this a executable (eg. .exe, .dll or other executable code)?
			//
			
			BOOL		fOpened = FALSE;
			IMAGE_INFO	ImageInfo = {0};

			ImageInfo.Size = sizeof (ImageInfo);

			fOpened = GetImageInfo (*m_pPathExtTrg, &ImageInfo);

			if (fOpened) {

				if (ImageInfo.ImageType == IMAGE_TYPE_WIN32) {
				
					//
					// Ok. This is a valid Win32 subsystem image, check if
					// it's  DLL or EXE.
					//
					
					if (ImageInfo.u.Win32.Characteristics & IMAGE_FILE_DLL) {
						m_iAttrib |= ImageDLL | TargetIsDebugable;
					} else {
						m_iAttrib |= ImageExe | TargetIsDebugable;
					}

				} else if (ImageInfo.ImageType == IMAGE_TYPE_CRASHDUMP &&
						   IsCrashDumpEnabled ()) {

					//
					// Win32 Crashdump is treated like a EXE.
					//

					m_iAttrib |= ImageExe | TargetIsDebugable;
				} else if(!fIsExternalTarget) {
					CString strPlat;
					CPlatform *pPlat;
					/* Could be an XBE */
					if(m_pOwner->GetStrProp(P_ExtOpts_Platform, strPlat) &&
						g_prjcompmgr.LookupPlatformByUIDescription(strPlat,
						pPlat) && pPlat->GetUniqueId() == xbox)
					{
						/* We're Xbox, so look at the filename */
						if (FileNameMatchesExtension (m_pPathExtTrg,
								_TEXT ("xbe")))
							m_iAttrib |= (ImageExe | TargetIsDebugable);
					}
				}

				//
				// NOTENOTE: Java support has been removed.
				//
			}
				

	
			// let's cheat and use
			// the extension to at least provide *some*
			// information back as to the image type if we
			// don't already have it AND we couldn't find the image
			
			if (!fOpened && !(m_iAttrib & (ImageExe | ImageDLL | ImageStaticLib)))
			{
				if (FileNameMatchesExtension (m_pPathExtTrg, _TEXT ("exe")))
					m_iAttrib |= (ImageExe | TargetIsDebugable);
				else if (FileNameMatchesExtension (m_pPathExtTrg, _TEXT ("dll")))
					m_iAttrib |= (ImageDLL | TargetIsDebugable);
				else if (FileNameMatchesExtension (m_pPathExtTrg, _TEXT ("lib")))
					m_iAttrib |= ImageStaticLib;
				else if (FileNameMatchesExtension (m_pPathExtTrg, _TEXT ("xbe")))
					m_iAttrib |= (ImageExe | TargetIsDebugable);
			}

			if (((CProject *)m_pOwner)->m_bProjIsExe)
			{
				CString strUIDescription;

				CPlatform * pPlatform;
				if (m_pOwner->GetStrProp(P_ExtOpts_Platform, strUIDescription) == valid)
					g_prjcompmgr.LookupPlatformByUIDescription(strUIDescription, pPlatform);

				if (!pPlatform->IsSupported() || !pPlatform->GetBuildable())
					m_iAttrib &= ~TargetIsDebugable;			// only supported platforms are debuggable
			}
		}
		// for an internal project
		else if (m_fAttribDirty)
		{
			// cache our target attributes
			CProjType * pProjType = m_pOwner->GetProjType(); ASSERT(pProjType != (CProjType *)NULL);
			m_iAttrib = pProjType->GetAttributes();
			CPlatform *pPlatform = pProjType->GetPlatform();
			if (!pPlatform->IsSupported() || !pPlatform->GetBuildable())
				m_iAttrib &= ~TargetIsDebugable;        
		}

		m_fAttribDirty = FALSE;
	}


	if (fForce)
		m_pOwner->ForceConfigActive();

	return m_iAttrib;	// return the attributes
}

//
// GetTargetFiles has been changed to dynamicly create a CFileRegSet instead of returning a pointer
// to m_setTargetFiles.  Caller must delete pointer returned by this function.
//
const CFileRegSet * ConfigurationRecord::GetTargetFiles(BOOL fPrimary /* TRUE */, BOOL fAll /* FALSE */)
{
	// FUTURE: remove this hack
	// It's v.slow, for now disable file-change notification while we do
	// this, we don't need them anyway
	BOOL g_oldFileRegDisableFC = g_FileRegDisableFC;
	g_FileRegDisableFC = TRUE;

	// Ensure that we support the combination
	// FUTURE: replace these two BOOLs with one!
	ASSERT(fPrimary || fAll && !(fPrimary && fAll));

	CFileRegSet * psetTargetFiles = new CFileRegSet(11);

	// Empty our target file set
#if 0	// m_setTargetFiles removed from ConfigurationRecord
	m_setTargetFiles.EmptyContent();
#endif

	// Ensure we are in the right config.
	m_pOwner->SetManualBagSearchConfig(this);

	// Obtain the primary target file name
	// In the case of an external makefile this is simply
	// stored in the property P_ExtsOpt_Targ.
	// For an internal makefile we must loop through the
	// schmooze tools asking each one for a primary target
	// name, only one should return such a file name.

	// Our parents projtype
	CProjType * pProjType = m_pOwner->GetProjType();

	// External target, from either an internal or exe project?
	if (m_pOwner->IsKindOf(RUNTIME_CLASS(CProject)) &&
		((CProject *)m_pOwner)->m_bProjIsExe)
	{
		CString strTargetFile;
		if (m_pOwner->GetStrProp(P_ExtOpts_Targ, strTargetFile) == valid)
		{
			CPath path;
			if (path.CreateFromDirAndFilename(((CProject *)m_pOwner)->GetWorkspaceDir(), strTargetFile))
			{
#ifndef REFCOUNT_WORK
				FileRegHandle frh = g_FileRegistry.RegisterFile((const CPath *)&path);
				psetTargetFiles->AddRegHandle(frh);
#else
				FileRegHandle frh = CFileRegFile::GetFileHandle(path);
				psetTargetFiles->AddRegHandle(frh);
				frh->ReleaseFRHRef();
#endif
			}
		}
	}
	else if (m_pOwner->IsKindOf(RUNTIME_CLASS(CProject)) &&
			 pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)))
    {
        CString strTargetFile;
        if (m_pOwner->GetStrProp(P_Proj_Targ, strTargetFile) == valid)
        {
            CPath path;
            if (path.CreateFromDirAndFilename(((CProject *)m_pOwner)->GetWorkspaceDir(), strTargetFile))
            {
#ifndef REFCOUNT_WORK
                FileRegHandle frh = g_FileRegistry.RegisterFile((const CPath *)&path);
                psetTargetFiles->AddRegHandle(frh);
#else
                FileRegHandle frh = CFileRegFile::GetFileHandle(path);
                psetTargetFiles->AddRegHandle(frh);
				frh->ReleaseFRHRef();
#endif
            }
        }
    }
    else
 	{
		CProjType * pProjType;
 		VERIFY(g_prjcompmgr.LookupProjTypeByName(GetOriginalTypeName(), pProjType));

		// ignore 'Unknown' ProjTypes
		if (!pProjType->IsSupported())
			goto Exit;

		// Only if the tools want this item!
		// FUTURE: for now 'know' that SchmoozeTool's only
		// work on CProject items
		if (!this->m_pOwner->IsKindOf(RUNTIME_CLASS(CProject)))
			goto Exit;

		GraphOpFn pfn = fPrimary ? CFileDepGraph::GetPrimaryTarget : CFileDepGraph::GetAllTarget;
		DWORD dw = (DWORD)psetTargetFiles;
		g_buildengine.GetDepGraph(this)->PerformOperation(pfn, dw);
 	}

Exit:
	// Re-enable file-change notification?
	g_FileRegDisableFC = g_oldFileRegDisableFC;

	// Reset the config.
	m_pOwner->ResetManualBagSearchConfig();

	// Check that we didn't punt due to an 'unknown' target type (CProjType)
#ifndef REFCOUNT_WORK
	if ( psetTargetFiles->GetContent()->IsEmpty() )
#else
	if ( psetTargetFiles->IsEmpty() )
#endif
	{
		delete psetTargetFiles;
		psetTargetFiles = NULL;
	}
	return psetTargetFiles;
}

CFileRegSet * ConfigurationRecord::GetTargetFilesList()
{
	CHECK_CACHE
	// N.B. uses IsEmpty so that TargetFiles default to output files
	// for tools that don't currently maintain a separate targets
	// list (Source tools, etc.)
	// GetTargetFilesList should only be called ConfigurationRecord's
	// owned by a CProject.  m_pTargetFiles will be NULL for anything else.
	ASSERT( m_pOwner->IsKindOf(RUNTIME_CLASS(CProject) ) );

	// REVIEW(tomse): m_pTargetFiles can be NULL if m_pOwner->m_pOwnerTarget==NULL.
	// Should check ConfigurationRecord::Refresh() logic to make sure that
	// m_pTargetFiles is correctly not created.
//	ASSERT( NULL != m_pTargetFiles );

	// speed:3883 - GPF's because m_pTargetFiles is NULL.
	if ( NULL == m_pTargetFiles )
	{
		m_pTargetFiles = new CFileRegSet(1);
	}
	return m_pTargetFiles;
}

void ConfigurationRecord::Refresh () 
{
	// FUTURE: remove this hack
	// It's v.slow, for now disable file-change notification while we do
	// this, we don't need them anyway
	BOOL g_oldFileRegDisableFC = g_FileRegDisableFC;
	g_FileRegDisableFC = TRUE;

	BOOL fForce = FALSE;

	// Here we setup the output and target file lists
	// The property pages rely on these lists having the
	// dominant output/target at the head of the list.
	// This means that the GetOutputPaths method of
	// source tools MUST return a list with the dominant
	// output/target at the head of the list

	m_LastUpdate = nSignature;

	// do we need to force ourselves into the same config.?
	if (m_pOwner->GetActiveConfig() != this)
	{
		m_pOwner->ForceConfigActive (this);
		fForce = TRUE;
	}

 	BOOL bExternal = FALSE;
	CProjType * pProjType = NULL;

	BOOL bOwnerIsProject = m_pOwner->IsKindOf(RUNTIME_CLASS(CProject));
	
	// Are we a regular v2 external makefile
	if (bOwnerIsProject && ((CProject *)m_pOwner)->m_bProjIsExe)
		bExternal = TRUE;
 	else
		// We have to get the projtype of the internal makefile, as we may
		// be dealing with an external target
		VERIFY(g_prjcompmgr.LookupProjTypeByName(GetOriginalTypeName(), pProjType ));

    BOOL bIsExtTarget = (pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)));
	// Are we external(i.e. a v2 external makefile, or a v3 external target
	if (bExternal || (bIsExtTarget && bOwnerIsProject))
	{
		CString strTrg, strBsc, strDir;
		CPath pathTrg, pathBsc, pathTemp;
		CDir Dir, *pProjDir;

		pProjDir = &(g_pActiveProject->GetWorkspaceDir());

		FreeOutputFiles();

		// Add the exe and browser as a target:
		// NOTE, these GetStrProp's may fail when we are in the
		// process of loading a project, and so we have to be able
		// to handle failure here.
		if (m_pOwner->GetStrProp(bIsExtTarget ? P_Proj_Targ : P_ExtOpts_Targ, strTrg) != valid)
			goto Exit_Refresh;
		if (m_pOwner->GetStrProp(bIsExtTarget ? P_Proj_BscName : P_ExtOpts_BscName, strBsc) != valid)
			goto Exit_Refresh;

		// always use the workspace dir, since P_Proj_Targ has the targ dir appended alreay
 		Dir = g_pActiveProject->GetWorkspaceDir();
 
		if (pathTrg.CreateFromDirAndFilename ( Dir, strTrg ))
		{
#ifndef REFCOUNT_WORK
			m_arryOutputFiles.Add(g_FileRegistry.RegisterFile ( &pathTrg ));
#else
			m_arryOutputFiles.Add(CFileRegFile::GetFileHandle(pathTrg ));
#endif

			// m_pTargetFiles is only allocated for CProject's.
			if ( bOwnerIsProject )
			{
				if (NULL==m_pTargetFiles)
					m_pTargetFiles = new CFileRegSet(1);

#ifndef REFCOUNT_WORK
				m_pTargetFiles->AddRegHandle(g_FileRegistry.RegisterFile ( &pathTrg ));
#else
				FileRegHandle frhTmp = CFileRegFile::GetFileHandle(pathTrg );
				m_pTargetFiles->AddRegHandle(frhTmp);
				frhTmp->ReleaseFRHRef();
#endif
			}
		}
		if (pathBsc.CreateFromDirAndFilename ( Dir, strBsc ))
		{
#ifndef REFCOUNT_WORK
			m_arryOutputFiles.Add(g_FileRegistry.RegisterFile ( &pathBsc ));
#else
			m_arryOutputFiles.Add(CFileRegFile::GetFileHandle(pathBsc ));
#endif

			// m_pTargetFiles is only allocated for CProject's.
			if ( bOwnerIsProject )
			{
				if (NULL==m_pTargetFiles)
					m_pTargetFiles = new CFileRegSet(1);

#ifndef REFCOUNT_WORK
				m_pTargetFiles->AddRegHandle(g_FileRegistry.RegisterFile ( &pathBsc ));
#else
				FileRegHandle frhTmp = CFileRegFile::GetFileHandle(pathBsc );
				m_pTargetFiles->AddRegHandle(frhTmp);
				frhTmp->ReleaseFRHRef();
#endif
			}
		}
	}
	// Figure out which build tool to use with this file.  Get a pointer to 
	// our project type from the name stored in the base record, and ask for
	// our tool.  Loop through all the configurations if bDoAllConfigs is 
	// true:
	else
	{
		// ignore 'Unknown' ProjTypes
		if (!pProjType->IsSupported())
			goto Exit_Refresh;

		FreeOutputFiles();
		
		CActionSlobList * plstActions = GetActionList();
		POSITION pos = plstActions->GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CActionSlob * pAction = (CActionSlob *)plstActions->GetNext(pos);

			CFileRegSet * pSet = pAction->GetOutput(); 
#ifndef REFCOUNT_WORK
			POSITION posFile = ((CPtrList *)pSet->GetContent())->GetHeadPosition();
			while (posFile != (POSITION)NULL)
			{
				FileRegHandle frh =	(FileRegHandle)((CPtrList *)pSet->GetContent())->GetNext(posFile);
#else
			pSet->InitFrhEnum();
			FileRegHandle frh = pSet->NextFrh();
			while (frh != NULL)
			{
#endif
				int count = m_arryOutputFiles.GetSize();
				int index;
				BOOL bGotOne = FALSE;
				for ( index=0; index < count; index++ )
				{
					if ( frh == m_arryOutputFiles[index] )
					{
						bGotOne = TRUE;
						break;
					}
				}

#ifndef REFCOUNT_WORK
				if ( bGotOne )
					continue;	// got already!

#else
				if ( bGotOne )
				{
					// Not added to m_arryOutputFiles.
					frh->ReleaseFRHRef();
					frh = pSet->NextFrh();
					continue;
				}

#endif
				m_arryOutputFiles.Add(frh);
				
				// is this a target tool?
				if (pAction->m_pTool->m_fTargetTool)
				{
					CSchmoozeTool * pTrgTool = (CSchmoozeTool *)pAction->m_pTool;

					if (pTrgTool->IsProductFile(g_FileRegistry.GetRegEntry(frh)->GetFilePath()))
					{
						if (NULL == m_parryTargetFiles)
						{
							m_parryTargetFiles = new CFRFPtrArray;
							ASSERT(NULL==m_pTargetFiles);

							if ( bOwnerIsProject )
								m_pTargetFiles = new CFileRegSet(1);
						}

						if (pTrgTool->m_fDeferredTool)
						{
							// add to back
							m_parryTargetFiles->Add(frh);
							if ( bOwnerIsProject )
							{
								ASSERT(NULL!=m_pTargetFiles);
								m_pTargetFiles->AddRegHandle(frh);
							}
						}
						else
						{
							// add to front
							m_parryTargetFiles->InsertAt(0,frh);
							if ( bOwnerIsProject )
							{
								ASSERT(NULL!=m_pTargetFiles);
								m_pTargetFiles->AddRegHandle(frh, TRUE, TRUE);
							}
						}
					}
				}
				// custom build tool on target?
				else if (bOwnerIsProject &&
						 pAction->m_pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool)))
				{
					// add to back
					if (NULL == m_parryTargetFiles)
					{
						m_parryTargetFiles = new CFRFPtrArray;

						ASSERT(NULL==m_pTargetFiles);
						m_pTargetFiles = new CFileRegSet(1);
					}

					m_parryTargetFiles->Add(frh);
					m_pTargetFiles->AddRegHandle(frh);
				}
#ifdef REFCOUNT_WORK
				frh = pSet->NextFrh();
#endif
			}
		}
	}

// used to expediate exit from Refresh()
Exit_Refresh:

	// Re-enable file-change notification?
	g_FileRegDisableFC = g_oldFileRegDisableFC;

	if (fForce)
		m_pOwner->ForceConfigActive();
}

BOOL ConfigurationRecord::BagSame(UINT idBag1, UINT idBag2, UINT nPropMin, UINT nPropMax, BOOL fUseDefaults, BOOL fCheckPrivate /* = TRUE */)
{
	CPropBag * pBag1 = GetPropBag(idBag1), * pBag2 = GetPropBag(idBag2);
	CProp * pprop1, * pprop2;
	ASSERT(pBag1 != NULL);
	ASSERT(pBag2 != NULL);

	int retval = 1; // default

	BOOL fFullRange = (nPropMin == -1 && nPropMax == -1);

	// check all available props for full compare
	// o first quick-check to see if # of props is same
	const int cBag1 = pBag1->GetPropCount();
	const int cBag2 = pBag2->GetPropCount();

	//
	// Bags are same if both are empty
	//
	if (0==cBag1 && 0==cBag2)
		return TRUE;

	if (fFullRange && cBag1 != cBag2 && (fCheckPrivate))
		return FALSE;	// different # of props, can't be same!

	// FUTURE (matthewt): partial ranges not supported yet!
	if (nPropMin == -1 && nPropMax != -1)
	{
		ASSERT(FALSE);
		return FALSE;
	}
	else if (nPropMin != -1 && nPropMax == -1)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	// full-range compare?
 	ASSERT(nPropMin <= nPropMax);

	BOOL fEndOfRange = FALSE;

	UINT nProp;		// for !fFullRange
	POSITION pos;	// for fFullRange

	//
	// Must also loop over Bag2 when !fCheckPrivate.
	//
	BOOL bLoopingOverBag2 = FALSE;
	int nFoundInBag1 = 0;
	int nFoundInBag2 = 0;

	if (fFullRange)
	{
		if (0==cBag1)
		{
			//
			// Switch over to bag 2 if !fCheckPrivate.
			//
			ASSERT(!fCheckPrivate);
			pos = pBag2->GetStartPosition();
			bLoopingOverBag2 = TRUE;
		}
		else
		{
			pos = pBag1->GetStartPosition();
		}

		ASSERT(NULL!=pos);
	}
	else
		nProp = nPropMin;

	fEndOfRange = fFullRange ? (pos == (POSITION)NULL) : (nProp > nPropMax);
	for ( ;!fEndOfRange; nProp++)
	{
		// get source prop.to compare (method depends on range type)
		if (!bLoopingOverBag2)
		{
			if (fFullRange)
			{
				pBag1->GetNextAssoc(pos, nProp, pprop1);

				// get corresponding destination prop. to compare
				if (cBag2 > nFoundInBag2)
				{
 					pprop2 = pBag2->FindProp(nProp);
					if (NULL!=pprop2)
						nFoundInBag2++;
				}
				else
				{
					pprop2 = NULL;
				}

				//
				// Setup looping through bag 2 if needed.
				//
				if (NULL==pos && !fCheckPrivate  && cBag2 > nFoundInBag2 )
				{
					//
					// Switch over to bag 2 if !fCheckPrivate.
					//
					pos = pBag2->GetStartPosition();
					bLoopingOverBag2 = TRUE;
				}
			}
			else
			{
				//
				// Looping from nPropMin to nPropMax
				//
				if (cBag1 == nFoundInBag1 && cBag2 == nFoundInBag2)
					break;

				if (cBag1 > nFoundInBag1)
				{
 					pprop1 = pBag1->FindProp(nProp);
					if (NULL!=pprop1)
						nFoundInBag1++;
				}
				else
					pprop1 = NULL;

				if (cBag2 > nFoundInBag2)
				{
 					pprop2 = pBag2->FindProp(nProp);
					if (NULL!=pprop2)
						nFoundInBag2++;
				}
				else
					pprop2 = NULL;
			}
		}
		else
		{
			if (cBag2 == nFoundInBag2)
			{
				// We are done!
				break;
			}

			ASSERT(cBag2 > nFoundInBag2);
			ASSERT(fFullRange);
			ASSERT(!fCheckPrivate);
			pBag2->GetNextAssoc(pos, nProp, pprop2);

			//
			// See if we have already processed everything in Bag1 while
			// looping through Bag2.
			//
			if (cBag1 > nFoundInBag1)
			{
 				pprop1 = pBag1->FindProp(nProp);

				//
				// If found in Bag1, proprety was already processed in Bag1 loop.
				// else this is a new proprety found in Bag2.
				//
				if (NULL!=pprop1)
				{
					nFoundInBag1++;
					continue;
				}
			}
			else
				pprop1 = NULL;

			ASSERT(NULL==pprop1);
			nFoundInBag2++;
		}

		// check for end_of_while condition (depends on range type)
		fEndOfRange = fFullRange ? (pos == (POSITION)NULL) : (nProp == nPropMax);

  		// in neither? (logical XOR)
		if (pprop1 == (CProp *)NULL || pprop2 == (CProp *)NULL)
		{
			if (pprop1 != (CProp *)NULL || pprop2 != (CProp *)NULL)
			{
				// normalize (pprop2 to be NULL)
				if (pprop2 != (CProp *)NULL)
				{
					pprop1 = pprop2; pprop2 = (CProp *)NULL;
				}

				// one of the property bags doesn't have a prop. use
				// the defaults for that bag in the comparision?
				// (*only* if we are the base-prop. bag, ie. the owner is *not* a child)
				if (!fUseDefaults || m_pOwner->GetContainer() != (CSlob *)NULL)
				{
					if (fCheckPrivate || IsPropPublic(nProp))
						return FALSE;	// different!
					retval = -1;
					continue;
				}

				// do we have an option handler?
				COptionHandler * popthdlr = g_prjcompmgr.FindOptionHandler(nProp);
				if (popthdlr == (COptionHandler *)NULL)
				{
					// no handler == no default, so different!
					if (fCheckPrivate || IsPropPublic(nProp))
						return FALSE;	// different!
					retval = -1;
					continue;
				}

				// make sure the option handling refers to our owner!
		 		m_pOwner->ForceConfigActive(this);
				popthdlr->SetSlob(m_pOwner);

				// is this a value in our default property map?
				BOOL fIsDefault = FALSE;

				// always show default props
				BOOL fOldUseDefault = m_pOwner->UseOptionDefaults();
		  		switch (pprop1->m_nType)
				{
					case integer:
					{
						int nVal;
						fIsDefault = popthdlr->GetDefIntProp(nProp, nVal) == valid &&
									 ((CIntProp*)pprop1)->m_nVal == nVal;
						break;
					}
   
					case string:
					{
						CString strVal;
						fIsDefault = popthdlr->GetDefStrProp(nProp, strVal) == valid &&
									 ((CStringProp*)pprop1)->m_strVal == strVal;
						break;
					}
		
					default:
						ASSERT(FALSE);
						break;
				}
	
				// reset the option handler CSlob
				(void) m_pOwner->UseOptionDefaults(fOldUseDefault);
		 		m_pOwner->ForceConfigActive();
				popthdlr->ResetSlob();

				if (!fIsDefault)
				{
					// not a default, so different!
					if (fCheckPrivate || IsPropPublic(nProp))
						return FALSE;	// different!
					retval = -1;
					continue;
				}
			}

			continue;	// the same...
			// next one
		}

		// same type?
		if (pprop1->m_nType != pprop2->m_nType)
		{
			if (fCheckPrivate || IsPropPublic(nProp))
				return FALSE;	// different!
			retval = -1;
			continue;
		}

  		switch (pprop1->m_nType)
		{
			case integer:
				if (((CIntProp*)pprop1)->m_nVal != ((CIntProp*)pprop2)->m_nVal)
				{
					if (fCheckPrivate || IsPropPublic(nProp))
						return FALSE;	// different!
					retval = -1;
					continue;
				}
				break;

			case string:
				if (((CStringProp*)pprop1)->m_strVal != ((CStringProp*)pprop2)->m_strVal)
				{
					if (fCheckPrivate || IsPropPublic(nProp))
						return FALSE;	// different!
					retval = -1;
					continue;
				}
				break;
		
			default:
				ASSERT(FALSE);
				return FALSE;
		}

		// next one
	}

	return retval;	// the same
}

void ConfigurationRecord::BagCopy(UINT idBagSrc, UINT idBagDest, UINT nPropMin, UINT nPropMax, BOOL fInform)
{
	CPropBag * pBagSrc = GetPropBag(idBagSrc), * pBagDest = GetPropBag(idBagDest);
	ASSERT(pBagSrc != NULL);
	ASSERT(pBagDest != NULL);

	CProp * ppropSrc, * ppropDest;

	// need to make sure we are in the right config in case of inform
	if (fInform)
		m_pOwner->SetManualBagSearchConfig(this);

	// copy from our source to the destination the props in
	// our range by either adding or replacing prop. values
	POSITION pos = pBagSrc->GetStartPosition();
	while (pos != NULL)
	{
		BOOL fDifferent = TRUE;

		UINT id;
		// get the next prop from our source
		pBagSrc->GetNextAssoc(pos, id, ppropSrc);

		// make range checkes against minimum and maximum props
		if ((nPropMin != -1 && id < nPropMin) ||
			(nPropMax != -1 && id > nPropMax))
			continue;

		// get the equiv. in our destination
		ppropDest = pBagDest->FindProp(id);

		// make sure the types are the same if found in both bags
		ASSERT(ppropDest == (CProp *)NULL || ppropSrc->m_nType == ppropDest->m_nType);

		// do the copy (FUTURE: no Undo)
		switch (ppropSrc->m_nType)
		{
			case integer:
				if (ppropDest != (CProp *)NULL)
				{
					// exists in destination, just do in-place set and check for equivalence
					fDifferent = ((CIntProp*)ppropDest)->m_nVal != ((CIntProp*)ppropSrc)->m_nVal;
					((CIntProp*)ppropDest)->m_nVal = ((CIntProp*)ppropSrc)->m_nVal;
				}
				else
				{
					// doesn't exist, add the prop.
					pBagDest->AddProp(id, new CIntProp(((CIntProp*)ppropSrc)->m_nVal));
				}
				break;

			case string:
				if (ppropDest != (CProp *)NULL)
				{
					// exists in destination, just do in-place set and check for equivalence
					fDifferent = ((CStringProp*)ppropDest)->m_strVal != ((CStringProp*)ppropSrc)->m_strVal;
					((CStringProp*)ppropDest)->m_strVal = ((CStringProp*)ppropSrc)->m_strVal;
				}
				else
				{
					// doesn't exist, add the prop.
					pBagDest->AddProp(id, new CStringProp(((CStringProp*)ppropSrc)->m_strVal));
				}
				break;
		
			default:
				ASSERT(FALSE);
				break;
		}

		if (fInform && fDifferent)
		{
			m_pOwner->InformDependants(id);

			// got an option handle for this?
			COptionHandler * popthdlr = g_prjcompmgr.FindOptionHandler(id);
			if (popthdlr != (COptionHandler *)NULL)
			{
				popthdlr->SetSlob(m_pOwner);

				// integer changes
				if (ppropSrc->m_nType == integer)
					popthdlr->OnOptionIntPropChange(id, ((CIntProp*)ppropSrc)->m_nVal);

				// string changes
				else if (ppropSrc->m_nType == string)
					popthdlr->OnOptionStrPropChange(id, ((CStringProp*)ppropSrc)->m_strVal);

				// reset the option handler CSlob
				popthdlr->ResetSlob();
			}
		}
	}

	// remove from our destination all those props. not copied
	pos = pBagDest->GetStartPosition();
	while (pos != NULL)
	{
		UINT id;
		// get the next prop in our destination
		pBagDest->GetNextAssoc(pos, id, ppropSrc);

		// make range checkes against minimum and maximum props
		if ((nPropMin != -1 && id < nPropMin) ||
			(nPropMax != -1 && id > nPropMax))
			continue;

		// not in our source bag, so delete (it exists!)
		if (!pBagSrc->FindProp(id) && pBagDest->FindProp(id))
		{
			if (fInform)
			{
				// m_pOwner->InformDependants(id);

				// review: should this also be deferred until after prop is changed

				// got an option handle for this?
				COptionHandler * popthdlr = g_prjcompmgr.FindOptionHandler(id);
				if (popthdlr != (COptionHandler *)NULL)
				{
					popthdlr->SetSlob(m_pOwner);

					// integer changes
					if (ppropSrc->m_nType == integer)
						popthdlr->OnOptionIntPropChange(id, 0);

					// string changes
					else if (ppropSrc->m_nType == string)
						popthdlr->OnOptionStrPropChange(id, "");

					// reset the option handler CSlob
					popthdlr->ResetSlob();
				}
			}

			pBagDest->RemoveProp(id);

			// defer inform until after prop is actually changed
			if (fInform)
			{
				m_pOwner->InformDependants(id);
			}
		}
	}

	// restore to previous state
	if (fInform)
		m_pOwner->ResetManualBagSearchConfig();
}

#ifdef _DEBUG
void ConfigurationRecord::AssertValid() const
{
	// should point to top level record
	ASSERT(m_pBaseRecord == m_pBaseRecord->m_pBaseRecord);
}

void ConfigurationRecord::Dump (CDumpContext &DC) const
{
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
// --------------------------------	CProjItem ---------------------------------
//
///////////////////////////////////////////////////////////////////////////////
#define theClass CProjItem
BEGIN_SLOBPROP_MAP(CProjItem, CProjSlob)
END_SLOBPROP_MAP()
#undef theClass

const CString & CProjItem::GetTargetName() { return GetTarget()->GetTargetName(); }

void CProjItem::DeleteContents()
{

	// free-up our output files references
	int nIndex, nSize = GetPropBagCount();
	for (nIndex = 0; nIndex < nSize; nIndex++)
		((ConfigurationRecord *)m_ConfigArray[nIndex])->FreeOutputFiles();

	// delete all its children
	// NOTE: We cannot use a normal GetHeadPosition, GetNext loop here,
	// as removing one item may result in removing another item in the list
	// This situation occurs when we remove a file item and this results in
	// the removal of the corresponding dependency container
	CObList * pContentList = GetContentList();
	if (pContentList)
	{
		while (!pContentList->IsEmpty())
		{
 	    	// any contained slobs should also be items..
			CProjItem * pItem = (CProjItem *) pContentList->GetHead();
			ASSERT(pItem != NULL);
			ASSERT_VALID(pItem);
			RemoveDependant(pItem);

			// delete
			pItem->MoveInto(NULL);
		}
	}

	// remove our reference?
#ifndef REFCOUNT_WORK
	FileRegHandle frh;
	if ((frh = GetFileRegHandle()) != (FileRegHandle)NULL)
		g_FileRegistry.ReleaseRegRef(frh);
#endif

	// delete our configuration records
	for (nIndex = 0; nIndex < nSize; nIndex++)
		delete (ConfigurationRecord *)m_ConfigArray[nIndex];

	m_ConfigArray.RemoveAll();
	m_ConfigMap.RemoveAll();

	m_pActiveConfig = NULL;
}
///////////////////////////////////////////////////////////////////////////////
CProjItem::CProjItem ()	: m_ConfigMap (3)
{
	m_ConfigMap.InitHashTable(3);

	// our current 'owner' project is undefined currently!
	m_pOwnerProject = (CProject *)NULL;

	// our current 'owner' target is undefined currently!
	m_pOwnerTarget = (CTargetItem *)NULL;

	// by default prop search uses inheritance
	// by default prop search uses default
	// by default we are not anti-options
	// no manual bag find
	m_optbeh = OBInherit | OBShowDefault;

	// by default we are not figuring a dep!
	m_fPropsDepFigure = FALSE;

	// default is outside config switches
	m_cp = EndOfConfigs;

	// no active or forced configuration
	m_pActiveConfig = m_pForcedConfig = NULL;

	// no manual config rec. by deault
	m_pManualConfigRec = NULL;

	// not getting a list string using ::GetStrProp
 	m_fGettingListStr = FALSE;

	// Default is to use the current property bag for each config. rec.
	m_idBagInUse = CurrBag;
}
///////////////////////////////////////////////////////////////////////////////
CProjItem::~CProjItem()
{
}
///////////////////////////////////////////////////////////////////////////////
CTargetItem * CProjItem::GetTarget()
{
	// do we have one cached?
	if (m_pOwnerTarget != (CTargetItem *)NULL)
		return m_pOwnerTarget;

	// no, so try to find it given the active target
	ConfigurationRecord * pcr = GetActiveConfig();
	CProject * pProject = GetProject();
	if (pcr == (ConfigurationRecord *)NULL || pProject == (CProject *)NULL)
		return (CTargetItem *)NULL;

	// the active one
	return pProject->GetTarget(pcr->GetConfigurationName());
}
///////////////////////////////////////////////////////////////////////////////
// Removes project item from view 
BOOL CProjItem::PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	// removing item altogether from project?
	if (fFromPrj || fPrjChanged)
	{
		// remove ourselves, and remove children..
		CFileRegistry * preg = GetRegistry();
		ASSERT(preg != (CFileRegistry *)NULL);
		preg->RemoveFromProject(this, CouldBeContainer(pContainer));

		// free-up our output files references
		int nIndex, nSize = GetPropBagCount();
		for (nIndex = 0; nIndex < nSize; nIndex++)
			((ConfigurationRecord *)m_ConfigArray[nIndex])->FreeOutputFiles();
	}

	return TRUE;	// success
}

// Alters the containment and fixes configs. of item,
// but not 'officially' in or out-of project
BOOL CProjItem::MoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	BOOL fRefreshDeps = FALSE;

	CTargetItem * pTarget = GetTarget();
	CProject * pProject = GetProject();

	// moving from a project?
	if ((fFromPrj) && (pProject != NULL))
	{
 
		ASSERT(pTarget != NULL);

		// set the project to into each valid config. for this target
		const CPtrArray * pCfgArray = pTarget->GetConfigArray();
		int iSize = pCfgArray->GetSize();
		for (int i = 0; i < iSize; i++)
		{
			ConfigurationRecord * pcr = (ConfigurationRecord *)pCfgArray->GetAt(i);

			if ( pcr->IsValid())
			{
				// refresh our container's dependencies?
				// project not being created/destroyed?
				if (!fRefreshDeps && pProject->m_bProjectComplete)
				{
					CProjTempConfigChange projTempConfigChange(pProject);
					projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);

					ASSERT(GetActiveConfig());
					CActionSlobList * pActions = GetActiveConfig()->GetActionList();

					projTempConfigChange.Release();
					
					// only check the first action for a file (we only have one currently)
					if (!pActions->IsEmpty() && pActions->GetCount() == 1)
					{
						CActionSlob * pAction = (CActionSlob *)pActions->GetHead();
#ifndef REFCOUNT_WORK
						fRefreshDeps = !pAction->GetScannedDep()->GetContent()->IsEmpty() ||
									   !pAction->GetSourceDep()->GetContent()->IsEmpty();
#else
						fRefreshDeps = !pAction->GetScannedDep()->IsEmpty() ||
									   !pAction->GetSourceDep()->IsEmpty();
#endif
					}
				}

				// FUTURE: optimize by only doing this if its moving to a new target...
				
				// un-assign actions to each of this items configurations
				// *OR* if we going into an undo then un-bind actions from the build instance
				ASSERT(fFromPrj);
				if (fPrjChanged || (!CouldBeContainer(pContainer) && !fToPrj))
					CActionSlob::UnAssignActions(this, (CPtrList *)NULL, pcr, CouldBeContainer(pContainer));
				else if (!fToPrj)
					CActionSlob::UnBindActions(this, (CPtrList *)NULL, pcr, TRUE /* CouldBeContainer(pContainer) */ );

			}
		}

		// removing item altogether from project?
		// and project not being created/destroyed?
		if ((!fToPrj || fPrjChanged) && GetProject()->m_bProjectComplete)
		{
			// if we're removing a project item from the project we need to 
			// isolate it from the container
			IsolateFromProject(); // Rvw: overkill for drag & drop
		}
	}	

	// fix our 'cached' containment
	// so that any notifies from below work
	// moved to a project?
	if (fToPrj)
	{
		// Search for our new 'owner' project correctly
		CSlob * pSlob = pContainer;
		CTargetItem * pNewTarg = NULL;
		while (!pSlob->IsKindOf(RUNTIME_CLASS(CProject)))
		{
			// can we find the target 'owner'?
			if (pSlob->IsKindOf(RUNTIME_CLASS(CTargetItem)))
				pNewTarg = (CTargetItem *)pSlob;

			// must be able to find a project 'owner'
			pSlob = pSlob->GetContainer();
			ASSERT(pSlob != (CSlob *)NULL);
		}

#if 0	// should already be done above!
		if (fFromPrj && ((GetProject() != (CProject *)pSlob) || fPrjChanged))
		{
			// if we're moving from one project to another we also need to 
			// isolate it from the old container
			IsolateFromProject(); // Rvw: overkill for drag & drop
		}
#endif

		if ((pNewTarg==NULL) && (IsKindOf(RUNTIME_CLASS(CTargetItem))))
			pNewTarg = (CTargetItem *)this; // hey, we are the target

		// Cache our notion of the 'owner' project
		SetOwnerProject((CProject *)pSlob, pNewTarg);
	}
	// else from a project

	// alter our containment
	// if pContainer == NULL then this will *delete*
	// ourselves, do *not* access object data from here
	// on out in this situation
	if (!CSlob::SetSlobProp(P_Container, pContainer))
		return FALSE;

	// fix our 'cached' containment
	// so that any notifies from below work
	// moved to a project?
	if (fToPrj)
	{
		// moving to a project by removing from a project or undo'ing?
		// yes, then match the new container's configurations.
		// (This may require the deletion of some)
		if (fFromPrj || fPrjChanged || (pOldContainer == NULL) || pOldContainer == &theUndoSlob)
			CloneConfigs((const CProjItem *)pContainer);
	}
	// else from a project into something else
	else if (theUndoSlob.IsRecording())
	{
		// *Not* a project item -> *No* owner project -> *No* owner target,
		// most likely contained in the undo CSlob, or some such thing
		ASSERT((pContainer == (CSlob *)NULL) || pContainer == &theUndoSlob || pContainer->IsKindOf(RUNTIME_CLASS(CClipboardSlob)));
		SetOwnerProject(NULL, NULL);
	}

	return TRUE;	// success
}

// Make item item 'officially' in or out-of project    
BOOL CProjItem::PostMoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	// moving into the project?
	if (fToPrj)
	{
 		// restore ourselves, include children only if not being created?
		CFileRegistry * preg = GetRegistry();
		ASSERT(preg != (CFileRegistry *)NULL);
		BOOL bContents = ((GetProject()== NULL) || (GetProject()->m_bProjectComplete)); // true except when reading project file
		preg->RestoreToProject(this, bContents); // REVIEW

		CTargetItem * pTarget = GetTarget();
		CProject * pProject = GetProject();

		ASSERT(pTarget!=NULL);
		ASSERT(pProject!=NULL);
		if ((pTarget==NULL) || (pProject==NULL))
			return FALSE; // can happen with bogus makefile

		// Set the project to into each valid config. for this target
		const CPtrArray * pCfgArray = pTarget->GetConfigArray();
		int iSize = pCfgArray->GetSize();
		for (int i = 0; i < iSize; i++)
		{
			ConfigurationRecord * pcr = (ConfigurationRecord *)pCfgArray->GetAt(i);

			if ( pcr->IsValid())
			{
				// assign actions to each of the items configurations or
				// *OR* if we coming in from an undo then re-bind to the build instance
				ASSERT(fToPrj);
				if (((!CouldBeContainer(pOldContainer)) && (!fFromPrj)) || fPrjChanged)
				{
					// ASSERT(pOldContainer);
					CActionSlob::AssignActions(this, (CPtrList *)NULL, pcr, CouldBeContainer(pOldContainer));
				}
				else if (!fFromPrj)
					CActionSlob::BindActions(this, (CPtrList *)NULL, pcr, CouldBeContainer(pOldContainer));

			}
		}

        // Dirty the project since we will have to save this change
		if (!IsKindOf(RUNTIME_CLASS(CDependencyFile)) && 
			!IsKindOf(RUNTIME_CLASS(CProjectDependency)) && 
			!IsKindOf(RUNTIME_CLASS(CDependencyContainer)))
		{
			pProject->DirtyProject();
		}
	}
	return TRUE;	// success
}

///////////////////////////////////////////////////////////////////////////////
void CProjItem::GetMoniker(CString& cs) 
{
	GetStrProp (P_ProjItemName, cs);
}
///////////////////////////////////////////////////////////////////////////////
int CProjItem::CompareMonikers (CString& str1, CString& str2)
{
	return _tcsicmp ((const char *)str1, (const char *)str2);
}	
///////////////////////////////////////////////////////////////////////////////
const CPath *CProjItem::GetFilePath () const
{
	FileRegHandle frh = GetFileRegHandle();
	if (frh) return g_FileRegistry.GetRegEntry(frh)->GetFilePath();
	else 	 return NULL;
} 
///////////////////////////////////////////////////////////////////////////////
FileRegHandle CProjItem::GetFileRegHandle () const 
{
	return NULL; 
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::SetFile (const CPath *pPath, BOOL bCaseApproved /* = FALSE */)
{
	TRACE ( "CProjItem::SetFile() called for non-File class %s\n", 
												GetRuntimeClass()->m_lpszClassName );
 	// ASSERT (FALSE);
	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////
// Verb Commands
BOOL CProjItem::CanDoScan()
{
	BOOL fOkToScan = !g_Spawner.SpawnActive();

	return fOkToScan;
}



void CProjItem::CmdCompile(BOOL bMenu)
{
	// StartCAP();
	UINT nPos = 0;
	if( !bMenu ){
		LPPROJECTWORKSPACEWINDOW pProjSysIFace = FindProjWksWinIFace();
		VERIFY(SUCCEEDED(pProjSysIFace->GetVerbPosition(&nPos)));
	}

 	BOOL bClearOutputWindow = (nPos == 0);

	CBuildIt wholeBuild;
	(VOID) DoTopLevelBuild(TOB_Build, NULL, fcaNeverQueried, TRUE, bClearOutputWindow, TRUE);

	// StopCAP();
}

void CProjItem::CmdClean()
{
	// StartCAP();
	CBuildIt wholeBuild(FALSE);
	(VOID) DoTopLevelBuild(TOB_Clean);
	// StopCAP();
}

void CProjItem::CmdReBuild()
{
	// StartCAP();
	CBuildIt wholeBuild;

	(VOID) DoTopLevelBuild(TOB_Clean);
	CBuildIt::m_mapConfigurationsBuilt.RemoveAll();
	(VOID) DoTopLevelBuild(TOB_ReBuild,NULL,fcaNeverQueried,TRUE,FALSE); // don't clear output window for second build
	// StopCAP();
}

void CProjItem::CmdBuild()
{
	// StartCAP();
	CBuildIt wholeBuild;
	(VOID) DoTopLevelBuild(TOB_Build);
	// StopCAP();
}

BOOL g_bBatchBuildInProgress = FALSE;
void CProjItem::CmdBatchBuild(BOOL bBuildAll, CStringList * pConfigs, BOOL bRecurse /* = TRUE */, BOOL bClean /* = FALSE */)
{
	// StartCAP();
	CBuildIt wholeBuild(bBuildAll || !bClean);

	g_bBatchBuildInProgress = TRUE;
	if (pConfigs->GetCount() > 0) {
		// save the original config
		CString strActiveConfig;
		// CProject * pProject = GetProject();
		// pProject->GetStrProp(P_ProjActiveConfiguration, strActiveConfig);

		// Do the build
		if( bBuildAll ) {
			ASSERT(!bClean);
			CStringList dupConfigs;
			for (POSITION pos = pConfigs->GetHeadPosition (); pos != NULL; )
			{
				dupConfigs.AddTail( pConfigs->GetNext (pos) );
			}

			(VOID) DoTopLevelBuild( TOB_Clean,pConfigs,fcaNeverQueried,TRUE,TRUE,bRecurse);
			CBuildIt::m_mapConfigurationsBuilt.RemoveAll();
			(VOID) DoTopLevelBuild( TOB_ReBuild, &dupConfigs,fcaNeverQueried,TRUE,FALSE,bRecurse); // don't clear output window for second build
		} else {
			(VOID) DoTopLevelBuild( bClean ? TOB_Clean : TOB_Build, pConfigs,fcaNeverQueried,TRUE,TRUE,bRecurse);
		}

		// Restore the original config.
		// pProject->SetStrProp(P_ProjActiveConfiguration, strActiveConfig);
	}
	g_bBatchBuildInProgress = FALSE;
	// StopCAP();
}

// FUTURE: move these into bldrfile.cpp
const TCHAR *CProjItem::EndToken = _TEXT ("End");
const TCHAR *CProjItem::pcFileMacroName = _TEXT ("SOURCE");
const TCHAR *CProjItem::pcFileMacroNameEx = _TEXT ("$(SOURCE)"); 	

BOOL CProjItem::WriteToMakeFile
(
	CMakFileWriter& pw,
	CMakFileWriter& mw,
	const CDir &BaseDir,
	CNameMunger& nm
)
{
	PFILE_TRACE ("%s at %p called CProjItem::WriteToMakeFile (do nothing)\n",
				 GetRuntimeClass()->m_lpszClassName, this);
	return TRUE;	// success by default
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir)
{
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////
/** If you change this table, you MUST change HasMakefileProperties to match */
const CProjItem::PropWritingRecord CProjItem::nMakeFileProps[] = {
	{P_ProjUseMFC,	 			_TEXT("Use_MFC"),				integer, PropRead | PropWrite | All},
	{P_UseDebugLibs,	 		_TEXT("Use_Debug_Libraries"),	integer, PropRead | PropWrite | All},
	{P_OutDirs_Target, 			_TEXT("Output_Dir"),			string, PropRead | PropWrite | All},
	{P_OutDirs_Intermediate,	_TEXT("Intermediate_Dir"),		string, PropRead | PropWrite | All},
	{P_Proj_TargDefExt,	 		_TEXT("Target_Ext"),			string, PropRead | PropWrite | Project},
	{P_Proj_CmdLine,			_TEXT("Cmd_Line"),				string, PropRead | PropWrite | Project},
	{P_Proj_RebuildOpt,			_TEXT("Rebuild_Opt"),			string, PropRead | PropWrite | Project},
	{P_Proj_Targ,				_TEXT("Target_File"),			string, PropRead | PropWrite | Project},
	{P_Proj_BscName,			_TEXT("Bsc_Name"),				string, PropRead | PropWrite | Project},
//    {P_Proj_WorkingDir,         _TEXT("Bld_Working_Dir"),       string, PropRead | PropWrite | Project},
	{P_ItemExcludedFromBuild,	_TEXT("Exclude_From_Build"),	integer, PropRead | PropWrite | File | TargetRef},
	{P_ItemExcludedFromScan,	_TEXT("Exclude_From_Scan"),	integer, PropRead | PropWrite | File | Group | NotPerConfig},
	{P_ProjClsWzdName,			_TEXT("Classwizard_Name"),		string, PropRead | PropWrite | Project},
	{P_GroupDefaultFilter,		_TEXT("Default_Filter"),	string, PropRead | PropWrite | Group | NotPerConfig },
	{P_Proj_PerConfigDeps,		_TEXT("AllowPerConfigDependencies"),	integer, PropRead | PropWrite | Project | NotPerConfig },
#ifdef VB_MAKEFILES
	{P_Proj_IsVB,				_TEXT("VB_Project"),		integer, PropRead | PropWrite | Project | NotPerConfig },
	{P_VBProjName,				_TEXT("VB_ProjectName"),		string, PropRead | PropWrite | Project | NotPerConfig },
#endif
	{P_Proj_IgnoreExportLib,	_TEXT("Ignore_Export_Lib"),		integer, PropRead | PropWrite | Project },
	{P_ItemIgnoreDefaultTool,	_TEXT("Ignore_Default_Tool"),		integer, PropRead | PropWrite | File },

// Scc
	{P_ProjSccProjName,			_TEXT("Scc_ProjName"),	string, PropRead | PropWrite | NotPerConfig | Project},
	{P_ProjSccRelLocalPath,		_TEXT("Scc_LocalPath"),	string, PropRead | PropWrite | NotPerConfig | Project},

// NB: if you add or remove any Writeable props here, you MUST also
// update the constant nWriteableProps constant below:
#ifdef VB_MAKEFILES
#define nWriteableProps (20)
#else
#define nWriteableProps (18)
#endif
// we should eliminate this redundancy, put in a marker item or put the two types of props in seperate arrays

	// Really, these are project only.  Also, they are not per conifg, so 
	// they would not be written out properly.  However, they are only read in:
	{P_ProjMarkedForScan,		_TEXT("Marked_For_Scan"),		integer, PropRead | Project | NotPerConfig },
	{P_ProjMarkedForSave,		_TEXT("Marked_For_Save"),		integer, PropRead | Project | NotPerConfig },
	{P_ProjHasNoOPTFile,		_TEXT("Has_No_OPT_File"),		integer, PropRead | Project | NotPerConfig },
	{P_TargetRefName,		_TEXT("Target_Ref_Name"),		string, PropRead | TargetRef },
	{P_Proj_TargDir,	 		_TEXT("Target_Dir"),			string, PropRead | Project},	// only needed for conversions (ignored)
// NB: if you add or remove any Read-only props here, you MUST also
// update the constant nReadOnlyProps constant below:
#define nReadOnlyProps (5) // should be eliminated, as above
	{P_END_MAP, NULL, null},
};

// FUTURE: move these into bldfile.cpp
const TCHAR *CProjItem::pcPropertyPrefix 	= _TEXT("PROP ");
const TCHAR *CProjItem::pcADD 				= _TEXT("ADD ");
const TCHAR *CProjItem::pcSUBTRACT			= _TEXT("SUBTRACT ");
const TCHAR *CProjItem::pcBASE				= _TEXT("BASE ");

// Consider: use this elsewhere
static CString & StripTrailingSlash(CString & str)
{
	int len = str.GetLength();
	_TCHAR * pchStart = str.GetBuffer(len);
	_TCHAR * pchEnd = pchStart + len;
	if (len > 1)
	{
		pchEnd = _tcsdec(pchStart, pchEnd);
		if ((pchEnd > pchStart) && ((*pchEnd =='/') || (*pchEnd == '\\')))
		{
			*pchEnd = 0;
			len = pchEnd - pchStart;
		}
	}

	str.ReleaseBuffer(len);
	return str;
}

BOOL CProjItem::GetOutDirString
(
	CString & str,
	CString * pstrValue, /* = NULL */
	BOOL bAllowItemMacro, /* = FALSE */
	BOOL fRecurse /* = FALSE */
)
{
	ASSERT(!bAllowItemMacro); // no longer supported

	// default is target prop for project
	UINT PropId = P_OutDirs_Target;

	CProjItem * pItem = GetProject();
	ASSERT(pItem != (CProjItem *)NULL);

	BOOL fForced = FALSE;

	// are we not a project or target item?
	if (this != pItem && this != GetTarget())
	{
		// no
		PropId = P_OutDirs_Intermediate;

		// can we get the property?
		if (GetPropBag()->FindProp(PropId))
		{
			// if (!bAllowItemMacro)
			return FALSE;

			// yes so use us!
			// pItem = (CProjItem *)this;
		}
		else
		{
			// try to get it from our parent project instead
			// (base-record *is* project config. record)
			ASSERT(GetActiveConfig());
			if (pItem->GetActiveConfig() != GetActiveConfig()->m_pBaseRecord)
			{
				pItem->ForceConfigActive((ConfigurationRecord *)GetActiveConfig()->m_pBaseRecord);
				fForced = TRUE;
			}
		}
	}

	BOOL fRet = pItem->GetOutDirString(str, PropId, pstrValue, TRUE, fRecurse);

	if (fForced) // restore original config.?
		pItem->ForceConfigActive();

	return fRet;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::GetOutDirString
(
	CString & str,
	UINT PropId,
	CString * pstrValue, /* = NULL */
	BOOL bMacro, /* = TRUE */
	BOOL fRecurse /* = FALSE */
)
{	
	// the rest are output directory related
	// find one somewhere in the hierarchy
	CProjItem * pItem = this;

	CProp * pprop = pItem->GetPropBag()->FindProp(PropId);
	while (fRecurse && pprop == (CProp *)NULL)
	{
		// *chain* the proper. config.
		CProjItem * pItemOld = pItem;
		pItem = (CProjItem *)pItem->GetContainerInSameConfig();
		if (pItemOld != (CProjItem *)this)
			pItemOld->ResetContainerConfig();

		if (pItem == (CProjItem *)NULL)
			break;	// not found in hierarchy

		pprop = pItem->GetPropBag()->FindProp(PropId);
	}

	// reset the last container we found
	if (pItem != (CProjItem *)NULL && pItem != (CProjItem *)this)
		pItem->ResetContainerConfig();

	// did we get one? fail if this isn't the target level
	if (pprop == (CProp *)NULL && !IsKindOf(RUNTIME_CLASS(CProject)))
		return FALSE;	// failure

	if (pstrValue)
	{
		// get the value
		CString strOutDir = pprop == (CProp *)NULL ? _TEXT("") : ((CStringProp *)pprop)->m_strVal;

		if (strOutDir.IsEmpty())
		{
			*pstrValue = _T('.');
		}
		else
		{
			strOutDir.TrimLeft();
			pstrValue->GetBuffer(strOutDir.GetLength() + 3);
			pstrValue->ReleaseBuffer(0);

			const TCHAR * pch = (const TCHAR *)strOutDir;

			// prepend ".\\" if not absolute or UNC path or already relative
			if ((pch[0] != _T('\\')) && 
				(pch[0] != _T('/')) &&
				(!(pch[0] == _T('.') && pch[1] == _T('\\'))) &&
				(!IsDBCSLeadByte (pch[0]) && (pch[1] != _T(':'))) || (IsDBCSLeadByte (pch[0])) 
				)
			{
				*pstrValue += _T(".\\");
			}

			*pstrValue += strOutDir;
			StripTrailingSlash(*pstrValue);
		}
	}

	str.GetBuffer(13); str.ReleaseBuffer(0);

	if (bMacro) str += _T("$(");

	// get the output directory macro name
	str += PropId == P_OutDirs_Intermediate ? _T("INTDIR") : _T("OUTDIR");

	// was this found in a file?
	if (IsKindOf(RUNTIME_CLASS(CFileItem)) && GetPropBag()->FindProp(PropId))
	{
		str += _T("_SRC");	// decorate the file-level output directory macro name
		return FALSE; // no longer supported
	}

	if (bMacro) str += _T(')');

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::SubstOutDirMacro(CString & str, const ConfigurationRecord * pcr)
{
	// substitute for $(OUTDIR) macro if nessessary

	if (pcr==NULL)	// this is not optional
		return FALSE;

	int nFndLoc;
	CString strOutDirMacro, strOutDirValue;
	BOOL retval = FALSE;
	if (pcr != GetActiveConfig())
		SetManualBagSearchConfig((ConfigurationRecord *)pcr);

	if (GetOutDirString(strOutDirMacro, &strOutDirValue))
	{
		nFndLoc = str.Find(strOutDirMacro);
		if (nFndLoc != -1)
		{
			// ASSERT(nFndLoc==0);
			str = str.Left(nFndLoc) + strOutDirValue + str.Mid(nFndLoc + strOutDirMacro.GetLength());
			retval = TRUE;
		}
	}

	if (pcr != GetActiveConfig())
		ResetManualBagSearchConfig();

	return retval;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::WriteOutDirMacro(CMakFileWriter & mw)
{
	ASSERT(g_bExportMakefile);
	ASSERT(mw.IsMakefile());

	// don't do this for CTargetItems
	if (IsKindOf(RUNTIME_CLASS(CTargetItem)))
		return TRUE;

	BOOL retVal = TRUE;

	TRY
	{
		CString strMacro, strValue;

		// write macros, if necessary
		if (GetOutDirString(strMacro, P_OutDirs_Target, &strValue, FALSE))
		{
			ASSERT(this == GetProject());
			mw.WriteMacro(strMacro, strValue);
		}

		if (GetOutDirString(strMacro, P_OutDirs_Intermediate, &strValue, FALSE))
			mw.WriteMacro(strMacro, strValue);
	}
	CATCH (CException, e)
	{
		retVal = FALSE;	// failure
	}
	END_CATCH

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::WriteOutDirDescBlk(CMakFileWriter & mw)
{
	ASSERT(mw.IsMakefile());

	BOOL retVal = TRUE;
	CProject * pProject = GetProject();
	ASSERT(pProject != (CProject *)NULL);

	TRY
	{
		CPropBag * pBag;

		ASSERT(GetActiveConfig());
		pBag = ((ConfigurationRecord *)GetActiveConfig())->GetPropBag(CurrBag);
		CStringProp * pOutProp = (CStringProp *)pBag->FindProp(P_OutDirs_Target);
		CStringProp * pIntProp = (CStringProp *)pBag->FindProp(P_OutDirs_Intermediate);

		pBag = ((ConfigurationRecord *)GetActiveConfig()->m_pBaseRecord)->GetPropBag(CurrBag);
		CStringProp * pProjOutProp = (CStringProp *)pBag->FindProp(P_OutDirs_Target);
		CStringProp * pProjIntProp = (CStringProp *)pBag->FindProp(P_OutDirs_Intermediate);

		CString strMacro;

		// next write dep line and 'build rule', if necessary
		if (pOutProp && !pOutProp->m_strVal.IsEmpty())
		{
			// Currently, only the project can have an OutDir defined
			ASSERT(this == pProject);

			// write '"$(OUTDIR)" :'
			VERIFY(GetOutDirString(strMacro, P_OutDirs_Target));
		}
		
		// write OUTDIR inference rule
		if (!strMacro.IsEmpty())
		{
			// write dependency line for '"$(...)" :'
			CString strValue;

			strValue = _T('\"');
			strValue += strMacro;
			strValue += _TEXT("\" :");
			mw.WriteString(strValue);
			mw.EndLine();

			// finally, write mkdir line
			strValue = _TEXT("    if not exist \"");
			strValue += strMacro;

			CString strNull;
			strNull.LoadString(IDS_NULL);
			strValue += _TEXT("/$(");
			strValue += strNull;
			strValue += _TEXT(")\" mkdir \"");
			strValue += strMacro;
			strValue += _TEXT("\"");

			mw.WriteString(strValue);

			mw.EndLine(); mw.InsureSpace();
		}

		if (pIntProp && !pIntProp->m_strVal.IsEmpty())
		{
			if (this == pProject ||
			    ((!pProjOutProp || pProjOutProp->m_strVal.CompareNoCase(pIntProp->m_strVal)) &&
				 (!pProjIntProp || pProjIntProp->m_strVal.CompareNoCase(pIntProp->m_strVal)))
			   )
			{
				if (!pOutProp || pOutProp->m_strVal.CompareNoCase(pIntProp->m_strVal))
				{
					// write '"$(INTDIR)" :'
					if ((GetOutDirString(strMacro, P_OutDirs_Intermediate)) && (!strMacro.IsEmpty()))
					{
						// write INTDIR inference rule
						// write dependency line for '"$(...)" :'
						CString strValue;

						strValue = _T('\"');
						strValue += strMacro;
						strValue += _TEXT("\" :");
						mw.WriteString(strValue);
						mw.EndLine();

						// finally, write mkdir line
						strValue = _TEXT("    if not exist \"");
						strValue += strMacro;

            			CString strNull;
			            strNull.LoadString(IDS_NULL);

            			strValue += _TEXT("/$(");
			            strValue += strNull;
            			strValue += _TEXT(")\" mkdir \"");

						strValue += strMacro;
						strValue += _TEXT("\"");

						mw.WriteString(strValue);

						mw.EndLine(); mw.InsureSpace();
					}
				}
			}
		}
	}
	CATCH (CException, e)
	{
		retVal = FALSE;	// failure
	}
	END_CATCH

	return retVal;
}
			
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::WriteMakeFileDependencies (CMakFileWriter& mw, CNameMunger& nm, BOOL /* bPerConfig */)
{
    // This is where you can write out per-config dep info, e.g.
    // take a look at CFileItem's override.
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::WritePerItemProperties (CMakFileWriter& mw)
{
	ASSERT(!mw.IsMakefile());

	CString strOutput, strBase;
	TCHAR buf [18];
	
	TRY
	{
		for (int i = 0; nMakeFileProps[i].idProp != P_END_MAP; i++)
		{
			// is this a valid prop. for the current item?
			if ((!(nMakeFileProps[i].nRdWrt & PropWrite)) // read-only?
			|| (!(nMakeFileProps[i].nRdWrt & Project) && IsKindOf(RUNTIME_CLASS(CProject))) ||
				(!(nMakeFileProps[i].nRdWrt & Group) && IsKindOf(RUNTIME_CLASS(CProjGroup))) ||
				(!(nMakeFileProps[i].nRdWrt & File) && IsKindOf(RUNTIME_CLASS(CFileItem)))   ||
				(!(nMakeFileProps[i].nRdWrt & TargetRef) && IsKindOf(RUNTIME_CLASS(CProjectDependency))) ||
				(!(nMakeFileProps[i].nRdWrt & Target) && IsKindOf(RUNTIME_CLASS(CTargetItem)))
			   )
				continue;
			
			// Only check props that are not per config
			if (!(nMakeFileProps[i].nRdWrt & NotPerConfig))
				continue;

			// construct our 'PROP <name> <value>' line
			strOutput = pcPropertyPrefix + (CString)nMakeFileProps[i].pPropName + _T(' ');

			UINT idProp = nMakeFileProps[i].idProp;

			switch (nMakeFileProps[i].type)
			{
				case string:
				{
					CString strVal;
					if (GetStrProp(idProp, strVal) != valid)
					{
						// ignore (empty string) if not valid
						continue;
					}
					strOutput += _T('\"') + strVal + _T('\"');
					break;
				}

				case integer:
				case booln:
				{
					int nVal = 0;
					if (GetIntProp(idProp, nVal) != valid)
					{
						// ignore (default 0) if not valid)
						continue;
					}
					// no need to write these if defaults
					if (nVal==0  && (idProp==P_ItemExcludedFromBuild || idProp==P_ItemExcludedFromScan || idProp==P_ItemIgnoreDefaultTool
#ifdef VB_MAKEFILES
							|| idProp==P_Proj_IsVB
#endif
					))
						continue;

					_itoa (nVal, &buf[0], 10);
					strOutput += buf;
					break;
				}

				default:
					ASSERT (FALSE); // Other types not supported.
					continue;
			}

			// write out this as a comment, throw exception if we can't
			mw.WriteComment(strOutput);
		}
	}
	CATCH (CException, e)
	{
		return FALSE;	// failure
	}
	END_CATCH

	return TRUE;	// success
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::WriteMakeFileProperties (CMakFileWriter& mw)
{
	ASSERT(!mw.IsMakefile());

	CString strOutput, strBase;
	TCHAR buf [18];
	
	TRY
	{
		// FUTURE (karlsi): really only need to write Int/Target dirs for base props
		BOOL fWriteBase = TRUE;	// first write out base props
		while (TRUE)
		{
			// our prefix
			strBase = (fWriteBase) ? pcBASE : _T("");
			
			ASSERT(GetActiveConfig());
			if (GetActiveConfig()->GetPropBag(fWriteBase ? BaseBag : CurrBag)->GetPropCount () != 0)
			{
				for (int i = 0; nMakeFileProps[i].idProp != P_END_MAP; i++)
				{
					// We don't write out non per config props here
					if (nMakeFileProps[i].nRdWrt & NotPerConfig)
						continue;
					
					UINT idProp = nMakeFileProps[i].idProp;
					// do we have a property in our bag?
					CProp * pProp = GetActiveConfig()->GetPropBag(fWriteBase ? BaseBag : CurrBag)
													 ->FindProp(idProp);
									   
					if (pProp == (CProp *)NULL)
						continue;	// no, try to get the next one

					// construct our 'PROP <name> <value>' line
					strOutput = pcPropertyPrefix + strBase + nMakeFileProps[i].pPropName + _T(' ');

					switch (pProp->m_nType)
					{
						case string:
						{
							strOutput += _T('\"') + ((CStringProp *)pProp)->m_strVal + _T('\"');
							break;
						}

						case integer:
						case booln:
						{
							int nVal = pProp->m_nType == integer ?
										((CIntProp *)pProp)->m_nVal : ((CBoolProp *)pProp)->m_bVal;

							// no need to write these if defaults
							if (nVal==0  && (idProp==P_ItemExcludedFromBuild || idProp==P_ItemExcludedFromScan || idProp==P_ItemIgnoreDefaultTool
#ifdef VB_MAKEFILES
							|| idProp==P_Proj_IsVB
#endif
							))
								continue;

							// adjust the value?
							if (idProp == P_ProjUseMFC)
							{
								// do we have a use of MFC by AppWiz (LSB3 set)?
								CProp * pProp = GetActiveConfig()->GetPropBag(fWriteBase ? BaseBag : CurrBag)
										->FindProp(P_ProjAppWizUseMFC);
								if( pProp == NULL )
									break;

								ASSERT(pProp->m_nType == integer);
								if (((CIntProp *)pProp)->m_nVal != 0)
									nVal = nVal | 0x4;	// AppWiz gen'ed use of MFC
							}

							_itoa (nVal, &buf[0], 10);
							strOutput += buf;
							break;
						}

						default:
							ASSERT (FALSE); // Other types not supported.
							continue;
					}

					// write out this as a comment, throw exception if we can't
					mw.WriteComment(strOutput);
				}
			}
 			
			// do we want to write out current?
			if (!fWriteBase)	break;	// write current props next
			fWriteBase = FALSE;
		}
	}
	CATCH (CException, e)
	{
		return FALSE;	// failure
	}
	END_CATCH

	return TRUE;	// success
}

///////////////////////////////////////////////////////////////////////////////
// FUTURE (karlsi): move into class CProp::operator==
BOOL CompareProps(const CProp * pProp1, const CProp * pProp2)
{
	if ((pProp1==NULL) || (pProp2==NULL))
		return (pProp1==pProp2);

	ASSERT(pProp1->m_nType==pProp2->m_nType);

	switch (pProp1->m_nType)
	{
		case integer:
			return (((CIntProp *)pProp1)->m_nVal==((CIntProp *)pProp2)->m_nVal);
		case booln:
			return (((CBoolProp *)pProp1)->m_bVal==((CBoolProp *)pProp2)->m_bVal);
		case string:
			return (((CStringProp *)pProp1)->m_strVal==((CStringProp *)pProp2)->m_strVal);
		default:
			ASSERT(0);	// other types not implemented
	};

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::HasMakefileProperties () const
{
	int i, j;
	ConfigurationRecord *pcr;
	int size = GetPropBagCount();

	// only look at the meaningful (writable) props
#ifdef _DEBUG
	// Currently, there are 5 R/O props, plus add 1 for the terminating NULL
	int nProps = (sizeof(nMakeFileProps) / sizeof(nMakeFileProps[0])) - (nReadOnlyProps+1);

	// Currently, there are 18 Props that we actually need to write here
	ASSERT(nProps==nWriteableProps);
#endif
	CProp * CmpProps[nWriteableProps]; // bump this value if nProps changes
	CProp * pProp;

	BOOL fCompareBase = FALSE;	// first compare current props
	while (TRUE)
	{
		for (j=0; j < size; j++)
		{
			pcr = (ConfigurationRecord*)m_ConfigArray[j];
			// if (pcr->GetPropBag(fCompareBase ? BaseBag : CurrBag)->GetPropCount () != 0)
			{
				for (i=0; i < nWriteableProps; i++)
				{
					ASSERT(nMakeFileProps[i].idProp != P_END_MAP);

					// is this a valid prop. for the current item?
					if ((!(nMakeFileProps[i].nRdWrt & Project) && IsKindOf(RUNTIME_CLASS(CProject))) ||
						(!(nMakeFileProps[i].nRdWrt & Group) && IsKindOf(RUNTIME_CLASS(CProjGroup))) ||
						(!(nMakeFileProps[i].nRdWrt & File) && IsKindOf(RUNTIME_CLASS(CFileItem)))   ||
						(!(nMakeFileProps[i].nRdWrt & TargetRef) && IsKindOf(RUNTIME_CLASS(CProjectDependency))) ||
						(!(nMakeFileProps[i].nRdWrt & Target) && IsKindOf(RUNTIME_CLASS(CTargetItem)))
					   )
						continue;

					pProp = (pcr->GetPropBag(fCompareBase ? BaseBag : CurrBag)->FindProp(nMakeFileProps[i].idProp));
					if (j==0)
					{
						// save props from first config for later comparison
						CmpProps[i] = pProp;
					}
					else
					{
						// check for props that differ between configs
						if (!CompareProps(pProp, CmpProps[i]))
						{
							return TRUE;
						}
					}
				}
			}
			
		}
		// repeat test for base props?
		if (fCompareBase)	break;
		fCompareBase = TRUE;	// compare base props next
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::IsPropComment (CObject *&pObject)
{
	// Pls. note that this will digest props regardless of whether or not they are
	// in the prop bag or not....
	if (!pObject->IsKindOf(RUNTIME_CLASS(CMakComment)))
		return FALSE; 

	BOOL retval = FALSE; // assume failure

	CMakComment * pMakComment = (CMakComment *)pObject;

	// get the comment and skip leading whitespace
	TCHAR * pc = pMakComment->m_strText.GetBuffer(1);
	SkipWhite(pc);

	// look for the 'Begin Custom Build' prefix
	if (_tcsnicmp(g_pcBeginCustomBuild, pc, _tcslen(g_pcBeginCustomBuild)) == 0)
	{
		CProject * pProject = GetProject();
		if (pProject->m_bConvertedDS5x || pProject->m_bConvertedDS4x)
		{
			CString strExt = GetFilePath()->GetExtension();
			if (strExt.CompareNoCase(_T(".idl"))==0) // REVIEW
			{
				SetIntProp(P_ItemIgnoreDefaultTool, TRUE);
			}
		}

		// read in the custom build rule
		BOOL fRet = CCustomBuildTool::DoReadBuildRule(this, pc);

		delete pObject; pObject = (CObject *)NULL;
		return fRet;
	}

	// look for the 'Begin Special Build Tool' prefix
	if (_tcsnicmp(g_pcBeginSpecialBuildTool, pc, _tcslen(g_pcBeginSpecialBuildTool)) == 0)
	{
		// read in the custom build rule
		BOOL fRet = CSpecialBuildTool::DoReadBuildRule(this, pc,(m_cp == EndOfConfigs));

		delete pObject; pObject = (CObject *)NULL;
		return fRet;
	}

	// look for the 'PROP' prefix
	if (_tcsnicmp(pcPropertyPrefix, pc, 4) != 0)
		return FALSE;

	int i, j, k, nConfigs;
	CString str;
	TCHAR *pcEnd;

	// Get the property name and look for it in the nMakeFileProps array:
	pc += 4;
	SkipWhite (pc);

	// check if BASE property
	BOOL fUseBase = _tcsnicmp(pcBASE, pc, 5) == 0; 
	if (fUseBase)
	{
		// skip over the "BASE" keyword
		pc += 5; SkipWhite(pc);
	}

 	// set the prop. bag usage
	int idOldBag = UsePropertyBag(fUseBase ? BaseBag : CurrBag);

	BOOL bBlankProp = FALSE;
	GetMakToken(pc, pcEnd);
 	bBlankProp = (*pcEnd == _T('\0'));
	*pcEnd = _T('\0');

	for (i=0; nMakeFileProps[i].idProp != P_END_MAP; i++)
	{
		if (!(nMakeFileProps[i].nRdWrt & PropRead)) continue;
		if ( _tcsicmp (nMakeFileProps[i].pPropName, pc) ==0 ) break;
	}
	if (nMakeFileProps[i].idProp != P_END_MAP )
	{
 		if (bBlankProp)
			pc = pcEnd;
		else
			pc = ++pcEnd;
        
        // Get the end of the prop line
        while (*pcEnd)
            pcEnd = _tcsinc(pcEnd);        

		if (m_cp == EndOfConfigs)
		{
			// global across *all configs* so
			// make sure we have all of our configs matching the project
			CreateAllConfigRecords();

			nConfigs = GetPropBagCount();
		}

		switch (nMakeFileProps[i].type)
		{
			case string:
			{
                // We strip quotes around the property, this is only
                // for compatability with previous makefiles, and should
                // not really be needed here.
                // FUTURE (colint): Delete this when we have a v2 bld convtr
                // that does this, and when most internal v3 makefiles have
                // been saved out to not have surrounding quotes!
                if (*pc == _T('"'))
                {
                    pc++;
                    if (*_tcsdec(pc, pcEnd) == _T('"'))
                        *(--pcEnd) = _T('\0');
                    else
                        ASSERT(FALSE);  // unmatched quotes
                }

				// copy the string arg. between quotes into buffer
				str = pc;

				// may need to re-relativize paths if converted project
				if (GetProject()->m_bConvertedDS4x)
				{
					int id = nMakeFileProps[i].idProp;
					if ((id == P_OutDirs_Target) || (id == P_OutDirs_Intermediate) || (id == P_Proj_TargDir))
					{
						// Use special workspace dir (not actual wkspcdir)
						if ((!str.IsEmpty()) && (str[0] != _T('\\')) && (str[0] != _T('/')) && ((str.GetLength() < 2) || (str[1] != _T(':'))))
						{
							// REVIEW: should really add a temp filename here
							ASSERT(g_pConvertedProject);
							ASSERT(*(LPCTSTR)g_pConvertedProject->GetWorkspaceDir());
							CPath pathDir;
							pathDir.SetAlwaysRelative();
							if (pathDir.CreateFromDirAndFilename(g_pConvertedProject->GetWorkspaceDir(), str))
							{
								if (_tcsicmp((LPCTSTR)pathDir, (LPCTSTR)GetProject()->GetWorkspaceDir())==0)
									str = ".";
								else
									pathDir.GetRelativeName(GetProject()->GetWorkspaceDir(), str);
							}
						}
					}
				}
				if ((m_cp == EndOfConfigs) && !(nMakeFileProps[i].nRdWrt & NotPerConfig))
				{
					for (k=0; k < nConfigs; k++)
					{
						// Force config. active, set prop, and then restore.
						ForceConfigActive((ConfigurationRecord *)m_ConfigArray[k]);
						VERIFY (SetStrProp(nMakeFileProps[i].idProp, str));
						ForceConfigActive();
					}
				}
				else
				{
					ASSERT((m_cp == EndOfConfigs) || !(nMakeFileProps[i].nRdWrt & NotPerConfig));
					CProject * pProject = GetProject();
					SetStrProp ( nMakeFileProps[i].idProp, str );
				}
				str.Empty ();
				break;
			}

			case integer:
			case booln:
			{
				BOOL b;
				j = atoi (pc);

				// using MFC property?
   				if (nMakeFileProps[i].idProp == P_ProjUseMFC)
				{
					// adjust...
					// do we have a use of MFC by AppWiz (LSB3 set)
					b = j & 0x4; j &= 0x3;
				}
										    
				if ((m_cp == EndOfConfigs) && !(nMakeFileProps[i].nRdWrt & NotPerConfig))
				{
					for (k=0; k < nConfigs; k++)
					{
						// Force config. active, set prop, and then restore.
						ForceConfigActive((ConfigurationRecord *)m_ConfigArray[k]);

						SetIntProp(nMakeFileProps[i].idProp, j);

						if (nMakeFileProps[i].idProp == P_ProjUseMFC)
							SetIntProp(P_ProjAppWizUseMFC, b);

						ForceConfigActive();
					}
				}
				else
				{
					ASSERT((m_cp == EndOfConfigs) || !(nMakeFileProps[i].nRdWrt & NotPerConfig));
					SetIntProp(nMakeFileProps[i].idProp, j);

					if (nMakeFileProps[i].idProp == P_ProjUseMFC)
						SetIntProp(P_ProjAppWizUseMFC, b);
				}
			 	break;
			}

			default:
				ASSERT (FALSE); // Other types not supported.
				break;

		}
	}

	// we'll just ignore those props that we don't recognise
	// rather than flag the .MAK as unreadable	
	retval = TRUE; // indicate success

   	// reset the prop. bag usage
 	UsePropertyBag(idOldBag);

	delete pObject; pObject = (CObject *)NULL;

	return retval;
}
///////////////////////////////////////////////////////////////////////////////
void CountkidsRecursively (CProjItem *pItem, int &rCount)
{
	CProjItem *pSubItem;
	rCount++;

	if (pItem->GetContentList ()== NULL
		||
		pItem->IsKindOf (RUNTIME_CLASS (CProject))
		) return;

	for (POSITION pos = pItem->GetHeadPosition (); pos != NULL; )
	{
		pSubItem = (CProjItem *) pItem->GetNext (pos);
		if (pSubItem->IsKindOf (RUNTIME_CLASS (CFileItem ))
			||
			pSubItem->IsKindOf (RUNTIME_CLASS (CProjGroup ))
			||
			pSubItem->IsKindOf (RUNTIME_CLASS (CProject))
			||
			pSubItem->IsKindOf (RUNTIME_CLASS (CTargetItem ))
			) 
		{
			CountkidsRecursively (pSubItem, rCount );
		}
	}
}
int	CProjItem::CountInterestingChildren ()
{
	int nCount = 1;
	if (GetContentList ())
	{
		for (POSITION pos = GetHeadPosition (); pos != NULL; )
		{
			CountkidsRecursively ( (CProjItem *) GetNext(pos), nCount );
		}
	}
	return nCount;
}
///////////////////////////////////////////////////////////////////////////////
int CompareConfigRecs ( const void *pv1, const void *pv2 )
{

	return (
		(*((ConfigurationRecord **) pv1))->GetConfigurationName ().Collate 
			(
			(const TCHAR *) (*((ConfigurationRecord **) pv2))->
														GetConfigurationName ()
			)
		);
}

///////////////////////////////////////////////////////////////////////////////
const TCHAR *CProjItem::GetNamePrefix (CProjItem *pItem)
{
	// FUTURE: nuke this

	CRuntimeClass *pRTC = pItem->GetRuntimeClass ();

	if (pRTC == RUNTIME_CLASS ( CFileItem )) 
										return _TEXT ("Source File" );
	else if (pRTC == RUNTIME_CLASS ( CProjGroup )) 	
										return _TEXT ("Group" );
	else if (pRTC == RUNTIME_CLASS ( CProject )) 	
										return _TEXT ("Project" );
	else if (pRTC == RUNTIME_CLASS ( CProjectDependency ))
										return _TEXT ("Project Dependency" );
	else if (pRTC == RUNTIME_CLASS ( CTargetItem ))
										return _TEXT ("Target" );	// Use BTarget so that first letter is
																	// unique!.

	TRACE ( "CProjItem::GetNamePrefix called for unknown class %s\n", 
												pRTC->m_lpszClassName );
	ASSERT (FALSE);
	return NULL;
}
///////////////////////////////////////////////////////////////////////////////
BOOL  CProjItem::IsEndToken (CObject *& pToken )
{
	if (pToken->IsKindOf ( RUNTIME_CLASS (CMakEndOfFile)))
	{
		delete (pToken); pToken = NULL;
		AfxThrowFileException (CFileException::generic);
	}
	
	if (!pToken->IsKindOf ( RUNTIME_CLASS (CMakComment))) return FALSE;

	const TCHAR *pc = ((CMakComment *) pToken)->m_strText;
	SkipWhite (pc);
	if ( _tcsnicmp ( pc, EndToken, (sizeof(EndToken) -1) / sizeof(TCHAR)) == 0) 
	{
		delete (pToken); pToken = NULL;
		return TRUE;
	}
	return FALSE; 
}
///////////////////////////////////////////////////////////////////////////////
CRuntimeClass *CProjItem::GetRTCFromNamePrefix (const TCHAR *pcPrefix)
{
	// Skip over the "Begin":
	SkipWhite    (pcPrefix);
	SkipNonWhite (pcPrefix);
	SkipWhite    (pcPrefix);

	if ( *pcPrefix == '\0')
		return NULL;

 	// Get the runtime class by looking at the name.
 	if (_tcsnicmp(pcPrefix, _TEXT ("Source File"), 11) == 0)
		return RUNTIME_CLASS ( CFileItem );
	else if (_tcsnicmp(pcPrefix, _TEXT ("Group"), 5) == 0)
		return RUNTIME_CLASS ( CProjGroup );
	else if (_tcsnicmp(pcPrefix, _TEXT ("Project Dependency"), 18) == 0)
		return RUNTIME_CLASS ( CProjectDependency );
	else if (_tcsnicmp(pcPrefix, _TEXT ("Project"), 7) == 0)
		return RUNTIME_CLASS ( CProject );
	else if (_tcsnicmp(pcPrefix, _TEXT ("Target"), 6) == 0)
		return RUNTIME_CLASS ( CTargetItem );
 
 	TRACE ( "CProjItem::GetRTCFromNamePrefix got unknown prefix %s\n",
						pcPrefix );
	return NULL;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::SuckDescBlk(CMakDescBlk *pDescBlk)
{
	BOOL retVal = TRUE;
	BOOL bOldInherit;
	CString str = pDescBlk->m_strTool;

	// get a project type
	CProjType * pprojtype = GetProjType();
	int len = str.GetLength();

	// ignore blk if pseudo-rule for OUTDIR/INTDIR/INTDIR_SRC
	if ((len >= 6) && (!str.Mid(2,4).Compare(_T("TDIR"))))
		return TRUE;

	// FUTURE: <ignatius 4/21/94>
	// Condition comes from ReadMasterDepsLine() in project.cpp
	// We probably should combine both functions together, instead of doing
	// a check for _T("ALL") in both here and ReadMasterDepsLine().
	// 
	if ( ( (_tcsicmp(_T("ALL"), pDescBlk->m_strTargets) == 0) ||
		   (_tcsicmp(_T("CLEAN"), pDescBlk->m_strTargets) == 0) )  && 
		 pprojtype->IsSupported()
	   )
		return TRUE;

	if ((len == 0) && (len = pDescBlk->m_strTargets.Find(_T('_'))) > 0)
	{
		// FUTURE: consider handling this in ParseDescBlk instead
		// also look for a "Fake" target of the form TOOLPREFIX_xxx
		str = pDescBlk->m_strTargets.Left(len);
		ASSERT(len==str.GetLength());
	}

	int i = 0, nMax = m_cp == EndOfConfigs ? GetPropBagCount() : 1;

	// bounds for iteration
	// iterate through all configs if m_cp == EndOfConfigs
	if (m_cp == EndOfConfigs)
	{ 
		// global across *all configs* so
		// make sure we have all of our configs matching the project
		CreateAllConfigRecords();
	
	 	(void) ForceConfigActive((ConfigurationRecord *)m_ConfigArray[0]);
	}

	// nothing to do unless unsupported projtype-specific block
	TRY
	{
		for (;;) // iterate through configs if necessary based on m_cp
		{
			// get a tool for the prefix?
			CBuildTool * pTool = len > 0 ? pprojtype->GetToolFromCodePrefix(str, len) : (CBuildTool *)NULL;

 			// get the action list
			ASSERT(GetActiveConfig());
			CActionSlobList * pActions = GetActiveConfig()->GetActionList();

			// currently got a tool?
			// FUTURE: support multiple actions per-item
			CBuildTool * pCurrentTool = (CBuildTool *)NULL;
			if (!pActions->IsEmpty() && !IsKindOf(RUNTIME_CLASS(CProject)))
				pCurrentTool = ((CActionSlob *)pActions->GetHead())->BuildTool();

			// preserve this tool-specific block
			// o if not an external target, external targets have their own build rules that we can safely ignore.
			// and,
			// o we recognise the block as for an unknown tool, or
			// o we don't have a tool already for the block
			if (!pprojtype->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)) &&
				(!pTool || pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)))
			   )
			{
				// if don't already have a tool, then use the current one
				// else create it
				if (!pCurrentTool)
				{
					if (!pTool)
					{
						// try to get an unknown tool
						const TCHAR * pchPrefix = len > 0 ? (const TCHAR *)str : _TEXT("UNK");
						pTool = pprojtype->GetToolFromCodePrefix(pchPrefix, 3);
						
						if( (_tcsicmp(pchPrefix,"BuildCmds") == 0) && (pTool == NULL) ){
							pTool = g_pBuildTool;
						} 
						
						if (pTool == (CBuildTool *)NULL)
						{
							// no, unknown one, create new projtype-specific 'unknown' tool
							pTool = new CUnknownTool(*pprojtype->GetPlatformName(), pchPrefix);
							pprojtype->AddTool(pTool);
						}
					}

					// add an action if we don't already have one for this tool...
					if (!pActions->Find(pTool))
					{
						// explicitly add an action for this tool
						CActionSlob * pAction;
						pAction = new CActionSlob(this, pTool, FALSE, GetActiveConfig());
						pActions->AddTail(pAction);
					}
				}
				else
				{
					// use the current tool
					pTool = pCurrentTool;
				}

				// read in the raw-data for this descriptor block

				if (pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)) && ( _tcsicmp(_T("ALL"), pDescBlk->m_strTargets) != 0))
				{
					str.Empty();
					bOldInherit = EnablePropInherit(FALSE);
					if (GetStrProp(((CUnknownTool *)pTool)->GetUnknownPropId(), str)!=valid)
						str.Empty();

					// make sure a blank line precedes the build rule
					int len1 = str.GetLength();
					ASSERT(len1==0 || len1 >= 4);
					if ((len1==0) || ((len1 >= 4) && (_tcscmp((LPCTSTR(str) + len1 - 4), _T("\r\n\r\n"))!=0)))
						str += _T("\r\n");

					str += pDescBlk->m_strRawData;
					SetStrProp(((CUnknownTool *)pTool)->GetUnknownPropId(), str);
					EnablePropInherit(bOldInherit);
				}
			}

			// repeat for next config, if necessary
			if (++i >= nMax) break;

			ForceConfigActive();
			ForceConfigActive((ConfigurationRecord *)m_ConfigArray[i]);
		}
	}
	CATCH (CException, e)
	{
		retVal = FALSE;	// failure
	}
	END_CATCH

	ForceConfigActive();
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
// Helper for CProjItem::SuckDependency
FileRegHandle CProjItem::SuckDependency(TCHAR *pchDep, CBuildTool * pTool, const CDir &BaseDir, BOOL & fDepIsPerConfig, BOOL bCaseApproved /*=FALSE*/)
{	
	ASSERT(pTool);
	ASSERT(g_pMakReadDepCache != NULL);
	ASSERT(g_pMakReadIncludeDepCache != NULL);


	CString str;  
	FileRegHandle frh = (FileRegHandle)NULL;

	BOOL bForceRelativePath = ((pchDep[0] == _T('.')) && (pchDep[1] == _T('.')));
	fDepIsPerConfig = FALSE;

	BOOL fIncludeDep = FALSE;

	BOOL fUseIncludeDepCache = FALSE;

	CDirMgr *pDirMgr = GetDirMgr();
	int nToolSet = -1;
	int cToolSet = pDirMgr->GetNumberOfToolsets();

	// substitute for $(OUTDIR) macro if nessessary
	if (*pchDep == _T('$'))
	{
		str = pchDep;
		if (GetProject()->SubstOutDirMacro(str, (ConfigurationRecord *)GetProject()->GetActiveConfig()))
		{
			pchDep = (TCHAR *)(const TCHAR *)str;
			fDepIsPerConfig = TRUE;
		}
	}
	else if (_tcsnicmp(_T("{$(INCLUDE)}"), pchDep, 12)==0)
	{
		fIncludeDep = TRUE;

		// HACK: we only use the cache for the compiler tool at these points.
		// The other tools could have their own tool specific include paths   
		//  and we don't want to keep a cache for #Tools * #Toolsets since 
		// the other tools will typically have only a few deps in the makefile.
		fUseIncludeDepCache = pTool->IsKindOf(RUNTIME_CLASS(CCCompilerTool));

		// if we are from include path, we should start with the macro {$(INCLUDE)}
		// Figure out the name without the $(INCLUDE) part.
		
		// skip backslash and terminate at '"' if there is one
		int cchBase = _tcslen(pchDep);
		if (cchBase < 14)	// bad syntax
			return (FileRegHandle)NULL;

		pchDep += 14;
        cchBase -= 14;
		if (*_tcsdec(pchDep, pchDep + cchBase) == _T('"'))
			*(pchDep + cchBase) = _T('\0');	// terminate

		CProjType *	pprojtype = GetProjType();
		CString strPlatNameTmp = *pprojtype->GetPlatformName();
		nToolSet = strPlatNameTmp.IsEmpty() ? (int)-1 : pDirMgr->GetPlatformToolset(strPlatNameTmp);

		int nToolSetOld = pDirMgr->GetCurrentToolset();
		
		pDirMgr->SetCurrentToolset(nToolSet);
		
		// If nToolSet is -1, which means we are using the current default.
		// To use the cache however we need to determine exactly what the default is.
		// We get that information by calling GetCurrentToolset.
		nToolSet = pDirMgr->GetCurrentToolset();

		if ( nToolSet < 0 || nToolSet >= cToolSet)
			fUseIncludeDepCache = FALSE;

		pDirMgr->SetCurrentToolset(nToolSetOld);

	}

	// see if this string is already in our "dep cache" (which
	// was cleared when we started to read the makefile).  If
	// so, we don't need to create a CPath object and can instead
	// just increment the ref count on the FileRegHandle.
	// There are two cases here.
	// a) We have a workspace relative or an absolute path in which case we can directly 
	//    look in the global cache.
	// b) If the dep had a $(INCLUDE) this could change for every toolset, so we 
	//    look in the toolset specific cache for that.

	void * pv;
	if (!fIncludeDep && g_pMakReadDepCache->Lookup(pchDep, pv))
	{
		frh = (FileRegHandle)pv;
#ifndef REFCOUNT_WORK
		g_FileRegistry.AddRegRef(frh);
#else
		frh->AddFRHRef();
#endif
	}
	else if (fUseIncludeDepCache && g_pMakReadIncludeDepCache[nToolSet].Lookup(pchDep, pv))
	{
		frh = (FileRegHandle)pv;
#ifndef REFCOUNT_WORK
		g_FileRegistry.AddRegRef(frh);
#else
		frh->AddFRHRef();
#endif
	}
	else
	{
		// our dependency file
		CPath cpFile;

		if ( fIncludeDep )
		{
			CString strIncludePath;
			TCHAR buf [MAX_PATH];

			int nToolSetOld = pDirMgr->GetCurrentToolset();

			pDirMgr->SetCurrentToolset(nToolSet);
			// get our include paths for this tool.
			pTool->GetIncludePaths(this, strIncludePath);
			pDirMgr->SetCurrentToolset(nToolSetOld);


			CString strOptionPath;
			int iFound;
			// this is a helper function in scanner.c which replace SearchPath
			if ((iFound=GetFileFullPath((TCHAR *)(LPCTSTR)strIncludePath, pchDep, buf, strOptionPath))!=NOTFOUND)
			{
				if (cpFile.Create(buf))
				{
#ifndef REFCOUNT_WORK
					frh = g_FileRegistry.RegisterFile (&cpFile, TRUE);
#else
					frh = CFileRegFile::GetFileHandle(cpFile);
#endif
				
					if ((iFound == ONPATH) && (frh != NULL))
					{
						g_FileRegistry.GetRegEntry(frh)->SetFileFoundInIncPath();

						if (_tcsstr(pchDep, _TEXT("..\\")))
						{
							g_FileRegistry.GetRegEntry(frh)->SetOrgName(pchDep);
						}
						else
						{
							g_FileRegistry.GetRegEntry(frh)->SetIndexNameRelative(_tcslen(pchDep));
						}
					}
				}
			}
			else
			{
				// well, we can't find this file from the path
				// do we warn user? in any case just register the file with the raw name
				if (cpFile.CreateFromDirAndRelative(BaseDir, pchDep))
#ifndef REFCOUNT_WORK
					frh = g_FileRegistry.RegisterFile(&cpFile, TRUE);
#else
					frh = CFileRegFile::GetFileHandle(cpFile);
#endif
			}
			
			if ( fUseIncludeDepCache && (frh != NULL))
			{
				// Add the FileRegHandle to the per toolset include "dep cache".
#ifndef REFCOUNT_WORK
				g_pMakReadIncludeDepCache[nToolSet].SetAt(pchDep, (void *)frh);
#else
				if (!g_pMakReadIncludeDepCache[nToolSet].Lookup(pchDep, pv))
				{
					g_pMakReadIncludeDepCache[nToolSet].SetAt(pchDep, (void *)frh);
					frh->AddFRHRef();
				}
#endif
			}
		}
		else 
		{
			if (cpFile.CreateFromDirAndRelative(BaseDir, pchDep))
#ifndef REFCOUNT_WORK
				frh = g_FileRegistry.RegisterFile(&cpFile, !bCaseApproved);
#else
				frh = CFileRegFile::GetFileHandle(cpFile);
#endif

			if (frh != NULL)
			{
				// if bCaseApproved then we read this CFileItem in from makefile
				// and will therefore assume correct case is already set...
				if (bCaseApproved)
					frh->SetMatchCase(TRUE, TRUE);

				if (_tcsstr(pchDep, _TEXT("..\\")))
				{
					g_FileRegistry.GetRegEntry(frh)->SetOrgName(pchDep);
					g_FileRegistry.GetRegEntry(frh)->SetFileNameRelativeToProj();
				}

				// Add the FileRegHandle to the global "dep cache".
	#ifndef REFCOUNT_WORK
				g_pMakReadDepCache->SetAt(pchDep, (void *)frh);
	#else
				if (!g_pMakReadDepCache->Lookup(pchDep, pv))
				{
					g_pMakReadDepCache->SetAt(pchDep, (void *)frh);
					frh->AddFRHRef();
				}
	#endif
			}
		}
	}

	if (bForceRelativePath && frh)
		g_FileRegistry.GetRegEntry(frh)->SetRelative(TRUE);

	return frh;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::SuckMacro(CMakMacro * pMacro, const CDir & BaseDir, BOOL bCaseApproved /*=FALSE*/)
{
//	Examine a macro and figure out what its for.  Either its the dependencies
//	macro (in which case it starts with DEP) or it belongs to some tool, in 
//  which case we can get the tool from the macro's prefix:
//  for a tool

	const TCHAR * pMacName = pMacro->m_strName;

	// ignore OUTDIR and INTDIR macros
	if ((_tcsnicmp(_TEXT("OUTDIR"), pMacName, 6) == 0) ||
	    (_tcsnicmp(_TEXT("INTDIR"), pMacName, 6) == 0)
	   )
	{
		return TRUE;	// ok
	}

	BOOL retVal = TRUE;
	FileRegHandle frh;

	BOOL fPerConfig = m_cp != EndOfConfigs;

	TRY
	{	
		// dependency macro?
		if (_tcsnicmp(_TEXT("DEP_"), pMacName, 4) == 0 || // scanned dependency
			_tcsnicmp(_TEXT("NODEP_"), pMacName, 6) == 0 || // missing dependency
			_tcsnicmp(_TEXT("USERDEP_"), pMacName, 8) == 0) // user-defined dependency
		{
			// scanned or missing dependency?
			UINT depType;
			if (*pMacName == _T('N'))
			{
				depType = DEP_Missing;
				pMacName += (6 * sizeof(TCHAR));
			}
			else if (*pMacName == _T('U'))
			{
				depType = DEP_UserDefined;
				pMacName += (8 * sizeof(TCHAR));
			}
			else
			{
				depType = DEP_Scanned;
				pMacName += (4 * sizeof(TCHAR));
			}

			// if we haven't got any actions then this must be for an unknown tool
			ASSERT(GetActiveConfig());
			CActionSlobList * pActions = GetActiveConfig()->GetActionList();
			if (pActions->IsEmpty())
			{
				CBuildTool * pTool;

				// search for tool prefix
				TCHAR * pUnderScore = _tcschr(pMacName, _T('_'));
				int index = pUnderScore != (TCHAR *)NULL ? pUnderScore - pMacName : -1;

				// found the tool prefix?
				CProjType *	pprojtype = GetProjType();

				TCHAR * pchPrefix;
				if (index == -1)
				{
					// we'll have to default to 'UNK' (unknown)
					pchPrefix = _TEXT("UNK");
					index = 3;
				}
				else
				{
					// we'll get the tool for the tool-prefix
					pchPrefix = (TCHAR *)pMacName;
				}

				// no tool?
				pTool = pprojtype->GetToolFromCodePrefix(pchPrefix, 3);
				/*
				if( (_tcsicmp(pchPrefix,"BuildCmds") == 0) && (pTool == NULL) ){
					pTool = g_pBuildTool;
				} 
				*/
				
				if (pTool == (CBuildTool *)NULL)
				{
					// create new projtype-specific 'unknown' tool
					pTool = new CUnknownTool(*pprojtype->GetPlatformName(), pchPrefix);
					pprojtype->AddTool(pTool);
					g_pLastTool = pTool;
				}

				// make sure we have this tool in our action-list if it's unknown
				if (pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)) &&
					!pActions->Find(pTool)
				   )
				{
					// explicitly add an action for this tool
					CActionSlob * pAction;
					ASSERT(GetActiveConfig());
					pAction = new CActionSlob(this, pTool, FALSE, GetActiveConfig());
					pActions->AddTail(pAction);
				}
			}

			ConfigurationRecord * pcrSrc = GetActiveConfig(), * pcrDest;

			// We're going to want this across all configs
			if (!fPerConfig)
				CreateAllConfigRecords();	// make sure we have all of the configs...

			// FUTURE: handle more than one tool on this source file
			ASSERT(pActions->GetCount() == 1);	  
			CActionSlob * pAction = (CActionSlob *)pActions->GetHead();

			// get the dependency list to create
			CFileRegSet * psetDep;
			if (depType == DEP_Scanned)
				psetDep = pAction->GetScannedDep();

			else if (depType == DEP_UserDefined)
			{
				// Do nothing.
			}

			else if (depType == DEP_Missing)
				psetDep = pAction->GetMissingDep();

			else
				ASSERT(FALSE);

			// Parse the dependants list to get our file name and dependents:

			// This is the accepted way to get a mungeable pointer a CString's
			// data.  Note we never call ReleaseBuffer:
			TCHAR * pBase = pMacro->m_strValue.GetBuffer (1);	

			// Squirrel away the length of the deps string, since we'll be 
			// writing all over it:
			const TCHAR * pDepEnd =  (const TCHAR *)pMacro->m_strValue + pMacro->m_strValue.GetLength();
	
			// possible token start and end
			TCHAR * pTokenEnd = max(pBase, (TCHAR *) pDepEnd-1);
			
			// generic buffer	
			CString str;

			for ( ; pTokenEnd < pDepEnd; pBase = _tcsinc (pTokenEnd) )
			{
				// skip whitespace
		  		SkipWhite (pBase);

				TCHAR * pTokenStart = pBase;

				// skip lead-quote?
				if (pBase[0] == _T('"'))
					pBase++;

				// get .mak token
				GetMakToken(pTokenStart, pTokenEnd);

				// back-up over end-quote?
				if (*(pTokenEnd - 1) == _T('"'))
					*(pTokenEnd - 1) = _T('\0');
				else
					*pTokenEnd = _T('\0'); 

				// get our dependency from this string
				BOOL fDepIsPerConfig = TRUE;	// we want the first one, so pretend per-config

				// per-configuration
				if (fPerConfig)
				{
					// Add user-defined dep to configuration record.
					if (DEP_UserDefined==depType)
					{
						CString strDeps;
						if (invalid == GetStrProp(P_UserDefinedDeps, strDeps) )
							strDeps.Empty();

						strDeps += _T("\"");
						strDeps += pBase;
						strDeps += _T("\"\t");
						VERIFY(SetStrProp(P_UserDefinedDeps, strDeps));
					}

					// add this per-config dep.
					if ((frh = SuckDependency(pBase, pAction->m_pTool, BaseDir, fDepIsPerConfig, FALSE)) == (FileRegHandle)NULL)
					{
						// just ignore bad deps
						continue;
					}

 					// olympus 778 (briancr)
					// a better fix to this bug is to still suck in the
					// dependencies, but then add them to the appropriate list
					if (depType == DEP_Scanned) {
						pAction->AddScannedDep(frh);
					}
					else if (depType == DEP_UserDefined) {
						pAction->AddSourceDep(frh);
					}
					else{
						pAction->AddMissingDep(frh);
					}
#ifdef REFCOUNT_WORK
					frh->ReleaseFRHRef();
#endif
				}
				else
				{
					int nSize = m_ConfigArray.GetSize();
					
#ifdef REFCOUNT_WORK
					frh = NULL;
#endif
					CProjTempConfigChange projTempConfigChange(GetProject());

					// add this dep. to all configs.
					for (int nConfig = 0; nConfig < nSize; nConfig++)
					{
						pcrDest = (ConfigurationRecord *)m_ConfigArray[nConfig];

						// Get dep list to copy to
						projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcrDest->m_pBaseRecord);

						// Should we do this?
						ASSERT(GetActiveConfig());
						if (!GetTarget()->GetActiveConfig()->IsValid())
							continue;

						CActionSlobList * pActions = GetActiveConfig()->GetActionList();
						if (!pActions->IsEmpty())
						{
							// FUTURE: handle more than one tool on this source file
							ASSERT(pActions->GetCount() == 1);	  
							CActionSlob * pAction = (CActionSlob *)pActions->GetHead();

 							// add this per-config dep.?
#ifdef REFCOUNT_WORK
							if (fDepIsPerConfig && NULL!=frh)
							{
								frh->ReleaseFRHRef();
								frh = NULL;
							}
#endif
							// Add user-defined dep to configuration record.
							if (DEP_UserDefined==depType)
							{
								CString strDeps;
								if (invalid == GetStrProp(P_UserDefinedDeps, strDeps) )
									strDeps.Empty();

								strDeps += _T("\"");
								strDeps += pBase;
								strDeps += _T("\"\t");
								VERIFY(SetStrProp(P_UserDefinedDeps, strDeps));
							}

							if (fDepIsPerConfig &&
								(frh = SuckDependency(pBase, pAction->m_pTool, BaseDir, fDepIsPerConfig, bCaseApproved && (DEP_UserDefined != depType))) == (FileRegHandle)NULL)
							{
								// just ignore bad deps
								continue;
							}

 							// olympus 778 (briancr)
							// a better fix to this bug is to still suck in the
							// dependencies, but then add them to the appropriate list
							
							if (frh != NULL)
							{
								if (depType == DEP_Scanned)
								{
									pAction->AddScannedDep(frh);
								}
								else if (depType == DEP_UserDefined)
								{
									pAction->AddSourceDep(frh);
								}
								else
								{
									pAction->AddMissingDep(frh);
								}
							}
						}
					}
#ifdef REFCOUNT_WORK
					if (NULL!=frh)
					{
						frh->ReleaseFRHRef();
						frh = NULL;
					}
#endif
				}
			}
		}
		// tool macro or some tool option macro?
		else
		{
			// probably a tool definition or tool macro, or some random macro
			CBuildTool * pTool = NULL;
			int index = pMacro->m_strName.Find(_T('_'));
			int i = 0, nMax = fPerConfig ? 1 : GetPropBagCount();

			// tool macro?
			if (index == -1 && IsKindOf(RUNTIME_CLASS(CProject)))
			{
				for (;;) // iterate through configs if necessary based on m_cp
				{
					if (!fPerConfig)
						ForceConfigActive((ConfigurationRecord *)m_ConfigArray[i]);

					// tool definition; extract tool name
					CProjType * pprojtype = GetProjType();
					/*
					if( _tcsicmp(pMacName,"BuildCmds") == 0 ){
						pTool = g_pBuildTool;
					} 
					else
					*/
					pTool = pprojtype->GetToolFromCodePrefix( pMacName, _tcslen(pMacName) );
					if ( pTool == NULL )
					{
						// create new projtype-specific 'unknown' tool
						pTool = new CUnknownTool(*pprojtype->GetPlatformName(),
										pMacro->m_strName, pMacro->m_strValue);

						pprojtype->AddTool(pTool);
						g_pLastTool = pTool;
					}

					// repeat for next config, if necessary
					if (++i >= nMax)
						break;
				}
			}
	 		else if (index != -1)
			{
				for (;;) // iterate through configs if necessary based on m_cp
				{
					if (!fPerConfig)
						ForceConfigActive((ConfigurationRecord *)m_ConfigArray[i]);

					if ((pTool = GetProjType()->GetToolFromCodePrefix(pMacName, index)) != NULL)
					{
						g_pLastTool = pTool;	// save most recently used tool
					}
					else
					{
						pTool = g_pLastTool;	// worst-case default
					}

					// found macro for some tool?
					if (pTool)
					{
						ASSERT(GetActiveConfig());
						CActionSlobList * pActions = GetActiveConfig()->GetActionList();

						// make sure we have this tool in our action-list if it's unknown
						if (pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)) &&
							!pActions->Find(pTool)
						   )
						{
							// explicitly add an action for this tool
							CActionSlob * pAction;
							pAction = new CActionSlob(this, pTool, FALSE, GetActiveConfig());
							pActions->AddTail(pAction);
						}

						if (!pTool->ReadToolMacro(pMacro, this))
							AfxThrowFileException(CFileException::generic);
					}

					// repeat for next config, if necessary
					if (++i >= nMax)
						break;
				}
			}
			else if( (_tcsicmp(pMacName,"BuildCmds") == 0) && (g_pBuildTool != NULL) 
					&& g_pBuildTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)) ){
				// append the whole macro to the unknow prop in the last tool!
				g_pBuildTool->ReadToolMacro(pMacro,this);
			}
		}
	}
	CATCH (CException, e)
	{
		retVal = FALSE;	// failure
	}
	END_CATCH

	ForceConfigActive();

	return retVal;	// success?
}
///////////////////////////////////////////////////////////////////////////////

BOOL CProjItem::IsAddSubtractComment (CObject *&pObject)
{
	if ( !pObject->IsKindOf ( RUNTIME_CLASS (CMakComment))) return FALSE; 
	TCHAR *pc, *pcOld;
	CBuildTool *pbt;
	BOOL bRetval = FALSE, bSubtract;
	BOOL bBase = FALSE;
	int i, nRepCount = 1;

	pc = ((CMakComment *) pObject)->m_strText.GetBuffer (1);
	SkipWhite (pc);

	// Look for the ADD or SUBTRACT prefix:
	if (  _tcsnicmp(pcADD, pc, 4) == 0 ) 
	{
		bSubtract = FALSE;
		pc += 4;
	}
	else if	(  _tcsnicmp(pcSUBTRACT, pc, 9) == 0 ) 
	{
		bSubtract = TRUE;
		pc += 9;
	}
	else return FALSE;

	SkipWhite (pc);

	// Check for BASE flag
	if (!_tcsnicmp(pcBASE, pc, 5))
	{
		bBase = TRUE;
		pc += 5;	// skip past base flag
		SkipWhite (pc);
	}
	
	// global across *all configs* so
	// make sure we have all of our configs matching the project
	CreateAllConfigRecords();

	if (m_cp == EndOfConfigs)
	{
		// FUTURE (karlsi): this is not very efficient, since we have to
		// reparse the same line n times.  Ideally, we could
		// defer this to when we set the properties, except
		// different configs might have different tools, and it
		// might get tricky. Reevaluate for V5.

		nRepCount = GetPropBagCount();
		pcOld = pc;	// save char pointer for each iteration
	}

	for (i = 0; i < nRepCount; i++)
	{
		if (m_cp == EndOfConfigs)
		{
			pc = pcOld;	// restore pointer to tool code-prefix

			// force config active
			ForceConfigActive((ConfigurationRecord *)m_ConfigArray[i]);
		}

		// Ask the current project type which one of its tools
		// matches the tool code-prefix.
		CProjType * pprojtype = GetProjType(); 
		g_pBuildTool = pbt = pprojtype->GetToolFromCodePrefix(pc, _tcslen (pc));
		

		if (pbt != NULL)
		{
			// Skip to start of flags to end of string
			while (*pc != _T('\0') && !_istspace(*pc))
				pc = _tcsinc(pc);	// next MBC character
	
			bRetval = pbt->ProcessAddSubtractString(this, pc, bSubtract, bBase);

			// FUTURE (karlsi): Do we want to allow/ignore failures
			//         when m_cp == EndOfConfigs?
			if (!bRetval)
			{
				// reset config.?
				if (m_cp == EndOfConfigs)
					ForceConfigActive();

				break; // quit if fails for any Config
			}
		}

		// reset config.?
		if (m_cp == EndOfConfigs)
			ForceConfigActive();
	}

	delete pObject;	pObject = NULL;

	// Something went wrong, but this token was for us:
	if (!bRetval)
	{
		ASSERT(GetProject());
		if (GetProject() && (!GetProject()->m_bProjConverted))
			AfxThrowFileException (CFileException::generic);
	}
	return TRUE;	

}
///////////////////////////////////////////////////////////////////////////////
void CProjItem::WriteConfigurationIfDef ( 
						CMakFileWriter& mw,
						const ConfigurationRecord *pcr
						)	 // (throw CFileException)
{

	CString str;
	char *pc;
	
	mw.InsureSpace ();	
	if ( m_cp == EndOfConfigs )		// Write !ENDIF
	{
		mw.WriteDirective (CMakDirective::DTYP_ENDIF, NULL);
	}	
	else
	{
		const CString &strCfg = pcr->GetConfigurationName ();

		int len = 16 + strCfg.GetLength ();
		pc = str.GetBuffer (len);
		_tcscpy ( pc, _TEXT (" \"$(CFG)\" == \"") );
		_tcscpy ( pc +	14, (const char *) strCfg);
		_tcscpy ( pc + len -2, _TEXT ("\"")); 

		mw.WriteDirective (
				(( m_cp == FirstConfig) ? CMakDirective::DTYP_IF : 
									   CMakDirective::DTYP_ELSEIF)
				,pc
				);
	}
	mw.EndLine ();
}
///////////////////////////////////////////////////////////////////////////////
// (throw CFileException if unknown confiugration)
void CProjItem::ReadConfigurationIfDef(CMakDirective *pmd, BOOL bCheckProj /* = FALSE */)
{
	// N.B. FUTURE (karlsi): Currently we don't handle nested switches
	TCHAR *pstart, *pend;

	if (pmd->m_dtyp == CMakDirective::DTYP_ENDIF)
	{
		if (bCheckProj)
		{
			ASSERT(IsKindOf(RUNTIME_CLASS(CProject)));
			CString strProject;
			CProject * pProject;
			CProject::InitProjectEnum();
			while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, FALSE)) != NULL)
			{
				pProject->m_cp = EndOfConfigs;
			}
		}
		else
		{
			m_cp = EndOfConfigs;
		}
	}
	else if (pmd->m_dtyp == CMakDirective::DTYP_ELSEIF ||
			 pmd->m_dtyp == CMakDirective::DTYP_IF)
	{
		// Skip over "$(CFG)" part
		pstart	= pmd->m_strRemOfLine.GetBuffer(1);
		if (!GetQuotedString (pstart, pend) ||
			_tcsncmp ( _TEXT("$(CFG)"), pstart, 6) != 0)
			AfxThrowFileException (CFileException::generic);

		pstart =_tcsinc(pend);
		if (!GetQuotedString(pstart, pend))
			AfxThrowFileException (CFileException::generic);

		*pend = _T('\0');

		// put the whole project into this configuration
		if (bCheckProj)
		{
			ASSERT(IsKindOf(RUNTIME_CLASS(CProject)));
			CString strProject;
			BOOL bFoundProject = FALSE;
			CProject * pProject = (CProject *)this;
			CProject::InitProjectEnum();
			while ((pProject = (CProject *)CProject::NextProjectEnum(strProject, FALSE)) != NULL)
			{
				if (!bFoundProject)
				{
					int i;
					ConfigurationRecord* pcr;
					int size = pProject->GetPropBagCount();
					for (i=0; i < size; i++)
					{
						pcr = (ConfigurationRecord*) pProject->GetConfigArray()->GetAt(i);
						ASSERT(pcr);
						if (pcr->GetConfigurationName().CompareNoCase(pstart) == 0)
						{
							bFoundProject = TRUE;
							g_pActiveProject = pProject;
							break;
						}
					}
				}
				pProject->m_cp = (pmd->m_dtyp == CMakDirective::DTYP_IF) ? 
						FirstConfig : MiddleConfig;									 
			}

			ASSERT(bFoundProject);
			if (!bFoundProject)
				AfxThrowFileException (CFileException::generic);

			g_pActiveProject->SetActiveConfig(pstart);
		}
		else	
		{
			GetProject()->SetActiveConfig(pstart);
			m_cp = (pmd->m_dtyp == CMakDirective::DTYP_IF) ? 
					FirstConfig : MiddleConfig;									 
		}

	}
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::GetDependenciesMacro(UINT depType, CString & strDeps, CNameMunger & nm)
{
    ConfigurationRecord * pcr = GetActiveConfig();
    if (pcr == (ConfigurationRecord *)NULL)
		return FALSE;

	CActionSlobList * pActions = pcr->GetActionList();
	if (pActions->IsEmpty())
		return FALSE;

	// FUTURE: handle more than one tool on this source file
	ASSERT(pActions->GetCount() == 1 || !IsKindOf(RUNTIME_CLASS(CFileItem)));
	CActionSlob * pAction = (CActionSlob *)pActions->GetHead();

	CFileRegSet * psetDep;

	// get approp. dep. set
	if (depType == DEP_Scanned)
		psetDep = pAction->GetScannedDep();

	else if (depType == DEP_Missing)
		psetDep =pAction->GetMissingDep();

	else if (depType == DEP_UserDefined)
		psetDep = NULL /*pAction->GetUserDefinedDep()*/;

	else
		ASSERT(FALSE);

	// any deps?
#ifndef REFCOUNT_WORK
	if (psetDep->GetContent()->IsEmpty())
		return FALSE;
#else
	if (depType == DEP_UserDefined )
	{
		BOOL fExcluded;
		CString strDeps;
		if (invalid == GetStrProp(P_UserDefinedDeps, strDeps) )
			return FALSE;
		else if (strDeps.IsEmpty())
			return FALSE;
		// REVIEW: we currently don't persist excluded custom build rules, so don't persist
		//			the user deps either
		else if ((GetIntProp(P_ItemExcludedFromBuild, fExcluded) == valid) && (fExcluded))
			return FALSE;
	}
	else
	{
		ASSERT(IsKindOf(RUNTIME_CLASS(CFileItem)));

		if (psetDep->IsEmpty() && !((CFileItem *)this)->m_bAlwaysWriteDepMacro)
		{
			return FALSE;
		}
	}
#endif

	CBuildTool * pTool = GetSourceTool();
	ASSERT(pTool != (CBuildTool *)NULL);

	// construct dependency macro of the form
	// 'DEP_<tool prefix>_<munged name>' eg. 'DEP_CPP_APPWIZ'
	CString strMungeName;
	nm.MungeName (GetFilePath()->GetFileName (), strMungeName);

	// get approp. dep. prefix
	if (depType == DEP_Scanned)
		strDeps = _TEXT("DEP_");

	else if (depType == DEP_Missing)
		strDeps = _TEXT("NODEP_");

	else if (depType == DEP_UserDefined)
	{
		// Only custom build rules can have user defined deps for 5.0.
		ASSERT((pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool))) ||
			(pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool))));

		strDeps = _TEXT("USERDEP_");
	}

	else
		ASSERT(FALSE);

	// tool prefix?
	strDeps += pTool->m_strToolPrefix;

	// filename postfix?
	if ((depType != DEP_UserDefined) || (!pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool))))
	{
		strDeps += _T("_");
		strDeps += strMungeName;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
HCURSOR CProjItem::GetCursor(int ddcCursorStyle)
{
	int iCursors = 0;	// At the moment we only have 1 set of cursors!

	switch (ddcCursorStyle)
	{
		case DDC_SINGLE:
			return g_Cursors.GetDDCur(iCursors);

		case DDC_COPY_SINGLE:
			return g_Cursors.GetDDCopyCur(iCursors);

		case DDC_MULTI:
			return g_Cursors.GetDDMultiCur(iCursors);

		case DDC_COPY_MULTI:
			return g_Cursors.GetDDMultiCopyCur(iCursors);

		default:
			ASSERT(FALSE);
			return NULL;
	}
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption)
{
	// no page
	
	return CProjSlob::SetupPropertyPages(pNewSel, bSetCaption);
}
///////////////////////////////////////////////////////////////////////////////
void CProjItem::InformDependants(UINT idChange, DWORD dwHint)
{
	CSlob::InformDependants(idChange, dwHint);

	BOOL fIsProxySlob = IsKindOf(RUNTIME_CLASS(CProxySlob));

	//
	// Inform other non-CSlob, dependents here ....
	//
	// only inform if not CProxySlob
	if (!fIsProxySlob)
	{
		CProject * pProject = GetProject();

		if (idChange == P_ProjUseMFC &&	pProject->m_bProjectComplete)
		{
			// we need to make sure we have the right setup
			// for an MFC project
			CProjType * pprojtype = pProject->GetProjType();
			int val;
			if (pprojtype != (CProjType *)NULL &&
				pProject->GetIntProp(P_ProjUseMFC, val)
			   )
			{
				pprojtype->PerformSettingsWizard(this, val);
			}
		}
	}

	// 
	// Inform other, non-CSlob, dependents here ....
	//
	// only inform if to current bag and not CProxySlob, ie. not the clone bag during Project.Settings
	if (m_idBagInUse == CurrBag && !fIsProxySlob)
	{
		// inform any associated actions so that they may add/remove inputs and outputs?
		// or that the file may become a part of the build
		if (idChange == P_ItemExcludedFromBuild)
		{
			CProject * pProject = GetProject();
			CTargetItem* pTarget = GetTarget();

			if (pProject != (CProject *)NULL && pProject->m_bProjectComplete &&	// project complete
						pTarget->IsFileInTarget(GetFileRegHandle()))	// part of project
			{
				if (!IsKindOf(RUNTIME_CLASS(CDependencyFile)) &&
					!IsKindOf(RUNTIME_CLASS(CDependencyContainer))
				   )
				{
#ifndef REFCOUNT_WORK
					// including or excluding in build?
					BOOL fExcluded;
					if (GetIntProp(P_ItemExcludedFromBuild, fExcluded) == valid)
					{
						if (fExcluded)
							CActionSlob::ReleaseRefFileItem(this);
						else	
							CActionSlob::AddRefFileItem(this);
					}
#else
					if (IsKindOf(RUNTIME_CLASS(CFileItem)))
					{
							// Add to dependency graph as possible input to schmooze tool.
						BOOL fExcluded;
						if (GetIntProp(P_ItemExcludedFromBuild, fExcluded) == valid)
						{
							ConfigurationRecord* pcr = GetActiveConfig();
							ASSERT( NULL!=pcr);
							CFileRegistry *preg = g_buildengine.GetRegistry(pcr);
							ASSERT(NULL!=preg);
							FileRegHandle frh = GetFileRegHandle();
							ASSERT(NULL!=frh);

							if (fExcluded)
								preg->ReleaseRegRef(frh);
							else
								preg->AddRegRef(frh);
						}
					}
#endif
				}

				// local, only affects outputs for those actions assoc. with this item
				CActionSlob::InformActions(this, P_ItemExcludedFromBuild, (ConfigurationRecord *)NULL, FALSE);
			}
		}
  		
		// inform our actions that their output state is likely to have changed?
 		// (many output files are derived from the output directories and/or remote target)
		else if (idChange == P_OutDirs_Target || idChange == P_OutDirs_Intermediate ||
				 idChange == P_TargetName || idChange == P_RemoteTarget ||
                 idChange == P_DeferredMecr)
		{
			CProject * pProject = GetProject();
			// only inform if the project is complete, ie. not constructing/destructing
			if (pProject != (CProject *)NULL && pProject->m_bProjectComplete)
			{
				// global, may affect childrens outputs
				CActionSlob::InformActions(GetTarget(), P_ToolOutput);

				// When the output directory changes make sure we rescan the rc files
				// This is a hack so that we fix up tlb dependencies correctly
//				if (!g_bNewUpdateDepModel&& idChange == P_OutDirs_Target)
				if (idChange == P_OutDirs_Target)
				{
					CObList ol;
					pProject->FlattenSubtree(ol, flt_Normal | flt_ExcludeDependencies);

					POSITION pos = ol.GetHeadPosition();
					while (pos)
					{
						CProjItem * pItem = (CProjItem *)ol.GetNext(pos);
						if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
						{
							CString strExt = pItem->GetFilePath()->GetExtension();
							if (strExt.CompareNoCase(_T(".RC")) == 0)
							{
								// Must rescan this item as it is a rc file.
//								FileItemDepUpdateQ((CFileItem *)pItem, NULL, NULL, g_FileForceUpdateListQ);
//								FileItemDepUpdateQ((CFileItem *)pItem, NULL, pItem->GetFileRegHandle(), g_FileDepUpdateListQ);
							}
						}
					}
				}

				// FUTURE:
				// this is a hack for project-level custom-build steps
				// informing that the inputs (.objs) to the .exe has changed
				// will remove the .exe and then re-create, we need
				// to re-affix the project-level custom-build step thus...
				CBuildTool * pTool;
				VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_Tool_CustomBuild), (CBldSysCmp *&)pTool));

				CPtrList lstSelectTools;
				lstSelectTools.AddTail(pTool);

				// Special build tool to handle pre-link/post-build.
				VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_Tool_SpecialBuild), (CBldSysCmp *&)pTool));
				lstSelectTools.AddTail(pTool);

#if 0
				CProjItem * pMyItem = pProject;
				CTargetItem * pMyTarget = pMyItem->GetTarget();
				ASSERT(pMyTarget->GetTarget());
#endif
				// un-assign any project-level custom build tool
				CActionSlob::UnAssignActions(pProject, &lstSelectTools, (ConfigurationRecord  *)NULL, FALSE);

				// try to re-assign any project-level custom build tool
				CActionSlob::AssignActions(pProject, &lstSelectTools, (ConfigurationRecord  *)NULL, FALSE);

			}
		}

		// setting the target output directory or ext. opts .bsc name? -> inform packages
		// (the user has potentially changed the .BSC name)
		if (idChange == P_OutDirs_Target || idChange == P_ExtOpts_BscName || idChange == P_Proj_BscName)
		{ 
			CProject * pProject = GetProject();
			// only inform if the project is complete, ie. not constructing/destructing
			if (pProject != (CProject *)NULL && pProject->m_bProjectComplete && pProject->m_bNotifyOnChangeConfig )
				theApp.NotifyPackages(PN_CHANGE_BSC);	
		}
		
		// configuration name changes may need to be forwarded to targets/target references displaying
		// the configuration name that has changed
		else if (idChange == P_ProjConfiguration && GetProject() && GetProject()->m_bProjectComplete)
		{
			CString strConfig;
			VERIFY(GetStrProp(P_ProjConfiguration, strConfig));

			CProject * pProject = GetProject();
			CBuildNode * pBldNode = GetBuildNode();
			pBldNode->InformBldSlobs(this, P_ProjConfiguration, dwHint);
		}

		// option handler action changes
		else if (idChange >= PROJ_BUILD_OPTION_FIRST && idChange <= PROJ_BUILD_OPTION_LAST)
		{
			// get the option handler for the prop?
			COptionHandler * pOptHdlr = g_prjcompmgr.FindOptionHandler(idChange);
			CBuildTool * pTool = pOptHdlr != (COptionHandler *)NULL ? pOptHdlr->m_pAssociatedBuildTool : (CBuildTool *)NULL;
			
			// affects the outputs?
			if (pTool != (CBuildTool *)NULL && pTool->AffectsOutput(pOptHdlr->MapActual(idChange)))
			{
				// local, only affects outputs of those actions interested in tool
				pTool->InformDependants(P_ToolOutput, (DWORD)this);
			}
		}

		// break else here... to test for SN_ALL

		// re-assign custom build actions changes
		if (idChange == P_CustomCommand || idChange == P_CustomOutputSpec ||	// semantic change
			idChange == P_UserDefinedDeps ||

			idChange == SN_ALL ||												// global change

			idChange == P_TargetName ||											// target name (binding)
			idChange == P_OutDirs_Target ||										// target output (binding)
			idChange == P_PreLink_Command ||
			idChange == P_PostBuild_Command ||
			idChange == P_RemoteTarget											// target output (binding)
		   )
		{
			CProject * pProject = GetProject();

			if (pProject != (CProject *)NULL && pProject->m_bProjectComplete)	// project complete
			{
				CBuildTool * pTool;
				VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_Tool_CustomBuild), (CBldSysCmp *&)pTool));

				CPtrList lstSelectTools;
				lstSelectTools.AddTail(pTool);

				// Special build tool to handle pre-link/post-build.
				VERIFY(g_prjcompmgr.LookupBldSysComp(GenerateComponentId(idAddOnGeneric, BCID_Tool_SpecialBuild), (CBldSysCmp *&)pTool));
				lstSelectTools.AddTail(pTool);

				// un-assign any custom build tool
#if 0
				if (pProject == this)
				{
					ASSERT(GetTarget());
					CActionSlob::UnAssignActions(GetTarget(), &lstSelectTools, (ConfigurationRecord  *)NULL, FALSE);

					// try to re-assign a custom build tool
					CActionSlob::AssignActions(GetTarget(), &lstSelectTools, (ConfigurationRecord  *)NULL, FALSE);
				}
				else
#endif
				{
					CActionSlob::UnAssignActions(this, &lstSelectTools, (ConfigurationRecord  *)NULL, FALSE);

					// try to re-assign a custom build tool
					CActionSlob::AssignActions(this, &lstSelectTools, (ConfigurationRecord  *)NULL, FALSE);
				}

#if 0
				// We are never actually changing this property when
				// idChange is any of the ones in the if stmt above.
				// calling this causes us to do gratuitous AddRef's or 
				// ReleaseRef's [sanjays]

				// possible buildable change
				InformDependants(P_ItemExcludedFromBuild);
#endif

				// possible target-name change
				if (idChange == P_TargetName)
				{
					CString strConfig;
					VERIFY(GetStrProp(P_ProjConfiguration, strConfig));

					CProject * pProject = GetProject();
					CBuildNode * pBldNode = GetBuildNode();
					pBldNode->InformBldSlobs(this, P_TargetName, dwHint);
				}
			}
		}

		// break else here... to test for SN_ALL

		// global action changes
 		// (SN_ALL or builder filename change)
		if (idChange == SN_ALL ||				// global change
			idChange == P_BldrFileName ||
			idChange == P_Proj_TargDefExt		// default target extension
		   )
		{
			COptionHandler * pOptHdlr;
			CPtrList lstTool;	 

			g_prjcompmgr.InitOptHdlrEnum();
			while (g_prjcompmgr.NextOptHdlr(pOptHdlr))
			{
				CBuildTool * pTool = pOptHdlr->m_pAssociatedBuildTool;

				// affects the outputs?
				if (pTool != (CBuildTool *)NULL &&
					(idChange == SN_ALL || pTool->AffectsOutput(idChange))
				   )
					lstTool.AddTail(pTool);
			}

			POSITION pos = lstTool.GetHeadPosition();
			while (pos != (POSITION)NULL)
				// local, only affects outputs of those actions interested in tool
				((CBuildTool *)lstTool.GetNext(pos))->InformDependants(P_ToolOutput, (DWORD)this);
		}
	}

	if (idChange == P_ItemIgnoreDefaultTool && (!fIsProxySlob))
	{
		ASSERT(IsKindOf(RUNTIME_CLASS(CFileItem)));
		if (GetProject() && GetProject()->m_bProjectComplete)
		{
			ConfigurationRecord * pcr = (ConfigurationRecord *)GetActiveConfig()->m_pBaseRecord;
			CActionSlob::UnAssignActions(this, NULL, pcr);
			CActionSlob::AssignActions(this, NULL, pcr);
		}
	}

	// Inform the Build.Settings tree of changes, so it can display changes to target names
	// and also display changes to item glyphs when exclude from build state changes.
	CProject * pProject;
	POSITION posProjects = CProject::GetProjectList()->GetHeadPosition();
	while (posProjects != NULL)
	{
		pProject = (CProject *)CProject::GetProjectList()->GetNext(posProjects);
		if ((pProject == NULL) || (!pProject->IsLoaded()))
			continue;

		COptionTreeCtl * pOptTreeCtl = pProject->GetOptionTreeCtl();
		if (pOptTreeCtl != (COptionTreeCtl *)NULL &&
			pOptTreeCtl->InterestedInPropChange(idChange, this))
			pOptTreeCtl->ProjItemChanged(this, idChange);
	}

	//
	// DO NOT dirty project for dependency file or container.
    // DO NOT dirty project for private build options
	//
    if ( idChange == P_Container || IsPropPublic(idChange) )
	{
		if (
			(!IsKindOf(RUNTIME_CLASS(CDependencyFile))) && 
			(!IsKindOf(RUNTIME_CLASS(CProjectDependency))) &&
			(!IsKindOf(RUNTIME_CLASS(CDependencyContainer)) &&
            idChange != P_DeferredMecr)
		   )
		{
			CProject *pProj = GetProject();
			if( pProj ){
				pProj->DirtyProject();
			}
		}
	}

	// FUTURE: inform the option handler?
	// FUTURE: get the option handler for this prop?
	// what about duplication with GetStrProp()
}

///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::CanDoTopLevelBuild ()
{
	// we can't do a build if :-
	// o we are already doing one

	return g_Spawner.CanSpawn();
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::CheckForDuplicateTargets (CObList &ol, UINT idMess /* = -1 */)
{
	// is this list empty?
	if (ol.IsEmpty())
		return FALSE;	// no duplicates!
		
	CProjItem *pItem, * pItem1;
	POSITION pos;
	CMapPtrToPtr FileMap (ol.GetCount() * 2);
	FileRegHandle frh;
	CString str;
	CString strExtension;

	for (pos = ol.GetHeadPosition (); pos != NULL;)
	{
		pItem = (CProjItem *) ol.GetNext (pos);

		if (!pItem->UsesSourceTool ()) continue;

		// We don't have to check for exlude from build properites--we assume 
		// this was done when the subtree was created.
		ASSERT(pItem->GetActiveConfig());
		CFRFPtrArray * parray = pItem->GetActiveConfig()->GetTargetFilesArray();
		ASSERT(parray);
		int index;
		int count = parray->GetSize();

		for ( index=0; index < count; index++ )
		{
			frh = parray->GetAt(index);
			strExtension =  g_FileRegistry.GetRegEntry(frh)->GetFilePath()->GetExtension();

			// we should just skip .pdb/.icc files since quite a few files
			// will have the same .pdb/.icc files.
			if (strExtension.CompareNoCase(_T(".PDB")) == 0 ||
				strExtension.CompareNoCase(_T(".IDB")) == 0)
				continue;

			if (FileMap.Lookup ( (void *) frh, (void *&) pItem1))
			{
				if (idMess != -1)
				{
					::MsgBox ( 
							Error, 
						   	::MsgText ( 
						   			str, 
						   			idMess,
									(const TCHAR *) *pItem->GetFilePath(),
						   			(const TCHAR *) *pItem1->GetFilePath(),
									(const TCHAR *) *g_FileRegistry.GetRegEntry(frh)->GetFilePath()
 									),
						   	MB_OK
						   );
				}
				return TRUE;
			}
			else  FileMap.SetAt ( frh, pItem );
		}
	}
	return FALSE;		// No dupes.
}

///////////////////////////////////////////////////////////////////////////////
// Log build time information to a remote server.
//
#ifdef _BTLOG
//#pragma message ("ALERT! : projitem.cpp : Including build time logging for QA...")
static void TransportBTInfo(const CString& strElapsedTime, CProject::BuildType bt, 
							BOOL bRebuildAll, CProject* pProject, DWORD dwTargetSize, 
							CString strPlatform)
{
	// Send the Build Time information over the net to the database.
	// Just use a hard-coded file name as this is completely
	// internal.
 	static char szLogFileName[] = "\\\\dtqalogs\\db\\editgo\\editgo.dat";
 	const int maxRetries = 1;
	HANDLE hFile ;

	for (int i = 0; i < maxRetries ; i++ )
	{
		if ( (hFile = CreateFile(szLogFileName,GENERIC_READ|GENERIC_WRITE,
								FILE_SHARE_READ,NULL,OPEN_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,NULL)
			  ) != INVALID_HANDLE_VALUE )
		{
			break;
		}
		Sleep(300); // wait 0.3 seconds if it failed.
	}
	if ( i == maxRetries )
		return ; // No luck couldn't open the file.

	// Start writing from the end of the file.
	SetFilePointer(hFile,0,NULL,FILE_END);

	// Write out the relevant information to the file in the
	// following format.
	// 10/6/93, COLINT3, Debug or Release IDE, Chicago or Daytona,
	// Elapsed build time
 	const DWORD MAX_SIZE = 512;
	char buffer[MAX_SIZE]; // Buffer to hold intermediate values.
	char logString[MAX_SIZE * 2]; // Complete string.
	DWORD  cBytesWritten ;

	logString[0] = '\n';
	_strdate(logString + 1); // get current date in mm/dd/yy form.
	lstrcat(logString,",");

	_strtime(logString + lstrlen(logString));
	lstrcat(logString,",");

	// Computer name.
	GetComputerName(buffer,(LPDWORD)&MAX_SIZE);
	lstrcat(logString,buffer);
	lstrcat(logString,",");

#ifdef _DEBUG
	lstrcat(logString, "Debug IDE,");
#else	
	lstrcat(logString, "Release IDE,");
#endif

	if (NotOnNT())
		lstrcat(logString, "Chicago,");
	else
		lstrcat(logString, "Daytona,");

	if (theApp.m_fOEM_MBCS)
		lstrcat(logString, "Multi-byte OS,");
	else
		lstrcat(logString, "Single-byte OS,");

	// Project name
	CString strProjectName;
	pProject->GetStrProp(P_ProjItemName, strProjectName);
	lstrcat(logString, strProjectName);
	lstrcat(logString, ",");

	// Platform name
	lstrcat(logString, strPlatform);
	lstrcat(logString, ",");

	// Target name
	ASSERT(pProject->GetActiveConfig());
	const ConfigurationRecord* pcr = pProject->GetActiveConfig();
	CString strTargetName = pcr->GetConfigurationName();
	lstrcat(logString, strTargetName);
	lstrcat(logString, ",");

	// Target size
	char szTargetSize[20];
	sprintf(szTargetSize, "%u,", dwTargetSize / 1000);
	lstrcat(logString, szTargetSize);

	// Build type
	switch (bt)
	{
		case CProject::Compile:
			// Should never log compiles, just builds, rebuilds, and
			// batch builds
			ASSERT(FALSE);
			break;

		case CProject::NormalBuild:
			lstrcat(logString, "Normal Build,");
 			break;

		case CProject::BatchBuild:
			lstrcat(logString, "Batch Build,");
			break;
		}

	if (bRebuildAll)
		lstrcat(logString, "Rebuild,");
	else
		lstrcat(logString, "Build,");

 	// elapsed build time
	lstrcat(logString, strElapsedTime);
 
	WriteFile(hFile,logString, lstrlen(logString),&cBytesWritten,NULL);

	// We are all done, close the file.
	CloseHandle(hFile);
}
#endif

///////////////////////////////////////////////////////////////////////////////
// Called for the highest level item to be built. This build only
// single items. Group have there own implementation.
//
BOOL CProjItem::DoTopLevelBuild (UINT buildType,
				CStringList * pConfigs /* = NULL */,
				FlagsChangedAction fca /* = fcaNeverQueried */,
				BOOL bVerbose /* = TRUE */,
				BOOL bClearOutputWindow /* = TRUE */,
				BOOL bRecurse /* = TRUE */
				) 
{ 
#ifdef PROF_BUILD_OVERHEAD
	StartCAPAll();
#endif

	CErrorContext *pEC;
	CProject::BuildResults br;
	CString str, strName;
	BOOL bCancelled = FALSE;
	BOOL bQuery;
	CProject::BuildType bt;
	CPlatform * pPlatform;
	BOOL bDeferMecr = FALSE;
	int iDeferredMecr = 0;

	CPath * pPath;

	DWORD dwStartTime = ::GetTickCount(); 

#ifdef _INSTRAPI
	LogNoteEvent(g_ProjectPerfLog, "devbldd.pkg", "DoTopLevelBuild()", letypeBegin, 0);
#endif

#ifdef _BTLOG
	CPtrList	ConfigPcrs;	// Used to figure out total target size
#endif

	DWORD errs = 0, warns = 0;
	DWORD TotalErrs = 0, TotalWarns = 0;

	if (!theApp.NotifyPackages(PN_QUERY_BEGIN_BUILD) || 
		!g_Spawner.CanSpawn())
	{
#ifdef PROF_BUILD_OVERHEAD
		StopCAPAll();
#endif
		return FALSE;
	}

	CProject * pProject = GetProject(); ASSERT_VALID (pProject);

	// Are we building a macintosh 68k target, if so then we
	// may be attempting to update the remote target.
	// If so do we have a remote executable file name, if not
	// then we should ask for one before we can do the build.
	CProjType *pProjType = GetProjType();
	if( pProjType == NULL ){
		return FALSE;
	}

	pPlatform = pProjType->GetPlatform();

	if ((pPlatform->GetUniqueId() == mac68k ||
         pPlatform->GetUniqueId() == macppc) && 
		 pConfigs == NULL)
	{
		VERIFY(pProject->GetIntProp(P_DeferredMecr, iDeferredMecr));
		if (iDeferredMecr == dabNotDeferred)
		{
			// Make sure we have a remote executable name...
			if (!pProject->GetInitialRemoteTarget())
			{
#ifdef PROF_BUILD_OVERHEAD
				StopCAPAll();
#endif
				return FALSE;
			}
		}
	}

	// No properties can change from here on out.  We can do this before a save 
	// because we know we won't be doing a save as, so the project filename and
	// and directory won't be changing
	ConfigCacheEnabler EnableCaching;				
	ASSERT (pProject->GetFilePath());

	// What type of build are we attempting to do, compile, build, or 
	// batch build?	
	if (IsKindOf(RUNTIME_CLASS(CFileItem)))
		bt = CProject::Compile;
	else if (pConfigs == NULL)
		bt = CProject::NormalBuild;
	else
		bt = CProject::BatchBuild;

	CWaitCursor wc;		// Display hourglass until this object destroyed

	// First flatten the subtree and check whether there is anything
	// to compile or not.
	CObList ol;
	FlattenSubtree (ol, bt == CProject::Compile ? (flt_Normal | flt_ExcludeDependencies | flt_ExcludeGroups) : (flt_Normal|flt_RespectItemExclude | flt_ExcludeDependencies | flt_RespectTargetExclude | flt_ExcludeGroups));

	BOOL bIsExternalTarget = (pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)));
	if (buildType!=TOB_Clean && !bIsExternalTarget && CheckForDuplicateTargets(ol, IDS_SAME_TARGET) && bt!=CProject::BatchBuild)
	{
#ifdef PROF_BUILD_OVERHEAD
		StopCAPAll();
#endif
		return FALSE;
	}

	wc.Restore();

	// ASSERT (pProject == this );
#if 0
	BOOL bFilesToBuild = TRUE; // (bt==CProject::BatchBuild);
	if (!bFilesToBuild)
	{
		POSITION pos = ol.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CProjItem * pItem = (CProjItem *)ol.GetNext(pos);
			if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)) || pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
			{
 				bFilesToBuild = TRUE;
				break;
			}
 		}
	}

	if (!bFilesToBuild  && (pProjType && !pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget))))
	{
		if( !g_bBatchBuildInProgress )
			MsgBox(Information, IDS_NO_SOURCE_FILE_BUILD);

#ifdef PROF_BUILD_OVERHEAD
		StopCAPAll();
#endif
		return TRUE;
	}
#endif
	
	// Do save before build (if appropriate):
	if (g_Spawner.IsSaveBeforeRunningTools(&bQuery))
	{
		if (bIsExternalTarget)
		{
			// save non-project files too for external target
			if (!theApp.SaveAll(bQuery, FILTER_DEBUG | FILTER_PROJECT))
			{
#ifdef PROF_BUILD_OVERHEAD
				StopCAPAll();
#endif
				return FALSE;
			}
		}
		else
		{
			if (!theApp.SaveAll(bQuery, FILTER_DEBUG | FILTER_PROJECT | FILTER_NON_PROJECT | FILTER_NEW_FILE))
			{
#ifdef PROF_BUILD_OVERHEAD
				StopCAPAll();
#endif
				return FALSE;
			}
		}
	}


	// Initialize the spawner and output window:
	ASSERT (!g_Spawner.SpawnActive ());

	if ((pEC = g_Spawner.InitSpawn (bClearOutputWindow)) == NULL )
	{
#ifdef PROF_BUILD_OVERHEAD
		StopCAPAll();
#endif
		return FALSE;
	}
	LogEnabler *pLog = NULL;
	if (buildType != TOB_Clean)
		pLog = new LogEnabler(GetProject());

	theApp.NotifyPackages (PN_BEGIN_BUILD, (void *)bIsExternalTarget);
	
	// Handle the default case - where we just build the active config

	BOOL bConfigAllocated = FALSE;
	if (pConfigs == NULL)
	{
		CString strActiveConfig;

// 		pProject->GetStrProp(P_ProjActiveConfiguration, strActiveConfig);
		ConfigurationRecord *pCfg = pProject->GetActiveConfig();
		ASSERT(pCfg != NULL);
		strActiveConfig = pCfg->GetConfigurationName();

		pConfigs = new CStringList;
		pConfigs->AddTail(strActiveConfig);
		bConfigAllocated = TRUE;
	} else {
		// Fixing a long standing bug in Batch build. This bit of code
		// goes through the configs list and attempts to remove
		// subprojects that are going to be built anyway.
		// Note this is a very slow procedure, but not nearly as slow
		// as actually compiling twice ! (KPerry)
		
/*
		CStringList HitList;
		POSITION pos = pConfigs->GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CString strConfigName = pConfigs->GetNext(pos);
			HBLDTARGET hTarget = g_BldSysIFace.GetTarget(strConfigName, NO_BUILDER);
			if (hTarget == NO_TARGET)
				continue;
			CTargetItem * pCurrentTarget = g_BldSysIFace.CnvHTarget(g_BldSysIFace.GetBuilder(hTarget), hTarget);
	
			POSITION pos2 = pConfigs->GetHeadPosition();
			while (pos2 != (POSITION)NULL)
			{
				CString strConfigName2 = pConfigs->GetNext(pos2);
				HBLDTARGET hTarget2 = g_BldSysIFace.GetTarget(strConfigName2, NO_BUILDER);
				if (hTarget2 == NO_TARGET)
					continue;
				CTargetItem * pTarget = g_BldSysIFace.CnvHTarget(g_BldSysIFace.GetBuilder(hTarget2), hTarget2);
		
				// get a str list of all sub project configurations.
				if ( pCurrentTarget->IsTargetReachable(pTarget) ){
					CString str1,str2;
					CString strTarg, strTarg2;
					// now if the 2 configurations match exactly then don't build this.
					str1 = strConfigName.Right(strConfigName.GetLength() - (strConfigName.Find(" - ")+3) );
					str2 = strConfigName2.Right(strConfigName2.GetLength() - (strConfigName2.Find(" - ")+3) );
					if ( str2 == str1 ){
						HitList.AddHead( strConfigName2 );
					}
				}
			}
		}

		// Now remove the elements. (Note this could be done in the previous
		// loop but it might have been tricky to get right).
		pos = HitList.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			POSITION pos2;
			CString strConfigName = HitList.GetNext(pos);
			pos2 = pConfigs->Find(strConfigName);
			if( pos2 )
				pConfigs->RemoveAt(pos2);
		}
	*/
	}

	while (!pConfigs->IsEmpty() && !bCancelled)
	{
		CString strConfigName;

		strConfigName = pConfigs->GetHead();
		HBLDTARGET hTarget = g_BldSysIFace.GetTarget(strConfigName, NO_BUILDER);
		ASSERT(hTarget);
		if (hTarget == NO_TARGET){
			pConfigs->RemoveHead();
			continue;
		}

		pProject = (CProject *)g_BldSysIFace.GetBuilder(hTarget);
		ASSERT(pProject);
		if (pProject==NULL){
			pConfigs->RemoveHead();
			continue;
		}

		// pProject->SetStrProp(P_ProjActiveConfiguration, strConfigName);
	
		CProjTempProjectChange projectChange(pProject);
		CProjTempConfigChange configChange(pProject);
		configChange.ChangeConfig(strConfigName);

		void *pDummy = NULL;
		if( CBuildIt::m_mapConfigurationsBuilt.Lookup(strConfigName, pDummy) ){
			pConfigs->RemoveHead();
			continue;
		}


		// If we are doing a batch build then we defer mecr by default
		// set on per config base
		if (bt == CProject::BatchBuild)
		{
			VERIFY(pProject->GetIntProp(P_DeferredMecr, iDeferredMecr));
			VERIFY(pProject->SetIntProp(P_DeferredMecr, dabDeferred));
			bDeferMecr = TRUE;
		}

#ifdef _BTLOG
		const ConfigurationRecord* pcr = GetActiveConfig();
		ASSERT(pcr);
		ConfigPcrs.AddTail((void*)pcr);
#endif
		// we basically need to get just the filename
		// of the main output generated by this projitem
		CPath pathTemp ;
		if (IsKindOf (RUNTIME_CLASS(CProject)) )
		{
			pPath = pProject->GetTargetFileName() ;
			if (pPath)
			{
				ASSERT(pPath); ASSERT(pPath->IsInit());
				strName = (TCHAR *) pPath->GetFileName() ;
				delete pPath;
			}
			else
			{	
				// If this is java, just set strName to the project name.  First make
				// sure this is a java target (if it's a batch build and a platform
				// doesn't exist, we can end up in this code for platforms other then
				// Java.

				CString strActiveConfig, strFlavor;

				// logic taken from CBldSysIFace::GetTargetNameFromFileSet
				ASSERT(pProject->GetActiveConfig());
				strActiveConfig = pProject->GetActiveConfig ()->GetConfigurationName ();
				g_BldSysIFace.GetFlavourFromConfigName(strActiveConfig, strFlavor);
				strActiveConfig = strActiveConfig.Left(strActiveConfig.GetLength() - 
					                                   strFlavor.GetLength());
				strName = strActiveConfig;
				int nSep = strActiveConfig.Find(_T(" - "));
				if (nSep > 1)
				{
					strName = strActiveConfig.Left(nSep);
					strActiveConfig = strActiveConfig.Mid(nSep + 3);
					// Review: Could also apply to generic projects
					// ASSERT(strActiveConfig == "Java Virtual Machine ");
				}
			}
		}
		else if (IsKindOf (RUNTIME_CLASS (CFileItem)) &&
			 (GetStrProp (P_ProjMainTarget, strName) == valid))
		{
			pathTemp.Create(strName) ;
			strName = pathTemp.GetFileName() ;
		}
		else 
		{
			GetStrProp (P_ProjItemName, strName) ;
			pathTemp.Create(strName) ;
			strName = pathTemp.GetFileName() ;
		}

		CTempMaker TM;
		if(bt == CProject::BatchBuild){
			br = pProject->DoBuild (pProject, buildType, buildType==TOB_Clean, fca, TM, *pEC, bt, bVerbose, bRecurse);
		} else {
			br = pProject->DoBuild (this, buildType, buildType==TOB_Clean, fca, TM, *pEC, bt, bVerbose, bRecurse);
		}

		
		TM.NukeFiles (*pEC);	// Clean up temporary files.

		g_Spawner.GetErrorCount ( errs, warns );

		errs = errs - TotalErrs;
		warns = warns - TotalWarns;
		// fixup case where an error occured but the spawner couldn't figure that out.
		// REVIEW: the whole counting of errors scheme should be reworked so we don't have to do this.
		int actualErrs = errs ? errs : ( br == CProject::BuildError);
		CBuildIt::m_actualErrs += actualErrs;
		CBuildIt::m_warns += warns;

		// BLOCK: Buzz the user...
		if (buildType != TOB_Clean)
		{
			UINT nBeepType = 0;
			UINT idsBeep;

			switch (br)
			{
			case CProject::BuildCanceled:
				break;

			case CProject::BuildError:
			case CProject::BuildComplete:
				if (actualErrs != 0)
				{
					idsBeep = IDS_SOUND_BUILD_ERROR;
					nBeepType = MB_ICONEXCLAMATION;
				}
				else if (warns != 0)
				{
					idsBeep = IDS_SOUND_BUILD_WARNING;
					nBeepType = MB_ICONQUESTION;
				}
				else
				{
					idsBeep = IDS_SOUND_BUILD_COMPLETE;
					nBeepType = MB_ICONASTERISK;
				}
				break;

			case CProject::NoBuild:
				idsBeep = IDS_SOUND_BUILD_UPTODATE;
				nBeepType = MB_ICONASTERISK;
				break;
			}

			if( buildType != TOB_Clean ) {
				if( PlayDevSound(idsBeep) )
					nBeepType = 0;
	
				if (nBeepType != 0)
					MessageBeep(nBeepType);
			}

			if (bDeferMecr)
			{
				if (pProject->GetCurrentPlatform()->GetUniqueId() == mac68k ||
					pProject->GetCurrentPlatform()->GetUniqueId() == macppc)
				{
					pEC->AddString("");
					pEC->AddString ( IDS_BATCH_BLD_NO_MECR );
				}
			}

			if (br == CProject::NoBuild) 
			{
				pEC->Reset() ;
				pEC->AddString (MsgText(str, IDS_BUILD_UPTODATE, (const char *) strName)) ;
			}
			else
			{
				TotalErrs = TotalErrs + errs;
				TotalWarns = TotalWarns + warns;
				
				pEC->AddString("");

				if (br == CProject::BuildCanceled)
				{
					pEC->AddString ( IDS_USERCANCELED );
					if (pConfigs->GetCount() > 1)
					{
						if ((!g_pAutomationState->DisplayUI()) || (MsgBox(Question, IDS_BUILD_BATCH_CONTINUE, MB_YESNO) == IDNO))
							bCancelled = TRUE;
						else
							g_Spawner.ReInitSpawn();
					}
				}
				else
				{
					pEC->AddString("\n");

					if( g_bHTMLLog ){
						CString strResults;
						strResults.LoadString(IDS_RESULTS);
						LogTrace(strResults);
					}
					pEC->AddString(MsgText(str, IDS_BUILD_COMPLETE, (const char *) strName, actualErrs, warns));
				}
			}
		}

			// If we are doing a batch build then reset the deferred state of mecr
		if (bt == CProject::BatchBuild)
		{
			VERIFY(pProject->SetIntProp(P_DeferredMecr, iDeferredMecr));
		}
		
		if (!bCancelled)
			pConfigs->RemoveHead();
	}

	// Delete the temporary CStringList created if we are just doing
	// a normal build
	if (bConfigAllocated)
	{
		pConfigs->RemoveAll();
		delete pConfigs;
	}


	DWORD dwEndTime = ::GetTickCount();
	DWORD dwElapsedTime = dwEndTime - dwStartTime;

	if ((g_bSpawnTime) && (buildType != TOB_Clean))
	{
		int nMinutes = dwElapsedTime / 60000;
		int nSeconds = (dwElapsedTime % 60000) / 1000;
		int nTenthsOfSecond = (dwElapsedTime % 1000) / 100;

 		CString strElapsedTime;
 		strElapsedTime.Format("Build Time %d:%2.2d.%d", nMinutes, nSeconds, nTenthsOfSecond);
		pEC->AddString(strElapsedTime);
	}

	theApp.NotifyPackages (PN_END_BUILD, (void *)bIsExternalTarget);


	g_Spawner.TermSpawn ();

#ifdef _BTLOG
	// Only log build info for successful builds and not source compiles
	if (bt != CProject::Compile && buildType != TOB_Clean && br == CProject::BuildComplete && g_bBuildLogging)
	{
		DWORD dwTargetSize = 0;
		CString strPlatform;
		
		// Get the total target file size
		POSITION pos = ConfigPcrs.GetHeadPosition();
 		while (pos != NULL && !bCancelled)
		{
 			ConfigurationRecord * pcr = (ConfigurationRecord*) ConfigPcrs.GetNext(pos);

			// Force the target level config. record, ie. the base
			pProject->ForceConfigActive((ConfigurationRecord *)pcr->m_pBaseRecord);

			CString strCurrentPlatform = *(pProject->GetProjType()->GetPlatformName());
			if (strCurrentPlatform.CompareNoCase(strPlatform) != 0)
			{
				if (strPlatform.IsEmpty())
					strPlatform = strCurrentPlatform;
				else
 					strPlatform = "Multi-platform";
			}

			CPath* pPath = pProject->GetTargetFileName();
			ASSERT(NULL!=pPath);

			pProject->ForceConfigActive();

			CString strPath = pPath->GetFullPath();
			HANDLE h = CreateFile(pPath->GetFullPath(), GENERIC_READ, 
								  FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
								  FILE_ATTRIBUTE_NORMAL, NULL);
			if (h != INVALID_HANDLE_VALUE)
			{
				dwTargetSize += GetFileSize(h, NULL);
				CloseHandle(h);
			}
			delete pPath;
		}

		CString strBuildTime; 
	 	strBuildTime.Format("%d", dwElapsedTime);
		TransportBTInfo(strBuildTime, bt, buildType, pProject, dwTargetSize, strPlatform);
	}
#endif

#ifdef _INSTRAPI
	LogNoteEvent(g_ProjectPerfLog, "devbldd.dll", "DoTopLevelBuild()", letypeEnd, 0);
#endif

	delete pLog;

#ifdef PROF_BUILD_OVERHEAD
	StopCAPAll();
#endif

	return br == CProject::BuildComplete || br == CProject::NoBuild;
}

	
CSlob * CProjItem::GetContainerInSameConfig()
{
	CProjItem * pItem = (CProjItem *)GetContainer();
	if (pItem != (CSlob *)NULL)
	{
		// if we have a manual than our containe uses the same manual,
		// else it uses our current if it needs to

		ConfigurationRecord * pRec = (ConfigurationRecord *)NULL;

		// use the same manual config. if we have one
		if (m_pManualConfigRec != (ConfigurationRecord *)NULL)
			pRec = m_pManualConfigRec;

		// don't do anything if we don't need to
		else
		{
			ASSERT(GetActiveConfig());
			// ASSERT(pItem->GetActiveConfig());
			if (pItem->GetActiveConfig(TRUE)->m_pBaseRecord != GetActiveConfig()->m_pBaseRecord)
				pRec = m_pActiveConfig;
		}

		pItem->SetManualBagSearchConfig(pRec);
	}
	return pItem;
}

void CProjItem::SetCurrentConfigAsBase(COptionHandler * popthdlr)
{
	ConfigurationRecord * pRec = GetActiveConfig();
	ASSERT(pRec != (ConfigurationRecord *)NULL);

	while (popthdlr != (COptionHandler *)NULL)
	{
		UINT nMinProp, nMaxProp ;
		popthdlr->GetMinMaxOptProps (nMinProp, nMaxProp) ;
		pRec->BagCopy(CurrBag, BaseBag, nMinProp, nMaxProp);
		// get the base-handler
		popthdlr = popthdlr->GetBaseOptionHandler();
	}
}

void CProjItem::SetBaseConfigAsCurrent(COptionHandler * popthdlr)
{
	ConfigurationRecord * pRec = GetActiveConfig();
	ASSERT(pRec != (ConfigurationRecord *)NULL);

	while (popthdlr != (COptionHandler *)NULL)
	{
		UINT nMinProp, nMaxProp ;
		popthdlr->GetMinMaxOptProps (nMinProp, nMaxProp) ;
		pRec->BagCopy(BaseBag, CurrBag, nMinProp, nMaxProp);
		// get the base-handler
		popthdlr = popthdlr->GetBaseOptionHandler();
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetPropBag
////////////////////////////////////////////////////////////////////////////////
int CProjItem::GetPropBagCount() const
{
	const CPtrArray * pCfgArray = GetConfigArray();
	return pCfgArray->GetSize();
}

CPropBag* CProjItem::GetPropBag(int nBag)
{
	// look for the *current* property bag?
	if (nBag == -1) /* get current */
	{
		// ASSERT(GetActiveConfig());
		return ((ConfigurationRecord *)GetActiveConfig(TRUE))->GetPropBag(m_idBagInUse);	// our prop bag for current active config
	}

	// get a particular property bag (using index)
	ConfigurationRecord *pcr;
	ASSERT(nBag >= 0);
	if (nBag > m_ConfigArray.GetUpperBound())
		return (CPropBag *)NULL ;

	pcr = (ConfigurationRecord *)m_ConfigArray[nBag];
	return pcr->GetPropBag(m_idBagInUse);
}

CPropBag * CProjItem::GetPropBag(const CString & strBagName)
{
	ConfigurationRecord * pcr = ConfigRecordFromConfigName(strBagName); 
	return pcr != (ConfigurationRecord *)NULL ? pcr->GetPropBag(m_idBagInUse) : (CPropBag *)NULL;
};
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::ConfigurationChanged(const ConfigurationRecord *pcr )
{
	POSITION pos;
#ifdef _DEBUG
	void * pv;
#endif
	BOOL retVal = FALSE; // return true if and only if "new" cr added
	CProjItem *	pProjItem;
	ConfigurationRecord * pcrNew;
	CString strType, strName;

	PCFG_TRACE ("%s at %p notfied of config change to base at %p (name: %s)\n",
	  GetRuntimeClass()->m_lpszClassName, this, pcr, ((CStringProp *) 
	  ((ConfigurationRecord *)pcr->m_pBaseRecord)->GetPropBag(CurrBag)->FindProp(P_ProjConfiguration))->m_strVal);

	ASSERT (pcr);
	ASSERT (pcr->m_pBaseRecord == pcr ); // pcr should be top level!

	// FUTURE (karlsi): Change subproject and warn if there's state if they don't
	// have a matching config.

	// This is where items other than projects find out that there's a new
	// config:  They're told to switch to a config they don't know about,
	// so they create it:
	if (!m_ConfigMap.Lookup((void *) pcr, (void *&) m_pActiveConfig))
	{
		strType = pcr->GetOriginalTypeName();
		strName = pcr->GetConfigurationName();
		pcrNew = ConfigRecordFromConfigName(strName, FALSE, FALSE);
		if ((pcrNew) && (pcrNew->GetOriginalTypeName()==strType))
		{
			// matched by name & projtype, so reuse this one
			m_pActiveConfig = pcrNew;
			VERIFY(m_ConfigMap.RemoveKey((void*)m_pActiveConfig->m_pBaseRecord)); // readded for new base below
			m_pActiveConfig->m_pBaseRecord = pcr->m_pBaseRecord;
		}
		else
		{
			m_pActiveConfig = new ConfigurationRecord (pcr, this);
			int x = m_ConfigArray.Add(m_pActiveConfig);
			ASSERT(((ConfigurationRecord *)m_ConfigArray[x])->GetConfigurationName()==pcr->GetConfigurationName());
			retVal = TRUE;
		}
		ASSERT(!m_ConfigMap.Lookup((void *)m_pActiveConfig->m_pBaseRecord, (void *&)pv));
		m_ConfigMap.SetAt((void *)m_pActiveConfig->m_pBaseRecord, m_pActiveConfig);
		ASSERT(m_ConfigMap.GetCount()==m_ConfigArray.GetSize());
	}
	ASSERT_VALID (m_pActiveConfig);

	// notify all of our children of this change
	// don't do this if we don't have a content list
	if (
		!GetContentList()
	   )
		return retVal;

	for (pos = GetHeadPosition(); pos != NULL; )
	{
		pProjItem = (CProjItem *)GetNext(pos);
		BOOL bChildAdded = pProjItem->ConfigurationChanged(pcr);
		if (retVal)	// FUTURE (karlsi): fix targdlgs so this ASSERT is always valid
		{	// if adding a new config record, should also be for children
			ASSERT(retVal==bChildAdded);
		}
	}
	return retVal;

}
///////////////////////////////////////////////////////////////////////////////
// make sure we have all of our configs matching the project
void CProjItem::CreateAllConfigRecords()
{
	CTargetItem *pTarget = GetTarget();
	CProject *pProject = GetProject();
	const CPtrArray * pCfgArray;
	BOOL fUseProjectConfig = FALSE;

	if (pTarget != NULL && pTarget != this && pProject != this)
	{
		pCfgArray = pTarget->GetConfigArray();
	}
	else if (pProject != NULL)
	{
		pCfgArray = pProject->GetConfigArray();
		fUseProjectConfig = TRUE;
	}

	if (pCfgArray != NULL)
	{
		int icfg, size = pCfgArray->GetSize();
		for (icfg = 0; icfg < size; icfg++)
		{
			ConfigurationRecord * pcr = (ConfigurationRecord *)pCfgArray->GetAt(icfg);
			if (fUseProjectConfig || pcr->IsValid())
				(void)ConfigRecordFromBaseConfig((ConfigurationRecord *)pcr->m_pBaseRecord, TRUE);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
ConfigurationRecord * CProjItem::GetActiveConfig(BOOL bCreate /* = FALSE */)
{
	// return a forced configuration?
	if (m_pForcedConfig != (ConfigurationRecord *) NULL)
		return m_pForcedConfig;

	if (m_pManualConfigRec)
	{
		ConfigurationRecord * pRec;
		if (m_ConfigMap.Lookup( 
				(void  *) m_pManualConfigRec->m_pBaseRecord,
				(void *&) pRec
				))
			return pRec;
		
		// ASSERT(0); should never get here, but if so, ignore m_pManualConfigRec
	}

	// do we have a matching config. with our owner project?
	// (compare base records)
	CProjItem * pItem = GetProject();
	
	// Is this item in a project, if not then it doesn't have
	// an active config
	if (pItem == NULL)
		return NULL;

	if (pItem != this)
	{
		ConfigurationRecord * pcrItem = pItem->GetActiveConfig(bCreate);
		if (!m_pActiveConfig || pcrItem != m_pActiveConfig->m_pBaseRecord)
		{
			if (pcrItem != NULL)
			{
				// get a config. record, create if needs be
				m_pActiveConfig = ConfigRecordFromBaseConfig(pcrItem, bCreate);
				if (bCreate)
				{
					ASSERT(m_pActiveConfig);
					ASSERT_VALID(m_pActiveConfig);
				}
			} 
			else
			{
				m_pActiveConfig = NULL;
			}
		}
	}

	// return the active configuration...
	return m_pActiveConfig;
}
///////////////////////////////////////////////////////////////////////////////
ConfigurationRecord* CProjItem::ConfigRecordFromBaseConfig
(
	ConfigurationRecord *	pcrBase,
	BOOL					fCreate /* = FALSE */
)
{
	ConfigurationRecord * pcr;

	// not found matching?
	if (!m_ConfigMap.Lookup((void *)pcrBase, (void *&)pcr))
	{
		// create?
		if (fCreate)
		{
			CString strType = pcrBase->GetOriginalTypeName();
			CString strName = pcrBase->GetConfigurationName();

			pcr = ConfigRecordFromConfigName(strName);
			if (pcr && pcr->GetOriginalTypeName() == strType)
			{
				// matched by name & projtype, so reuse this one
				VERIFY(m_ConfigMap.RemoveKey((void*)pcr->m_pBaseRecord)); // readded for new base below
				pcr->m_pBaseRecord = pcrBase;
			}
			else
			{

				// Make sure we are not creating a ConfigurationRecord on 
				// these objects which does not belong in the target.
				// Currently there are still cases where we hit this assert,
				// so it is not turned on by default.
				// You can turn it on if you need to check for bogus configs. 
#if 0 
				if (IsKindOf(RUNTIME_CLASS(CFileItem)) ||
					IsKindOf(RUNTIME_CLASS(CDependencyFile)) ||
					IsKindOf(RUNTIME_CLASS(CDependencyContainer)))
				{
					CTargetItem *pTarget = GetTarget();
					ASSERT(pTarget);
					const CPtrArray * pArrayCfg = pTarget->GetConfigArray();
					int j ;
					ConfigurationRecord * pcr ;

					for ( j = 0 ; j < pArrayCfg->GetSize() ; j++)
					{
						pcr = (ConfigurationRecord *)pArrayCfg->GetAt(j);
						if (pcrBase->m_pBaseRecord == pcr->m_pBaseRecord)
							break;
					}

					if ( j == pArrayCfg->GetSize() || !pcr->IsValid())
						ASSERT(FALSE);
				}
#endif // 0
					
				// not  matched, so create a new one
				pcr = new ConfigurationRecord(pcrBase, this);
				int ipcr = m_ConfigArray.Add(pcr);
				ASSERT(((ConfigurationRecord *)m_ConfigArray[ipcr])->GetConfigurationName() == pcrBase->GetConfigurationName());
			}

			// add this into our config. map (it's already in our array)
#ifdef _DEBUG
			void * pv;
#endif // _DEBUG
			ASSERT(!m_ConfigMap.Lookup((void *)pcr->m_pBaseRecord, (void *&)pv));
			m_ConfigMap.SetAt((void *)pcr->m_pBaseRecord, pcr);

			ASSERT(m_ConfigMap.GetCount() == m_ConfigArray.GetSize());
		}
		else
			pcr = (ConfigurationRecord *)NULL;
	}

	return pcr;
}
///////////////////////////////////////////////////////////////////////////////
ConfigurationRecord *CProjItem::ConfigRecordFromConfigName
(
	const TCHAR *	pszConfig,
	BOOL			fCreate /* = FALSE */,
	BOOL fMatchExact /* = TRUE */
)
{
	CString strConfigToMatch, strConfig;
	TCHAR * pszChar;
	ConfigurationRecord * pcrMatch = NULL;
	if (!fMatchExact)
	{
		TCHAR * pszChar = _tcsstr(pszConfig, _T(" - "));
		if (pszChar != NULL)
		{
			strConfigToMatch = pszChar + 3;
		}
		else
		{
			fMatchExact = TRUE;
		}
	}
	int size = GetPropBagCount();
	for (int i = 0; i < size; i++)
	{
		ConfigurationRecord * pcr = (ConfigurationRecord *)m_ConfigArray[i];
		strConfig = pcr->GetConfigurationName();
		if (strConfig.CompareNoCase(pszConfig) == 0)
			return pcr;

		if ((!fMatchExact) && (pcrMatch == NULL))
		{
			pszChar = _tcsstr(strConfig, _T(" - "));
			if ((pszChar != NULL) && (strConfigToMatch.CompareNoCase(pszChar + 3) == 0))
			{
				// found possible match
				pcrMatch = pcr;
			}
		}
	}

	if (pcrMatch != NULL)
		return pcrMatch;

	// not found, so create?
	if (fCreate)
	{
		// must have one in project, so create one based on this project's config.
		return ConfigRecordFromBaseConfig(GetProject()->ConfigRecordFromConfigName(pszConfig), TRUE);
	}

	return NULL;
}

CProjType * CProjItem::GetProjType ()
{
	CProject * pProject = GetProject();
	ASSERT(pProject != (CProject *)NULL);

	if (pProject->m_bProjIsExe)
		return (CProjType *)NULL;
	
	ConfigurationRecord * pcr = GetActiveConfig();
	if (pcr == (ConfigurationRecord *)NULL)
		return (CProjType *)NULL;	// no current configuration -> no project type!

	ASSERT(pcr->m_pBaseRecord);

	CStringProp * pProp = (CStringProp *) ((ConfigurationRecord *)pcr->m_pBaseRecord)
						 ->GetPropBag(CurrBag)->FindProp (P_ProjOriginalType);
	if (pProp==NULL)
	{
		return NULL;
		// ASSERT(0);
	}

	ASSERT(pProp->m_nType == string); 

	// ignore return
	CProjType * pprojtype;
	if (!g_prjcompmgr.LookupProjTypeByName(pProp->m_strVal, pprojtype))
		return (CProjType *)NULL;

	return pprojtype;
}
///////////////////////////////////////////////////////////////////////////////
void CProjItem::FlattenSubtree (CObList &rObList, int fo) 
{
 	BOOL fAddItem = FALSE, fAddContent = FALSE;

	FlattenQuery(fo, fAddContent, fAddItem);

	// If we have items contained then flatten each of these as well
	if (fAddContent && GetContentList())
	{
		POSITION pos;
 		for (pos = GetHeadPosition(); pos != (POSITION)NULL;)
		{
			CProjItem * pProjItem = (CProjItem *) GetNext (pos);
			if (!(fo & flt_ExpandSubprojects) && pProjItem->IsKindOf(RUNTIME_CLASS(CProject)))
				continue;

			pProjItem->FlattenSubtree(rObList, fo);
		}
	}

	if ((fo & flt_OnlyTargRefs) && !IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		return;	// only include target references!
					
	// Add the item if we need to
	if (fAddItem)
		rObList.AddTail(this);
}

void CProjItem::FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem)
{
	fAddContent = FALSE;
	fAddItem = FALSE;
}

///////////////////////////////////////////////////////////////////////////////
CFileRegistry* CProjItem::GetRegistry()
{
	// we'll use our target's registry,
	// and if we don't have one (ie. external makefiles)
	// we'll use the global registry
	CTargetItem * pTarget = GetTarget();
	if (pTarget == NULL)
		return &g_FileRegistry;
	else
		return pTarget->GetRegistry();
}
///////////////////////////////////////////////////////////////////////////////
// CanContain - our internal (within VPROJ) CanAdd method
 
BOOL CProjItem::CanContain(CSlob* pSlob)
{
	// Must override
	ASSERT(FALSE);
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
GPT CProjItem::GetIntProp(UINT idProp, int& val)
{
//#pragma message ("ALERT! : projitem.cpp : Including P_QA_ProjItemType hook for QA...")
	// return to QA what the type of this project item is
	if (idProp == P_QA_ProjItemType)
	{
		if (IsKindOf(RUNTIME_CLASS(CProject)))
			val = QA_TypeIs_Target;
		else if (IsKindOf(RUNTIME_CLASS(CProjGroup)))
			val = QA_TypeIs_Group;
		else if (IsKindOf(RUNTIME_CLASS(CFileItem)))
			val = QA_TypeIs_File;
		else if (IsKindOf(RUNTIME_CLASS(CDependencyContainer)))
			val = QA_TypeIs_DepGroup;
		else if (IsKindOf(RUNTIME_CLASS(CDependencyFile)))
			val = QA_TypeIs_DepFile;
		else ASSERT(FALSE);	// unrecognised type!

		return valid;	// success
	}

	GPT gpt;
	// want to handle special case here:
	if (idProp == P_ItemOptState || idProp == P_ItemOptChanged)
	{
		if (IsKindOf (RUNTIME_CLASS (CProject)) || IsKindOf (RUNTIME_CLASS (CFileItem)) || 
			IsKindOf (RUNTIME_CLASS (CProjGroup)) || IsKindOf (RUNTIME_CLASS (CTargetItem)))
		{
			gpt = CSlob::GetIntProp (P_ItemOptState, val);
			if (gpt == invalid)
				val = 1;
			else 
			{
				if (idProp == P_ItemOptState)
					val = val & 0x7fffffff; // masking off the highest bit
				else
					val = (val & 0x80000000) ? 1 : 0; // get the highest bit
			}
			return valid;
		}
		else
			return invalid;
	}

	if ((idProp == P_ItemExcludedFromScan) || (idProp == P_IPkgProject))
	{
		// not per config and not valid by default
		// override to do anything special
		val = FALSE;
		return invalid;
	}

	if ((idProp == P_Proj_PerConfigDeps) && (GetProject() != NULL))
	{
		val = GetProject()->m_bProj_PerConfigDeps;
		return valid;
	}

#if 0
	// fake prop that is actually maintained by the Scc Manager
	if (idProp == P_SccStatus)
	{
		const CPath * pPath = GetFilePath();
		ASSERT(pPath);
		LPSOURCECONTROL pSccManager;
		VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControl, (LPVOID FAR*)&pSccManager)));
		g_pSccManager->GetStatus(&val, (LPCTSTR)*pPath);	
		pSccManager->Release();
		return (val!=-1) ? valid : invalid;
	}
#endif

	// get the option handler
	COptionHandler * popthdlr = g_prjcompmgr.FindOptionHandler(idProp);

	if (popthdlr != (COptionHandler *)NULL)
	{
		// if we have an option handler for this prop we must be an option
		// check it's dependencies
		if (!(m_optbeh & OBNoDepCheck))
		{
			BOOL fCheckDepOK;

			// the DepCheck needs to view the whole 'context' ie.
			// the tool options with inheritance
			// prevent the DepCheck() recursing
			OptBehaviour optbehOld = m_optbeh;
			m_optbeh |= OBInherit | OBShowDefault | OBNoDepCheck;
		
			// make sure the option handling refers to us!
			popthdlr->SetSlob(this);
			fCheckDepOK = popthdlr->CheckDepOK(idProp);
			popthdlr->ResetSlob();

			m_optbeh = optbehOld;

			// is this an invalid option?
			if (!fCheckDepOK)	return invalid;
		}
	}

	// usual CSlob property behaviour
	// ask the base-class to actual find the prop.
	gpt = CSlob::GetIntProp(idProp, val);

	// do we want to supply this ourselves?
	if (gpt == invalid && (idProp == P_ItemExcludedFromBuild))
	{
		val = FALSE; gpt = valid;	// valid and FALSE by default
	}

	// does this property exist for this CSlob?
	if (gpt == invalid)
	{
		// are we not at the root and inheriting?
		if (m_optbeh & OBInherit)
		{
			// automatically detect if we need to do a manual config?
			// we don't need to if we already have a manual config rec. to use
			CProjItem * pContainer = (CProjItem *) GetContainerInSameConfig();
			while (pContainer != (CSlob *)NULL)
			{
				// yes, so let's search in our container CSlob if we have one
				OptBehaviour optbehOld = pContainer->m_optbeh;
 				int idOldBag = pContainer->UsePropertyBag(UsePropertyBag());

				// make them use our option behaviour, but return any context sensitive
				// information such as defaults .. we'll do the inheritance chaining here!
				pContainer->m_optbeh = (m_optbeh | OBBadContext | OBNoDepCheck) & ~OBInherit;

				GPT gptContainer = pContainer->GetIntProp(idProp, val);

				pContainer->m_optbeh = optbehOld;
 	   			(void) pContainer->UsePropertyBag(idOldBag);

				if (gptContainer == valid)
				{
					pContainer->ResetContainerConfig();
					return valid;
				}

				// chain our config.
				// look in the next container
				CProjItem * pNextContainer = (CProjItem *) pContainer->GetContainerInSameConfig();
				pContainer->ResetContainerConfig();
				pContainer = pNextContainer;
			}
		}

		//
		// *** base-case ***
		//
		// right context?
		if (!(m_optbeh & OBBadContext))
		{
			if (popthdlr != (COptionHandler *)NULL)
			{
				// make sure the option handling refers to us!
				popthdlr->SetSlob(this);

				// we need to find this in our default property map
				// ask our helper which map it is in

				// always show fake props, if inheriting
				if (((m_optbeh & OBInherit) && popthdlr->IsFakeProp(idProp)) ||
					(m_optbeh & OBShowDefault))
				{
					OptBehaviour optbehOld = m_optbeh;

					m_optbeh |= OBShowDefault;
					gpt = popthdlr->GetDefIntProp(idProp, val);
					m_optbeh = optbehOld;
				}
				
				// reset the option handler CSlob
				popthdlr->ResetSlob();
			}
		}
	}

 	// mustn't be OBAnti, the option handler takes care of this
	ASSERT((m_optbeh & OBAnti) == 0);

	return gpt;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::SetIntProp(UINT idProp, int val)
{
	BOOL fOk;

	if ((idProp == P_Proj_PerConfigDeps) && (GetProject() != NULL))
	{
		GetProject()->m_bProj_PerConfigDeps = val;
	}

#if 0
	// fake prop that is actually maintained by the Scc Manager
	if (idProp == P_SccStatus)
	{
		const CPath * pPath = GetFilePath();
		ASSERT(pPath);
		LPSOURCECONTROL pSccManager;
		VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControl, (LPVOID FAR*)&pSccManager)));
		VERIFY(SUCCEEDED(pSccManager->SetStatus((LPCTSTR)*pPath, val)));	
		pSccManager->Release();
		return valid;
	}
#endif

	// mustn't be OBAnti, the option handler takes care of this for integer props
	ASSERT((m_optbeh & OBAnti) == 0);

	// need to do special handling here
	int valTemp = 0;
	GPT gpt;
	if (idProp == P_ItemOptState)
	{
		gpt = GetIntProp (P_ItemOptChanged, valTemp);
		if (gpt == valid)
		{
			val = valTemp ? 0x80000000 | val : val; // set the high bit if valTemp is set
			fOk = CSlob::SetIntProp(P_ItemOptState, val);
		}
		else
		{
			ASSERT (FALSE);
			return FALSE;
		}
	}
	else if (idProp == P_ItemOptChanged)
	{
		gpt = GetIntProp (P_ItemOptState, valTemp);
		if (gpt == valid)
		{
			val = val ? 0x80000000 | valTemp : valTemp;
			fOk = CSlob::SetIntProp(P_ItemOptState, val);
		}
		else
		{
			ASSERT (FALSE);
			return FALSE;
		}
	}
	else
	{
		fOk = CSlob::SetIntProp(idProp, val);
	}

	if (fOk && idProp == P_ItemExcludedFromBuild)
	{
		// FUTURE: Do THIS?
		// Finally, we want to mark the item's project to suggest
		// a relink.  Do this by specifying the last build flags
		// string to be empty:
		CProject * pProject = GetProject();
		ASSERT(pProject != (CProject *)NULL);

		// broadcast to our option handlers
		COptionHandler * popthdlr;
		g_prjcompmgr.InitOptHdlrEnum();
		while (g_prjcompmgr.NextOptHdlr(popthdlr))
		{
			// inform of this option change
			// make sure the option handling refers to us!
			popthdlr->SetSlob(this);
			popthdlr->OnOptionIntPropChange(P_ItemExcludedFromBuild, val);

			// reset the option handler CSlob
			popthdlr->ResetSlob();
		}
	}

	// get the option handler for the prop
	COptionHandler * popthdlr;
	if (fOk && ((popthdlr = g_prjcompmgr.FindOptionHandler(idProp)) != (COptionHandler *)NULL))
	{
		// inform of this option change
		// make sure the option handling refers to us!
		popthdlr->SetSlob(this);
		popthdlr->OnOptionIntPropChange(idProp, val);

		// reset the option handler CSlob
		popthdlr->ResetSlob();
	}

	return fOk;
}
///////////////////////////////////////////////////////////////////////////////
BOOL CProjItem::SetStrProp(UINT idProp, const CString & str)
{
	// get the option handler for this prop
	COptionHandler * popthdlr = g_prjcompmgr.FindOptionHandler(idProp);

	BOOL fIsListStrProp = (popthdlr != (COptionHandler *)NULL) && popthdlr->IsListStrProp(idProp);

	// only string lists and unk. opt. hdlr strings are available as anti-options
	ASSERT ((m_optbeh & OBAnti) == 0 || fIsListStrProp || popthdlr->IsKindOf(RUNTIME_CLASS(COptHdlrUnknown)));

	if (popthdlr != (COptionHandler *)NULL)
	{
		// do we to do a type conversion ourselves?
		// ie. does the CSlob *not* support this tool option type?
		if (popthdlr->NeedsSubtypeConversion(idProp))
		{	
			// make sure the option handling refers to us!
			popthdlr->SetSlob(this);

			int nVal;

			BOOL fRet;

			// is this the empty string? if so we want
			// to nuke it from our property bag
			if (str.IsEmpty())
			{
				CPropBag * pPropBag = GetPropBag();
				ASSERT(pPropBag != (CPropBag *)NULL);

				pPropBag->RemoveProp(idProp);
				fRet = TRUE;
			}
			else if (!popthdlr->ConvertFromStr(idProp, str, nVal))
			{
				// no change, inform dependants of this
				InformDependants(idProp);
				fRet = FALSE;
			}
			else
				fRet = SetIntProp(idProp, nVal);

			// reset option handler CSlob
			popthdlr->ResetSlob();
			
			return fRet;
		}
 	}

	// setting the output directories?
	if (idProp == P_OutDirs_Target || idProp == P_OutDirs_Intermediate)
	{
		// if we're setting the output dirs props to an empty string, remove
		// the prop if it exists, only if we're a child that way the removal
		// will reset the prop to inherit from the parent
		if (m_pContainer != (CSlob *)NULL && str.IsEmpty())
		{
			GetPropBag()->RemovePropAndInform(idProp, this);
			return TRUE; 
		}
	}

	BOOL fOk = FALSE;

	// do we have a list of strings for this property?
	if (fIsListStrProp)
	{
		// figure add and subtract string props
		popthdlr->SetListStrProp(this, idProp, (CString &)str, m_optbeh & OBInherit, m_optbeh & OBAnti);
		fOk = TRUE;
	}
	else
	{
		fOk = CSlob::SetStrProp(idProp, str);
	}

	// inform of this option change
	if (fOk && popthdlr != (COptionHandler *)NULL)
	{
		// make sure the option handling refers to us!
		popthdlr->SetSlob(this);
		popthdlr->OnOptionStrPropChange(idProp, str);

		// reset option handler CSlob
		popthdlr->ResetSlob();
	}

	return fOk;
}
///////////////////////////////////////////////////////////////////////////////
GPT CProjItem::GetStrProp(UINT idProp, CString& val)
{
	// fake prop that is actually maintained by the Scc Manager
	if (idProp == P_SccStatus)
	{
		// translate to string
		LPSOURCECONTROLSTATUS pInterface;
		VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControlStatus, (LPVOID FAR*)&pInterface)));
		BOOL bIsSccInstalled = (pInterface->IsSccInstalled() == S_OK);
		pInterface->Release();
		LPSOURCECONTROL pSccManager;
		VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControl, (LPVOID FAR*)&pSccManager)));
		GPT gpt = (bIsSccInstalled && (GetFilePath()!=NULL) && (SUCCEEDED(pSccManager->GetStatusText(*GetFilePath(), val)))) ? valid : invalid;
		pSccManager->Release();
		return gpt;
	}

	if (idProp == P_Title) // alias
		return GetStrProp(P_ProjItemName, val);

	// get the option handler for this prop
	COptionHandler * popthdlr = g_prjcompmgr.FindOptionHandler(idProp);

	// are we a string list prop?
	BOOL fIsListStrProp = (popthdlr != (COptionHandler *)NULL) && popthdlr->IsListStrProp(idProp);

	// only string lists and unk. opt. hdlr strings are available as anti-options
	ASSERT ((m_optbeh & OBAnti) == 0 || fIsListStrProp || popthdlr->IsKindOf(RUNTIME_CLASS(COptHdlrUnknown)));
	
	if (popthdlr != (COptionHandler *)NULL)
	{
		// do we to do a type conversion ourselves?
		// ie. does the CSlob *not* support this tool option type?
		if (popthdlr->NeedsSubtypeConversion(idProp))
		{	
			// make sure the option handling refers to us!
			popthdlr->SetSlob(this);
			GPT gptRet;

			// is this a valid prop?
			int nVal;
			if (GetIntProp(idProp, nVal) != valid)
			{
				gptRet = invalid;	
			}
			else
			{
				// is this a default, ie. not in any bag?
				OptBehaviour optbehOld = m_optbeh;
				m_optbeh &= ~OBShowDefault;
				GPT gpt = GetIntProp(idProp, nVal);
				m_optbeh = optbehOld;

				if (gpt != valid)
				{
					val = "";	// for defaults return the empty string
					gptRet = valid;
				}
				else
					gptRet = popthdlr->ConvertToStr(idProp, nVal, val) ? valid : invalid;		
			}

			// reset the option handler CSlob
			popthdlr->ResetSlob();

			return gptRet;
		}

	 	// if we have a default option map this must be an option
		// check it's dependencies
		if (!(m_optbeh & OBNoDepCheck))
		{
			BOOL fCheckDepOK;

			// the DepCheck needs to view the whole 'context' ie.
			// the tool options with inheritance
			// prevent the DepCheck() recursing
			OptBehaviour optbehOld = m_optbeh;
			m_optbeh |= OBInherit | OBShowDefault | OBNoDepCheck;

			// make sure the option handling refers to us!
			popthdlr->SetSlob(this);
			fCheckDepOK = popthdlr->CheckDepOK(idProp);
			popthdlr->ResetSlob();

			m_optbeh = optbehOld;

			// is this an invalid option?
			if (!fCheckDepOK)	return invalid;
		}
	}

  	GPT gpt = invalid;	// default is the prop. find is invalid

	// do we have a list of strings for this property?
	// if so then we'll want to show this list
	if (fIsListStrProp && !m_fGettingListStr)
	{
		SUBTYPE st = popthdlr->GetDefOptionSubType(idProp);

		// join our directory string lists with ';'
		TCHAR chJoin = st == dirST ? _T(';') : _T(',');

		// show add and/or subtract props combined for this
		m_fGettingListStr = TRUE;
		gpt = popthdlr->GetListStrProp(this, idProp, val, m_optbeh & OBInherit, m_optbeh & OBAnti, chJoin) ? valid : invalid;
		m_fGettingListStr = FALSE;
	}
	else
	{
		// usual CSlob property behaviour
		// ask the base-class to actual find the prop.
		gpt = CSlob::GetStrProp(idProp, val);
	}

	// does this property exist for this CSlob?
	if (gpt == invalid)
	{
		// fake prop for custom build description .. <- no inheritance!
		if ((m_optbeh & OBShowDefault) && idProp == P_CustomDescription)
		{
			VERIFY(val.LoadString(IDS_CUSTOMBUILD_DESC));
			return valid;
		}

		// are we not at the root and inheriting?
		if (m_optbeh & OBInherit)
		{
			// automatically detect if we need to do a manual config?
			// we don't need to if we already have a manual config rec. to use
			CProjItem * pContainer = (CProjItem *) GetContainerInSameConfig();
			while (pContainer != (CSlob *)NULL)
			{
				// yes, so let's search in our container CSlob if we have one
				OptBehaviour optbehOld = pContainer->m_optbeh;
				int idOldBag = pContainer->UsePropertyBag(UsePropertyBag());

				// make them use our option behaviour, but return any context sensitive
				// information such as defaults .. we'll do the inheritance chaining here!
				pContainer->m_optbeh = (m_optbeh | OBBadContext | OBNoDepCheck) & ~OBInherit;

				GPT gptContainer = pContainer->GetStrProp(idProp, val);

				pContainer->m_optbeh = optbehOld;
   				(void) pContainer->UsePropertyBag(idOldBag);

				if (gptContainer == valid)
				{
					pContainer->ResetContainerConfig();
					return valid;
				}

				// chain our config.
				// look in the next container
				CProjItem * pNextContainer = (CProjItem *) pContainer->GetContainerInSameConfig();
				pContainer->ResetContainerConfig();
				pContainer = pNextContainer;
			}
		}

		//
		// *** base-case ***
		//
		// right context?
		if (!(m_optbeh & OBBadContext))
		{
			if (popthdlr != (COptionHandler *)NULL)
			{
				// make sure the option handling refers to us!
				popthdlr->SetSlob(this);

				// we need to find this in our default property map
				// ask our helper which map it is in

				// always show fake props, if inheriting
				if (((m_optbeh & OBInherit) && popthdlr->IsFakeProp(idProp)) ||
					(m_optbeh & OBShowDefault))
				{
					OptBehaviour optbehOld = m_optbeh;

					m_optbeh |= OBShowDefault;
					gpt = popthdlr->GetDefStrProp(idProp, val);
					m_optbeh = optbehOld;
				}

				// reset the option handler CSlob
				popthdlr->ResetSlob();
			}
			// show the defaults for non-tool option props?
			else if (m_optbeh & OBShowDefault)
			{
				// only have target directory for project
				// don't have intermediate directories for groups
				if (idProp == P_OutDirs_Target || idProp == P_OutDirs_Intermediate)
				{
					val = ""; // default is nothing for the int/trg directories
					gpt = valid;
				}
			}
		}
	}

	return gpt;
}

void CProjItem::SetOwnerProject(CProject * pProject, CTargetItem * pTarget)
{
	m_pOwnerProject = pProject;
	m_pOwnerTarget = pTarget;
	
	// repeat for all children*
	if (GetContentList())
	{
		POSITION pos;
	 	for (pos=GetHeadPosition(); pos!=NULL;)
	  	{
			// Any contained slobs should also be items:
			((CProjItem *)GetNext(pos))->SetOwnerProject(pProject, pTarget);
		}
	}
}

void CProjItem::IsolateFromProject()
{
	// update each config record
	UINT nIndex, nSize = GetPropBagCount();

	// copying into a non-projitem, so remove any links to Project
	for (nIndex = 0; nIndex < nSize; nIndex++)
	{
		// copy important pieces from project base record
		ConfigurationRecord * pcr = (ConfigurationRecord *)m_ConfigArray[nIndex];
		ForceConfigActive(pcr);

		theUndoSlob.Pause();	// don't record SetStrProp within SetSlopProp
		SetStrProp(P_ProjConfiguration,	pcr->m_pBaseRecord->GetConfigurationName());
		SetStrProp(P_ProjOriginalType, pcr->m_pBaseRecord->GetOriginalTypeName());
		theUndoSlob.Resume();
		
		// now set ourselves as our own Base record
		VERIFY(m_ConfigMap.RemoveKey((void*)pcr->m_pBaseRecord));
		pcr->m_pBaseRecord = pcr;
		m_ConfigMap.SetAt((void *)pcr->m_pBaseRecord, (void *)pcr);

		// reset config.
		ForceConfigActive();
	}

	// repeat for all children*
	if (GetContentList())
	{
		POSITION pos;
	 	for (pos=GetHeadPosition(); pos!=NULL;)
	  	{
			// Any contained slobs should also be items:
			((CProjItem *)GetNext(pos))->IsolateFromProject();
		}
	}
}

BOOL CProjItem::SetSlobProp(UINT idProp, CSlob * val)
{
	// only interested in container property changes
	if (idProp != P_Container)
		// pass on to the base-class
		return CSlob::SetSlobProp(idProp, val);

	// old container?
	CSlob * pOldContainer = m_pContainer; 

	// REVIEW: this currently happens during UNDO.  Bogus Undo record
	// ASSERT(pOldContainer != val);
	if (pOldContainer == val)
		return TRUE; // nothing to do

	// from a project?
	BOOL fFromPrj = m_pContainer && m_pContainer->IsKindOf(RUNTIME_CLASS(CProjItem)) && (((CProjItem *)m_pContainer)->GetProject() != NULL);

	// into a project?
	BOOL fToPrj = val && val->IsKindOf(RUNTIME_CLASS(CProjItem));

	BOOL fPrjChanged = FALSE;
	if (fFromPrj && fToPrj)
	{
		fPrjChanged = (((CProjItem *)val)->GetProject() != ((CProjItem *)m_pContainer)->GetProject());
	}
	// pre-move item
	if (!PreMoveItem(val, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;

	// do the 'containment' part of the move
	if (!MoveItem(val, pOldContainer, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;

	// pos-move item, only call if not deleted!
	// ie. not deleted if moving to another container or
	// we're recording if, and hence moving into the undo slob.
	if (CouldBeContainer(val) &&
		!PostMoveItem(val, pOldContainer, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;

	return TRUE;	// success
}
///////////////////////////////////////////////////////////////////////////////
const CString CProjItem::EndOfBagsRecord = "\x08\x14\x53\x53\x42\x52"; // :-)

BOOL CProjItem::SerializeProps ( CArchive &ar )
{
	PFILE_TRACE ("SerializeProps for a %s at %p\n", 
							GetRuntimeClass()->m_lpszClassName, this );
	SerializePropMap(ar);

	// Now write info in the property bags:
	if (ar.IsStoring())
	{
		ConfigurationRecord *pcr;
		int i;
		int size = GetPropBagCount();
		for (i=0; i < size; i++)
		{
			pcr = (ConfigurationRecord *) m_ConfigArray[i];
			if (pcr->GetPropBag(CurrBag)->GetPropCount () == 0 ) continue;
			ar << pcr->GetConfigurationName ();
			pcr->GetPropBag(CurrBag)->Serialize (ar, this);
		}
		ar << EndOfBagsRecord;
	}
	else
	{
		CString str;
		CPropBag *pBag, DummyBag;
		while (1)
		{
			ar >> str;
			if (str == EndOfBagsRecord)
				break;

			pBag = GetPropBag(str);
			if (NULL==pBag&&IsKindOf(RUNTIME_CLASS(CProject)))
			{
				//
				// PropBag for this item was not found for current project so enumerate
				// thouugh other projects to see if appropriate PropBag can be found.
				//
				CProject* pProject = NULL;
				POSITION pos = CProject::m_lstProjects.GetHeadPosition();  // use our own private pos pointer
				while (pos != NULL)
				{
					pProject = (CProject *)CProject::m_lstProjects.GetNext(pos);

					// Already checked g_pActiveProject
					if (pProject==g_pActiveProject)
						continue;

					pBag = pProject->GetPropBag(str);
					if (NULL!=pBag)
						break;
				}

			}
			if (pBag)
				pBag->Serialize ( ar, this );
			else
			{
				// No matching config, but we still have to 
				// move through the archive if we want to keep reading:
            	DummyBag.Serialize(ar, this);
				DummyBag.Empty();
			}
		}
	}

	return TRUE;	// success
}
///////////////////////////////////////////////////////////////////////////////
//
//	Override CSlob's serialize function to save only private properties:

BOOL CProjItem::SerializePropMapFilter(int nProp)
{
	return ::IsPropPrivate (nProp);
}

///////////////////////////////////////////////////////////////////////////////
// Return a container-less, symbol-less, clone of 'this'
CSlob* CProjItem::Clone()
{
	CSlob * pClone = CSlob::Clone(); // call base class to creat clone
	ASSERT(pClone->IsKindOf (RUNTIME_CLASS (CProjItem)));

	((CProjItem*)pClone)->SetFile(GetFilePath());

	if (GetProject() != NULL)
		((CProjItem *)pClone)->IsolateFromProject();

	return pClone;
}

///////////////////////////////////////////////////////////////////////////////
void CProjItem::CopyProps(CSlob * pClone)
{
	// set up the configs to match that of 'this'

	// do we need to bother?
	if (pClone->GetPropBagCount() == 0)
	{
		ASSERT(pClone->IsKindOf(RUNTIME_CLASS(CProjItem)));
		((CProjItem *)pClone)->CloneConfigs(this);
	}

	// now ask the base-class to actually copy the prop maps
	// and these newly created prop bags
	CSlob::CopyProps(pClone);

	// if it is a projitem, then we want to copy 
	// the default prop bags as well
	if (pClone->IsKindOf(RUNTIME_CLASS (CProjItem)))
	{
		CopyDefaultPropBags((CProjItem *)pClone);
	}
}

////////////////////////////////////////////////////////////////////////////////
void CProjItem::CopyDefaultPropBags(CProjItem * pItemClone)
{
	int nBags = GetPropBagCount();
	int idOldBag = UsePropertyBag(BaseBag);
	int idOldCloneBag = pItemClone->UsePropertyBag(BaseBag);

	for (int i = 0 ; i < nBags ; i++)
	{
		CPropBag * pBag = GetPropBag(i);
		ASSERT(pBag != (CPropBag *)NULL);

		CPropBag * pCloneBag = pItemClone->GetPropBag(i);
		ASSERT(pCloneBag != (CPropBag *)NULL);
		pCloneBag->Clone(pItemClone, pBag);
	}

	(void) pItemClone->UsePropertyBag(idOldCloneBag);
	(void) UsePropertyBag(idOldBag);
}

///////////////////////////////////////////////////////////////////////////////
void CProjItem::CloneConfigs(const CProjItem * pItem)
{
//	We have moved into a new project tree and need to adopt its configurations
//	We'll enumerate throught the new parents configurations and create new
//	configs for any we don't have, and delete any configurations that the 
//	parent doesn't support.
//
//	We say one our configurations matches a parent configuration if its based
//	on the same project type and has the same name.
//
//	In the special case that we have no configurations, just blindly copy the
//	parent's:

	if (GetPropBagCount() == 0)
	{
		UINT nSize = pItem->GetPropBagCount();

		// create the new configuration records
		for (UINT nIndex = 0; nIndex < nSize; nIndex++)
		{
			const ConfigurationRecord * pcr = (ConfigurationRecord *) pItem->m_ConfigArray[nIndex];
			ASSERT_VALID(pcr);
			ASSERT_VALID(pcr->m_pBaseRecord);

			VERIFY(ConfigurationChanged(pcr->m_pBaseRecord));

			// validate newly-created CR
			pcr = (ConfigurationRecord *)m_ConfigArray[nIndex]; 
			ASSERT_VALID(pcr);
			ASSERT_VALID(pcr->m_pBaseRecord);													
		}
		ASSERT(GetPropBagCount()==pItem->GetPropBagCount()); // make sure they all got created

		// Finally, make sure we have the same active config:
		if (pItem->m_pActiveConfig)	// May be null during desrialize
		{		
			VERIFY(!ConfigurationChanged (pItem->m_pActiveConfig->m_pBaseRecord));
			ASSERT_VALID (m_pActiveConfig);
		}
		else
		{
			m_pActiveConfig = NULL; // don't leave as bogus value
		}
	}
	else
	{
		// if we got here we must be undo/redo'ing, dragging/dropping or cloning
		// if (!theUndoSlob.InUndoRedo())
		{
			// Configuration Matching. This is only used by cut/paste & drag/drop
			
			// For drag and drop the configurations should be identical
			// so we check this here!
			// However, for clipboard cut/paste,
			// the configurations will not necessarily match identically
			// and we will have to be somewhat cleverer than we already are. 

			int nIndex, nSize = pItem->GetPropBagCount();
			const ConfigurationRecord *pcr;

			for (nIndex = 0; nIndex < nSize; nIndex++)
			{
				pcr = (const ConfigurationRecord*)pItem->m_ConfigArray[nIndex];
				ASSERT_VALID (pcr);
				ASSERT_VALID (pcr->m_pBaseRecord);

				// look for matching config
				if (ConfigurationChanged(pcr->m_pBaseRecord))
				{
					// FUTURE: try for partial match
					// Last cr in m_ConfigArray is new
					// try to initialize based on projtype
				}
			}

			// Delete any unreferenced configs and reorder, if necessary
			FixConfigs(pItem);
			
			// Finally, make sure we have the same active config:
			if (pItem->m_pActiveConfig)	// May be null during desrialize
			{
				VERIFY(!ConfigurationChanged (pItem->m_pActiveConfig->m_pBaseRecord));
				ASSERT_VALID (m_pActiveConfig);
			}
  		}
	}
	ASSERT(GetPropBagCount()==pItem->GetPropBagCount());
}

///////////////////////////////////////////////////////////////////////////////
void CProjItem::FixConfigs(const CProjItem * pItem)
{
// Garbage collection

	// delete any unneeded source configs
	CPtrList deathRow;
	ConfigurationRecord * pcr;
	const ConfigurationRecord * pcrBase;
	int nSize = GetPropBagCount();
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		// look up the base config for pItem that matches each entry
		// by name.  If the base configs do not match, then this is
		// an unneeded entry that should be deleted.

		pcr = (ConfigurationRecord *)m_ConfigArray[nIndex];
		ASSERT_VALID (pcr);
		ASSERT_VALID (pcr->m_pBaseRecord);

		pcrBase = ((CProjItem *)pItem)->ConfigRecordFromConfigName(pcr->GetConfigurationName());
		if (!pcrBase || pcr->m_pBaseRecord != pcrBase->m_pBaseRecord)
		{
			// this one needs to be deleted
			deathRow.AddTail((void *)pcr->m_pBaseRecord);
		}
	}

	while (!deathRow.IsEmpty())
		DeleteConfigFromRecord((ConfigurationRecord *)deathRow.RemoveHead(), FALSE);

	ASSERT(GetPropBagCount()==pItem->GetPropBagCount());

	// Now reorder remaining configs, if necessary, based on pItem
	nSize = pItem->GetPropBagCount();
	ASSERT(nSize==GetPropBagCount()); // consistency check
	for (nIndex = 0; nIndex < nSize; nIndex++)
	{	
		pcrBase = ((ConfigurationRecord *)pItem->m_ConfigArray[nIndex])->m_pBaseRecord;
		VERIFY(m_ConfigMap.Lookup((void*)pcrBase, (void *&)pcr));
		m_ConfigArray[nIndex] = pcr;
	}
	
	//	Repeat for all children:
	if (GetContentList ())
	{
		POSITION pos;
		for (pos=GetHeadPosition(); pos!=NULL;)
		{
			// Any contained slobs should also be cleaned up
			((CProjItem *)GetNext(pos))->FixConfigs(pItem);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////
void CProjItem::DeleteConfigFromRecord (ConfigurationRecord *prec, BOOL bChildren)
{
//	Delete the configuration and tell our kids to do the same.  Note that
//  subprojects that get called for a parents config will do nothing since
// 	they won't find prec.

	PCFG_TRACE ("%s at %p deleting config with base at %p (name: %s)\n",
	  GetRuntimeClass()->m_lpszClassName, this, prec, ( (CStringProp *) 
	  ((ConfigurationRecord *)prec->m_pBaseRecord)->GetPropBag(CurrBag)->FindProp(P_ProjConfiguration))->m_strVal);

	// Notify target items about config deletions
	if (this->IsKindOf(RUNTIME_CLASS(CTargetItem)) ||
		this->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{
 		InformDependants(SN_DESTROY_CONFIG, (DWORD)prec);
	}
 
	if (bChildren && GetContentList())
	{
		for (POSITION pos = GetHeadPosition(); pos != (POSITION)NULL;)
			((CProjItem *) GetNext (pos))->DeleteConfigFromRecord (prec);
	}

	// We may not have the config rec that we want to delete, in which case we
	// don't do anything (however we do still recurse to our children just above,
	// in case they have the config rec).
	ConfigurationRecord *pMyRec;
	if (m_ConfigMap.Lookup(prec, (void *&) pMyRec))
	{
		ASSERT_VALID(pMyRec);
		ASSERT(pMyRec->m_pBaseRecord == prec);

		// remove from both Map and Array
		int i = 0;
		int size = GetPropBagCount();

		m_ConfigMap.RemoveKey(prec);
		while (i < size)
		{
			if (pMyRec == m_ConfigArray.GetAt(i))
			{
				m_ConfigArray.RemoveAt(i);
				break;	// stop if found
			}

			i++;
		}

		// if we're deleting our current active then 
		// set it to NULL which will cause ::GetActiveConfig()
		// to re-figure it out
		if (m_pActiveConfig == pMyRec)
			m_pActiveConfig = (ConfigurationRecord *)NULL;

		delete pMyRec;
	}
}

///////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CProjItem::AssertValid () const
{
	CProjItem * ncthis = (CProjItem *)this; // Brute force since we're const.

	// validate base component
	CObject::AssertValid();

	// validate content list
	if (ncthis->GetContentList()) ASSERT_VALID (ncthis->GetContentList());

	// validate our path
	if (GetFilePath ()) ASSERT_VALID (GetFilePath());

#if 0
	// We can't really check the dependencies because the currently active config
	// could be something which is not applicable to this object. We end up creating
	// bogus config records if we do this.  [sanjays]

	// validate our list of dependencies
	if (GetDependenciesList ()) ASSERT_VALID (GetDependenciesList());
#endif
}
///////////////////////////////////////////////////////////////////////////////
void CProjItem::Dump (CDumpContext &DC) const
{
	//ASSERT_VALID(this);
	POSITION pos;
	CObject *pObject;
	ConfigurationRecord *pcr;
	void *pv;
	TCHAR buf[257]; buf[256]=0;
	CProjItem *ncthis = (CProjItem *) this; // Brute force since we're const.

	if (DC.GetDepth () > 1) DC << 
	"************************************************************************";

	_snprintf ( buf, 256, "Dump of %s at %p:"
		" File path = \"%s\"",
		(const char *) GetRuntimeClass()->m_lpszClassName, (void *) this,
		(GetFilePath ()) ? ((const char *) *GetFilePath ()) : " (no file) " );
	DC << buf;

#if 0	// Doesn't work with new dependency mechanism.
	if (m_dependants.IsEmpty ()) DC << 	"DependantsList list is emtpy\n";
	else
	{
		DC << "DependantsList list:\n";
		for (pos = m_dependants.GetHeadPosition(); pos != NULL;)
		{
			pObject = m_dependants.GetNext (pos);
			_snprintf (buf, 256, "     a %s at %p    ",
			 		(const char *) pObject->GetRuntimeClass()->m_lpszClassName, 
					(void *) pObject);
			DC << buf; 
		}
	}
#endif

	int i;
	int size = GetPropBagCount();
	if (!size)	DC << 	"ConfigArray is empty\n";
	else
	{
		_snprintf ( buf, 256, "Config array has %i entries. Active is %p",
				size, (void *)((CProjItem *)this)->GetActiveConfig());
		DC << buf;	
		for (i =0; i < size; i++)
		{
			DC << (ConfigurationRecord *)m_ConfigArray[i];
		}
	}
	if (m_ConfigMap.IsEmpty ())	 DC << 	"ConfigMap is empty\n";
	else
	{
		_snprintf ( buf, 256, "Config map has %i entries. Active is %p",
				m_ConfigMap.GetCount (), (void *) ((CProjItem *)this)->GetActiveConfig());
		DC << buf;	
		for (pos = m_ConfigMap.GetStartPosition (); pos != NULL;)
		{
			m_ConfigMap.GetNextAssoc ( pos, pv, (void *&) pcr );
			DC << pcr;
		}
	}

	if (ncthis->GetContentList()) 
	{
		if (ncthis->GetContentList()->IsEmpty()) 
									DC << "Contents list is empty.";
		else
		{
			DC << "Contents list:";
			for (pos = ncthis->GetContentList()->GetHeadPosition(); pos != NULL;)
			{
				pObject = ncthis->GetContentList()->GetNext (pos);
				if (DC.GetDepth () > 1)	pObject->Dump ( DC );
				else
				{
					_snprintf (buf, 256, "     a %s at %p",
				 		(const char *) pObject->GetRuntimeClass()->m_lpszClassName,
						(void *) pObject);
					DC << buf; 
				}
			}
		}
	}
}
#endif

BOOL CProjItem::CanAct(ACTION_TYPE action)
{
	if (action == act_rename)
		return FALSE;

	return CProjSlob::CanAct(action);
}

void CProjItem::CopyCommonProps(CProjItem * pSrcItem, const CString & strTarget)
{
    ConfigurationRecord * pcrSrc = pSrcItem->ConfigRecordFromConfigName(strTarget, FALSE, FALSE);
    ConfigurationRecord * pcrDest = GetActiveConfig();
	if( pcrSrc == NULL ){
		// this is a sanity check since ConfigR...gName() can return NULL
		// if it does return NULL then there is no configuration to copy.
		// this should be ok.
		return;
	}

    CPropBag * pBagSrc = pcrSrc->GetPropBag(m_idBagInUse);
    CPropBag * pBaseBagSrc = pcrSrc->GetPropBag(BaseBag);
    CPropBag * pBagDest = pcrDest->GetPropBag(m_idBagInUse);
    CPropBag * pBaseBagDest = pcrDest->GetPropBag(BaseBag);

    // Copy all of the properties here
    //pBagDest->Clone(this, pBagSrc, FALSE);

    // Now we have to check for string list properties and potentially munge
    // the copied versions so that we only copy common settings!
    CProp * ppropSrc, * ppropBaseSrc, * ppropDest, * ppropBaseDest;

    POSITION pos = pBagSrc->GetStartPosition();
    while (pos != NULL)
    {
        UINT id;
		BOOL bInform;
        pBagSrc->GetNextAssoc(pos, id, ppropSrc);

		switch (ppropSrc->m_nType)

		{
			case integer:
				//
				// Inform dependants if setting P_ItemExcludedFromBuild to TRUE and
				// proprety for P_ItemExcludedFromBuild wasn't previously in pBagDest.
				// This is needed because CProjItem::GetIntProp has a hack to return
				// P_ItemExcludedFromBuild as TRUE when the proprety doesn't exist.  This
				// causes AssignActions to do an AddRefRef that needs to be undone.
				//
				bInform = P_ItemExcludedFromBuild==id && 
					NULL==pBagDest->FindProp(P_ItemExcludedFromBuild) &&
					((CIntProp *)ppropSrc)->m_nVal;

				pBagDest->SetIntProp(this, id, ((CIntProp *)ppropSrc)->m_nVal);

				if (bInform)
					InformDependants(id);
				break;

			case string:
			{
				COptionHandler * popthdlr = g_prjcompmgr.FindOptionHandler(id);
				BOOL fIsListStrProp = (popthdlr != (COptionHandler *)NULL) && popthdlr->IsListStrProp(id-1);
				if (!fIsListStrProp)
 					pBagDest->SetStrProp(this, id, ((CStringProp *)ppropSrc)->m_strVal);
 				else
				{
					// Ok now we may need to start munging to get correct props

					// Get all of our properties from the various bags
					ppropBaseSrc = pBaseBagSrc->FindProp(id);
					ppropDest = pBagDest->FindProp(id);
					ppropBaseDest = pBaseBagDest->FindProp(id);

					CString strNewProp;
					if (ppropDest)
					{
						// Work out the differences we need to apply to the destination bag
						COptionList optlstAdd, optlstSub;
						COptionList optlist;
						optlist.SetString(((CStringProp *)ppropBaseSrc)->m_strVal);

						optlist.Components(((CStringProp *)ppropSrc)->m_strVal, optlstAdd, optlstSub);

						// Apply the differences to the base prop val in the destination bag
						COptionList optlstDest;
						optlstDest.SetString(((CStringProp *)ppropDest)->m_strVal);
						optlstDest.Subtract(optlstSub);
						optlstDest.Append(optlstAdd);

						// Get the new prop string
						optlstDest.GetString(strNewProp);
					}
					else
						strNewProp = ((CStringProp *)ppropSrc)->m_strVal;

					// set the prop value
					pBagDest->SetStrProp(this, id, strNewProp);
				}
                break;
			}

			default:
				ASSERT(FALSE);
				break;
		}
    }
}

int CProjItem::CompareSlob(CSlob * pCmpSlob)
{
	ASSERT(pCmpSlob->IsKindOf(RUNTIME_CLASS(CProjItem)));

	// Put dependency containers at the end of this container
	if (IsKindOf(RUNTIME_CLASS(CDependencyContainer)))
		return 1;

	if (((CProjItem *)pCmpSlob)->IsKindOf(RUNTIME_CLASS(CDependencyContainer)))
		return -1;

	// Put Groups at the start of this container
	if (((CProjItem *)pCmpSlob)->IsKindOf(RUNTIME_CLASS(CProjGroup)))
		return 1;

	if (IsKindOf(RUNTIME_CLASS(CProjGroup)))
		return -1;

	// Put subprojects at the start of this container
	if (((CProjItem *)pCmpSlob)->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		return 1;

	if (IsKindOf(RUNTIME_CLASS(CProjectDependency)))
		return -1;

	CString strExt, strCmpExt; 
	BOOL bCmpIsFileItem = (((CProjItem *)pCmpSlob)->IsKindOf(RUNTIME_CLASS(CFileItem)));
	BOOL bIsFileItem = (IsKindOf(RUNTIME_CLASS(CFileItem)));
	if (bCmpIsFileItem)
		strCmpExt = ((CFileItem *)pCmpSlob)->GetFilePath()->GetExtension();
	if (bIsFileItem)
		strExt = GetFilePath()->GetExtension();

	// Put .lib files last and don't sort them alphabetically
	if (bIsFileItem && (strExt.CompareNoCase(".lib")==0))
		return 1;

	if (bCmpIsFileItem && (strCmpExt.CompareNoCase(".lib")==0))
		return -1;

	// Put .obj files last and don't sort them alphabetically
	if (bIsFileItem && (strExt.CompareNoCase(".obj")==0))
		return 1;

	if (bCmpIsFileItem && (strCmpExt.CompareNoCase(".obj")==0))
		return -1;

	// Everything else is sorted based on their P_Title property, i.e alphabetically

	int retval = CProjSlob::CompareSlob(pCmpSlob);

	// as a tie breaker, use the absolute path to at least get consistency
	if (retval == 0)
	{
		ASSERT(GetFilePath() != NULL);
		ASSERT(((CProjItem *)pCmpSlob)->GetFilePath() != NULL);
		retval = _tcsicmp((LPCTSTR)*GetFilePath(), (LPCTSTR)*((CProjItem *)pCmpSlob)->GetFilePath());
	}
	ASSERT(retval != 0); // should never be equal
	return retval;
}

///////////////////////////////////////////////////////////////////////////////
//
// --------------------------------	CFileItem ---------------------------------
//
///////////////////////////////////////////////////////////////////////////////
#define theClass CFileItem
BEGIN_SLOBPROP_MAP(CFileItem, CProjItem)
END_SLOBPROP_MAP()
#undef theClass

BOOL CFileItem::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// WARNING! This does not fall through to a message map.  You must
	// handle both CN_COMMAND and CN_UPDATE_COMMAND_UI if you want your
	// commands to be enabled.

	if (nCode == CN_COMMAND && pExtra == NULL)
	{
		if (nID == IDM_PROJITEM_OPEN)
		{
			OnOpen();
			return TRUE;
		}
	}
	else if (nCode == CN_UPDATE_COMMAND_UI)
	{
		ASSERT(pExtra != NULL);
		CCmdUI* pCmdUI = (CCmdUI*)pExtra;
		ASSERT(pCmdUI->m_nID == nID);           // sanity assert

		if (nID == IDM_PROJITEM_OPEN)
		{
			pCmdUI->Enable(TRUE);
			return TRUE;
		}
	}

	// pass on to the default handler
	return CProjItem::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

CFileItem::CFileItem () 
{
	// our file handle
	m_hndFileReg = NULL;

	// the Project Window item inset
	m_nIconIdx = 3; 

	// display name dirty to begin with
	m_fDisplayName_Dirty = TRUE;

	// by default, path is in the project dir
	m_bIsInProjDir = FALSE;

	// default is to scan a file for classes
	m_bExcludedFromScan = FALSE;

	m_bAlwaysWriteDepMacro = FALSE;
}

BOOL CFileItem::SetIntProp(UINT idProp, int val)
{
	// which prop are we setting?
	switch (idProp)
	{
		case P_ItemExcludedFromBuild:
		{
			// don't do anything if we are initializing from a project file
			CProject * pProject = GetProject(); ASSERT(pProject != (CProject *)NULL);
			if (pProject->m_bPrivateDataInitialized)
			{
				CPtrList * pFileList;
				CObList FileItemList;
				CFileItem * pItem;
				const ConfigurationRecord * pcr = GetActiveConfig();

				ASSERT(GetFilePath());
				// is this a file we can only have on of included in the build?
				if (!val &&	// wanting to include this in the build?
					GetRegistry()->GetFileList(GetFilePath(), (const CPtrList * &)pFileList) &&
					GetRegistry()->GetFileItemList(GetTarget(), pFileList, FileItemList, TRUE, pcr) &&
					FileItemList.GetCount() == 1 &&
					(pItem = (CFileItem * )FileItemList.GetHead()) != this)	// one already included in build?
				{
 					// ask the user if they want to do this?
					TCHAR * pExt = pFileList == GetRegistry()->GetRCFileList() ? _TEXT(".rc") : _TEXT(".def");
					CString strPath;
					GetFilePath()->GetRelativeName(pProject->GetWorkspaceDir(), strPath);
					if ( pcr != NULL && QuestionBox(IDS_FILE_INCL_TOGGLE, MB_YESNO, pExt, (TCHAR *)(const TCHAR *)strPath, pExt) == IDYES)
					{
						// toggle the other one to 'excluded from build'
						pItem->ForceBaseConfigActive((ConfigurationRecord *)pcr->m_pBaseRecord);
						pItem->SetIntProp(P_ItemExcludedFromBuild, TRUE);
						pItem->ForceConfigActive();
					}
					else
						return FALSE;	// don't do this
 				}

			}
			break;
		}
		case P_ItemIgnoreDefaultTool:
		{
			BOOL retval = CProjItem::SetIntProp(idProp, val);
			if (retval && GetProject() && GetProject()->m_bProjectComplete)
			{
				ConfigurationRecord * pcr = GetActiveConfig();
				CActionSlob::UnAssignActions(this, (CPtrList *)NULL, (ConfigurationRecord *)pcr->m_pBaseRecord);
				CActionSlob::AssignActions(this, (CPtrList *)NULL, (ConfigurationRecord *)pcr->m_pBaseRecord);
			}
			return retval;
		}
		break;
		case P_ItemExcludedFromScan:
		{
			// not per config props are stored locally
			m_bExcludedFromScan = val;
			return TRUE;
			break;
		}
		default:
			break;
	}

	// pass on to base-class
	return CProjItem::SetIntProp(idProp, val);
}

BOOL CFileItem::SetStrProp(UINT idProp, const CString & str)
{
	if (idProp == P_ProjItemOrgPath)
	{
		if (str.IsEmpty())
		{
			GetMoniker(m_strOrgPath);  // default
			return TRUE;
		}

		// UNDONE: much further validation needed here!!!

		m_strOrgPath = str;
		return TRUE;
	}
	return CProjItem::SetStrProp(idProp, str);
}

GPT CFileItem::GetStrProp(UINT idProp, CString& val)
{
	// is this a fake prop?
	switch (idProp)
	{
		case P_ProjItemName:	// yes
			if (m_fDisplayName_Dirty)
			{
				ASSERT(m_hndFileReg != NULL);
				m_strDisplayName = (g_FileRegistry.GetRegEntry(m_hndFileReg)->GetFilePath())
					->GetFileName ();
				m_fDisplayName_Dirty = FALSE;
			}
			val = m_strDisplayName;
			return valid;

		case P_ProjItemOrgPath:
		case P_FakeProjItemOrgPath:
			ASSERT(m_hndFileReg != NULL);
			if (m_strOrgPath.IsEmpty())
			{
				const TCHAR * pszOrgName = g_FileRegistry.GetRegEntry(m_hndFileReg)->GetOrgName();
				if (pszOrgName != NULL)
					m_strOrgPath = pszOrgName;
			}
			if (m_strOrgPath.IsEmpty())
				GetMoniker(m_strOrgPath);

			val = m_strOrgPath;
			return valid;

		case P_FileName:
		case P_ProjItemFullPath:
			ASSERT(m_hndFileReg != NULL);
			val = (const TCHAR*) *g_FileRegistry.GetRegEntry(m_hndFileReg)->GetFilePath();
			return valid;

		case P_ProjItemDate:
			ASSERT(m_hndFileReg != NULL);
			if (!g_FileRegistry.GetRegEntry(m_hndFileReg)->GetFileTime(val))
				VERIFY(val.LoadString(IDS_FILE_ITEM_NONEXISTANT));
			return valid;

		case P_ProjMainTarget:
			{
				ASSERT(GetActiveConfig());
				CFRFPtrArray * parray = GetActiveConfig()->GetTargetFilesArray();

				if (parray != NULL && parray->GetSize() > 0)
				{
					FileRegHandle frh = parray->GetAt(0);
					if (frh != (FileRegHandle)NULL)
					{
						val = (const TCHAR *) *g_FileRegistry.GetRegEntry(frh)->GetFilePath();
						return valid;
					}
				}
				return invalid;
			}

		case P_ProjMainTargetDate:
			{
				ASSERT(GetActiveConfig());
				CFRFPtrArray * parray = GetActiveConfig()->GetTargetFilesArray();

				if (parray != NULL && parray->GetSize() > 0)
				{
					FileRegHandle frh = parray->GetAt(0);
					if (frh != (FileRegHandle)NULL)
					{
						if (!g_FileRegistry.GetRegEntry(frh)->GetFileTime(val))
							VERIFY(val.LoadString(IDS_NO_TOOL_OUTPUT));
						return valid;
					}
				}
				return invalid;
			}

		// not allowed for file items
		case P_OutDirs_Target:
			if ((m_optbeh & OBShowMacro) && (GetProject()->GetOutDirString(val)))
				return valid; // config doesn't matter since always "$(OUTDIR)"
			else
				return invalid;

		case P_OutDirs_Intermediate:
			{
				// must only use INTDIR Macro, never INTDIR_SRC if defined
				if ((m_optbeh & OBShowMacro) && (GetOutDirString(val)))
					return valid;
				break;
			}
		default:
			break;
	}

	// no, pass on to base class
	return CProjItem::GetStrProp(idProp, val);
}

GPT CFileItem::GetIntProp(UINT idProp, int& val)
{
	// is this a fake prop?
	switch (idProp)
	{
		case P_ItemExcludedFromBuild:
		{
			// If this is an external target then all files are excluded from the build
			CProject * pProject = GetProject();

			// FUTURE: the top half of this 'if' may be redundant
            CProjType * pProjType;
            if (m_pForcedConfig != (ConfigurationRecord *) NULL)
            {
                pProject->ForceConfigActive((ConfigurationRecord *)m_pForcedConfig->m_pBaseRecord);
				pProjType = pProject->GetProjType();
                pProject->ForceConfigActive();
			}
            else
                pProjType = /*pProject->*/GetProjType();

#if 0
			BOOL bIsExternalTarget = pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget));
			if (bIsExternalTarget)
			{
				val = TRUE;
				return valid;
			}
			break;
#endif
		}

		case P_ItemIgnoreDefaultTool:
		{
#if 0
			int bExcluded;
			if (GetIntProp(P_ItemExcludedFromBuild, bExcluded) == valid && bExcluded)
			{
				val = 0;
				return invalid;
			}
#endif

			GPT retval = CProjItem::GetIntProp(idProp, val);
			if (retval != valid)
			{
				val = 0;
				retval = valid;	// always valid if not excluded
			}
			return retval;
		}
		break;
		case P_ItemExcludedFromScan:
		{
			// not per config props are stored locally
			val = m_bExcludedFromScan;

			// ignore this prop if zero
			return (val) ? valid : invalid;
			break;
		}

		default:
			break;
	}

	// no, pass on to base class
	return CProjItem::GetIntProp(idProp, val);
}

BOOL CFileItem::SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption)
{
	// test for multi selection.
	if (pNewSel->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
	{
		POSITION posProv = pNewSel->GetHeadPosition();
		int count = 0;
		while (posProv != NULL)
		{
			CSlob* pSlob = (CSlob*)pNewSel->GetNext(posProv);
			count++;
			if( count > 1 ){
				return TRUE;
			}
		}
	}

	// Are we dealing with an external target, since external targets do not
	// have input/output pages
	CProjType * pProjType = GetProjType();
	BOOL bIsExternalTarget = (pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget)));

#if 1	// REVIEW: bogus
	CString strConfig;
	CTargetItem * pTarget = GetTarget();
	BOOL bMatchingConfig = g_BldSysIFace.GetTargetNameFromFileSet((HFILESET)pTarget, strConfig, ACTIVE_BUILDER, TRUE);

	bIsExternalTarget = bIsExternalTarget || !bMatchingConfig;
#endif

	// items which have the PassThroughTool don't
	// have the output directories
	CBuildTool * pSrcTool = GetSourceTool();

	// if we have no source tool or if our source
	// tool is the pass-through tool, let the base
	// class deal with this
	// if( pSrcTool == (CBuildTool*)NULL )
	// 	AddPropertyPage(&g_DepGeneralPage, this);
	// else
		AddPropertyPage(&g_FileGeneralPage, this);	// is this the File 'General' page?

	// is this the File 'Inputs', 'Outputs', 'Dependencies' pages?
	if( !bIsExternalTarget )
	{
		AddPropertyPage(&g_InputsPage, this);
		AddPropertyPage(&g_OutputsPage, this);
		AddPropertyPage(&g_DepsPage, this);
	}

	CProjItem::SetupPropertyPages(pNewSel, bSetCaption);

	if( bSetCaption )
		SetPropertyCaption(CString(MAKEINTRESOURCE(IDS_FILEITEM)));

	return TRUE;
}

// Return a container-less, symbol-less, clone of 'this'
CSlob * CFileItem::Clone()
{
	// call base class to create clone
	CSlob * pClone = CProjItem::Clone();
	ASSERT(pClone->IsKindOf(RUNTIME_CLASS(CFileItem)));
	ASSERT(((CFileItem *)pClone)->GetFileRegHandle()==GetFileRegHandle());

	return pClone;
}

/////////////////////////////////////////////////////////////////////////
CFileRegSet * CFileItem::GetDependenciesList() const
{		    
    ConfigurationRecord * pcr = ((CFileItem *)this)->GetActiveConfig();
    if (pcr == (ConfigurationRecord *)NULL)
		return (CFileRegSet *)NULL;

	CActionSlobList * pActions = pcr->GetActionList();
	if (pActions->IsEmpty())
		return (CFileRegSet *)NULL;

	ASSERT(pActions->GetCount() == 1);

	return ((CActionSlob *)pActions->GetHead())->GetScannedDep();
}
//////////////////////////////////////////////////////////////////////////
BOOL CFileItem::SetFile(const CPath *pPath, BOOL bCaseApproved /*=FALSE*/) 
{
	// need to refresh this cache
	m_fDisplayName_Dirty = TRUE;

	// keep copy of old registry handle
	FileRegHandle hndFileRegOld = m_hndFileReg;

	// register this new file
#ifndef REFCOUNT_WORK
	m_hndFileReg = g_FileRegistry.RegisterFile(pPath, !bCaseApproved);
#else
	m_hndFileReg = CFileRegFile::GetFileHandle(*pPath);
#endif

	BOOL bChanged = (m_hndFileReg != hndFileRegOld);

	// release a ref. to the old file
	if (hndFileRegOld)
	{
#ifndef REFCOUNT_WORK
		g_FileRegistry.ReleaseRegRef(hndFileRegOld);
#else
	hndFileRegOld->ReleaseFRHRef();
#endif
	}

	if ((m_hndFileReg != (FileRegHandle)NULL) && bChanged)
	{
		// invalidate cached relative path
		m_strMoniker.Empty();
		if ((GetProject()!= NULL) && (GetProject()->m_bProjectComplete))
		{
			m_strOrgPath.Empty();
			GetProject()->DirtyProject();
		}

		// by default, path is in the project dir
		m_bIsInProjDir = FALSE;

		((CPath *)GetFilePath())->SetAlwaysRelative();
		// if bCaseApproved then we read this CFileItem in from makefile
		// and will therefore assume correct case is already set...
		if (bCaseApproved || !m_hndFileReg->IsMatchCase())
		{
			if (bCaseApproved && !m_hndFileReg->IsMatchCase())
			{
				// someone added this file without regard to case
				// but we DO care, so let's do the calculations
				((CFileRegFile *)m_hndFileReg)->SetFilePath(pPath);
			}
			m_hndFileReg->SetMatchCase(TRUE, bCaseApproved);
		}
 	}

	return m_hndFileReg != (FileRegHandle)NULL;	// success?
}
///////////////////////////////////////////////////////////////////////////////
BOOL CFileItem::PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	// do the base-class thing
	if (!CProjItem::PreMoveItem(pContainer, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;

#if 0
	if (g_bNewUpdateDepModel)
		return TRUE;

	// Are we deleting this file without undo switched on
	if (!CouldBeContainer(pContainer) && fFromPrj && !fToPrj)
	{
			// Then make sure that we remove the item from the dependency updating
			// queue. Note PostMoveItem will not be called if we get here.
			FileRegHandle frh = GetFileRegHandle();
			TCHAR * pch = _tcschr((LPCTSTR)(*frh->GetFilePath()), '.');

			// don't scan the deps for now, hopefully background parser will do this for us quick enough
			if (pch && (_tcsicmp(pch, ".cpp")==0 || _tcsicmp(pch, ".cxx")==0 || _tcsicmp(pch, ".c")==0 ))
			{
				// cache the file list, so we can make sure that all the deps are updated before we write out to makefile
				FileItemDepUpdateQ(this, NULL, NULL, g_FileDepUpdateListQ, fFromPrj);
				FileItemDepUpdateQ(this, NULL, NULL, g_FileForceUpdateListQ, fFromPrj);
			}
			else
			{
				// non-C files, queue it on for updating deps
				// also put it on the force update Q, so we don't miss it if idle update didn't catch it
				FileItemDepUpdateQ(this, NULL, NULL, g_FileForceUpdateListQ, fFromPrj);
				FileItemDepUpdateQ(this, NULL, frh, g_FileDepUpdateListQ, fFromPrj);
			}
	}
#endif

	return TRUE;	// success
}

BOOL CFileItem::MoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	CTargetItem * pTarget;
	
	// Get our file reg handle
	FileRegHandle frh = GetFileRegHandle();

	// moving into or out of the project?
	// project not being created/destroyed?
	CProjItem * pProjCntr = (CProjItem *)(fToPrj ? pContainer : pOldContainer);
	ASSERT(pProjCntr != (CProjItem *)NULL);

	// If no undo then we send out a delete notify now.
	// We MUST do this before the real MoveItem as we will be deleted
	if (!theUndoSlob.IsRecording() && fFromPrj && (!fToPrj) && frh != (FileRegHandle)NULL)
	{
		pTarget = pProjCntr->GetTarget();
		if (pTarget != NULL)
		{
			pTarget->AddToNotifyList(frh, fFromPrj);	
		}
	}

	if (!CProjItem::MoveItem(pContainer, pOldContainer, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;

	// Send out file addition notifications here
	if (!theUndoSlob.IsRecording() && fToPrj && (fPrjChanged || !fFromPrj) && frh != (FileRegHandle)NULL)
	{
		// If doing drag n drop then send out a delete
		if (fFromPrj)
		{
			pTarget = ((CProjItem *)pOldContainer)->GetTarget();
			if (pTarget != NULL)
			{
				pTarget->AddToNotifyList(frh, TRUE);	
			}
		}

		// send out the add
		pTarget = ((CProjItem *)pContainer)->GetTarget();
		if (pTarget != NULL)
		{
			pTarget->AddToNotifyList(frh, FALSE);	
		}

	}

 	return TRUE;	// sucess
}

BOOL CFileItem::PostMoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
 	if (!CProjItem::PostMoveItem(pContainer, pOldContainer, fFromPrj, fToPrj, fPrjChanged))
		return FALSE;
 
	// moving into or out of the project?
	// project not being created/destroyed?
	CProjItem * pProjCntr = (CProjItem *)(fToPrj ? pContainer : pOldContainer);
	ASSERT(pProjCntr != (CProjItem *)NULL);

	if ((fFromPrj || fToPrj) && pProjCntr->GetProject()->m_bProjectComplete)
	{
		// Send out 'addition' of file notification
		FileRegHandle frh = GetFileRegHandle();
		if (frh != (FileRegHandle)NULL)
		{
			// Send out 'addition' or 'deletion' of file notification
			if (fFromPrj && fToPrj)
			{
				ASSERT(pOldContainer != pContainer);
				if (fPrjChanged)
				{
					ASSERT(((CProjContainer *)pOldContainer)->GetTarget()!=((CProjContainer *)pContainer)->GetTarget());
					// Send out a delete followed by an add, for drag/n/drop
					((CProjItem *)pOldContainer)->GetTarget()->AddToNotifyList(frh, TRUE);
					((CProjItem *)pContainer)->GetTarget()->AddToNotifyList(frh, FALSE);
					
					// invalidate the moniker beacuse it changed projects.
					if( fPrjChanged ){
						m_strOrgPath.Empty();
						m_strMoniker.Empty();
					}
				}
				else
				{
					ASSERT(((CProjContainer *)pOldContainer)->GetTarget()==((CProjContainer *)pContainer)->GetTarget());
					// REVIEW: do nothing in this case
				}
			}
			else
			{
				pProjCntr->GetTarget()->AddToNotifyList(frh, fFromPrj);
			}

#if 0 // dead code.
			if (!g_bNewUpdateDepModel)
			{
				// do dep updates for the files move into project
				// 1) C files, put on the waiting queue, this queue will be updated only before 
				//    makefile being written out most of the time, deps will be updated through 
				//    parser calls, items on the queue will be removed
				// 2) Non-C files, will be put on unpdate queue, scan deps will happen in the idle loop.

				// get extension
				TCHAR * pch = _tcschr((LPCTSTR)(*frh->GetFilePath()), '.');

				// don't scan the deps for now, hopefully background parser will do this for us quick enough
				if (pch && (_tcsicmp(pch, ".cpp")==0 || _tcsicmp(pch, ".cxx")==0 || _tcsicmp(pch, ".c")==0 ))
				{
					// cache the file list, so we can make sure that all the deps are updated before we write out to makefile
					FileItemDepUpdateQ(this, NULL, NULL, g_FileForceUpdateListQ, fFromPrj);
				}
				else
				{
					// non-C files, queue it on for updating deps
					// also put it on the force update Q, so we don't miss it if idle update didn't catch it
					FileItemDepUpdateQ(this, NULL, NULL, g_FileForceUpdateListQ, fFromPrj);
					FileItemDepUpdateQ(this, NULL, frh, g_FileDepUpdateListQ, fFromPrj);
				}
			}
#endif // dead code.
		}
	}

	return TRUE;	// sucess
}
///////////////////////////////////////////////////////////////////////////////
void CFileItem::FlattenQuery(int fo, BOOL& bAddContent, BOOL& fAddItem)
{
	if (fo & flt_RespectItemExclude)
	{
		// get excluded from and convert into fAddItem by negating
		VERIFY(GetIntProp(P_ItemExcludedFromBuild, fAddItem) == valid);
		fAddItem = !fAddItem;
	}
	else
		fAddItem = TRUE;
}
///////////////////////////////////////////////////////////////////////////////
void CFileItem::GetMoniker( CString& cs )
{
	if (m_strMoniker.IsEmpty())
	{
		// Get the file's name relative to its project directory:
		const CPath *pPath = GetFilePath ();	
		ASSERT (pPath);
		// ASSERT(pPath->GetAlwaysRelative());
		if (!pPath->GetAlwaysRelative()) // this could fail if shared from SCC, etc.
			((CPath *)pPath)->SetAlwaysRelative(); // needed for GetRelativeName() call

		if (!pPath->GetRelativeName(GetProject()->GetWorkspaceDir(), m_strMoniker))
			{
				m_bIsInProjDir = FALSE;
				m_strMoniker = *pPath;
			}
		else if ((m_strMoniker[0]==_T('.')) && (m_strMoniker[1]==_T('\\')) &&
				(_tcschr(LPCTSTR(m_strMoniker) + 2, _T('\\'))==NULL))
		{
			// can only use inference rules if in project directory
			// FUTURE: allow subdirs as well
			m_bIsInProjDir = TRUE;
		}
		else
			m_bIsInProjDir = FALSE;

		ASSERT(!m_strMoniker.IsEmpty());
	}

	cs = m_strMoniker;
}

///////////////////////////////////////////////////////////////////////////////
// Verb Commands
void CFileItem::OnOpen()
{
	const CPath * pPath = g_FileRegistry.GetRegEntry(m_hndFileReg)->GetFilePath();
	ASSERT(pPath != (const CPath *)NULL);

	// cannot open a file that would be opened as
	// a project [DOLPHIN bug #13674]
	CDocument * pDocument = NULL;

	// We should open makefiles as text
	CString strFileName = *pPath;
	if (strFileName == *g_BldSysIFace.GetBuilderFile((HBUILDER)GetProject()))
	{
		CDocument* pOpenDoc;
		CDocTemplate * pTemplate =
			theApp.GetOpenTemplate(strFileName, pOpenDoc);

		// Check to see if its already open
		if (pTemplate != NULL)
		{
			ASSERT(pOpenDoc->IsKindOf(RUNTIME_CLASS(CPartDoc)));

			((CPartDoc*) pOpenDoc)->ActivateDoc();
			return;
		}

		// This makefile is not the same as the builder currently loaded, so we
		pTemplate = theApp.GetTemplate(CLSID_TextDoc /*CLSID_TextDocument*/);
		if (pTemplate != NULL)
		{
			// can safely open it as text!
			theApp.LockRecentFileList(TRUE);
			pTemplate->OpenDocumentFile(*pPath);
			theApp.LockRecentFileList(FALSE);
		}
		else
			TRACE("CProjItem::OnOpen() failed to open makefile as text\n");

 		return;
	}
	//else
	//{
		// Attempting to open the current project's makefile as text, but
		// we can only have a file open by one editor at a time
		//MsgBox(Information, IDS_ERR_OPENING_MAK_AS_TEXT);
		//return;
	//}

	// before OpenDocumentFileEx(), it checks if the file exists on disk
	// or not. If it doesn't exist, it should ask if an empty file should be
	// created or not. We create empty file if we answer yes.
	// Modify the above with "give source control a shot first"
	COleRef<ISourceControl> sourceControl;
	BOOL bScc = FALSE;
// REVIEW: this code should be after the check through loaded documents
// so that if a file is open but deleted, double clicking goes to
// the open document.
	if ((!pPath->ExistsOnDisk()) &&
		// give the user a chance to "get latest" if it doesn't exist.
			!(SUCCEEDED(theApp.FindInterface(IID_ISourceControl, (void**)&sourceControl))
			&& (sourceControl != NULL)
			&& (bScc = (sourceControl->IsValidOp(*pPath, SccSyncOp) == S_OK))
			&& (QuestionBox (IDS_SCCGET_NONEXISTANT_FILE, MB_YESNO, pPath->GetFullPath()) == IDYES)
			&& (sourceControl->Sync(*pPath, TRUE) == S_OK) // TRUE to sleep
			&& (pPath->ExistsOnDisk()))
		)
	{
		if (bScc) {
			return;
		}
	// REVIEW: not sure if we want the "look through opened docs" code to run in the
	// scc case or not.

		// doesn't exist on disk
		CPartTemplate * pTemplate, * pOpenTemplate = (CPartTemplate *)NULL;
		CDocTemplate::Confidence confidence;

		POSITION pos = theApp.GetFirstDocTemplatePosition();

		while (pos != NULL)
		{
			pTemplate = (CPartTemplate*)theApp.GetNextDocTemplate(pos);
			if (!pTemplate->IsKindOf(RUNTIME_CLASS(CPartTemplate)))
				continue;

			// use the basic MFC template matching, this file doesn't exist
			// and package template matchers return CDocTemplate::noAttempt
			// if they can't open the file so that they can check the format
			CDocument * pDoc;
			confidence = pTemplate->CDocTemplate::MatchDocType(*pPath, pDoc);

			// already open? if so don't create .. use the current opened one
			// use this template to create the document?
			if (confidence == CDocTemplate::yesAlreadyOpen ||
				confidence == CDocTemplate::yesAttemptNative)
			{
				pOpenTemplate = pTemplate;
				break;
			}
		}

		// can we just open the currently opened one?
		if (confidence != CDocTemplate::yesAlreadyOpen)
		{
			// file doesn't exist, so we should ask if one should be created
			if (QuestionBox (IDS_CREATE_FILE, MB_YESNO, pPath->GetFullPath()) == IDNO)
				return;

			// did we find a template to create this?
			if (pOpenTemplate != (CPartTemplate *)NULL)
			{
				CDocument * pDoc;
				if ((pDoc = pOpenTemplate->OpenDocumentFile(NULL)) != (CDocument *)NULL)
				{
					// save this
					ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CPartDoc)));
					((CPartDoc *)pDoc)->DoSave(*pPath, TRUE);
				}
			}
			else
			{
				// create a blank file (no template matched this)
				HANDLE h;
				h = CreateFile(pPath->GetFullPath(), 
							   GENERIC_READ | GENERIC_WRITE,
							   FILE_SHARE_READ,
							   NULL,
							   CREATE_NEW,
							   FILE_ATTRIBUTE_NORMAL,
							   NULL);
				if (h == INVALID_HANDLE_VALUE)
				{
					ErrorBox (IDS_CANNOT_CREATE_FILE, pPath->GetFullPath());
					return;
				}
				else
					CloseHandle (h);
			}
		}
	}

	// open up this file (which does now exist), or execute it if it is an executable
	const char * szFileName = (const TCHAR *)*pPath;
	ASSERT(szFileName != (const TCHAR *)NULL);

	if (IsExecutableFile(szFileName))
	{
		ShellExecute(AfxGetApp()->m_pMainWnd->m_hWnd, NULL, 
					 szFileName, NULL, NULL, SW_SHOWNORMAL);
	}
	else
	{
		// Call OpenDocumentFileEx instead of OpenDocumentFile.  The
		// extended function tries to open the file with ShellExecute 
		// if there's not a good template match.
		theApp.LockRecentFileList(TRUE);
		theApp.OpenDocumentFileEx(szFileName, TRUE);
		theApp.LockRecentFileList(FALSE);
	}
}

BOOL CFileItem::ScanDependencies (BOOL bUseAnyMethod /* = FALSE */, 
	BOOL bWriteOutput /* = TRUE */ )
{
	BOOL bRetval = TRUE;
	CBuildTool * pTool = (CBuildTool *)GetSourceTool();
	ConfigurationRecord* pcr = GetActiveConfig();


	if (g_bNewUpdateDepModel && NULL != pcr && NULL != pTool && pTool->HasDependencies(pcr))
	{
		ASSERT(pcr->m_pOwner==this);

		CStringArray strArray;
		if (pTool->GetDependencies(pcr,strArray))
		{
			// See if dependencies have changed and then update
			// update if necessary
			BOOL bDirty = FALSE;
			CActionSlob* pAction = NULL;
			CActionSlobList* pActions = pcr->GetActionList();
			if (NULL==pActions)
				return FALSE;

			POSITION pos = pActions->GetHeadPosition();
			if (NULL==pos)
				return FALSE;

			// Get CActionSlob* from pcr that matches this tool.  Assume that
			// the CActionSlobList can have multiple CActionSlob's since we
			// may be allowing more than one tool to be associated with a tool
			// eventually.
			while (NULL!=pos)
			{
				CActionSlob* pLastAction = (CActionSlob*)pActions->GetNext(pos);

				if (pTool==pLastAction->m_pTool)
				{
					pAction = pLastAction;
					break;
				}
			}

			if (NULL==pAction)
				return FALSE;

			//
			// Determine if dependencies have changed.
			//
			{
				// remember previously scanned deps.
				// and also any old ones (so we can compare diffs)
				CMapPtrToPtr depsPrevious;

				// construct the map of current deps
				FileRegHandle frh;
				CFileRegSet * pfrs;
				
				pfrs = pAction->GetScannedDep();
				pfrs->InitFrhEnum();
				while ((frh = pfrs->NextFrh()) != (FileRegHandle)NULL)
#ifndef REFCOUNT_WORK
					depsPrevious.SetAt(frh, (void*)DEP_Scanned);
#else
				{
					depsPrevious.SetAt(frh, (void*)DEP_Scanned);
					frh->ReleaseFRHRef();
				}
#endif

				pfrs = pAction->GetMissingDep();
				pfrs->InitFrhEnum();
				while ((frh = pfrs->NextFrh()) != (FileRegHandle)NULL)
#ifndef REFCOUNT_WORK
					depsPrevious.SetAt(frh, (void*)DEP_Missing);
#else
				{
					depsPrevious.SetAt(frh, (void*)DEP_Missing);
					frh->ReleaseFRHRef();
				}
#endif

				for (int i=0; i < strArray.GetSize(); i++ )
				{
					CPath path;
					VERIFY(path.Create(strArray[i]));
#ifndef REFCOUNT_WORK
					FileRegHandle frh = g_FileRegistry.LookupFile(&path);
#else
					FileRegHandle frh = CFileRegFile::LookupFileHandleByName(path);
					if (NULL!=frh)
						frh->ReleaseFRHRef();
#endif
					if (NULL==frh)
					{
						bDirty = TRUE;
						break;
					}
					else if ( !depsPrevious.RemoveKey((void *)frh) )
					{
						bDirty = TRUE;
						break;
					}
				}
				bDirty = bDirty || !depsPrevious.IsEmpty();
			}

			if (bDirty)
			{
				// clear the action's dependencies list
				pAction->ClearScannedDep();
				pAction->ClearMissingDep();

#ifdef USE_LATEST_IDB
				//
				// Add to dep scanner if necessary
				//
				// review(tomse) : checked in disabled because of risk that special files
				// such as the .pch that are configuration specific could be erroneously added
				// as a dependency to another configuration.  This would be bad for files that
				// were dependent only one the source and the pch files.
				//
				BOOL bAddToScannerCache = FALSE;
				if (bUseAnyMethod)
				{
					if (!g_ScannerCache.LookupFile(GetFileRegHandle()))
					{
						pAction->SetDepState(DEP_Empty);
						bAddToScannerCache = TRUE;
					}
				}
#endif // #ifdef USE_LATEST_IDB

				// add new ones
				for (int i=0; i < strArray.GetSize(); i++ )
				{
					CPath path;
					VERIFY(path.Create(strArray[i]));
#ifndef REFCOUNT_WORK
					FileRegHandle frh = g_FileRegistry.RegisterFile(&path);
					if (NULL!=frh)
						pAction->AddScannedDep(frh);
#else
					FileRegHandle frh = CFileRegFile::GetFileHandle(path);
					if (NULL!=frh)
					{
						pAction->AddScannedDep(frh);

#ifdef USE_LATEST_IDB
						//
						// Store in scanner cache so file doesn't need to be scanned.
						//
						if (bAddToScannerCache)
						{
							// Add scanned deps here
							CString str = path;
							VERIFY(pAction->AddScannedDep(0xff, str, 0, FALSE));
						}
#endif // #ifdef USE_LATEST_IDB
						frh->ReleaseFRHRef();
					}
#endif
				}

#ifdef USE_LATEST_IDB
				if (bAddToScannerCache)
				{
					CIncludeEntryList* pEntryList = pAction->GetIncludeDep();

					// FUTURE: remove hack init.
					// do we need an 'dummy' list?
					if (pEntryList == DEP_Empty)
						pEntryList = new CIncludeEntryList(5);

  					g_ScannerCache.Add(GetFileRegHandle(), pEntryList);
				}
#endif // #ifdef USE_LATEST_IDB

				// No need to dirty project anymore
				// GetProject()->DirtyProject();
			}

			pAction->UpdateDepInfoTimeStamp();

			bRetval = TRUE;
		}
		else if (bUseAnyMethod)
		{
			//
			// Try to use the scanner.  
			// review(tomse): May try to use NCB information if available.
			//
			ConfigCacheEnabler EnableCaching;

			if (pTool != (CBuildTool *)NULL && pTool->IsScannable())
			{
				g_ScannerCache.BeginCache();
				bRetval = pTool->ScanDependencies(this,bWriteOutput);
				g_ScannerCache.EndCache();
			}
		}
		else
		{
			bRetval = FALSE;
		}
	}
	else
	{
		ConfigCacheEnabler EnableCaching;

		if (pTool != (CBuildTool *)NULL && pTool->IsScannable())
		{
			g_ScannerCache.BeginCache();
			bRetval = pTool->ScanDependencies(this,bWriteOutput);
			g_ScannerCache.EndCache();
		}
	}

	return bRetval;
}

//auto update dep, don't want check in this yet
BOOL CFileItem::UpdateDependencies (CPtrList *pdepList)
{
	// Should not reach this code when using new model.
	ASSERT(!g_bNewUpdateDepModel);

	BOOL bRetval = TRUE;
	CBuildTool * pTool = (CBuildTool *)GetSourceTool();

	if (pTool != (CBuildTool *)NULL && pTool->IsScannable())
	{
		bRetval = pTool->UpdateDependencies(this, pdepList);
	}

	return bRetval;
}

///////////////////////////////////////////////////////////////////////////////
BOOL CFileItem::WriteMakeFileDependencies (CMakFileWriter& mw, CNameMunger& nm, BOOL bPerConfig)
{
    // This is where you can write out per-config dep info, e.g.
    // take a look at CFileItem's override.

    const CDir& BaseDir = GetProject()->GetWorkspaceDir();

    ConfigurationRecord * pcr = GetActiveConfig();
    ASSERT(pcr != (ConfigurationRecord *)NULL);

	CActionSlobList * pActions = pcr->GetActionList();
	if (pActions->IsEmpty())
		return TRUE;	// ignore

	ASSERT(pActions->GetCount() == 1);

	CActionSlob * pAction = (CActionSlob *)pActions->GetHead();
	CBuildTool * pTool = pAction->m_pTool;

	CString strDeps, strDepMacro;
	if (!mw.IsMakefile())
	{
	// no need to write deps for internal project if
	// MRE deps are supported
	if ((!mw.IsDepFile()) && (pTool->DontWriteDependencies(pcr)))
		return TRUE;

	CFileRegSet * pregsetScanned = pAction->GetScannedDep();
#if 0
	// Not used.
	CFileRegSet * pregsetSource = pAction->GetSourceDep();
#endif
	CFileRegSet * pregsetMissing = pAction->GetMissingDep();


	// write the dependencies macro which is used by all the tools?
#ifndef REFCOUNT_WORK
	if (!pregsetScanned->GetContent()->IsEmpty())
#else
	if ((!pregsetScanned->IsEmpty())
// FAST_EXPORT
		 || (bPerConfig && (!GetProject()->m_bProj_PerConfigDeps) && ScanDependencies(TRUE, FALSE) && (!(pregsetScanned = pAction->GetScannedDep())->IsEmpty()))
// FAST_EXPORT
		)
#endif
	{
		// start text
        strDeps = _TEXT("\\\r\n\t");

		// add scanned dependencies?
#ifndef REFCOUNT_WORK
		if (!pregsetScanned->GetContent()->IsEmpty())
#endif
		{
			MakeQuotedString(
						pregsetScanned,
						strDeps,
						&BaseDir,
						TRUE,
						NULL,
						NULL,
						_TEXT("\\\r\n\t"),
						FALSE, NULL,	// all, not outdir relative
						TRUE			// sort output
						);
			// need an extra separator if scanned dependencies is not null
			strDeps += _TEXT("\\\r\n\t"); 
		}
    }

// FAST_EXPORT
	m_bAlwaysWriteDepMacro = ((!bPerConfig) && (!GetProject()->m_bProj_PerConfigDeps) && (!strDeps.IsEmpty()));
// FAST_EXPORT

	// have any dependencies to write out?
	if (!strDeps.IsEmpty())
	{
		if (mw.IsDepFile())
		{
			// CObList lstItem;
			// pTool->GetTargets(pAction, lstItem, strTargets, FALSE);
			// REVIEW: copy from static
			mw.WriteDesc(strSrc, strDeps);
		}
		else if (GetDependenciesMacro(DEP_Scanned, strDepMacro, nm))
		{
		// write out on macro lines 'DEP_CPP_FOOBAR = '
			mw.WriteMacro(strDepMacro, strDeps);
		}
	}

	if (!mw.IsDepFile())
	{
#ifndef REFCOUNT_WORK
	if (!pregsetMissing->GetContent()->IsEmpty())
#else
	if (!pregsetMissing->IsEmpty())
#endif
	{
		// start text
        strDeps = _TEXT("\\\r\n\t");

		// add scanned dependencies?
#ifndef REFCOUNT_WORK
		if (!pregsetMissing->GetContent()->IsEmpty())
#endif
		{
			MakeQuotedString(
						pregsetMissing,
						strDeps,
						&BaseDir,
						TRUE,
						NULL,
						NULL,
						_TEXT("\\\r\n\t"),
						FALSE, NULL,	// all, not outdir relative
						TRUE			// sort output
						);
			// need an extra separator if missing dependencies is not null
			strDeps += _TEXT("\\\r\n\t"); 
		}

		// write out on macro lines 'NODEP_CPP_FOOBAR = '
		if (GetDependenciesMacro(DEP_Missing, strDepMacro, nm))
			mw.WriteMacro(strDepMacro, strDeps);
	}
	}	// !IsDepFile()

	}	// !IsMakefile()

	if (!mw.IsDepFile())
	{
		//
		// Code for handling User-defined dependencies
		//
		if (valid == GetStrProp(P_UserDefinedDeps, strDeps) )
		{
			if (!strDeps.IsEmpty())
			{
				// write out on macro lines 'NODEP_CPP_FOOBAR = '
				if (GetDependenciesMacro(DEP_UserDefined, strDepMacro, nm))
					mw.WriteMacro(strDepMacro, strDeps);
			}
		}
	}

    return TRUE;
}

BOOL CFileItem::WriteToMakeFile
( 
	CMakFileWriter& pw, 
	CMakFileWriter& mw, 
	const CDir &BaseDir,
	CNameMunger& nm
)
{
	// FUTURE (karlsi): remove BaseDir and just use CProject::GetProjDir()
	ASSERT(BaseDir==GetProject()->GetWorkspaceDir());

	int i, size;
	CString str, strOut;
	ConfigurationRecord *pcr;
	BOOL bNamedTool= FALSE;
	BOOL bHasTool= FALSE;
	CBuildTool * pBuildTool = NULL;
	const TCHAR *pc = GetNamePrefix (this);

	PFILE_TRACE ("CFileItem \"%s\" at %p writing to CMakFileWriter at %p\n",
				(const TCHAR *)	*GetFilePath(),this, &mw);
	
	TRY			  
	{
		if (g_bWriteProject)
		{
			// pw.WriteSeperatorString ();
			pw.WriteCommentEx ("Begin %s", pc);
			pw.EndLine ();
		}
		if (g_bExportMakefile)
		{
			// mw.WriteSeperatorString ();
			// pw.EndLine ();
		}
		
		strSrc = m_strOrgPath;
		if (strSrc.IsEmpty())	// REVIEW: additional validation required
			GetMoniker(strSrc);
		else {
			CString strDummy;
			GetMoniker(strDummy);
		}

		if (::ScanPathForSpecialCharacters(strSrc)){
			TCHAR *szSrcNoQuote = StripQuotes( strSrc );
			CString strSrcNoQuote = szSrcNoQuote;
			strSrc = _T('\"') + strSrcNoQuote + _T('\"');
			delete szSrcNoQuote;
		}
		
		// Examine all of our configs and see if in any of them we
		// have a tool with a name.  If there are any, then we'll need
		// a dpendencies macro:
		size = GetPropBagCount();
		for (i = 0; i < size; i++)
		{
			pcr = (ConfigurationRecord *) m_ConfigArray[i];
			ASSERT_VALID (pcr);
			pBuildTool = pcr->GetBuildTool();
			if (pBuildTool != NULL)
			{ 
				bHasTool = TRUE;
				if (!pBuildTool->GetName()->IsEmpty()) 
				{
					bNamedTool = TRUE; 
					break;
				}
			}
		}

		if (g_bWriteProject)
			pw.WriteMacro ( pcFileMacroName, strSrc );	

		if (g_bExportMakefile && bHasTool)
			mw.WriteMacro ( pcFileMacroName, strSrc );	

#if 0	// breaks things
//#ifdef _DEBUG
		ConfigurationRecord * pcr = GetActiveConfig();
		const ConfigurationRecord * pcrSaveBase = pcr->m_pBaseRecord;
#endif
		if (!g_buildfile.WriteToolForNMake(this, &nm))
			AfxThrowFileException(CFileException::generic);
#if 0
// #ifdef _DEBUG
		ASSERT(pcr == GetActiveConfig());
		ASSERT(pcrSaveBase == pcr->m_pBaseRecord);
#endif

		if (g_bWriteProject)
			pw.WriteCommentEx ( "%s %s", EndToken, pc);
	}
	CATCH(CException, e)
	{
		return FALSE;	// failure
	}
	END_CATCH

	return TRUE;	// success
}
///////////////////////////////////////////////////////////////////////////////
//	Read in this file from the makefile reader
//

struct Pair {
	CObject *pObject;
	ConfigurationRecord *pcr;
	CBuildTool *pTool;
};


BOOL CFileItem::ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir)
{
	CPtrList ListMacros;
	BOOL bPerConfigDeps = TRUE, bPerConfigDesc = TRUE;
	TCHAR *pBase, *pTokenEnd;
	CObject *pObject = NULL;
	Pair *pPair;

	PFILE_TRACE ("CFileItem at %p reading from CMakFileWriter at %p\n",
														this, &mr);
	TRY
	{
		// The next element better be a comment marking our start:
		pObject = mr.GetNextElement ();
		ASSERT (pObject->IsKindOf ( RUNTIME_CLASS(CMakComment)));

		delete pObject;

		// The next item should be a macro with our file name:
		pObject = mr.GetNextElement ();
		if (!pObject->IsKindOf ( RUNTIME_CLASS(CMakMacro))
				||
			((CMakMacro*) pObject)->m_strName.CompareNoCase (pcFileMacroName)
			)  AfxThrowFileException (CFileException::generic);

		// This is the accpeted way to get a mungable pointer a CString's
		// data.  Note we never call ReleaseBuffer:

		pBase =	((CMakMacro*) pObject)->m_strValue.GetBuffer (1);	
		SkipWhite (pBase);
		GetMakToken (pBase, pTokenEnd);

		*pTokenEnd = _T('\0');

		CPath cpFile;
		m_strOrgPath = pBase;

		// translate $(MACRO_NAME) here.
		CString strBase = pBase;
		if( !ReplaceEnvVars(strBase) ){
			strBase = pBase;
		}
		
		if (pBase == pTokenEnd ||
			!cpFile.CreateFromDirAndRelative (BaseDir, strBase))
			AfxThrowFileException (CFileException::generic);

		// preserve relative path if it begins with "..\"
		// if ((pBase[0] == _T('.')) && (pBase[1] == _T('.')))
		cpFile.SetAlwaysRelative();

		if (!SetFile(&cpFile, TRUE))
			AfxThrowFileException (CFileException::generic);

		delete pObject;

		m_cp = EndOfConfigs;
	   	for (pObject = mr.GetNextElement(); !IsEndToken(pObject); pObject = mr.GetNextElement())
	   	{
			if (IsPropComment(pObject) || IsAddSubtractComment(pObject))
				continue;

			if (pObject->IsKindOf(RUNTIME_CLASS(CMakMacro)))
			{
				const TCHAR * pMacName = (const TCHAR *)((CMakMacro *)pObject)->m_strName;
				
				// Per-config deps.?
				if (_tcsnicmp("DEP_", pMacName, 4) == 0 ||
					_tcsnicmp("NODEP_", pMacName, 6) == 0 ||
					_tcsnicmp(_TEXT("USERDEP_"), pMacName, 8) == 0) // user-defined dependency
				{
					if (m_cp == EndOfConfigs)
						bPerConfigDeps = FALSE;
				}

				// Got a tool macro, store it away for later processing, after
				// we have created all the actions
				pPair = new Pair;
				pPair->pObject = pObject;
				pPair->pcr = GetActiveConfig();
				pPair->pTool = g_pBuildTool;
				ListMacros.AddTail( pPair );

				pObject = (CObject *)NULL;
				continue;
			}
			else if (pObject->IsKindOf(RUNTIME_CLASS(CMakDescBlk)))
			{
				// Got a tool desc. block macro, store it away for later processing, after
				// we have created all the actions
				if (m_cp == EndOfConfigs)
					bPerConfigDesc = FALSE;

				pPair = new Pair;
				pPair->pObject = pObject;
				pPair->pcr = GetActiveConfig();
				pPair->pTool = NULL;
				ListMacros.AddTail( pPair );

				pObject = (CObject *)NULL;
				continue;
			}

			if (pObject->IsKindOf(RUNTIME_CLASS(CMakDirective))) 
			{
				// Setup this configuration
				ReadConfigurationIfDef((CMakDirective *)pObject);
			}

	   		delete pObject; pObject = (CObject *)NULL;
		}				 

		// Make sure we got a file name:
		ASSERT (GetFilePath () != NULL);

		// Do rest of the move
		(void)PostMoveItem(GetContainer(), (CSlob *)NULL, FALSE, TRUE, FALSE);

		//
		// Process tool macros now
		//
 		ConfigurationRecord * pcr;
		CProject * pProject = GetProject();
		CProjTempConfigChange projTempConfigChange(pProject);

		while( !ListMacros.IsEmpty() )
		{
			pPair =(Pair *)ListMacros.RemoveHead();
			pObject = pPair->pObject;
			pcr = pPair->pcr;
			g_pBuildTool = pPair->pTool;
			delete pPair;

			// Make sure we're in the right configuration
			projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);

			// Descriptor block or macro?
			if (pObject->IsKindOf(RUNTIME_CLASS(CMakMacro)))
			{
				if (!bPerConfigDeps)
					m_cp = EndOfConfigs;
				else
					m_cp = FirstConfig;

				if (!SuckMacro((CMakMacro *)pObject, BaseDir, TRUE))
					AfxThrowFileException(CFileException::generic);
			}
			else
			{
				if (!bPerConfigDesc)
					m_cp = EndOfConfigs;
				else
					m_cp = FirstConfig;

				if (!SuckDescBlk((CMakDescBlk *)pObject))
					AfxThrowFileException(CFileException::generic);				
			}
				
			delete pObject; pObject = (CObject *)NULL;
		}
	}
	CATCH ( CException, e)
	{
		// free the object that we were working on when the exception occured.
		if (pObject != (CObject *)NULL)
		{
	   		delete pObject; pObject = (CObject *)NULL;
		}

		// free all Tool macros that we hadn't processed yet.
		while( !ListMacros.IsEmpty() )
		{
			pPair = (Pair *)ListMacros.RemoveHead();
			delete pPair->pObject;
			delete pPair;
		}
		
		return FALSE;	// failure
	}
	END_CATCH

	return TRUE;	// success
}

BOOL CFileItem::CanAct(ACTION_TYPE action)
{
	if (action == act_delete || action == act_cut
#define FILE_RENAME_SUPPORTED
#ifdef FILE_RENAME_SUPPORTED
	 	|| action == act_rename
#endif
		)
		return (!g_Spawner.SpawnActive());

    // File items can be dragged into CTargetItems and CProjGroups
    if (action == act_drag)
        return TRUE;

	return CProjItem::CanAct(action);
}

BOOL CFileItem::PreAct(ACTION_TYPE action)
{
	if (action == act_delete || action == act_cut)
	{
		COleRef<IPkgProject> pPkgProj = NULL;
		VERIFY(SUCCEEDED(g_BldSysIFace.GetPkgProject((HBUILDER)GetProject(), &pPkgProj)));
		NOTIFYINFO notify;
		notify.pPkgProject = pPkgProj;
		notify.hFileSet = (HFILESET)GetTarget();
		notify.lstFiles.AddTail((CPath*)GetFilePath());

		BOOL bRetVal = theApp.NotifyPackages(PN_QUERY_DEL_FILE, (void *)&notify);

		notify.lstFiles.RemoveAll();
	
		return bRetVal;
	}

    if (action == act_drag)
        return TRUE;

	return CProjItem::PreAct(action);
}

///////////////////////////////////////////////////////////////////////////////
//
// ---------------------------	CDependencyFile -------------------------------
//
///////////////////////////////////////////////////////////////////////////////
#define theClass CDependencyFile
BEGIN_SLOBPROP_MAP(CDependencyFile, CProjItem)
END_SLOBPROP_MAP()
#undef theClass

BOOL CDependencyFile::OnCmdMsg(UINT nID, int nCode, void* pExtra,
	AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// WARNING! This does not fall through to a message map.  You must
	// handle both CN_COMMAND and CN_UPDATE_COMMAND_UI if you want your
	// commands to be enabled.

	if (nCode == CN_COMMAND && pExtra == NULL)
	{
		if (nID == IDM_PROJITEM_OPEN)
		{
			OnOpen();
			return TRUE;
		}
	}
	else if (nCode == CN_UPDATE_COMMAND_UI)
	{
		ASSERT(pExtra != NULL);
		CCmdUI* pCmdUI = (CCmdUI*)pExtra;
		ASSERT(pCmdUI->m_nID == nID);           // sanity assert

		if (nID == IDM_PROJITEM_OPEN)
		{
			pCmdUI->Enable(TRUE);
			return TRUE;
		}
	}

	// pass on to the default handler
	return CProjItem::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

CDependencyFile::CDependencyFile()
{
	m_hndFileReg = NULL;
 
 	// the Project Window icon inset
	m_nIconIdx = 3;

	// display name dirty to begin with
	m_fDisplayName_Dirty = TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Verb Commands
void CDependencyFile::OnOpen()
{
	const CPath * pPath = g_FileRegistry.GetRegEntry(m_hndFileReg)->GetFilePath();
	ASSERT(pPath != (const CPath *)NULL);

	// cannot open a file that would be opened as
	// a project [DOLPHIN bug #13674]
	CDocument * pDocument = NULL;
	theApp.LockRecentFileList(TRUE);
	theApp.OpenDocumentFile(*g_FileRegistry.GetRegEntry(m_hndFileReg)->GetFilePath());
	theApp.LockRecentFileList(FALSE);
}

BOOL CDependencyFile::SetFile(const CPath *pPath, BOOL bCaseApproved /*=FALSE*/) 
{
	FileRegHandle hndFileRegOld = m_hndFileReg;

	// add a reference to the new file
#ifndef REFCOUNT_WORK
	m_hndFileReg = g_FileRegistry.RegisterFile(pPath, !bCaseApproved);
#else
	m_hndFileReg = CFileRegFile::GetFileHandle(*pPath);
#endif

	// remove the reference to the old file
	if (hndFileRegOld)
#ifndef REFCOUNT_WORK
		g_FileRegistry.ReleaseRegRef(hndFileRegOld);
#else
	hndFileRegOld->ReleaseFRHRef();
#endif

	return m_hndFileReg != NULL;
}

GPT CDependencyFile::GetStrProp(UINT idProp, CString& val)
{
	// is this a fake prop?
	switch (idProp)
	{
		case P_ProjItemName:	// yes
			if (m_fDisplayName_Dirty)
			{
				ASSERT(m_hndFileReg != NULL);
				m_strDisplayName = (g_FileRegistry.GetRegEntry(m_hndFileReg)->GetFilePath())->GetFileName ();
				m_fDisplayName_Dirty = FALSE;
			}
			val = m_strDisplayName;
			return valid;

		case P_ProjItemFullPath:
			ASSERT(m_hndFileReg != NULL);
			val = (const TCHAR*) *g_FileRegistry.GetRegEntry(m_hndFileReg)->GetFilePath();
			return valid;

		case P_ProjItemDate:
			{
				FILETIME ft;
				CTime time;

				ASSERT(m_hndFileReg != NULL);
				if (g_FileRegistry.GetRegEntry(m_hndFileReg)->GetFileTime(ft))
				{		 
					time = ft;
					CString strDate = theApp.m_CPLReg.Format(time, DATE_ALL);
					CString strTime = theApp.m_CPLReg.Format(time, TIME_ALL);
					val = strTime + _TEXT(" ") + strDate;
				}
				else
					VERIFY(val.LoadString(IDS_FILE_ITEM_NONEXISTANT));
				return valid;
			}
	}

	// no, pass on to base class
	return CProjItem::GetStrProp(idProp, val);
}

BOOL CDependencyFile::PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged)
{
	// go down the project from pContainer to find where we should go
	if (fToPrj)
	{
		if (pContainer->IsKindOf(RUNTIME_CLASS(CProjContainer)) &&
			!pContainer->IsKindOf(RUNTIME_CLASS(CDependencyContainer)))
		{
			pContainer = ((CProjContainer *)pContainer)->GetDependencyContainer(TRUE);

			// if no depedency container then we can't do this!
			if (pContainer == (CSlob *)NULL)
				return FALSE;
		}
	}

	// do the base-class thing
	return CProjItem::PreMoveItem(pContainer, fFromPrj, fToPrj, fPrjChanged);
}

void CDependencyFile::SetFileRegHandle(FileRegHandle hndFileReg)
{
	m_fDisplayName_Dirty = TRUE;	// need to refresh this cache

#ifdef REFCOUNT_WORK
	FileRegHandle oldfrh = m_hndFileReg;
#endif

	m_hndFileReg = hndFileReg;
#ifndef REFCOUNT_WORK
	g_FileRegistry.AddRegRef(hndFileReg); 
#else
	hndFileReg->AddFRHRef();
	if (NULL!=oldfrh)
		oldfrh->ReleaseFRHRef();
#endif
}

BOOL CDependencyFile::CanAct(ACTION_TYPE action)
{
	switch (action)
	{
	case act_insert_into_undo_slob:
	case act_delete:
	case act_cut:
		return FALSE;

	case act_drag: // File items can be dragged into CTargetItems and CProjGroups
		return TRUE;

	case act_copy:	// UNDONE: make this work
	case act_paste:
        return FALSE;
	}

	return CProjItem::CanAct(action);
}

BOOL CDependencyFile::SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption)
{
	AddPropertyPage(&g_DepGeneralPage, this);

	if( bSetCaption )
		SetPropertyCaption(CString(MAKEINTRESOURCE(IDS_DEPFILE)));

	return TRUE;
}

void CDependencyFile::FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem)
{
	if (!(fo & flt_ExcludeDependencies))
	{
		fAddItem = TRUE;
		fAddContent = TRUE;
	}
}

BOOL CDependencyFile::SetSlobProp(UINT idProp, CSlob * val)
{
#if 1
	// only interested in container property changes
	if (idProp != P_Container)
		// pass on to the base-class
		return CProjItem::SetSlobProp(idProp, val);

	// old container?
	CSlob * pOldContainer = m_pContainer; 
	if ((val != NULL) && ((val->IsKindOf(RUNTIME_CLASS(CProjGroup))) || (val->IsKindOf(RUNTIME_CLASS(CTargetItem)))))
	{
		CFileItem * pItem = new CFileItem; // review
		pItem->SetFile(GetFilePath());
		if (pItem->SetSlobProp(idProp, val))
		{
			return CProjItem::SetSlobProp(idProp, NULL);
		}
		else
		{
			delete pItem;
			return FALSE;
		}
	}
#endif
	return CProjItem::SetSlobProp(idProp, val);
}
