#include "stdafx.h"
#include "ConfigRecords.h"
#include "BuildEngine.h"

CBldRecord::CBldRecord(CBldRecord* pParentRecord, IDispatch* pCurrent)
{
	m_pParentRecord = pParentRecord;
	if (pCurrent)
		m_pCurrentObject = pCurrent;
}

CBldRecord::~CBldRecord()
{
	Close();
}

void CBldRecord::Close()
{
	VCPOSITION pos = m_pChildPtrRecordMap.GetStartPosition();
	while (pos != NULL)
	{
		CBldRecord* pChildRecord;
		IDispatch* pChildObject;
		m_pChildPtrRecordMap.GetNextAssoc(pos, (void *&)pChildObject, (void *&)pChildRecord);
		delete pChildRecord;
	}
	m_pChildPtrRecordMap.RemoveAll();
}

CBldRecord* CBldRecord::GetChildRecord(IDispatch* pChild, BOOL bCreateIfNeeded /* = TRUE */)
{
	CBldRecord* pChildRecord = NULL;
	if (m_pChildPtrRecordMap.Lookup((void *)pChild, (void*&)pChildRecord) && pChildRecord)
		return pChildRecord;

	if (!bCreateIfNeeded)
		return NULL;

	return AddChildRecord(pChild);
}

CBldRecord* CBldRecord::AddChildRecord(IDispatch* pChild)
{
	VSASSERT(pChild != NULL, "Do not add NULL child record to BldRecord.  Bad programmer, bad programmer.");
	RETURN_ON_NULL2(pChild, NULL);

	CBldRecord* pChildRecord = NULL;

	VSASSERT(!m_pChildPtrRecordMap.Lookup((void *)pChild, (void*&)pChildRecord), "Do not try to add same child record to BldRecord twice.");

	pChildRecord = CreateChildRecord(pChild);
	if (pChildRecord == NULL)
	{
		VSASSERT(FALSE, "Failed to create child record for BldRecord");
		return NULL;
	}

	m_pChildPtrRecordMap.SetAt((void *)pChild, (void *&)pChildRecord);

	return pChildRecord;
}

void CBldRecord::RemoveChildRecord(IDispatch* pChild)
{
	VSASSERT(pChild, "Must specify child to remove.  NULL not valid.");
	if (pChild == NULL)
		return;

	CBldRecord* pChildRecord = NULL;
	if (!m_pChildPtrRecordMap.Lookup((void *)pChild, (void *&)pChildRecord) || !pChildRecord)
		return;		// nothing to do

	delete pChildRecord;
	m_pChildPtrRecordMap.RemoveKey(pChild);
}

CBldProjectRecord* CBldSolutionRecord::GetProjectRecord(IDispatch* pProject, BOOL bCreateIfNeeded /* = TRUE */) 
{ 
	return (CBldProjectRecord*)GetChildRecord(pProject, bCreateIfNeeded); 
}

CBldCfgRecord* CBldSolutionRecord::GetCfgRecord(IDispatch* pProject, IDispatch* pCfg, BOOL bCreateIfNeeded /* = TRUE */)
{
	CBldProjectRecord* pProjectRecord = GetProjectRecord(pProject, bCreateIfNeeded);
	RETURN_ON_NULL2(pProjectRecord, NULL);

	return (CBldCfgRecord*)(pProjectRecord->GetChildRecord(pCfg, bCreateIfNeeded)); 
}

CBldRecord* CBldSolutionRecord::CreateChildRecord(IDispatch* pChild)
{
	return new CBldProjectRecord(this, pChild);
}

void CBldSolutionRecord::RemoveProjectRecord(IDispatch* pProject)
{
	RemoveChildRecord(pProject);
}
void CBldSolutionRecord::RemoveCfgRecord(IDispatch* pProject, IDispatch* pCfg)
{
	CBldProjectRecord* pProjectRecord = GetProjectRecord(pProject, FALSE);
	if (pProjectRecord == NULL)
		return;

	pProjectRecord->RemoveCfgRecord(pCfg);
}

CBldProjectRecord::CBldProjectRecord(CBldRecord* pSolutionRecord, IDispatch* pProject) 
	: CBldRecord(pSolutionRecord, pProject)
{
	VSASSERT(pSolutionRecord && pProject, "Cannot create a ProjectRecord without valid solution and project info");
	m_pVCProject = pProject;
}

CBldRecord* CBldProjectRecord::CreateChildRecord(IDispatch* pChild)
{
	return new CBldCfgRecord(this, pChild);
}

CBldSolutionRecord* CBldProjectRecord::GetSolutionRecord()
{ 
	return (CBldSolutionRecord*)GetParentRecord();
}

CBldCfgRecord* CBldProjectRecord::GetCfgRecord(IDispatch* pCfg, BOOL bCreateIfNeeded /* = TRUE */)
{ 
	return (CBldCfgRecord*)(GetChildRecord(pCfg, bCreateIfNeeded)); 
}

void CBldProjectRecord::RemoveCfgRecord(IDispatch* pCfg)
{
	RemoveChildRecord(pCfg);
}

DWORD CBldCfgRecord::s_nSignature = 0;
int CBldCfgRecord::s_nCongfigCacheCount = 0;

CBldCfgRecord::CBldCfgRecord(CBldRecord* pProjectRecord, IDispatch* pCfg)
	: CBldRecord(pProjectRecord, pCfg)
{
	VSASSERT(pProjectRecord && pCfg, "Cannot create a CfgRecord without a valid ProjectRecord and config");
	m_pVCConfiguration = pCfg;
}

CBldSolutionRecord* CBldCfgRecord::GetSolutionRecord()
{ 
	return ((CBldProjectRecord*)GetParentRecord())->GetSolutionRecord();
}

CBldProjectRecord* CBldCfgRecord::GetProjectRecord()
{ 
	return (CBldProjectRecord*)GetParentRecord(); 
}

CVCPtrList* CBldCfgRecord::GetBaseToolList()
{
	CComQIPtr<IVCConfigurationImpl> spProjCfgImpl = m_pVCConfiguration;
	CVCPtrList* pPtrList = NULL;
	if ((spProjCfgImpl == NULL) || FAILED(spProjCfgImpl->get_BaseToolList((void **)&pPtrList)))
	{
		VSASSERT(FALSE, "Failed to get base tool list in CfgRecord.  Deep trouble.");
		pPtrList = NULL;
	}
	return pPtrList;
}

VCProject* CBldCfgRecord::GetVCProject()	// warning: not ref-counted
{
	CBldProjectRecord* pProjRecord = GetProjectRecord();
	VSASSERT(pProjRecord != NULL, "Managed to create a CfgRecord without a valid ProjectRecord.  Bad program, bad program.");
	RETURN_ON_NULL2(pProjRecord, NULL);

	return pProjRecord->GetVCProject();
}

void CBldCfgRecord::BeginConfigCache()
{
	if (s_nCongfigCacheCount == 0) s_nSignature++;
	s_nCongfigCacheCount++;

}
void CBldCfgRecord::EndConfigCache()
{
	s_nCongfigCacheCount--;
}
	  
CBldConfigurationHolder::CBldConfigurationHolder(CBldCfgRecord* pcr, VCFileConfiguration* pFileCfg, 
	VCConfiguration* pProjCfg)
{
	m_pcr = pcr;
	m_pFileCfg = pFileCfg;
	m_pProjCfg = pProjCfg;
	m_bIsProject = (m_pFileCfg == NULL);
}

BldFileRegHandle CBldConfigurationHolder::GetFileRegHandle()
{
	BldFileRegHandle frh;

	CComQIPtr<IVCBuildableItem> spBuildableItem;
	if (m_pFileCfg == NULL)
		spBuildableItem = m_pProjCfg;
	else
		spBuildableItem = m_pFileCfg;
	VSASSERT(spBuildableItem != NULL, "Managed to create a ConfigurationHolder without a valid item to attach it to.");
	RETURN_ON_NULL2(spBuildableItem, NULL);

	spBuildableItem->get_FileRegHandle((void **)&frh);
	return frh;
}

void CBldConfigurationHolder::GetActionList(IVCBuildActionList** ppActions, BOOL bFileLevel /* = FALSE */)
{
	CComQIPtr<IVCBuildableItem> spBuildableItem;
	if (m_pFileCfg == NULL)
		spBuildableItem = m_pProjCfg;
	else
		spBuildableItem = m_pFileCfg;
	VSASSERT(spBuildableItem != NULL, "Managed to create a ConfigurationHolder without a valid item to attach it to.");
	if (spBuildableItem == NULL)
		return;

	*ppActions = NULL;
	spBuildableItem->get_ActionList(ppActions);
	VSASSERT(*ppActions != NULL, "No action list associated with the ConfigurationHolder's item.  Bad initialization of the action.");
}

