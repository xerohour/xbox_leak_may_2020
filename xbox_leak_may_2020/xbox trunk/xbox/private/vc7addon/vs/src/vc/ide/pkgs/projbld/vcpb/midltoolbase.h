// VCMidlTool.h: Definition of the CVCMidlTool class
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "vctool.h"
#include "settingspage.h"

template<class T, class IFace>
class ATL_NO_VTABLE CVCMidlBasePage :
	public IDispatchImpl<IFace, &(__uuidof(IFace)), &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber, 0, CVsTypeInfoHolder>,
	public CPageObjectImpl<T,VCMIDLTOOL_MIN_DISPID,VCMIDLTOOL_MAX_DISPID>,
	public CComObjectRoot
{
public:

BEGIN_COM_MAP(T)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IFace)
	COM_INTERFACE_ENTRY(IProvidePropertyBuilder)
END_COM_MAP()

	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<IFace, &__uuidof(IFace), &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}
};

class ATL_NO_VTABLE CVCMidlGeneralPage :
	public CVCMidlBasePage<CVCMidlGeneralPage, IVCMidlGeneralPage>
{
// IVCMidlPage
public:
	// general (/ms_ext, /c_ext left out because they're archaic); @response deliberately left out
	STDMETHOD(get_PreprocessorDefinitions)(BSTR* pbstrDefines);	// (/D![macro]) define macro for MIDL.  can have multiple.
	STDMETHOD(put_PreprocessorDefinitions)(BSTR bstrDefines);
	STDMETHOD(get_AdditionalIncludeDirectories)(BSTR* pbstrIncludePath);	// (/I![path]) directory to add to include path, may have multiple
	STDMETHOD(put_AdditionalIncludeDirectories)(BSTR bstrIncludePath);
	STDMETHOD(get_IgnoreStandardIncludePath)(enumMidlIgnoreStandardIncludePathBOOL* pbIgnore);	// (/no_def_idir) ignore current and INCLUDE path
	STDMETHOD(put_IgnoreStandardIncludePath)(enumMidlIgnoreStandardIncludePathBOOL bIgnore);
	STDMETHOD(get_MkTypLibCompatible)(enumMkTypLibCompatibleBOOL* pbCompatible);	// (/mktyplib203) forces compatibility with mktyplib.exe version 2.03
	STDMETHOD(put_MkTypLibCompatible)(enumMkTypLibCompatibleBOOL bCompatible);
	STDMETHOD(get_WarningLevel)(midlWarningLevelOption* poptSetting);	// see enum above (/W0 - /W4)
	STDMETHOD(put_WarningLevel)(midlWarningLevelOption optSetting);
	STDMETHOD(get_WarnAsError)(enumWarnAsErrorBOOL* pbWarnAsError);	// (/WX) treat warnings as errors
	STDMETHOD(put_WarnAsError)(enumWarnAsErrorBOOL bWarnAsError);
	STDMETHOD(get_SuppressStartupBanner)(enumSuppressStartupBannerBOOL* pbNoLogo);	// (/nologo) enable suppression of copyright message
	STDMETHOD(put_SuppressStartupBanner)(enumSuppressStartupBannerBOOL bNoLogo);
	STDMETHOD(get_DefaultCharType)(midlCharOption* poptSetting);	// see enum above (/char)
	STDMETHOD(put_DefaultCharType)(midlCharOption optSetting);
	STDMETHOD(get_TargetEnvironment)(midlTargetEnvironment* poptSetting);	// (/env win64 vs. /env win32) target Win64 (or Win32) environment
	STDMETHOD(put_TargetEnvironment)(midlTargetEnvironment optSetting);
	STDMETHOD(get_GenerateStublessProxies)(enumGenerateStublessProxiesBOOL* poptSetting);	// (/Oicf)
	STDMETHOD(put_GenerateStublessProxies)(enumGenerateStublessProxiesBOOL optSetting);

// helpers
public:
	virtual void GetBaseDefault(long id, CComVariant& varValue);
	virtual BOOL UseDirectoryPickerDialog(long id) { return (id == VCMIDLID_AdditionalIncludeDirectories); }
};

class ATL_NO_VTABLE CVCMidlOutputPage :
	public CVCMidlBasePage<CVCMidlOutputPage, IVCMidlOutputPage>
{
// IVCMidlOutputPage
public:
	// output file names
	STDMETHOD(get_OutputDirectory)(BSTR* pbstrOut);	// (/out dir) directory to place output files in; default: project directory
	STDMETHOD(put_OutputDirectory)(BSTR bstrOut);
	STDMETHOD(get_HeaderFileName)(BSTR* pbstrHeaderFile);	// (/h[eader] filename) specify name of header file generated; default: <idlfile>.h
	STDMETHOD(put_HeaderFileName)(BSTR bstrHeaderFileName);
	STDMETHOD(get_DLLDataFileName)(BSTR* pbstrDLLData);	// (/dlldata filename) specify name of DLLDATA file generated; default: dlldata.c
	STDMETHOD(put_DLLDataFileName)(BSTR bstrDLLData);
	STDMETHOD(get_InterfaceIdentifierFileName)(BSTR* pbstrIID);	// (/iid filename) specify name of IID file; default: <idlfile>_i.c
	STDMETHOD(put_InterfaceIdentifierFileName)(BSTR bstrIID);
	STDMETHOD(get_ProxyFileName)(BSTR* pbstrProxyFile);	// (/proxy filename) specify name of proxy file; default: <idlfile>_p.c
	STDMETHOD(put_ProxyFileName)(BSTR bstrProxyFile);
	STDMETHOD(get_GenerateTypeLibrary)(enumGenerateTypeLibraryBOOL* poptSetting);	// (/notlb for no) generate a type library or not; default: yes
	STDMETHOD(put_GenerateTypeLibrary)(enumGenerateTypeLibraryBOOL optSetting);
	STDMETHOD(get_TypeLibraryName)(BSTR* pbstrTLBFile);	// (/tlb filename) specify name of TLB file; default: <idlfile>.tlb
	STDMETHOD(put_TypeLibraryName)(BSTR bstrTLBFile);

// helpers
public:
	virtual void GetBaseDefault(long id, CComVariant& varValue);
};

class ATL_NO_VTABLE CVCMidlAdvancedPage :
	public CVCMidlBasePage<CVCMidlAdvancedPage, IVCMidlAdvancedPage>
{
// IVCMidlAdvancedPage
public:
	// advanced (/nocpp ignored as it is not in docs)
	STDMETHOD(get_EnableErrorChecks)(midlErrorCheckOption* poptSetting);	// see enum above (/error {none|all}) 
	STDMETHOD(put_EnableErrorChecks)(midlErrorCheckOption optSetting);
	STDMETHOD(get_ErrorCheckAllocations)(enumErrorCheckAllocationsBOOL* pbErrorCheck);	// (/error allocation) check for out of memory errors
	STDMETHOD(put_ErrorCheckAllocations)(enumErrorCheckAllocationsBOOL bErrorCheck);
	STDMETHOD(get_ErrorCheckBounds)(enumErrorCheckBoundsBOOL* pbErrorCheck);	// (/error bounds_check) check size	vs. transmission length specifications
	STDMETHOD(put_ErrorCheckBounds)(enumErrorCheckBoundsBOOL bErrorCheck);
	STDMETHOD(get_ErrorCheckEnumRange)(enumErrorCheckEnumRangeBOOL* pbErrorCheck);	// (/error enum) check enum values to be in allowable range; not in docs
	STDMETHOD(put_ErrorCheckEnumRange)(enumErrorCheckEnumRangeBOOL bErrorCheck);
	STDMETHOD(get_ErrorCheckRefPointers)(enumErrorCheckRefPointersBOOL* pbErrorCheck);	// (/error ref) check ref pointers to be non-NULL
	STDMETHOD(put_ErrorCheckRefPointers)(enumErrorCheckRefPointersBOOL bErrorCheck);
	STDMETHOD(get_ErrorCheckStubData)(enumErrorCheckStubDataBOOL* pbErrorCheck);	// (/error stub_data) emit additional check for server side data stub validity
	STDMETHOD(put_ErrorCheckStubData)(enumErrorCheckStubDataBOOL bErrorCheck);
	STDMETHOD(get_ValidateParameters)(enumValidateParametersBOOL* pbValidate);	// (/robust) generate additional information to validate parameters; not in docs
	STDMETHOD(put_ValidateParameters)(enumValidateParametersBOOL bValidate);
	STDMETHOD(get_StructMemberAlignment)(midlStructMemberAlignOption* poptSetting);	// see enum above (/Zp[num])
	STDMETHOD(put_StructMemberAlignment)(midlStructMemberAlignOption optSetting);
	STDMETHOD(get_RedirectOutputAndErrors)(BSTR* pbstrOutput);	// (/o file) redirect output from screen to a file
	STDMETHOD(put_RedirectOutputAndErrors)(BSTR bstrOutput);
	STDMETHOD(get_CPreprocessOptions)(BSTR* pbstrOpt);	// (/cpp_opt"[option]") C-compiler preprocessor option to pass to MIDL.  must be in exact format for desired switches
	STDMETHOD(put_CPreprocessOptions)(BSTR bstrOpt);
	STDMETHOD(get_UndefinePreprocessorDefinitions)(BSTR* pbstrUndefines);	// (/U![macro]) remove any previous definition of symbol.  can have multiple.
	STDMETHOD(put_UndefinePreprocessorDefinitions)(BSTR bstrUndefines);
};
