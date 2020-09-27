// CBuildEngine
//
// Build engine.
//
// [matthewt]
//

#include "stdafx.h"
#pragma hdrstop

#include <utilbld_.h>	// environment
#include "engine.h"		// local header
#include "scanner.h"
#include "depgraph.h"	// dependency graph	
#include "bldslob.h"
#include "vwslob.h"
#include "toolcplr.h"
#include "mrdepend.h"

#include "toollink.h"

extern BOOL bJavaSupported, bJavaOnce; // defined in vproj.cpp
extern BOOL g_bHTMLLog;

IMPLEMENT_DYNAMIC(CActionSlobList, CObList);
IMPLEMENT_DYNAMIC(CBuildEngine, CObject);
IMPLEMENT_DYNAMIC(CActionSlob, CSlob);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
//
// Class that is used to compare the base configuration of 
// a ConfigurationRecord* or CProjItem*.
//
class BaseConfiguration
{
public:
	BaseConfiguration( const ConfigurationRecord* pcr )
	{
		if ( NULL == pcr )
			m_pBaseRecord = NULL;
		else
			m_pBaseRecord = pcr->m_pBaseRecord;
	}
	BaseConfiguration( CProjItem* pItem )
	{
		if ( NULL == pItem )
			m_pBaseRecord = NULL;
		else
			m_pBaseRecord = pItem->GetActiveConfig()->m_pBaseRecord;
	}

	const ConfigurationRecord* m_pBaseRecord;
};

BOOL SameBaseConfig( BaseConfiguration a, BaseConfiguration b)
{
	//
	// return true if either is NULL since this is usually within an ASSERT.
	// NULL conditions for a ProjItem* or ConfigurationRecord* should use a
	// separate assertion.
	//
	if ( NULL == a.m_pBaseRecord || NULL == b.m_pBaseRecord )
		return TRUE;
	else
		return a.m_pBaseRecord == b.m_pBaseRecord;
}
#endif

// Try to find a tool in an action in our list
POSITION CActionSlobList::Find(CBuildTool * pTool)
{
	POSITION pos = GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		POSITION posCurr = pos;
		CActionSlob * pAction = (CActionSlob *)GetNext(pos);
		if (pTool == pAction->BuildTool())
			return posCurr;
	}

	return (POSITION)NULL;
}


#ifdef _DEBUG
void CActionSlobList::Dump(CDumpContext & dc) const
{
	// dump our actions
	int i = 0;
	dc << "Action List:\n";

	if (IsEmpty())
	{
		dc << "\tEmpty\n";
	}
	else
	{
		POSITION pos = GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CActionSlob * pAction = (CActionSlob *)GetNext(pos); 
			CBuildTool * pTool = pAction->BuildTool();

			dc << "\t";
			dc << i;
			dc << " : ";
			dc << *pTool->GetUIName();
			dc << " (";
			dc << (DWORD)pAction;
			dc << ")\n";

			i++;
		}
	}
}
#endif

BEGIN_INTERFACE_MAP(CEnumActions,CCmdTarget)
	INTERFACE_PART(CEnumActions, IID_IEnumBSActions, EnumActions )
END_INTERFACE_MAP()

BEGIN_INTERFACE_MAP(CBSAction,CCmdTarget)
	INTERFACE_PART(CBSAction, IID_IBSAction, Action )
END_INTERFACE_MAP()

// Our global build regstry map
CMapPtrToPtr g_mapBldRegistry;

typedef struct tagMEMFILE
{
	HANDLE	        hFile;
	HANDLE	        hMap;
	const TCHAR *	pcFileMap;
} MEMFILE;			  

// our instance of the build engine
CBuildEngine AFX_DATA_EXPORT g_buildengine;

// List of target references that we want to AddRefFileItem on when
// reading in a makefile. We have to defer these till we have read in
// the entire makefile, as we need to be able to get the target files
// of all targets to do this (potentially).
CPtrList g_lsttargref;

const TCHAR CActionSlob::szDefVCFile[] = _TEXT("vc60");

// our action marker
WORD CActionSlob::m_wActionMarker = 0;
BOOL CActionSlob::s_bIgnoreCachedOptions = TRUE;

CActionSlob::CActionSlob(CProjItem * pItem, CBuildTool * pTool, BOOL fBuilder, ConfigurationRecord * pcr)
	: m_frsInput(37), m_frsOutput(7), m_frsSchmoozableOutput(7)
{
	ASSERT( SameBaseConfig( pcr, pItem ) );
	// ASSERT((fBuilder) || (!pItem->IsKindOf(RUNTIME_CLASS(CProjContainer))) || (!pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool))));

	// default
	m_pIncludeList = (CIncludeEntryList *)NULL;
//	m_frsScannedDep = m_frsMissingDep = m_frsSourceDep = (CFileRegSet *)NULL;
	m_fBatchNotify = m_fInGenOutput = FALSE;
	m_wMark = 0; m_dwData = 0;

	// REVIEW: unused?
	m_dwEvent = 0; m_state = _Unknown;

	pCBSAction = NULL;
	pIAction = NULL;

	// associate
	m_pItem = pItem;
	// ASSERT(!pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)));
	m_pcrOur = pcr;

	m_pTool = m_pOldTool = (CBuildTool *)NULL;
	m_fBuilder = fBuilder;
	m_pTrgAction = (CActionSlob *)NULL;
	m_frhInput = (FileRegHandle)NULL;

	// registry & dep.graph??
	if (m_fBuilder && m_pcrOur != (ConfigurationRecord *)NULL)
	{
		m_pregistry = g_buildengine.GetRegistry(m_pcrOur);
		m_pdepgraph = g_buildengine.GetDepGraph(m_pcrOur);
	}
	else
	{
		m_pregistry = &g_FileRegistry;
		m_pdepgraph = (CFileDepGraph *)NULL;
	}

	// set up our input and output to use this target registry
#ifndef REFCOUNT_WORK
	m_frsInput.SetFileRegistry(m_pregistry);
	m_frsOutput.SetFileRegistry(m_pregistry);
#else
	m_frsSchmoozableOutput.SetFileRegistry(m_pregistry);
#endif

	// FUTURE: For now we don't care about the incr. update
	// of input and output properties, eg. timestamp, exist on disk etc.

	// This was the review comment but I don't really understand these sets
	// completely and there is no way in hell I am changing anything about this
	// two days before building the release candidate. I am changing this to FUTURE
	// and we should understand this review comment and fix if neccessary for 4.1

	// REVIEW: (KPerry) As agreed by KPerry, Brick B, and Bjoyce, this is an experiment to fix
	// V6.0 bug #22343, "deletion of outputs not being tracked".
	// Unlike the person above, I do understand these guys, and know that the
	// only ones that need tracking is the depgraph sets, not this one.
	m_frsOutput.CalcAttributes(TRUE);
	m_frsInput.CalcAttributes(FALSE);
#ifdef REFCOUNT_WORK
	m_frsSchmoozableOutput.CalcAttributes(FALSE);
#endif

	// bind to our tool?
	if (pTool != (CBuildTool *)NULL)
	{
		// inform of created action then perform bind
		pTool->OnActionEvent(ACTEVT_Create, this);
		Bind(pTool);
	}

	// assume that our settings are ok, but dirty
	m_fDirtyCommand = TRUE;
	m_fPossibleOptionChange = FALSE;

	m_wMarkRefCmdOpt = 0;

	m_LastDepUpdate.dwLowDateTime = 0;
	m_LastDepUpdate.dwHighDateTime = 0;
}

CActionSlob::~CActionSlob()
{
	// clean up our source, missing and scanned deps.
	ClearSourceDep();
	ClearMissingDep();
	ClearScannedDep();


	// unbind from the tool?
	if (m_pTool != (CBuildTool *)NULL)
	{
		// remember if we had a tool
		// so we can use this info. after UnBind()
		CBuildTool * pTool = m_pTool;

		// perform unbind then inform of destroyed action
		pTool->OnActionEvent(ACTEVT_Destroy, this);
		UnBind();
	}


	// delete any objects we may have alloc'd
#ifndef REFCOUNT_WORK
	if (m_pfrsSourceDep != (CFileRegSet *)NULL)
		m_pfrsSourceDep->ReleaseFRHRef();

	if (m_pfrsMissingDep != (CFileRegSet *)NULL)
		m_pfrsMissingDep->ReleaseFRHRef();

	if (m_pfrsScannedDep != (CFileRegSet *)NULL)
		m_pfrsScannedDep->ReleaseFRHRef();
#else
#endif
}

// Binding and un-binding to tools
void CActionSlob::Bind(CBuildTool * pTool)
{
	// Set our tool
	SetToolI((CBuildTool *)pTool);

	// update our output
	if (m_fBuilder && m_pcrOur != NULL)
		OnInform(m_pItem, P_ToolOutput, 0);
}

void CActionSlob::UnBind()
{
	// disable this as we may potential change *a lot* of outputs
	// (we don't want the feedback just yet anyway, it'll mess up our
	//  dependency graph as it is being constructed)
	BOOL g_oldFileRegDisableFC = g_FileRegDisableFC;
	g_FileRegDisableFC = TRUE;

	// un-bind our source deps
	ClearSourceDep();

	// nuke our other deps
	ClearScannedDep();
	ClearMissingDep();

	// clean up our input & output file/set references
	ClearOutput();
	ClearInput();

	// set our tool					   
	SetToolI((CBuildTool *)NULL);

	// ensure that our deps are using the global registry
	// ie. detach from the current action's target registry
#ifndef REFCOUNT_WORK
	if (m_pfrsSourceDep != (CFileRegSet *)NULL)
		m_pfrsSourceDep->SetFileRegistry(&g_FileRegistry);

	if (m_pfrsMissingDep != (CFileRegSet *)NULL)
		m_pfrsMissingDep->SetFileRegistry(&g_FileRegistry);

	if (m_pfrsScannedDep != (CFileRegSet *)NULL)
		m_pfrsScannedDep->SetFileRegistry(&g_FileRegistry);
#else
#if 0 // review(tomse): Useless code?
	// File sets no longer need registry.
	if (m_pfrsSourceDep != (CFileRegSet *)NULL)
		m_pfrsSourceDep->SetFileRegistry(NULL);

	if (m_pfrsMissingDep != (CFileRegSet *)NULL)
		m_pfrsMissingDep->SetFileRegistry(NULL);

	if (m_pfrsScannedDep != (CFileRegSet *)NULL)
		m_pfrsScannedDep->SetFileRegistry(NULL);
#endif
#endif

	// re-enable
	g_FileRegDisableFC = g_oldFileRegDisableFC;
}

// Notification of input or output changing
// Rx for file change events
void CActionSlob::OnNotify(FileRegHandle hndFileReg, UINT idChange, DWORD dwHint)
{
	// interested in this?
#ifndef REFCOUNT_WORK
	if (!(idChange == FRI_ADD || idChange == FRI_DESTROY || idChange == FRI_LCL_DESTROY))
#else
	if (!(idChange == FRI_ADD || idChange == FRI_DESTROY))
#endif
		return;	// no

	// ignore due to batching?
	if (m_fBatchNotify)
		return;	// yes
												
	// convert hint into a file registry handle
	FileRegHandle frhHint = (FileRegHandle)dwHint;
									    
	// must be concerning our 'possible' input!
	ASSERT(hndFileReg == m_frhInput);

	// valid input?
	if (m_pTool->m_fTargetTool && !ValidInput(frhHint))
		return;

	// add/remove these input dependencies to/from the graph
	if (m_pdepgraph != NULL)
		VERIFY(m_pdepgraph->ModifyDep(frhHint, this, DEP_Input, idChange == FRI_ADD /* add or remove? */));

	// FUTURE(briancr, sanjays): I've fixed this bug for now with this 3
	// line hack.
	// When a .def file is added/deleted to the project, we need to tell
	// the CActionSlob for the link step that the options
	// may have changed, since adding or removing a .def file changes the linker
	// options (something no other file addition/removal does). The CActionSlob
	// caches the command line options and will only update them if told they
	// may have changed.
	// There are some questions I have with this code:
	// - Is this the correct place to make the determination that the options are dirty?
	// - Will this mark all actions as dirty when a .def file is added (I assume so)?
	// olympus: 1256 (briancr)
	// addition/removal of def file changes options
	if (_tcsicmp((frhHint->GetFilePath())->GetExtension(), _T(".def")) == 0) {
		m_fPossibleOptionChange = TRUE;
	}

	// dirty input
	m_fDirtyInput = TRUE;

	// inform item dependents of input change
	m_pItem->InformDependants(P_ItemInput);

#ifdef _DEBUG
	// show graph dbg. info?
	if (m_pTool->m_dwDebugging & DBG_BT_NOT)
	{
		const TCHAR * pchConfigName = m_pcrOur ? (const TCHAR *)m_pcrOur->GetConfigurationName() : "<Unspecified>";
		const TCHAR * pchToolName = (const TCHAR *)*m_pTool->GetName();

		TRACE("\r\nBldSys : Tool '%s', config. '%s'", pchToolName, pchConfigName);

		if (idChange == FRI_ADD)
			TRACE(" add input");

		else
			TRACE(" delete input");

		TRACE("\r\n"); m_pregistry->GetRegEntry(frhHint)->Dump(afxDump);
	}
#endif
}

// Notification of output changing
// Rx for property change events, eg. we're interested in P_ToolOutput
void CActionSlob::OnInform(CSlob * pChangedSlob, UINT idChange, DWORD dwHint)
{
	// is this the output?
	// and does it affect us?
	if (idChange == P_ToolOutput || idChange == P_ItemExcludedFromBuild)	// output?
	{
		// us?
		BOOL fUs = FALSE;

		// HACK: if this is a tool then take the hint as to which is the changed slob
		if (pChangedSlob->IsKindOf(RUNTIME_CLASS(CBuildTool)))
			pChangedSlob = (CSlob *)dwHint;

		ASSERT(pChangedSlob->IsKindOf(RUNTIME_CLASS(CProjItem)));
		ASSERT(!pChangedSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));

		// is this us, or our target (in v3 settings on project)?
		fUs = ((CProjItem *)pChangedSlob == m_pItem ||
				(CProjItem *)pChangedSlob == m_pItem->GetTarget() ||
				(CProjItem *)pChangedSlob == m_pItem->GetProject()) &&
				((CProjItem *)pChangedSlob)->GetActiveConfig()->m_pBaseRecord == m_pcrOur->m_pBaseRecord;

		if (!fUs)
			return;	// not us ... so ignore

		// it's us so refresh output
		RefreshOutputI();
	}
	// is this the command-line?
	else if (idChange == P_ToolCommand)
	{
		// force a refresh of our command-line options
		// FOR NOW ignore m_pcrOur == NULL
		if (m_pcrOur != NULL)
		{
			m_fPossibleOptionChange = TRUE;
			
			BOOL fDummy;
			(void) RefreshCommandOptions(fDummy);

			m_fPossibleOptionChange = FALSE;
		}
	}
}

void CActionSlob::AddInput()
{
	// FOR NOW ignore m_pcrOur == NULL
	if (!m_fBuilder || m_pcrOur == NULL)
		return;

	// no tool, no input to add!
	if (m_pTool == (CBuildTool *)NULL)
		return;

	// create the input set or file
	if (m_pTool->m_fTargetTool)
	{
		// 'possible' input is a file filter
		CString strFilter;
		if (m_pTool->GetStrProp(P_ToolInput, strFilter) == valid)
		{
#ifndef REFCOUNT_WORK
			if ((m_frhInput = m_pregistry->LookupFileFilter((const CString *)&strFilter)) == (FileRegHandle)NULL)
				m_frhInput = m_pregistry->RegisterFileFilter((const CString *)&strFilter);		
			else
				m_pregistry->AddRegRef(m_frhInput);
#else
			m_frhInput = m_pregistry->RegisterFileFilter((const CString *)&strFilter);		
#endif

#ifdef _DEBUG_BLD
			TRACE("\r\nFile filter <%s>:%lx in %lx", (const TCHAR *)strFilter, m_frhInput, m_pregistry);
#endif // _DEBUG_BLD
		}
	}
	else
	{	
		const CPath * pPath;
		BOOL bIsProject = (m_pItem->IsKindOf(RUNTIME_CLASS(CProject)) || (m_pItem->IsKindOf(RUNTIME_CLASS(CTargetItem))));

		// 'possible' input is the file itself
		if (bIsProject)
		{
			pPath = m_pItem->GetProject()->GetTargetFileName();
		}
		else
		{
			pPath = m_pItem->GetFilePath();
		}

		if (pPath != (const CPath *)NULL)
		{
#ifndef REFCOUNT_WORK
			if ((m_frhInput = m_pregistry->LookupFile(*pPath)) == (FileRegHandle)NULL)
				m_frhInput = m_pregistry->RegisterFile(pPath);		
			else
				m_pregistry->AddRegRef(m_frhInput);
#else
			m_frhInput = m_pregistry->RegisterFile(pPath);		
#endif

#ifdef _DEBUG_BLD
			const TCHAR * pchFile = (const TCHAR *)*m_pregistry->GetRegEntry(m_frhInput)->GetFilePath();
			TRACE("\r\nInput <%s>:%lx in %lx", pchFile, m_frhInput, m_pregistry);
#endif // _DEBUG_BLD
		}

		if (bIsProject)
		{
			delete (CPath*)pPath;
		}
	}
}

void CActionSlob::RemoveInput()
{
	// FOR NOW ignore m_pcrOur == NULL
	if (!m_fBuilder || m_pcrOur == NULL)
		return;

	// no tool or no input to remove?
	if (m_pTool == (CBuildTool *)NULL || m_frhInput == (FileRegHandle)NULL)
		return;

	// clean up our 'possible' input file/set references
#ifndef REFCOUNT_WORK
	m_pregistry->ReleaseRegRef(m_frhInput);
#else
	if (m_frhInput->IsNodeType(nodetypeRegFile))
		m_pregistry->ReleaseRegRef(m_frhInput);
	else
		m_frhInput->ReleaseFRHRef();
#endif
}

BOOL CActionSlob::ValidInput(FileRegHandle frhIn)
{
	return !m_frsOutput.RegHandleExists(frhIn);	// ok?
}

// FUTURE: remove this
void CActionSlob::SetToolI(CBuildTool * pTool)
{
	COptionHandler * popthdlr;

	if (m_pTool != (CBuildTool *)NULL)
	{
		// remove ourselves as a dependent of
		// the outputs of this tool
		popthdlr = m_pTool->GetOptionHandler();
		while (popthdlr != (COptionHandler *)NULL)
		{
			CBuildTool * pTool = popthdlr->m_pAssociatedBuildTool;
			if (pTool != (CBuildTool *)NULL)
				pTool->RemoveDependant(this);
			popthdlr = popthdlr->GetBaseOptionHandler();
		}

		// make sure we *don't* get input change events
		if (m_frhInput != (FileRegHandle)NULL)
			m_frhInput->RemoveNotifyRx(this);

		// remove the input for the old tool
 		RemoveInput();
	}
								   
	// add our input for this new tool?
	m_pOldTool = m_pTool;
	if ((m_pTool = pTool) != (CBuildTool *)NULL)
	{
		AddInput();

		// make sure we get 'possible' input and output change events
		if (m_frhInput != (FileRegHandle)NULL)
			m_frhInput->AddNotifyRx(this);

		// make ourselves a dependent on
		// the outputs of this tool
		popthdlr = m_pTool->GetOptionHandler();
		while (popthdlr != (COptionHandler *)NULL)
		{
			CBuildTool * pTool = popthdlr->m_pAssociatedBuildTool;
			if (pTool != (CBuildTool *)NULL)
				pTool->AddDependant(this);
			popthdlr = popthdlr->GetBaseOptionHandler();
		}
	}

	// input and output is dirty
	m_fDirtyInput = TRUE;
	m_fDirtyOutput = TRUE;
}


void CActionSlob::Serialize(CArchive & ar)
{
	// don't bother with base-class
	// FUTURE: if we are interested in CSlob
	// archiving we may do this.
	 
	// storing?
	if (ar.IsStoring())
	{
#if 0			// NEVER do this anymore
		// unknown tool?
		if (m_pTool->IsSupported())
		{
			ar << ((BYTE)TRUE); // yes

#if 0			// NEVER do this anymore
			// dirty command-line?
			if (!m_fDirtyCommand)
			{
				ar << ((BYTE)FALSE); // no

				// possible option change + current command-line
				ar << ((BYTE)m_fPossibleOptionChange);
				ar << m_strOptions;
			}
			else
#endif
			{
				ar << ((BYTE)TRUE); // yes
			}
		}
		else
		{
			ar << ((BYTE)FALSE); // no
		}
#endif
	}
	else
	{
		// unknown tool?
		BOOL fIsSupported = FALSE;
		ar >> ((BYTE &)fIsSupported);

		// skip can we read this?
		if (!fIsSupported)
		{
			CString strDummy;
			BOOL fDummy = FALSE;

			ar >> ((BYTE &)fDummy);
			ar >> strDummy;
		}
		else if (fIsSupported)
		{
			// dirty command-line?
			BOOL fDirtyCommand = FALSE;
						
			ar >> ((BYTE &)fDirtyCommand);

			if (!s_bIgnoreCachedOptions)
				m_fDirtyCommand = fDirtyCommand;

			if (!fDirtyCommand)
			{
				// possible option change + current command-line
				m_fCmdOptChanged = TRUE;
				BOOL fPossibleOptionChange = FALSE;
				ar >> ((BYTE &)fPossibleOptionChange);

				if (s_bIgnoreCachedOptions)
				{
					CString strDummy;
					ar >> strDummy;
				}
				else 
				{
					m_fPossibleOptionChange = fPossibleOptionChange;
					ar >> m_strOptions;
				}
			}
		}
	}
}

BOOL CActionSlob::LoadFromArchiveVCP(CArchive & ar)
{
	//return TRUE;	// do nothing, buggy still!
	
	ASSERT(!ar.IsStoring());	// sanity check

	// get the builder
	CProject * pProject = g_pActiveProject;
	ASSERT(pProject != (CProject *)NULL);

	TRY
	{
		// get our old configuration
		ConfigurationRecord * pcrOld = pProject->GetActiveConfig();
		CProject * pProjectOld = pProject;
		
		// action slob list
		CActionSlobList * pActions;
		CActionSlob * pAction;
		POSITION posAction; 

		CString strTargetName, strFileName;
		CTargetItem * pTarget;

		// read in our target names
		ar >> strTargetName;
		while (!strTargetName.IsEmpty())
		{	
			// get the target for this
			// UNDONE: this isn't valid for subprojects on conversion:
			pTarget = pProject->GetTarget(strTargetName);
			if (pTarget == (CTargetItem *)NULL)
			{
				pTarget = (CTargetItem *)g_BldSysIFace.GetFileSetFromTargetName(strTargetName, NO_BUILDER);
				if (pTarget != NULL)
				{
					pProject = pTarget->GetProject();
				}
				else
				{
					return FALSE;	// problem! <- target item not found
				}
			}

			// shouldn't have unknown project types
			CProjType * pProjType = pTarget->GetProjType();
			if (!pProjType->IsSupported())
				return FALSE;	// problem! <- unknown target found

			// get the target's file registry
			CFileRegistry * pRegistry = pTarget->GetRegistry();
			ASSERT(pRegistry != NULL);

			// set us in the right config.
			CProjTempConfigChange projTempConfigChange(pProject);
			projTempConfigChange.ChangeConfig(strTargetName);

			// read in 'project-level'
			pActions = pProject->GetActiveConfig()->GetActionList();
			WORD cActions;
			ar >> (WORD)cActions;    // number of actions to read
			posAction = pActions->GetHeadPosition();
			while (cActions && posAction != (POSITION)NULL)
			{
				pAction = (CActionSlob *)pActions->GetNext(posAction);
				// ignore unknown tool
				if (pAction && pAction->m_pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)))
					continue;
				pAction->Serialize(ar);
				cActions--;
			}

			// get the files for this
			ar >> strFileName;
			while (!strFileName.IsEmpty())
			{
				CProjItem * pItem = NULL;

				// get the file for this

				// check to see if the file is in the target
				FileRegHandle frh = pRegistry->LookupFile(strFileName);
				if (frh == NULL)
					return FALSE;	// problem! <- not in target!

				// the file is there, so now get the associated list of
				// file items for this file reg entry, and if there is
				// no list then this item is not really part of the target
				// (i.e. it is a dependency, or an intermediate target)
				CObList * oblist = g_FileRegistry.GetRegEntry(frh)->plstFileItems;
				if (oblist == NULL)
					return FALSE;	// problem! <- no file-items

				POSITION pos = oblist->GetHeadPosition();
				while (pos != NULL)
				{
					// Get the file item and its parent
					CFileItem * pFile = (CFileItem *)oblist->GetNext(pos);
					CTargetItem * pContainer = (CTargetItem *)pFile->GetContainer();

					// Is this file item in our target, if so we've found the file
					if (pContainer == pTarget)
					{
						pItem = pFile;
						break;
 					}
				}

				// found file?
				if (pItem == (CProjItem *)NULL)
					return FALSE;	// problem! <- no file-item

#if 0
				// read in 'file-level'
				pActions = pItem->GetActiveConfig()->GetActionList();
				posAction = pActions->GetHeadPosition();
				
				while (posAction != (POSITION)NULL)
				{
					pAction = (CActionSlob *)pActions->GetNext(posAction);
					pAction->Serialize(ar);
				}
#else
				// assume exactly one action (could fail if none)
				CActionSlob tempAction(NULL, NULL, FALSE, NULL);
				tempAction.Serialize(ar);
#endif

				// next file
				ar >> strFileName;
			}

			// next target
			ar >> strTargetName;
 		}

	}
	CATCH_ALL (e)
	{
		// failed
		return FALSE;
	}
	END_CATCH_ALL

	return TRUE; // ok
}

BOOL CActionSlob::LoadFromArchive(CArchive & ar)
{
	//return TRUE;	// do nothing, buggy still!
	
	ASSERT(!ar.IsStoring());	// sanity check

	// get the builder
	CProject * pProject = g_pActiveProject;
	ASSERT(pProject != (CProject *)NULL);

	TRY
	{
		// get our old configuration
		ConfigurationRecord * pcrOld = pProject->GetActiveConfig();
		
		// action slob list
		CActionSlobList * pActions;
		CActionSlob * pAction;
		POSITION posAction; 

		CString strTargetName, strFileName;
		CTargetItem * pTarget;

		// read in our target names
		ar >> strTargetName;
		while (!strTargetName.IsEmpty())
		{	
			// get the target for this

			HBLDTARGET hTarget = g_BldSysIFace.GetTarget(strTargetName, ACTIVE_BUILDER);
			pProject = (CProject*)g_BldSysIFace.GetBuilder(hTarget);
			if (pProject == (CProject *)NULL)
				return FALSE;	// problem! <- target item not found
			pTarget = pProject->GetTarget(strTargetName);
			if (pTarget == (CTargetItem *)NULL)
				return FALSE;	// problem! <- target item not found

			// get the target's file registry
			CFileRegistry * pRegistry = pTarget->GetRegistry();
			ASSERT(pRegistry != NULL);

			// set us in the right config.
			CProjTempConfigChange projTempConfigChange(pProject);
			projTempConfigChange.ChangeConfig(strTargetName);

			// shouldn't have unknown project types
			CProjType * pProjType = pTarget->GetProjType();
			if (!pProjType->IsSupported())
				return FALSE;	// problem! <- unknown target found

			// read in 'project-level'
			pActions = pProject->GetActiveConfig()->GetActionList();
			WORD cActions;
			ar >> (WORD)cActions;    // number of actions to read
			posAction = pActions->GetHeadPosition();
			while (cActions && posAction != (POSITION)NULL)
			{
				pAction = (CActionSlob *)pActions->GetNext(posAction);
				// ignore unknown tool
				if (pAction && pAction->m_pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)))
					continue;
				pAction->Serialize(ar);
				cActions--;
			}

			// get the files for this
			ar >> strFileName;
			while (!strFileName.IsEmpty())
			{
#if 0
				if (strFileName.GetLength() < 2)
				{
					ASSERT(0);
					ar >> strFileName;
					continue;  // skip junk
				}
#endif

				CProjItem * pItem = NULL;

				// get the file for this

				// check to see if the file is in the target
				FileRegHandle frh = pRegistry->LookupFile(strFileName);
				if (frh == NULL)
				{
					// break;
					return FALSE;	// problem! <- not in target!
				}

				// the file is there, so now get the associated list of
				// file items for this file reg entry, and if there is
				// no list then this item is not really part of the target
				// (i.e. it is a dependency, or an intermediate target)
				CObList * oblist = g_FileRegistry.GetRegEntry(frh)->plstFileItems;
#ifdef REFCOUNT_WORK
				frh->ReleaseFRHRef();
#endif
				if (oblist == NULL)
					return FALSE;	// problem! <- no file-items

				POSITION pos = oblist->GetHeadPosition();
				while (pos != NULL)
				{
					// Get the file item and its parent
					CFileItem * pFile = (CFileItem *)oblist->GetNext(pos);
					CTargetItem * pContainer = pFile->GetTarget();

					// Is this file item in our target, if so we've found the file
					if (pContainer == pTarget)
					{
						pItem = pFile;
						break;
 					}
				}

				// found file?
				if (pItem == (CProjItem *)NULL)
					return FALSE;	// problem! <- no file-item

				// read in 'file-level'
				pActions = pItem->GetActiveConfig()->GetActionList();
				posAction = pActions->GetHeadPosition();
				while (posAction != (POSITION)NULL)
				{
					pAction = (CActionSlob *)pActions->GetNext(posAction);
					pAction->Serialize(ar);
				}

				// next file
				ar >> strFileName;
			}

			// next target
			ar >> strTargetName;
 		}

	}
	CATCH_ALL (e)
	{
		// failed
		return FALSE;
	}
	END_CATCH_ALL

	return TRUE; // ok
}

#if 0
BOOL CActionSlob::SaveToArchive(CArchive & ar)
{
	//return TRUE;	// do nothing, buggy still!

	ASSERT(ar.IsStoring());	// sanity check

	// get the builder
	CProject * pProject = g_pActiveProject;
	ASSERT(pProject != (CProject *)NULL);

	TRY
	{
		// get our old configuration
		CProjTempConfigChange projTempConfigChange(pProject);

		// action slob list
		CActionSlobList * pActions;
		CActionSlob * pAction;
		POSITION posAction; 

		CString strTargetName;
		CTargetItem * pTarget;

		CString str;

		// loop through all projects (CTargetItem) of the builder (CProject)
		pProject->InitTargetEnum();
		while (pProject->NextTargetEnum(strTargetName, pTarget))
		{
			// set us in the right config.
			projTempConfigChange.ChangeConfig(strTargetName);

			// ignore unknown project types
			CProjType * pProjType = pTarget->GetProjType();
			if (!pProjType->IsSupported())
				continue;	// don't write these out

			// write out the target name
			ar << strTargetName;


			// write out 'project-level'
			pActions = pProject->GetActiveConfig()->GetActionList();
			WORD cActions = pActions->GetCount();
			// find out actual count, ignoring unknown tool
			posAction = pActions->GetHeadPosition();
			while (posAction != (POSITION)NULL)
			{
				pAction = (CActionSlob *)pActions->GetNext(posAction);
				// ignore unknown tool
				if (pAction && pAction->m_pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)))
					cActions--;					
			}
			ar << (WORD) cActions;
			posAction = pActions->GetHeadPosition();
			
			while (posAction != (POSITION)NULL)
			{
				pAction = (CActionSlob *)pActions->GetNext(posAction);
				// ignore unknown tool
				if (pAction && pAction->m_pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)))
					continue;
				// BOOL fChanged;
				// pAction->RefreshCommandOptions(fChanged);
					pAction->Serialize(ar);
			}

			// enumerate files in this target
			CObList * plstContent;
			CObList lstContent;
			pTarget->FlattenSubtree(lstContent, (CProjItem::flt_Normal | CProjItem::flt_ExcludeDependencies | CProjItem::flt_ExcludeProjects | CProjItem::flt_ExcludeGroups));
			plstContent = &lstContent;

			POSITION pos = plstContent->GetHeadPosition();
			while (pos != (POSITION)NULL)
			{
				CProjItem * pItem = (CProjItem *)plstContent->GetNext(pos);

				// file item?
				if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
				{
					// write out the file-item name
					str = (const TCHAR *)*pItem->GetFilePath();
					ar << str;

					// write out 'file-level'
					pActions = pItem->GetActiveConfig()->GetActionList();
					posAction = pActions->GetHeadPosition();
					while (posAction != (POSITION)NULL)
					{
						pAction = (CActionSlob *)pActions->GetNext(posAction);
						// ignore unknown tool
						if ((!pAction) || (pAction->m_pTool==NULL) || (pAction->m_pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool))))
							continue;
						BOOL fExcluded;
						// pAction->RefreshCommandOptions(fChanged);
						if (pItem->GetIntProp(P_ItemExcludedFromBuild, fExcluded) == valid && !fExcluded)
							pAction->Serialize(ar);
					}
				}
			}

			// empty file name is end of files marker
			str = _TEXT("");
			ar << str;
		}
		

		// empty target name is end of targets marker
		str = _TEXT("");
		ar << str;

	}
	CATCH_ALL (e)
	{
		// failed
		return FALSE;
	}
	END_CATCH_ALL

	return TRUE; // ok
}

#endif



void CActionSlob::BindActions
(
	CProjItem * pTheItem,
	CPtrList * plstSelectTools /* = NULL */,
	ConfigurationRecord * pcr /* = (ConfigurationRecord *)NULL */,
	BOOL fBindContained /* = TRUE */
)
{
	ASSERT(pTheItem != NULL);
	CTargetItem * pTarget = pTheItem->GetTarget();
	if (pTarget == (CTargetItem *)NULL)
		return;

	// Set the project config.?
	CProjTempConfigChange projTempConfigChange(pTheItem->GetProject());

	if (pcr != (ConfigurationRecord *)NULL)
	{
		projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);
	}
	else {
		pcr = pTheItem->GetProject()->GetActiveConfig();
	}

	// valid?
	if (pTarget == (CTargetItem *)NULL || pTarget->GetActiveConfig()->IsValid())
	{
		CProjType * pProjType = pTarget->GetProjType();

		// Buildable project type?
		BOOL fBuildableProjType = !pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeUnknown));

		// Get the target type tools list
		CPtrList * plstTools = plstSelectTools == (CPtrList *)NULL ? pProjType->GetToolsList() : plstSelectTools;

		// Content?
		CObList * plstContent;
		CObList lstContent;
		pTheItem->FlattenSubtree(lstContent, (CProjItem::flt_Normal | CProjItem::flt_ExcludeDependencies | CProjItem::flt_ExcludeProjects | CProjItem::flt_ExcludeGroups));
		plstContent = &lstContent;

		POSITION posItem = fBindContained && (plstContent != (CObList *)NULL) ? plstContent->GetHeadPosition() : (POSITION)NULL;
		
		CProjItem * pItem = pTheItem;
		while (pItem != (CProjItem *)NULL)
		{
 			// don't do this for dependency files or folders!
			if (!pItem->IsKindOf(RUNTIME_CLASS(CDependencyFile)) &&
				!pItem->IsKindOf(RUNTIME_CLASS(CDependencyContainer)) &&
                !pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency))
			   )
			{
				BOOL bItemIgnoreDefaultTool = FALSE;
				BOOL bIsFileItem = (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)));
#ifndef REFCOUNT_WORK
				// If binding 'en masse' then make file itself part of the
				// dependency graph
				if (plstSelectTools == (CPtrList *)NULL)
				{
					// included in build?
					BOOL fExcluded;
					if (pItem->GetIntProp(P_ItemExcludedFromBuild, fExcluded) == valid && !fExcluded)
						AddRefFileItem(pItem);
				}
				BOOL fIgnoreTool;
				bItemIgnoreDefaultTool = (pItem->GetIntProp(P_ItemIgnoreDefaultTool, fIgnoreTool) == valid && fIgnoreTool);
#else
				if (bIsFileItem)
				{
					if (NULL==plstSelectTools)
					{
						BOOL fExcluded;
						if (pItem->GetIntProp(P_ItemExcludedFromBuild, fExcluded) == valid && !fExcluded)
						{
							ASSERT( NULL!=pcr );
							// Add to dependency graph as possible input to schmooze tool.
							CFileRegistry *preg = g_buildengine.GetRegistry(pcr);
							ASSERT(NULL!=preg);
							FileRegHandle frh = pItem->GetFileRegHandle();
							ASSERT(NULL!=frh);
							preg->AddRegRef(frh);
//							frh->ReleaseFRHRef();
						}
					}
					BOOL fIgnoreTool;
					bItemIgnoreDefaultTool = (pItem->GetIntProp(P_ItemIgnoreDefaultTool, fIgnoreTool) == valid && fIgnoreTool);
				}
#endif

				FileRegHandle frh = pItem->GetFileRegHandle();

				// Get the list of actions
				// Put these actions on the project if it's a target
				CProjItem * pItemActions = pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pItem->GetProject() : pItem;
				CActionSlobList * pActions = pItemActions->GetActiveConfig()->GetActionList();
				BOOL fActionsChange = FALSE;

				// Go through them and bind any tools to this item that want to be

				CActionSlob * pAction = NULL;

				CBuildTool * pTool;
				POSITION pos = plstTools->GetHeadPosition();
				while (pos != (POSITION)NULL)
				{
					pTool = (CBuildTool *)plstTools->GetNext (pos);

					// Ignore unknown tools
					if (pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)))
						continue;
					
					// skip default action if custom build tool specified
					if (bItemIgnoreDefaultTool && (!pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool))))
						continue;

					// Allocate a 'building' action to to list?
					if (pTool->AttachToFile(frh, pItem))
					{
						// Do we already have an 'old' action for this tool in our list?
						POSITION pos = pActions->GetHeadPosition();
						BOOL fFound = FALSE;
						while (pos != (POSITION)NULL)
						{
							pAction = (CActionSlob *)pActions->GetNext(pos);

							if (pTool == pAction->m_pOldTool)
							{
								fFound = TRUE;
								break;
							}
						}

						// Re-bind?
						if (fFound)
						{
							// Set our config.
							ASSERT(pAction);
							pAction->m_pcrOur = pcr;
							ASSERT( pAction->m_pItem == pItemActions );
							ASSERT( SameBaseConfig( pcr, pItemActions ) );

							// Re-bind
							pAction->Bind(pTool);
						}
						// Assign!
						else
						{
							// check for multiple assignment
							ASSERT(!pActions->Find(pTool));

							pAction = new CActionSlob(pItem, pTool, fBuildableProjType, pcr);
							pActions->AddTail(pAction);
							fActionsChange = TRUE;
						}
					}
					else
						continue;	// ignore
				}

				// Delete any actions not re-bound to
				pos = pActions->GetHeadPosition();
				while (pos != (POSITION)NULL)
				{
					POSITION posAction = pos;
					pAction = (CActionSlob *)pActions->GetNext(pos);

					// Are we bound to a tool?
					if (pAction->m_pTool == (CBuildTool *)NULL)
					{
						// Remove from list and de-allocate
						pActions->RemoveAt(posAction);
						fActionsChange = TRUE;
						delete pAction;
					}
				}

				// Actions changed?
				if (fActionsChange)
				{
					// inform item dependents of output change
					pItem->InformDependants(P_ItemTools);
#ifdef _DEBUG
					if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
					{
						ASSERT(pActions->GetCount() <= 1);
					}
#endif
				}
			}

				do {
					pItem = (CProjItem *)(posItem != (POSITION)NULL ? plstContent->GetNext(posItem) : NULL); 
				} while (pItem == pTheItem); // don't process pTheItem twice!!
		}
	}

/* 
R.I.P. for v4.0 with VBA?
	// We've done it for our regular tools, now global custom tools...?
	if (plstSelectTools == (CPtrList *)NULL)
		CActionSlob::BindActions(pTheItem,
								   g_prjcompmgr.GetCustomToolList(),
								   (ConfigurationRecord *)NULL, fBindContained);
*/

}

void CActionSlob::UnBindActions
(
	CProjItem * pTheItem,
	CPtrList * plstSelectTools /* = NULL */,
	ConfigurationRecord * pcr /* = (ConfigurationRecord *)NULL*/,
	BOOL fUnbindContained /* TRUE */  
)
{
	CTargetItem * pTarget = pTheItem->GetTarget();
	if (pTarget == (CTargetItem *)NULL)
		return;

	// Set the project config.?
	CProjTempConfigChange projTempConfigChange(pTheItem->GetProject());

	if (pcr != (ConfigurationRecord *)NULL)
	{
		projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);
	}
	else
	{
		pcr = pTheItem->GetProject()->GetActiveConfig();
	}

	// valid?
	if (pTarget == (CTargetItem *)NULL || pTarget->GetActiveConfig()->IsValid())
	{
		// Content?
		CObList * plstContent;
		CObList lstContent;
		pTheItem->FlattenSubtree(lstContent, (CProjItem::flt_Normal | CProjItem::flt_ExcludeDependencies | CProjItem::flt_ExcludeProjects | CProjItem::flt_ExcludeGroups));
		plstContent = &lstContent;

		POSITION posItem = fUnbindContained && (plstContent != (CObList *)NULL) ? plstContent->GetHeadPosition() : (POSITION)NULL;
		
		CProjItem * pItem = pTheItem;
		while (pItem != (CProjItem *)NULL)
		{
 			// don't do this for dependency files or folders!
			if (!pItem->IsKindOf(RUNTIME_CLASS(CDependencyFile)) &&
				!pItem->IsKindOf(RUNTIME_CLASS(CDependencyContainer))
			   )
			{
#ifndef REFCOUNT_WORK
				// If unbinding 'en masse' then make file itself part of the
				// dependency graph
				if (plstSelectTools == (CPtrList *)NULL)
				{
					// included in build?
					BOOL fExcluded;
					if (pItem->GetIntProp(P_ItemExcludedFromBuild, fExcluded) == valid && !fExcluded)
						ReleaseRefFileItem(pItem);
				}
#else
				if (NULL==plstSelectTools && pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
				{
					BOOL fExcluded;
					if (pItem->GetIntProp(P_ItemExcludedFromBuild, fExcluded) == valid && !fExcluded)
					{
						// Remove from dependency graph as possible input to schmooze tool.
						ASSERT( NULL!=pcr );
						// Add to dependency graph as possible input to schmooze tool.
						CFileRegistry *preg = g_buildengine.GetRegistry(pcr);
						ASSERT(NULL!=preg);
						FileRegHandle frh = pItem->GetFileRegHandle();
						ASSERT(NULL!=frh);
						preg->ReleaseRegRef(frh);
//						frh->ReleaseFRHRef();
					}
				}
#endif

				FileRegHandle frh = pItem->GetFileRegHandle();

				// Get the list of actions
				// Put these actions on the project if it's a target
				CProjItem * pItemActions = pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pItem->GetProject() : pItem;
				CActionSlobList * pActions = pItemActions->GetActiveConfig()->GetActionList();

				// If unbinding 'en masse' then clear up our output files
				// FUTURE: remove this
				if (plstSelectTools == (CPtrList *)NULL)
					pItemActions->GetActiveConfig()->FreeOutputFiles();

				// Go through them and unbind them from the tools
				// for their owner target

				CActionSlob * pAction;

				POSITION pos = pActions->GetHeadPosition();
				while (pos != (POSITION)NULL)
				{
					pAction = (CActionSlob *)pActions->GetNext(pos);

					// Un-bind from the tool?
					if (plstSelectTools != (CPtrList *)NULL && !plstSelectTools->Find(pAction->m_pTool))
						continue;	// no, it's not in our list

					pAction->UnBind();

					// Clear our config.
					pAction->m_pcrOur = pcr;
					ASSERT( pAction->m_pItem == pItemActions );
					ASSERT( SameBaseConfig( pcr, pItemActions ) );
				}
			}
			do {
				pItem = (CProjItem *)(posItem != (POSITION)NULL ? plstContent->GetNext(posItem) : NULL); 
			} while (pItem == pTheItem); // don't process pTheItem twice!!
 		}
	}

/*
	// We've done it for our regular tools, now global custom tools...?
	if (plstSelectTools == (CPtrList *)NULL)
		CActionSlob::UnBindActions(pTheItem,
								   g_prjcompmgr.GetCustomToolList(),
								   (ConfigurationRecord *)NULL, fUnbindContained);
*/

}
	
void CActionSlob::AssignActions
(
	CProjItem * pTheItem,
	CPtrList * plstSelectTools /* = NULL */,
	ConfigurationRecord * pcr /* = NULL */,
	BOOL fAssignContained /* = TRUE */,
	BOOL fAssignContainedOnly /* = FALSE */
)
{
	CTargetItem * pTarget = pTheItem->GetTarget();
	if (pTarget == (CTargetItem *)NULL)
		return;

	// Set the project config.?
	CProject * pProject = pTheItem->GetProject();
	CProjTempConfigChange projTempConfigChange(pProject);
	if (pcr != (ConfigurationRecord *)NULL)
	{
		projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);
	}
	else
	{
		pcr = pProject->GetActiveConfig();
	}

	// valid?
	if (pTarget->GetActiveConfig()->IsValid())
	{
		CProjType * pProjType = pTarget->GetProjType();

		// Buildable project type?
		BOOL fBuildableProjType = !pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeUnknown));

		// Get the target type tools list
		CPtrList * plstTools = (plstSelectTools == (CPtrList *)NULL) ? pProjType->GetToolsList() : plstSelectTools;

		// Content?
		CObList * plstContent;
		CObList lstContent;
		pTheItem->FlattenSubtree(lstContent, (CProjItem::flt_Normal | CProjItem::flt_ExcludeDependencies | CProjItem::flt_ExcludeProjects | CProjItem::flt_ExcludeGroups | CProjItem::flt_ExcludeProjDeps));
		plstContent = &lstContent;

		POSITION posItem = fAssignContained && (plstContent != (CObList *)NULL) ? plstContent->GetHeadPosition() : (POSITION)NULL;
		
		CProjItem * pItem = pTheItem;

		if (fAssignContainedOnly)
		{
			//
			// Skip parent item and go to first item in its contents or NULL.
			//
			do {
				pItem = (CProjItem *)(posItem != (POSITION)NULL ? plstContent->GetNext(posItem) : NULL); 
			} while (pItem == pTheItem); // don't process pTheItem twice!!
		}

		while (pItem != (CProjItem *)NULL)
		{
 			// don't do this for dependency files or folders!
			if (!pItem->IsKindOf(RUNTIME_CLASS(CDependencyFile)) &&
				!pItem->IsKindOf(RUNTIME_CLASS(CDependencyContainer)) &&
                !pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency))
 			   )
			{
				BOOL bItemIgnoreDefaultTool = FALSE;
				BOOL bIsFileItem = (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)));

#ifndef REFCOUNT_WORK
				// If assigning 'en masse' then make file itself part of the
				// dependency graph
				if (plstSelectTools == (CPtrList *)NULL)
				{
					// included in build?
					BOOL fExcluded;
					if (pItem->GetIntProp(P_ItemExcludedFromBuild, fExcluded) == valid && !fExcluded)
					{
						AddRefFileItem(pItem);
					}

				}
				BOOL fIgnoreTool;
				bItemIgnoreDefaultTool = (pItem->GetIntProp(P_ItemIgnoreDefaultTool, fIgnoreTool) == valid && fIgnoreTool);
#else
				if (bIsFileItem)
				{
					if (NULL==plstSelectTools)
					{
						BOOL fExcluded;
						if (pItem->GetIntProp(P_ItemExcludedFromBuild, fExcluded) == valid && !fExcluded)
						{
							ASSERT( NULL!=pcr );
							// Add to dependency graph as possible input to schmooze tool.
							CFileRegistry *preg = g_buildengine.GetRegistry(pcr);
							ASSERT(NULL!=preg);
							FileRegHandle frh = pItem->GetFileRegHandle();
							ASSERT(NULL!=frh);
							preg->AddRegRef(frh);
//							frh->ReleaseFRHRef();

						}
					}
					BOOL fIgnoreTool;
					bItemIgnoreDefaultTool = (pItem->GetIntProp(P_ItemIgnoreDefaultTool, fIgnoreTool) == valid && fIgnoreTool);
				}
#endif

				// stored on the project...even though attach to target
				FileRegHandle frh = pItem->GetFileRegHandle();

				// Get the list of actions
				// Put these actions on the project if it's a target
				// or default to the project if no target
				BOOL fTarget = pTarget == (CTargetItem *)NULL || pTarget == pItem;

				CProjItem * pItemActions = fTarget ? pItem->GetProject() : pItem;
				CActionSlobList * pActions = pItemActions->GetActiveConfig()->GetActionList();
				BOOL fActionsChange = FALSE;

				// Go through them and attach any to this item that want to be
				CBuildTool * pTool;
				POSITION pos = plstTools->GetHeadPosition();

				// 	No need to loop through tools.
				if ((NULL==frh && !fTarget) || (bIsFileItem && (pActions->GetCount() > 0)))
					pos = NULL;

				while (pos != (POSITION)NULL)
				{
					pTool = (CBuildTool *)plstTools->GetNext (pos);
					
					// Ignore unknown tools
					if (pTool->IsKindOf(RUNTIME_CLASS(CUnknownTool)))
						continue;

					// skip default action if custom build tool specified
					if (bItemIgnoreDefaultTool && (!pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool))))
						continue;

					CActionSlob * pAction;
					// allocate a 'building' + 'option' action to to list?

					// We are in bad shape if the active configuration of pItem doesn't match
					// pcr->m_pBaseRecord.
					ASSERT( SameBaseConfig( pcr, pItem ) );
					if (pTool->AttachToFile(frh, pItem))
					{
						pAction = new CActionSlob(pItem, pTool, fBuildableProjType, pcr);
					}
					// allocate an 'option' action if the item is a target?
					else if (fTarget)
					{
						pAction = new CActionSlob(pItem, pTool, FALSE, pcr);
					}
					else
						continue;	// ignore

					// check for multiple assignment
					ASSERT(!pActions->Find(pTool));

					pActions->AddTail(pAction);
					fActionsChange = TRUE;

					// cannot have multiple tools per-file for v3.0
					if (!fTarget && !pItem->IsKindOf(RUNTIME_CLASS(CProject)) )
						break;
				}

				// Actions changed?
				if (fActionsChange)
				{
					// inform item dependents of output change
					pItem->InformDependants(P_ItemTools);
#ifdef _DEBUG
					if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
					{
						ASSERT(pActions->GetCount() <= 1);
					}
#endif
				}
			}

			do {
				pItem = (CProjItem *)(posItem != (POSITION)NULL ? plstContent->GetNext(posItem) : NULL); 
			} while (pItem == pTheItem); // don't process pTheItem twice!!
		}
	}

/* 
R.I.P. for v4.0 with VBA?
	// We've done it for our regular tools, now global custom tools...?
	if (plstSelectTools == (CPtrList *)NULL)
		CActionSlob::AssignActions(pTheItem,
								   g_prjcompmgr.GetCustomToolList(),
								   (ConfigurationRecord *)NULL, fAssignContained);
*/

}

void CActionSlob::UnAssignActions
(
	CProjItem * pTheItem,
	CPtrList * plstSelectTools /* = NULL */,
	ConfigurationRecord * pcr /* = NULL */,
	BOOL fUnassignContained /* = TRUE */
)
{
	CTargetItem * pTarget = pTheItem->GetTarget();
	if (pTarget == (CTargetItem *)NULL)
		return;

	// Set the project config.?
	CProjTempConfigChange projTempConfigChange(pTheItem->GetProject());
	if (pcr != (ConfigurationRecord *)NULL)
	{
		projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);
	}
	else
	{
		pcr = pTheItem->GetProject()->GetActiveConfig();
	}

	// valid?
	if (pTarget == (CTargetItem *)NULL || pTarget->GetActiveConfig()->IsValid())
	{
		// Content?
		CObList * plstContent;
		CObList lstContent;
		pTheItem->FlattenSubtree(lstContent, (CProjItem::flt_Normal | CProjItem::flt_ExcludeDependencies | CProjItem::flt_ExcludeProjects | CProjItem::flt_ExcludeGroups | CProjItem::flt_ExcludeProjDeps));
		plstContent = &lstContent;

		POSITION posItem = fUnassignContained && (plstContent != (CObList *)NULL) ? plstContent->GetHeadPosition() : (POSITION)NULL;
		
		CProjItem * pItem = pTheItem;
		while (pItem != (CProjItem *)NULL)
		{
 			// don't do this for dependency files or folders!
			if (!pItem->IsKindOf(RUNTIME_CLASS(CDependencyFile)) &&
				!pItem->IsKindOf(RUNTIME_CLASS(CDependencyContainer)) &&
				!pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency))
			   )
			{
#ifndef REFCOUNT_WORK
				// If unassigning 'en masse' then make file itself part of the
				// dependency graph
				if (plstSelectTools == (CPtrList *)NULL)
				{
					// included in build?
					BOOL fExcluded;
					if (pItem->GetIntProp(P_ItemExcludedFromBuild, fExcluded) == valid && !fExcluded)
						ReleaseRefFileItem(pItem);
				}
#else
				if (NULL==plstSelectTools && pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
				{
					BOOL fExcluded;
					if (pItem->GetIntProp(P_ItemExcludedFromBuild, fExcluded) == valid && !fExcluded)
					{
						// Remove from dependency graph as possible input to schmooze tool.
						ASSERT( NULL!=pcr );
						// Add to dependency graph as possible input to schmooze tool.
						CFileRegistry *preg = g_buildengine.GetRegistry(pcr);
						ASSERT(NULL!=preg);
						FileRegHandle frh = pItem->GetFileRegHandle();
						ASSERT(NULL!=frh);
						if( frh!=NULL)
							preg->ReleaseRegRef(frh);
//						frh->ReleaseFRHRef();
					}
				}
#endif
				
				FileRegHandle frh = pItem->GetFileRegHandle();

				// Get the list of actions
				// Put these actions on the project if it's a target
				CProjItem * pItemActions = pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pItem->GetProject() : pItem;
				CActionSlobList * pActions = pItemActions->GetActiveConfig()->GetActionList();
				BOOL fActionsChange = FALSE;

				// If unassigning 'en masse' then clear up our output files
				// FUTURE: remove this
				if (plstSelectTools == (CPtrList *)NULL)
					pItemActions->GetActiveConfig()->FreeOutputFiles();

				// Go through them and detach them 

				CActionSlob * pAction;
				POSITION pos = pActions->GetHeadPosition();
				while (pos != (POSITION)NULL)
				{
					POSITION posAction = pos;
					pAction = (CActionSlob *)pActions->GetNext (pos);

					// Remove from list and de-allocate?
					if (plstSelectTools != (CPtrList *)NULL && !plstSelectTools->Find(pAction->m_pTool))
						continue;	// no, it's not in our list
					pActions->RemoveAt(posAction);
					fActionsChange = TRUE;
					delete pAction;
				}

				// Actions changed?
				if (fActionsChange)
				{
					// inform item dependents of output change
					pItem->InformDependants(P_ItemTools);
				}
			}

			do {
				pItem = (CProjItem *)(posItem != (POSITION)NULL ? plstContent->GetNext(posItem) : NULL); 
			} while (pItem == pTheItem); // don't process pTheItem twice!!
 		}
	}

/* 
R.I.P. for v4.0 with VBA?
	// We've done it for our regular tools, now global custom tools...?
	if (plstSelectTools == (CPtrList *)NULL)
		CActionSlob::UnAssignActions(pTheItem,
									 g_prjcompmgr.GetCustomToolList(),
									 (ConfigurationRecord *)NULL, fUnassignContained);
*/

}	    

void CActionSlob::ReleaseRefFileItem(CProjItem * pItem)
{
	ConfigurationRecord * pcr = pItem->GetActiveConfig();
	CFileRegistry * pRegistry = g_buildengine.GetRegistry(pcr);

	FileRegHandle frh;

	// Only do this for "real" items
	if (pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
	{
		// all target files
        ConfigurationRecord * pcrProjDep = ((CProjectDependency *)pItem)->GetTargetConfig();
        if (pcrProjDep != NULL)
        {
		    CFileRegSet * preg = (CFileRegSet *)pcrProjDep->GetTargetFiles(FALSE, TRUE);

            if (preg == NULL)
                return;

    		preg->InitFrhEnum();

	    	while ((frh = preg->NextFrh()) != (FileRegHandle)NULL)
			{
				// Speed:50: To fix Oly1951, in CActionSlob::AddRefFileItem
				// we didn't add the .sbr file to the main project's file registry.
				// So we should not remove these here as we end up deleting
				// the file reg items for the .sbr's too early.

 			    const CPath * pPath = g_FileRegistry.GetRegEntry(frh)->GetFilePath();

#ifndef REFCOUNT_WORK
				if (_tcsicmp(pPath->GetExtension(), ".sbr") == 0)
					continue;

		    	pRegistry->ReleaseRegRef(frh);
#else
				if (_tcsicmp(pPath->GetExtension(), ".sbr") != 0)
			    	pRegistry->ReleaseRegRef(frh);

				frh->ReleaseFRHRef();
#endif
			}

			delete preg;
        }
 	}
#ifndef REFCOUNT_WORK
	else if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{
 		frh = pItem->GetFileRegHandle();
		pRegistry->ReleaseRegRef(frh);
	}
#endif //#ifndef REFCOUNT_WORK
}

void CActionSlob::AddRefFileItem(CProjItem * pItem,CPtrList *lstOutputs )
{
	// Add in a whole bunch of refs that are not needed.
	// This is to fix olympus bug #5894
 
	ConfigurationRecord * pcr = pItem->GetActiveConfig();
	CFileRegistry * pRegistry = g_buildengine.GetRegistry(pcr);

	// Only do this for "real" items
	if (pItem->IsKindOf(RUNTIME_CLASS(CProjectDependency)))
	{
		// all target files
        ConfigurationRecord * pcrProjDep = ((CProjectDependency *)pItem)->GetTargetConfig();
        if (pcrProjDep != NULL)
        {
		    CFileRegSet * preg = (CFileRegSet *)pcrProjDep->GetTargetFiles(FALSE, TRUE);
    		FileRegHandle frh;

            if (preg == NULL)
                return;

	    	preg->InitFrhEnum();
		    while ((frh = preg->NextFrh()) != (FileRegHandle)NULL)
		    {
 			    const CPath * pPath = g_FileRegistry.GetRegEntry(frh)->GetFilePath();

				// Oly1951: We don't want to add a subproject's sbr files to the
				// main project's registry.  What we are really trying to accomplish
				// here is just to make sure any implibs from dll subprojects are
				// linked into the main project.
#ifndef REFCOUNT_WORK
				if (_tcsicmp(pPath->GetExtension(), ".sbr") == 0)
					continue;

			    if (pPath != NULL)
		    	{
			    	FileRegHandle frhLcl;
				    if ((frhLcl = pRegistry->LookupFile((const TCHAR *)*pPath)) == (FileRegHandle)NULL)
					    (void)pRegistry->RegisterFile(pPath, FALSE, FALSE);	// Filter the file
				    else
					    pRegistry->AddRegRef(frhLcl);
			    }
#else
				if (_tcsicmp(pPath->GetExtension(), ".sbr") != 0)
				{
				    pRegistry->AddRegRef(frh);
					if( lstOutputs )
						lstOutputs->AddHead(frh);
				}
				frh->ReleaseFRHRef();
#endif
		    }
			delete preg;
        }
 	}
#ifndef REFCOUNT_WORK
	else if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{
 		const CPath * pPath = pItem->GetFilePath();
		if (pPath != NULL)
		{
			FileRegHandle frhLcl;
			if ((frhLcl = pRegistry->LookupFile((const TCHAR *)*pPath)) == (FileRegHandle)NULL)
				(void)pRegistry->RegisterFile(pPath, FALSE, FALSE);	// Filter the file
			else
				pRegistry->AddRegRef(frhLcl);
		}
	}
#endif //#ifndef REFCOUNT_WORK
 }


CActionSlob * CActionSlob::GetTargetAction()
{
	// link it to the target-level action if we need to?
	if (m_pItem->GetTarget() != m_pItem)
	{
		// Get the target-level actions
		CActionSlobList * pTrgActions = m_pItem->GetProject()->GetActiveConfig()->GetActionList();

		// search for the tools' target-level action
		POSITION pos = pTrgActions->GetHeadPosition();
		m_pTrgAction = NULL;
		while (pos != (POSITION)NULL)
		{
			CActionSlob * pTrgAction = (CActionSlob *)pTrgActions->GetNext(pos);
			if (pTrgAction->m_pTool == m_pTool)
			{
				m_pTrgAction = pTrgAction;
				break;	// got one
			}
		}
	}

	return m_pTrgAction;
}

void CActionSlob::InformActions
(
	CProjItem * pTheItem,
	UINT idChange,
	ConfigurationRecord * pcr /* = NULL */,
	BOOL fInformContained /* = TRUE */
)
{
	if (pTheItem == (CProjItem *)NULL)
		return;

	CTargetItem * pTarget = pTheItem->GetTarget();
	if (pTarget == (CTargetItem *)NULL)
		return;

	// Set the project config.?
	CProjTempConfigChange projTempConfigChange(pTheItem->GetProject());

	if (pcr != (ConfigurationRecord *)NULL)
		projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);

	// valid?
	if (pTarget == (CTargetItem *)NULL || pTarget->GetActiveConfig()->IsValid())
	{
		// Content?
		CObList * plstContent;
		CObList lstContent;
//		pTheItem->FlattenSubtree(lstContent, (CProjItem::flt_Normal | CProjItem::flt_ExcludeDependencies | CProjItem::flt_ExcludeProjects | CProjItem::flt_ExcludeGroups));
		pTheItem->FlattenSubtree(lstContent, (CProjItem::flt_Normal));
		plstContent = &lstContent;

		POSITION posItem = fInformContained && (plstContent != (CObList *)NULL) ? plstContent->GetHeadPosition() : (POSITION)NULL;
		
		CProjItem * pItem = pTheItem;
		while (pItem != (CProjItem *)NULL)
		{
			// Get the list of actions
			// Put these actions on the project if it's a target
			CProjItem * pItemActions = pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pItem->GetProject() : pItem;
			// ASSERT( pItemActions->GetActiveConfig() );
			ConfigurationRecord *pcr = pItemActions->GetActiveConfig(TRUE);
			CActionSlobList * pActions = pcr->GetActionList();

			POSITION pos = pActions->GetHeadPosition();
			while (pos != (POSITION)NULL)
				// pass onto the action
				((CActionSlob *)pActions->GetNext(pos))->OnInform(pItem, idChange, 0);

			do {
				pItem = (CProjItem *)(posItem != (POSITION)NULL ? plstContent->GetNext(posItem) : NULL); 
			} while (pItem == pTheItem); // don't process pTheItem twice!!
 		}
	}

}

void CActionSlob::RefreshInputI()
{
	// make our inputs dirty
	m_fDirtyInput = TRUE;
	(void) GetInput();
}

void CActionSlob::RefreshOutputI()
{
	// disable this as we may potential change *a lot* of outputs
	// (we don't want the feedback just yet anyway, it'll mess up our
	//  dependency graph as it is being constructed)
	BOOL g_oldFileRegDisableFC = g_FileRegDisableFC;
	g_FileRegDisableFC = TRUE;

	// is this item being excluded from the build?
	BOOL fExcluded;
	if (m_pItem->GetIntProp(P_ItemExcludedFromBuild, fExcluded) != valid)
		fExcluded = TRUE;	// assume yes

	if (fExcluded)
	{
		// clear our source dep. on the output
		ClearSourceDep();

		// clear the actual output
		ClearOutput();
	}
	else
	{
		// make our outputs dirty
		// so that we update the output
		m_fDirtyOutput = TRUE;
		(void) GetOutput();
	}

	// inform item dependents of output change
	m_pItem->InformDependants(P_ItemOutput);

	// re-enable
	g_FileRegDisableFC = g_oldFileRegDisableFC;
}

void CActionSlob::ClearOutput()
{
	// ignore FRN_LCL_DESTROY changes to input
	// that may occur because of removal
	// of 'input->output' deps. in graph
	m_fBatchNotify = TRUE;

	int i = 0;

#ifndef REFCOUNT_WORK
	const CPtrList * plstFrh = m_frsOutput.GetContent();
	POSITION pos = plstFrh->GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		FileRegHandle frh = (FileRegHandle)plstFrh->GetNext(pos);

		// re-gen'ing the output?
		if (m_fInGenOutput)
		{
			// remember the old outputs...
			m_pfrsOldOutput->AddRegHandle(frh);
			m_paryOldOutputAttrib->Add(m_aryOutputAttrib.GetAt(i++));
 		}
		else
		{
			// remove this output dependency from the graph
			// FOR NOW ignore m_pcrOur == NULL
			if (m_fBuilder && m_pcrOur != NULL)
				if (m_pdepgraph != NULL)
					VERIFY(m_pdepgraph->ModifyDep(frh, this, DEP_Output, FALSE /* remove */));
		}

		// remove the entry in the registry if we created it in ::AddOutput()
		m_pregistry->ReleaseRegRef(frh);
	}
#else
	CFileRegSet* pregset = &m_frsOutput;
	FileRegHandle frh = NULL;
	pregset->InitFrhEnum();
	frh = pregset->NextFrh();
	while (frh != NULL)
	{

		// re-gen'ing the output?
		if (m_fInGenOutput)
		{
			// remember the old outputs...
			m_pfrsOldOutput->AddRegHandle(frh);
			m_paryOldOutputAttrib->Add(m_aryOutputAttrib.GetAt(i++));
 		}
		else
		{
			// remove this output dependency from the graph
			// FOR NOW ignore m_pcrOur == NULL
			if (m_fBuilder && m_pcrOur != NULL)
				if (m_pdepgraph != NULL)
					VERIFY(m_pdepgraph->ModifyDep(frh, this, DEP_Output, FALSE /* remove */));
		}

		// remove the entry in the registry if we created it in ::AddOutput()
		ASSERT( m_frsOutput.RegHandleExists(frh) );
		m_frsOutput.RemoveRegHandle(frh);

		if (m_frsSchmoozableOutput.RegHandleExists(frh))
		{
			m_frsSchmoozableOutput.RemoveRegHandle(frh);
		}

		// Next.
		frh->ReleaseFRHRef();
		frh = pregset->NextFrh();
	}
#endif

	// empty our output contents and assoc. attributes ...
#ifndef REFCOUNT_WORK
	m_frsOutput.EmptyContent();
#else
	ASSERT(m_frsOutput.IsEmpty());
	ASSERT(m_frsSchmoozableOutput.IsEmpty());
#endif
	m_aryOutputAttrib.RemoveAll();

	// take notice of all input changes....
	m_fBatchNotify = FALSE;
}

// retrieve our scanned dependencies
CFileRegSet * CActionSlob::GetScannedDep()
{
#if 0
	if (m_frsScannedDep == (CFileRegSet *)NULL)
	{
		m_frsScannedDep = new CFileRegSet; //(53);
#ifndef REFCOUNT_WORK
		m_frsScannedDep->AddFRHRef();	// prevent this from being auto-deleted
#endif
	}
#endif

	return &m_frsScannedDep;
}

// retrieve our missing dependencies
CFileRegSet * CActionSlob::GetMissingDep(BOOL bAlloc /*= TRUE*/)
{
#if 0
	if ( (m_frsMissingDep == (CFileRegSet *)NULL) && bAlloc )
	{
		m_frsMissingDep = new CFileRegSet;
#ifndef REFCOUNT_WORK
		m_frsMissingDep->AddFRHRef();	// prevent this from being auto-deleted
#endif
	}
#endif

	return &m_frsMissingDep;
}

// retrieve our source dependencies
CFileRegSet * CActionSlob::GetSourceDep()
{
#if 0
	if (m_frsSourceDep == (CFileRegSet *)NULL)
	{
		m_frsSourceDep = new CFileRegSet;
#ifndef REFCOUNT_WORK
		m_frsSourceDep->AddFRHRef();	// prevent this from being auto-deleted
#else
		//
		// Source deps should go into registry.
		//
		if (&g_FileRegistry!=m_pregistry)
			m_frsSourceDep->SetFileRegistry(m_pregistry);
#endif
	}
#endif

	return &m_frsSourceDep;
}

void CActionSlob::ClearDepI(UINT depType)
{
	// don't allow this for non-builders..
	if (!m_fBuilder)
		return;

	CFileRegSet * pfrsDep = NULL;
	if (depType == DEP_Scanned)
		pfrsDep = &m_frsScannedDep;

	else if (depType == DEP_Missing)
		pfrsDep = &m_frsMissingDep;

	else if (depType == DEP_Source)
		pfrsDep = &m_frsSourceDep;
	
	if (pfrsDep == (CFileRegSet *)NULL)
		return;	// nothing to do!

#ifndef REFCOUNT_WORK
	const CPtrList * plstFrh = pfrsDep->GetContent();
	POSITION pos = plstFrh->GetHeadPosition();
	while (pos != (POSITION)NULL)
		VERIFY(RemoveDepI(depType, (FileRegHandle)plstFrh->GetNext(pos)));
#else
	pfrsDep->InitFrhEnum();
	FileRegHandle frh = pfrsDep->NextFrh();
	while (frh != NULL)
	{
		VERIFY(RemoveDepI(depType, frh));
		frh->ReleaseFRHRef();
		frh = pfrsDep->NextFrh();
	}
#endif

	// empty our dependency contents...
	pfrsDep->EmptyContent();
}


void CActionSlob::SubScannedDep( FileRegHandle frh )
{
	// don't allow this for non-builders..
	if (!m_fBuilder)
		return;

	// Actually we don't know whether the dep was scanned or missing
	// so check in both.

	if ( m_frsScannedDep.RegHandleExists(frh) ) {
		RemoveDepI(DEP_Scanned, frh);
		m_frsScannedDep.RemoveRegHandle(frh);
	}
	else if ( m_frsMissingDep.RegHandleExists(frh) ){
		RemoveDepI(DEP_Missing, frh);
		m_frsMissingDep.RemoveRegHandle(frh);
	}

	return;	

}

BOOL CActionSlob::AddDepI(UINT depType, CString & strFile)
{
	// don't allow this for non-builders..
	if (!m_fBuilder)
		return TRUE;

	FileRegHandle frh;

	// if we're not a target tool try to form absolute
	// relative to source
	CPath pathFile;
	if (!m_pTool->m_fTargetTool)
	{
		const CPath * pPath = m_pregistry->GetRegEntry(m_frhInput)->GetFilePath();
		CDir dirFile;
		if (dirFile.CreateFromPath(*pPath) && 
			pathFile.CreateFromDirAndFilename(dirFile, strFile))
		{
			strFile = (const TCHAR *)pathFile;
		}
		else if (!pathFile.Create(strFile))
			return FALSE;
	}
	else if (!pathFile.Create(strFile))
		return FALSE;

	CFileRegistry * pregistry = depType == DEP_Source ? m_pregistry : &g_FileRegistry;

	// do we need to register this?
#ifndef REFCOUNT_WORK
	if ((frh = pregistry->LookupFile((const TCHAR *)pathFile)) == (FileRegHandle)NULL)
	{
		frh = pregistry->RegisterFile(&pathFile);	// incr. ref
	}
	else
		pregistry->AddRegRef(frh);	// incr. ref
#else
	frh = CFileRegFile::GetFileHandle(pathFile);	// incr. ref
#endif

	if (frh == (FileRegHandle)NULL)
		return FALSE;

#ifndef REFCOUNT_WORK
	return AddDepI(depType, frh);
#else
	BOOL bResult = AddDepI(depType,frh);
	frh->ReleaseFRHRef();
	return bResult;
#endif
}

BOOL CActionSlob::RemoveDepI(UINT depType, FileRegHandle frh)
{
	// don't allow this for non-builders..
	if (!m_fBuilder)
		return TRUE;

	CFileRegistry * pregistry = depType == DEP_Source ? m_pregistry : &g_FileRegistry;

	CFileRegSet * pfrsDep = NULL;
	if (depType == DEP_Scanned)
		pfrsDep = &m_frsScannedDep;

	else if (depType == DEP_Missing)
		pfrsDep = &m_frsMissingDep;

	else if (depType == DEP_Source)
		pfrsDep = &m_frsSourceDep;

	if (pfrsDep == (CFileRegSet *)NULL)
		return TRUE;	// nothing to do!

	if (depType == DEP_Scanned || depType == DEP_Missing)
	{
		// added as a source dep.?
		if (GetSourceDep()->RegHandleExists(frh))
			if (!RemoveSourceDep(frh))	// remove as a source dep. as well!
				return FALSE;
	}
	else if (depType == DEP_Source)
	{
		if (m_pdepgraph != NULL)
			// remove this source dependency from the graph now...
			VERIFY(m_pdepgraph->ModifyDep(frh, this, DEP_Source, FALSE /* remove */));
	}

#ifndef REFCOUNT_WORK
	pfrsDep->RemoveRegHandleI(frh);
	pregistry->ReleaseRegRef(frh);
#else
	if (pfrsDep->RegHandleExists(frh))
	{
		pfrsDep->RemoveRegHandleI(frh);
#if 0
		// This is done if pfrsDep is set to m_pregistry.
		pregistry->ReleaseRegRef(frh);
#endif
	}
#endif

	return TRUE;
}

BOOL CActionSlob::AddDepI(UINT depType, FileRegHandle frh)
{
	// is this a scanned dep. that should really be a source dep.?
	CDepGrNode * pgrnDep;

	CFileRegistry * pregistry = depType == DEP_Source ? m_pregistry : &g_FileRegistry;

	CFileRegSet * pfrsDep = NULL;
	if (depType == DEP_Scanned)
		pfrsDep = GetScannedDep();

	else if (depType == DEP_Missing)
		pfrsDep = GetMissingDep();

	else if (depType == DEP_Source)
		pfrsDep = GetSourceDep();

	else
		ASSERT(0);

	// must have a dependency set by this point
	ASSERT(pfrsDep != (CFileRegSet *)NULL);

	if (depType == DEP_Scanned)
	{
		// not found on disk?
		// so add as a missing dep.
		if ((g_pActiveProject != NULL) && (!pregistry->GetRegEntry(frh)->ExistsOnDisk()))
		{
#if 0
			return AddMissingDep(frh);
#else
			//
			// Add as a missing dep by changing depType and pfrsDep
			//
			depType = DEP_Missing;
			pfrsDep = GetMissingDep();

			// must have a dependency set by this point
			ASSERT(pfrsDep != (CFileRegSet *)NULL);
#endif
		}
	}

	if (depType == DEP_Scanned || depType == DEP_Missing)
	{
		// found this dep.?
		if (m_pdepgraph != NULL)
		{
			// make it a source dep. as well?
			if (m_pdepgraph->FindDep(frh, pgrnDep) && !AddSourceDep(frh))
				return FALSE;
		}
	}

	// add this source dependencies to the graph now...
	BOOL fOldInGenOutput = m_fInGenOutput;
	m_fInGenOutput = FALSE;

	if (depType == DEP_Source)
	{
		if (m_pdepgraph != NULL)
			VERIFY(m_pdepgraph->ModifyDep(frh, this, DEP_Source, TRUE /* add */));
	}
	
	m_fInGenOutput = fOldInGenOutput;
#ifndef REFCOUNT_WORK
	return pfrsDep->AddRegHandle(frh);
#else
	if (pfrsDep->RegHandleExists(frh))
	{
		// frh is already in pfrsDep.
		return TRUE;
	}
	else
	{
#if 0
		// This is done if pfrsDep is set to m_pregistry.
		if (!pfrsDep->RegHandleExists(frh))
			pregistry->AddRegRef(frh);
#endif

		// Assume that handle is *always* added!
		return pfrsDep->AddRegHandle(frh);
	}
#endif
}

// retrieve our input(s) (may be a file set)
CFileRegSet * CActionSlob::GetInput()
{
	// dirty?
	if (m_fDirtyInput)
	{
		if ((m_frhInput == NULL) && (m_pTool!=NULL) && (m_pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool))) && (m_pItem->IsKindOf(RUNTIME_CLASS(CProjContainer))))
		{
			// HACK: try adding it now
			AddInput();
		}

		if (m_frhInput != NULL)
		{
			ClearInput();

			// initialize the input

			// just return our input?
			if (!m_pTool->m_fTargetTool)
			{
				m_frsInput.AddRegHandle(m_frhInput, FALSE);		// *don't* incr. ref.
			}
			// else validate our inputs
			else
			{
				CFileRegSet * psetInput = (CFileRegSet *)m_pregistry->GetRegEntry(m_frhInput);

				FileRegHandle frh;
				psetInput->InitFrhEnum();
				while ((frh = psetInput->NextFrh()) != (FileRegHandle)NULL)
#ifndef REFCOUNT_WORK
					if (ValidInput(frh))
						m_frsInput.AddRegHandle(frh, FALSE);	// *don't* incr. ref.
#else
				{
					if (ValidInput(frh))
#if 0
#pragma message( "Check for library as input" )
					{
						if (frh->IsNodeType(CFileRegNotifyRx::nodetypeRegFile))
							ASSERT( 0 != _tcsicmp(frh->GetFilePath()->GetExtension(), ".lib" ) );
						m_frsInput.AddRegHandle(frh, FALSE);	// *don't* incr. ref.
					}
#else
						m_frsInput.AddRegHandle(frh, FALSE);	// *don't* incr. ref.
#endif
					frh->ReleaseFRHRef();
				}
#endif
			}

			m_fDirtyInput = FALSE;
		}
	}

	return &m_frsInput;
}

void CActionSlob::ClearInput()
{
	// clear our input
	// (perhaps a file set containing file sets so just remove files...)
#ifndef REFCOUNT_WORK
	const CPtrList * plstFrh = m_frsInput.GetContent();;
	POSITION pos = plstFrh->GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		FileRegHandle frh = (FileRegHandle)plstFrh->GetNext(pos);
		
		m_frsInput.RemoveRegHandleI(frh, FALSE);	// *don't* decr. ref.
	}
#else

#if 0
	CFileRegSet* pregset = &m_frsInput;
	pregset->InitFrhEnum();
	FileRegHandle frh = pregset->NextFrh();
	while (frh != NULL)
	{
		m_frsInput.RemoveRegHandleI(frh, FALSE);	// *don't* decr. ref.
		frh->ReleaseFRHRef();
		frh = pregset->NextFrh();
	}
#else
	m_frsInput.EmptyContent();
#endif

#endif
}

// retrieve our list of outputs (list of file registr handles)
CFileRegSet * CActionSlob::GetOutput()
{
	// return old output if we're currently re-gen'ing new output...
	if (m_fInGenOutput)
		return m_pfrsOldOutput;

	// got dirty output?
	// FOR NOW ignore m_pcrOur == NULL
	if (m_fDirtyOutput && m_fBuilder && m_pcrOur)
	{

		// set up temporary regset and array until we are done generating outputs
		CFileRegSet	frsOldOutput;
		CWordArray	aryOldOutputAttrib;
		frsOldOutput.SetFileRegistry(m_pregistry);
	    frsOldOutput.CalcAttributes(FALSE);
		m_pfrsOldOutput = &frsOldOutput;
		m_paryOldOutputAttrib = &aryOldOutputAttrib;

		// the output is no longer dirty
		m_fDirtyOutput = FALSE;

		// we're currently re-gen'ing the output
		m_fInGenOutput = TRUE;
	
		// clear and remember the old output
		ClearOutput();

	 	// re-initialise the output

 		// set the project config.
		CProjTempConfigChange projTempConfigChange(m_pItem->GetProject());
		projTempConfigChange.ChangeConfig((ConfigurationRecord *)m_pcrOur->m_pBaseRecord);

		CActionSlobList lstActions;
		lstActions.AddTail(this);
		if( !m_pTool->GenerateOutput(AOGO_Default, lstActions, g_DummyEC) ){
			m_fInGenOutput = FALSE;
			return &m_frsOutput;
		}

		// reset the project config.
		projTempConfigChange.Release();

		if (m_fInGenOutput)
		{
			//
			// modify the graph with any differences we detect
			//

#ifndef REFCOUNT_WORK
			POSITION pos;
#else
			FileRegHandle frh = NULL;
#endif
			int i = 0;

			// additions and same
#ifndef REFCOUNT_WORK
			const CPtrList * plstFrh = m_frsOutput.GetContent();
			pos = plstFrh->GetHeadPosition();
			while (pos != (POSITION)NULL)
			{
				FileRegHandle frh = (FileRegHandle)plstFrh->GetNext(pos);
#else
			CFileRegSet* pregset = &m_frsOutput;

			CFileRegSet Additions;
			CFileRegSet Deletions;
			CWordArray	AdditionsAttrib;

			pregset->InitFrhEnum();
			frh = pregset->NextFrh();
			while (frh != NULL)
			{
#endif
				WORD attrib = m_aryOutputAttrib.GetAt(i++);
				BOOL fWithAction = !!attrib;

				// found in our old outputs?
				if (!m_pfrsOldOutput->RegHandleExists(frh))
				{
					// new dep.
					// not found, so add this output dependencies to the graph
					Additions.AddRegHandle(frh);
					AdditionsAttrib.Add(fWithAction);
				}
				else
				{
					// same dep.
					m_pfrsOldOutput->RemoveRegHandleI(frh);
				}
#ifdef REFCOUNT_WORK
				// Next.
				frh->ReleaseFRHRef();
				frh = pregset->NextFrh();
#endif
			}

			// deletions
#ifndef REFCOUNT_WORK
			plstFrh = m_pfrsOldOutput->GetContent();
			pos = plstFrh->GetHeadPosition();
			while (pos != (POSITION)NULL)
			{
				FileRegHandle frh = (FileRegHandle)plstFrh->GetNext(pos);
#else
			pregset = m_pfrsOldOutput;
			pregset->InitFrhEnum();
			frh = pregset->NextFrh();
			while (frh != NULL)
			{
#endif

				// remove this output dependency from the graph
				Deletions.AddRegHandle(frh);

#ifdef REFCOUNT_WORK
				// Next.
				frh->ReleaseFRHRef();
				frh = pregset->NextFrh();
#endif
			}
			m_pfrsOldOutput->EmptyContent();

			if (m_pdepgraph != NULL)
			{
				pregset = &Additions;
				pregset->InitFrhEnum();
				frh = pregset->NextFrh();
				int iAdd = 0;
				while (frh != NULL)
				{
					VERIFY(m_pdepgraph->ModifyDep(frh, this, DEP_Output, TRUE /* add */, AdditionsAttrib.GetAt(iAdd)));

					// Next.
					frh->ReleaseFRHRef();
					frh = pregset->NextFrh();
					iAdd++;
				}

				pregset = &Deletions;
				pregset->InitFrhEnum();
				frh = pregset->NextFrh();
				while (frh != NULL)
				{
					VERIFY(m_pdepgraph->ModifyDep(frh, this, DEP_Output, FALSE /* remove */));

					// Next.
					frh->ReleaseFRHRef();
					frh = pregset->NextFrh();
				}
			}

			m_paryOldOutputAttrib->RemoveAll();

			// no longer re-gen'ing the output
			m_fInGenOutput = FALSE;
		}
	}

	return &m_frsOutput;
}

BOOL CActionSlob::AddOutputI(FileRegHandle frh, BOOL fWithAction)
{
	BOOL fRet = FALSE;

	m_fDirtyOutput = FALSE;
#ifndef REFCOUNT_WORK
	if (m_frsOutput.AddRegHandle(frh))
	{
		// re-gen'ing the output
		if (m_fInGenOutput)
		{
			m_aryOutputAttrib.Add((WORD)fWithAction);
		}
		else
		{
			// add this output dependencies to the graph
			// FOR NOW ignore m_pcrOur == NULL
			if (m_fBuilder && m_pcrOur != NULL)
				if (m_pdepgraph != NULL)
					VERIFY(m_pdepgraph->ModifyDep(frh, this, DEP_Output, TRUE /* add */, fWithAction));
		}

		fRet = TRUE;
	}
#else
	if (!m_frsOutput.RegHandleExists(frh))
	{
		m_frsOutput.AddRegHandle(frh);
	}

	// re-gen'ing the output
	if (m_fInGenOutput)
	{
		m_aryOutputAttrib.Add((WORD)fWithAction);
	}
	else
	{
		// add this output dependencies to the graph
		// FOR NOW ignore m_pcrOur == NULL
		if (m_fBuilder && m_pcrOur != NULL)
			if (m_pdepgraph != NULL)
				VERIFY(m_pdepgraph->ModifyDep(frh, this, DEP_Output, TRUE /* add */, fWithAction));
	}

	fRet = TRUE;
#endif

	return fRet;
}

BOOL CActionSlob::AddOutput(const CPath * pPath, BOOL fWithAction /* = TRUE */, BOOL fAllowFilter /* = TRUE */)
{
	FileRegHandle frh;
	BOOL fFilter = FALSE;

	CFileRegFile * pFileReg;

	// do we need to register this?
#ifndef REFCOUNT_WORK
	if ((frh = m_pregistry->LookupFile(*pPath)) == (FileRegHandle)NULL)
	{
		frh = m_pregistry->RegisterFile(pPath);	// incr. ref, no filter
		if (frh == (FileRegHandle)NULL)
			return FALSE;

		fFilter = fAllowFilter;
		pFileReg = (CFileRegFile *)m_pregistry->GetRegEntry(frh);

		// set default output dir type
		pFileReg->SetOutDirType(m_pTool->m_fTargetTool ? 1 : 2);

	}
	else
		m_pregistry->AddRegRef(frh);	// incr. ref

	BOOL fRet = AddOutputI(frh, fWithAction);
#else
	frh = CFileRegFile::GetFileHandle(*pPath);	// incr. ref, no filter

	if (NULL==frh)
		return FALSE;

	pFileReg = (CFileRegFile *)m_pregistry->GetRegEntry(frh);

	// set default output dir type
	pFileReg->SetOutDirType(m_pTool->m_fTargetTool ? 1 : 2);

	BOOL fRet = AddOutputI(frh, fWithAction);
	frh->ReleaseFRHRef();
#endif

	// we delayed the filtering until after it was in our output
	// now filter the file it	
#ifndef REFCOUNT_WORK
	if (fRet && fFilter)
		m_pregistry->FilterFile(frh);
#else
	if (fRet && fAllowFilter && !m_frsSchmoozableOutput.RegHandleExists(frh))
	{
		m_frsSchmoozableOutput.AddRegHandle(frh);
	}
#endif

	return fRet;
}
   
BOOL CActionSlob::AddOutput
(
	UINT			nFileNameId,
	const TCHAR *	pcDefExtension,
	const CDir *	pBaseDir,
	const TCHAR *	pcDefFilename, /* = NULL */
	UINT			nEnablePropId /* = -1 */,
	BOOL			fWithAction /* = TRUE */
)
{
	ASSERT (pBaseDir);

	// First check the enabling prop
	BOOL bVal;
	if (nEnablePropId != -1 && ((m_pItem->GetIntProp(nEnablePropId, bVal) != valid) || !bVal))
		return TRUE;

	BOOL fGotFileName = FALSE;

	// Output filename? 
	CString strFileName;
  	if (m_pItem->GetStrProp(nFileNameId, strFileName) == valid && !strFileName.IsEmpty())
	{
		// Try to substitute for $(INTDIR)
		if (!m_pItem->SubstOutDirMacro(strFileName, m_pItem->GetActiveConfig()))
		{
			// Try $(OUTDIR) if $(INTDIR) doesn't work
			m_pItem->GetProject()->SubstOutDirMacro(strFileName, m_pItem->GetProject()->GetActiveConfig());
		}

		// Okay, the item has an apperently non empty-name entry.  See if its
		// a directory by checking last character.  If it is,
		int len = strFileName.GetLength();
		const TCHAR * pc = (const TCHAR *) strFileName;
		pc = _tcsdec(pc, (TCHAR *)(pc+len));
	
		// need a filename?
		if (*pc != _T('/') && *pc != _T('\\'))
			fGotFileName = TRUE;	// no ... it doesn't appear to be a dir.
	}

	// got a filename?
	if (!fGotFileName)
	{
		// no, create a default..
		if (pcDefFilename)
			strFileName += pcDefFilename;
		else
			// just append and change extension later
			strFileName += m_pItem->GetFilePath()->GetFileName();
	}

	// create a path for this filename and add it as an output
	CPath tp;
	if (tp.CreateFromDirAndFilename(*pBaseDir, strFileName))
	{
		tp.ChangeExtension (pcDefExtension);

		return AddOutput((const CPath *)&tp, fWithAction);
	}

	return FALSE;
}

void CActionSlob::DirtyAllCommands
(
	CProjItem  * pTheItem,
	BOOL fPossibleChange /* = FALSE */,
	ConfigurationRecord * pcr /* = (ConfigurationRecord *)NULL */)
{
	CTargetItem * pTarget = pTheItem->GetTarget();
	if (pTarget == (CTargetItem *)NULL)
		return;

	// Set the project config.?
	CProjTempConfigChange projTempConfigChange(pTheItem->GetProject());
	if (pcr != (ConfigurationRecord *)NULL)
	{
		projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);
	}
	else
	{
		pcr = pTheItem->GetProject()->GetActiveConfig();
	}

	// valid?
	if (pTarget == (CTargetItem *)NULL || pTarget->GetActiveConfig()->IsValid())
	{
		// Content?
		CObList * plstContent;
		CObList lstContent;
		pTheItem->FlattenSubtree(lstContent, (CProjItem::flt_Normal | CProjItem::flt_ExcludeDependencies));
		plstContent = &lstContent;

		POSITION posItem = plstContent != (CObList *)NULL ? plstContent->GetHeadPosition() : (POSITION)NULL;

		CProjItem * pItem = pTheItem;
		while (pItem != (CProjItem *)NULL)
		{
 			// don't do this for dependency files or folders!
			if (!pItem->IsKindOf(RUNTIME_CLASS(CDependencyFile)) &&
				!pItem->IsKindOf(RUNTIME_CLASS(CDependencyContainer))
			   )
			{
				// Get the list of actions
				// Put these actions on the project if it's a target
				// or default to the project if no target
				BOOL fTarget = pTarget == (CTargetItem *)NULL || pTarget == pItem;

				CProjItem * pItemActions = fTarget ? pItem->GetProject() : pItem;
				CActionSlobList * pActions = pItemActions->GetActiveConfig()->GetActionList();
				CActionSlob * pAction;

				POSITION pos = pActions->GetHeadPosition();
				while (pos != (POSITION)NULL)
				{
					pAction = (CActionSlob *)pActions->GetNext(pos);

					// possible command change
					if (fPossibleChange)
						pAction->m_fPossibleOptionChange = TRUE;
					else 
						pAction->DirtyCommand();
					
					// cannot have multiple tools per-file for v3.0
					if (!fTarget)
					{
						ASSERT(pos == NULL);
						break;
					}
				}
			}

			do {
				pItem = (CProjItem *)(posItem != (POSITION)NULL ? plstContent->GetNext(posItem) : NULL); 
			} while (pItem == pTheItem); // don't process pTheItem twice!!
		}
	}

}

BOOL CActionSlob::RefreshCommandOptions(BOOL & fChanged, BOOL fNoSet /* = FALSE */)
{
	BOOL fRet = TRUE;

	CActionSlob * pTrgAction = GetTargetAction();

	// dirty command-line?
	if (!m_fDirtyCommand)
	{
		// if we've already done this return whether we changed
		if (m_wMarkRefCmdOpt == CActionSlob::m_wActionMarker)
		{
			// what was the status of the last refresh?
			fChanged = m_fCmdOptChanged;
			return TRUE;
		}

		// don't if there's no possible change at our level
		if (!m_fPossibleOptionChange &&
			// and at the target-level if we are a file-level action
			(pTrgAction == (CActionSlob *)NULL ||
			 (!pTrgAction->m_fPossibleOptionChange && !pTrgAction->m_fDirtyCommand))
		   )
		{
			// no change....
			fChanged = FALSE;
			return TRUE;
		}
	}

	BOOL fCheckTargetAction = FALSE;

	// Should we check target action for command options?
	// Never if we are already the target action as in the case
	// of a CCustomBuildTool.
	if (this!=pTrgAction)
	{
		// are we a file-level action with no per-file options?
		COptionHandler * popthdlr = m_pTool->GetOptionHandler();
		popthdlr->SetSlob(m_pItem);

		fCheckTargetAction = pTrgAction != (CActionSlob *)NULL && 
							   !popthdlr->HasAnyOfOurProperties(m_pItem, TRUE);

		popthdlr->ResetSlob();
	}

	if (fCheckTargetAction)
	{
		// yes, so we can get them from the target level
		fRet = pTrgAction->RefreshCommandOptions(fChanged, fNoSet);
		
		if (fRet && !fNoSet)
			fRet = pTrgAction->GetCommandOptions(m_strOptions);
	}
	else
	{
		// no, so get them from the tool
		CString strOldOptions(m_strOptions);

		CProjTempConfigChange projTempConfigChange(m_pItem->GetProject());
		projTempConfigChange.ChangeConfig((ConfigurationRecord *)m_pcrOur->m_pBaseRecord);
		
		// just make the check and not actually set the new command-line
		if (fNoSet)
		{
			// changed?

			// dirty command-line?
			if (m_fDirtyCommand)
			{
				fChanged = FALSE;
			}
			else
			{
				CString strDummyOptions; fRet = m_pTool->GetCommandOptions(this, strDummyOptions);
				fChanged = strDummyOptions != strOldOptions;
			}
		}
		else
		{
			// changed?
			m_strOptions = ""; fRet = m_pTool->GetCommandOptions(this, m_strOptions);

			// dirty command-line?
			if (m_fDirtyCommand)
			{
				fChanged = FALSE;
				m_fDirtyCommand = FALSE;	// we've got the command-line -> not dirty!
			}
			else
			{
				fChanged = m_strOptions != strOldOptions;
			}

		}

	}

	if (!fNoSet)
		m_fPossibleOptionChange = FALSE;

	// make sure we don't attempt to retrieve it again
	m_fCmdOptChanged = fChanged;
	m_wMarkRefCmdOpt = CActionSlob::m_wActionMarker;

	return fRet;
}

BOOL CActionSlob::AddScannedDep(int type, CString & strFile, int nLine, BOOL fScan)
{
	ASSERT(m_pIncludeList != DEP_None);

	// need to allocate our include entry list?
	if (m_pIncludeList == DEP_Empty)
	{
		if ((m_pIncludeList = new CIncludeEntryList(5)) == (CIncludeEntryList *)NULL)
			return FALSE;	// failure
	}

	// is this a system include? ignore?
	// Pass project if it exists.
	if (!g_SysInclReg.IsSysInclude(strFile, NULL==Item()? NULL : Item()->GetProject() ))
	{

#ifdef _KIP_PCH_
		// START KIP HACK
		BOOL b;

		if( m_pTool->IsKindOf( RUNTIME_CLASS(CCCompilerTool) ) ){
		 	if( (m_pItem->GetIntProp( m_pTool->MapLogical(P_PchUse), b ) == valid) && b ) {
				void *tmp;
				CString strTemp = strFile;
				strTemp.MakeLower();
				CMapStringToPtr *pPchMap = m_pItem->GetTarget()->GetPchMap();
		 		if( pPchMap->Lookup( strTemp, tmp ) ) {
		 			return TRUE;
				}
			}
		} 
		// STOP KIP HACK
#endif  // _KIP_PCH_

		// construct the include entry
		CIncludeEntry * pEntry = new CIncludeEntry;
		pEntry->m_EntryType = type;	  	
		pEntry->m_nLineNumber = nLine;
		pEntry->m_bShouldBeScanned = fScan;
		pEntry->m_FileReference = strFile;

		pEntry->CheckAbsolute();	// check the absoluteness of scanned dep.

		// add this to ourselves
		m_pIncludeList->AddTail(pEntry);

#ifdef _KIP_PCH_
		// START KIP HACK
		if( m_pTool->IsKindOf( RUNTIME_CLASS(CCCompilerTool) ) ) {
		 	if( (m_pItem->GetIntProp( m_pTool->MapLogical(P_PchCreate), b ) == valid) && b ) {
				CMapStringToPtr *pPchMap = m_pItem->GetTarget()->GetPchMap();
				CString strTemp = strFile;
				strTemp.MakeLower();
				pPchMap->SetAt( strTemp, NULL );
			}
		}
		// STOP KIP HACK
#endif  // _KIP_PCH_

	}

	return TRUE; // success
}

#ifdef _DEBUG
void CActionSlob::Dump(CDumpContext & dc) const
{
	const TCHAR * pchToolName = (const TCHAR *)*m_pTool->GetName();

	// dump the tool name followed by assoc. file?
	dc << pchToolName << "\r\n";
	if (m_fBuilder)
	{
		CFileRegEntry * pregentry;
		if (m_pTool->m_fTargetTool)
			pregentry = (CFileRegEntry *)&m_frsOutput;
		else
			pregentry = m_pregistry->GetRegEntry(m_frhInput);

		pregentry->Dump(afxDump);
	}
	dc << "\r\n";
}
#endif

void CActionSlob::FillToolLBox(CListBox * pLBox, CProjItem * pItem)
{
	// where are we getting the actions from?
	CProjItem * pItemActions = pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ? pItem->GetProject() : pItem;

	// get the action list for the active config. of this item
	CActionSlobList * plstActions = pItemActions->GetActiveConfig()->GetActionList();

	// empty the listbox.
	pLBox->ResetContent();

	// fill the listbox. with the tool name for each action
	// and assoc. the CActionSlob * as ptr. data
	POSITION pos = plstActions->GetHeadPosition();
	CString strTool;
	while (pos != (POSITION)NULL)
	{
		CActionSlob * pAction = (CActionSlob *)plstActions->GetNext(pos);
		ASSERT(pAction != (CActionSlob *)NULL);

		// must be 'builder' action, not 'option' action
		if (!pAction->m_fBuilder)
			continue;

		// add it to the listbox
		VERIFY(pLBox->SetItemDataPtr(pLBox->AddString((LPCTSTR)*pAction->m_pTool->GetUIName()), (void *)pAction) != CB_ERR);
	}
}

CActionSlob * CActionSlob::GetToolLBoxAction(CListBox * pLBox)
{
	int isel = pLBox->GetCurSel();
	return isel != LB_ERR ? (CActionSlob *)pLBox->GetItemDataPtr(isel) : (CActionSlob *)NULL;
}

CFileRegEntry * CActionSlob::GetFileGridEntry(CListBox * pBox)
{
	int isel = pBox->GetCurSel();
	return isel != -1 ? (CFileRegEntry *)pBox->GetItemDataPtr(isel) : (CFileRegEntry *)NULL;
}

int CActionSlob::FillFileGridI(CActionSlob * pAction, CFileRegEntry * pEntry, CListBox * pBox, int istart)
{
	// anything to do?
	if (pEntry == (CFileRegEntry *)NULL)
		return 0;

	ASSERT(pAction != (CActionSlob *)NULL);

	// set?
	if (pEntry->IsKindOf(RUNTIME_CLASS(CFileRegSet)))
	{
		// yes

		istart = pBox->GetCount();
#ifndef REFCOUNT_WORK
		CPtrList * plstFrh = (CPtrList *)((CFileRegSet *)pEntry)->GetContent();
		POSITION pos = plstFrh->GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			FileRegHandle frh =	(FileRegHandle)plstFrh->GetNext(pos);
			
			// recurse
			FillFileGridI(pAction, pAction->m_pregistry->GetRegEntry(frh), pBox, istart);
		}
#else
		CFileRegSet* pregset = (CFileRegSet *)pEntry;
		pregset->InitFrhEnum();
		FileRegHandle frh = pregset->NextFrh();
		while (frh != NULL)
		{
			// recurse
			FillFileGridI(pAction, pAction->m_pregistry->GetRegEntry(frh), pBox, istart);
			frh->ReleaseFRHRef();
			frh = pregset->NextFrh();
		}
#endif
	}
	else
	{
		// no

		const CPath * pPath = pEntry->GetFilePath();
		
		// get path relative to project directory
		CString strRelativePath;

		// add it to the grid with the registry entry as item ptr. data
		const TCHAR * pch;
		if (pPath->GetRelativeName(pAction->m_pItem->GetProject()->GetWorkspaceDir(), strRelativePath))
		 	pch = (const TCHAR *)strRelativePath;
		else
		 	pch = (const TCHAR *)*pPath;

		int insert = -1;	// where do we insert this? (default = at end)
		// use insertion sort		  
		CString strList;
		int index = istart, icount = pBox->GetCount();
		while (index < icount)
		{
			// get the text
			pBox->GetText(index, strList);

			// compare, can we insert?
			if (_tcsicmp(pch, (const TCHAR *)strList) <= 0)
			{
				// yes
				insert = index;
				break;
			}
			index++;
		}
	 	VERIFY(pBox->SetItemDataPtr(pBox->InsertString(insert, (const TCHAR *)pch), (void *)pEntry));

		// might need horz scrollbars
		CClientDC dc(pBox);
		int width = dc.GetTextExtent(pch, _tcslen(pch)).cx;
		if (width > pBox->GetHorizontalExtent())
			pBox->SetHorizontalExtent(width);
	}

	return pBox->GetCount();
}

void CActionSlob::FillFileGrid(CActionSlob * pAction, CListBox * pBox, UINT depType /* = DEP_Input */)
{ 
 	// disable the grid. if no viable action
	pBox->EnableWindow(pAction != (CActionSlob *)NULL);
	 
	// empty the grid.
	pBox->ResetContent();
	pBox->SetHorizontalExtent(0);

	if (pAction != (CActionSlob *)NULL)
	{
		// input, output or dependencies?
		if (depType == DEP_Input)
		{
			(void) FillFileGridI(pAction, pAction->GetInput(), pBox, 0);
		}
		else if (depType == DEP_Output)
		{
			(void) FillFileGridI(pAction, pAction->GetOutput(), pBox, 0);
		}
		else if (depType == DEP_Dependency)
		{
			int iend = FillFileGridI(pAction, pAction->GetSourceDep(), pBox, 0);
			(void) FillFileGridI(pAction, pAction->GetScannedDep(), pBox, iend);
		}
		else
			ASSERT(FALSE);	// not supported!
	}
}

//
// Update dependency stuff for CActionSlob
//
BOOL CActionSlob::IsDepInfoInitialized()
{
	return 0 != m_LastDepUpdate.dwLowDateTime || 0 != m_LastDepUpdate.dwLowDateTime;
}


BOOL CActionSlob::IsNewDepInfoAvailable()
{
	if (NULL!=m_pItem && m_pItem->IsKindOf( RUNTIME_CLASS(CFileItem) ) )
	{
		if (NULL != m_pTool && m_pTool->HasDependencies(m_pItem->GetActiveConfig()))
		{
			//
			// Dependency information from minimal rebuild engine.
			//	See if .obj date is newer than m_LastDepUpdate.
			//
			CFileRegSet* pRegSet = GetOutput();
			ASSERT(NULL!=pRegSet);
			if (NULL!=pRegSet)
			{
				FileRegHandle frh = pRegSet->GetFirstFrh();
				if (NULL!=frh)
				{
					FILETIME ftime;

					//
					// GetTimeProp will not return correct timestamp if closing project.
					//
					ASSERT(!g_bInProjClose);
					ASSERT(0==_tcsicmp(frh->GetFilePath()->GetExtension(),_T(".obj")));

					// Refresh attributes since file change notifications are disabled.
					frh->RefreshAttributes();

					VERIFY( frh->GetTimeProp(P_NewTStamp, ftime) );
					frh->ReleaseFRHRef();

#if 0
					//
					// Log time
					//
					SYSTEMTIME systime;
					FileTimeToSystemTime(&ftime,&systime);
					TRACE("File updated:");
					TRACE("%4d-%02d-%02d %02d:%02d:%02d.%04d\n",
						systime.wYear,systime.wMonth,systime.wDay,
						systime.wHour,systime.wMinute,systime.wSecond,systime.wMilliseconds );
					FileTimeToSystemTime(&m_LastDepUpdate,&systime);
					TRACE("Dep updated:");
					TRACE("%4d-%02d-%02d %02d:%02d:%02d.%04d\n",
						systime.wYear,systime.wMonth,systime.wDay,
						systime.wHour,systime.wMinute,systime.wSecond,systime.wMilliseconds );
#endif

					return ftime > m_LastDepUpdate;
				}
			}

		}
		else
		{
			//
			// Dependency information from scanner or NCB.
			//	See dependency file set date is later than m_LastDepUpdate.
			//
			FILETIME ftime;

			//
			// GetTimeProp will not return correct timestamp if closing project.
			//
			ASSERT(!g_bInProjClose);

			// Check self
			// Note: GetFileRegHandle() doesn't ref count...yet.
			FileRegHandle frh = m_pItem->GetFileRegHandle();
			ASSERT(NULL!=frh);
			VERIFY( frh->GetTimeProp(P_NewTStamp, ftime) );
			if (ftime > m_LastDepUpdate)
				return TRUE;

			// Check sources.
			VERIFY( GetSourceDep()->GetTimeProp(P_NewTStamp, ftime) );
			if (ftime > m_LastDepUpdate)
				return TRUE;

			// Check dependencies.
			VERIFY( GetScannedDep()->GetTimeProp(P_NewTStamp, ftime) );

			return ftime > m_LastDepUpdate;
		}
	}
	return FALSE;
}

//
// Update dependency information if new information is available.
// Set bForceUpdate = TRUE to force update if updating has been turned off
// minimal rebuild info is not available.
BOOL CActionSlob::UpdateDepInfo(BOOL bForceUpdate /* = FALSE */)
{
	// Not ready for bForceUpdate yet.
	ASSERT(!bForceUpdate);

	//
	// Don't update if new information is not availabe.
	//
	if ( !bForceUpdate && !IsNewDepInfoAvailable())
		return FALSE;

	if (NULL!=m_pItem && m_pItem->IsKindOf( RUNTIME_CLASS(CFileItem) ) )
	{
		if (m_pItem->ScanDependencies())
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	return FALSE;
}

void CActionSlob::UpdateDepInfoTimeStamp()
{
	GetSystemTimeAsFileTime(&m_LastDepUpdate);
}

BEGIN_MESSAGE_MAP(CFileGridPage, CProjPage)
	//{{AFX_MSG_MAP(CFileGridPage)
	//}}AFX_MSG_MAP
	ON_LBN_SELCHANGE(IDC_TOOLS, OnToolsSelChange)
	ON_LBN_SELCHANGE(IDC_TOOL_FILES, OnFileGridSelChange)
END_MESSAGE_MAP()

CFileGridPage::CFileGridPage(UINT depType /* = DEP_Input */)
{
	m_depType = depType;
}

BOOL CFileGridPage::OnInitDialog()
{ 
	// call base-class first
	if (!CProjPage::OnInitDialog())
		return FALSE;

	return TRUE;
}

void CFileGridPage::InitPage()
{
	// call base-class
	CProjPage::InitPage();

	// setup our help id
	if (m_depType == DEP_Input)
		m_nIDHelp = IDDP_TOOL_INPUTS;

	else if (m_depType == DEP_Output)
		m_nIDHelp = IDDP_TOOL_OUTPUTS;

	else if (m_depType == DEP_Dependency)
		m_nIDHelp = IDDP_TOOL_DEPENDENCIES;

	// update our tools list
	OnPropChange(P_ItemTools);
}

BOOL CFileGridPage::OnPropChange(UINT nProp)
{
	// can we handle this?
	switch (nProp)
	{
		case P_ItemTools:
		{
			// fill listbox with our list of tools for this item
			CListBox * pLBox = (CListBox *)GetDlgItem(IDC_TOOLS);

			CProjItem * pItem;
			CBuildSlob * pBuildSlob = NULL;
			if (m_pSlob->IsKindOf(RUNTIME_CLASS(CBuildSlob)))
				pBuildSlob = (CBuildSlob *)m_pSlob;
			if (m_pSlob->IsKindOf(RUNTIME_CLASS(CBuildViewSlob)))
				pBuildSlob = ((CBuildViewSlob *)m_pSlob)->GetBuildSlob();

			if (pBuildSlob)
				pItem = pBuildSlob->GetProjItem();				
 			else
				pItem = (CProjItem *)m_pSlob;

			// Set the correct active config on the item
			if (pBuildSlob)
				pBuildSlob->FixupItem();

			CActionSlob::FillToolLBox(pLBox, pItem);

			// Reset the original active config for the item
			if (pBuildSlob)
				pBuildSlob->UnFixupItem();

			// set tools selection to first
			pLBox->SetCurSel(0);	
			OnToolsSelChange();

			// set grid selection to first
			CListBox * plboxFiles = (CListBox *)GetDlgItem(IDC_TOOL_FILES);
			plboxFiles->SetCurSel(0);
			OnFileGridSelChange();
			break;
		}

		case P_ItemInput:
		case P_ItemOutput:
		case P_ItemDependency:
		{
			// only update if interested
			if ((nProp == P_ItemInput && m_depType == DEP_Input) ||
				(nProp == P_ItemOutput && m_depType == DEP_Output) ||
				(nProp == P_ItemDependency && m_depType == DEP_Dependency)
			   )
			{
				// show outputs for the current action
				CListBox * plboxTools = (CListBox *)GetDlgItem(IDC_TOOLS);
				CListBox * plboxFiles = (CListBox *)GetDlgItem(IDC_TOOL_FILES);
				CActionSlob::FillFileGrid(CActionSlob::GetToolLBoxAction(plboxTools), plboxFiles, m_depType);
			}
			break;
		}

		default:
			// no, pass on to base-class
			return CProjPage::OnPropChange(nProp);
	}

	return TRUE;
}

void CFileGridPage::OnToolsSelChange()
{
	// update our settings/outputs
	if (m_depType == DEP_Input)
		OnPropChange(P_ItemInput);

	else if (m_depType == DEP_Output)
		OnPropChange(P_ItemOutput);

	else if (m_depType == DEP_Dependency)
		OnPropChange(P_ItemDependency);
}

void CFileGridPage::OnFileGridSelChange()
{
	// update our timestamp view
	CString strTimeStamp;
	CListBox * plboxFiles = (CListBox *)GetDlgItem(IDC_TOOL_FILES);
	CFileRegEntry * pEntry = CActionSlob::GetFileGridEntry(plboxFiles);
	if (pEntry != (CFileRegEntry *)NULL)
	{
		if (!pEntry->GetFileTime(strTimeStamp))
		{
			UINT idStr = 0;
			if (m_depType == DEP_Input)
				idStr = IDS_NO_TOOL_INPUT;

			else if (m_depType == DEP_Output)
				idStr = IDS_NO_TOOL_OUTPUT;

			else if (m_depType == DEP_Dependency)
				idStr = IDS_NO_TOOL_DEP;

			ASSERT(idStr);
			VERIFY(strTimeStamp.LoadString(idStr));
		}
	}


	// set the static text
	GetDlgItem(IDC_FILE_DATE)->SetWindowText(strTimeStamp);
}

CInputsPage g_InputsPage;
BEGIN_IDE_CONTROL_MAP(CInputsPage, IDDP_TOOL_FILES, IDS_INPUTS)
END_IDE_CONTROL_MAP()

COutputsPage g_OutputsPage;
BEGIN_IDE_CONTROL_MAP(COutputsPage, IDDP_TOOL_FILES, IDS_OUTPUTS)
END_IDE_CONTROL_MAP()

CDepsPage g_DepsPage;
BEGIN_IDE_CONTROL_MAP(CDepsPage, IDDP_TOOL_FILES, IDS_DEPENDENCIES)
END_IDE_CONTROL_MAP()

// our 
static TCHAR szRegKey[]		= _TEXT("Build System");

CBuildEngine::CBuildEngine()
{
#ifdef _DEBUG
	DWORD dwDbg = 0;

	// construct our registry key
	CString strKeyName(::GetRegistryKeyName());
	strKeyName += _TEXT("\\");
	strKeyName += szRegKey;

	HKEY hKey;
	// load the '\\Debugging' key
	if (RegOpenKeyEx(HKEY_CURRENT_USER, strKeyName, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwType, dwValueSize = sizeof(DWORD);
		if (RegQueryValueEx(hKey, _TEXT("Debugging"), NULL, &dwType, (LPBYTE)&dwDbg, &dwValueSize) == ERROR_SUCCESS)
		{
		  	// make sure we get the value we expect
			ASSERT(dwType == REG_DWORD);
			ASSERT(dwValueSize == sizeof(DWORD));
		}

		RegCloseKey(hKey); // close the key we just opened
	} 

#endif

#ifdef _DEBUG
	m_dwDebugging = dwDbg;
#else // _DEBUG
	m_dwDebugging = 0;	// off
#endif // !_DEBUG

	// batching is off by default
	m_fBatchCmdLines = FALSE;

	// create our output window error context
	m_pECOutputWin = new COutputWinEC();
}

CBuildEngine::~CBuildEngine()
{
	ASSERT(m_MemFiles.IsEmpty());
	//ASSERT(m_mapBldInst.IsEmpty());         //??? add an extra HadBuildState call
	// ASSERT(m_slCmds.GetCount() == 0);

	// delete our output window error context
	delete m_pECOutputWin;
}

CMD CBuildEngine::FlushCmdLines()
{
	CMD cmd = CMD_Complete;

	// execute our currently batched commands
	cmd = ExecuteCmdLinesI
			(
				m_plCmds,
				m_dirLast,
				m_fLastCheckForComSpec, m_fLastIgnoreErrors,
				*m_pECLast
			);

	// flush
	m_plCmds.RemoveAll();

	return cmd;
}

void CBuildEngine::ClearCmdLines()
{
    // Clear out any batched cmds - important if we stop
    // a build in the middle.
    m_plCmds.RemoveAll();
}

CMD CBuildEngine::BatchCmdLines(BOOL fBatch /* = TRUE */)
{	
	CMD cmd = CMD_Complete;

	if (m_fBatchCmdLines != fBatch)
	{
		// set-it
		m_fBatchCmdLines = fBatch;

		// turning off batching?
		if (!fBatch)
			cmd = FlushCmdLines();
	}

	return cmd;
}

CMD CBuildEngine::ExecuteCmdLines
(
	CPtrList & plCmds,
	CDir & dir,
	BOOL fCheckForComSpec, BOOL fIgnoreErrors,
	CErrorContext & EC
)
{
	CMD cmd = CMD_Complete;

	// batching?
	if (m_fBatchCmdLines)
	{
		//
		// batching 
		//
		BOOL fRememberContext = FALSE;

		// need to flush current if context changhed?
		if (m_plCmds.GetCount() > 0)
		{
			if (dir != m_dirLast ||
				fCheckForComSpec != m_fLastCheckForComSpec ||
				fIgnoreErrors != m_fLastIgnoreErrors ||
				&EC != m_pECLast
			   )
			{
				// flush our currently batched commands
				cmd = FlushCmdLines();

				// remember this new context
				fRememberContext = TRUE;
			}
		}
		else
		{
			// initialise the context
			fRememberContext = TRUE;
		}

		// remember the last context?
		if (fRememberContext)
		{
			m_dirLast = dir;
			m_fLastCheckForComSpec = fCheckForComSpec;
			m_fLastIgnoreErrors = fIgnoreErrors;
			m_pECLast = &EC;
		}

		// perform batch
		m_plCmds.AddTail(&plCmds);

		// done
	}
	else
	{
		// 
		// perform build commands
		//
		cmd = ExecuteCmdLinesI(plCmds,dir, fCheckForComSpec, fIgnoreErrors, EC);
	}

	// Make sure cmd is set to a valid value.
	ASSERT(cmd == CMD_Canceled || cmd == CMD_Complete || cmd == CMD_Error);

	return cmd;
}

CMD CBuildEngine::ExecuteCmdLinesI
(
	CPtrList & plCmds,
	CDir & dir,
	BOOL fCheckForComSpec,
	BOOL fIgnoreErrors,
	CErrorContext & EC
)
{
	// perform the command
	int ret = g_Spawner.DoSpawn(plCmds, dir, fCheckForComSpec, fIgnoreErrors, EC);

	// The return value we've got now is  actually an int error code returned 
	// by NTSPAWN.  Translate it to one of our CMD_ values.
	if (ret != 0) 
	{
		if ( CMD_Canceled == (UINT)ret )
			return CMD_Canceled;
		else
			return CMD_Error;
	}

	return CMD_Complete;
}

void CBuildEngine::LogBuildError(UINT idError, void * pvContext, CErrorContext & EC)
{
	CString strError;
	BOOL fLogAsError = FALSE;

	// construct the error
	switch (idError)
	{
		case BldError_CantGetCmdLines:
			::MsgText(strError, idError, (const TCHAR *)*((CBuildTool *)pvContext)->GetName());
			fLogAsError = TRUE;
			break;

		case BldError_ScanError:
			::MsgText(strError, IDS_SCAN_ERROR, (const TCHAR *)*(CPath *)pvContext);
			break;
	}

	// display the error
	if (&EC == &g_DummyEC)
	{
		if (theApp.m_bInvokedCommandLine)
		{
			theApp.WriteLog(strError, TRUE);
		}
		else
		{
			g_VPROJIdeInterface.GetOutputWindow()->WriteStringToOutputWindow(strError, TRUE, TRUE);
			g_VPROJIdeInterface.GetOutputWindow()->ShowOutputWindow();
		}
	}
	else
		EC.AddString(strError);

	// log this as an error?
	if (fLogAsError)
		g_Spawner.m_dwProjError++;
}

BOOL CBuildEngine::OpenMemFile
(
	FileRegHandle frh,
	HANDLE & hMap,
	const TCHAR * & pcFileMap,
	DWORD & dwFileSize, CErrorContext & EC
)
{
	BOOL fRet = FALSE;	// success?
	HANDLE hFile;

	// pointer to memory mapped file (== (const TCHAR *)NULL if not available)
	pcFileMap = (const TCHAR *)NULL;	// default

	const CPath * pPath = g_FileRegistry.GetRegEntry(frh)->GetFilePath();

	for	(;;)
	{
		hMap = NULL;
		pcFileMap = NULL;
		hFile = CreateFile(
				(LPCSTR) *pPath,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			break;	// failure

		DWORD dwFileSize2;
		dwFileSize = GetFileSize(hFile, &dwFileSize2);			

		if (dwFileSize == 0xffffffff)
			break;	// failure

		if (dwFileSize == 0)
		{
			CloseHandle(hFile);
			return TRUE;	// ignore
		}

		hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);

		if (hMap == NULL)
			break;	// failure

		pcFileMap = (const TCHAR *) MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);

		break;			// break-out
	}

	if (pcFileMap == (const TCHAR *)NULL)
	{
	  	TCHAR errstring [256];
		DWORD dw = GetLastError();

		if (hMap != NULL)
			CloseHandle(hMap);

		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle (hFile);
				
		//	Try to get a text message for the error.  This fails if NT
		//	doesn't have its error resource:
		errstring[0] = 0;
	 	FormatMessage(
					  FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					  0,		// ignored
					  dw,
					  LOCALE_USER_DEFAULT,
					  errstring,
					  256,
					  0		// ignored
					 );

		RemoveNewlines(errstring);

		CString strError;
		::MsgText(strError, IDS_OPEN_FOR_SCAN_FAILED, (const TCHAR *) *pPath, (const TCHAR *) errstring);
		/* 
		   // ignore this error for now, since we got file save notification on different thread, 
		   // we can get cases like this, we will let the scan dep to do scan again instead of output error.
		if (&EC == &g_DummyEC)
		{
			if (theApp.m_bInvokedCommandLine)
			{
				theApp.WriteLog(strError, TRUE);
			}
			else
			{
				g_VPROJIdeInterface.GetOutputWindow()->WriteStringToOutputWindow(strError, TRUE, TRUE);
				g_VPROJIdeInterface.GetOutputWindow()->ShowOutputWindow();
			}
		}
		else
			EC.AddString(strError);
		*/
	}
	else
	{
		fRet = TRUE;	// success
		MEMFILE * pMemFile = new MEMFILE;
		pMemFile->hFile = hFile;
		pMemFile->hMap = hMap;
		pMemFile->pcFileMap = pcFileMap;

		m_MemFiles.AddTail(pMemFile);
	}

	return fRet;
}

BOOL CBuildEngine::CloseMemFile(HANDLE & hMap, CErrorContext & EC)
{
	// Look for a memory mapped file entry in our list which has
	// the same mapping object handle
    MEMFILE * pMemFile; POSITION posCurr;
	POSITION pos = m_MemFiles.GetHeadPosition();
	while (pos != NULL)
	{
        posCurr = pos;
		pMemFile = (MEMFILE *) m_MemFiles.GetNext(pos);
		if (pMemFile->hMap == hMap)
		{
			// close this memory mapped file
			UnmapViewOfFile((LPVOID) pMemFile->pcFileMap);
			CloseHandle(pMemFile->hMap);
			CloseHandle(pMemFile->hFile);

            // Remove the MEMFILE entry from our list
            delete pMemFile;
            m_MemFiles.RemoveAt(posCurr);

			return TRUE;
		}		
	}

	// We failed to close the requested file	
	return FALSE;
}

BOOL CBuildEngine::DeleteFile(FileRegHandle frh, CErrorContext & EC)
{
	BOOL fRet = TRUE;	// default is success

	if (!g_FileRegistry.DeleteFile(frh))
	{
		DWORD dw;

		dw = GetLastError();
		if (dw !=  ERROR_FILE_NOT_FOUND && dw != ERROR_PATH_NOT_FOUND)
		{
			TCHAR buf[256]; buf[0] = _T('\0');

			// we get NO_ERROR sometimes!
			if (dw != NO_ERROR)
			{
				// place a ': ' between 'cannot delete' and system messages
				buf[0] = _T(':'); buf[1] = _T(' ');

				FormatMessage(
							  FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
							  0,		// ignored
							  dw,
							  LOCALE_USER_DEFAULT,
							  buf + 2,
							  (sizeof(buf) / sizeof(TCHAR)) - 2,
							  0		// ignored
						   	 );
				RemoveNewlines(buf);
			}

			CString strError;
			::MsgText(
					  strError,
				 	  IDS_PROJ_COULD_NOT_DELETE_FILE,
					  (const TCHAR *) *g_FileRegistry.GetRegEntry(frh)->GetFilePath(),
 					  buf
					 );

			if (&EC == &g_DummyEC)
			{
				if (theApp.m_bInvokedCommandLine)
				{
					theApp.WriteLog(strError, TRUE);
				}
				else
				{
					g_VPROJIdeInterface.GetOutputWindow()->WriteStringToOutputWindow(strError, TRUE, TRUE);
					g_VPROJIdeInterface.GetOutputWindow()->ShowOutputWindow();
				}
			}
			else
			{
				EC.AddString(strError);
			}

			fRet = FALSE;	// failure
		}
	}

	return fRet;
}

#define MAX_CMD_LEN 256
// REVIEW: #define MAX_CMD_LEN 127

BOOL CBuildEngine::FormCmdLine(CString & strToolExeName, CString & strCmdLine, CErrorContext & EC, BOOL fRspFileOk)
{
	// is this line too long?
	if (strCmdLine.GetLength() + strToolExeName.GetLength() + 2 > MAX_CMD_LEN - 1)
	{
		CPath pathRsp;
		if (!fRspFileOk || !TmpMaker.CreateTempFileFromString(strCmdLine, pathRsp, EC)) 
			return FALSE;

		// quote response file path if necessary
		CString strRspPath = pathRsp;
		if (pathRsp.ContainsSpecialCharacters())
			strRspPath = _T('\"') + strRspPath + _T('\"');

		// use response file
		strCmdLine = (strToolExeName + _TEXT(" @")) + strRspPath;
	}
	else
	{
		// don't use response file

		// replace '\r\n\t' with spaces
		int cchLen = strCmdLine.GetLength();
		TCHAR * pch = (TCHAR *)(const TCHAR *)strCmdLine;
		for (int ich = 0; ich < cchLen;)
		{
			TCHAR ch = pch[ich];
			if (ch == _T('\r') || ch == _T('\n') || ch == _T('\t'))
				pch[ich] = _T(' ');

			ich += _tclen(pch + ich);
		}

		strCmdLine = (strToolExeName + _T(' ')) + strCmdLine;
	}
//	strCmdLine.AnsiToOem();  // BruceMa 10-15-97
	#ifdef PROJ_LOG
	CString strCommand;
	strCommand.LoadString(IDS_CREATING_COMMANDLINE); 
	PBUILD_TRACE ((const TCHAR *)strCommand, (const TCHAR *)strCmdLine );
	#endif

	return TRUE;	// success
}

BOOL CBuildEngine::FormBatchFile( CString & strCmdLine, CErrorContext & EC)
{
	// is this line too long?
	CPath pathRsp;

	strCmdLine.AnsiToOem();

	if ( !TmpMaker.CreateTempFileFromString(strCmdLine, pathRsp, EC, TRUE ) )
		return FALSE;

	// use response file
	strCmdLine = pathRsp;

	// quote path if necessary
	if (pathRsp.ContainsSpecialCharacters())
		strCmdLine = _T('\"') + strCmdLine + _T('\"');

	#ifdef PROJ_LOG
	CString strCommand;
	strCommand.LoadString(IDS_CREATING_COMMANDLINE); 
	PBUILD_TRACE((const TCHAR *)strCommand,(const TCHAR *)	strCmdLine );
	#endif

	return TRUE;	// success
}

CFileRegistry * CBuildEngine::GetRegistry(ConfigurationRecord * pcr)
{
	// get a build instance for the currently active config.
	SBldInst * pbldinst;

	// if one doesn't exist, create one on demand
	if (!m_mapBldInst.Lookup((void *)pcr->m_pBaseRecord, (void *&)pbldinst))
		pbldinst = AddBuildInst(pcr);

	return pbldinst->s_preg;
}

CFileDepGraph * CBuildEngine::GetDepGraph(ConfigurationRecord * pcr)
{
	// get a build instance for the currently active config.
	SBldInst * pbldinst;

	// if one doesn't exist, create one on demand
	if (!m_mapBldInst.Lookup((void *)pcr->m_pBaseRecord, (void *&)pbldinst))
		pbldinst =  AddBuildInst(pcr);

	return pbldinst->s_pgraph;
}

void CBuildEngine::SetGraphMode(UINT mode, CProject * pBuilder)
{
	const CPtrArray * pCfgArray = pBuilder->GetConfigArray();
	int isize = pCfgArray->GetSize();
	for (int i = 0; i < isize; i++)
		GetDepGraph((ConfigurationRecord *)pCfgArray->GetAt(i))->SetGraphMode(mode);
}

CBuildEngine::SBldInst * CBuildEngine::AddBuildInst(ConfigurationRecord * pcr)
{
	SBldInst * pbldinst;

	if (m_mapBldInst.Lookup((void *)pcr->m_pBaseRecord, (void *&)pbldinst))
		return pbldinst;	// already got one!

	// add a build instance for the currently active config.

	// allocate
	pbldinst = new SBldInst;
	pbldinst->s_preg = new CFileRegistry;
	pbldinst->s_pgraph = new CFileDepGraph(pbldinst->s_preg);

#ifndef REFCOUNT_WORK
	// allow global registry access to this
  	g_FileRegistry.AllowAccess(pbldinst->s_preg);
#endif

	// set in our lookup map
	m_mapBldInst.SetAt((void *)pcr->m_pBaseRecord, (void *)pbldinst);

	return pbldinst;
}

void CBuildEngine::RemoveBuildInst(ConfigurationRecord * pcr)
{
	// remove a build instance for the currently active config.
	SBldInst * pbldinst;
	VERIFY(m_mapBldInst.Lookup((void *)pcr->m_pBaseRecord, (void *&)pbldinst));

#ifndef REFCOUNT_WORK
	// dis-allow global registry access to this
	g_FileRegistry.DisallowAccess(pbldinst->s_preg);
#endif

	// de-allocate
	delete pbldinst->s_pgraph;
	delete pbldinst->s_preg;
	delete pbldinst;

	// remove from our lookup map
	m_mapBldInst.RemoveKey((void *)pcr->m_pBaseRecord);
}

BOOL CBuildEngine::HasBuildState
(
	ConfigurationRecord * pcrBuild,
	CPtrList & lstFrh,														  
	UINT statefilter /* = DS_OutOfDate */,
	FileRegHandle frhStart /* = (FileRegHandle)NULL*/
)
{
	// get our build dependency graph for this config.
	CFileDepGraph * pdepgraph = GetDepGraph(pcrBuild);

	// use a different action marker than the last time we did a build
	CActionSlob::m_wActionMarker++;

	// make sure we have the v.latest dep. graph state
	// so flush any possible file change notifications
	FileChangeDeQ();

	//and possible dep update
	// FileItemDepUpdateDeQ();

	// If this is a java project, then set frhStart to NULL to visit all nodes in the depgraph.
    if (!bJavaOnce)
	{
		bJavaOnce=TRUE;
		bJavaSupported = IsPackageLoaded(PACKAGE_LANGJVA);
	}
	if (bJavaSupported)
	{
		CString strFlavor, strName = pcrBuild->GetConfigurationName();

		VERIFY(g_BldSysIFace.GetFlavourFromConfigName(strName, strFlavor));

		int nSep = strName.Find (_T (" - "));
	    strName = strName.Mid(nSep);
		if (strName == " - Java Virtual Machine " + strFlavor)
            frhStart = NULL;
    }

	//
	// retrieve our actions
	//
	CActionSlobList lstActions;
	UINT aor = AOR_Recurse | AOR_ChkOpts | AOR_PreChk | AOR_ChkInp;
	UINT cmdRetRA = pdepgraph->RetrieveBuildActions(lstActions, lstFrh, g_DummyEC, frhStart, statefilter, aor);
	if (cmdRetRA != CMD_Complete)
	{
		// cannot retrive actions because of error or user cancel
		lstActions.RemoveAll();
		return TRUE;
	}

	return !lstActions.IsEmpty();
}

CMD CBuildEngine::DoBuild
(
	ConfigurationRecord * pcrBuild,																  
 	CErrorContext & EC,
	FileRegHandle frhStart /* = (FileRegHandle)NULL*/,
	DWORD aob /* = AOB_Default */,
	UINT type /* = TOB_Build */,
	BOOL fRecurse /* = TRUE */,
	BOOL fVerbose /* = TRUE */
)
{
	// get our build dependency graph and registry for this config.
	CFileDepGraph * pdepgraph = GetDepGraph(pcrBuild);
	CFileRegistry * pregistry = GetRegistry(pcrBuild);

	BOOL bPreLinkDone = FALSE;
	BOOL bPostBuildDone = FALSE;

	// environment of the build
	CEnvironmentVariableList bldenviron;

	// get the environment from the directories manager
	g_pActiveProject->GetEnvironmentVariables(&bldenviron);
	bldenviron.SetVariables();				// set it

	// what dependencies do we want to build?
	UINT stateFilter = DS_OutOfDate | DS_ForcedOutOfDate;

	/// if we're re-building or forcing a build then build 'everything'
	if (type == TOB_Clean || type == TOB_ReBuild || (aob & AOB_Forced))
		stateFilter |= (DS_Current | DS_DepMissing);

	// return value of the build
	// default is 'complete', assuming finish build passes without 'cancel' or 'error'
 	CMD cmdRet = CMD_Complete;

	// start the passes
	pdepgraph->StartPasses();

//
// perform the build passes
//

// pass number
WORD wPass = 0;

	//
	// retry these actions
	//
	CActionSlobList lstReTryActions;

	// Actions attempted in TOB_Stage.
	CActionSlobList lstStagedActions;


	CString strCommandLines;
	if( g_bHTMLLog )
	    strCommandLines.LoadString(IDS_HTML_COMMANDLINE);
	else
	    strCommandLines.LoadString(IDS_NORMAL_COMMANDLINE);
	    
	// LogTrace("<h3>Command Lines</h3>\n");
	LogTrace(strCommandLines);
PerformBuildPass:

	bPreLinkDone = FALSE;

	// increment our pass number
	wPass++;

	// state filter should get everything on a rebuild all.
	// so there is no need to lookup file changes or Deps.

	if (! (type == TOB_Clean || (aob & AOB_Forced)) ) {
		// make sure we have the v.latest dep. graph state
		// so flush any possible file change notifications
		FileChangeDeQ();
	
		//and possible dep update
		FileItemDepUpdateDeQAll();
	}

	//
	// retrieve our actions (prepend with our retry actions)
	//
	CActionSlobList lstActions;
	lstActions.AddTail(&lstReTryActions);
	lstReTryActions.RemoveAll();

	UINT aor = fRecurse ? AOR_Recurse : AOR_None;

	// check option changes and input changes on first pass
	if (wPass == 1)	aor |= (AOR_ChkOpts | AOR_ChkInp);

	// verbose?
	if (fVerbose)	aor |= AOR_Verbose;

	CPtrList lstFrh;
	UINT cNonDeferred = 0;
	

	CFileRegFile::AllowRetryFailedWatches(TRUE);

	UINT cmdRetRA = pdepgraph->RetrieveBuildActions(lstActions, lstFrh, EC, frhStart, stateFilter, aor);
	if (cmdRetRA != CMD_Complete)
	{
		// cannot retrive actions because of error or user cancel
		cmdRet = cmdRetRA;
		goto EndBuildPass;
	}

#ifdef _DEBUG
	// dump our actions before processing
	lstActions.Dump(afxDump);
#endif // _DEBUG

	//
	// post-process our actions
	//
	{
		// post-process stage 1.

		POSITION posSentinel = (POSITION)NULL;
		POSITION pos = lstActions.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			// get the next action and remember its position
			POSITION posOld = pos;
			CActionSlob * pAction = (CActionSlob *)lstActions.GetNext(pos);
			
			if (posSentinel == posOld)
				break;	// we're done
			
			// o shove all deferred tools to the end
			if (pAction->m_pTool->m_fDeferredTool)
			{
				// remove in preparation for a replace
				lstActions.RemoveAt(posOld);

				DAB dab = pAction->m_pTool->DeferredAction(pAction);
				if (dab == dabDeferred || dab == dabNeverBuild)
				{
					// don't use this one, don't replace
					continue;
				}

				ASSERT (dab == dabNotDeferred);

				// replace at end
				POSITION posTmp = lstActions.AddTail(pAction);

				if (posSentinel == (POSITION)NULL)
					posSentinel = posTmp;
			}
			else
			{
				cNonDeferred++;	// no
			}
		} 

		// post-process stage 2.

		// don't do deferred tools until no-more non-deferred tools
		// and in this case move them to the re-try list from the current
		if (cNonDeferred != 0)
		{
			while (posSentinel != (POSITION)NULL)
			{
				POSITION posOld = posSentinel;
				// move to re-try
				lstReTryActions.AddTail(lstActions.GetNext(posSentinel));

				// remove from current
				lstActions.RemoveAt(posOld);
			}
		}

#if 0
		// post-process stage 3.
		POSITION schmoozePos = NULL;

		// pack together items with the same tool
		pos = lstActions.GetTailPosition();
		while (pos != (POSITION)NULL)
		{
			CBuildTool * pTool = ((CActionSlob *)lstActions.GetPrev(pos))->BuildTool();

			// last one
			if (pos == (POSITION)NULL)
				continue;

			// already packed?
			CBuildTool * pToolPrev = ((CActionSlob *)lstActions.GetAt(pos))->BuildTool();
			if (pToolPrev == pTool)
				continue;

			// don't re-order these custom-build steps
			if ( pTool->IsKindOf(RUNTIME_CLASS(CCustomBuildTool)) ){
					continue;
			}

			if ( pTool->IsKindOf(RUNTIME_CLASS(CSpecialBuildTool)) ){
					continue;
			}

			// after we have found the schmooze tool all custom-build steps
			// can be pushed to the front
			if (pTool->IsKindOf(RUNTIME_CLASS(CSchmoozeTool))){
				schmoozePos = pos;
			}
			

			POSITION posTest = pos;
			while (posTest != (POSITION)NULL)
			{
				POSITION posCurr = posTest;
				CActionSlob * pAction = (CActionSlob *)lstActions.GetPrev(posTest);

				if (pAction->BuildTool() == pTool && posCurr != pos)
				{
					lstActions.RemoveAt(posCurr);
					if ( pTool->IsKindOf(RUNTIME_CLASS(CCCompilerTool)) && schmoozePos )
						lstActions.InsertBefore(schmoozePos, pAction);
					else
						lstActions.InsertAfter(pos, pAction);
				}
			}
		}
#else	
		// put all actions in their positional buckets
		#define MAX_TOOL_BUCKET 7
		CObList bucket[MAX_TOOL_BUCKET];
		pos = lstActions.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CActionSlob *pAction = (CActionSlob *)lstActions.GetNext(pos);
			CBuildTool * pTool = pAction->BuildTool();
			CProjItem *pItem =pAction->Item();
			if( pItem->IsKindOf(RUNTIME_CLASS(CProject)) || pItem->IsKindOf(RUNTIME_CLASS(CTargetItem)) ){
				bucket[1].AddTail(pAction);
			} else {
				bucket[pTool->GetOrder()].AddTail(pAction);
			}
		}
		// now clear the original list and join all the seperate lists together.
		lstActions.RemoveAll();
		for( int i=(MAX_TOOL_BUCKET-1); i>=0; i-- ){
			lstActions.AddTail( &(bucket[i]) );
		}
#endif
	}
	
#ifdef _DEBUG
	// dump our actions after processing
	lstActions.Dump(afxDump);
#endif // _DEBUG
		

	// anything still to do?
	if (lstActions.IsEmpty())
		goto EndBuildPass;

	//
	// perform the actions for each of our stages
	//
	{
		// TOB_Pre -> TOB_Stage -> TOB_Post
		UINT lastStage;
		if( type == TOB_Clean )
			lastStage = TOB_Pre;
		else
			lastStage = TOB_Post;

		for (UINT stage = TOB_Pre; stage <= lastStage; stage++)
		{
			// batch?
			if (stage == TOB_Stage)
			{
				// don't batch the defered ones
				if (cNonDeferred != 0)
					VERIFY(BatchCmdLines() == CMD_Complete);
			}

			CActionSlobList lstToolActions;
			POSITION pos = lstActions.GetHeadPosition();
			while (pos != (POSITION)NULL)
			{
				// collect together our actions for the same tool
				CBuildTool * pTool = (CBuildTool *)NULL;
				while (pos != (POSITION)NULL)
				{
					POSITION posOld = pos;
					CActionSlob * pAction = (CActionSlob *)lstActions.GetNext(pos);

					// o can we do this action now if errors encountered?
					if (cmdRet == CMD_Error && !pAction->m_pTool->m_fPerformIfAble)
						// no, so ignore
						continue;

					if (pTool == (CBuildTool *)NULL || pAction->m_pTool == pTool)
					{
						if (pTool == (CBuildTool *)NULL)
						{
							// init. our state
							lstToolActions.RemoveAll();
							pTool = pAction->m_pTool;
						}

						// add this to the tool's list of actions
						lstToolActions.AddTail(pAction);
					}
					else
					{
						// backtrack and breakout
						pos = posOld;
						break;
					}
				}

				// any tool to perform for this stage?
				if (pTool == (CBuildTool *)NULL)
					continue;

				if (TOB_Stage==stage && !bPreLinkDone && pTool->IsKindOf(RUNTIME_CLASS(CLinkerTool)))
				{
					cmdRet = DoSpecialCommandLines( pcrBuild->m_pOwner, TRUE, aob, EC );
					if ( CMD_Complete != cmdRet )
						goto EndBuildPass;

					bPreLinkDone =  TRUE;
				}

				// perform the stage of this build
				UINT actRet = pTool->PerformBuildActions(type, stage, lstToolActions, aob, EC);

				// Add lstToolActions to lstStagedActions.
				if (TOB_Stage==stage)
				{
					lstStagedActions.AddTail(&lstToolActions);
				}

				if (TOB_Stage==stage && ACT_Complete==actRet && !bPostBuildDone && NULL==pos && NULL==frhStart)
				{
					bPostBuildDone =  TRUE;
				}

				switch (actRet)
				{
					case ACT_Canceled:
						cmdRet = CMD_Canceled;
						// cancel whole build
						goto EndBuildPass;
						break;

					case ACT_Error:
						cmdRet = CMD_Error;
						// continue with the build passes in case any tools
						// can still 'Perform If Able' regardless of errors
						break;

					case ACT_Complete:
						// cmdRet = CMD_Complete;	// Don't set if already had an error
						break;

					default:
						ASSERT(FALSE);
						break;
				}
			}

			// flush batch?
			if (stage == TOB_Stage)
			{
				CString strOutputWindow;
				if( g_bHTMLLog )
					strOutputWindow.LoadString(IDS_HTML_OUTPUTWINDOW);
				else
					strOutputWindow.LoadString(IDS_NORMAL_OUTPUTWINDOW);
				LogTrace(strOutputWindow);
				if (cmdRet == CMD_Complete)
				{
			 		cmdRet = BatchCmdLines(FALSE);
				}
				else
				{
					// clear any still batched but not flushed cmd lines now
					ClearCmdLines();
				}
	
				if (cmdRet == CMD_Canceled)
				{
					// cancel whole build
					goto EndBuildPass;
				}
			}
		}
	}

	// now just do a build, as the first pass set us up for a rebuild
	stateFilter = DS_OutOfDate;

	goto PerformBuildPass;

//
// End the build passes
//
EndBuildPass:

	if (cmdRet == CMD_Complete && bPostBuildDone)
	{
		cmdRet = DoSpecialCommandLines( pcrBuild->m_pOwner, FALSE, aob, EC );
	}

	// Update dependencies
	if (g_bNewUpdateDepModel && !lstStagedActions.IsEmpty())
	{
		// Update files.
		FileChangeDeQ();

		IMreDependencies* pMreDepend = NULL;
		BOOL fRefreshDepCtr = FALSE;
		CActionSlob* pFirstAction = (CActionSlob*)lstStagedActions.GetHead();
		CProject * pOurProject = pFirstAction->m_pItem->GetProject();
		CPath pathIdb = pOurProject->GetMrePath(pFirstAction->m_pcrOur);
		if (!pathIdb.IsEmpty())
		{
			// Minimal rebuild dependency interface.
			pMreDepend = 
				IMreDependencies::GetMreDependenciesIFace((LPCTSTR)pathIdb);
		}

		// Update dependencies.
		// Set up a new cache for this target			
		g_ScannerCache.BeginCache();


		POSITION posCurrentAction = lstStagedActions.GetHeadPosition();

		while (NULL!=posCurrentAction)
		{
			CActionSlob* pCurrentAction = (CActionSlob*)lstStagedActions.GetNext(posCurrentAction);

			ASSERT(NULL!=pCurrentAction);

			fRefreshDepCtr = pCurrentAction->UpdateDepInfo() || fRefreshDepCtr;
			fRefreshDepCtr = TRUE;	// Always.
		}

		g_ScannerCache.EndCache();
		if (NULL!=pMreDepend)
			pMreDepend->Release();

		
// FAST_EXPORT
		const CPtrArray * pCfgArray = pOurProject->GetConfigArray();
		int i, isize = pCfgArray->GetSize();
		for (i = 0; i < isize; i++)
		{
			if (pcrBuild->m_pBaseRecord == (ConfigurationRecord *)pCfgArray->GetAt(i))
			{
				pOurProject->m_nScannedConfigIndex = i;
				break;
			}
		}
// FAST_EXPORT

		// Refresh dependency folder.
		if (NULL!=pFirstAction->m_pItem && fRefreshDepCtr)
			pFirstAction->m_pItem->GetTarget()->RefreshDependencyContainer();
	}

	// clear any still batched but not flushed cmd lines now
	ClearCmdLines();

	// nuke any temporary files created as a part of this build
	NukeTempFiles(EC);

	bldenviron.ResetVariables ();	// may do nothing

	// Force any failed file watches to be re-tried (we might have created the files
	// during the build).
	CFileRegFile::AllowRetryFailedWatches(TRUE);

	return cmdRet;
}

STDMETHODIMP_(ULONG) CBSAction::XAction::AddRef(){
	METHOD_PROLOGUE_EX(CBSAction, Action);
	return pThis->ExternalAddRef();
};

STDMETHODIMP_(ULONG) CBSAction::XAction::Release(){
	METHOD_PROLOGUE_EX(CBSAction, Action);
	return pThis->ExternalRelease();
};

STDMETHODIMP CBSAction::XAction::QueryInterface(REFIID iid, LPVOID far * ppvObj){
	METHOD_PROLOGUE_EX(CBSAction, Action);
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
};


STDMETHODIMP CBSAction::XAction::GetInputs(LPBUILDFILESET *ppFileSet){
	METHOD_PROLOGUE_EX(CBSAction, Action);

	if( pThis->pAction ) {
		*ppFileSet = pThis->pAction->GetInput();
		return NOERROR;
	} else {
		*ppFileSet = NULL;
		return E_UNEXPECTED;
	}

};

STDMETHODIMP CBSAction::XAction::GetOutputs(LPBUILDFILESET *ppFileSet){
	METHOD_PROLOGUE_EX(CBSAction, Action);

	if( pThis->pAction ) {
		*ppFileSet = pThis->pAction->GetOutput();
		return NOERROR;
	} else {
		*ppFileSet = NULL;
		return E_UNEXPECTED;
	}

};

STDMETHODIMP CBSAction::XAction::GetCommandOptions(CString *pOptions){
	METHOD_PROLOGUE_EX(CBSAction, Action);

	if( pThis->pAction ) {
		CString strOptions;
		pThis->pAction->GetCommandOptions(strOptions);
		*pOptions = strOptions;
		return NOERROR;
	} else {
		return E_UNEXPECTED;
	}

};
STDMETHODIMP CBSAction::XAction::GetTool(LPBUILDTOOL *ppTool ){
	METHOD_PROLOGUE_EX(CBSAction, Action);

	if( pThis->pAction ){
		// get the IBuildTool from the real tool
		CBuildTool *pTool = pThis->pAction->BuildTool();
		*ppTool = pTool->GetToolInterface();
		return NOERROR;
	} else {
		*ppTool = NULL;
		return E_UNEXPECTED;
	}

};

STDMETHODIMP CBSAction::XAction::GetFile(LPBUILDFILE *ppFile){
	METHOD_PROLOGUE_EX(CBSAction, Action);
	if( pThis->pAction ){
		// *ppFile = pThis->pAction->GetFile();
		// FileRegHandle frh = pItem->GetFileRegHandle();
		return NOERROR;
	} else {
		*ppFile = NULL;
		return E_UNEXPECTED;
	}
};

STDMETHODIMP CBSAction::XAction::GetConfiguration(HCONFIGURATION *hCfg){
	METHOD_PROLOGUE_EX(CBSAction, Action);
	if( pThis->pAction ){
		// *hCfg = pThis->pAction->GetConfig();
		*hCfg = NULL;
		return NOERROR;
	} else {
		*hCfg = NULL;
		return E_UNEXPECTED;
	}
};

STDMETHODIMP CBSAction::XAction::AddOutput(const CPath *pPath){
	METHOD_PROLOGUE_EX(CBSAction, Action);

	if( pThis->pAction ){
		pThis->pAction->AddOutput( pPath );
		return NOERROR;
	} else {
		return E_UNEXPECTED;
	}
};


////////////////////////////////////////////////////////////////////////////
// IUnknown methods

STDMETHODIMP_(ULONG) CEnumActions::XEnumActions::AddRef(){
	METHOD_PROLOGUE_EX(CEnumActions, EnumActions);
	return pThis->ExternalAddRef();
};

STDMETHODIMP_(ULONG) CEnumActions::XEnumActions::Release(){
	METHOD_PROLOGUE_EX(CEnumActions, EnumActions);
	return pThis->ExternalRelease();
};

STDMETHODIMP CEnumActions::XEnumActions::QueryInterface(REFIID iid, LPVOID far * ppvObj){
	METHOD_PROLOGUE_EX(CEnumActions, EnumActions);
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
};

////////////////////////////////////////////////////////////////////////////
// IEnumAction methods

STDMETHODIMP CEnumActions::XEnumActions::Next(THIS_ ULONG celt, IBSAction **rgelt, ULONG *pceltFetched){
	METHOD_PROLOGUE_EX(CEnumActions, EnumActions);
 	ULONG i = 0;
	if( pThis->pList ){
		while ( (pThis->pos != (POSITION)NULL) && (i < celt) )
		{
			CActionSlob *pAction = (CActionSlob *)pThis->pList->GetNext(pThis->pos);
			// this does an add ref on the action
			rgelt[i] = pAction->GetInterface();
			i++;
		}
		*pceltFetched = i;
		return NOERROR;
	}
	return E_UNEXPECTED;
};

STDMETHODIMP CEnumActions::XEnumActions::Skip(THIS_ ULONG celt){
	METHOD_PROLOGUE_EX(CEnumActions, EnumActions);
 	ULONG i = 0;
	if( pThis->pList ){
		while ( (pThis->pos != (POSITION)NULL) && (i < celt) )
		{
			pThis->pList->GetNext(pThis->pos);
			i++;
		}
		return NOERROR;
	}
	return E_UNEXPECTED;
};

STDMETHODIMP CEnumActions::XEnumActions::Reset(THIS){
	METHOD_PROLOGUE_EX(CEnumActions, EnumActions);
	if( pThis->pList ) {
		pThis->pos = pThis->pList->GetHeadPosition();
	}
	return NOERROR;
};

STDMETHODIMP CEnumActions::XEnumActions::Clone(THIS_ IEnumBSActions **ppenum){
	METHOD_PROLOGUE_EX(CEnumActions, EnumActions);
	return E_NOTIMPL;
};
