/////////////////////////////////////////////////////////////////////////////
//
//	CFileRegEntry, CFileRegSet, CFileRegFilter, CFileRegFile, CProjFileRegistry
//
//	File registry database classes
//
//	History:
//	Date						Who						What
//	07/20/94					colint					Cleaned up existing
//														file registry and
//														added file filters.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop
#include "tchar.h"
#include "path.h"
#include "pfilereg.h"
#include "exttarg.h"

#include "fcntl.h"

#include <utilbld_.h>
#include <prjapi.h>
#include <prjguid.h>
#include <sys\types.h>
#include <sys\utime.h>
#include "oleref.h"

#ifdef _WIN32
#define _SUSHI_PROJECT
#include "project.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifdef _DEBUG
#include <crtdbg.h>
#include "memstat.h"
#endif

#ifdef _BLD_REG_HISTORY
//#pragma message ("INFO : pfilereg.cpp : Using file registry history debugging")
#endif

extern BOOL g_fCreatingProject;

/////////////////////////////////////////////////////////////////////////////
// Local heap

#ifdef USE_LOCAL_BUILD_HEAP
CLclHeap::CLclHeap(USHORT cPages /* = 0 */, BOOL fCreate /* = FALSE */)
{
	m_hHeap = (HANDLE)NULL;	// not created yet
	m_cPages = cPages;

	// create now?
	if (fCreate) VERIFY(Create());
}

CLclHeap::~CLclHeap()
{
	// need to destroy?
	if (m_hHeap != (HANDLE)NULL) VERIFY(Destroy());
}

// create + destroy our heap
BOOL CLclHeap::Create()
{
	ASSERT(m_hHeap == (HANDLE)NULL);

	m_hHeap = ::HeapCreate(HEAP_NO_SERIALIZE, m_cPages * 4096, 0);
	return m_hHeap != (HANDLE)NULL;
}

BOOL CLclHeap::Destroy()
{
	ASSERT(m_hHeap != (HANDLE)NULL);

	BOOL fRet = ::HeapDestroy(m_hHeap);
	m_hHeap = (HANDLE)NULL;
	return fRet;
}

#ifdef _DEBUG
// dump heap statistics
void CLclHeap::Dump(CDumpContext & dc) 
{
	// show our heap information
	TCHAR buf[256];
	_stprintf(buf, "0x%lx", m_hHeap);
	 
	dc << "Heap : " << buf;
	dc << " has allocated " << (DWORD)m_lAlloc << " bytes\n";

	// get heap information
	PROCESS_HEAP_ENTRY infoHeap;
	infoHeap.lpData = NULL;	// initiate

	// accumulate info.
	DWORD cbData = 0, cbOverhead = 0, cbCommitted = 0, cbUnCommitted = 0;
	
	while (::HeapWalk(m_hHeap, &infoHeap))
	{
		ASSERT(!(infoHeap.wFlags & PROCESS_HEAP_ENTRY_DDESHARE));	// not supported!
		ASSERT(!(infoHeap.wFlags & PROCESS_HEAP_ENTRY_MOVEABLE));	// not supported!

		{
			if (infoHeap.wFlags & PROCESS_HEAP_REGION)
			{
				// accum. info. for this region
				cbCommitted += infoHeap.Region.dwCommittedSize;
				cbUnCommitted += infoHeap.Region.dwUnCommittedSize;
				cbOverhead += infoHeap.cbOverhead;
			}

			if (infoHeap.wFlags & PROCESS_HEAP_ENTRY_BUSY)
			{
				// accum. info. for this block
				cbData += infoHeap.cbData;
				cbOverhead += infoHeap.cbOverhead;
			}
		}
	}

	DWORD dw = ::GetLastError();
	if (dw == ERROR_NO_MORE_ITEMS)
	{
		dc << " Data size    : " << cbData << " bytes\n";
		dc << " Overhead     : " << cbOverhead << " bytes\n";
		dc << " Committed    : " << cbCommitted << " bytes (" << div(cbCommitted, 4096).quot << " pages)\n";
		dc << " UnCommitted  : " << cbUnCommitted << " bytes (" << div(cbUnCommitted, 4096).quot << " pages)\n";
	}
	else if (dw == ERROR_CALL_NOT_IMPLEMENTED)
	{
		dc << "Statistics unavailable Win32 API ::HeapWalk not implemented";
	}
	else
	{
		dc << "!! Error retrieving stats !!";
	}
}
#endif // _DEBUG
#endif // USE_LOCAL_BUILD_HEAP

#ifdef _BLD_REG_HISTORY
void CFRHHistory::Dump(CDumpContext &DC) const
{
    // Dump frh reference counting history here

    if (m_bAddRef)
        DC << "Add Ref\n";
    else
        DC << "Release Ref\n";

    DC << "File: " << m_szFile << "\n";
    DC << "Line: " << m_nLine << "\n";

    if (m_pNext)
        m_pNext->Dump(DC);
}
#endif

#ifdef _DEBUG
TCHAR *FRHToName( FileRegHandle  frh ) {
	TCHAR *pch = (TCHAR *)((CPath *)g_FileRegistry.GetRegEntry(frh)->GetFilePath())->GetFullPath();
	return pch;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// Setting this flag to TRUE will prevent any further requests for file
// change notification, eg. it is used during builder creation
BOOL g_FileRegDisableFC = FALSE;
BOOL g_bFileDepULQInUse = FALSE;
BOOL g_bUpdateDep = FALSE;
BOOL g_bInProjLoad = FALSE;
BOOL g_bExcludedNoWarning = FALSE;

/////////////////////////////////////////////////////////////////////////////
// Our queue of file registry entry changes
class CSafeAttribList : public CPtrList {
public:
    ~CSafeAttribList(){
	while ( !IsEmpty() ) {
		S_ATTRIB * ptr = (S_ATTRIB *) GetHead();
#ifdef REFCOUNT_WORK
		ptr->_frh->ReleaseFRHRef();
#endif
		delete ptr;
		RemoveHead();
	}
    }
};

CSafeAttribList g_FileChangeQ;

//queue which has all the info needed for update dep
CObList g_FileDepUpdateListQ;
CObList AFX_DATA_EXPORT g_FileForceUpdateListQ;

CTestableSection g_sectionFileChangeQ;

CTestableSection g_sectionDepUpdateQ;

#ifndef REFCOUNT_WORK
CTestableSection CFileRegistry::g_sectionFileMap;
#else
CTestableSection CFileRegFile::g_sectionFileMap;
#endif

void FileChangeQ(FileRegHandle frh, BOOL fInSccOp /* = FALSE */)
{
	// q the file?
	if (frh != (FileRegHandle)NULL)
	{
		{
			// Ensure sequential access to file change Q
			CritSectionT cs(g_sectionFileChangeQ);
 
#ifndef REFCOUNT_WORK
			// Ensure sequential access to file map
			CritSectionT cs2(CFileRegistry::g_sectionFileMap);
 
			// find the registry entry?
			if (g_FileRegistry.LookupFile(frh->GetFilePath()) == frh)
			{
				// allocate and cache
				S_ATTRIB * pattrib = new S_ATTRIB;
				pattrib->_frh = frh;
				pattrib->_fInSccOp = fInSccOp;

				// add to our Q
				g_FileChangeQ.AddTail((void *)pattrib);

				// FUTURE: add ref.?
			}
#else
			// allocate and cache
			S_ATTRIB * pattrib = new S_ATTRIB;
			pattrib->_frh = frh;
			pattrib->_frh->AddFRHRef();
			pattrib->_fInSccOp = fInSccOp;

			// add to our Q
			g_FileChangeQ.AddHead((void *)pattrib);
#endif
		}


		// We are done.
		ASSERT(g_bNewUpdateDepModel);
		if (g_bNewUpdateDepModel)
			return;

#if 0	// Unused code.
		// figure out if this is non C syntax file, may change later		
		TCHAR *pch = (TCHAR *)frh->GetFilePath()->GetExtension();

		if (pch && ((_tcsicmp(pch, ".rc")==0) || (_tcsicmp(pch, ".r")==0) || (_tcsicmp(pch, ".odl")==0) ||
			(_tcsicmp(pch, ".f")==0) || (_tcsicmp(pch, ".for")==0) || (_tcsicmp(pch, ".f90")==0) ))
		{
			// not C-Syntax file, we need to update deps
			ASSERT(!g_bFileDepULQInUse);
			FileItemDepUpdateQ(NULL, NULL, frh, g_FileDepUpdateListQ);
		}
#endif
	}
}

int FileChangeDeQ()
{
	// this rc file
	NOTIFYINFO Notify;
	const CPath * pPath;

	Notify.hFileSet = (HFILESET)NULL;
	Notify.pPkgProject = NULL; // UNUSED

	BOOL fNotifyBldrFileChng = FALSE;
	BOOL fInSccOp = FALSE;
	CString strExt;

	{
		// Ensure sequential access to file change Q
		CritSectionT cs(g_sectionFileChangeQ);
 
#ifndef REFCOUNT_WORK
		// Ensure sequential access to file map
		CritSectionT cs2(CFileRegistry::g_sectionFileMap);
#endif

		// anything to do?
		POSITION pos = g_FileChangeQ.GetHeadPosition();
		if (pos != (POSITION)NULL)
		{
			// while de-q'ing don't let the registry try
			// to FAddFile() to the file change thread
			// its inefficient
			BOOL oldFileRegDisableFC = g_FileRegDisableFC;
			g_FileRegDisableFC = TRUE;

			// refresh attributes for q'd files
			while (pos != (POSITION)NULL)
			{
				S_ATTRIB * pattrib = (S_ATTRIB *)g_FileChangeQ.GetAt(pos);

				// remove this one
				g_FileChangeQ.RemoveAt(pos);

				FileRegHandle frh = pattrib->_frh;
				if (frh != NULL)
				{
					// set the current attributes
					if (((CFileRegFile *)g_FileRegistry.GetRegEntry(pattrib->_frh))->RefreshAttributes())
					{

						// only do this if it actually changed
						pPath = ((CFileRegFile *)g_FileRegistry.GetRegEntry(pattrib->_frh))->GetFilePath();
						strExt = pPath->GetExtension();
						// is this the builder?
						if (strExt.CompareNoCase(BUILDER_EXT)==0)
						{
							if (pPath->ExistsOnDisk())
							{
								Notify.lstFiles.AddTail((CPath *)pPath);
								fNotifyBldrFileChng = TRUE;
								// REVIEW: these could be different
								fInSccOp = fInSccOp || (pattrib->_fInSccOp != 0);
							}
							else
							{
								// REVIEW: warn user
								// dsp file has been deleted, so mark as dirty
								CObList * pList = g_FileRegistry.GetRegEntry(pattrib->_frh)->plstFileItems;
								if (pList != NULL)
								{
									ASSERT(pList->GetCount() == 1);
									CProject * pProject = (CProject *)pList->GetHead();
									ASSERT(pProject);
									if (pProject != NULL)
										pProject->DirtyProject();
								}
							}
						}
					}
#ifdef REFCOUNT_WORK
					frh->ReleaseFRHRef();
#endif
				}

				// de-allocate
				delete pattrib;

				// FUTURE: release ref.?

				// next...always use head, somebody may be deleting items
				// from us in ->SetAttributes() as above
				pos = g_FileChangeQ.GetHeadPosition();
			}

			g_FileChangeQ.RemoveAll();

			g_FileRegDisableFC = oldFileRegDisableFC;
		}
		else
		{
			return FALSE;
		}
	}

	// Notify of external builder file change?
	if (fNotifyBldrFileChng)
	{
		Notify.hFileSet = (HFILESET)fInSccOp;  // hack
		theApp.NotifyPackages(PN_EXTCHNG_PROJECT, (void *)&Notify);
	}

	return TRUE;
}

// add/remove to/from a list file items needed to upate deps
void FileItemDepUpdateQ
(
	CFileItem * pItem,
	CPtrList * pdepList,
	FileRegHandle frh,
	CObList & updateList,
	BOOL fRemove, /* = FALSE */
	HBLDTARGET hTarg /* = NO_TARGET */
)
{
	ASSERT(!g_bNewUpdateDepModel);
	ASSERT(NULL==pdepList);
	ASSERT((&updateList) == &g_FileForceUpdateListQ);

	CritSectionT cs(g_sectionDepUpdateQ);
	if ((&updateList) == &g_FileDepUpdateListQ)
	{
		ASSERT(!g_bFileDepULQInUse);
		g_bFileDepULQInUse = TRUE;
	}
	CDepUpdateRec * pdepRec;

	// remove?
	if (fRemove)
	{
		POSITION pos = updateList.GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			POSITION posCurr = pos;
			pdepRec = (CDepUpdateRec *)updateList.GetNext(pos);
			if (pdepRec &&
				pdepRec->m_pFileItem == pItem)
			{
				updateList.RemoveAt(posCurr);
				delete pdepRec;
			}
		}	
	}
	// add...
	else
	{
		pdepRec = new CDepUpdateRec;
		pdepRec->m_pFileItem = pItem;
		pdepRec->m_pdepList = pdepList;
		pdepRec->m_frh = frh;
		pdepRec->m_hTarg = (UINT)hTarg;

		updateList.AddTail(pdepRec);
	}
	if ((&updateList) == &g_FileDepUpdateListQ)
	{
		g_bFileDepULQInUse = FALSE;
	}
}

BOOL ProcessDepUpdate(CDepUpdateRec *pdepRec, CProject *pActiveProject)
{
	// Should not reach this code when using new model.
	ASSERT(!g_bNewUpdateDepModel);

#if 0	// Unused code.

	if (pdepRec && 
		(pdepRec->m_pdepList != (CPtrList *)NULL) &&
		(pdepRec->m_pFileItem != (CFileItem *)NULL) &&
		(pdepRec->m_frh == (FileRegHandle)NULL)	)
	{
		// This is queued from the update dep calls from the parser
		// C syntax files only
		CProject * pProject = pdepRec->m_pFileItem->GetProject();
		if ((pProject != (CProject *)NULL) && (pProject == pActiveProject))
		{
			CString strTarget;
			g_BldSysIFace.GetTargetName((HBLDTARGET)pdepRec->m_hTarg, strTarget, (HBUILDER)pActiveProject);
			CTargetItem * pTarget = (CTargetItem *)g_BldSysIFace.CnvHTarget((HBUILDER)pActiveProject, (HBLDTARGET)pdepRec->m_hTarg);
			
			if (pTarget != (CTargetItem *)NULL)
			{
				ConfigurationRecord * pcrOld = pProject->GetActiveConfig();
				ConfigurationRecord *pcr = pTarget->ConfigRecordFromConfigName((const TCHAR *)strTarget);

				ASSERT(pcr);
		 		if (pcr->IsValid())
				{
					CProjTempConfigChange projTempConfigChange(pProject);
					projTempConfigChange.ChangeConfig((ConfigurationRecord*)pcr->m_pBaseRecord);

					pdepRec->m_pFileItem->UpdateDependencies(pdepRec->m_pdepList);
					pTarget->RefreshDependencyContainer();
				}
			}
		}
		else
		{
			// NOTE: This assert is really harmless.
			//ASSERT(FALSE);	// item removed from project and not de-Q'd?
		}

		// remove the depList
		pdepRec->m_pdepList->RemoveAll();

		delete pdepRec->m_pdepList;
		pdepRec->m_pdepList = NULL; // to avoid duplicate deletion
	}
	// following are non-C syntax files case
	else if (pdepRec && (pdepRec->m_pFileItem != (CFileItem *)NULL) &&
			 (pdepRec->m_frh != NULL) )
	{
		// we know the FileItem, we called from mirrored or file move into proj dep update
		CProject * pProject = pdepRec->m_pFileItem->GetProject();
		if (pProject != (CProject *)NULL && (pProject == pActiveProject) )
		{
			CTargetItem * pTarget = pdepRec->m_pFileItem->GetTarget();
			ASSERT(pTarget);

			CProjTempConfigChange projTempConfigChange(pProject);

			const CPtrArray * paryConfigs = pTarget->GetConfigArray();
			ASSERT(paryConfigs);

			int i = 0, iLast = paryConfigs->GetUpperBound();
		
			//Loop through all the configuration for this target, so all the deps updated
			g_ScannerCache.BeginCache();
			ConfigurationRecord *pcr;
			BOOL fReFreshDepCtr = FALSE;
			while (i <= iLast)
			{
				pcr = (ConfigurationRecord *) paryConfigs->GetAt(i++);	// get this one and advance
				ASSERT(pcr);

				if (pcr->IsValid())
				{
					projTempConfigChange.ChangeConfig((ConfigurationRecord*)pcr->m_pBaseRecord);
					if (!pdepRec->m_pFileItem->ScanDependencies())
					{
						// scan failed, wait for next round to do it again
						g_ScannerCache.EndCache();
						g_bFileDepULQInUse = FALSE;
						return FALSE;
					}
					fReFreshDepCtr = TRUE;
				}
			}

			g_ScannerCache.EndCache();
			projTempConfigChange.Release();

			if (fReFreshDepCtr)
				pTarget->RefreshAllMirroredDepCntrs();
		}
		else
		{
			ASSERT(FALSE);	// item removed from project and not de-Q'd?
		}
	}
	else if (pdepRec && (pdepRec->m_pFileItem == (CFileItem *)NULL) &&
			 (pdepRec->m_frh != NULL) )
	{
		// we are called from file save notification
		CObList *oblist;

		// Get current project
		CProject * pProject = pActiveProject;
		if (pProject != (CProject *)NULL)
		{
			CProjTempConfigChange projTempConfigChange(pProject);
					
			// Get file items for this frh
			// Is this right, it seems we are getting called repeated???
			oblist = g_FileRegistry.GetRegEntry(pdepRec->m_frh)->plstFileItems;
			if (oblist)
			{				
				POSITION pos = oblist->GetHeadPosition();		
				while (pos != (POSITION)NULL )
				{
					CFileItem *pItem = (CFileItem *)oblist->GetNext(pos);

					if (pItem)
					{
						// update for all project configurations
						CTargetItem * pTarget = pItem->GetTarget();
						
						if (pTarget != (CTargetItem *)NULL)
						{
							const CPtrArray * pArrayCfg = pTarget->GetConfigArray();
							ASSERT(pArrayCfg);

							int i=0, iLast = pArrayCfg->GetUpperBound();
							ConfigurationRecord *pcr;
							BOOL fReFreshDepCtr = FALSE;
							g_ScannerCache.BeginCache();
							while (i <= iLast)
							{
								pcr = (ConfigurationRecord *) pArrayCfg->GetAt(i++);	// get this one and advance
								ASSERT(pcr);

								if (pcr->IsValid())
								{
									projTempConfigChange.ChangeConfig((ConfigurationRecord*)pcr->m_pBaseRecord);
									if (!pItem->ScanDependencies())
									{
										// scan failed, wait for next round to do it again
										g_ScannerCache.EndCache();
										g_bFileDepULQInUse = FALSE;
										return FALSE;
									}
									fReFreshDepCtr = TRUE;
									//pTarget->RefreshDependencyContainer();
								}
							}
							g_ScannerCache.EndCache();
							if (fReFreshDepCtr)
								pTarget->RefreshAllMirroredDepCntrs();
						}
					}
				}
			}
		}
	}
#endif	// Unused code.
	return TRUE;
}

// update deps for the item on the list
// one at a time, to keep the other event respond time
int FileItemDepUpdateDeQ()
{
	ASSERT(g_bNewUpdateDepModel);
	if (g_bNewUpdateDepModel)
	{
		ASSERT(g_FileDepUpdateListQ.IsEmpty());
		return FALSE;
	}

#if 0	// Unused code.

	LPSOURCECONTROLSTATUS pInterface;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControlStatus, (LPVOID FAR*)&pInterface)));
	BOOL bIsSccOpInProgress = (pInterface->IsSccOpInProgress() == S_OK);
	pInterface->Release();
	if (bIsSccOpInProgress)
		return FALSE; // don't try this if we're in the middle of an scc op

	CDepUpdateRec *pdepRec = NULL;

	POSITION pos = g_FileDepUpdateListQ.GetHeadPosition();

	if (pos == (POSITION)NULL)
		return FALSE; // nothing to do

	CProject * pActiveProject = (CProject *)g_BldSysIFace.GetActiveBuilder();

	// This must be at top-level scope!!
	CProjDeferDirty deferDirty(pActiveProject);

	if (pos != (POSITION)NULL) // n.b. do not remove this test, it has a purpose!
	{
		CritSectionT cs(g_sectionDepUpdateQ);
		if (g_bFileDepULQInUse)
		{
			return TRUE; // avoid recursive OnIdle call!!!
		}
		else
			g_bFileDepULQInUse = TRUE;

		pos = g_FileDepUpdateListQ.GetHeadPosition(); // need to update pos after Crit

//		while (pos != (POSITION)NULL)
		if (pos != (POSITION)NULL)
		{
			pdepRec = (CDepUpdateRec *)g_FileDepUpdateListQ.GetNext(pos);
		}
		g_bFileDepULQInUse = FALSE;
	}

	// ProcessDepUpdate is called outside the critical section, since it may result
	// in the creation of new nodes which need dependencies.
	if (pdepRec != NULL && ProcessDepUpdate(pdepRec, pActiveProject))
	{
		CritSectionT cs(g_sectionDepUpdateQ);

		ASSERT(!g_bFileDepULQInUse);
		g_bFileDepULQInUse = TRUE;

		// Delete the item from the list (it should still be at the head).
		POSITION pos = g_FileDepUpdateListQ.GetHeadPosition();
		POSITION posNext = pos;
		if (pdepRec == (CDepUpdateRec *)g_FileDepUpdateListQ.GetNext(posNext))
		{
			g_FileDepUpdateListQ.RemoveAt(pos);
		}
		else
			ASSERT(FALSE);

		RemoveFromForceUpdateQueue(pdepRec);
		delete pdepRec;
		g_bFileDepULQInUse = FALSE;
	}
#endif // Unused code.

	return TRUE; // avoid recursive OnIdle call!!!
}

void RemoveFromForceUpdateQueue(CDepUpdateRec *pdepRec)
{
	//loop through g_FileForceUpdateListQ and remove the files updated from the list
	POSITION pos = g_FileForceUpdateListQ.GetHeadPosition();
	if (pos != (POSITION)NULL)
	{
		while (pos != (POSITION)NULL)
		{
			POSITION posPrev=pos;
			CDepUpdateRec *pdepRecTmp = (CDepUpdateRec *)g_FileForceUpdateListQ.GetNext(pos);
			//remove the record with same file item and target
			if (pdepRecTmp && pdepRec && (pdepRec->m_pFileItem == pdepRecTmp->m_pFileItem) )
				{
					delete pdepRecTmp;			
					g_FileForceUpdateListQ.RemoveAt(posPrev);
					break;
				}
		}
	}
}

// remove items with this target from the queue
void RemoveItemsFromUpdateQueue(UINT hTarget)
{
	//loop through g_FileForceUpdateListQ and remove the files updated from the list
	POSITION pos = g_FileDepUpdateListQ.GetHeadPosition();
	if (pos != (POSITION)NULL)
	{
		CritSectionT cs(g_sectionDepUpdateQ);
		if (g_bFileDepULQInUse)
			return;
		else
			g_bFileDepULQInUse = TRUE;

		pos = g_FileDepUpdateListQ.GetHeadPosition(); // must update after Crit
		while (pos != (POSITION)NULL)
		{
			POSITION posPrev=pos;
			CDepUpdateRec *pdepRecTmp = (CDepUpdateRec *)g_FileDepUpdateListQ.GetNext(pos);
			//remove the record with same file item and target
			if (pdepRecTmp && (pdepRecTmp->m_hTarg == hTarget) )
			{
				// remove the depList
				delete pdepRecTmp;
				g_FileDepUpdateListQ.RemoveAt(posPrev);
			}
		}
		g_bFileDepULQInUse = FALSE;
	}
}


//update the deps if there is any on the g_ForceUpdateListQ
void UpdateBeforeWriteOutMakeFile()
{
	CFileRegFile::AllowRetryFailedWatches(TRUE);

	if (g_FileForceUpdateListQ.GetHeadPosition() != NULL)
	{
		// Should not reach this code when using new model.
//		ASSERT(!g_bNewUpdateDepModel);

		CProject * pActiveProject = (CProject *)g_BldSysIFace.GetActiveBuilder();
		CProjTempConfigChange projTempConfigChange(pActiveProject);

		// Iterate over the list of targets
		CString strTargetName;
		CTargetItem *pTargetCurr;
		pActiveProject->InitTargetEnum();

		// If a file from the target is on the force list, we will ScanDependencies
		// on those files. It is important that we iterate target-wise, because
		// we can use the g_ScannerCache per target and this will speed this operation 
		// considerably.
		while (pActiveProject->NextTargetEnum(strTargetName, pTargetCurr))
		{

			g_ScannerCache.BeginCache( );

			ConfigurationRecord *pcr = pTargetCurr->ConfigRecordFromConfigName(strTargetName);
			ASSERT(pcr != NULL);
			ASSERT(pcr->IsValid());

			// Force this to be our active config.
			projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);

			//loop through g_FileForceUpdateListQ 
			POSITION pos = g_FileForceUpdateListQ.GetHeadPosition();
			BOOL fRefreshDepCtr = FALSE;

			while (pos != (POSITION)NULL)
			{	
				CDepUpdateRec *pdepRec = (CDepUpdateRec *)g_FileForceUpdateListQ.GetNext (pos);

				if (pdepRec && (pdepRec->m_pFileItem != (CFileItem *)NULL) )
				{
					CProject * pProject = pdepRec->m_pFileItem->GetProject();
					if ((pProject != (CProject *)NULL) && (pProject == pActiveProject))
					{
						CTargetItem *pTargetItem = pdepRec->m_pFileItem->GetTarget();
						ASSERT(pTargetItem);

						if (pTargetItem == pTargetCurr)	  // The file belongs to this target.
						{
							pdepRec->m_pFileItem->ScanDependencies();
							fRefreshDepCtr = TRUE;
							//refresh view
							//pTarget->RefreshDependencyContainer();
						}
					}
				}
			}

			if (fRefreshDepCtr)
				pTargetCurr->RefreshAllMirroredDepCntrs();

			g_ScannerCache.EndCache();
		}

		// Free up all the pdepRec here.
		POSITION pos = g_FileForceUpdateListQ.GetHeadPosition();
		while (pos != NULL)
		{
			CDepUpdateRec *pdepRec = (CDepUpdateRec *)g_FileForceUpdateListQ.GetNext(pos);
			delete pdepRec;
		}
 		g_FileForceUpdateListQ.RemoveAll();
	}

	// clean up the auto update queue if there is any
	POSITION pos = g_FileDepUpdateListQ.GetHeadPosition();
	if (pos != (POSITION)NULL)
	{
		// Should not reach this code when using new model.
		ASSERT(!g_bNewUpdateDepModel);

		CritSectionT cs(g_sectionDepUpdateQ);
		if (g_bFileDepULQInUse)
			return;
		else
			g_bFileDepULQInUse = TRUE;
		pos = g_FileDepUpdateListQ.GetHeadPosition(); // must update after Crit
		while (pos != (POSITION)NULL)
		{
			CDepUpdateRec *pdepRec = (CDepUpdateRec *)g_FileDepUpdateListQ.GetNext(pos);
			delete pdepRec;
		} //while

		// done these, remove them
		g_FileDepUpdateListQ.RemoveAll();
		g_bFileDepULQInUse = FALSE;
 	} //if
}

// update deps for the item on the list
void FileItemDepUpdateDeQAll()
{
	POSITION pos= g_FileDepUpdateListQ.GetHeadPosition();

	// Should not have any updates in queue when using new model.
	ASSERT(NULL==pos || !g_bNewUpdateDepModel);

	while (pos != (POSITION)NULL)
	{
		FileItemDepUpdateDeQ();
		pos = g_FileDepUpdateListQ.GetHeadPosition();
	}

    // Make sure anything on our forced list is scanned
    // before we build.
    UpdateBeforeWriteOutMakeFile();
}

void RemoveFileItemFromUpdateQ(CFileItem *pFileItem)
{
	{
		CritSectionT cs(g_sectionDepUpdateQ);

		//loop through g_FileForceUpdateListQ and remove the files updated from the list
		POSITION pos = g_FileForceUpdateListQ.GetHeadPosition();
		if (pos != (POSITION)NULL)
		{
			while (pos != (POSITION)NULL)
			{
				POSITION posPrev=pos;
				CDepUpdateRec *pdepRecTmp = (CDepUpdateRec *)g_FileForceUpdateListQ.GetNext(pos);
				//remove the record with same file item and target
				if (pdepRecTmp && pFileItem && (pFileItem == pdepRecTmp->m_pFileItem) )
				{
					delete pdepRecTmp;
					g_FileForceUpdateListQ.RemoveAt(posPrev);
					//break;
				}
			}
		}

		//loop through g_FileDepUpdateListQ and remove the files updated from the list
		pos = g_FileDepUpdateListQ.GetHeadPosition();
		if (pos != (POSITION)NULL)
		{
			while (pos != (POSITION)NULL)
			{
				POSITION posPrev=pos;
				CDepUpdateRec *pdepRecTmp = (CDepUpdateRec *)g_FileDepUpdateListQ.GetNext(pos);
				//remove the record with same file item and target
				if (pdepRecTmp && pFileItem && (pFileItem == pdepRecTmp->m_pFileItem) )
				{
					delete pdepRecTmp;
					g_FileDepUpdateListQ.RemoveAt(posPrev);
					//break;
				}
			}
		}
	}
}

void BldSrcFileChangeCallBack(const FCItem* pfc, DWORD dwUser)
{
	// Should always be FCItem::opFileChanged.
	ASSERT (pfc->_op == FCItem::opFileChanged);

	// Nothing to do..
	if (pfc->_op != FCItem::opFileChanged)
		return;

#ifndef REFCOUNT_WORK
	FileRegHandle frh = g_FileRegistry.LookupFile(pfc->_strFileName);
#else
	FileRegHandle frh = CFileRegFile::LookupFileHandleByName(pfc->_strFileName);
#endif

	// ignore any file-change we don't know about
	if (frh == (FileRegHandle)NULL)
		return;	// ignore

	// directory deleted?
	if (pfc->_fcb.fDirDel)
	{
		// directory has been deleted, and hence this file of course...
		// this is no longer under file-watch
		((CFileRegFile *)frh)->m_bFileWatch = FALSE;
		((CFileRegFile *)frh)->m_bFileAddedToFC = FALSE;
		((CFileRegFile *)frh)->m_tickFailedWatch = 0;

#if 0
		TRACE("\nDeleted directory of file '%s'", pfc->_strFileName);
#endif
	}
	else
	{
#if 0
		TRACE("\nFile change '%s'", pfc->_strFileName);
#endif
	}

#if 0
	// If this file has been deleted then we can release our ref on it.
	if (pfc->_op == FCItem::opDel)
	{
		// The file change thread has really stopped watching this file
		// so we can release our ref on it.
#ifdef REFCOUNT_WORK
		// Release 2 references, lookup at beginning of this function and file change thread.
 		frh->ReleaseFRHRef();
#endif
 		frh->ReleaseFRHRef();
		return;
	}
#endif
 
	// queue these for processing at a more convenient time
	// ie. when we're in the main thread ;-)

	// Scc doesn't care about mere attribute changes
	FCBits fcTest = pfc->_fcb;
	fcTest.fAttrs = 0; // clearing fAttrs allows quick test if anything else set
	
	// NB. Test for SccOp exactly equal to TRUE is a V4 HACK, since this can
	// be set to -1 in the case where we're saving the file and we want
	// don't want to treat this as an Scc-generated file-change
	ASSERT(NULL!=g_pSccManager);
	int nSccOpInProgress = g_pSccManager->IsOpInProgressValue();
	if (fcTest.fAll != 0) // ignore attribute-only changes
 		FileChangeQ(frh, (nSccOpInProgress==TRUE && (fcTest.fAll != 0)));

	// This is a good time to allow failed file watches to be re-tried.
	CFileRegFile::AllowRetryFailedWatches(FALSE);

#ifdef REFCOUNT_WORK
	// reference from lookup
	frh->ReleaseFRHRef();
#endif
}

#ifdef _BLD_SRC_FC_ASYNC
void BldSrcFileAddedCallBack(const FCItem* pfc)
{
	// Ensure sequential access to file change Q
	CritSectionT cs(g_sectionFileChangeQ);
 
#ifndef REFCOUNT_WORK
	// Ensure sequential access to file map
	CritSectionT cs2(CFileRegistry::g_sectionFileMap);
#endif

	// file reg handle
	S_ATTRIB attrib;
	attrib._frh = (FileRegHandle)pfc->_pvUser1;

#if 0
	TRACE("\nFile async. add attempt '%s'", pfc->_strFileName);
#endif

	// successful addition?
#ifndef REFCOUNT_WORK
	if (pfc->_fAsyncReturn &&	// passed
		g_FileRegistry.LookupFile(attrib._frh->GetFilePath()) == attrib._frh	// we still have it
	   )
#else
	if (pfc->_fAsyncReturn )
#endif
	{
		// got it, get the attributes from the FCItem
		attrib._fOk = TRUE;
		attrib._fExists = pfc->_statNew.dwAttrs != 0;
		attrib._filetime = pfc->_statNew.fcft.filetime;

		// under file-watch
		((CFileRegFile *)attrib._frh)->m_bFileWatch = TRUE;

		// set them
		((CFileRegFile *)g_FileRegistry.GetRegEntry(attrib._frh))->SetAttributes(&attrib);

#if 0
		TRACE(" .... ok!");
#endif
#ifdef REFCOUNT_WORK
		// reference from lookup
 		attrib._frh->ReleaseFRHRef();
#endif
	}
	else
	{
		// try to add assync. again
		((CFileRegFile *)attrib._frh)->m_bFileAddedToFC = FALSE;

		// Decrement file reg ref count, since we failed
		((CFileRegFile *)attrib._frh)->ReleaseFRHRef();

#if 0
		TRACE(" .... failed");
#endif
	}

	// This is a good time to allow failed file watches to be re-tried.
	CFileRegFile::AllowRetryFailedWatches(FALSE);
}
#endif // #ifdef _BLD_SRC_FC_ASYNC

///////////////////////////////////////////////////////////////////////////////
//
//	CFileRegEntry

// file reg. entry local heap
#ifdef USE_LOCAL_BUILD_HEAP
CLclHeap CFileRegEntry::g_heapEntries(20);
#endif

#ifdef REFCOUNT_WORK
static const int s_nRegSetRefCount = 1000;
#endif

IMPLEMENT_DYNAMIC(CFileRegEntry, CObject)

#ifdef REFCOUNT_WORK
#ifdef _DEBUG

// Defining PRESERVE_REGFILE causes CFileRegFile's to not be deleted until
// program exits.  This helps debugging reference count problems.
//#define PRESERVE_REGFILE
// AddFRHRef and ReleaseFRHRef are defined inline in pfilereg.h for non-debug builds.
void CFileRegEntry::AddFRHRef()
{
	m_nRefCount++;
}

void CFileRegEntry::ReleaseFRHRef()
{
	// Keep information around for CFileRegFile
	ASSERT(0!=m_nRefCount);
	if (0==m_nRefCount)
		return;

	m_nRefCount--;
#ifdef PRESERVE_REGFILE
	if (!IsNodeType(nodetypeRegFile))
	{
		if (m_nRefCount == 0)
			SafeDelete();
	}
#else
	if (m_nRefCount == 0)
		SafeDelete();
#endif
}
#endif // _DEBUG
#endif // REFCOUNT_WORK

CFileRegEntry::CFileRegEntry()
{
	// Set our node type
	dwNodeType |= nodetypeRegEntry;

	// Initialize the basic file registry entry properties
	m_nExistsOnDisk = FALSE;

	// Initialize the non informing properties
	m_nRefCount = 0;

#if 0
	// Removed from class.
	m_nProjRefCount = 0;
#endif

#ifndef REFCOUNT_WORK
	m_bRelative = TRUE;
#endif
	m_bMatchCase = FALSE;	
	m_bWantActualCase = FALSE;	
	m_bUseIntDir = FALSE;
	m_bUseOutDir = FALSE;

	// Initialize the update atrributes and dirty bit
	m_bDirty = FALSE;
	m_bUpdateAttribs = TRUE;

	m_bFoundInPath = FALSE;     
	m_bRelativeToProjName = FALSE;     
	m_ich = 0;
	plstFileItems = (CObList *)NULL;
	m_pOrgName = (TCHAR *)NULL;
}

CFileRegEntry::~CFileRegEntry()
{
#ifdef REFCOUNT_WORK
	ASSERT(0==m_nRefCount);
#endif

	// Delete our list of associated file items
	if (plstFileItems != (CObList*) NULL)
		delete plstFileItems;
	if (m_pOrgName)
		free(m_pOrgName);
}

void CFileRegEntry::OnNotify(FileRegHandle hndFileReg, UINT idChange, DWORD dwHint)
{
	ASSERT(FALSE);
}

void CFileRegEntry::InformNotifyRxs(UINT idChange, DWORD dwHint /* = 0 */)
{
#ifndef REFCOUNT_WORK
	// Inform dependants that we have changed in some way!

	// We must make a temporary copy of the dependant list.
	// This is an array if there are few dependants, otherwise it is a dynamically
	// allocated copy.
#define cpvLocal	8
	if (m_Dependants.GetSize() <= cpvLocal)
	{
		// Use an array as the holding area.
		void *rgpv[cpvLocal];
		int ipvLim = 0;
		for (POSITION posT = m_Dependants.GetHeadPosition(); posT != NULL; )
		{
			rgpv[ipvLim++] = m_Dependants.GetNext(posT);
		}
		ASSERT(ipvLim <= cpvLocal);

		for (int ipv = 0; ipv < ipvLim; ipv++)
		{
			CFileRegNotifyRx * pfrerx = (CFileRegNotifyRx*)rgpv[ipv];

			// *** warning: duplicated code ***
			// Handle FRN_LCL_DESTROY 'carefully' because rx's of 
			// may remove themselves from our dependency list!
			if (idChange != FRN_LCL_DESTROY || m_Dependants.Find(pfrerx) != (POSITION)NULL)
				pfrerx->OnNotify(this, idChange, dwHint);
		}
	}
	else
	{
		// Use a temporary copy of the set.
		CSmallPtrSet lstInform;		// used if !fUseArray
		lstInform.Copy(m_Dependants);

		for (POSITION pos = lstInform.GetHeadPosition(); pos != NULL; )
		{
			CFileRegNotifyRx * pfrerx = (CFileRegNotifyRx*) lstInform.GetNext(pos);

			// *** warning: duplicated code ***
			// Handle FRN_LCL_DESTROY 'carefully' because rx's of 
			// may remove themselves from our dependency list!
			if (idChange != FRN_LCL_DESTROY || m_Dependants.Find(pfrerx) != (POSITION)NULL)
				pfrerx->OnNotify(this, idChange, dwHint);
		}
	}
#else // ifndef REFCOUNT_WORK
	for (POSITION pos = m_Dependants.GetHeadPosition(); pos != NULL; )
	{
		CFileRegNotifyRx * pfrerx = (CFileRegNotifyRx*) m_Dependants.GetNext(pos);

		pfrerx->OnNotify(this, idChange, dwHint);
	}
#endif // ifndef REFCOUNT_WORK
}

void CFileRegEntry::AddNotifyRx(CFileRegNotifyRx* pfre)
{
	// Add a notification receiver to our dependency list
	ASSERT(pfre != this);
	POSITION pos = m_Dependants.Find(pfre);
	if (pos == NULL)
		m_Dependants.AddTail(pfre);

#ifdef REFCOUNT_WORK
	ASSERT(pos == NULL);
#endif
}

void CFileRegEntry::RemoveNotifyRx(CFileRegNotifyRx* pfre)
{
#ifdef REFCOUNT_WORK
//	ASSERT(NULL!=m_Dependants.Find(pfre));
#endif

	// Remove a notification receiver from our dependency list
	m_Dependants.RemoveItem(pfre);
}

BOOL CFileRegEntry::SetIntProp(UINT idProp, int nVal)
{
	// set an integer property value
	ASSERT(idProp == P_ExistsOnDisk);	// We only have one bool property

	int nOldVal = m_nExistsOnDisk;

	// any change?
	if (nOldVal != nVal)
	{
		m_nExistsOnDisk = nVal;

		if (!g_bInProjLoad) // skip this during project load
		{
			// notify our dependants that a integer property value has changed
			InformNotifyRxs(idProp, (UINT)nOldVal);
		}
	}

	return TRUE;
}

BOOL CFileRegEntry::GetIntProp(UINT idProp, int& nVal)
{
	// Get an integer property value
	(void)UpdateAttributes();
 	
	ASSERT(idProp == P_ExistsOnDisk);
	nVal = m_nExistsOnDisk;
	return TRUE;
}

#if 0
// inlined in pfilereg.h
long CFileRegEntry::MyCompareFileTime(FILETIME* pTime1, FILETIME* pTime2)
{
	if (pTime1->dwHighDateTime == pTime2->dwHighDateTime)
	{
		if (pTime1->dwLowDateTime == pTime2->dwLowDateTime)
			return 0;

		if (pTime1->dwLowDateTime < pTime2->dwLowDateTime)
			return -1;

		return 1;
	}

	if (pTime1->dwHighDateTime < pTime2->dwHighDateTime)
 		return -1;
	
	return 1;
}
#endif

//#ifndef REFCOUNT_WORK
void CFileRegEntry::AssociateFileItem (CFileItem * pItem)
{
	if (plstFileItems == (CObList *)NULL)
		if ((plstFileItems = new CObList) == (CObList *)NULL)
			return;

	// N.B. assumes no duplicates
	plstFileItems->AddTail(pItem);
}

BOOL CFileRegEntry::DisassociateFileItem (CFileItem * pItem)
{
	if (plstFileItems == (CObList *)NULL)
		return TRUE;	// not in here

	POSITION pos = plstFileItems->GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		POSITION posCurr = pos;
		// N.B. assumes no duplicates
		if (pItem == (CFileItem*)plstFileItems->GetNext(pos))
		{
			plstFileItems->RemoveAt(posCurr);
			if (plstFileItems->IsEmpty())
			{
				delete plstFileItems;
				plstFileItems = (CObList *)NULL;
			}
			return TRUE;	// found it
		}
	}

	return FALSE;	// didn't find it
}
//#endif // ifndef REFCOUNT_WORK

BOOL CFileRegEntry::UpdateAttributes()
{
	BOOL fRefreshed = FALSE;
	if (CFileRegEntry::DoRefresh())
	{
		m_bUpdateAttribs = FALSE;
		fRefreshed = RefreshAttributes();
		m_bUpdateAttribs = TRUE;
	}

	return fRefreshed;
}

#ifdef _DEBUG
void CFileRegEntry::Dump(CDumpContext& dc) const
{
	dc << "{CFileRegEntry@" << (void*)this << ": ";
	dc << "RefCnt:" << m_nRefCount << ";";
	dc << "FileItems:" << (NULL!=plstFileItems?plstFileItems->GetCount():0) << ";";
	dc << "Dependants:" << m_Dependants.GetSize() << "}";
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
//	CFileRegSet

IMPLEMENT_DYNAMIC(CFileRegSet, CFileRegEntry)

CFileRegSet::CFileRegSet(int nHashSize)
	: CFileRegEntry(), m_Contents(nContentsBlockSize)
{
	// Set our node type
	dwNodeType |= nodetypeRegSet;
	m_pmapContents = NULL;			// no map yet
	m_nHashSizeMapContents = nHashSize;
	m_nExistsOnDisk = TRUE;

	// Set the default property values for an empty set
	FILETIME NewestTime;
	
	// Newest date
	NewestTime.dwLowDateTime = NewestTime.dwHighDateTime = 0;

	// If set is empty then it exists...
	SetIntProp(P_ExistsOnDisk, TRUE);
 
	// Set the default property values
	SetTimeProp(P_NewTStamp, NewestTime);

	// Initialize private cache helping members
	m_nNotExists = 0;

	// Calculate contents attribute changes
	// (off by default)
	m_fCalcAttrib = FALSE;

#ifndef REFCOUNT_WORK
	m_pFileRegistry = &g_FileRegistry;
#else
	m_pFileRegistry = NULL;

	// Set at s_nRegSetRefCount so it will never be deleted accidently.
	m_nRefCount = s_nRegSetRefCount;
#endif
}

CFileRegSet::~CFileRegSet()
{
	m_bUpdateAttribs = FALSE;	// don't attempt to update
 	m_fCalcAttrib = FALSE;		// don't re-calc attrib.

	POSITION pos = m_Contents.GetHeadPosition();
	POSITION lastPos;
	while (pos != NULL)
	{
		// WARNING: this code accessses the m_pFileRegistry member.  If the
		// current object is itself a CFileRegistry then this pointer is a
		// self-pointer, and we go into CFileRegistry methods after the
		// CFileRegistry destructor has been called!  This should be changed.
		lastPos = pos;
		FileRegHandle hndFileReg = (FileRegHandle) m_Contents.GetNext(pos);
#ifndef REFCOUNT_WORK
/*
		if( g_bInProjClose ){
			hndFileReg->RemoveNotifyRx(this);
			if ( hndFileReg->IsKindOf(RUNTIME_CLASS(CFileRegFilter)) )
				delete (CFileRegFilter *)hndFileReg;

			// in place of FRN_DESTROY doing this for us
			m_Contents.RemoveAt(lastPos);
		} 
		else {
*/
			RemoveRegHandleI(hndFileReg); // calling directly to level I function to avoid n^2 lookup
/*
		}
*/
		RemoveRegHandle(hndFileReg);
#else //#ifndef REFCOUNT_WORK
		RemoveRegHandleI(hndFileReg); // calling directly to level I function to avoid n^2 lookup
#endif //#ifndef REFCOUNT_WORK
	}

	if (m_pmapContents != NULL)
	{
		delete m_pmapContents;	// one has been allocated, so we must delete it
	}
#ifdef REFCOUNT_WORK
	// Unhook from registry.
	if (NULL!=m_pFileRegistry)
		SetFileRegistry(NULL);

	ASSERT(s_nRegSetRefCount==m_nRefCount);
	m_nRefCount=0;
#endif
}

void CFileRegSet::InitFrhEnum()
{
	// init. the enumerator
	m_posEnum = m_Contents.GetHeadPosition();
	m_fInitSet = TRUE;
}

FileRegHandle CFileRegSet::NextFrh()
{
TryAgain:

	// exhausted?
	if (m_posEnum == (POSITION)NULL)
		return (FileRegHandle)NULL;

	// is this a set?
	POSITION posEnum = m_posEnum;
	FileRegHandle frh = (FileRegHandle)m_Contents.GetNext(posEnum);
	if (((CFileRegEntry *)frh)->IsNodeType(nodetypeRegSet))
	{
		ASSERT(((CFileRegEntry *)frh)->IsKindOf(RUNTIME_CLASS(CFileRegSet)));
		// init. enumerator of this set?
		if (m_fInitSet)
		{
			// init. then remember that we have
			((CFileRegSet *)frh)->InitFrhEnum();
			m_fInitSet = FALSE;
		}

		// set got one?
		if ((frh = ((CFileRegSet *)frh)->NextFrh()) == (FileRegHandle)NULL)
		{
			// reset so that we init. next set
			m_fInitSet = TRUE;

			// advance...
			m_posEnum = posEnum;
			goto TryAgain;
		}
	}
	else
	{
		ASSERT(!(((CFileRegEntry *)frh)->IsKindOf(RUNTIME_CLASS(CFileRegSet))));
		// advance...
		m_posEnum = posEnum;

#ifdef REFCOUNT_WORK
		// The NextFrh() caller needs to ReleaseFRHRef.
		frh->AddFRHRef();
#endif
	}

	return frh;
}

#ifdef REGISTRY_FILEMAP
BOOL
CFileRegSet::RegHandleExists(FileRegHandle hndFileReg) const
{
	WORD w;
	// ASSERT((m_pmapContents==NULL) || ((UINT)m_pmapContents->GetCount() < ((UINT)m_pmapContents->GetHashTableSize() << 1))); // UNDONE
	return m_pmapContents != NULL
		   ? m_pmapContents->Lookup((void *)hndFileReg, w)
		   : (BOOL)m_Contents.Find((void *)hndFileReg);
}
#endif


BOOL CFileRegSet::AddRegHandle(FileRegHandle hndFileReg, BOOL bAddRef /* = TRUE */, BOOL bAddFront /* = FALSE */)
{
	ASSERT(hndFileReg);

#ifdef REFCOUNT_WORK
	// Shouldn't be adding again.
//	ASSERT( !RegHandleExists(hndFileReg) );
	ASSERT( !hndFileReg->IsNodeType(nodetypeRegRegistry) );
	ASSERT( !hndFileReg->IsNodeType(nodetypeRegFilter) );
#endif

	// Already exists in set....return
	if (RegHandleExists(hndFileReg))
		return TRUE;

#ifdef REFCOUNT_WORK
	// Reference held by this file set.
	hndFileReg->AddFRHRef();
#endif

	// This may result in new dependency update requests, so we must not be locking
	// the lists at this time.
	ASSERT(!g_sectionDepUpdateQ.FOwned());

	// Is the CFileRegEntry a CFileRegSet if so then we must
	// set up the associated file registry...
#ifndef REFCOUNT_WORK
	CFileRegSet * pFileRegSet = (CFileRegSet *)m_pFileRegistry->GetRegEntry(hndFileReg);
#else
	CFileRegSet * pFileRegSet = (CFileRegSet *)(NULL==m_pFileRegistry?g_FileRegistry.GetRegEntry(hndFileReg):m_pFileRegistry->GetRegEntry(hndFileReg));
#endif
	if (pFileRegSet->IsNodeType(nodetypeRegSet))
		pFileRegSet->SetFileRegistry(m_pFileRegistry);

	// Add the CFileRegEntry to the set
	if (bAddFront)
		m_Contents.AddHead(hndFileReg);
	else
		m_Contents.AddTail(hndFileReg);

	if (m_pmapContents == NULL && m_Contents.GetCount() >= cMapThreshold)
	{
		// We now have enough items to create a content map ... do so and fill it with
		// the items so far.
		m_pmapContents = new CMapPtrToWord;
		m_pmapContents->InitHashTable(m_nHashSizeMapContents);
		for (POSITION pos = m_Contents.GetHeadPosition(); pos != NULL; )
		{
			m_pmapContents->SetAt(m_Contents.GetNext(pos), 0);
		}
	}
	else if (m_pmapContents != NULL)
	{
		// ASSERT((UINT)m_pmapContents->GetCount() < ((UINT)m_pmapContents->GetHashTableSize() << 1)); // UNDONE
		m_pmapContents->SetAt(hndFileReg, 0);
	}

	// Increase the reference count on the CFileRegEntry item
#ifndef REFCOUNT_WORK
	if (bAddRef)
		m_pFileRegistry->AddRegRef(hndFileReg);
#endif
 
	// Add this set as a dependant of the item just added
	// and inform ourselves of this addition
 	hndFileReg->AddNotifyRx(this);
#ifndef REFCOUNT_WORK
	OnNotify(hndFileReg, FRN_ADD, 0);
#else
	if (m_fCalcAttrib)
	{
		FILETIME Time;
		FILETIME ChangedTime;

		// we're going to calculate attribs now, 
		// don't allow recursion
		m_fCalcAttrib = FALSE;

 		// Maintain our cache value of the number of items that
		// do not exist but are contained in us
		BOOL bExists;
		if (hndFileReg->GetIntProp(P_ExistsOnDisk, bExists))
		{
		// If this new file does not exist on disk then we
			// should not consider the file times it has set
			// as they don't mean anything!.
			if (bExists)
 			{
	 			if (!hndFileReg->GetTimeProp(P_NewTStamp, ChangedTime) || !GetTimeProp(P_NewTStamp, Time))
				{
					MakeDirty();
				}
				else
				{
					if (ChangedTime > Time)
						SetTimeProp(P_NewTStamp, ChangedTime);
				}
			}
			else
				m_nNotExists++; 

 			SetIntProp(P_ExistsOnDisk, m_nNotExists == 0);
		}
		else
		{
			MakeDirty();
		}

		m_fCalcAttrib = TRUE;
	}
	InformNotifyRxs(FRI_ADD, (DWORD)hndFileReg);
#endif
 	
	return TRUE;
}

BOOL CFileRegSet::RemoveRegHandle(FileRegHandle hndFileReg, BOOL bReleaseRef /* = TRUE */)
{
	ASSERT(hndFileReg);

	// Remove the CFileRegEntry from the set
	POSITION pos = m_Contents.Find(hndFileReg);
	if (pos != NULL)
		RemoveRegHandleI(hndFileReg, bReleaseRef);

	return TRUE;	// only return FALSE in error
}

void CFileRegSet::RemoveRegHandleI(FileRegHandle hndFileReg, BOOL bReleaseRef /* = TRUE */)
{
	// Remove us as a dependant of the item just removed
	hndFileReg->RemoveNotifyRx(this);

#ifndef REFCOUNT_WORK
	// Notify the container that we have removed the item.
	// This gives us a chance to update things like the
	// newest/oldest time and the exists on disk props.
	OnNotify(hndFileReg, FRN_DESTROY, 0);

	// Decrease the reference count on the CFileRegEntry item
	// NOTE: This must be the last reference in this function
	// to the hndFileReg, as this call may force its self
	// destruction
	if (bReleaseRef)
		m_pFileRegistry->ReleaseRegRef(hndFileReg);
#else

	POSITION pos = m_Contents.Find(hndFileReg);
	ASSERT(pos != (POSITION)NULL);
	if (pos != (POSITION)NULL)
		m_Contents.RemoveAt(pos);
	else
		return; /* FALSE */

	//
	// Notify dependents this handle is being destroyed.
	InformNotifyRxs(FRI_DESTROY, (DWORD)hndFileReg);

	// If the item being destroyed is still in us then remove it.
 	if (m_pmapContents != NULL)
	{
		VERIFY(m_pmapContents->RemoveKey(hndFileReg));
	}

	// calculate new attrib?
	if (m_fCalcAttrib)
	{
		FILETIME Time;
		FILETIME ChangedTime;

		// we're going to calculate attribs now, 
		// don't allow recursion
		m_fCalcAttrib = FALSE;

		// Maintain our cache value of the number of items that
		// do not exist but are contained in us
		BOOL bExists;
		if (hndFileReg->GetIntProp(P_ExistsOnDisk, bExists))
		{
			if (!bExists)
			{
				ASSERT(m_nNotExists > 0);
				m_nNotExists--;
			}
			else
			{
				// If the item being destroyed does not exist on
				// disk then it cannot influence our cached new and
				// old time stamps for the set.
				if (bExists && !g_bInProjClose )
				{		 			
					if (!GetTimeProp(P_NewTStamp, Time) || !hndFileReg->GetTimeProp(P_NewTStamp, ChangedTime))
					{
						MakeDirty();
					}
					else
					{
 						// If the item going away is currently our newest time stamped item
						// then we have to find a new newest time stamped item in our set
						if (ChangedTime==Time)
		 					FindTimeAndExists(P_NewTStamp);
					}
				}
			}

			SetIntProp(P_ExistsOnDisk, m_nNotExists == 0);
		}
		else
		{
 			MakeDirty();
		}


		m_fCalcAttrib = TRUE;
	}

	// Release reference held by this file set.
	hndFileReg->ReleaseFRHRef();

	return; /* TRUE */
#endif
}

// This routine cannot be called 
// during project destruction or it will assert.
void CFileRegSet::EmptyContent(BOOL bReleaseRef /* = TRUE */)
{
	// Empty our contents
	POSITION pos = m_Contents.GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		POSITION posCurr = pos;
		FileRegHandle hndFileReg = (FileRegHandle)m_Contents.GetNext(pos);

#ifndef REFCOUNT_WORK
		// Do what we do for CFileRegSet::RemoveRegHandleI()
		// but don't have the container rx a FRN_DESTROY
		hndFileReg->RemoveNotifyRx(this);

		if (bReleaseRef)
			m_pFileRegistry->ReleaseRegRef(hndFileReg);

		// in place of FRN_DESTROY doing this for us
		m_Contents.RemoveAt(posCurr);
		if (m_pmapContents != NULL)
		{
			m_pmapContents->RemoveKey(hndFileReg);
		}
#else
		RemoveRegHandle(hndFileReg);
#endif
	}

	// Set the default property values for an empty set
	FILETIME NewestTime;
	
	// Newest date
	NewestTime.dwLowDateTime = 0;
	NewestTime.dwHighDateTime = 0;

	// Set is empty then it exists...
	SetIntProp(P_ExistsOnDisk, TRUE);
 
	// Set the default property values
	SetTimeProp(P_NewTStamp, NewestTime);

	//  Initialize private cache helping members
	m_nNotExists = 0;
}

#ifndef REFCOUNT_WORK
void CFileRegSet::ReplaceContent(CPtrList * plstContent, BOOL fAddRef /* = TRUE */)
{
}
#endif

BOOL CFileRegSet::CalcAttributes(BOOL fOn /* = TRUE */)
{
	BOOL fOldOn = m_fCalcAttrib;

	// different to old value?
	if (fOn != fOldOn)
	{
		// setting back on?
		if (!fOldOn)
			(void)RefreshAttributes();		// refresh our attributes if changed on

		m_fCalcAttrib = fOn;
	}

	return fOldOn;
}

BOOL CFileRegSet::DoRefresh()
{
	// updating?
	if ((!m_bUpdateAttribs) || (g_bInProjLoad))
		return FALSE;	// no

	// base-class refresh?
	if (CFileRegEntry::DoRefresh())
		return TRUE;	// yes

 	POSITION pos = m_Contents.GetHeadPosition();
	while (pos != NULL)
		if (((FileRegHandle)m_Contents.GetNext(pos))->DoRefresh())
			return TRUE;	// yes

	return FALSE;	// no
}

BOOL CFileRegSet::UpdateAttributes()
{
	if ( g_bInProjLoad )
		return TRUE;

	BOOL fRefreshed = FALSE;
 	POSITION pos = m_Contents.GetHeadPosition();
	while (pos != (POSITION)NULL)
		if (g_FileRegistry.GetRegEntry((FileRegHandle)m_Contents.GetNext(pos))->UpdateAttributes())
			fRefreshed = TRUE;
					 
	return TRUE;	// ok
}

BOOL CFileRegSet::RefreshAttributes()
{
	// No longer dirty
	m_bDirty = FALSE;
	
	// Refreshing the attributes on a set means updating,
	// ie. a refresh if dirty, the attributes of
	// all referenced CFileRegEntry items
	// contained, and then doing a sweep to find our 
	// accumulative attributes

	// Find the number that exist
	// Set both the oldest and newest timestamp props.
	// This function will ensure that all props. are updated
	// for each of our contents
	FindTimeAndExists(P_NewTStamp,TRUE);

	return TRUE;	// performed refresh
}

void CFileRegSet::FindTimeAndExists(UINT idProp, BOOL bFindExists)
{
	BOOL fOldOn = m_fCalcAttrib;
	m_fCalcAttrib = FALSE;

	BOOL fFindNew = idProp == P_NewTStamp;

	ASSERT(fFindNew);

	FILETIME NewTime, CurrentTime;
 	BOOL bExistsOnDisk = FALSE;
	unsigned nNotExists = 0;

	NewTime.dwLowDateTime = NewTime.dwHighDateTime = 0;

	POSITION pos = m_Contents.GetHeadPosition();
	while (pos != NULL)
	{
		FileRegHandle hndFileReg = (FileRegHandle)m_Contents.GetNext(pos);
		if (hndFileReg->GetIntProp(P_ExistsOnDisk, bExistsOnDisk))
		{
			if (bExistsOnDisk)
			{
				// is this a set?
				if (hndFileReg->IsNodeType(nodetypeRegSet))
				{	
					// for sets, figure out the oldest, newest times or both
					if (fFindNew &&
						hndFileReg->GetTimeProp(P_NewTStamp, CurrentTime) &&
						(MyCompareFileTime(&CurrentTime, &NewTime) > 0)
					   )
						NewTime = CurrentTime;
				}
				else
				{
					// for files new time == old time....
					if (!hndFileReg->GetTimeProp(P_NewTStamp, CurrentTime))
						continue;

					// figure out the oldest, newest times or both
					if (fFindNew &&
						(MyCompareFileTime(&CurrentTime, &NewTime) > 0)
					   )
 						NewTime = CurrentTime;
				}
			}
			else
			{
				nNotExists++;
			}
		}
		else
		{
			MakeDirty();
		} 
 	}

	m_fCalcAttrib = fOldOn;

	// set the oldest, newest times or both
	if (fFindNew)	SetTimeProp(P_NewTStamp, NewTime);

	//
	// Update "exists" attributes.
	if (bFindExists)
	{
		m_nNotExists = nNotExists;
		SetIntProp(P_ExistsOnDisk, m_nNotExists == 0);
	}
}

void CFileRegSet::FindExists()
{
	BOOL fOldOn = m_fCalcAttrib;
	m_fCalcAttrib = FALSE;

	unsigned nNotExists = 0;

	// get the prop. for each of our contents
 	POSITION pos = m_Contents.GetHeadPosition();
	while (pos != NULL)
	{
		FileRegHandle hndFileReg = (FileRegHandle) m_Contents.GetNext(pos);
	
		// This function will ensure that all props. are updated
		// for each of our contents
		BOOL bExists;
		VERIFY(hndFileReg->GetIntProp(P_ExistsOnDisk, bExists));
		if (!bExists)
			nNotExists++;
	}

	m_fCalcAttrib = fOldOn;

	// set whether all our contents exist or not
	m_nNotExists = nNotExists;
	SetIntProp(P_ExistsOnDisk, m_nNotExists == 0);
}

BOOL CFileRegSet::Touch()
{
	BOOL bRetVal = TRUE;

	// get the prop. for each of our contents
 	POSITION pos = m_Contents.GetHeadPosition();
	while (pos != NULL)
	{
		FileRegHandle hndFileReg = (FileRegHandle) m_Contents.GetNext(pos);
		bRetVal = bRetVal && hndFileReg->Touch();
	}

	return bRetVal;
}

void CFileRegSet::DeleteFromDisk()
{
	// get the prop. for each of our contents
 	POSITION pos = m_Contents.GetHeadPosition();
	while (pos != NULL)
	{
		FileRegHandle hndFileReg = (FileRegHandle) m_Contents.GetNext(pos);
		hndFileReg->DeleteFromDisk();
	}
}

void CFileRegSet::OnNotify(FileRegHandle hndFileReg, UINT idChange, DWORD dwHint)
{
	FILETIME Time;
	FILETIME ChangedTime;

	ASSERT(hndFileReg);

	// Handle notifications about content changes here
	switch (idChange)
	{
 		case P_NewTStamp:
 		{
			// calculate new attrib?
			if (!m_fCalcAttrib)
				break;	// no

			FILETIME * pPreviousTime = (FILETIME *)dwHint;
 
			if( !g_bInProjClose ){
				if (GetTimeProp(P_NewTStamp, Time) && hndFileReg->GetTimeProp(P_NewTStamp, ChangedTime))
				{
					// If the changed time is newer than the current newest time then set
					// the newest time to be the changed time
					// If the changed time is older than the current newest time and the
					// time that changed was the newest time then we have to search the
					// set to find the newest time
					if (MyCompareFileTime(&ChangedTime, &Time) >= 0)
						SetTimeProp(P_NewTStamp, ChangedTime);
	
					else if (MyCompareFileTime(pPreviousTime, &Time) == 0)
						FindTimeAndExists(P_NewTStamp);
				}
				else
				{ 
					MakeDirty();
				}
			}
 			break;
		}

		case P_ExistsOnDisk:
		{
			// calculate new attrib?
			if (!m_fCalcAttrib)
				break;	// no

			// find whether we exist
			FindExists();
			break;
		}

#ifndef REFCOUNT_WORK
		case FRN_LCL_DESTROY:

			// Only do a FRN_DESTROY if it a local destroy in us
			if ((CFileRegistry *)dwHint != m_pFileRegistry)
				return;	// ignore completely

			// Remove us as a dependant of the item just removed
			hndFileReg->RemoveNotifyRx(this);

		// Fall thru'

		case FRN_DESTROY:
		{
			// If the item being destroyed is still in us then remove it.
			POSITION pos = m_Contents.Find(hndFileReg);
			if (pos != (POSITION)NULL)
				m_Contents.RemoveAt(pos);
 
 			if (m_pmapContents != NULL)
			{
				VERIFY(m_pmapContents->RemoveKey(hndFileReg));
			}

			// calculate new attrib?
			if (!m_fCalcAttrib)
				break;	// no

			// we're going to calculate attribs now, 
			// don't allow recursion
			m_fCalcAttrib = FALSE;

			// Maintain our cache value of the number of items that
			// do not exist but are contained in us
			BOOL bExists;
			if (hndFileReg->GetIntProp(P_ExistsOnDisk, bExists))
			{
				if (!bExists)
				{
					ASSERT(m_nNotExists > 0);
					m_nNotExists--;
				}

				SetIntProp(P_ExistsOnDisk, m_nNotExists == 0);
			}
			else
			{
 				MakeDirty();
				goto End_FRN_DESTROY;
			}

			// If the item being destroyed does not exist on
			// disk then it cannot influence our cached new and
			// old time stamps for the set.
			if (bExists && !g_bInProjClose )
			{		 			
				if (!GetTimeProp(P_NewTStamp, Time) || !hndFileReg->GetTimeProp(P_NewTStamp, ChangedTime))
				{
					MakeDirty();
					goto End_FRN_DESTROY;
				}
 				
 				// If the item going away is currently our newest time stamped item
				// then we have to find a new newest time stamped item in our set
				if (MyCompareFileTime(&ChangedTime, &Time) == 0)
		 			FindTimeAndExists(P_NewTStamp);
			}

  		End_FRN_DESTROY:

			m_fCalcAttrib = TRUE;
			break;
		}

		case FRN_ADD:
		{
			// calculate new attrib?
			if (!m_fCalcAttrib)
				break;	// no

			// we're going to calculate attribs now, 
			// don't allow recursion
			m_fCalcAttrib = FALSE;

 			// Maintain our cache value of the number of items that
			// do not exist but are contained in us
			BOOL bExists;
			if (hndFileReg->GetIntProp(P_ExistsOnDisk, bExists))
			{
				if (!bExists)
					m_nNotExists++; 

 				SetIntProp(P_ExistsOnDisk, m_nNotExists == 0);
			}
			else
			{
				MakeDirty();
				goto End_FRN_ADD;
			}

			// If this new file does not exist on disk then we
			// should not consider the file times it has set
			// as they don't mean anything!.
			if (bExists)
 			{
	 			if (!hndFileReg->GetTimeProp(P_NewTStamp, ChangedTime) || !GetTimeProp(P_NewTStamp, Time))
				{
					MakeDirty();
					goto End_FRN_ADD;
				}

				if (MyCompareFileTime(&ChangedTime, &Time) > 0)
					SetTimeProp(P_NewTStamp, ChangedTime);
			}

		End_FRN_ADD:

			m_fCalcAttrib = TRUE;

 			break;
		}
#endif //ifndef REFCOUNT_WORK

 		case FRN_ALL:
		{
			MakeDirty();
			break;
		}
 	}

#ifndef REFCOUNT_WORK
	// propagate inform?
	if( m_Dependants.GetSize() ) {
		if (idChange & INF_MASK)
			InformNotifyRxs(idChange, dwHint);
		else if (idChange <= FRN_LAST) { // send as an inform?
			InformNotifyRxs(idChange | INF_MASK , (DWORD)hndFileReg);
		}
	}
#endif
}

BOOL CFileRegSet::GetTimeProp(UINT idProp, FILETIME& ftime)
{
	// Get a time property value
	if( g_bInProjClose ){
		ftime.dwLowDateTime = 0xffffffff;
		ftime.dwHighDateTime = 0x7fffffff;
		return TRUE;
	}
	
	(void)UpdateAttributes();
 
	switch (idProp)
	{
		case P_NewTStamp:
			ftime = m_NewTime;
			break;

		default:
			TRACE1("Unhandled GetTimeProp(%d)!\n", idProp);
			return FALSE;
	}

	return TRUE;
}

BOOL CFileRegSet::SetTimeProp(UINT idProp, FILETIME ftime)
{
 	// Set a time property value
 	FILETIME oldtime;
 	switch (idProp)
	{
		case P_NewTStamp:
			oldtime = m_NewTime;
			m_NewTime = ftime;
			break;

		default:
			TRACE1("Unhandled SetTimeProp(%d)!\n", idProp); 
			return FALSE;
	}

	// any change?
	if (oldtime.dwLowDateTime != ftime.dwLowDateTime ||
		oldtime.dwHighDateTime != ftime.dwHighDateTime
	   )
		if (!g_bInProjLoad) // skip this during project load
		{
			// notify our dependants that a time property value has changed
			InformNotifyRxs(idProp, (DWORD)&oldtime);
		}

	return TRUE;
}

#ifdef _DEBUG
void CFileRegSet::Dump(CDumpContext& dc) const
{
	// Call our base class first
	CFileRegEntry::Dump(dc);

	// Now do the stuff for our specific class
	dc << "{CFileRegSet:";
	if (m_Contents.IsEmpty())
	{
		dc << "m_Contents is <empty>";
	}
	else
	{
		dc << "m_Contents list:";

		int i = 0;

		// Dump the contents list
		POSITION pos = m_Contents.GetHeadPosition();
		while (pos != NULL)
		{
			FileRegHandle hndFileReg = (FileRegHandle) m_Contents.GetNext(pos);
			dc << "\n\t<" << i << "> ";
			hndFileReg->Dump(dc);
			i++;
		}
	}
	dc << "\nm_pFileRegistry@" << (void*)m_pFileRegistry << "}";
}
#endif

#ifdef REFCOUNT_WORK
void CFileRegSet::SetFileRegistry(CFileRegistry * pFileRegistry)
{
	// This should be a one time only operation.
//	ASSERT( IsEmpty() );

	if (NULL!=pFileRegistry)
	{
		POSITION pos = m_Contents.GetHeadPosition();
		while (NULL!=pos)
		{
			FileRegHandle frh = (FileRegHandle)m_Contents.GetNext(pos);
			pFileRegistry->AddRegRef(frh);
		}

		ASSERT(NULL==m_pFileRegistry);
		AddNotifyRx(pFileRegistry);
	}
	else if (NULL!=m_pFileRegistry)
	{
		RemoveNotifyRx(m_pFileRegistry);

		POSITION pos = m_Contents.GetHeadPosition();
		while (NULL!=pos)
		{
			FileRegHandle frh = (FileRegHandle)m_Contents.GetNext(pos);
			m_pFileRegistry->ReleaseRegRef(frh);
		}
	}
	else
	{
//		ASSERT(FALSE);
	}

	m_pFileRegistry = pFileRegistry;
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
//	CFileRegFilter

IMPLEMENT_DYNAMIC(CFileRegFilter, CFileRegSet)

CFileRegFilter::CFileRegFilter(CString strFilter)
{
	// Set our node type
	dwNodeType |= nodetypeRegFilter;

	m_strFilter = strFilter;
	m_strFilterLC = strFilter;
	m_strFilter.MakeUpper();
	m_strFilterLC.MakeLower();
}

CFileRegFilter::~CFileRegFilter()
{
	m_bUpdateAttribs = FALSE;	// don't attempt to update
 	m_fCalcAttrib = FALSE;		// don't re-calc attrib.

#ifndef REFCOUNT_WORK
	POSITION pos = m_Contents.GetHeadPosition();
	while (pos != NULL)
	{
		FileRegHandle hndFileReg = (FileRegHandle) m_Contents.GetNext(pos);
		
		// *don't* decr. the ref. count
		RemoveRegHandle(hndFileReg, FALSE);
	}

	// CFileRegSet::~CFileRegSet()
	// This does the same as above, but we do it first
	// so that it doesn't have any content to remove and *decr.* ref. count
	// which we don't want!
#endif
}

BOOL CFileRegFilter::Filter(LPCTSTR pszText)
{
	LPCTSTR pchLastText = NULL;
	LPCTSTR pchPat = m_strFilter;
	LPCTSTR pchPatLC = m_strFilterLC;
	int ichPat = 0, ichLastStar = -1;
	int c;

    if (theApp.GetCP() != 0)
	{
		// DBCS and German case
		LPCTSTR pchLastStar = NULL;
		while (*pchPat != _T('\0') && *pszText != _T('\0'))
		{
			if (*pchPat == _T('*'))
			{
				pchLastStar = pchPat;
				pchLastText = pszText;
			}
			else if (*pchPat == _T('?'))
			{
				pchPat = _tcsinc(pchPat);
			}
			else if (_tcsncicmp(pchPat,pszText,1)!=0) // handles DBCS correctly
			{
				if (pchLastStar != NULL)
				{
					pchPat = pchLastStar;
					pchLastText = _tcsinc(pchLastText);
					pszText = pchLastText;
				}
				else
					return FALSE;
			}
			else
				pszText = _tcsinc(pszText);

			pchPat = _tcsinc(pchPat);
		}
	}
	else
	{
		// optimized non-DBCS case (assumes no German ss/B in filters)
		ASSERT(m_strFilter.GetLength() == m_strFilterLC.GetLength());

		while ((*pszText != _T('\0')) && ((c = pchPat[ichPat]) != _T('\0')))
		{
			if (c == _T('*'))
			{
				ichLastStar = ichPat;
				pchLastText = pszText;
			}
			else if (c == _T('?'))
			{
				ichPat++;
			}
			else if ((c!=*pszText) && (pchPatLC[ichPat] != *pszText))
			{
				if (ichLastStar != -1)
				{
					ichPat = ichLastStar;
					pszText = ++pchLastText;
				}
				else
					return FALSE;
			}
			else
				pszText++;

			ichPat++;
		}
		pchPat += ichPat;
	}

	// at least one of these must be null to break out of loop
	ASSERT((*pchPat == _T('\0')) || (*pszText == _T('\0')));

	return (*pchPat == *pszText); // == _T('\0')
}

BOOL CFileRegFilter::AddRegHandle(FileRegHandle hndFileReg, BOOL bAddRef /* = TRUE */)
{
#ifndef REFCOUNT_WORK
	// Filters can only contain files
	ASSERT(hndFileReg->IsKindOf(RUNTIME_CLASS(CFileRegFile)));
	ASSERT(hndFileReg->IsNodeType(nodetypeRegFile));

	// Only add files that match our filter

	if (Filter(*((CFileRegFile*)hndFileReg)->GetFilePath()))
		// Filters do not addref their contents
		return CFileRegSet::AddRegHandle(hndFileReg, FALSE);
  	else
		return FALSE;
#else
	// This should never be called since the CFileRegistry should be 
	// managing the filter contents.
	ASSERT( FALSE );
	return FALSE;
#endif
}

BOOL CFileRegFilter::RemoveRegHandle(FileRegHandle hndFileReg, BOOL bReleaseRef /* = TRUE */)
{
#ifndef REFCOUNT_WORK
	// Filters do not addref their contents
	return CFileRegSet::RemoveRegHandle(hndFileReg, FALSE);
#else
	// This should never be called since the CFileRegistry should be 
	// managing the filter contents.
	ASSERT( FALSE );
	return FALSE;
#endif
}

#ifdef REFCOUNT_WORK
BOOL CFileRegFilter::AddFilterHandle(FileRegHandle hndFileReg)
{
	// Filters can only contain files
	ASSERT(hndFileReg->IsKindOf(RUNTIME_CLASS(CFileRegFile)));
	ASSERT(hndFileReg->IsNodeType(nodetypeRegFile));

	// Only add files that match our filter

	if (Filter(*((CFileRegFile*)hndFileReg)->GetFilePath()))
	{
#if 0
		ASSERT(NULL==m_Contents.Find(hndFileReg));
		VERIFY( m_Contents.AddTail(hndFileReg) );
		InformNotifyRxs(FRI_ADD, (DWORD)hndFileReg);
		return TRUE;
#else
		// Call base method.
		return CFileRegSet::AddRegHandle(hndFileReg);
#endif
	}
  	else
		return FALSE;
}

BOOL CFileRegFilter::RemoveFilterHandle(FileRegHandle hndFileReg)
{
	if (Filter(*((CFileRegFile*)hndFileReg)->GetFilePath()))
	{
		POSITION pos = m_Contents.Find(hndFileReg);
		ASSERT(NULL!=pos);
		if (NULL!=pos)
		{
#if 0
			m_Contents.RemoveAt(pos);
			InformNotifyRxs(FRI_DESTROY, (DWORD)hndFileReg);
			return TRUE;
#else
		// Call base method.
		return CFileRegSet::RemoveRegHandle(hndFileReg);
#endif
		}
	}
	return FALSE;
}
#endif

#ifdef _DEBUG
void CFileRegFilter::Dump(CDumpContext& dc) const
{
	// Call our base class function first
	CFileRegSet::Dump(dc);

	dc << "{CFileRegFilter:\"" << m_strFilter << "\"}";
}
#endif

#ifdef REFCOUNT_WORK

///////////////////////////////////////////////////////////////////////////////
//
//	CFileRegFilterSet

IMPLEMENT_DYNAMIC(CFileRegFilterSet, CFileRegSet)

CFileRegFilterSet::CFileRegFilterSet()
{
	dwNodeType |= nodetypeRegFilterSet;
}

CFileRegFilterSet::~CFileRegFilterSet()
{
#if 0
	//
	// Release references to CFileRegFilter's.
	//
	POSITION pos = m_FilterList.GetHeadPosition();

	while (NULL!=pos)
	{
		CFileRegFilter *pFilter = (CFileRegFilter*)m_FilterList.GetNext(pos);
		ASSERT(pFilter->IsNodeType(nodetypeRegFilter));
		pFilter->ReleaseFRHRef();
	}
#endif
}

void CFileRegFilterSet::OnNotify(FileRegHandle hndFileReg, UINT idChange, DWORD dwHint)
{
	FileRegHandle frh = (FileRegHandle)dwHint;
	switch (idChange)
	{
	case FRI_ADD:
		ASSERT( NULL!=m_FilterList.Find((void*)hndFileReg) );
		ASSERT(frh->IsNodeType(nodetypeRegFile));
		CFileRegSet::AddRegHandle(frh);
		break;

	case FRI_DESTROY:
		ASSERT( NULL!=m_FilterList.Find((void*)hndFileReg) );
		ASSERT(frh->IsNodeType(nodetypeRegFile));
		CFileRegSet::RemoveRegHandle(frh);
		break;

	default:
		CFileRegSet::OnNotify(hndFileReg, idChange, dwHint);
		break;
	}
}

BOOL CFileRegFilterSet::AddFilter( CFileRegFilter* pFilter )
{
	ASSERT(pFilter->IsNodeType(nodetypeRegFilter));
	ASSERT( NULL==m_FilterList.Find((void*)pFilter) );

	m_FilterList.AddTail(pFilter);
//	pFilter->AddFRHRef();

	pFilter->InitFrhEnum();

	// Add pFilter contents to CFileRegFilterSet contents.
	FileRegHandle frh = NULL;
	while (NULL!=(frh=pFilter->NextFrh()))
	{
		CFileRegSet::AddRegHandle(frh);
		frh->ReleaseFRHRef();
	}

	// Interested in FRI_ADD and FRI_DESTROY notifications from pFilter.
	pFilter->AddNotifyRx(this);

	return TRUE;
}

#endif //#ifdef REFCOUNT_WORK

///////////////////////////////////////////////////////////////////////////////
//
//	CFileRegFile

IMPLEMENT_DYNAMIC(CFileRegFile, CFileRegEntry)

ULONG	CFileRegFile::s_tickCurrent = 1;
#ifdef _DEBUG
ULONG	CFileRegFile::s_cFailedWatches = 0;
#endif

#ifdef REFCOUNT_WORK
#if 0
FileNameMap s_RegFileMap(911);
FileNameMap::FileNameMap(int nHashSize) : m_FileNameMap(16)
{
	m_FileNameMap.InitHashTable(nHashSize);
}

FileNameMap::~FileNameMap()
{
}
#endif

CGrowingMapStringToPtr*	CFileRegFile::s_pFileNameMap = NULL;
CFileRegFile::CInitMap s_InitRegFileMap;
CFileRegFile::CInitMap::CInitMap(int nHashSize)
{
	CFileRegFile::s_pFileNameMap = new CGrowingMapStringToPtr(16);
	CFileRegFile::s_pFileNameMap->InitHashTable(nHashSize);	// init our map too!
}

CFileRegFile::CInitMap::~CInitMap()
{
	// Prevent access to file map by other threads.
	CritSectionT cs(g_sectionFileMap);

#ifdef _DEBUG
	// delete file handles so we won't have memory leaks.
	POSITION pos = CFileRegFile::s_pFileNameMap->GetStartPosition();

	if (NULL!=pos)
		TRACE( "\nNote: Left over files may actually be held in global objects that haven't been deleted yet\n" );


	while(NULL!=pos)
	{
		CString strKey;
		FileRegHandle hndFileReg;
		CFileRegFile::s_pFileNameMap->GetNextAssoc(pos, strKey, (void*&) hndFileReg);
		ASSERT(NULL!=hndFileReg);
		TRACE( "\nLeft over file: " );
		AfxDump((CObject*)hndFileReg);

#ifdef PRESERVE_REGFILE
		if ( 0 == hndFileReg->GetRefCount() )
			delete hndFileReg;
#endif
	}
#endif
	delete CFileRegFile::s_pFileNameMap;
	CFileRegFile::s_pFileNameMap = NULL;
}


FileRegHandle CFileRegFile::GetFileHandle(LPCTSTR szFileName, LPTSTR szKeyName)
{
    TCHAR szKeyBuffer[MaxKeyLen];
	LPTSTR szKey = NULL;

	//
	// Allow user to specify storage for key so that it can be reused.
	//
	if (NULL==szKeyName)
		szKey = szKeyBuffer;
	else
		szKey = szKeyName;

	// Prevent access to file map by other threads.
	CritSectionT cs(g_sectionFileMap);

	FileRegHandle hndFileReg = LookupFileHandleByName(szFileName, szKey);

	if (NULL==hndFileReg)
	{
		CPath path;
		if (path.Create(szFileName))
		{
			CFileRegFile * pregfile = new CFileRegFile(&path);
			if (NULL!=pregfile)
			{
				hndFileReg = (FileRegHandle)pregfile;

				ASSERT(NULL!=s_pFileNameMap);
				if (NULL!=s_pFileNameMap)
					s_pFileNameMap->SetAt(szKey, hndFileReg);

				hndFileReg->AddFRHRef();
			}
		}
	}

	return hndFileReg;
}


FileRegHandle CFileRegFile::LookupFileHandleByName(LPCTSTR szFileName, LPTSTR szKeyName)
{
	FileRegHandle hndFileReg = NULL;
    TCHAR szKeyBuffer[MaxKeyLen];
	LPTSTR szKey = NULL;

	// No map no handle.
	if (NULL==s_pFileNameMap)
		return NULL;

	//
	// Allow user to specify storage for key so that it can be reused.
	//
	if (NULL==szKeyName)
		szKey = szKeyBuffer;
	else
		szKey = szKeyName;

	ASSERT(MaxKeyLen>_tcslen(szFileName));

	//
	// Key is uppercase full path name.
	// 
    _tcscpy(szKey, szFileName);
    _tcsupr(szKey);

	// Prevent access to file map by other threads.
	CritSectionT cs(g_sectionFileMap);

	s_pFileNameMap->Lookup(szKey,(void*&) hndFileReg);

	if (NULL!=hndFileReg)
		hndFileReg->AddFRHRef();

	return hndFileReg;
}


FileRegHandle CFileRegFile::LookupFileHandleByKey(LPCTSTR szKeyName)
{
	FileRegHandle hndFileReg = NULL;

	// No map no handle.
	if (NULL==s_pFileNameMap)
		return NULL;

	ASSERT(MaxKeyLen>_tcslen(szKeyName));

	// Prevent access to file map by other threads.
	CritSectionT cs(g_sectionFileMap);

	s_pFileNameMap->Lookup(szKeyName,(void*&) hndFileReg);

	if (NULL!=hndFileReg)
		hndFileReg->AddFRHRef();

	return hndFileReg;
}

void CFileRegFile::SafeDelete()
{
	// Prevent access to file map by other threads.
	CritSectionT cs(g_sectionFileMap);

	if (0==m_nRefCount)
		delete this;
}

#endif

CFileRegFile::CFileRegFile
(
	const CPath * pPath,
	BOOL bMatchCase /* = FALSE */,
	BOOL bFiltered /* = FALSE */
)
	: CFileRegEntry()
{
	// Set our node type
	dwNodeType |= nodetypeRegFile;
	m_nExistsOnDisk = TRUE;

	ASSERT(pPath->IsInit());
	m_Path = *pPath;
	ASSERT(0<_tcslen(m_Path.GetFullPath()));

#ifndef REFCOUNT_WORK
	m_bFiltered = bFiltered;
#endif
	
	m_bWantActualCase = (bMatchCase!=0);

	// Review: avoid setting case here if possible
	if (m_bMatchCase = (bMatchCase==TRUE)) // use bMatchCase = -1 to defer this
	{
		m_Path.GetActualCase();
	}
	m_bDirty = TRUE;
	m_bFileWatch = m_bFileAddedToFC = FALSE;
	m_tickFailedWatch = 0;	// haven't failed yet
}

CFileRegFile::~CFileRegFile()
{
#ifndef REFCOUNT_WORK
	{
		// Check if this frh is in the file change Q
		CritSectionT cs(g_sectionFileChangeQ);

		POSITION pos = g_FileChangeQ.GetHeadPosition();
		if (pos != (POSITION)NULL)
		{
			// refresh attributes for q'd files
			while (pos != (POSITION)NULL)
			{
				POSITION posCurr = pos;
				S_ATTRIB * pattrib = (S_ATTRIB *)g_FileChangeQ.GetNext(pos);

				// set from cache and then deallocate
				if (pattrib->_frh == (FileRegHandle)this)
				{
					// Remove this entry in the Q
					g_FileChangeQ.RemoveAt(posCurr);

					// FUTURE: release ref.?

					// de-allocate
					delete pattrib;
				}
			}
		}
	}
#endif

	// assynchronous file deletion?
	if (m_bFileAddedToFC)
#ifdef _BLD_SRC_FC_ASYNC
//#pragma message ("INFO : pfilereg.cpp : Using ASYNC file chng. add ....")
	{
		// asynchronous
		GetFileChange()->FDelFileAsync(m_Path, BldSrcFileChangeCallBack, NULL);
	}
#else // _BLD_SRC_FC_ASYNC
//#pragma message ("INFO : pfilereg.cpp : Using SYNC file chng. add ....")
	{
		ASSERT(0<_tcslen(m_Path.GetFullPath()));
		GetFileChange()->FDelFileAsync(m_Path, BldSrcFileChangeCallBack, NULL);
	}
#endif // !_BLD_SRC_FC_ASYNC

#ifdef REFCOUNT_WORK
	//
	// Remove from map (if map hasn't been destroyed yet during shutdown).
	//
	// No map no handle.
	if (NULL==s_pFileNameMap)
		return;

    TCHAR szKey[MAX_PATH];
    _tcscpy(szKey, *GetFilePath());
    _tcsupr(szKey);

	VERIFY(s_pFileNameMap->RemoveKey(szKey));
#endif
}

void
CFileRegFile::AllowRetryFailedWatches(BOOL fForceAll)
{
#ifdef _DEBUG
	// Failed watch calls are expensive because they cause us to try to stat()
	// files in nonexistent directories ... so monitor them.
	static ULONG cMaxFailedWatches = 10;			// arbitrary reporting limit
	if (s_cFailedWatches > cMaxFailedWatches + 10)	// arbitrary threshold
	{
		cMaxFailedWatches = s_cFailedWatches;
		TRACE("pfilereg.cpp: %lu failed AddWatch calls (this tick).\n", cMaxFailedWatches);
	}
	s_cFailedWatches = 0;
#endif
	// Bump the tick count so that attempts at watching files will be re-tried.
	s_tickCurrent += (fForceAll ? ctickRetryInterval : 1);
	if (s_tickCurrent == 0)
	{
		s_tickCurrent++;	// 0 is special, not a valid tick time
	}
}

BOOL CFileRegFile::WatchFile(BOOL & fRefreshed)
{
	BOOL fAddtoFC = !g_FileRegDisableFC && !m_bFileWatch && !m_bFileAddedToFC;
	fRefreshed = FALSE;

	// assynchronous file addition?
	if (fAddtoFC)
	{
#ifdef _BLD_SRC_FC_ASYNC
#ifndef _BLD_NO_FILE_CHNG
		// Make sure the file reg object hangs around
		AddFRHRef();

		if (FCanWatch() && GetFileChange()->
			FAddFileAsync(m_Path, BldSrcFileChangeCallBack, BldSrcFileAddedCallBack, (FileRegHandle)this, NULL, TRUE))
		{
			// asynchronous
			// do nothing, wait for async. notification
			m_bFileAddedToFC = TRUE;
			fRefreshed = TRUE;	// we will be in the async callback
		}
        else
#endif // _BLD_NO_FILE_CHNG
		{
			// Record that we failed, and when (will try again later).
			DEBUG_ONLY(s_cFailedWatches++);
			m_tickFailedWatch = s_tickCurrent;
            m_bDirty = TRUE;    // File change watch failed, so we'd
                                // better dirty ourselves so we get
                                // uptodate details from disk
		}

#else // _BLD_SRC_FC_ASYNC
#ifndef _BLD_NO_FILE_CHNG
		// we must not be locking the dep update list, since the file change thread
		// may need to add items to it.
		ASSERT(!g_sectionDepUpdateQ.FOwned());

#ifdef REFCOUNT_WORK
		ASSERT(0<_tcslen(m_Path.GetFullPath()));
#endif

		if (FCanWatch() && GetFileChange()->
			FAddFile(m_Path, BldSrcFileChangeCallBack, TRUE))
		{
			// synchronous
#ifdef REFCOUNT_WORK
			// file change thread reference.
//			AddFRHRef();
#endif
			m_bFileWatch = m_bFileAddedToFC = TRUE;
			fRefreshed = RefreshAttributes();
		}
        else
#endif // _BLD_NO_FILE_CHNG
		{
			// Record that we failed, and when (will try again later).
			DEBUG_ONLY(s_cFailedWatches++);
			m_tickFailedWatch = s_tickCurrent;
            m_bDirty = TRUE;    // File change watch failed, so we'd
                                // better dirty ourselves so we get
                                // uptodate details from disk
		}
#endif // !_BLD_SRC_FC_ASYNC	
	}

	return m_bFileWatch;	// now watched?
}

BOOL CFileRegFile::Touch()
{
	// This will cause the file change to be ignored. So things like the 
	// source editor will not get notified of the change.
	CIgnoreFile ignoreFile(GetFileChange(), m_Path);

	int fh = _utime ((LPCSTR)m_Path, NULL);
	if (fh == -1)
		return FALSE;
	return TRUE;
}

void CFileRegFile::DeleteFromDisk()
{
	// This will cause the file change to be ignored. So things like the 
	// source editor will not get notified of the change.
	m_Path.DeleteFromDisk();
	return;
}




BOOL CFileRegFile::GetFileTime(FILETIME& ft)
{
	(void)UpdateAttributes();
	return m_nExistsOnDisk && GetTimeProp(P_NewTStamp, ft);
}

BOOL CFileRegFile::GetFileTime(CString & str)
{
	FILETIME ft;
	if (GetFileTime(ft))
	{		 
		CTime time(ft);

		CString strTime = theApp.m_CPLReg.Format(time, TIME_ALL);
		CString strDate = theApp.m_CPLReg.Format(time, DATE_ALL);
		str = strTime + _TEXT(" ") + strDate;

		return TRUE;
	}

	return FALSE;
}

BOOL CFileRegFile::GetTimeProp(UINT idProp, FILETIME& ftime)
{
	// Get a time property value
	(void)UpdateAttributes();
 
	ASSERT(idProp == P_NewTStamp);
	ftime = m_FileTime;
 	return TRUE;
}

BOOL CFileRegFile::SetTimeProp(UINT idProp, FILETIME ftime)
{
 	if (idProp != P_NewTStamp )
	{
		TRACE1("Unhandled SetTimeProp(%d)!\n", idProp); 
 		return FALSE;
	}

 	FILETIME oldtime = m_FileTime;
 
	// any change?
	if (oldtime.dwLowDateTime != ftime.dwLowDateTime ||
		oldtime.dwHighDateTime != ftime.dwHighDateTime
	   )
	{
		m_FileTime = ftime;

		if (!g_bInProjLoad) // skip this during project load
		{
			// notify our dependants that a time property value has changed
			InformNotifyRxs(P_NewTStamp, (DWORD)&oldtime);
		}
	}

	return TRUE;
}

BOOL CFileRegFile::RefreshAttributes()
{
	// No longer dirty
	m_bDirty = FALSE;

	// Get the current file attributes
	S_ATTRIB attrib;
	GetAttributes(&attrib);

	// anything to do?
	if (m_FileTime.dwLowDateTime == attrib._filetime.dwLowDateTime &&
		m_FileTime.dwHighDateTime == attrib._filetime.dwHighDateTime &&
		m_nExistsOnDisk == attrib._fExists
	   )
		return FALSE;	// nothing to do

	// Set the new file attributes
 	SetAttributes(&attrib);

	// Ensure sequential access to file change Q
	CritSectionT cs(g_sectionFileChangeQ);

#ifndef REFCOUNT_WORK
	// Ensure sequential access to file map
	CritSectionT cs2(CFileRegistry::g_sectionFileMap);
#endif

	// Remove this file from our Q 'cos we have already sync'd!
	POSITION pos = g_FileChangeQ.GetHeadPosition();
	if (pos != (POSITION)NULL)
	{
		// refresh attributes for q'd files
		while (pos != (POSITION)NULL)
		{
			POSITION posCurr = pos;
			S_ATTRIB * pattrib = (S_ATTRIB *)g_FileChangeQ.GetNext(pos);

			// find the registry entry?
			if (pattrib->_frh == (FileRegHandle)this)
			{
				// remove
				g_FileChangeQ.RemoveAt(posCurr);

#ifdef _DEBUG
				const CPath * pPath = GetFilePath();
				TRACE("\nRemove in refresh '%s'", pPath->GetFileName());
#endif

				// FUTURE release ref.?
#ifdef REFCOUNT_WORK
				pattrib->_frh->ReleaseFRHRef();
#endif

				// de-allocate
				delete pattrib;
			}
		}
	}

	return TRUE;	// performed a refresh!
}

void CFileRegFile::SetAttributes(S_ATTRIB * pattrib)
{
 	// Are we being notified of changes on this
	// file?
 	ASSERT(pattrib->_fOk);
	{
	 	SetIntProp(P_ExistsOnDisk, pattrib->_fExists);
		SetTimeProp(P_NewTStamp, pattrib->_filetime);
	}
}

extern BOOL (WINAPI *g_pGetFileAttributesEx_DevBld)( LPCTSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
									 LPVOID lpFileInformation);
__inline BOOL Compatible_GetFileAttributesEx_DevBld( LPCTSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
									 LPVOID lpFileInformation)
{
	return (*g_pGetFileAttributesEx_DevBld)( lpFileName, fInfoLevelId, lpFileInformation);
}

void CFileRegFile::GetAttributes(S_ATTRIB * pattrib)
{
	if ((!m_bMatchCase) && (m_bWantActualCase))
	{
		WIN32_FIND_DATA find;
		HANDLE hfind = ::FindFirstFile((LPCTSTR)m_Path, &find);

		pattrib->_fOk = TRUE;
		if (hfind != INVALID_HANDLE_VALUE)
		{
			VERIFY(::FindClose(hfind));

			pattrib->_filetime = find.ftLastWriteTime;
			pattrib->_fExists = TRUE;

			m_Path.SetActualCase(find.cFileName);
		}
		else
		{
			pattrib->_filetime.dwLowDateTime = pattrib->_filetime.dwHighDateTime = 0;	// none
			pattrib->_fExists = FALSE;
		}
	}
	else
	{
		WIN32_FILE_ATTRIBUTE_DATA find;

		pattrib->_fOk = TRUE;
		if (Compatible_GetFileAttributesEx_DevBld( (LPCTSTR)m_Path, GetFileExInfoStandard, &find))
		{
			pattrib->_filetime = find.ftLastWriteTime;
			pattrib->_fExists = TRUE;
		}
		else
		{
			pattrib->_filetime.dwLowDateTime = pattrib->_filetime.dwHighDateTime = 0;	// none
			pattrib->_fExists = FALSE;
		}
	}
}

void CFileRegFile::SetMatchCase(BOOL bMatchCase, BOOL bCaseApproved /*=FALSE*/)
{
	if (!bCaseApproved && (!m_bMatchCase && bMatchCase))
 		m_Path.GetActualCase();

	m_bWantActualCase = bMatchCase;
	m_bMatchCase = bMatchCase;
}

BOOL CFileRegFile::UpdateAttributes()
{
	// if not dirty and not updating dep, can we rely on file watching?
	BOOL fRefreshed = FALSE;
	if (!m_bDirty && !g_bUpdateDep && FCanWatch() && WatchFile(fRefreshed))
		return fRefreshed;	// update performed if refreshed during watch

	// no, so do we'll have to maybe do it
 	return CFileRegEntry::UpdateAttributes();
}

#ifdef _DEBUG
void CFileRegFile::Dump(CDumpContext& dc) const
{
	// Call our base class function first
	CFileRegEntry::Dump(dc);

	dc << "{CFileRegFile:" << m_Path.GetFullPath();
 	dc << ";ExistsOnDisk: " << m_nExistsOnDisk;
#ifndef REFCOUNT_WORK
 	dc << ";Relative: " << m_bRelative;
#endif
	dc << ";MatchCase: " << m_bMatchCase;
	dc << ";WantActualCase: " << m_bWantActualCase;
	dc << "}";
}
#endif

/////////////////////////////////////////////////////////////////////////////
//
//	CFileRegistry

IMPLEMENT_DYNAMIC(CFileRegistry, CFileRegSet)

CFileRegistry::CFileRegistry(int nHashSize)
#ifdef REGISTRY_FILEMAP
	: CFileRegSet(nHashSize), m_FileMap(16)
#else
	: CFileRegSet(nHashSize)
#endif
{
	// Set our node type
	dwNodeType |= nodetypeRegRegistry;

	// Set up the file registry pointer
	m_pFileRegistry = this;

	// We must have a contents map because we use it for reference counting.
	m_pmapContents = new CMapPtrToWord;
	m_pmapContents->InitHashTable(nHashSize, FALSE);	// set hash size as requested
#ifdef REGISTRY_FILEMAP
	// FUTURE: tune size (may be too big)
	m_FileMap.InitHashTable(nHashSize);	// init our map too!
#endif

	// jimsc 10-21-96 If this is a java or static lib project, then we don't care about
	// how many of these things get entered. 

	m_fCareAboutRCFiles = TRUE;
	m_fCareAboutDefFiles = TRUE;
	CProject *pProj = (CProject *) g_BldSysIFace.GetActiveBuilder ();
	if (pProj)
	{
		CProjType * pProjType = pProj->GetProjType();

		if (pProjType && pProjType->GetUniqueTypeId() == CProjType::javaapplet )
		{
			m_fCareAboutRCFiles = FALSE;
			m_fCareAboutDefFiles = FALSE;
		}
		else
		{
			m_fCareAboutRCFiles = pProj->m_bCareAboutRCFiles;
			m_fCareAboutDefFiles = pProj->m_bCareAboutDefFiles;
		}
	}


#ifdef _DEBUG
	// Used by CFileRegistry::AssertValid
	m_bAssertValidCalled = FALSE;
#endif
}

CFileRegistry::~CFileRegistry()
{
#ifndef REFCOUNT_WORK
	// Only clean up the global file registry
	if (this != &g_FileRegistry)
		return;

	// Delete any remaining file reg entries, although there really
	// shouldn't be any at this point.
	DeleteAll();

#ifdef _BLD_REG_HISTORY
	// Remove all ref counting histories
	FileRegHandle frh;
	CFRHHistory * pHistory;
	pos = m_mapFRHHistory.GetStartPosition();
	while (pos != NULL)
	{
		m_mapFRHHistory.GetNextAssoc(pos, (void *&)frh, (void *&)pHistory);
		delete pHistory;
	}
	m_mapFRHHistory.RemoveAll();
#endif

#else //ifndef REFCOUNT_WORK

#ifdef _DEBUG
	if (!IsEmpty() )
		AfxDump(this);
#endif
	ASSERT( IsEmpty() );

	// Remove filters.
	POSITION pos0 = m_FilterMap.GetStartPosition();
	while(NULL!=pos0)
	{
		FileRegHandle frh0;
		CString strKey;
		m_FilterMap.GetNextAssoc(pos0, strKey, (void*&)frh0);
		if (frh0->IsNodeType(nodetypeRegFilter))
		{
			delete frh0;
		}
		else if (frh0->IsNodeType(nodetypeRegFilterSet))
		{
			delete frh0;
		}
		else
		{
			// Must be a CFileRegFilter or CFileRegSet containing filters.
			ASSERT(FALSE);
		}
	}

#endif //ifndef REFCOUNT_WORK
}

// olympus 16 (briancr)
// Provide mechanism for clearing file registry. Theoretically, this
// shouldn't be needed, since each FRH is deleted when its ref count
// gets to zero.
// Warning: Calling this function other than when a CFileRegistry object
// is being deleted may have ill side-effects. Specifically, if there is an
// FRH cached somewhere and it's later used to look up something in
// the CFileRegistry, the lookup will fail, or worse, return bogus info.
#ifndef REFCOUNT_WORK
void CFileRegistry::DeleteAll(void)
{
#ifdef _DEBUG
	const char* const THIS_FUNCTION = "CFileRegistry::DeleteAll";
#endif

	// Clear the scanner cache
	// The scanner cache must be cleared before the FRHs are deleted from m_Contents
	g_ScannerCache.Clear();

	// Delete all entries from m_Contents
#ifdef _DEBUG
	int nCount = m_Contents.GetCount();
	if (nCount > 0) {
		TRACE("%s: deleting %d still referenced file reg entries from m_Contents.\n", THIS_FUNCTION, nCount);
	}
#endif

	// Remove any file reg entries that still exist, by destroying them
	// completely. NOTE: Removing them from our containment list is not
	// enough, as they may have ref counts that are higher than 1 and
	// will cause them to stay around, causing memory leaks.
	POSITION pos;
	for ( pos = m_Contents.GetHeadPosition(); pos != (POSITION)NULL; ) {
		FileRegHandle hndFileReg = (FileRegHandle)m_Contents.GetNext(pos);
		delete hndFileReg;
	}
	m_Contents.RemoveAll();

	// Delete all entries from m_pmapContents
#ifdef _DEBUG
	nCount = m_pmapContents->GetCount();
	if (nCount > 0) {
		TRACE("%s: removing %d entries from m_pmapContents.\n", THIS_FUNCTION, nCount);
	}
#endif
	m_pmapContents->RemoveAll();

	// Delete all entries from m_filelistDEF
#ifdef _DEBUG
	nCount = m_filelistDEF.GetCount();
	if (nCount > 0) {
		TRACE("%s: removing %d entries from m_filelistDEF.\n", THIS_FUNCTION, nCount);
	}
#endif
	m_filelistDEF.RemoveAll();

	// Delete all entries from m_filelistODL
#ifdef _DEBUG
	nCount = m_filelistODL.GetCount();
	if (nCount > 0) {
		TRACE("%s: removing %d entries from m_filelistODL.\n", THIS_FUNCTION, nCount);
	}
#endif
	m_filelistODL.RemoveAll();

	// Delete all entries from m_filelistRC
#ifdef _DEBUG
	nCount = m_filelistRC.GetCount();
	if (nCount > 0) {
		TRACE("%s: removing %d entries from m_filelistRC.\n", THIS_FUNCTION, nCount);
	}
#endif
	m_filelistRC.RemoveAll();

	// Delete all entries from m_LocalFileRegs
#ifdef _DEBUG
	nCount = m_LocalFileRegs.GetCount();
	if (nCount > 0) {
		TRACE("%s: removing %d entries from m_LocalFileRegs.\n", THIS_FUNCTION, nCount);
	}
#endif
	m_LocalFileRegs.RemoveAll();

	// Delete all entries from m_FileMap
#ifdef _DEBUG
	nCount = m_FileMap.GetCount();
	if (nCount > 0) {
		TRACE("%s: removing %d entries from m_FileMap.\n", THIS_FUNCTION, nCount);
	}
#endif
	m_FileMap.RemoveAll();

	// Delete all entries from m_FilterMap
#ifdef _DEBUG
	nCount = m_FilterMap.GetCount();
	if (nCount > 0) {
		TRACE("%s: removing %d entries from m_FilterMap.\n", THIS_FUNCTION, nCount);
	}
#endif
	m_FilterMap.RemoveAll();

	// Delete all entries from g_FileChangeQ
#ifdef _DEBUG
	nCount = g_FileChangeQ.GetCount();
	if (nCount > 0) {
		TRACE("%s: removing %d entries from g_FileChangeQ.\n", THIS_FUNCTION, nCount);
	}
#endif
	// "new'd" structs on this list must be freed!
	while ( !g_FileChangeQ.IsEmpty() ) {
		S_ATTRIB * ptr = (S_ATTRIB *) g_FileChangeQ.GetHead();
		delete ptr;
		g_FileChangeQ.RemoveHead();
	}
	g_FileChangeQ.RemoveAll();

	// Delete all entries from g_FileDepUpdateListQ
#ifdef _DEBUG
	nCount = g_FileDepUpdateListQ.GetCount();
	if (nCount > 0) {
		TRACE("%s: removing %d entries from g_FileDepUpdateListQ.\n", THIS_FUNCTION, nCount);
	}
#endif
	g_FileDepUpdateListQ.RemoveAll();

	// Delete all entries from g_FileForceUpdateListQ
#ifdef _DEBUG
	nCount = g_FileForceUpdateListQ.GetCount();
	if (nCount > 0) {
		TRACE("%s: removing %d entries from g_FileForceUpdateListQ.\n", THIS_FUNCTION, nCount);
	}
#endif
	g_FileForceUpdateListQ.RemoveAll();
}
#endif //ifndef REFCOUNT_WORK
// end olympus 16 fix

FileRegHandle CFileRegistry::RegisterFile
(
	const CPath* pPath,
	BOOL bMatchCase /* = FALSE */,
	BOOL bNoFilter /* = TRUE */
)
{
	if (!pPath->IsInit())
	{
		ASSERT(FALSE);				// caller shouldn't allow this
		return (FileRegHandle)NULL;	// can't do
	}

	// Lookup file in the global file registry thing
	// If its's there then get its FileRegHandle
	// otherwise create a new CFileRegFile item
	// and get its FileRegHandle
	FileRegHandle hndFileReg = NULL;
    TCHAR szKey[MAX_PATH];

#ifndef REFCOUNT_WORK
    _tcscpy(szKey, *pPath);
    _tcsupr(szKey);

	// Ensure sequential access to file map
	{
		CritSectionT cs(g_sectionFileMap);
		g_FileRegistry.m_FileMap.Lookup(szKey,(void*&) hndFileReg);
	}

	if( this != &g_FileRegistry ){
#ifdef _DEBUG
		// Save for comparison with handle returned by g_FileRegistry.RegisterFile.
		FileRegHandle tmpFRH = hndFileReg;
#endif
		hndFileReg = g_FileRegistry.RegisterFile( pPath, bMatchCase, bNoFilter );

#ifdef _DEBUG
		// g_FileRegistry.RegisterFile should return existing handle.
        if( tmpFRH != NULL )
			ASSERT( tmpFRH==hndFileReg );
#endif
	}

	if (hndFileReg == NULL)
	{
		ASSERT( this == &g_FileRegistry );

		// Should not already be in this registry.
		ASSERT( NULL == LookupFile((LPCTSTR)*pPath) );

#ifdef _DEBUG
		if ( !m_bAssertValidCalled )
		{
			ASSERT_VALID( this );
			m_bAssertValidCalled = TRUE; // Can't be done in AssertValid
		}
#endif

		// Require filtered?
		CFileRegFile * pregfile = new CFileRegFile(pPath, bMatchCase, !bNoFilter);

		{
			CritSectionT cs(g_sectionFileMap);
			m_FileMap.SetAt(szKey, hndFileReg = (FileRegHandle)pregfile);
		}
		AddRegHandle(hndFileReg);
	}
	else
	{
		// Check to see if we are trying to register
		// a file that is already registered with this
		// file registry. If so then we fail the
		// registration
		// UNDONE (colint): Irrelevant
        // VERIFY(m_FileMap.Lookup(szKey, (void*&)hndFileReg));
 		FileRegHandle hndTemp;
		// Don't do a 2nd LookupFile if this is g_FileRegistry.
		if (this == &g_FileRegistry)
			hndTemp = hndFileReg;
		else
 			hndTemp = LookupFile((LPCTSTR)*pPath);

		if (hndTemp == NULL)
		{
#ifdef _DEBUG
			if ( !m_bAssertValidCalled )
			{
				ASSERT_VALID( this );
				m_bAssertValidCalled = TRUE; // Can't be done in AssertValid
			}
#endif

			ASSERT( &g_FileRegistry != this );

 			// Require filtered?
			{
 				CritSectionT cs(g_sectionFileMap);
	 			m_FileMap.SetAt(szKey, hndFileReg);
			}

			((CFileRegFile *)hndFileReg)->m_bFiltered = !bNoFilter;
 			AddRegHandle(hndFileReg);
		}
		else
			AddRegRef(hndFileReg);

		hndFileReg->SetMatchCase(bMatchCase);
   	}
#else
	hndFileReg = CFileRegFile::GetFileHandle(*pPath, szKey);
	if (NULL!=hndFileReg)
	{
		AddRegRef(hndFileReg);
		hndFileReg->SetMatchCase(bMatchCase);
		hndFileReg->ReleaseFRHRef();
	}
#endif //#ifndef REFCOUNT_WORK

	return hndFileReg;
}

#ifndef REFCOUNT_WORK
void CFileRegistry::AllowAccess(CFileRegistry* pfreg)
{
	POSITION pos = m_LocalFileRegs.Find(pfreg);
	if (pos == (POSITION)NULL)
		m_LocalFileRegs.AddTail(pfreg);
}

BOOL CFileRegistry::DisallowAccess(CFileRegistry* pfreg)
{
	POSITION pos = m_LocalFileRegs.Find(pfreg);
	if (pos != (POSITION)NULL)
	{
		m_LocalFileRegs.RemoveAt(pos);
		return TRUE;
	}
	return FALSE;
}
#endif //ifndef REFCOUNT_WORK

#ifndef REFCOUNT_WORK
BOOL CFileRegistry::RemoveFile(const CPath* pPath)
{
	// Ensure sequential access to file map
	CritSectionT cs(g_sectionFileMap);

	// Lookup the file in this file registry
	// If it exists then we can remove it	
	FileRegHandle hndFileReg;

	ASSERT(pPath->IsInit());

    TCHAR szKey[MAX_PATH];
    _tcscpy(szKey, *pPath);
    _tcsupr(szKey);

	BOOL fRet = FALSE;
 
 	if (m_FileMap.Lookup(szKey, (void*&)hndFileReg))
	{
		// remove from map and from our set
 		m_FileMap.RemoveKey(szKey);

		(void)RemoveRegHandle(hndFileReg);

		// Release extra reference in global registry.
		if (this!=&g_FileRegistry)
			g_FileRegistry.ReleaseRegRef(hndFileReg);

		fRet = TRUE;
	}

	return fRet;
}
#endif //ifndef REFCOUNT_WORK

#ifdef _BLD_REG_HISTORY
void CFileRegistry::AddRegRefI(FileRegHandle hndFileReg, TCHAR * szFile, int nLine)
#else
void CFileRegistry::AddRegRef(FileRegHandle hndFileReg)
#endif
{
#ifdef _BLD_REG_HISTORY
	CFRHHistory * pOldHistory = NULL;
	m_mapFRHHistory.Lookup(hndFileReg, (void *&)pOldHistory);
	CFRHHistory * pHistory = new CFRHHistory(szFile, nLine, TRUE, pOldHistory);
	m_mapFRHHistory.SetAt(hndFileReg, pHistory);
#endif

#ifndef REFCOUNT_WORK

#if 0	// defined(_DEBUG)
	// A CFileRegFile must already be ref counted somewhere if not in this->m_Contents.
	if ( NULL == m_Contents.Find(hndFileReg) )
	{
		// Note: GetFilePath returns NULL if hndFileReg is not a CFileRegFile.
		CPath const * pPath = hndFileReg->GetFilePath();
		CString strPath;
		if ( pPath )
		{
			strPath = pPath->GetFullPath();

			// Must have been added somewhere else already.
			ASSERT( hndFileReg->GetRefCount() > 0 );
		}
	}
#endif

	ASSERT(hndFileReg);
 	hndFileReg->AddFRHRef();

	CFileRegEntry * pFileRegEntry = GetRegEntry(hndFileReg);

	// increment our set reference count
	{
		WORD wRefCount = 0;
		
		ASSERT(m_pmapContents != NULL);
		BOOL bFound = m_pmapContents->Lookup(hndFileReg, wRefCount);

        // FUTURE (colint): I would really like to enable this assert
        // but we need to fix addition of things to sets so that we
        // don't end up with an entry in a registry's m_pmapContents
        // but not in it's m_Contents.
		//if (hndFileReg->IsKindOf(RUNTIME_CLASS(CFileRegFile)))
		//	ASSERT(bFound);

		m_pmapContents->SetAt(hndFileReg, wRefCount+1);
	}
#else //ifndef REFCOUNT_WORK

	ASSERT(m_pmapContents != NULL);

	WORD wRefCount = 0;
	BOOL bFound = m_pmapContents->Lookup(hndFileReg, wRefCount);

	if (!bFound)
	{
		// Add to registry.
		// AddRegHandle takes care of actual ref counts on hndFileReg.
		VERIFY( AddRegistryHandle(hndFileReg) );
		wRefCount = 0;
	}

	wRefCount++;
	m_pmapContents->SetAt(hndFileReg, wRefCount);

#endif //#ifndef REFCOUNT_WORK
}

#ifdef _BLD_REG_HISTORY
void CFileRegistry::ReleaseRegRefI(FileRegHandle hndFileReg, TCHAR * szFile, int nLine)
#else
void CFileRegistry::ReleaseRegRef(FileRegHandle hndFileReg)
#endif
{
#ifdef _BLD_REG_HISTORY
	CFRHHistory * pOldHistory = NULL;
	m_mapFRHHistory.Lookup(hndFileReg, (void *&)pOldHistory);
	CFRHHistory * pHistory = new CFRHHistory(szFile, nLine, FALSE, pOldHistory);
	m_mapFRHHistory.SetAt(hndFileReg, pHistory);
#endif

	ASSERT(hndFileReg);
	if (!hndFileReg)
		return; // just bail

#ifndef REFCOUNT_WORK
	CFileRegEntry * pFileRegEntry = GetRegEntry(hndFileReg);

	// decrement the set reference count
	{
		WORD wRefCount;
		ASSERT(m_pmapContents != NULL);
		if (!m_pmapContents->Lookup(hndFileReg, wRefCount))
			wRefCount = 0;

		if (wRefCount != 0)
		{
 			wRefCount--;
			m_pmapContents->SetAt(hndFileReg, wRefCount);

			// Check to see if the registry reference count went to 0
			if (wRefCount == 0)
			{
				// A CFileRegEntry has been removed from this registry
				// so we may need to unfilter it, only do this if we are not going to
				// do a global destroy
				if (hndFileReg->GetRefCount() > 1)
					pFileRegEntry->InformNotifyRxs(FRN_LCL_DESTROY, (DWORD)(void *)m_pFileRegistry);

                // Make sure that any m_pmapContents (i.e. local ref count),
                // is removed completely
				ASSERT(m_pmapContents != NULL);
                m_pmapContents->RemoveKey(hndFileReg);
			}
		}
	}

	// Must release the actual ref after we have checked for whether
	// this CFileRegEntry has been removed from this registry, as
	// otherwise the object may delete itself.
	hndFileReg->ReleaseFRHRef();

#else //ifndef REFCOUNT_WORK

	WORD wRefCount;
	ASSERT(m_pmapContents != NULL);
	ASSERT (m_pmapContents->Lookup(hndFileReg, wRefCount));

	if (!m_pmapContents->Lookup(hndFileReg, wRefCount))
		wRefCount = 0;

	ASSERT(0!=wRefCount);

	if (wRefCount != 0)
 		wRefCount--;

	if (wRefCount != 0)
	{
		m_pmapContents->SetAt(hndFileReg, wRefCount);
	}
	else
	{
		m_pmapContents->RemoveKey(hndFileReg);

		// RemoveRegHandle takes care of actual ref counts on hndFileReg.
		VERIFY(RemoveRegistryHandle(hndFileReg));
	}

#endif //ifndef REFCOUNT_WORK

}

FileRegHandle CFileRegistry::FindBasicFilter(const CString* pstrBasicFilter)
{
	// Find a basic file filter
	FileRegHandle hndFileFilter;

	if (m_FilterMap.Lookup(*pstrBasicFilter, (void*&)hndFileFilter))
		return hndFileFilter;

#ifndef REFCOUNT_WORK
	// Ok, so now we have to check any sets we have as file filters
	POSITION pos = m_FilterMap.GetStartPosition();
	while (pos != NULL)
	{
		FileRegHandle hndFileReg;
		CString strFile;
		m_FilterMap.GetNextAssoc(pos, strFile, (void*&)hndFileReg);
		if (!hndFileReg->IsNodeType(nodetypeRegFilter))
		{
 			// This set will only contain file filters, so check these!
			const CPtrList* pFilterList = ((CFileRegSet*)hndFileReg)->GetContent();

			POSITION FilterPos = pFilterList->GetHeadPosition();
			while (FilterPos != NULL)
			{
				hndFileReg = (FileRegHandle) pFilterList->GetNext(FilterPos);
				ASSERT(hndFileReg->IsKindOf(RUNTIME_CLASS(CFileRegFilter)));
				ASSERT(hndFileReg->IsNodeType(nodetypeRegFilter));
				if (*((CFileRegFilter*)hndFileReg)->GetFilter() == *pstrBasicFilter)
					return hndFileReg;
			}
		}
	}
#endif

	return NULL;
}

FileRegHandle CFileRegistry::RegisterFileFilter(const CString* pstrFilter)
{
	FileRegHandle hndFileReg;

	hndFileReg = LookupFileFilter(pstrFilter);

	// Are we trying to register a file filter that has already been
	// registered?
#ifndef REFCOUNT_WORK
	if (hndFileReg)
		return NULL;
#else
	if (hndFileReg)
	{
		hndFileReg->AddFRHRef();
		return hndFileReg;
	}
#endif

	CStringList BasicFilters;
	GetBasicFilters(pstrFilter, BasicFilters);

	if (BasicFilters.GetCount() > 1)
	{
#ifndef REFCOUNT_WORK
		// Create the set
		hndFileReg = (FileRegHandle) new CFileRegSet;

		AddRegHandle(hndFileReg);
#else
		// Create the set
		hndFileReg = (FileRegHandle) new CFileRegFilterSet;

		hndFileReg->AddFRHRef();
#endif
 
		// Multiple file filter
		CPtrList NewFilters;

		FileRegHandle hndFileFilter;
		POSITION pos = BasicFilters.GetHeadPosition();
		while (pos != NULL)
		{
			CString strBasicFilter = BasicFilters.GetNext(pos);
			hndFileFilter = FindBasicFilter(&strBasicFilter);
			if (hndFileFilter == NULL)
			{
#ifndef REFCOUNT_WORK
	 			// Nope we don't have one so now we create it and 
				// set an entry in our map.
				hndFileFilter = (FileRegHandle)new CFileRegFilter(strBasicFilter);
				 		
				// Ensure sequential access to file map
				CritSectionT cs(g_sectionFileMap);

				// Filter all files through this file filter
				CString strFileKey;
				FileRegHandle hndFile;
				POSITION pos = m_FileMap.GetStartPosition();
				while (pos != NULL)
				{
					m_FileMap.GetNextAssoc(pos, strFileKey, (void*&)hndFile);
					ASSERT(hndFile->IsNodeType(nodetypeRegFile);
					((CFileRegFilter*)hndFileFilter)->AddRegHandle(hndFile);
				}
#else
				hndFileFilter = RegisterFileFilter(&strBasicFilter);
				hndFileFilter->ReleaseFRHRef();
#endif
 			}

#ifndef REFCOUNT_WORK
			((CFileRegSet*)hndFileReg)->AddRegHandle(hndFileFilter);
#else
			((CFileRegFilterSet*)hndFileReg)->AddFilter((CFileRegFilter*)hndFileFilter);
#endif
 		}
 	}
	else
	{
		// Single basic file filter
		// First we had better check to see if we actually have created a
		// file filter like this, but it is not registered.
		CString strKey = BasicFilters.GetHead();
		hndFileReg = FindBasicFilter(&strKey);
		if (hndFileReg == NULL)
		{
 			// Nope we don't have one so now we create it and 
			// set an entry in our map.
			hndFileReg = (FileRegHandle)new CFileRegFilter(strKey);
#ifndef REFCOUNT_WORK
 			AddRegHandle(hndFileReg);
#else
			hndFileReg->AddFRHRef();
#endif
 
#ifndef REFCOUNT_WORK
			// Ensure sequential access to file map
			CritSectionT cs(g_sectionFileMap);
#endif

			// Filter all files through this file filter
			CString strFileKey;
			FileRegHandle hndFile;
#ifdef REGISTRY_FILEMAP
			POSITION pos = m_FileMap.GetStartPosition();
#else
			POSITION pos = m_Contents.GetHeadPosition();
#endif
			while (pos != NULL)
			{
#ifdef REGISTRY_FILEMAP
				m_FileMap.GetNextAssoc(pos, strFileKey, (void*&)hndFile);
#ifndef REFCOUNT_WORK
				((CFileRegFilter*)hndFileReg)->AddRegHandle(hndFile);
#else
				((CFileRegFilter*)hndFileReg)->AddFilterHandle(hndFile);
#endif
#else	//	REGISTRY_FILEMAP
				hndFile = (FileRegHandle)m_Contents.GetNext(pos);
				ASSERT(NULL!=hndFile);
				if (hndFile->IsNodeType(nodetypeRegFile))
					((CFileRegFilter*)hndFileReg)->AddFilterHandle(hndFile);
#endif
			}
		}
		else
		{
#ifndef REFCOUNT_WORK
			// We already have the file filter so just use it!
			if (!AddRegHandle(hndFileReg))
				return NULL;
#else
			// FindBasicFilter() should not succeed after LookupFileFilter() failed.
			ASSERT(FALSE);
#endif
		}
	}

	// Put an entry in our registered file filter map
    TCHAR szKey[MAX_PATH];
    _tcscpy(szKey, *pstrFilter);
    _tcsupr(szKey);
 	m_FilterMap.SetAt(szKey, hndFileReg);

	return hndFileReg;
}

BOOL CFileRegistry::RemoveFileFilter(const CString* pstrFilter)
{
	// We can only remove file filters that have been registered
	FileRegHandle hndFileReg;

    TCHAR szKey[MAX_PATH];
    _tcscpy(szKey, *pstrFilter);
    _tcsupr(szKey);

 	if (!m_FilterMap.Lookup(szKey, (void*&)hndFileReg))
 		return FALSE;

 	if (!hndFileReg->IsNodeType(nodetypeRegFilter))
	{
		ASSERT(hndFileReg->IsKindOf(RUNTIME_CLASS(CFileRegSet)));
		ASSERT(hndFileReg->IsNodeType(nodetypeRegSet));
 		CFileRegSet* hndFileSet = (CFileRegSet*)hndFileReg;

#ifndef REFCOUNT_WORK
		const CPtrList* pFilterList = hndFileSet->GetContent();
#else
		const CPtrList* pFilterList = &hndFileSet->m_Contents;
#endif

		POSITION pos = pFilterList->GetHeadPosition();
		while (pos != NULL)
		{
			FileRegHandle hndFileRegFilter = (FileRegHandle) pFilterList->GetNext(pos);
#ifndef REFCOUNT_WORK
			hndFileSet->RemoveRegHandle(hndFileRegFilter);
#else
			POSITION posFilter = m_Contents.Find(hndFileRegFilter);
			if (NULL!=posFilter)
			{
				hndFileSet->m_Contents.RemoveAt(posFilter);
				hndFileRegFilter->ReleaseFRHRef();
			}
#endif
		}
 	}

	m_FilterMap.RemoveKey(szKey);

#ifndef REFCOUNT_WORK
	return RemoveRegHandle(hndFileReg);
#else
	hndFileReg->ReleaseFRHRef();
	return TRUE;
#endif
}

FileRegHandle CFileRegistry::LookupFile(LPCTSTR lpszFile)
{
#ifdef REGISTRY_FILEMAP

#ifndef REFCOUNT_WORK
	// Ensure sequential access to file map
	CritSectionT cs(g_sectionFileMap);
#endif

	FileRegHandle hndFileReg = (FileRegHandle)NULL;

    TCHAR szKey[MAX_PATH];
    _tcscpy(szKey, lpszFile);
    _tcsupr(szKey);

	// In our map?
	// ASSERT((UINT)m_FileMap.GetCount() < ((UINT)m_FileMap.GetHashTableSize() << 1)); // UNDONE
	if (!m_FileMap.Lookup(szKey, (void*&) hndFileReg))
	{
		POSITION pos = m_LocalFileRegs.GetHeadPosition();
#ifdef REFCOUNT_WORK
		// Local registries not used.
		ASSERT(pos==NULL);
#endif
		while (pos != NULL)
		{
			// In any of our local maps?
			CFileRegistry* pfreg = (CFileRegistry*) m_LocalFileRegs.GetNext(pos);
			hndFileReg = pfreg->LookupFile(lpszFile);
			if (hndFileReg != (FileRegHandle)NULL)
				break;
		}
	}
#ifdef REFCOUNT_WORK
	else
	{
		ASSERT(NULL!=hndFileReg);
		hndFileReg->AddFRHRef();
	}
#endif

#else
	FileRegHandle hndFileReg = CFileRegFile::GetFileHandle(lpszFile);
	if (NULL!=hndFileReg)
	{
		if (!RegHandleExists(hndFileReg))
		{
			hndFileReg->ReleaseFRHRef();
			hndFileReg = NULL;
		}
	}
#endif

	return hndFileReg;
}

FileRegHandle CFileRegistry::LookupFile(const CPath* pPath)
{
	// REVIEW: this override should be eliminated!!!

	ASSERT(pPath->IsInit());
	return CFileRegistry::LookupFile(LPCTSTR(*pPath));
}

FileRegHandle CFileRegistry::LookupFileFilter(const CString* pstrFilter)
{
	// Create a list of the basic file filters
 	CStringList BasicFilters;

	GetBasicFilters(pstrFilter, BasicFilters);

	// FUTURE (colint): NYI
 	// Generate permutations of the basic file filters
	CStringList Permutations;

	// FUTURE (colint): For now we just add one of the permutations
    TCHAR szTempPerm[MAX_PATH];
    _tcscpy(szTempPerm, *pstrFilter);
    _tcsupr(szTempPerm);
	Permutations.AddTail(szTempPerm);

	// Lookup each of the permutations
	FileRegHandle hndFileReg;

	
	POSITION pos = Permutations.GetHeadPosition();
	while (pos != NULL)
	{
		CString strKey = Permutations.GetNext(pos);
		if (m_FilterMap.Lookup(strKey, (void*&)hndFileReg))
			return hndFileReg;
	}

#ifndef REFCOUNT_WORK
	// We haven't found the filter in this registry, so look in 
	// associated ones too!
	pos = m_LocalFileRegs.GetHeadPosition();
	while (pos != NULL)
	{
		CFileRegistry* pfreg = (CFileRegistry*) m_LocalFileRegs.GetNext(pos);
		hndFileReg = pfreg->LookupFileFilter(pstrFilter);
		if (hndFileReg != NULL)
			return hndFileReg;
	}
#endif
 
 	return NULL;
}

void CFileRegistry::GetBasicFilters(const CString* pstrFilter, CStringList& strList)
{  		 
	CString strKey = *pstrFilter;
	strKey.MakeUpper();
	strKey += _T(";");

	int nBreak = -1;
	do
	{
		nBreak = strKey.Find(_T(';'));
		if (nBreak != -1)
		{
			CString strBasicFilter = strKey.Left(nBreak);
			strList.AddTail(strBasicFilter);
			strKey = strKey.Right(strKey.GetLength() - nBreak - 1);
		}
	} while (nBreak != -1);
}

void CFileRegistry::FilterFile(FileRegHandle hndFile)
{
	ASSERT(hndFile->IsKindOf(RUNTIME_CLASS(CFileRegFile)));
	ASSERT(hndFile->IsNodeType(nodetypeRegFile));

	// Notify all file filters that we have in the registry at
	// the moment.
	POSITION pos = m_FilterMap.GetStartPosition();
	while (pos != NULL)
	{
		FileRegHandle hndFileReg;
		CString strFile;
		m_FilterMap.GetNextAssoc(pos, strFile, (void*&)hndFileReg);
		if (hndFileReg->IsNodeType(nodetypeRegFilter))
		{
#ifndef REFCOUNT_WORK
			((CFileRegFilter*)hndFileReg)->AddRegHandle(hndFile);
#else
			((CFileRegFilter*)hndFileReg)->AddFilterHandle(hndFile);
#endif
		}
#ifndef REFCOUNT_WORK
		else
		{
			ASSERT(hndFileReg->IsKindOf(RUNTIME_CLASS(CFileRegSet)));
			ASSERT(hndFileReg->IsNodeType(nodetypeRegSet));

 			// This set will only contain file filters, so check these!
			const CPtrList* pFilterList = ((CFileRegSet*)hndFileReg)->GetContent();

			POSITION FilterPos = pFilterList->GetHeadPosition();
			while (FilterPos != NULL)
			{
				hndFileReg = (FileRegHandle) pFilterList->GetNext(FilterPos);
				ASSERT(hndFileReg->IsKindOf(RUNTIME_CLASS(CFileRegFilter)));
				ASSERT(hndFileReg->IsNodeType(nodetypeRegFilter));
				((CFileRegFilter*)hndFileReg)->AddRegHandle(hndFile);
 			}
		}
#endif //#ifndef REFCOUNT_WORK
	}
}

#ifdef REFCOUNT_WORK
void CFileRegistry::UnFilterFile(FileRegHandle hndFile)
{
	ASSERT(hndFile->IsKindOf(RUNTIME_CLASS(CFileRegFile)));
	ASSERT(hndFile->IsNodeType(nodetypeRegFile));

	// Notify all file filters that we have in the registry at
	// the moment.
	POSITION pos = m_FilterMap.GetStartPosition();
	while (pos != NULL)
	{
		FileRegHandle hndFileReg;
		CString strFile;
		m_FilterMap.GetNextAssoc(pos, strFile, (void*&)hndFileReg);
		if (hndFileReg->IsNodeType(nodetypeRegFilter))
		{
			((CFileRegFilter*)hndFileReg)->RemoveFilterHandle(hndFile);
		}
	}
}
#endif //#ifdef REFCOUNT_WORK

void CFileRegistry::OnNotify(FileRegHandle hndFileReg, UINT idChange, DWORD dwHint)
{
	ASSERT(hndFileReg);
	
	switch (idChange)
	{
#ifndef REFCOUNT_WORK
		case FRN_ADD:
		{
 			// If we are being told that a file has been added
			// then notify our filters
			if (hndFileReg->IsNodeType(nodetypeRegFile))
			{
				if (((CFileRegFile *)hndFileReg)->m_bFiltered)
					FilterFile(hndFileReg);
 			}
			break;
	  	}

		case FRN_LCL_DESTROY:
		{
			// Only do a FRN_DESTROY if it a local destroy in us
			if ((CFileRegistry *)dwHint != m_pFileRegistry)
				return;	// ignore completely

			// Remove us as a dependant of the item just removed
			hndFileReg->RemoveNotifyRx(this);

			// Fall thru'
		}

		case FRN_DESTROY:
		{
			// Something is being destroyed that we contain
			// Better remove any trace of it from us
			POSITION pos = m_Contents.Find(hndFileReg);
			if (pos != NULL)
				m_Contents.RemoveAt(pos);

			ASSERT(m_pmapContents != NULL);
			m_pmapContents->RemoveKey(hndFileReg);

			// Ensure sequential access to file map
			CritSectionT cs(g_sectionFileMap);

			CString strKey;
			if (hndFileReg->IsNodeType(nodetypeRegFile))
			{
#ifdef REGISTRY_FILEMAP
				// A file is going away that we still contain,
				// so remove it from our FileMap
				TCHAR szKey[MAX_PATH];
			    _tcscpy(szKey, *hndFileReg->GetFilePath());
				_tcsupr(szKey);
 
				m_FileMap.RemoveKey(szKey);
#endif
			}
			else if (hndFileReg->IsNodeType(nodetypeRegFilter))
			{
				// A basic filter is going away that we still contain
				// so we remove it from our FilterMap
				TCHAR szKey[MAX_PATH];
				_tcscpy(szKey, *((CFileRegFilter*)hndFileReg)->GetFilter());
				_tcsupr(szKey);
 
				m_FilterMap.RemoveKey(szKey);
			}
			else
			{
				// A compound filter is going away that we still contain
				// so we remove it from out FilterMap
				ASSERT(hndFileReg->IsKindOf(RUNTIME_CLASS(CFileRegSet)));
				ASSERT(hndFileReg->IsNodeType(nodetypeRegSet));

				const CPtrList* pFilterList = ((CFileRegSet*)hndFileReg)->GetContent();

				// Figure out the name of this compound filter based on the
				// basic filters that compose it.
				POSITION FilterPos = pFilterList->GetHeadPosition();
				while (FilterPos != NULL)
				{
					hndFileReg = (FileRegHandle) pFilterList->GetNext(FilterPos);
					ASSERT(hndFileReg->IsKindOf(RUNTIME_CLASS(CFileRegFilter)));
					ASSERT(hndFileReg->IsNodeType(nodetypeRegFilter));
					strKey += *((CFileRegFilter*)hndFileReg)->GetFilter();
					if (FilterPos != NULL)
						strKey += _T(';');
 				}

				m_FilterMap.RemoveKey(strKey);
			}

			break;
		}
#else //#ifndef REFCOUNT_WORK

		//
		// Take care of references counts.
		// (Add/Release)RegRef() will do the work.
		//
		case FRI_ADD:
		{
			CFileRegSet* pregset = (CFileRegSet*)hndFileReg;

			ASSERT(pregset->IsNodeType(nodetypeRegSet));
			ASSERT(!pregset->IsNodeType(nodetypeRegRegistry));
			ASSERT(this==pregset->m_pFileRegistry);

			CFileRegEntry* regent = (CFileRegEntry*)dwHint;
			AddRegRef(regent);
			break;
		}
		case FRI_DESTROY:
		{
			CFileRegSet* pregset = (CFileRegSet*)hndFileReg;

			ASSERT(pregset->IsNodeType(nodetypeRegSet));
			ASSERT(!pregset->IsNodeType(nodetypeRegRegistry));
			ASSERT(this==pregset->m_pFileRegistry);

			CFileRegEntry* regent = (CFileRegEntry*)dwHint;
			ReleaseRegRef(regent);
			break;
		}
#endif //#ifndef REFCOUNT_WORK

	}
}

BOOL CFileRegistry::DeleteFile(FileRegHandle hndFileReg)
{
	ASSERT(hndFileReg);
	ASSERT(hndFileReg->IsKindOf(RUNTIME_CLASS(CFileRegFile)));
	ASSERT(hndFileReg->IsNodeType(nodetypeRegFile));

	const CPath * pPath = ((CFileRegFile*)hndFileReg)->GetFilePath();
 	ASSERT(pPath != (const CPath *)NULL);

	BOOL fRet = ::DeleteFile((const TCHAR*)*pPath);

	if (fRet)
	{
		// it's gone, let's assume this and set the attributes
		S_ATTRIB attrib;
		attrib._fOk = TRUE;
		attrib._filetime.dwLowDateTime = attrib._filetime.dwHighDateTime = 0;	// none
		attrib._fExists = FALSE;

		((CFileRegFile*)hndFileReg)->SetAttributes(&attrib);
	}

	return fRet;
}

BOOL CFileRegistry::GetFileItemList(const CTargetItem * pTargetFilter,
									const CPtrList * pFileList,
									CObList & FileItemList,
									BOOL fOnlyBuildable /*= TRUE*/, const ConfigurationRecord * pcr /*= NULL*/)
{
	FileItemList.RemoveAll();

	POSITION pos = pFileList->GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		CObList * oblist = ((CFileRegEntry *)pFileList->GetNext(pos))->plstFileItems;
		if (oblist==NULL)
		{
			ASSERT(0);
			continue;
		}
		POSITION pos2 = oblist->GetHeadPosition();
		while (pos2 != (POSITION)NULL)
		{
			CFileItem * pFile = (CFileItem *)oblist->GetNext(pos2);
			CTargetItem * pTarget = pFile->GetTarget();
			CProject * pProject = pTarget->GetProject();

			// If the caller doesn't want FileItem's for this target, 
			// continue on.
			// pInputTarget == NULL implies caller wants FileItem's for all
			// targets.

			if (pTargetFilter != NULL && pTargetFilter != pTarget )
				continue;

			// do this with the all project items
			// in the specified config.
			CProjTempConfigChange projTempConfigChange(pProject);

			if (pcr != NULL)
				projTempConfigChange.ChangeConfig((ConfigurationRecord *)pcr->m_pBaseRecord);

			// add to our list if this is buildable, or we don't care
			int fExcluded;
			ConfigurationRecord* pcrTarget = (ConfigurationRecord*) pTarget->GetActiveConfig();
			ASSERT(pTarget != NULL);

			if ((!fOnlyBuildable || (pFile->GetIntProp(P_ItemExcludedFromBuild, fExcluded) == valid && !fExcluded)) &&
				pcrTarget->IsValid())
				FileItemList.AddTail(pFile);

		}
	}

	return TRUE;
}

BOOL CFileRegistry::GetFileList(const CPath * pPath, const CPtrList * & pFileList)
{
	ASSERT(pPath->IsInit());
	pFileList = (const CPtrList *)NULL;
	const TCHAR * pchExt = pPath->GetExtension();

	// resource file?
	if (m_fCareAboutRCFiles && _tcsicmp(pchExt, _TEXT(".rc")) == 0)
		pFileList = GetRCFileList();

	// definition file?
	else if (m_fCareAboutDefFiles && _tcsicmp(pchExt, _TEXT(".def")) == 0)
		pFileList = GetDEFFileList();
		
	// odl file?
	else if (_tcsicmp(pchExt, _TEXT(".odl")) == 0)
		pFileList = GetODLFileList();
		
	return pFileList != (const CPtrList *)NULL;
}
	
UINT CFileRegistry::GetFileCount(const CTargetItem * pTargetFilter, const CPtrList * pFileList,
									 BOOL fOnlyBuildable /*= TRUE*/, const ConfigurationRecord * pcr /*= NULL*/)
{
	CObList FileItemList;
	if (GetFileItemList(pTargetFilter, pFileList, FileItemList, fOnlyBuildable, pcr))
		return FileItemList.GetCount();
	else
		return 0;	// couldn't get the list, return none
}

void CFileRegistry::RemoveFromProject(CProjItem * pItem, BOOL fRemoveContained)
{
	// do we have a file reghandle to remove?
	FileRegHandle hndFileReg = pItem->GetFileRegHandle();
	if (hndFileReg != (FileRegHandle)NULL)
	{

	CFileRegEntry * pRegEntry = GetRegEntry(hndFileReg); ASSERT (pRegEntry);

	ASSERT(NULL!=pItem);
	CTargetItem* pTarget = pItem->GetTarget();

	if (!pTarget->IsFileInTarget(pRegEntry))
	{
		ASSERT(0); // bogus!
		return;	// not in the project!
	}

	// interested in this item?
	if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{
//#ifndef REFCOUNT_WORK
		pRegEntry->DisassociateFileItem((CFileItem *)pItem);
//#endif

		// remove from a list of files we are interested in?
		CPtrList * pFileList;
		if (GetFileList(pRegEntry->GetFilePath(), (const CPtrList * &)pFileList))
		{
			POSITION pos = pFileList->Find(pRegEntry);
			if (pos != (POSITION)NULL)
			{
				pFileList->RemoveAt(pos);

				// If we deleted an RC file then we may need to send out a notification
				if (pFileList == GetRCFileList())
				{
					// Only send notifications when we are not initialising or destroying the project
					CProject * pProject = pItem->GetProject(); ASSERT(pProject != (CProject *)NULL);
 					if (pProject->m_bProjectComplete)
					{
						// Construct the NOTIFYINFO packet of data regarding the deletion of
						// this rc file
						NOTIFYINFO Notify;
						const CPath * pPath = ((CFileRegFile *)pRegEntry)->GetFilePath();
						COleRef<IPkgProject> pPkgProj = NULL;
						VERIFY(SUCCEEDED(g_BldSysIFace.GetPkgProject((HBUILDER)pItem->GetProject(), &pPkgProj)));
						Notify.pPkgProject = pPkgProj;

						CTargetItem * pTarget = pItem->GetTarget();
						ASSERT(pTarget->IsKindOf(RUNTIME_CLASS(CTargetItem)));
						Notify.hFileSet = (HFILESET)pTarget;

						Notify.lstFiles.AddTail((CPath *)pPath);

						// Send the notification
						theApp.NotifyPackages(PN_DEL_RCFILE, (void *)&Notify);

						// Clean up the data
						Notify.lstFiles.RemoveAll();
					}
				}
			}
		}
 	}

//	pRegEntry->ReleaseProjRef();

	if (this != &g_FileRegistry)
	{
		// We are a target registry so we'd better remove our reference
		// to this file. NOTE: when we have v2 mak conversion to proper
		// v3 maks  i.e. containing target item nodes then we will always
		// do this.
#ifndef REFCOUNT_WORK
		RemoveFile(pRegEntry->GetFilePath());
#else
		ReleaseRegRef(pRegEntry);
#endif
	}
	}

	// remove all our children?
	if (fRemoveContained && pItem->GetContentList())
	{
		POSITION pos = pItem->GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CProjItem * pContainedItem = (CProjItem *)pItem->GetNext(pos);
			pContainedItem->GetRegistry()->RemoveFromProject(pContainedItem, fRemoveContained);
		}
	}
}
	
void CFileRegistry::RestoreToProject(CProjItem * pItem, BOOL fRestoreContained)
{
	// do we have a file reghandle to restore?
	FileRegHandle hndFileReg = pItem->GetFileRegHandle();
	if (hndFileReg != (FileRegHandle)NULL)
	{

	CFileRegEntry *pRegEntry = GetRegEntry(hndFileReg); ASSERT(pRegEntry);

	ASSERT(NULL!=pItem);
	CProject* pProject = pItem->GetProject();
	ASSERT(NULL!=pProject);

	if (this != &g_FileRegistry)
	{
		// For target registries we must effectively register the file too
		// NOTE : When we have conversion support for v2 maks to v3 maks.
		// i.e. containing proper v3 target items then we will always do
		// this
#ifndef REFCOUNT_WORK
 		RegisterFile(pRegEntry->GetFilePath());
#else
		AddRegRef(hndFileReg);
#endif
	}

	// interested in this item?
	if (pItem->IsKindOf(RUNTIME_CLASS(CFileItem)))
	{
//#ifndef REFCOUNT_WORK
		pRegEntry->AssociateFileItem((CFileItem *)pItem);
//#endif
		CProject * pProject = pItem->GetProject(); ASSERT(pProject != (CProject *)NULL);

		// add to a list of files we are interested in?
		CPtrList * pFileList;
		if ((GetFileList(pRegEntry->GetFilePath(), (const CPtrList * &)pFileList)) && (g_BldSysIFace.IsScanableFile((HBLDFILE)pItem)))
		{
			// don't allow multiple .RC or .DEF files, exclude this file if
			// there are already these files in the project
			if (pFileList == GetRCFileList() || pFileList == GetDEFFileList())
			{
				// is the associated project initializing from a project file?
				if (pProject->m_bPrivateDataInitialized)
				{
					// no
					BOOL fShowWarning = FALSE;
					
					// check this addition for all project configurations
					// (examine item included in build property)
					int iSize = pProject->GetPropBagCount();
					const CPtrArray * pArrayCfg = pProject->GetConfigArray();
					for (int i = 0; i < iSize; i++)
					{
						ConfigurationRecord * pcr = (ConfigurationRecord *)(*pArrayCfg)[i]; 
						CProjTempConfigChange projTempConfigChange(pProject);
						projTempConfigChange.ChangeConfig(pcr);

						CProjType * pProjType = pProject->GetProjType();
						BOOL bIsExternalTarget = pProjType && pProjType->IsKindOf(RUNTIME_CLASS(CProjTypeExternalTarget));

						if (!bIsExternalTarget)
						{
							int nBuildable = GetFileCount(pItem->GetTarget(), pFileList, TRUE, pcr);
							if (nBuildable >= 1) // buildable one already exists for this config.
							{
								// don't do warning if they are *all* excluded
								int fExcluded;
								pItem->GetIntProp(P_ItemExcludedFromBuild, fExcluded);
								if (!fExcluded)
								{
#if 1	// Begin hack
									//
									// HACK alert!
									// Since setting P_ItemExcludedFromBuild to TRUE will cause
									// a ReleaseRegRef() to occur, do an AddRegRef on
									// the build registry if P_ItemExcludedFromBuild doesn't
									// exist yet.
									// 
									// This is needed because CProjItem::GetIntProp has a hack to return
									// P_ItemExcludedFromBuild as TRUE when the proprety doesn't exist.  This
									// causes AssignActions to do an AddRefRef that needs to be undone.
									//
									CPropBag* pBag = pItem->GetPropBag();
									if (NULL!=pBag && NULL==pBag->FindProp(P_ItemExcludedFromBuild))
									{
										ConfigurationRecord* pcr = pItem->GetActiveConfig();
										ASSERT( NULL!=pcr);
										CFileRegistry *preg = g_buildengine.GetRegistry(pcr);
										ASSERT(NULL!=preg);
										FileRegHandle frh = pItem->GetFileRegHandle();
										ASSERT(NULL!=frh);

										preg->AddRegRef(frh);
									}
#endif
									pItem->SetIntProp(P_ItemExcludedFromBuild, TRUE);
									
									// Show warning only for the 2nd addition after
									// the buildable one
									if (GetFileCount(pItem->GetTarget(), pFileList, FALSE, pcr) == 1)
										fShowWarning = TRUE;
								}
							}
						}

  					}

					// Show the warning message if necessary
					if ((fShowWarning) && (!g_bExcludedNoWarning))
						InformationBox(IDS_FILE_TYPE_EXISTS, pFileList == GetRCFileList() ? _TEXT(".rc") : _TEXT(".def"));
				}
			}

			pFileList->AddTail(pRegEntry);

			// If we added an rc file then we may need to send out a notification
			if (pFileList == GetRCFileList() && !g_fCreatingProject && pProject->m_bProjectComplete)
			{
				// Get the state of the workspace
				LPPROJECTWORKSPACE pProjSysIFace = FindProjWksIFace();
				BOOL bWorkspaceInitialised = pProjSysIFace->IsWorkspaceInitialised() == S_OK;

				// Only send notifications after PN_WORKSPACE_INIT happened
 				if (bWorkspaceInitialised)
				{
					// Construct the NOTIFYINFO packet of data regarding the addition of
					// this rc file
					NOTIFYINFO Notify;
					const CPath * pPath = ((CFileRegFile *)pRegEntry)->GetFilePath();
					
					CTargetItem * pTarget = pItem->GetTarget();
					ASSERT(pTarget->IsKindOf(RUNTIME_CLASS(CTargetItem)));

					COleRef<IPkgProject> pPkgProj = NULL;
					VERIFY(SUCCEEDED(g_BldSysIFace.GetPkgProject((HBUILDER)pItem->GetProject(), &pPkgProj)));
					Notify.pPkgProject = pPkgProj;
					Notify.hFileSet = (HFILESET)pTarget;
					Notify.lstFiles.AddTail((CPath *)pPath);

					// Send the notification
					theApp.NotifyPackages(PN_ADD_RCFILE, (void *)&Notify);
					
					// Clean up the data
					Notify.lstFiles.RemoveAll();
				}
			}
		}
	}

//	pRegEntry->AddProjRef();

	}

	// restore all our children?
	if (fRestoreContained && pItem->GetContentList())
	{
		POSITION pos = pItem->GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			CProjItem * pContainedItem = (CProjItem *)pItem->GetNext(pos);
			pContainedItem->GetRegistry()->RestoreToProject(pContainedItem, fRestoreContained);
		}
	}
}

#ifdef _DEBUG
void CFileRegistry::Dump(CDumpContext& dc) const
{
	CFileRegSet::Dump(dc);
#ifdef REGISTRY_FILEMAP

	dc << "{CFileRegistry: m_FileMap";
	CString str;

	POSITION pos;
	FileRegHandle hndFileReg;
	for (pos = m_FileMap.GetStartPosition(); pos != NULL; )
	{
		m_FileMap.GetNextAssoc(pos, str, (void*&)hndFileReg);
		dc << "\n@" << (void*)hndFileReg << " " << str;
	}
#endif
	dc << "\nend of CFileRegistry}";
}

void CFileRegistry::AssertValid() const
{
	if ( m_bAssertValidCalled )
		return;


	CObject::AssertValid();
	// Expect that this registry is g_FileRegistry or a local registry of g_FileRegistry
#ifndef REFCOUNT_WORK
	ASSERT( this == &g_FileRegistry || IsLocalRegistryOf( &g_FileRegistry ) );
#endif
}

#ifndef REFCOUNT_WORK
BOOL CFileRegistry::IsLocalRegistryOf( CFileRegistry* pParent) const
{
	// Included directly in parent?
	if ( NULL != pParent->m_LocalFileRegs.Find( (void*)this ) )
		return TRUE;
	else
	{
		// Included indirectly?

		POSITION pos = pParent->m_LocalFileRegs.GetHeadPosition();
		while (pos != NULL)
		{
			CFileRegistry* pfreg = (CFileRegistry*) pParent->m_LocalFileRegs.GetNext(pos);
			if ( IsLocalRegistryOf( pfreg ) )
				return TRUE;
		}
		return FALSE;
	}
}
#endif //ifndef REFCOUNT_WORK

#endif

#ifdef REFCOUNT_WORK
//
// New CFileRegistry functions.
//

BOOL CFileRegistry::AddRegistryHandle(FileRegHandle hndFileReg)
{
	ASSERT(hndFileReg);

	// Shouldn't be adding again.
	ASSERT( !RegHandleExists(hndFileReg) );
	ASSERT( !hndFileReg->IsNodeType(nodetypeRegRegistry) );
	ASSERT( !hndFileReg->IsNodeType(nodetypeRegFilter) );

	// Already exists in set....return
	if (RegHandleExists(hndFileReg))
		return TRUE;

	// Reference held by this file set.
	hndFileReg->AddFRHRef();

	// This may result in new dependency update requests, so we must not be locking
	// the lists at this time.
	ASSERT(!g_sectionDepUpdateQ.FOwned());

	// Is the CFileRegEntry a CFileRegSet if so then we must
	// set up the associated file registry...
	CFileRegSet * pFileRegSet = (CFileRegSet *)hndFileReg;

	if (pFileRegSet->IsNodeType(nodetypeRegSet))
	{
		pFileRegSet->SetFileRegistry(this);

		// We need to receive notifications from this fileset.
		// (Already added to nofify list in SetFileRegistry).
		// pFileRegSet->AddNotifyRx(this);
	}

	// Add the CFileRegEntry to the registry.
	m_Contents.AddTail(hndFileReg);

	//
	// Add files to file map.
	//
	if (pFileRegSet->IsNodeType(nodetypeRegFile))
	{
#ifdef REGISTRY_FILEMAP
		TCHAR szKey[MAX_PATH];
		_tcscpy(szKey, *hndFileReg->GetFilePath());
		_tcsupr(szKey);

		m_FileMap.SetAt(szKey, hndFileReg);
#endif

		// Filter file.
		FilterFile(hndFileReg);
	}

	InformNotifyRxs(FRI_ADD, (DWORD)hndFileReg);
 	
	return TRUE;
}

BOOL CFileRegistry::RemoveRegistryHandle(FileRegHandle hndFileReg)
{
	// If the item being destroyed is still in us then remove it.
	POSITION pos = m_Contents.Find(hndFileReg);
	ASSERT(NULL!=pos);
	if (NULL==pos)
		return FALSE;


	//
	// Notify dependents this handle is being destroyed.
	//
	InformNotifyRxs(FRI_DESTROY, (DWORD)hndFileReg);

	// Is the CFileRegEntry a CFileRegSet if so then we must
	// set up the associated file registry...
	CFileRegSet * pFileRegSet = (CFileRegSet *)hndFileReg;

	if (pFileRegSet->IsNodeType(nodetypeRegSet))
	{
		// Remove from notification list.
//		pFileRegSet->RemoveNotifyRx(this);

		pFileRegSet->SetFileRegistry(NULL);
	}

	m_Contents.RemoveAt(pos);

	if (pFileRegSet->IsNodeType(nodetypeRegFile))
	{
#ifdef REGISTRY_FILEMAP
		TCHAR szKey[MAX_PATH];
		_tcscpy(szKey, *hndFileReg->GetFilePath());
		_tcsupr(szKey);
		VERIFY(m_FileMap.RemoveKey(szKey));
#endif

		// Remove from filters.
		UnFilterFile(hndFileReg);
	}

	// Release reference held by this file set.
	hndFileReg->ReleaseFRHRef();

	return TRUE;
}


#endif //#ifdef REFCOUNT_WORK


#ifdef _DEBUG

// DumpHeap() function, for use from the debugger.

// TrackFreeblocks: call this to have the CRT start saving freed blocks instead of actually
//		releasing them.  This is for measurement of heap churn.
extern void __cdecl TrackFreeblocks()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) |
				   _CRTDBG_DELAY_FREE_MEM_DF);
}
extern BOOL __cdecl DumpHeap()	// for use from debugger
{
	CWaitCursor wc;
	CMemStat stat;
	stat.ReportInExcel();
	return TRUE;
}

#endif

///////////////////////////////////////////////////////////////////////////////
//
//	Instantiate the file registry

CFileRegistry AFX_DATA_EXPORT g_FileRegistry;

// Helper function: Takes a list of FileRegHandles and make a CString out
// of them:

static CString strCachedOutDirValue;

void ClearMQSCache()
{
	strCachedOutDirValue = _T("<empty>");
}

void MakeQuotedString
( 
	CFileRegSet * pregset,
	CString & strOut,
	const CDir * pBaseDir /* = NULL*/, 
	BOOL bQuote /*= TRUE*/,
	BOOL (*pfn) (DWORD, FileRegHandle) /*= NULL*/,
	DWORD dwFuncCookie /*= NULL*/,
	const TCHAR *pSeparator /*= NULL*/,
	BOOL bOneOnly /* = FALSE */,
	const CObList *plItems, /* = NULL */
	BOOL bSort /* = FALSE */
)
{
	CFRFPtrArray * pregarray = new CFRFPtrArray;
#ifndef REFCOUNT_WORK
	pregarray->SetSize( pregset->GetContent()->GetCount() );
#else
	pregarray->SetSize( pregset->GetCount() );
#endif

	// init. the fileset enumeration
	pregset->InitFrhEnum();

	FileRegHandle frh;
	int index = 0;
	while ((frh = pregset->NextFrh()) != (FileRegHandle)NULL)
	{
		pregarray->SetAt( index, frh );
		index++;
	}

#ifndef REFCOUNT_WORK
	ASSERT( pregarray->GetSize() == pregset->GetContent()->GetCount() );
#else
	ASSERT( pregarray->GetSize() == pregset->GetCount() );
#endif

	MakeQuotedString( pregarray, strOut, pBaseDir, bQuote, pfn, dwFuncCookie, 
		pSeparator, bOneOnly, plItems, bSort );

#ifdef REFCOUNT_WORK
	for (index=0; index < pregarray->GetSize(); index++)
		(*pregarray)[index]->ReleaseFRHRef();
#endif

	delete pregarray;
}

int __cdecl CmpCStrings(const void * pstr1, const void * pstr2)
{
	return ((CString *)pstr1)->CompareNoCase((LPCSTR)*(CString *)pstr2);
}

void MakeQuotedString
( 
	CFRFPtrArray * pregarray,
	CString & strOutput,
	const CDir * pBaseDir /* = NULL*/, 
	BOOL bQuote /*= TRUE*/,
	BOOL (*pfn) (DWORD, FileRegHandle) /*= NULL*/,
	DWORD dwFuncCookie /*= NULL*/,
	const TCHAR *pSeparator /*= NULL*/,
	BOOL bOneOnly /* = FALSE */,
	const CObList *plItems, /* = NULL */
	BOOL bSort /* = FALSE */
)
{
	// increase size of output buffer
	// (note, it is not necessarily empty to begin with)
	int len = strOutput.GetLength();
	strOutput.GetBuffer(2048);
	strOutput.ReleaseBuffer(len);

	// default seperator?
	if (pSeparator == (TCHAR *)NULL)
		pSeparator = _TEXT(" ");

	CString str;
	CString strOutDirMacro, strOutDirValue;
	static CDir CachedOutDir;
	static CDir CachedBaseDir;
	CDir * pOutDir = (CDir *)NULL;
	CProjItem * pItem;
	CProjItem * pItemOld;
	CProjItem * pHeadItem = NULL;
	ConfigurationRecord * pcrToMatch = (ConfigurationRecord *)NULL;

	POSITION pos2 = plItems != (const CObList *)NULL ? plItems->GetHeadPosition() : (POSITION)NULL;

	// we must match the config. of the item at the head for the rest of the items
	if (pos2 != (POSITION)NULL)
	{
		if (plItems->GetCount() > 1)
			pHeadItem = ((CProjItem *)plItems->GetNext(pos2));
		else
			pHeadItem = ((CProjItem *)plItems->GetHead());

		pcrToMatch = (ConfigurationRecord *)pHeadItem->GetActiveConfig();
	}

	// copy of formatted strings to be sorted
	CStringArray strOutArray;
	int nOutSize = pregarray->GetSize();
	strOutArray.SetSize(nOutSize);
	CString strOut;
	int index = 0;
	int nOutDirType;
	UINT nID;
	BOOL bOutDir;
	
	for ( int ifrh = 0; ifrh < pregarray->GetSize(); ifrh++ )
	{
		FileRegHandle frh = (*pregarray)[ifrh];
		if (pfn != NULL && !(*pfn)(dwFuncCookie, frh))
		{
			nOutSize--;
			continue; 
		}

		CFileRegFile * preg = (CFileRegFile *)g_FileRegistry.GetRegEntry(frh);
		CPath * pPath = (CPath *)preg->GetFilePath();
		ASSERT_VALID(pPath);

		// when at the end of the item list, keep using the last known
		if (pos2 != (POSITION)NULL)
		{
			ASSERT(pcrToMatch);
			pOutDir = (CDir *)NULL;
			bOutDir = FALSE;
			pItem = (CProjItem *)plItems->GetNext(pos2);

			if (pItem != NULL) // no outdir if pItem is NULL
			{
				pItem->SetManualBagSearchConfig(pcrToMatch);
				
		 		bOutDir = (pItem->GetOutDirString(strOutDirMacro, &strOutDirValue));
				pItem->ResetManualBagSearchConfig();
			}
			else if ((nOutDirType = preg->GetOutDirType()) > 0)
			{
				ASSERT(pHeadItem != NULL);
				ASSERT(nOutDirType <= 2);
				nID = (nOutDirType==1) ? P_OutDirs_Target : P_OutDirs_Intermediate;

				pItem = pHeadItem;
				while ((pItem!=NULL) && (!pItem->IsKindOf(RUNTIME_CLASS(CProject))))
				{
					pItemOld = pItem;
					pItem = (CProjItem *)pItem->GetContainerInSameConfig();
					if (pItemOld != pHeadItem)
						pItemOld->ResetContainerConfig();
				}

				if (pItem != NULL)
				{
					bOutDir = pItem->GetOutDirString(strOutDirMacro, nID, &strOutDirValue, TRUE, FALSE);
				}
			}

			if (bOutDir && (pBaseDir!=NULL))
			{
				// FUTURE (karlsi): slightly broken on LHS when (pItem && !bOneOnly)
				// olympus 1797 [patbr]: don't create OutDir from relative path because
				// the current working directory may be used, which means that the GetRelativeName()
				// call below will return a non-relative path (i.e., without ".\" at start) which
				// means that all the $(OUTDIR) and $(INTDIR) macros disappear from the MAK file.
				if ((strOutDirValue == strCachedOutDirValue) && (*pBaseDir==CachedBaseDir))
				{
					pOutDir = &CachedOutDir;
				}
				else // must base $(OUTDIR) on project dir
				{
					ASSERT(pBaseDir!=NULL);
					CPath tempPath;
					CString strTemp = strOutDirValue + _T("\\a");
					if ((tempPath.CreateFromDirAndRelative(*pBaseDir, strTemp)) && (CachedOutDir.CreateFromPath(tempPath)))
					{
						strCachedOutDirValue = strOutDirValue;
						CachedBaseDir = *pBaseDir;
						pOutDir = &CachedOutDir;
					}
				}
			}
		}

		strOut.GetBuffer(63);
		strOut.ReleaseBuffer(0);

		int nPathStart = 0;
		if (pOutDir &&
			(pPath->GetRelativeName(*pOutDir, str, FALSE, TRUE)) &&
			(str[nPathStart++] == _T('.') && (str[nPathStart] == _T('\\'))))
		{
#if 0	// REVIEW: should no longer be necessary for .objs/.sbrs or anything
			// replace '.\' at head of relative path with './' to fix NMAKE bug
			str.SetAt(1, _T('/')); // REVIEW: is this fixed yet in NMAKE???
#endif

			// quote?
			if (bQuote) strOut += _T('"');

			strOut += strOutDirMacro;
			strOut += ((const TCHAR *)str + 1);	// skip '.'

			// quote?
			if (bQuote) strOut += _T('"');
		}
		else if (pBaseDir)
		{
			// always get a relative path
			BOOL fOldAlwaysRelative = pPath->GetAlwaysRelative();
			pPath->SetAlwaysRelative();
			pPath->GetRelativeName(*pBaseDir, str);
			pPath->SetAlwaysRelative(fOldAlwaysRelative);
			CDir dir;
			dir.CreateFromPath(*pPath);

			// only the include files will go through the ifs, because none of the other cases
			// will set the condition we test for.
			if ( dir == *pBaseDir )
			{
				// quote?
				if (bQuote) strOut += _T('"');

				strOut += str;

				// quote?
				if (bQuote)	strOut += _T('"');
			}	
			else if (preg->IsFileFoundInIncPath() && (g_nSharability != SHARE_LOW) )
			{
				strOut += _T("{$(INCLUDE)}");

				// quote?
				if (bQuote)	strOut += _T('"');

				// strOut += _T('\\');

				// need to exclude the path from the name
				int ich = preg->GetIndexNameRelative();
				if (ich)
					strOut += str.Right(ich);
				else
					// need original name 
					strOut += preg->GetOrgName(); 

				// quote?
				if (bQuote)	strOut += _T('"');
			}
			else if ( g_nSharability == SHARE_HIGH )
			{
				strOut += _T("{$(INCLUDE2)}");

				// quote?
				if (bQuote)	strOut += _T('"');

				strOut += _T('\\');

				int ich = preg->GetIndexNameRelative();
				if (ich)
					strOut += str.Right(ich);
				else
					strOut += pPath->GetFileName();

				// quote?
				if (bQuote)	strOut += _T('"');
			}
			else
			{
				// quote?
				if (bQuote)	strOut += _T('"');

				strOut += str;

				// quote?
				if (bQuote)	strOut += _T('"');
			}
		}
		else
		{
			// Quote really means quote if there are special charactes
			// involved:
			if (bQuote && pPath->ContainsSpecialCharacters())
			{ 
				strOut += _T('"');
				strOut += (const TCHAR *) *pPath;
				strOut += _T('"');
			}
			else
			{
				strOut += (const TCHAR *) *pPath;
			}
		}

		strOutArray.SetAt(index++, strOut);

		if (bOneOnly)	// stop if we only want the first valid item
			break;
	}

	nOutSize = index; // update size

	// sort if required
	if ((bSort) && (nOutSize > 1))
	{
		qsort(strOutArray.GetData(), nOutSize, sizeof(CString *), CmpCStrings);
	}

	// do final formatting here into strOutput
	for ( index = 0; index < nOutSize; index++)
	{
		if (index!=0)
			strOutput += pSeparator;

		strOutput += strOutArray[index];
	}
	strOutArray.RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////
//
//	Special collection class for dependency lists.

void CSmallPtrSet::RemoveItem(void *pItem)
{
	for (int i = 0; i < m_nSize; i++)
	{
		if (m_pData[i] == pItem)
		{
			RemoveAt(i);
			break;		// only once
		}
	}
}

void* CSmallPtrSet::GetNext(POSITION& rPosition) const
{
	ASSERT(rPosition != NULL && int(rPosition) <= m_nSize);

	void *pItem = m_pData[int(rPosition) - 1];

	rPosition = POSITION(int(rPosition) + 1);
	if (int(rPosition) > m_nSize)
		rPosition = NULL;

	return pItem;
}

POSITION CSmallPtrSet::Find(void const * const pItem)
{
	for (int i = 0; i < m_nSize; i++)
	{
		if (m_pData[i] == pItem)
		{
			return POSITION(i + 1);
		}
	}
	return NULL;
}

//
// Slow GetFileAttributesEx for operating systems that do not support it.
//

extern BOOL (WINAPI *g_pGetFileAttributesEx_DevBld)( LPCTSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
									 LPVOID lpFileInformation);
static BOOL WINAPI Choose_GetFileAttributesEx( LPCTSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
									 LPVOID lpFileInformation);
static BOOL WINAPI Slow_GetFileAttributesEx( LPCTSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
									 LPVOID lpFileInformation);


BOOL (WINAPI *g_pGetFileAttributesEx_DevBld)( LPCTSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
									 LPVOID lpFileInformation) = Choose_GetFileAttributesEx;

static BOOL WINAPI Choose_GetFileAttributesEx( LPCTSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
									 LPVOID lpFileInformation)
{
	//
	// Block any other threads using mutex.  This is normally a one shot deal.
	//
	TCHAR szMutexName[50];

	//
	// Create a mutex name that is unique each process.
	//
	_stprintf( szMutexName, _T("Choose_GetFileAttributesEx:devbld:%lX"), GetCurrentProcessId() );

	HANDLE hMutex = CreateMutex( NULL, FALSE, szMutexName );
	if (NULL==hMutex)
	{
		ASSERT(0);
		if (&Choose_GetFileAttributesEx==g_pGetFileAttributesEx_DevBld)
		{
			g_pGetFileAttributesEx_DevBld = &Slow_GetFileAttributesEx;
			TRACE("Slow_GetFileAttributesEx was selected by Choose_GetFileAttributesEx because of failure\n");
		}
	}
	else
	{
		// Time out after 10 seconds. 
		DWORD dwWait = WaitForSingleObject(hMutex, 10000L);

		switch(dwWait)
		{
		case WAIT_OBJECT_0:
			if (&Choose_GetFileAttributesEx==g_pGetFileAttributesEx_DevBld)
			{
				HINSTANCE hLib = GetModuleHandle( _T("kernel32.dll") );
				if (NULL!=hLib)
				{
#ifdef _UNICODE
					FARPROC lpProc = GetProcAddress(hLib, "GetFileAttributesExW" );
#else
					FARPROC lpProc = GetProcAddress(hLib, "GetFileAttributesExA" );
#endif
					if (NULL!=lpProc)
					{
						g_pGetFileAttributesEx_DevBld = 
							(BOOL (WINAPI *)( LPCTSTR, GET_FILEEX_INFO_LEVELS,LPVOID))lpProc;
						TRACE("GetFileAttributesEx was selected by Choose_GetFileAttributesEx\n");
					}
					else
					{
						g_pGetFileAttributesEx_DevBld = &Slow_GetFileAttributesEx;
						TRACE("Slow_GetFileAttributesEx was selected by Choose_GetFileAttributesEx\n");
					}
				}

			}
			VERIFY(ReleaseMutex(hMutex));
			break;

		case WAIT_TIMEOUT:
		case WAIT_ABANDONED:
		default:
			//
			// Use Slow_GetFileAttributesEx when checking for GetFileAttributesEx fails.
			//
			ASSERT(0);
			if (&Choose_GetFileAttributesEx==g_pGetFileAttributesEx_DevBld)
			{
				g_pGetFileAttributesEx_DevBld = &Slow_GetFileAttributesEx;
			}
			TRACE("Slow_GetFileAttributesEx was selected by Choose_GetFileAttributesEx because of failure\n");
			break;
		}
		CloseHandle(hMutex);
	}
	return (*g_pGetFileAttributesEx_DevBld)( lpFileName, fInfoLevelId, lpFileInformation);
}

//
// Slow_GetFileAttributesEx is used when GetFileAttributesEx is unsupported.
//
static BOOL WINAPI Slow_GetFileAttributesEx( LPCTSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
									 LPVOID lpFileInformation)
{
	WIN32_FILE_ATTRIBUTE_DATA*  pFileData = (WIN32_FILE_ATTRIBUTE_DATA*)lpFileInformation;

	ASSERT(GetFileExInfoStandard==fInfoLevelId);

	WIN32_FIND_DATA find;
	HANDLE hfind = ::FindFirstFile(lpFileName, &find);

	if (hfind != INVALID_HANDLE_VALUE)
	{
		::FindClose(hfind);

		pFileData->dwFileAttributes = find.dwFileAttributes;
		pFileData->ftCreationTime = find.ftCreationTime; 
		pFileData->ftLastAccessTime = find.ftLastAccessTime; 
		pFileData->ftLastWriteTime = find.ftLastWriteTime; 
		pFileData->nFileSizeHigh = find.nFileSizeHigh; 
		pFileData->nFileSizeLow = find.nFileSizeLow; 

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
