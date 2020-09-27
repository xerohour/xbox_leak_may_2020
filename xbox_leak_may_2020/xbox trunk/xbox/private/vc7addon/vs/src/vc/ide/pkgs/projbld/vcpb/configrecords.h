#pragma once

#include <vccolls.h>
#include "FileRegistry.h"

// classes in this header
class CBldRecord;
	class CBldSolutionRecord;
	class CBldProjectRecord;
	class CBldCfgRecord;
class CBldConfigurationHolder;
class CBldConfigCacheEnabler;

class CBldRecord
{
public:
	CBldRecord(CBldRecord* pParentRecord, IDispatch* pChild);
	virtual ~CBldRecord();

	enum bldRecordType 
	{ 
		bldRecordUnknown, 
		bldRecordSolution, 
		bldRecordProject, 
		bldRecordCfg 
	};
	virtual bldRecordType GetRecordType() { return bldRecordUnknown; }
	virtual CVCMapPtrToPtr* GetPtrRecordMap() { return &m_pChildPtrRecordMap; }
	virtual CBldRecord* GetChildRecord(IDispatch* pChild, BOOL bCreateIfNeeded = TRUE);
	virtual CBldRecord* AddChildRecord(IDispatch* pChild);
	virtual void RemoveChildRecord(IDispatch* pChild);
	virtual IDispatch* GetCurrentObject() { return m_pCurrentObject; }	// warning: not ref-counted
	virtual CBldRecord* GetParentRecord() { return m_pParentRecord; }
	void Close();

protected:
	virtual CBldRecord* CreateChildRecord(IDispatch* pChild) { VSASSERT(FALSE, "CreateChildRecord must be overridden"); return NULL; }

protected:
	CVCMapPtrToPtr m_pChildPtrRecordMap;
	IDispatch* m_pCurrentObject;
	CBldRecord* m_pParentRecord;
};

class CBldSolutionRecord : public CBldRecord
{
public:
	CBldSolutionRecord() : CBldRecord(NULL, NULL) {}
	virtual ~CBldSolutionRecord() {}
	virtual bldRecordType GetRecordType() { return bldRecordSolution; }
	CBldProjectRecord* GetProjectRecord(IDispatch* pProject, BOOL bCreateIfNeeded = TRUE);
	CBldCfgRecord* GetCfgRecord(IDispatch* pProject, IDispatch* pCfg, BOOL bCreateIfNeeded = TRUE);
	void RemoveProjectRecord(IDispatch* pProject);
	void RemoveCfgRecord(IDispatch* pProject, IDispatch* pCfg);

protected:
	virtual CBldRecord* CreateChildRecord(IDispatch* pChild);
};

class CBldProjectRecord : public CBldRecord
{
public:
	CBldProjectRecord(CBldRecord* pSolutionRecord, IDispatch* pProject);
	virtual ~CBldProjectRecord() {}
	virtual bldRecordType GetRecordType() { return bldRecordProject; }
	CBldSolutionRecord* GetSolutionRecord();
	CBldCfgRecord* GetCfgRecord(IDispatch* pCfg, BOOL bCreateIfNeeded = TRUE);
	void RemoveCfgRecord(IDispatch* pCfg);

	VCProject* GetVCProject() { return m_pVCProject; }	// warning: not ref-counted

protected:
	virtual CBldRecord* CreateChildRecord(IDispatch* pChild);

protected:
	CComQIPtr<VCProject> m_pVCProject;
};

class CBldCfgRecord : public CBldRecord
{
public:
	CBldCfgRecord(CBldRecord* pProjectRecord, IDispatch* pConfig);
	virtual ~CBldCfgRecord() {}
	virtual bldRecordType GetRecordType() { return bldRecordCfg; }
	CBldSolutionRecord* GetSolutionRecord();
	CBldProjectRecord* GetProjectRecord();
	VCProject* GetVCProject();	// warning: not ref-counted

	VCConfiguration* GetVCConfiguration() { return m_pVCConfiguration; }		// warning: not ref-counted
	CVCPtrList* GetBaseToolList();

	// To turn caching on and off for all config record.  Begin cache
	// only if you know that nobody is setting any properties anywhere
	// (e.g. during build):
	static void	BeginConfigCache();
	static void EndConfigCache();

protected:
	CComQIPtr<VCConfiguration> m_pVCConfiguration;

private:
	static DWORD s_nSignature;
	static int s_nCongfigCacheCount;
};

class CBldConfigurationHolder
{
public:
	CBldConfigurationHolder(CBldCfgRecord* pcr, VCFileConfiguration* pFileCfg, VCConfiguration* pProjCfg);
	BldFileRegHandle GetFileRegHandle();
	void GetActionList(IVCBuildActionList** ppActions, BOOL bFileLevel = TRUE);
	VCFileConfiguration* GetFileCfg() { return m_pFileCfg; }
	VCConfiguration* GetProjCfg() { return m_pProjCfg; }
	CVCPtrList* GetToolList() { return m_pcr->GetBaseToolList(); }
	BOOL IsProject() { return m_bIsProject; }
	CBldCfgRecord* GetPCR() { return m_pcr; }

protected:
	CBldCfgRecord* m_pcr;
	VCFileConfiguration* m_pFileCfg;
	VCConfiguration* m_pProjCfg;
	BOOL m_bIsProject;
};

// Helper object for config caching.  Garentees it will be reanbled
// when object goes out of scope:
class CBldConfigCacheEnabler
{
	BOOL	m_bDidEnable;
public:
	CBldConfigCacheEnabler()  
	{
		m_bDidEnable = TRUE;
		CBldCfgRecord::BeginConfigCache();
	};
	~CBldConfigCacheEnabler() 
	{
		if (m_bDidEnable)
			CBldCfgRecord::EndConfigCache();
	};
	void EndCacheNow()
	{
		if (m_bDidEnable) 
		{
			CBldCfgRecord::EndConfigCache();
			m_bDidEnable = FALSE;
		}
	};
};

