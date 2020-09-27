#include "stdafx.h"
#include <sys\utime.h>

#include "FileRegistry.h"
#include "BuildEngine.h"
#include "VCProjectEngine.h"

CTestableSection g_sectionRegistryFull;
CTestableSection g_sectionRegistrySingle;
CTestableSection g_sectionRegistryFilter;
CTestableSection g_sectionFileDelete;

///////////////////////////////////////////////////////////////////////////////
//
//	CBldFileRegEntry

// file reg. entry local heap
static const int s_nRegSetRefCount = 1000;

// this variable helps us to only update time stamps on an as-needed basis
static DWORD g_dwUpdateTickCount = 0;

CTestableSection g_sectionDepUpdateQ;

CTestableSection CBldFileRegFile::g_sectionFileMap;

#ifdef _DEBUG

// AddFRHRef and ReleaseFRHRef are defined inline in pfilereg.h for non-debug builds.
void CBldFileRegEntry::AddFRHRef()
{
	// Prevent access to registry by other threads.
	CritSectionT cs(g_sectionRegistrySingle);
	m_nRefCount++;
}

void CBldFileRegEntry::ReleaseFRHRef()
{
	// Prevent access to registry by other threads.
	CritSectionT cs(g_sectionRegistrySingle);

	// Keep information around for CBldFileRegFile
	VSASSERT(0!=m_nRefCount, "Trying to release a reg handle too many times");
	if (0 == m_nRefCount)
		return;

	m_nRefCount--;
	
	if (m_nRefCount == 0)
		SafeDelete();
}
#endif // _DEBUG

CBldFileRegEntry::CBldFileRegEntry()
{
	m_dwNodeType = nodetypeRegEntry | nodetypeRegNotifyRx;
	
	// Initialize the basic file registry entry properties
	m_nExistsOnDisk = FALSE;

	// Initialize the non informing properties
	m_nRefCount = 0;

	m_bMatchCase = FALSE;	
	m_bWantActualCase = FALSE;	
	m_bUseIntDir = FALSE;
	m_bUseOutDir = FALSE;

	// Initialize the update atrributes and dirty bit
	m_bDirty = FALSE;
	m_bUpdateAttribs = TRUE;

	m_bFoundInPath = FALSE;     
	m_bRelativeToProjName = FALSE;     
}

CBldFileRegEntry::~CBldFileRegEntry()
{
	VSASSERT(0==m_nRefCount, "Failed to completely release a reg handle");

}

void CBldFileRegEntry::OnNotify(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC, UINT idChange, void* pHint)
{
	VSASSERT(FALSE, "Base class method OnNotify must be overridden");
}

void CBldFileRegEntry::InformNotifyRxs(UINT idChange, IVCBuildErrorContext* pEC, void* pHint /* = NULL */)
{
	INT_PTR nMax = m_Dependants.GetSize();
	for (INT_PTR idx = 0; idx < nMax; idx++)
	{
		CBldFileRegNotifyRx * pfrerx = (CBldFileRegNotifyRx*)m_Dependants.GetAt(idx);
		if (pfrerx)
			pfrerx->OnNotify(this, pEC, idChange, pHint);
	}
}

void CBldFileRegEntry::AddNotifyRx(CBldFileRegNotifyRx* pfre, BOOL bCheckExistence)
{
	// Add a notification receiver to our dependency list
	VSASSERT(pfre != this, "Trying to add ourself to the list of dependents on us.  Bad recursion about to ensue when we do an OnNotify...");
	int idx = -1;
	if (bCheckExistence)
		idx = m_Dependants.Find(pfre);
	if (idx < 0)
		m_Dependants.AddTail(pfre);

	VSASSERT(idx < 0, "Trying to add the same dependent twice");
}

void CBldFileRegEntry::RemoveNotifyRx(CBldFileRegNotifyRx* pfre)
{
	// Remove a notification receiver from our dependency list
	m_Dependants.RemoveItem(pfre);
}

BOOL CBldFileRegEntry::SetIntProp(UINT idProp, int nVal)
{
	// set an integer property value
	VSASSERT(idProp == P_ExistsOnDisk, "Trying to set an int prop we don't have");	// We only have one bool property

	// Prevent access to registry by other threads.
	CritSectionT cs(g_sectionRegistrySingle);

	int nOldVal = m_nExistsOnDisk;

	// any change?
	if (nOldVal != nVal)
		m_nExistsOnDisk = nVal;

	return TRUE;
}

BOOL CBldFileRegEntry::GetIntProp(UINT idProp, int& nVal, BOOL bUpdate)
{
	// Get an integer property value
	if (bUpdate)
		(void)UpdateAttributes();
 	
	VSASSERT(idProp == P_ExistsOnDisk, "Trying to get info about an int prop we don't have");
	nVal = m_nExistsOnDisk;
	return TRUE;
}

BOOL CBldFileRegEntry::UpdateAttributes()
{
	m_bUpdateAttribs = FALSE;
	BOOL fRefreshed = RefreshAttributes();
	m_bUpdateAttribs = TRUE;

	return fRefreshed;
}

///////////////////////////////////////////////////////////////////////////////
//
//	CBldFileRegSet

CBldFileRegSet::CBldFileRegSet(int nHashSize)
	: CBldFileRegEntry(), m_Contents(nContentsBlockSize), m_bSkipUpdate(FALSE)
{
	m_dwNodeType = nodetypeRegEntry | nodetypeRegSet | nodetypeRegNotifyRx;

	m_pmapContents = NULL;			// no map yet
	m_nHashSizeMapContents = (USHORT)nHashSize;
	m_nExistsOnDisk = TRUE;
	m_posEnum = NULL;

	// Newest date
	m_NewTime.dwLowDateTime = m_NewTime.dwHighDateTime = 0;	// not using SetTimeProp 'cause why send out notifications about this?

	// If set is empty then it exists...
	SetIntProp(P_ExistsOnDisk, TRUE);
 
	// Initialize private cache helping members
	m_nNotExists = 0;
	m_fInitSet = FALSE;

	// Calculate contents attribute changes (off by default)
	m_fCalcAttrib = FALSE;

	m_pFileRegistry = NULL;

	// Set at s_nRegSetRefCount so it will never be deleted accidently.
	m_nRefCount = s_nRegSetRefCount;
}

CBldFileRegSet::~CBldFileRegSet()
{
	m_bUpdateAttribs = FALSE;	// don't attempt to update
 	m_fCalcAttrib = FALSE;		// don't re-calc attrib.

	VCPOSITION pos = m_Contents.GetHeadPosition();
	VCPOSITION lastPos;
	while (pos != NULL)
	{
		// WARNING: this code accessses the m_pFileRegistry member.  If the
		// current object is itself a CBldFileRegistry then this pointer is a
		// self-pointer, and we go into CBldFileRegistry methods after the
		// CBldFileRegistry destructor has been called!  This should be changed.
		lastPos = pos;
		BldFileRegHandle hndFileReg = (BldFileRegHandle)m_Contents.GetNext(pos);
		RemoveRegHandleI(hndFileReg); // calling directly to level I function to avoid n^2 lookup
	}

	if (m_pmapContents != NULL)
		delete m_pmapContents;	// one has been allocated, so we must delete it

	// Unhook from registry.
	if (NULL != m_pFileRegistry)
		SetFileRegistry(NULL);

	VSASSERT(s_nRegSetRefCount == m_nRefCount, "Ref count mismatch on the file registry");
	m_nRefCount = 0;
}

void CBldFileRegSet::InitFrhEnum()
{
	// init. the enumerator
	m_posEnum = m_Contents.GetHeadPosition();
	m_fInitSet = TRUE;
}

BldFileRegHandle CBldFileRegSet::NextFrh()
{
TryAgain:
	// exhausted?
	if (m_posEnum == (VCPOSITION)NULL)
		return (BldFileRegHandle)NULL;

	// is this a set?
	VCPOSITION posEnum = m_posEnum;
	BldFileRegHandle frh = (BldFileRegHandle)m_Contents.GetNext(posEnum);
	if (((CBldFileRegEntry *)frh)->IsNodeType(nodetypeRegSet))
	{
		// init. enumerator of this set?
		if (m_fInitSet)
		{
			// init. then remember that we have
			((CBldFileRegSet *)frh)->InitFrhEnum();
			m_fInitSet = FALSE;
		}

		// set got one?
		if ((frh = ((CBldFileRegSet *)frh)->NextFrh()) == (BldFileRegHandle)NULL)
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
		// advance...
		m_posEnum = posEnum;

		// The NextFrh() caller needs to ReleaseFRHRef.
		frh->AddFRHRef();
	}

	return frh;
}

BOOL CBldFileRegSet::AddRegHandle(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC /* = NULL */,
	BOOL bAddRef /* = TRUE */, BOOL bAddFront /* = FALSE */)
{
	VSASSERT(hndFileReg, "Trying to add NULL reg handle to registry");
	VSASSERT( !hndFileReg->IsNodeType(nodetypeRegRegistry), "Not allowed to add a registry to a registry" );
	VSASSERT( !hndFileReg->IsNodeType(nodetypeRegFilter), "Not allowed to add a filter to a registry" );

	// Already exists in set....return
	if (RegHandleExists(hndFileReg))
		return TRUE;

	// Reference held by this file set.
	hndFileReg->AddFRHRef();

	// This may result in new dependency update requests, so we must not be locking
	// the lists at this time.
	VSASSERT(!g_sectionDepUpdateQ.FOwned(), "Trying to lock the lists while doing dependency updates.  Deadly embrace.");

	// Is the CBldFileRegEntry a CBldFileRegSet if so then we must
	// set up the associated file registry...
	CBldFileRegSet * pFileRegSet = NULL;
	if (m_pFileRegistry == NULL)
		pFileRegSet = (CBldFileRegSet *)(g_FileRegistry.GetRegEntry(hndFileReg));
	else
		pFileRegSet = (CBldFileRegSet *)(m_pFileRegistry->GetRegEntry(hndFileReg));

	BOOL bIsFirstMember = FALSE;
	{
		// Prevent access to registry by other threads.
		CritSectionT cs(g_sectionRegistrySingle);

		if (pFileRegSet->IsNodeType(nodetypeRegSet))
			pFileRegSet->SetFileRegistry(m_pFileRegistry);

		// Add the CBldFileRegEntry to the set
		if (bAddFront)
			m_Contents.AddHead(hndFileReg);
		else
			m_Contents.AddTail(hndFileReg);

		if (m_pmapContents == NULL && m_Contents.GetCount() >= cMapThreshold)
		{
			// We now have enough items to create a content map ... do so and fill it with
			// the items so far.
			m_pmapContents = new CVCMapPtrToWord;
			m_pmapContents->InitHashTable(m_nHashSizeMapContents);
			for (VCPOSITION pos = m_Contents.GetHeadPosition(); pos != NULL; )
			{
				m_pmapContents->SetAt(m_Contents.GetNext(pos), 0);
			}
		}
		else if (m_pmapContents != NULL)
			m_pmapContents->SetAt(hndFileReg, 0);
		else
			bIsFirstMember = m_Contents.GetCount() == 1;
	}

	// Add this set as a dependant of the item just added
	// and inform ourselves of this addition
 	hndFileReg->AddNotifyRx(this, FALSE);
	if (m_fCalcAttrib)
	{
		FILETIME Time;
		FILETIME ChangedTime;

		// we're going to calculate attribs now, don't allow recursion
		m_fCalcAttrib = FALSE;

 		// Maintain our cache value of the number of items that do not exist but are contained in us
		BOOL bExists;
		if (hndFileReg->GetIntProp(P_ExistsOnDisk, bExists))
		{
			// If this new file does not exist on disk then we should not consider the file times it 
			// has set as they don't mean anything!.
			if (bExists)
 			{
				BOOL bOldSkipUpdate = m_bSkipUpdate;
				m_bSkipUpdate = !bIsFirstMember;
				if (!hndFileReg->GetTimeProp(P_NewTStamp, ChangedTime, FALSE) || 
					!GetTimeProp(P_NewTStamp, Time, bIsFirstMember))
					MakeDirty();
				else if (ChangedTime > Time)
					SetTimeProp(P_NewTStamp, ChangedTime);
				m_bSkipUpdate = bOldSkipUpdate;
			}
			else
				m_nNotExists++; 

			BOOL bOldSkipUpdate = m_bSkipUpdate;
			m_bSkipUpdate = TRUE;
 			SetIntProp(P_ExistsOnDisk, m_nNotExists == 0);
			m_bSkipUpdate = bOldSkipUpdate;
		}
		else
			MakeDirty();

		m_fCalcAttrib = TRUE;
	}

	InformNotifyRxs(FRI_ADD, pEC, (void *)hndFileReg);
 	
	return TRUE;
}

BOOL CBldFileRegSet::RemoveRegHandle(BldFileRegHandle hndFileReg, BOOL bReleaseRef /* = TRUE */)
{
	VSASSERT(hndFileReg, "Cannot remove NULL reg handles...");

	// Remove the CBldFileRegEntry from the set
	VCPOSITION pos = m_Contents.Find(hndFileReg);
	if (pos != NULL)
		RemoveRegHandleI(hndFileReg, NULL, bReleaseRef);

	return TRUE;	// only return FALSE in error
}

void CBldFileRegSet::RemoveRegHandleI(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC /* = NULL */, 
	BOOL bReleaseRef /* = TRUE */)
{
	// Remove us as a dependant of the item just removed
	hndFileReg->RemoveNotifyRx(this);

	{
		// Prevent access to registry by other threads.
		CritSectionT cs(g_sectionRegistryFull);

		VCPOSITION pos = m_Contents.Find(hndFileReg);
		VSASSERT(pos != (VCPOSITION)NULL, "Reg handle we're trying to remove isn't in our list.  Probably released too many times elsewhere.");
		if (pos != (VCPOSITION)NULL)
			m_Contents.RemoveAt(pos);
		else
			return; /* FALSE */
	}

	//
	// Notify dependents this handle is being destroyed.
	InformNotifyRxs(FRI_DESTROY, pEC, (void *)hndFileReg);

	{
		// Prevent access to registry by other threads.
		CritSectionT cs(g_sectionRegistryFull);

		// If the item being destroyed is still in us then remove it.
 		if (m_pmapContents != NULL)
			m_pmapContents->RemoveKey(hndFileReg);
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
				if (m_nNotExists)
					m_nNotExists--;
			}
			else
			{
				// If the item being destroyed does not exist on
				// disk then it cannot influence our cached new and
				// old time stamps for the set.
				if (bExists && PROJECT_IS_LOADED())
				{	
					BOOL bOldSkipUpdate = m_bSkipUpdate;
					m_bSkipUpdate = TRUE;
					if (!hndFileReg->GetTimeProp(P_NewTStamp, ChangedTime, FALSE) || !GetTimeProp(P_NewTStamp, Time))
						MakeDirty();
					else
					{
 						// If the item going away is currently our newest time stamped item
						// then we have to find a new newest time stamped item in our set
						if (ChangedTime==Time)
		 					FindTimeAndExists(P_NewTStamp);
					}
					m_bSkipUpdate = bOldSkipUpdate;
				}
			}

			BOOL bOldSkipUpdate = m_bSkipUpdate;
			SetIntProp(P_ExistsOnDisk, m_nNotExists == 0);
			m_bSkipUpdate = bOldSkipUpdate;
		}
		else
 			MakeDirty();

		m_fCalcAttrib = TRUE;
	}

	// Release reference held by this file set.
	hndFileReg->ReleaseFRHRef();

	return; /* TRUE */
}

// This routine cannot be called during project destruction or it will assert.
void CBldFileRegSet::EmptyContent(BOOL bReleaseRef /* = TRUE */)
{
	// Prevent access to registry by other threads.
	CritSectionT cs(g_sectionRegistryFull);

	// Empty our contents
	VCPOSITION pos = m_Contents.GetHeadPosition();
	while (pos != (VCPOSITION)NULL)
	{
		BldFileRegHandle hndFileReg = (BldFileRegHandle)m_Contents.GetNext(pos);
		RemoveRegHandle(hndFileReg);
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

BOOL CBldFileRegSet::CalcAttributes(BOOL fOn /* = TRUE */)
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

BOOL CBldFileRegSet::DoRefresh()
{
	// updating?
	if ((!m_bUpdateAttribs) || (!PROJECT_IS_LOADED()))
		return FALSE;	// no

	// base-class refresh?
	if (CBldFileRegEntry::DoRefresh())
		return TRUE;	// yes

 	VCPOSITION pos = m_Contents.GetHeadPosition();
	while (pos != NULL)
		if (((BldFileRegHandle)m_Contents.GetNext(pos))->DoRefresh())
			return TRUE;	// yes

	return FALSE;	// no
}

BOOL CBldFileRegSet::UpdateAttributes()
{
	if (!PROJECT_IS_LOADED())
		return TRUE;

	BOOL fRefreshed = FALSE;
 	VCPOSITION pos = m_Contents.GetHeadPosition();
	while (pos != (VCPOSITION)NULL)
		if (g_FileRegistry.GetRegEntry((BldFileRegHandle)m_Contents.GetNext(pos))->UpdateAttributes())
			fRefreshed = TRUE;
					 
	return TRUE;	// ok
}

BOOL CBldFileRegSet::RefreshAttributes()
{
	// No longer dirty
	m_bDirty = FALSE;
	
	// Refreshing the attributes on a set means updating, ie. a refresh if dirty, the attributes 
	// of all referenced CBldFileRegEntry items contained, and then doing a sweep to find our 
	// accumulative attributes

	// Find the number that exist
	// Set the newest timestamp prop.
	// This function will ensure that all props. are updated for each of our contents
	FindTimeAndExists(P_NewTStamp, TRUE);

	return TRUE;	// performed refresh
}

void CBldFileRegSet::FindTimeAndExists(UINT idProp, BOOL bFindExists)
{
	BOOL fOldOn = m_fCalcAttrib;
	m_fCalcAttrib = FALSE;

	VSASSERT(idProp == P_NewTStamp, "Trying to get info about a time prop we don't have.");
	if (idProp != P_NewTStamp)	// dunno anything else
		return;

	FILETIME NewTime, CurrentTime;
 	BOOL bExistsOnDisk = FALSE;
	unsigned nNotExists = 0;

	NewTime.dwLowDateTime = NewTime.dwHighDateTime = 0;

	VCPOSITION pos = m_Contents.GetHeadPosition();
	while (pos != NULL)
	{
		BldFileRegHandle hndFileReg = (BldFileRegHandle)m_Contents.GetNext(pos);
		if (hndFileReg->GetIntProp(P_ExistsOnDisk, bExistsOnDisk))
		{
			if (bExistsOnDisk)
			{
				// is this a set?
				if (hndFileReg->IsNodeType(nodetypeRegSet))
				{
					// figure out the newest time
					if (hndFileReg->GetTimeProp(P_NewTStamp, CurrentTime, FALSE) &&
						(MyCompareFileTime(&CurrentTime, &NewTime) > 0))
						NewTime = CurrentTime;
				}
				else
				{
					if (!hndFileReg->GetTimeProp(P_NewTStamp, CurrentTime, FALSE))
						continue;

					// figure out the newest time
					if (MyCompareFileTime(&CurrentTime, &NewTime) > 0)
 						NewTime = CurrentTime;
				}
			}
			else
				nNotExists++;
		}
		else
			MakeDirty();
 	}

	m_fCalcAttrib = fOldOn;

	// set the newest time
	SetTimeProp(P_NewTStamp, NewTime);

	//
	// Update "exists" attributes.
	if (bFindExists)
	{
		m_nNotExists = (USHORT)nNotExists;
		SetIntProp(P_ExistsOnDisk, m_nNotExists == 0);
	}
}

void CBldFileRegSet::FindExists()
{
	BOOL fOldOn = m_fCalcAttrib;
	m_fCalcAttrib = FALSE;

	unsigned nNotExists = 0;

	// get the prop. for each of our contents
 	VCPOSITION pos = m_Contents.GetHeadPosition();
	while (pos != NULL)
	{
		BldFileRegHandle hndFileReg = (BldFileRegHandle) m_Contents.GetNext(pos);
	
		// This function will ensure that all props. are updated
		// for each of our contents
		BOOL bExists;
		BOOL bOK = hndFileReg->GetIntProp(P_ExistsOnDisk, bExists);
		VSASSERT(bOK, "Failed to determine whether the file exists on disk!");
		if (!bExists)
			nNotExists++;
	}

	m_fCalcAttrib = fOldOn;

	// set whether all our contents exist or not
	m_nNotExists = (USHORT)nNotExists;
	SetIntProp(P_ExistsOnDisk, m_nNotExists == 0);
}

BOOL CBldFileRegSet::Touch()
{
	BOOL bRetVal = TRUE;

	// get the prop. for each of our contents
 	VCPOSITION pos = m_Contents.GetHeadPosition();
	while (pos != NULL)
	{
		BldFileRegHandle hndFileReg = (BldFileRegHandle) m_Contents.GetNext(pos);
		bRetVal = bRetVal && hndFileReg->Touch();
	}

	return bRetVal;
}

void CBldFileRegSet::DeleteFromDisk()
{
	// get the prop. for each of our contents
 	VCPOSITION pos = m_Contents.GetHeadPosition();
	while (pos != NULL)
	{
		BldFileRegHandle hndFileReg = (BldFileRegHandle) m_Contents.GetNext(pos);
		hndFileReg->DeleteFromDisk();
	}
}

void CBldFileRegSet::OnNotify(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC, UINT idChange, void* pHint)
{
	FILETIME Time;
	FILETIME ChangedTime;

	VSASSERT(hndFileReg, "Trying to do notifications with a NULL reg handle.  Bad business.");
	if (!PROJECT_IS_LOADED())
		return;	// can't do anything during project load/unload

	// Handle notifications about content changes here
	switch (idChange)
	{
 		case P_NewTStamp:
 		{
			// calculate new attrib?
			if (!m_fCalcAttrib)
				break;	// no

			FILETIME * pPreviousTime = (FILETIME *)pHint;
 			if (GetTimeProp(P_NewTStamp, Time, FALSE) && hndFileReg->GetTimeProp(P_NewTStamp, ChangedTime, FALSE))
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
				MakeDirty();
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


 		case FRN_ALL:
		{
			MakeDirty();
			break;
		}
 	}
}

BOOL CBldFileRegSet::GetTimeProp(UINT idProp, FILETIME& ftime, BOOL bUpdate)
{
	// Get a time property value
	if (!PROJECT_IS_LOADED())
	{
		ftime.dwLowDateTime = 0xffffffff;
		ftime.dwHighDateTime = 0x7fffffff;
		return TRUE;
	}
	
	if (bUpdate && !m_bSkipUpdate)
		(void)UpdateAttributes();
 
	switch (idProp)
	{
		case P_NewTStamp:
			ftime = m_NewTime;
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

BOOL CBldFileRegSet::SetTimeProp(UINT idProp, FILETIME ftime)
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
			return FALSE;
	}

	// any change?
	if (oldtime.dwLowDateTime != ftime.dwLowDateTime ||
		oldtime.dwHighDateTime != ftime.dwHighDateTime
	   )
	   if (PROJECT_IS_LOADED()) // skip this during project load/unload
		{
			// notify our dependants that a time property value has changed
			InformNotifyRxs(idProp, NULL, (void *)&oldtime);
		}

	return TRUE;
}

void CBldFileRegSet::SetFileRegistry(CBldFileRegistry * pFileRegistry)
{
	// This should be a one time only operation.
//	VSASSERT( IsEmpty(), "Calling SetFileRegistry on a regset more than once." );

	if (NULL != pFileRegistry)
	{
		VCPOSITION pos = m_Contents.GetHeadPosition();
		while (NULL != pos)
		{
			BldFileRegHandle frh = (BldFileRegHandle)m_Contents.GetNext(pos);
			pFileRegistry->AddRegRef(frh);
		}

		VSASSERT(NULL == m_pFileRegistry, "Trying to SetFileRegistry more than once on same regset");
		AddNotifyRx(pFileRegistry, FALSE);
	}
	else if (NULL != m_pFileRegistry)
	{
		RemoveNotifyRx(m_pFileRegistry);

		VCPOSITION pos = m_Contents.GetHeadPosition();
		while (NULL!=pos)
		{
			BldFileRegHandle frh = (BldFileRegHandle)m_Contents.GetNext(pos);
			m_pFileRegistry->ReleaseRegRef(frh);
		}
	}
	else
	{
//		VSASSERT(FALSE, "Bad else case");
	}

	m_pFileRegistry = pFileRegistry;
}

void CBldFileRegSet::MakeContentsDirty(CBldFileRegistry* pRegistry)
{
	InitFrhEnum();
	BldFileRegHandle frh;
	while ((frh = NextFrh()) != (BldFileRegHandle)NULL)
	{
		pRegistry->GetRegEntry(frh)->MakeDirty();
		frh->ReleaseFRHRef();
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	CBldFileRegFilter

CBldFileRegFilter::CBldFileRegFilter(CStringW strFilter)
{
	m_dwNodeType = nodetypeRegEntry | nodetypeRegSet | nodetypeRegNotifyRx | nodetypeRegFilter;
	m_strFilter = strFilter;
	m_strFilterLC = strFilter;
	m_strFilter.MakeUpper();
	m_strFilterLC.MakeLower();
}

CBldFileRegFilter::~CBldFileRegFilter()
{
	m_bUpdateAttribs = FALSE;	// don't attempt to update
 	m_fCalcAttrib = FALSE;		// don't re-calc attrib.
}

BOOL CBldFileRegFilter::Filter(const wchar_t* pszText)
{
	const wchar_t* pchLastText = NULL;
	const wchar_t* pchPat = m_strFilter;
	const wchar_t* pchPatLC = m_strFilterLC;
	int ichPat = 0, ichLastStar = -1;
	int c;

	while ((*pszText != L'\0') && ((c = pchPat[ichPat]) != L'\0'))
	{
		if (c == L'*')
		{
			ichLastStar = ichPat;
			pchLastText = pszText;
		}
		else if (c == L'?')
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

	// at least one of these must be null to break out of loop
	VSASSERT((*pchPat == L'\0') || (*pszText == L'\0'), "Broke out of loop too early in CBldFileRegFilter::Filter");

	return (*pchPat == *pszText); // == L'\0'
}

BOOL CBldFileRegFilter::AddRegHandle(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC /* = NULL */, 
	BOOL bAddRef /* = TRUE */, BOOL bAddFront /* = FALSE */)
{
	// This should never be called since the CBldFileRegistry should be 
	// managing the filter contents.
	VSASSERT(FALSE, "Filter trying to manage filter contents; file registry should be doing that.");
	return FALSE;
}

BOOL CBldFileRegFilter::RemoveRegHandle(BldFileRegHandle hndFileReg, BOOL bReleaseRef /* = TRUE */)
{
	// This should never be called since the CBldFileRegistry should be 
	// managing the filter contents.
	VSASSERT(FALSE, "Filter trying to manage filter contents; file registry should be doing that.");
	return FALSE;
}

BOOL CBldFileRegFilter::AddFilterHandle(BldFileRegHandle hndFileReg)
{
	// Only add files that match our filter
	if (!Filter(*((CBldFileRegFile*)hndFileReg)->GetFilePath()))
		return FALSE;

	return CBldFileRegSet::AddRegHandle(hndFileReg);	// Call base method.
}

BOOL CBldFileRegFilter::RemoveFilterHandle(BldFileRegHandle hndFileReg)
{
	if (Filter(*((CBldFileRegFile*)hndFileReg)->GetFilePath()))
	{
		// Prevent access to registry by other threads.
		CritSectionT cs(g_sectionRegistryFilter);

		VCPOSITION pos = m_Contents.Find(hndFileReg);
		if (NULL != pos)	// won't find it if we weren't filtering it (as in it was excluded from build...)
			return CBldFileRegSet::RemoveRegHandle(hndFileReg);	// Call base method.
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
//	CBldFileRegFilterSet

CBldFileRegFilterSet::CBldFileRegFilterSet()
{
	m_dwNodeType = nodetypeRegEntry | nodetypeRegSet | nodetypeRegNotifyRx | nodetypeRegFilterSet;
}

CBldFileRegFilterSet::~CBldFileRegFilterSet()
{
}

void CBldFileRegFilterSet::OnNotify(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC, UINT idChange, void* pHint)
{
	BldFileRegHandle frh = (BldFileRegHandle)pHint;
	switch (idChange)
	{
	case FRI_ADD:
		VSASSERT(NULL != m_FilterList.Find((void*)hndFileReg), "Failed to find the frh in the filter list!");
		VSASSERT(frh->IsNodeType(nodetypeRegFile), "Can only add a file reg handle to a filter set!");
		CBldFileRegSet::AddRegHandle(frh, pEC);
		break;

	case FRI_DESTROY:
		VSASSERT(NULL != m_FilterList.Find((void*)hndFileReg), "Failed to find the frh in the filter list!");
		VSASSERT(frh->IsNodeType(nodetypeRegFile), "Can only remove a file reg handle from a filter set!");
		CBldFileRegSet::RemoveRegHandle(frh);
		break;

	default:
		CBldFileRegSet::OnNotify(hndFileReg, pEC, idChange, pHint);
		break;
	}
}

BOOL CBldFileRegFilterSet::AddFilter( CBldFileRegFilter* pFilter )
{
	VSASSERT(pFilter->IsNodeType(nodetypeRegFilter), "Only call AddFilter with filters!");
	VSASSERT(NULL == m_FilterList.Find((void*)pFilter), "Only call AddFilter once with any given filter!");

	m_FilterList.AddTail(pFilter);

	pFilter->InitFrhEnum();

	// Add pFilter contents to CBldFileRegFilterSet contents.
	BldFileRegHandle frh = NULL;
	while (NULL!=(frh=pFilter->NextFrh()))
	{
		CBldFileRegSet::AddRegHandle(frh);
		frh->ReleaseFRHRef();
	}

	// Interested in FRI_ADD and FRI_DESTROY notifications from pFilter.
	pFilter->AddNotifyRx(this, TRUE);

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
//	CBldFileRegFile

CVCMapStringWToPtr*	CBldFileRegFile::s_pFileNameMap = NULL;
CBldFileRegFile::CInitMap s_InitRegFileMap(1499);
CBldFileRegFile::CInitMap::CInitMap(int nHashSize)
{
	CBldFileRegFile::s_pFileNameMap = new CVCMapStringWToPtr(128);
	CBldFileRegFile::s_pFileNameMap->InitHashTable(nHashSize);	// init our map too!
}

CBldFileRegFile::CInitMap::~CInitMap()
{
	// Prevent access to file map by other threads.
	CritSectionT cs(g_sectionFileMap);

	if (CBldFileRegFile::s_pFileNameMap)
		delete CBldFileRegFile::s_pFileNameMap;
	CBldFileRegFile::s_pFileNameMap = NULL;
}

BldFileRegHandle CBldFileRegFile::GetFileHandle(LPCOLESTR szFileName, BOOL bVerifyCase)
{
	CPathW path;
	LPCOLESTR szKey;
	
	if (bVerifyCase)
	{
		if (!path.Create(szFileName))
			return NULL;
		path.GetActualCase(TRUE);
		szKey = path;
	}
	else
		szKey = szFileName;

	BldFileRegHandle hndFileReg = NULL;
	{
		// Prevent access to file map by other threads.
		CritSectionT cs(g_sectionFileMap);
		hndFileReg = LookupFileHandleByName(szKey, FALSE);
	}

	if (NULL == hndFileReg)
	{
		if (!bVerifyCase)
		{
			if (!path.Create(szFileName))
				return NULL;
		}

		CBldFileRegFile * pregfile = new CBldFileRegFile(&path, FALSE /* already verified */);
		if (NULL != pregfile)
		{
			hndFileReg = (BldFileRegHandle)pregfile;

			VSASSERT(NULL != s_pFileNameMap, "File name map not initialized before we tried to use it.");
			if (NULL != s_pFileNameMap)
			{
				CritSectionT cs(g_sectionFileMap);
				s_pFileNameMap->SetAt(szKey, hndFileReg);
			}
			hndFileReg->AddFRHRef();
		}
	}

	return hndFileReg;
}

BldFileRegHandle CBldFileRegFile::LookupFileHandleByName(LPCOLESTR szFileName, BOOL bVerifyCase)
{
	BldFileRegHandle hndFileReg = NULL;

	// No map, no handle.
	if (NULL == s_pFileNameMap)
		return NULL;

	// Key is actual case full path name.
	LPCOLESTR szKey;
	CPathW path;
	if (bVerifyCase)
	{
		if (!path.Create(szFileName))
			return NULL;
		path.GetActualCase(TRUE);
		szKey = path;
	}
	else
	{
		szKey = szFileName;
	}


	// Prevent access to file map by other threads.
	{
		CritSectionT cs(g_sectionFileMap);
		s_pFileNameMap->Lookup(szKey, (void*&)hndFileReg);
	}

	if (NULL != hndFileReg)
		hndFileReg->AddFRHRef();

	return hndFileReg;
}


BldFileRegHandle CBldFileRegFile::LookupFileHandleByKey(LPCOLESTR szKeyName)
{
	BldFileRegHandle hndFileReg = NULL;

	// No map, no handle.
	if (NULL == s_pFileNameMap)
		return NULL;

	VSASSERT(MaxKeyLen > wcslen(szKeyName), "File name too long for us to be able to look up.");

	{
		// Prevent access to file map by other threads.
		CritSectionT cs(g_sectionFileMap);
		s_pFileNameMap->Lookup(szKeyName, (void*&) hndFileReg);
	}

	if (NULL != hndFileReg)
		hndFileReg->AddFRHRef();

	return hndFileReg;
}

void CBldFileRegFile::UpdateTimeStampTick()
{
	g_dwUpdateTickCount++;
}

void CBldFileRegFile::SafeDelete()
{
	// Prevent access to file map by other threads.
	CritSectionT cs(g_sectionFileMap);

	if (0 == m_nRefCount)
		delete this;
}

CBldFileRegFile::CBldFileRegFile
(
	const CPathW * pPath,
	BOOL bMatchCase /* = FALSE */,
	BOOL bFiltered /* = FALSE */
)
	: CBldFileRegEntry()
{
	m_dwNodeType = nodetypeRegEntry | nodetypeRegNotifyRx | nodetypeRegFile;
	m_nExistsOnDisk = TRUE;
	m_dwUpdateTick = 0;

	VSASSERT(pPath->IsInit(), "File path not initialized before creating a reg handle for it.");
	m_Path = *pPath;
	VSASSERT(0 < wcslen(m_Path.GetFullPath()), "Trying to create a reg handle for a NULL file path.  Bad programmer, bad programmer.");

	m_bWantActualCase = (bMatchCase != 0);

	// Review: avoid setting case here if possible
	m_bMatchCase = (bMatchCase == TRUE);
	if (m_bMatchCase) // use bMatchCase = -1 to defer this
		m_Path.GetActualCase(FALSE);
	m_bDirty = TRUE;
	m_bFileNotOnDisk = FALSE;
	plstFileItems = (CVCPtrList *)NULL;
	m_ich = 0;
	m_pOrgName = (wchar_t *)NULL;

	m_FileTime.dwLowDateTime = m_FileTime.dwHighDateTime = 0;
}

CBldFileRegFile::~CBldFileRegFile()
{
	// Remove from map (if map hasn't been destroyed yet during shutdown).

	// No map, no handle.
	if (NULL==s_pFileNameMap)
		return;

	// Delete our list of associated file items
	if (plstFileItems != (CVCPtrList*) NULL)
		delete plstFileItems;

	if (m_pOrgName)
		free(m_pOrgName);

	CritSectionT cs(g_sectionFileMap);
	BOOL b = s_pFileNameMap->RemoveKey(*GetFilePath());
	if( b == FALSE )
	{
		return;
	}
}

BOOL CBldFileRegFile::Touch()
{
	int fh = _wutime (m_Path.GetFullPath(), NULL);
	if (fh == -1)
		return FALSE;
	return TRUE;
}

void CBldFileRegFile::DeleteFromDisk()
{
	// This will cause the file change to be ignored. So things like the 
	// source editor will not get notified of the change.
	m_Path.DeleteFromDisk();
	return;
}

BOOL CBldFileRegFile::GetFileTime(FILETIME& ft)
{
	(void)UpdateAttributes();
	return m_nExistsOnDisk && GetTimeProp(P_NewTStamp, ft, FALSE);
}

BOOL CBldFileRegFile::GetTimeProp(UINT idProp, FILETIME& ftime, BOOL bUpdate)
{
	// Get a time property value
	if (bUpdate || (m_FileTime.dwLowDateTime == 0 && m_FileTime.dwHighDateTime == 0))
		(void)UpdateAttributes();
 
	VSASSERT(idProp == P_NewTStamp, "Only understand P_NewTStamp time prop!");
	ftime = m_FileTime;
 	return TRUE;
}

BOOL CBldFileRegFile::SetTimeProp(UINT idProp, FILETIME ftime)
{
 	if (idProp != P_NewTStamp)
 		return FALSE;

 	FILETIME oldtime = m_FileTime;
 
	// any change?
	if (oldtime.dwLowDateTime != ftime.dwLowDateTime || oldtime.dwHighDateTime != ftime.dwHighDateTime)
	{
		m_FileTime = ftime;

		if (PROJECT_IS_LOADED()) // skip this during project load/unload
		{
			// notify our dependents that a time property value has changed
			InformNotifyRxs(P_NewTStamp, NULL, (void *)&oldtime);
		}
	}

	return TRUE;
}

BOOL CBldFileRegFile::RefreshAttributes()
{
	// No longer dirty
	m_bDirty = FALSE;

	// Get the current file attributes
	if (m_dwUpdateTick == g_dwUpdateTickCount)
		return FALSE;	// nothing to do
	m_dwUpdateTick = g_dwUpdateTickCount;

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

	return TRUE;	// performed a refresh!
}

void CBldFileRegFile::SetAttributes(S_ATTRIB * pattrib)
{
 	// Are we being notified of changes on this file?
 	VSASSERT(pattrib->_fOk, "Bad input parameter!");

 	SetIntProp(P_ExistsOnDisk, pattrib->_fExists);
	SetTimeProp(P_NewTStamp, pattrib->_filetime);
}

BOOL WINAPI OGetFileAttributesExW( LPCOLESTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, WIN32_FILE_ATTRIBUTE_DATA *lpFileInformation )
{
	if ( ::IsUnicodeSystem() )
	{
		return GetFileAttributesExW( lpFileName, fInfoLevelId, lpFileInformation );
	}
	else
	{
		CStringA strFileName = lpFileName;	// yes, ANSI -- this only is used on Win9X systems
		return GetFileAttributesExA(strFileName, fInfoLevelId, lpFileInformation);
	}
}

void CBldFileRegFile::GetAttributes(S_ATTRIB * pattrib)
{
	if ((!m_bMatchCase) && (m_bWantActualCase))
	{
		WIN32_FIND_DATAW find;
		HANDLE hfind = ::FindFirstFileW((LPCOLESTR)m_Path, &find);

		pattrib->_fOk = TRUE;
		if (hfind != INVALID_HANDLE_VALUE)
		{
			::FindClose(hfind);

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
		if (OGetFileAttributesExW( (LPCOLESTR)m_Path, GetFileExInfoStandard, &find))
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

void CBldFileRegFile::SetMatchCase(BOOL bMatchCase, BOOL bCaseApproved /*=FALSE*/)
{
	if (!bCaseApproved && (!m_bMatchCase && bMatchCase))
 		m_Path.GetActualCase(FALSE);

	m_bWantActualCase = bMatchCase;
	m_bMatchCase = bMatchCase;
}

BOOL CBldFileRegFile::UpdateAttributes()
{
 	return CBldFileRegEntry::UpdateAttributes();
}

/////////////////////////////////////////////////////////////////////////////
//
//	CBldFileRegistry

CBldFileRegistry::CBldFileRegistry(CBldCfgRecord* pCfgRecord, int nHashSize) : CBldFileRegSet(nHashSize)
{
	m_dwNodeType = nodetypeRegEntry | nodetypeRegSet | nodetypeRegNotifyRx | nodetypeRegRegistry;

	// Set up the file registry pointer
	m_pFileRegistry = this;

	// We must have a contents map because we use it for reference counting.
	m_pmapContents = new CVCMapPtrToWord;
	m_pmapContents->InitHashTable(nHashSize, FALSE);	// set hash size as requested
}

CBldFileRegistry::~CBldFileRegistry()
{
	VSASSERT(IsEmpty(), "File registry should be empty by d'tor time!");

	// Remove filters.
	VCPOSITION pos0 = m_FilterMap.GetStartPosition();
	while (NULL != pos0)
	{
		BldFileRegHandle frh0;
		CStringW strKey;
		m_FilterMap.GetNextAssoc(pos0, strKey, (void*&)frh0);
		if (frh0->IsNodeType(nodetypeRegFilter))
			delete frh0;
		else if (frh0->IsNodeType(nodetypeRegFilterSet))
			delete frh0;
		else
		{
			// Must be a CBldFileRegFilter or CBldFileRegSet containing filters.
			VSASSERT(FALSE, "Can only remove CBldFileRegFilter or CBldFileRegSet objects!");
		}
	}
}

BldFileRegHandle CBldFileRegistry::RegisterFile
(
	const CPathW* pPath,
	BOOL bMatchCase /* = TRUE */,
	BOOL bNoFilter /* = TRUE */
)
{
	if (!pPath->IsInit())
	{
		VSASSERT(FALSE, "Path must be initialized before file can be registered!");		// caller shouldn't allow this
		return (BldFileRegHandle)NULL;	// can't do
	}

	// Lookup file in the global file registry thing.  If it's there then, get its BldFileRegHandle.
	// Otherwise, create a new CBldFileRegFile item and get its BldFileRegHandle.
	BldFileRegHandle hndFileReg = NULL;

	hndFileReg = CBldFileRegFile::GetFileHandle(*pPath, !pPath->IsActualCase());
	if (NULL != hndFileReg)
	{
		AddRegRef(hndFileReg, bNoFilter);
		hndFileReg->SetMatchCase(bMatchCase, TRUE /* case already approved */);
		hndFileReg->ReleaseFRHRef();
	}

	return hndFileReg;
}

void CBldFileRegistry::AddRegRef(BldFileRegHandle hndFileReg, BOOL bNoFilter /* = FALSE */)
{
	VSASSERT(m_pmapContents != NULL, "Our map is NULL!");

	WORD wRefCount = 0;
	BOOL bFound = m_pmapContents->Lookup(hndFileReg, wRefCount);

	if (!bFound)
	{
		// Add to registry.
		// AddRegHandle takes care of actual ref counts on hndFileReg.
		AddRegistryHandle(hndFileReg, NULL, bNoFilter);
		wRefCount = 0;
	}

	wRefCount++;
	m_pmapContents->SetAt(hndFileReg, wRefCount);
}

void CBldFileRegistry::ReleaseRegRef(BldFileRegHandle hndFileReg, BOOL bNoFilter /* = FALSE */)
{
	VSASSERT(hndFileReg, "Cannot release NULL frh!");
	if (!hndFileReg)
		return; // just bail

	WORD wRefCount;
	VSASSERT(m_pmapContents != NULL, "Our map was not initialized!");
	VSASSERT(m_pmapContents->Lookup(hndFileReg, wRefCount), "Trying to release an frh that is already gone!");

	if (!m_pmapContents->Lookup(hndFileReg, wRefCount))
		wRefCount = 0;

	VSASSERT(0 != wRefCount, "Releasing the last refcount too early!");

	if (wRefCount != 0)
 		wRefCount--;

	// Prevent access to registry by other threads.
	CritSectionT cs(g_sectionRegistryFull);

	if (wRefCount != 0)
	{
		m_pmapContents->SetAt(hndFileReg, wRefCount);
		if (bNoFilter && hndFileReg->IsNodeType(nodetypeRegFile))
		{
			// Remove from filters.
			UnFilterFile(hndFileReg);
		}
	}
	else
	{
		m_pmapContents->RemoveKey(hndFileReg);

		// RemoveRegHandle takes care of actual ref counts on hndFileReg.
		RemoveRegistryHandle(hndFileReg);
	}
}

BldFileRegHandle CBldFileRegistry::FindBasicFilter(const CStringW* pstrBasicFilter)
{
	// Find a basic file filter
	BldFileRegHandle hndFileFilter;

	if (m_FilterMap.Lookup(*pstrBasicFilter, (void*&)hndFileFilter))
		return hndFileFilter;

	return NULL;
}

BldFileRegHandle CBldFileRegistry::RegisterFileFilter(const CStringW* pstrFilter)
{
	BldFileRegHandle hndFileReg;

	hndFileReg = LookupFileFilter(pstrFilter);

	// Are we trying to register a file filter that has already been registered?
	if (hndFileReg)
	{
		hndFileReg->AddFRHRef();
		return hndFileReg;
	}

	CVCStringWList BasicFilters;
	GetBasicFilters(pstrFilter, BasicFilters);

	if (BasicFilters.GetCount() > 1)
	{
		// Create the set
		hndFileReg = (BldFileRegHandle) new CBldFileRegFilterSet;
		hndFileReg->AddFRHRef();
 
		// Multiple file filter
		CVCPtrList NewFilters;

		BldFileRegHandle hndFileFilter;
		VCPOSITION pos = BasicFilters.GetHeadPosition();
		while (pos != NULL)
		{
			CStringW strBasicFilter = BasicFilters.GetNext(pos);
			hndFileFilter = FindBasicFilter(&strBasicFilter);
			if (hndFileFilter == NULL)
			{
				hndFileFilter = RegisterFileFilter(&strBasicFilter);
				hndFileFilter->ReleaseFRHRef();
 			}

			((CBldFileRegFilterSet*)hndFileReg)->AddFilter((CBldFileRegFilter*)hndFileFilter);
 		}
 	}
	else
	{
		// Single basic file filter
		// First we had better check to see if we actually have created a
		// file filter like this, but it is not registered.
		CStringW strKey = BasicFilters.GetHead();
		hndFileReg = FindBasicFilter(&strKey);
		if (hndFileReg == NULL)
		{
 			// Nope, we don't have one so now we create it and set an entry in our map.
			hndFileReg = (BldFileRegHandle)new CBldFileRegFilter(strKey);
			hndFileReg->AddFRHRef();
 
			// Filter all files through this file filter
			CStringW strFileKey;
			BldFileRegHandle hndFile;
			VCPOSITION pos = m_Contents.GetHeadPosition();
			while (pos != NULL)
			{
				hndFile = (BldFileRegHandle)m_Contents.GetNext(pos);
				VSASSERT(NULL != hndFile, "Found NULL frh in content list!");
				if (hndFile->IsNodeType(nodetypeRegFile))
					((CBldFileRegFilter*)hndFileReg)->AddFilterHandle(hndFile);
			}
		}
		else
		{
			VSASSERT(FALSE, "FindBasicFilter() should not succeed after LookupFileFilter() failed.");
		}
	}

	// Put an entry in our registered file filter map
	CStringW strKey = *pstrFilter;
	strKey.MakeUpper();
 	m_FilterMap.SetAt(strKey, hndFileReg);

	return hndFileReg;
}

BOOL CBldFileRegistry::RemoveFileFilter(const CStringW* pstrFilter)
{
	// We can only remove file filters that have been registered
	BldFileRegHandle hndFileReg;

	CStringW strKey = *pstrFilter;
	strKey.MakeUpper();

 	if (!m_FilterMap.Lookup(strKey, (void*&)hndFileReg))
 		return FALSE;

 	if (!hndFileReg->IsNodeType(nodetypeRegFilter))
	{
		VSASSERT(hndFileReg->IsNodeType(nodetypeRegSet), "Can only call RemoveFileFilter on filters and regsets!");
 		CBldFileRegSet* hndFileSet = (CBldFileRegSet*)hndFileReg;

		const CVCPtrList* pFilterList = &hndFileSet->m_Contents;

		VCPOSITION pos = pFilterList->GetHeadPosition();
		while (pos != NULL)
		{
			BldFileRegHandle hndFileRegFilter = (BldFileRegHandle) pFilterList->GetNext(pos);
			VCPOSITION posFilter = m_Contents.Find(hndFileRegFilter);
			if (NULL != posFilter)
			{
				hndFileSet->m_Contents.RemoveAt(posFilter);
				hndFileRegFilter->ReleaseFRHRef();
			}
		}
 	}

	m_FilterMap.RemoveKey(strKey);

	hndFileReg->ReleaseFRHRef();
	return TRUE;
}

BldFileRegHandle CBldFileRegistry::LookupFile(LPCOLESTR lpszFile, BOOL bVerifyCase)
{
	BldFileRegHandle hndFileReg = CBldFileRegFile::GetFileHandle(lpszFile, bVerifyCase);
	if (NULL != hndFileReg)
	{
		if (!RegHandleExists(hndFileReg))
		{
			hndFileReg->ReleaseFRHRef();
			hndFileReg = NULL;
		}
	}

	return hndFileReg;
}

BldFileRegHandle CBldFileRegistry::LookupFile(const CPathW* pPath, BOOL bVerifyCase)
{
	VSASSERT(pPath->IsInit(), "Path not initialized before use!");
	if (bVerifyCase)
		bVerifyCase = !pPath->IsActualCase();
	return CBldFileRegistry::LookupFile(LPCOLESTR(*pPath), bVerifyCase);
}

BldFileRegHandle CBldFileRegistry::LookupFileFilter(const CStringW* pstrFilter)
{
	// Create a list of the basic file filters
 	CVCStringWList BasicFilters;

	GetBasicFilters(pstrFilter, BasicFilters);

	// FUTURE (colint): NYI
 	// Generate permutations of the basic file filters
	CVCStringWList Permutations;

	// FUTURE (colint): For now we just add one of the permutations
	CStringW strTempPerm= *pstrFilter;
	strTempPerm.MakeUpper();
	Permutations.AddTail(strTempPerm);

	// Lookup each of the permutations
	BldFileRegHandle hndFileReg;

	VCPOSITION pos = Permutations.GetHeadPosition();
	while (pos != NULL)
	{
		CStringW strKey = Permutations.GetNext(pos);
		if (m_FilterMap.Lookup(strKey, (void*&)hndFileReg))
			return hndFileReg;
	}

 	return NULL;
}

void CBldFileRegistry::GetBasicFilters(const CStringW* pstrFilter, CVCStringWList& strList)
{  		 
	CStringW strKey = *pstrFilter;
	strKey.MakeUpper();
	strKey += L";";

	int nBreak = -1;
	do
	{
		nBreak = strKey.Find(L';');
		if (nBreak != -1)
		{
			CStringW strBasicFilter = strKey.Left(nBreak);
			strList.AddTail(strBasicFilter);
			strKey = strKey.Right(strKey.GetLength() - nBreak - 1);
		}
	} while (nBreak != -1);
}

void CBldFileRegistry::FilterFile(BldFileRegHandle hndFile)
{
	VSASSERT(hndFile->IsNodeType(nodetypeRegFile), "Can only filter files!");

	// Notify all file filters that we have in the registry at the moment.
	VCPOSITION pos = m_FilterMap.GetStartPosition();
	while (pos != NULL)
	{
		BldFileRegHandle hndFileReg;
		CStringW strFile;
		m_FilterMap.GetNextAssoc(pos, strFile, (void*&)hndFileReg);
		if (hndFileReg->IsNodeType(nodetypeRegFilter))
			((CBldFileRegFilter*)hndFileReg)->AddFilterHandle(hndFile);
	}
}

void CBldFileRegistry::UnFilterFile(BldFileRegHandle hndFile)
{
	VSASSERT(hndFile->IsNodeType(nodetypeRegFile), "Can only unfilter files!");

	// Notify all file filters that we have in the registry at the moment.
	VCPOSITION pos = m_FilterMap.GetStartPosition();
	while (pos != NULL)
	{
		BldFileRegHandle hndFileReg;
		CStringW strFile;
		m_FilterMap.GetNextAssoc(pos, strFile, (void*&)hndFileReg);
		if (hndFileReg->IsNodeType(nodetypeRegFilter))
			((CBldFileRegFilter*)hndFileReg)->RemoveFilterHandle(hndFile);
	}
}

void CBldFileRegistry::OnNotify(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC, UINT idChange, void* pHint)
{
	VSASSERT(hndFileReg, "Bad input parameter!");
	
	switch (idChange)
	{
		// Take care of references counts.
		// (Add/Release)RegRef() will do the work.
		case FRI_ADD:
		{
			CBldFileRegSet* pregset = (CBldFileRegSet*)hndFileReg;
			VSASSERT(pregset->IsNodeType(nodetypeRegSet), "Can only add to regsets!");
			VSASSERT(!pregset->IsNodeType(nodetypeRegRegistry), "Don't use OnNotify to add to a registry!");
			VSASSERT(this == pregset->m_pFileRegistry, "Registry mismatch between what's passed in and us!");

			CBldFileRegEntry* regent = (CBldFileRegEntry*)pHint;
			AddRegRef(regent);
			break;
		}
		case FRI_DESTROY:
		{
			CBldFileRegSet* pregset = (CBldFileRegSet*)hndFileReg;
			VSASSERT(pregset->IsNodeType(nodetypeRegSet), "Can only remove from regsets!");
			VSASSERT(!pregset->IsNodeType(nodetypeRegRegistry), "Don't use OnNotify to remove from a registry!");
			VSASSERT(this == pregset->m_pFileRegistry, "Registry mismatch between what's passed in and us!");

			CBldFileRegEntry* regent = (CBldFileRegEntry*)pHint;
			ReleaseRegRef(regent);
			break;
		}
	}
}

BOOL CBldFileRegistry::DeleteFile(BldFileRegHandle hndFileReg)
{
	VSASSERT(hndFileReg, "Cannot delete NULL frh!");
	VSASSERT(hndFileReg->IsNodeType(nodetypeRegFile), "Can only call DeleteFile for files!");

	const CPathW * pPath = ((CBldFileRegFile*)hndFileReg)->GetFilePath();
 	VSASSERT(pPath != (const CPathW *)NULL, "No path for our file!");

	// Prevent access to DeleteFile by other threads.
	CritSectionT cs(g_sectionFileDelete);

	BOOL fRet = ::DeleteFileW((const wchar_t*)*pPath);

	if (fRet)
	{
		// it's gone, let's assume this and set the attributes
		S_ATTRIB attrib;
		attrib._fOk = TRUE;
		attrib._filetime.dwLowDateTime = attrib._filetime.dwHighDateTime = 0;	// none
		attrib._fExists = FALSE;

		((CBldFileRegFile*)hndFileReg)->SetAttributes(&attrib);
	}

	return fRet;
}

void CBldFileRegistry::RemoveFromProject(IVCBuildableItem* pItem, BOOL fRemoveContained)
{
	// do we have a file reghandle to remove?
	BldFileRegHandle hndFileReg = NULL;
	pItem->get_FileRegHandle((void **)&hndFileReg);
	if (hndFileReg != (BldFileRegHandle)NULL)
	{
		CBldFileRegEntry* pRegEntry = GetRegEntry(hndFileReg); 
		VSASSERT(pRegEntry, "Item not in registry.  Must have been released too early somewhere else.");
		if (this != &g_FileRegistry)
			ReleaseRegRef(pRegEntry);
	}

	// remove all our children?
	CComPtr<IEnumVARIANT> spContentList;
	if (fRemoveContained && pItem->get_ContentList(&spContentList) == S_OK && spContentList != NULL)
	{
		CBldFileRegistry* pThisRegistry = NULL;
		if (SUCCEEDED(pItem->get_Registry((void **)&pThisRegistry)) && pThisRegistry != NULL)
		{
			spContentList->Reset();
			while (TRUE)
			{
				CComVariant var;
				HRESULT hr = spContentList->Next(1, &var, NULL);
				if (hr != S_OK)
					break;
				if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
					continue;
				IDispatch* pDispContained = var.pdispVal;
				CComQIPtr<IVCBuildableItem> spContained = pDispContained;
				if (spContained == NULL)
					continue;

				pThisRegistry->RemoveFromProject(spContained, fRemoveContained);
			}
		}
	}
}
	
void CBldFileRegistry::RestoreToProject(IVCBuildableItem* pItem, BOOL fRestoreContained)
{
	// do we have a file reghandle to restore?
	BldFileRegHandle hndFileReg = NULL;
	pItem->get_FileRegHandle((void **)&hndFileReg);
	if (hndFileReg != (BldFileRegHandle)NULL)
	{
#ifdef _DEBUG
		CBldFileRegEntry *pRegEntry = GetRegEntry(hndFileReg); VSASSERT(pRegEntry, "Failed to find a reg entry for our file!");
#endif
		if (this != &g_FileRegistry)
			AddRegRef(hndFileReg, TRUE);	// no filtering here...
	}

	// restore all our children?
	CComPtr<IEnumVARIANT> spContentList;
	if (fRestoreContained && pItem->get_ContentList(&spContentList) == S_OK && spContentList != NULL)
	{
		CBldFileRegistry* pThisRegistry = NULL;
		if (SUCCEEDED(pItem->get_Registry((void **)&pThisRegistry)) && pThisRegistry != NULL)
		{
			spContentList->Reset();
			while (TRUE)
			{
				CComVariant var;
				HRESULT hr = spContentList->Next(1, &var, NULL);
				if (hr != S_OK)
					break;
				if (var.vt != VT_DISPATCH && var.vt != VT_UNKNOWN)
					continue;
				IDispatch* pDispContained = var.pdispVal;
				CComQIPtr<IVCBuildableItem> spContained = pDispContained;
				if (spContained == NULL)
					continue;

				pThisRegistry->RestoreToProject(spContained, fRestoreContained);
			}
		}
	}
}

//
// New CBldFileRegistry functions.
//

BOOL CBldFileRegistry::AddRegistryHandle(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC /* = NULL */, BOOL bNoFilter /* = FALSE */)
{
	VSASSERT(hndFileReg, "Trying to add a NULL frh!");

	// Shouldn't be adding again.
	VSASSERT(!RegHandleExists(hndFileReg), "Trying to add a reg handle more than once!");
	VSASSERT(!hndFileReg->IsNodeType(nodetypeRegRegistry), "Cannot call AddRegistryHandle with a registry!");
	VSASSERT(!hndFileReg->IsNodeType(nodetypeRegFilter), "Cannot call AddRegistryHandle with a filter!");

	// Already exists in set....return
	if (RegHandleExists(hndFileReg))
		return TRUE;

	// Reference held by this file set.
	hndFileReg->AddFRHRef();

	// This may result in new dependency update requests, so we must not be locking
	// the lists at this time.
	VSASSERT(!g_sectionDepUpdateQ.FOwned(), "List locked at a bad time!");

	// Is the CBldFileRegEntry a CBldFileRegSet if so then we must
	// set up the associated file registry...
	CBldFileRegSet* pFileRegSet = (CBldFileRegSet *)hndFileReg;

	if (pFileRegSet->IsNodeType(nodetypeRegSet))
		pFileRegSet->SetFileRegistry(this);

	{
		// Prevent access to registry by other threads.
		CritSectionT cs(g_sectionRegistryFull);

		// Add the CBldFileRegEntry to the registry.
		m_Contents.AddTail(hndFileReg);
	}

	// Add files to file map.
	if (!bNoFilter && pFileRegSet->IsNodeType(nodetypeRegFile))
	{
		// Filter file.
		FilterFile(hndFileReg);
	}

	InformNotifyRxs(FRI_ADD, pEC, (void *)hndFileReg);
 	
	return TRUE;
}

BOOL CBldFileRegistry::RemoveRegistryHandle(BldFileRegHandle hndFileReg, IVCBuildErrorContext* pEC /* = NULL */)
{
	// If the item being destroyed is still in us then remove it.
	VCPOSITION pos = m_Contents.Find(hndFileReg);
	VSASSERT(NULL != pos, "Failed to find the frh we're trying to remove!");
	if (NULL == pos)
		return FALSE;

	// Notify dependents this handle is being destroyed.
	InformNotifyRxs(FRI_DESTROY, pEC, (void *)hndFileReg);

	// Is the CBldFileRegEntry a CBldFileRegSet? If so, then we must set up the associated file registry...
	CBldFileRegSet* pFileRegSet = (CBldFileRegSet *)hndFileReg;

	if (pFileRegSet->IsNodeType(nodetypeRegSet))
		pFileRegSet->SetFileRegistry(NULL);

	{
		// Prevent access to registry by other threads.
		CritSectionT cs(g_sectionRegistryFull);
		m_Contents.RemoveAt(pos);
	}

	if (pFileRegSet->IsNodeType(nodetypeRegFile))
	{
		// Remove from filters.
		UnFilterFile(hndFileReg);
	}

	// Release reference held by this file set.
	hndFileReg->ReleaseFRHRef();

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//
//	Instantiate the file registry

CBldFileRegistry g_FileRegistry;

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

int CSmallPtrSet::Find(void const * const pItem)
{
	for (int i = 0; i < m_nSize; i++)
	{
		if (m_pData[i] == pItem)
			return i;
	}

	return -1;
}

//
// Slow GetFileAttributesEx for operating systems that do not support it.
//
