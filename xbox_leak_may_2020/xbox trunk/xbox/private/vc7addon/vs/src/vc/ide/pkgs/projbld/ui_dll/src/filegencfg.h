// filegencfg.h
// declaration of the CFileGenCfg object

#ifndef __FILEGENCFG_H_
#define __FILEGENCFG_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "rsrc.h"       // main symbols
// CFileGenCfg
// 

class CFileGenCfg : 
	public CComObjectRoot,
	public IVCCfg,
	public IVsCfg,
	public ISpecifyPropertyPages
{
public:
	CFileGenCfg( void );
	~CFileGenCfg( void );
	
	void Initialize( VCFileConfiguration *pCfg );
	static HRESULT CreateInstance( IVsCfg **ppGenCfg, VCFileConfiguration* pFileCfg );

BEGIN_COM_MAP(CFileGenCfg)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IVsCfg)
	COM_INTERFACE_ENTRY(IVCCfg)
END_COM_MAP()

	// IVCCfg methods
	STDMETHOD(get_Tool)( VARIANT toolIndex, IUnknown **ppTool );
	STDMETHOD(get_Object)( IDispatch **ppTool );
	
	// IVsCfg
	STDMETHOD(get_DisplayName)( BSTR *pVal );
	STDMETHOD(get_IsDebugOnly)( BOOL *pVal );
	STDMETHOD(get_IsReleaseOnly)( BOOL *pVal );

	// ISpecifyPropertyPages methods
    STDMETHOD(GetPages)( /* [out] */ CAUUID *pPages );

	VCFileConfiguration* GetVCFileConfiguration(){ return (VCFileConfiguration*)m_pFileConfig; }
private:
	CComPtr<VCFileConfiguration> m_pFileConfig;
	
};

#endif // end include fence
