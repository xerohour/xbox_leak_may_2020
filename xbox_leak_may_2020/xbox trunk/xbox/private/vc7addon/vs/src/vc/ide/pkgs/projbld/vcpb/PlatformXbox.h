// Platform.h: Definition of the CBasePlatform, CPlatformWin32, and CPlatformWin64 classes
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "vccolls.h"
#include "comlist.h"
#include "collection.h"
#include "platform.h"

/////////////////////////////////////////////////////////////////////////////
// CPlatformXbox

class CPlatformXbox : 
	public CBasePlatform<CPlatformXbox, &CLSID_VCPlatformXbox>
{
public:
DECLARE_VS_REGISTRY_RESOURCEID(IDR_PLATFORM_XBOX)

// VCPlatform
public:
	STDMETHOD(get_Name)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(MatchName)(BSTR bstrNameToMatch, VARIANT_BOOL bFullOnly, VARIANT_BOOL *pbMatched);

// IVCPlatformImpl
	STDMETHOD(SaveObject)(IStream *xml, IVCPropertyContainer *pPropCnt, long nIndent);
	STDMETHOD(Initialize)( void ); 	
	STDMETHOD(IsToolInToolset)(toolSetType listStyle, IVCToolImpl* pTool, VARIANT_BOOL* pbInToolset);

protected:
	static CVCStringWList		s_strToolsExcludeList[5];		// tools excluded for the tool type
};
