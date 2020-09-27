// CLTool.h : Declaration of the CCLTool

#pragma once

#include "vctool.h"
#include "settingspage.h"

template<class T, class IFace>
class ATL_NO_VTABLE CVCCLCompilerBasePage :
	public IDispatchImpl<IFace, &(__uuidof(IFace)), &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber, 0, CVsTypeInfoHolder>,
	public CPageObjectImpl<T,VCCLCOMPILERTOOL_MIN_DISPID,VCCLCOMPILERTOOL_MAX_DISPID>,
	public CComObjectRoot
{
public:

BEGIN_COM_MAP(T)
	COM_INTERFACE_ENTRY(IPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IVsPerPropertyBrowsing)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IFace)
	COM_INTERFACE_ENTRY(IVCPropertyContainer)
	COM_INTERFACE_ENTRY(IProvidePropertyBuilder)
END_COM_MAP()

	// IDispatch override
	STDMETHOD(Invoke)( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pdispParams, VARIANT *pvarResult, EXCEPINFO *pexcepInfo, UINT *puArgErr )
	{
		IDispatchImpl<IFace, &(__uuidof(IFace)), &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>::Invoke( dispid, riid, lcid, wFlags, pdispParams, pvarResult, pexcepInfo, puArgErr );
		return S_OK;
	}
};
	
class ATL_NO_VTABLE CVCCLCompilerOptimizationPage :
	public CVCCLCompilerBasePage<CVCCLCompilerOptimizationPage, IVCCLCompilerOptimizationPage>
{
// IVCCLCompilerOptimizationPage
public:
	STDMETHOD(get_Optimization)(optimizeOption* poptSetting);	// see enum above (/O1, /O2, /Od, /Ox)
	STDMETHOD(put_Optimization)(optimizeOption optSetting);
	STDMETHOD(get_GlobalOptimizations)(enumGlobalOptimizationsBOOL* pbGlobalOpt);	// (/Og, /Og-) enable/disable global optimizations; incompatible with all RTC options, GZ, ZI, EEf
	STDMETHOD(put_GlobalOptimizations)(enumGlobalOptimizationsBOOL bGlobalOpt);
	STDMETHOD(get_InlineFunctionExpansion)(inlineExpansionOption* poptSetting);	// see enum above (/Ob0, /Ob1, /Ob2)
	STDMETHOD(put_InlineFunctionExpansion)(inlineExpansionOption optSetting);
	STDMETHOD(get_EnableIntrinsicFunctions)(enumEnableIntrinsicFunctionsBOOL* pbEnableIntrinsic);	// (/Oi, /Oi-) enable/disable intrinsic functions; works best with /Og
	STDMETHOD(put_EnableIntrinsicFunctions)(enumEnableIntrinsicFunctionsBOOL bEnableIntrinsic);
	STDMETHOD(get_ImproveFloatingPointConsistency)(enumImproveFloatingPointConsistencyBOOL* pbImproveFloat);	// (/Op, /Op-) (don't) improve floating-point consistency
	STDMETHOD(put_ImproveFloatingPointConsistency)(enumImproveFloatingPointConsistencyBOOL bImproveFloat);
	STDMETHOD(get_FavorSizeOrSpeed)(favorSizeOrSpeedOption* poptSetting);	// (/Os, Ot) favor size/speed; works best with /Og
	STDMETHOD(put_FavorSizeOrSpeed)(favorSizeOrSpeedOption optSetting);
	STDMETHOD(get_OmitFramePointers)(enumOmitFramePointersBOOL* poptSetting);	// (/Oy, Oy-) enable/disable frame pointer omission
	STDMETHOD(put_OmitFramePointers)(enumOmitFramePointersBOOL optSetting);
	STDMETHOD(get_EnableFiberSafeOptimizations)(enumEnableFiberSafeOptimizationsBOOL* pbFiberSafeTLS);	// (/GT) generate fiber-safe TLS accesses
	STDMETHOD(put_EnableFiberSafeOptimizations)(enumEnableFiberSafeOptimizationsBOOL bFiberSafeTLS);
	STDMETHOD(get_OptimizeForProcessor)(ProcessorOptimizeOption* poptSetting);	// see ProcessorOptimizeOption enum (/G5, /G6, /GB)
	STDMETHOD(put_OptimizeForProcessor)(ProcessorOptimizeOption optSetting);
	STDMETHOD(get_OptimizeForWindowsApplication)(enumOptimizeForWindowsApplicationBOOL* poptSetting); // (/GA) optimize for Windows app
	STDMETHOD(put_OptimizeForWindowsApplication)(enumOptimizeForWindowsApplicationBOOL optSetting);

// IVsPerPropertyBrowsing
public:
	STDMETHOD(IsPropertyReadOnly)( DISPID dispid, BOOL *fReadOnly);
};

class ATL_NO_VTABLE CVCCLCompilerPreProcessorPage :
	public CVCCLCompilerBasePage<CVCCLCompilerPreProcessorPage, IVCCLCompilerPreProcessorPage>
{
// IVCCLCompilerPreProcessorPage
public:
	STDMETHOD(get_PreprocessorDefinitions)(BSTR* pbstrDefines);		// (/D[name]) defines, can have multiple
	STDMETHOD(put_PreprocessorDefinitions)(BSTR bstrDefines);
	STDMETHOD(get_IgnoreStandardIncludePath)(enumIgnoreStandardIncludePathBOOL* pbIgnoreInclPath);	// (/X) ignore standard include path
	STDMETHOD(put_IgnoreStandardIncludePath)(enumIgnoreStandardIncludePathBOOL bIgnoreInclPath);
	STDMETHOD(get_GeneratePreprocessedFile)(preprocessOption* poptSetting);	// see preprocessOption enum (/P, /EP /P)
	STDMETHOD(put_GeneratePreprocessedFile)(preprocessOption optSetting);
	STDMETHOD(get_KeepComments)(enumKeepCommentsBOOL* pbkeepComments);	// (/C) don't strip comments, requires one of /E, /EP, /P switches
	STDMETHOD(put_KeepComments)(enumKeepCommentsBOOL bkeepComments);
};

class ATL_NO_VTABLE CVCCLCompilerAdvancedPage :
	public CVCCLCompilerBasePage<CVCCLCompilerAdvancedPage, IVCCLCompilerAdvancedPage>
{
// IVCCLCompilerAdvancedPage
public:
	STDMETHOD(get_CallingConvention)(callingConventionOption* poptSetting);	// see callingConventionOption enum (/Gd, /Gr, /Gz)
	STDMETHOD(put_CallingConvention)(callingConventionOption optSetting);
	STDMETHOD(get_CompileAs)(CompileAsOptions* pcompileAs);	// see enum above (/TC, /TP)
	STDMETHOD(put_CompileAs)(CompileAsOptions compileAs);
	STDMETHOD(get_DisableSpecificWarnings)(BSTR* pbstrDisableSpecificWarnings);	// (/wd<num>) disable specific warnings; multi-prop
	STDMETHOD(put_DisableSpecificWarnings)(BSTR bstrDisableSpecificWarnings);
	STDMETHOD(get_ForcedIncludeFiles)(BSTR* pbstrName);	// (/FI![name]) name forced include file, can have multiple
	STDMETHOD(put_ForcedIncludeFiles)(BSTR bstrName);
	STDMETHOD(get_ForcedUsingFiles)(BSTR* pbstrName);	// (/FU![name]) name forced #using file, can have multiple
	STDMETHOD(put_ForcedUsingFiles)(BSTR bstrName);
	STDMETHOD(get_ShowIncludes)(enumShowIncludesBOOL* pbShowInc);	// (/showIncludes)
	STDMETHOD(put_ShowIncludes)(enumShowIncludesBOOL bShowInc);
	STDMETHOD(get_UndefinePreprocessorDefinitions)(BSTR* pbstrUndefines);	// (/U[name]) undefine predefined macro, can have multiple
	STDMETHOD(put_UndefinePreprocessorDefinitions)(BSTR bstrUndefines);	
	STDMETHOD(get_UndefineAllPreprocessorDefinitions)(enumUndefineAllPreprocessorDefinitionsBOOL* pbPredefinedMacros);	// (/u) undefine all predefined macros 
	STDMETHOD(put_UndefineAllPreprocessorDefinitions)(enumUndefineAllPreprocessorDefinitionsBOOL bPredefinedMacros);
};

class ATL_NO_VTABLE CVCCLCompilerCodeGenPage :
	public CVCCLCompilerBasePage<CVCCLCompilerCodeGenPage, IVCCLCompilerCodeGenPage>
{
// IVCCLCompilerCodeGenPage
public:
	STDMETHOD(get_StringPooling)(enumStringPoolingBOOL* pbPool);	// (/GF) enable read-only string pooling
	STDMETHOD(put_StringPooling)(enumStringPoolingBOOL bPool);
	STDMETHOD(get_MinimalRebuild)(enumMinimalRebuildBOOL* pbMinimalRebuild);	// (/Gm, /FD) enable minimal rebuild vs. generate dependencies, /Gm requires /ZI or /Zi
	STDMETHOD(put_MinimalRebuild)(enumMinimalRebuildBOOL bMinimalRebuild);
	STDMETHOD(get_ExceptionHandling)(enumExceptionHandlingBOOL* poptSetting);	// /EHsc
	STDMETHOD(put_ExceptionHandling)(enumExceptionHandlingBOOL optSetting);
	STDMETHOD(get_BasicRuntimeChecks)(basicRuntimeCheckOption* poptSetting);	// /RTCs, /RTCu, /RTC1
	STDMETHOD(put_BasicRuntimeChecks)(basicRuntimeCheckOption optSetting);
	STDMETHOD(get_SmallerTypeCheck)(enumSmallerTypeCheckBOOL* pbSmallerType);	// (/RTCc) convert to smaller types check, incompatible with Og, O1, O2, Ox
	STDMETHOD(put_SmallerTypeCheck)(enumSmallerTypeCheckBOOL bSmallerType);
	STDMETHOD(get_RuntimeLibrary)(runtimeLibraryOption* poptSetting);	// see runtimeLibraryOption enum (/MD, /MDd, /ML, /MLd, /MT, /MTd)
	STDMETHOD(put_RuntimeLibrary)(runtimeLibraryOption optSetting);
	STDMETHOD(get_StructMemberAlignment)(structMemberAlignOption* poptSetting);	// see structMemberAlignOption enum (/Zp[num])
	STDMETHOD(put_StructMemberAlignment)(structMemberAlignOption optSetting);
	STDMETHOD(get_BufferSecurityCheck)(enumBufferSecurityCheckBOOL* bSecure);	// (/GS) enable buffer overrun checks; buffer security from hackers
	STDMETHOD(put_BufferSecurityCheck)(enumBufferSecurityCheckBOOL bSecure);
	STDMETHOD(get_EnableFunctionLevelLinking)(enumEnableFunctionLevelLinkingBOOL* pbPackage);	// (/Gy) enable function level linking (no explicit off)
	STDMETHOD(put_EnableFunctionLevelLinking)(enumEnableFunctionLevelLinkingBOOL bPackage);

// helpers
public:
	virtual void GetBaseDefault(long id, CComVariant& varValue);
};


class ATL_NO_VTABLE CVCCLCompilerLanguagePage :
	public CVCCLCompilerBasePage<CVCCLCompilerLanguagePage, IVCCLCompilerLanguagePage>
{
// IVCCLCompilerLanguagePage
public:
	STDMETHOD(get_DisableLanguageExtensions)(enumDisableLanguageExtensionsBOOL* pbDisableExtensions);	// (/Za, /Ze) disable/enable language extensions (/Ze default)
	STDMETHOD(put_DisableLanguageExtensions)(enumDisableLanguageExtensionsBOOL bDisableExtensions);
	STDMETHOD(get_DefaultCharIsUnsigned)(enumDefaultCharIsUnsignedBOOL* pbIsUnsigned);	// (/J) default char type is unsigned
	STDMETHOD(put_DefaultCharIsUnsigned)(enumDefaultCharIsUnsignedBOOL bIsUnsigned);
	STDMETHOD(get_TreatWChar_tAsBuiltInType)(enumTreatWChar_tAsBuiltInTypeBOOL* pbBuiltInType);
	STDMETHOD(put_TreatWChar_tAsBuiltInType)(enumTreatWChar_tAsBuiltInTypeBOOL bBuiltInType);
	STDMETHOD(get_ForceConformanceInForLoopScope)(enumForceConformanceInForLoopScopeBOOL* pbConform);	// (/Zc:forScope) check for conformance of vars for 'for' scope
	STDMETHOD(put_ForceConformanceInForLoopScope)(enumForceConformanceInForLoopScopeBOOL bConform);
	STDMETHOD(get_RuntimeTypeInfo)(enumRTTIBOOL* pbRTTI);	// (/GR, /GR-) enable/disable C++ RTTI
	STDMETHOD(put_RuntimeTypeInfo)(enumRTTIBOOL bRTTI);
};

class ATL_NO_VTABLE CVCCLCompilerPCHPage :
	public CVCCLCompilerBasePage<CVCCLCompilerPCHPage, IVCCLCompilerPCHPage>
{
// IVCCLCompilerPCHPage
public:
	STDMETHOD(get_UsePrecompiledHeader)(pchOption* poptSetting);	// see enum above (/Yc, /YX, /Yu)
	STDMETHOD(put_UsePrecompiledHeader)(pchOption optSetting);
	STDMETHOD(get_PrecompiledHeaderThrough)(BSTR* pbstrFile);	// optional argument to UsePrecompiledHeader property specifying file to use to generate/use PCH
	STDMETHOD(put_PrecompiledHeaderThrough)(BSTR bstrFile);
	STDMETHOD(get_PrecompiledHeaderFile)(BSTR* pbstrPCH);	// (/Fp[name]) name the precompiled header file, can be directory location or leave off .pch extension
	STDMETHOD(put_PrecompiledHeaderFile)(BSTR bstrPCH);

// helpers
public:
	virtual void GetBaseDefault(long id, CComVariant& varValue);
};

class ATL_NO_VTABLE CVCCLCompilerOutputPage :
	public CVCCLCompilerBasePage<CVCCLCompilerOutputPage, IVCCLCompilerOutputPage>
{
// IVCCLCompilerOutputPage
public:
	STDMETHOD(get_ExpandAttributedSource)(enumExpandAttributedSourceBOOL* pbExpandAttributedSource);	// (/Fx) expand attributed code into source file listing
	STDMETHOD(put_ExpandAttributedSource)(enumExpandAttributedSourceBOOL pExpandAttributedSource);
	STDMETHOD(get_AssemblerOutput)(asmListingOption* poptSetting);	// see enum above (/FA, /FAc, /FAs, /FAcs)
	STDMETHOD(put_AssemblerOutput)(asmListingOption optSetting);
	STDMETHOD(get_AssemblerListingLocation)(BSTR* pbstrName);	// (/Fa[name]) specify directory and/or name for listing file from AssemblerOutput property
	STDMETHOD(put_AssemblerListingLocation)(BSTR bstrName);
	STDMETHOD(get_ObjectFile)(BSTR* pbstrName);	// (/Fo[name]) name obj file
	STDMETHOD(put_ObjectFile)(BSTR bstrName);
	STDMETHOD(get_ProgramDataBaseFileName)(BSTR* pbstrName);	// (/Fd[file]) name PDB file.  can be directory name
	STDMETHOD(put_ProgramDataBaseFileName)(BSTR bstrName);

// helpers
public:
	virtual void GetBaseDefault(long id, CComVariant& varValue);
};

class ATL_NO_VTABLE CVCCLCompilerBrowsePage :
	public CVCCLCompilerBasePage<CVCCLCompilerBrowsePage, IVCCLCompilerBrowsePage>
{
// IVCCLCompilerBrowsePage
public:
	STDMETHOD(get_BrowseInformation)(browseInfoOption* poptSetting);	// see enum above, (/FR[name], /Fr[name])
	STDMETHOD(put_BrowseInformation)(browseInfoOption optSetting);
	STDMETHOD(get_BrowseInformationFile)(BSTR* pbstrFile);		// optional name to go with BrowseInformation property
	STDMETHOD(put_BrowseInformationFile)(BSTR bstrFile);

// helpers
public:
	virtual void GetBaseDefault(long id, CComVariant& varValue);
};

class ATL_NO_VTABLE CVCCLCompilerGeneralPage :
	public CVCCLCompilerBasePage<CVCCLCompilerGeneralPage, IVCCLCompilerGeneralPage>
{
// IVCCLCompilerGeneralPage
public:
	STDMETHOD(get_AdditionalIncludeDirectories)(BSTR* pbstrIncludePath);	// (/I![path]) directory to add to include path, may have multiple
	STDMETHOD(put_AdditionalIncludeDirectories)(BSTR bstrIncludePath);
	STDMETHOD(get_AdditionalUsingDirectories)(BSTR* pbstrIncludePath);	// (/AI![path]) directory to add to LIBPATH path, may have multiple
	STDMETHOD(put_AdditionalUsingDirectories)(BSTR bstrIncludePath);
	STDMETHOD(get_DebugInformationFormat)(debugOption* poptSetting);	// see debugOption enum (/Z7, Zd, /ZI, /Zi)
	STDMETHOD(put_DebugInformationFormat)(debugOption optSetting);
	STDMETHOD(get_CompileAsManaged)(compileAsManagedOptions* poptSetting);	// (/clr[:noAssembly])
	STDMETHOD(put_CompileAsManaged)(compileAsManagedOptions optSetting);
	STDMETHOD(get_SuppressStartupBanner)(enumSuppressStartupBannerBOOL* pbNoLogo);	// (/nologo, /nologo-) enable/disable suppression of copyright message
	STDMETHOD(put_SuppressStartupBanner)(enumSuppressStartupBannerBOOL bNoLogo);
	STDMETHOD(get_WarningLevel)(warningLevelOption* poptSetting);	// see warningLevelOption enum (/W0 - /W4)
	STDMETHOD(put_WarningLevel)(warningLevelOption optSetting);
	STDMETHOD(get_Detect64BitPortabilityProblems)(enumDetect64BitPortabilityProblemsBOOL* pbDetect64BitPortabilityProblems);	// (/Wp64) detect 64 bit portability problems
	STDMETHOD(put_Detect64BitPortabilityProblems)(enumDetect64BitPortabilityProblemsBOOL bDetect64BitPortabilityProblems);
	STDMETHOD(get_WarnAsError)(enumWarnAsErrorBOOL* pbWarnAsError);	// (/WX, /WX-) treat warnings as errors
	STDMETHOD(put_WarnAsError)(enumWarnAsErrorBOOL bWarnAsError);

// helpers
public:
	virtual void GetBaseDefault(long id, CComVariant& varValue);
	virtual BOOL UseDirectoryPickerDialog(long id) 
		{ return (id == VCCLID_AdditionalIncludeDirectories || id == VCCLID_AdditionalUsingDirectories); }
};

