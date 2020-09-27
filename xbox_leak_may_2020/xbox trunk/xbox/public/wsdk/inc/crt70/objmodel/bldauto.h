// Microsoft Visual Studio Object Model
// Copyright (C) 1996-1997 Microsoft Corporation
// All rights reserved.

/////////////////////////////////////////////////////////////////////////////
// bldauto.h

// Declaration of dual interfaces to objects found in the type library
//  VISUAL STUDIO 97 PROJECT SYSTEM (SharedIDE\bin\ide\devbld.pkg)

#ifndef __BLDAUTO_H__
#define __BLDAUTO_H__

#include "appauto.h"
#include "blddefs.h"

#ifndef BEGIN_INTERFACE
#define BEGIN_INTERFACE
#endif

/////////////////////////////////////////////////////////////////////////////
// Interfaces declared in this file:

// IGenericProject
	interface IBuildProject;

interface IConfiguration;
interface IConfigurations;


interface IBuildItems;
interface IBuildItem;

/////////////////////////////////////////////////////////////////////////
// BuildProject Object

// IBuildProject interface

#undef INTERFACE
#define INTERFACE IBuildProject

DECLARE_INTERFACE_(IBuildProject, IGenericProject)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;

    /* IGenericProject methods */
    STDMETHOD(get_Name)(THIS_ BSTR FAR* Name) PURE;
    STDMETHOD(get_FullName)(THIS_ BSTR FAR* Name) PURE;
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application) PURE;
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* Parent) PURE;
    STDMETHOD(get_Type)(THIS_ BSTR FAR* pType) PURE;
    STDMETHOD(Reserved1)(THIS) PURE;
    STDMETHOD(Reserved2)(THIS) PURE;
    STDMETHOD(Reserved3)(THIS) PURE;
    STDMETHOD(Reserved4)(THIS) PURE;
    STDMETHOD(Reserved5)(THIS) PURE;
    STDMETHOD(Reserved6)(THIS) PURE;
    STDMETHOD(Reserved7)(THIS) PURE;
    STDMETHOD(Reserved8)(THIS) PURE;
    STDMETHOD(Reserved9)(THIS) PURE;
    STDMETHOD(Reserved10)(THIS) PURE;
#endif

	/* IBuildProject methods */
	STDMETHOD(get_Configurations)(THIS_ IConfigurations FAR* FAR* Configurations) PURE;
	STDMETHOD(AddFile)(THIS_ BSTR szFile, VARIANT Reserved) PURE;
	STDMETHOD(AddConfiguration)(THIS_ BSTR szConfig, VARIANT varBstrPlatform) PURE;
	STDMETHOD(get_BuildItems)(THIS_ IDispatch * FAR* BuildItems) PURE;
	STDMETHOD(AddFolder)(THIS_ BSTR bstrName, BSTR bstrFolderExt, VARIANT Reserved) PURE;
	STDMETHOD(AddProjectDependency)(THIS_ VARIANT varProject) PURE;
	STDMETHOD(ExportMakefile)(THIS_ VARIANT_BOOL bSaveDeps) PURE;
	STDMETHOD(Remove)(THIS) PURE;
	STDMETHOD(Save)(THIS) PURE;
	STDMETHOD(MoveItemToFolder)(THIS_ VARIANT varFile, VARIANT varFolder) PURE;
	STDMETHOD(get_Active)(THIS_ VARIANT_BOOL FAR* pbActive) PURE;
	STDMETHOD(put_Active)(THIS_ VARIANT_BOOL bActive) PURE;
	STDMETHOD(get_Tools)(THIS_ VARIANT FAR* pvarTools) PURE;
	STDMETHOD(get_GlobalNamespace)(THIS_ IDispatch * FAR* GlobalNamespace) PURE;
	STDMETHOD(get_RelativePath)(THIS_ BSTR FAR* RelativePath) PURE;
	STDMETHOD(put_RelativePath)(THIS_ BSTR RelativePath) PURE;
	STDMETHOD(get_IntermediateDirectory)(THIS_ VARIANT varConfig, BSTR *pBstrIntermediateDirectory) PURE;
	STDMETHOD(put_IntermediateDirectory)(THIS_ VARIANT varConfig, BSTR   bstrIntermediateDirectory) PURE;
	STDMETHOD(get_OutputDirectory)(THIS_ VARIANT varConfig, BSTR *pBstrIntermediateDirectory) PURE;
	STDMETHOD(put_OutputDirectory)(THIS_ VARIANT varConfig, BSTR   bstrOutputDirectory) PURE;
	STDMETHOD(get_UseOfMFC)(THIS_ VARIANT varConfig, long *plUseOfMFC) PURE;
	STDMETHOD(put_UseOfMFC)(THIS_ VARIANT varConfig, long lUseOfMFC) PURE;
	STDMETHOD(get_BuildsLibrary)(THIS_ VARIANT varConfig, VARIANT_BOOL * pbBuildsLibrary) PURE;
	STDMETHOD(put_BuildsLibrary)(THIS_ VARIANT varConfig, VARIANT_BOOL bBuildsLibrary) PURE;
	STDMETHOD(AddBuildEvent)(THIS_ DsBuildEventTimes lType, IDispatch **ppDisp) PURE;
	STDMETHOD(get_PropertyNames)(THIS_ VARIANT FAR * varNames) PURE;
	STDMETHOD(get_NamedProperty)(THIS_ VARIANT varConfig, VARIANT varName, VARIANT FAR* varValue) PURE;
	STDMETHOD(put_NamedProperty)(THIS_ VARIANT varConfig, VARIANT varName, VARIANT varValue) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// Configuration object

// IConfiguration interface

#undef INTERFACE
#define INTERFACE IConfiguration

DECLARE_INTERFACE_(IConfiguration, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

    /* IConfiguration methods */
	STDMETHOD(get_Name)(THIS_ BSTR FAR* Name) PURE;
	STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application) PURE;
	STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* Parent) PURE;

	STDMETHOD(AddToolSettings)(THIS_ BSTR szTool, BSTR szSettings, VARIANT Reserved) PURE;
	STDMETHOD(RemoveToolSettings)(THIS_ BSTR szTool, BSTR szSettings, VARIANT Reserved) PURE;
	STDMETHOD(AddCustomBuildStep)(THIS_ BSTR szCommand, BSTR szOutput, BSTR szDescription, VARIANT Reserved) PURE;
	STDMETHOD(get_Configurations)(THIS_ IConfigurations FAR* FAR* Configurations) PURE;
	STDMETHOD(Reserved11)(THIS) PURE;
	STDMETHOD(MakeCurrentSettingsDefault)(THIS_ VARIANT Reserved) PURE;
	STDMETHOD(AddFileSettings)(THIS_ BSTR szFile, BSTR szSettings, VARIANT Reserved) PURE;
	STDMETHOD(RemoveFileSettings)(THIS_ BSTR szFile, BSTR szSettings, VARIANT Reserved) PURE;
	STDMETHOD(AddCustomBuildStepToFile)(THIS_ BSTR szFile, BSTR szCommand, BSTR szOutput, BSTR szDescription, VARIANT Reserved) PURE;
	STDMETHOD(get_Active)(THIS_ VARIANT_BOOL *bValue) PURE;
	STDMETHOD(put_Active)(THIS_ VARIANT_BOOL bNewValue) PURE;
	STDMETHOD(get_Platform)(THIS_ IDispatch * FAR* Platform) PURE;
	STDMETHOD(get_PrimaryOutput)(THIS_ BSTR * pbstrPrimaryOutput) PURE;
	STDMETHOD(Remove)(THIS) PURE;
	STDMETHOD(get_DebugExe)(THIS_ BSTR *DebugExe) PURE;
	STDMETHOD(put_DebugExe)(THIS_ BSTR  DebugExe) PURE;
	STDMETHOD(get_DebugWorkingDir)(THIS_ BSTR *DebugWorkingDir) PURE;
	STDMETHOD(put_DebugWorkingDir)(THIS_ BSTR  DebugWorkingDir) PURE;
	STDMETHOD(get_DebugProgramArguments)(THIS_ BSTR *DebugProgramArguments) PURE;
	STDMETHOD(put_DebugProgramArguments)(THIS_ BSTR  DebugProgramArguments) PURE;
	STDMETHOD(get_DebugRemoteExe)(THIS_ BSTR *szRemoteExe) PURE;
	STDMETHOD(put_DebugRemoteExe)(THIS_ BSTR szRemoteExe) PURE;
	STDMETHOD(get_DebugRemoteMachine)(THIS_ BSTR *szRemoteExe) PURE;
	STDMETHOD(put_DebugRemoteMachine)(THIS_ BSTR szRemoteExe) PURE;
	STDMETHOD(get_DebugRemoteAttach)(THIS_ VARIANT_BOOL *fRemoteAttach) PURE;
	STDMETHOD(put_DebugRemoteAttach)(THIS_ VARIANT_BOOL fRemoteAttach) PURE;
	STDMETHOD(get_DebugRemoteDebug)(THIS_ VARIANT_BOOL *fRemoteDebug) PURE;
	STDMETHOD(put_DebugRemoteDebug)(THIS_ VARIANT_BOOL fRemoteDebug) PURE;
    STDMETHOD(get_UsesAttributes)(THIS_ VARIANT_BOOL FAR* bValue) PURE;
    STDMETHOD(put_UsesAttributes)(THIS_ VARIANT_BOOL bValue) PURE;
};


/////////////////////////////////////////////////////////////////////////
// Configurations collection object

// IConfigurations interface

#undef INTERFACE
#define INTERFACE IConfigurations

DECLARE_INTERFACE_(IConfigurations, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

    /* IConfigurations methods */
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application) PURE;
    STDMETHOD(get_Count)(THIS_ long FAR* Count) PURE;
    STDMETHOD(get_Parent)(THIS_ IBuildProject FAR* FAR* Parent) PURE;
    STDMETHOD(get__NewEnum)(THIS_ IUnknown * FAR* _NewEnum) PURE;
    STDMETHOD(Item)(THIS_ VARIANT Index, IConfiguration FAR* FAR* Item) PURE;
};


// IBuildItems interface

#undef INTERFACE
#define INTERFACE IBuildItems

DECLARE_INTERFACE_(IBuildItems, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

    /* IBuildItems methods */
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application) PURE;
    STDMETHOD(get_Count)(THIS_ long FAR* Count) PURE;
    STDMETHOD(get_Parent)(THIS_ IBuildProject FAR* FAR* Parent) PURE;
    STDMETHOD(get__NewEnum)(THIS_ IUnknown * FAR* _NewEnum) PURE;
    STDMETHOD(Item)(THIS_ VARIANT Index, IDispatch FAR* FAR* Item) PURE;

    STDMETHOD(get_Filter)(THIS_ long FAR* pdwFilter) PURE;
    STDMETHOD(put_Filter)(THIS_ long dwFilter) PURE;
};


// IBuildItem interface

#undef INTERFACE
#define INTERFACE IBuildItem

DECLARE_INTERFACE_(IBuildItem, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

    /* IBuildItem methods */
    STDMETHOD(get_Name)(THIS_ BSTR FAR* Name) PURE;
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application) PURE;
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* Parent) PURE;

	STDMETHOD(get_Type)(THIS_ long FAR *plType) PURE;
	STDMETHOD(get_PropertyNames)(THIS_ VARIANT FAR * varNames) PURE;
	STDMETHOD(get_NamedProperty)(THIS_ const VARIANT FAR& varConfig, const VARIANT FAR& varName, VARIANT FAR* varValue) PURE;
	STDMETHOD(put_NamedProperty)(THIS_ const VARIANT FAR& varConfig, const VARIANT FAR& varName, const VARIANT FAR& varValue) PURE;
	STDMETHOD(Remove)(THIS ) PURE;
};


// IBuildFile interface

#undef INTERFACE
#define INTERFACE IBuildFile

DECLARE_INTERFACE_(IBuildFile, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

	/* IBuildItem methods */
	STDMETHOD(get_Name)(THIS_ BSTR FAR* Name) PURE;
	STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application) PURE;
	STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* Parent) PURE;

	STDMETHOD(get_Type)(THIS_ long FAR *plType) PURE;
	STDMETHOD(get_PropertyNames)(THIS_ VARIANT FAR * varNames) PURE;
	STDMETHOD(get_BuildItems)(THIS_ IDispatch FAR* FAR* BuildItems) PURE;
	STDMETHOD(get_NamedProperty)(THIS_ VARIANT varConfig, VARIANT varName, VARIANT FAR* varValue) PURE;
	STDMETHOD(put_NamedProperty)(THIS_ VARIANT varConfig, VARIANT varName, VARIANT varValue) PURE;
	STDMETHOD(Remove)(THIS ) PURE;

	/* IBuildFile methods */
	STDMETHOD(get_FullName)(THIS_ BSTR FAR* FullName) PURE;
	STDMETHOD(get_RelativePath)(THIS_ BSTR FAR* RelativePath) PURE;
	STDMETHOD(put_RelativePath)(THIS_ BSTR RelativePath) PURE;
	STDMETHOD(get_SCCStatus)(THIS_ BSTR FAR* SCCStatus) PURE;
	STDMETHOD(get_Time)(THIS_ BSTR FAR * varTime) PURE;
	STDMETHOD(get_ExcludeFromBuild)(THIS_ VARIANT varConfig, VARIANT_BOOL * varExcludeFromBuild) PURE;
	STDMETHOD(put_ExcludeFromBuild)(THIS_ VARIANT varConfig, VARIANT_BOOL varExcludeFromBuild) PURE;
	STDMETHOD(get_ExistsOnDisk)(THIS_ VARIANT_BOOL * bExistsOnDisk) PURE;
	STDMETHOD(Compile)(THIS ) PURE;
	STDMETHOD(get_Tool)(THIS_ VARIANT varConfig, BSTR *pBstrTool) PURE;
	STDMETHOD(put_Tool)(THIS_ VARIANT varConfig, BSTR   bstrTool) PURE;
	STDMETHOD(get_Commands)(THIS_ VARIANT varConfig, BSTR * Commands) PURE;
	STDMETHOD(put_Commands)(THIS_ VARIANT varConfig, BSTR Commands) PURE;
	STDMETHOD(get_ReadOnly)(THIS_ VARIANT_BOOL * varExcludeFromBuild) PURE;
	STDMETHOD(put_ReadOnly)(THIS_ VARIANT_BOOL varExcludeFromBuild) PURE;
	STDMETHOD(get_Document)(THIS_ IDispatch FAR* FAR* Document) PURE;
};


// IBuildFolder interface

#undef INTERFACE
#define INTERFACE IBuildFolder

DECLARE_INTERFACE_(IBuildFolder, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

	/* IBuildItem methods */
    STDMETHOD(get_Name)(THIS_ BSTR FAR* Name) PURE;
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application) PURE;
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* Parent) PURE;
	
	STDMETHOD(get_Type)(THIS_ long FAR *plType) PURE;
	STDMETHOD(get_PropertyNames)(THIS_ VARIANT FAR * varNames) PURE;
	STDMETHOD(get_NamedProperty)(THIS_ const VARIANT FAR& varConfig, const VARIANT FAR& varName, VARIANT FAR* varValue) PURE;
	STDMETHOD(put_NamedProperty)(THIS_ const VARIANT FAR& varConfig, const VARIANT FAR& varName, const VARIANT FAR& varValue) PURE;
	STDMETHOD(Remove)(THIS ) PURE;

    /* IBuildFolder methods */
    STDMETHOD(put_Name)(THIS_ BSTR Name) PURE;
	STDMETHOD(get_FolderExtensions)(THIS_ BSTR * pbstrExtensions) PURE; 
	STDMETHOD(put_FolderExtensions)(THIS_ BSTR bstrExtensions) PURE; 
	STDMETHOD(get_BuildItems)(THIS_ IDispatch FAR* FAR* BuildItems) PURE;
};


// IBuildDependentProject interface

#undef INTERFACE
#define INTERFACE IBuildDependentProject

DECLARE_INTERFACE_(IBuildDependentProject, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

	/* IBuildItem methods */
    STDMETHOD(get_Name)(THIS_ BSTR FAR* Name) PURE;
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application) PURE;
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* Parent) PURE;
	
	STDMETHOD(get_Type)(THIS_ long FAR *plType) PURE;
	STDMETHOD(get_PropertyNames)(THIS_ VARIANT FAR * varNames) PURE;
	STDMETHOD(get_NamedProperty)(THIS_ const VARIANT FAR& varConfig, const VARIANT FAR& varName, VARIANT FAR* varValue) PURE;
	STDMETHOD(put_NamedProperty)(THIS_ const VARIANT FAR& varConfig, const VARIANT FAR& varName, const VARIANT FAR& varValue) PURE;
	STDMETHOD(Remove)(THIS ) PURE;

    /* IBuildDependentProject methods */
	STDMETHOD(get_Project)(THIS_ IDispatch * FAR * Project) PURE;
};


// IBuildEvent interface

#undef INTERFACE
#define INTERFACE IBuildEvent

DECLARE_INTERFACE_(IBuildEvent, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

	/* IBuildItem methods */
    STDMETHOD(get_Name)(THIS_ BSTR FAR* Name) PURE;
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application) PURE;
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* Parent) PURE;

	STDMETHOD(get_Type)(THIS_ long FAR *plType) PURE;
	STDMETHOD(get_PropertyNames)(THIS_ VARIANT FAR * varNames) PURE;
	STDMETHOD(get_NamedProperty)(THIS_ const VARIANT FAR& varConfig, const VARIANT FAR& varName, VARIANT FAR* varValue) PURE;
	STDMETHOD(put_NamedProperty)(THIS_ const VARIANT FAR& varConfig, const VARIANT FAR& varName, const VARIANT FAR& varValue) PURE;
	STDMETHOD(Remove)(THIS) PURE;

    /* IBuildEvent methods */
	STDMETHOD(get_Commands)(THIS_ VARIANT varConfig, BSTR * Commands) PURE;
	STDMETHOD(put_Commands)(THIS_ VARIANT varConfig, BSTR Commands) PURE;
	STDMETHOD(Compile)(THIS_ VARIANT varConfig) PURE;
	STDMETHOD(get_Description)(THIS_ VARIANT varConfig, BSTR * Description) PURE;
	STDMETHOD(put_Description)(THIS_ VARIANT varConfig, BSTR Description) PURE;
	STDMETHOD(get_BuildEventTime)(THIS_ DsBuildEventTimes* eventTime) PURE;
};



/////////////////////////////////////////////////////////////////////////////
// Platforms collection object

// IPlatforms interface

#undef INTERFACE
#define INTERFACE IPlatforms

DECLARE_INTERFACE_(IPlatforms, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

    /* IPlatforms methods */
    STDMETHOD(get_Count)(THIS_ long FAR* Count) PURE;
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* ppApplication) PURE;
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* ppParent) PURE;
    STDMETHOD(get__NewEnum)(THIS_ IUnknown * FAR* _NewEnum) PURE;
    STDMETHOD(Item)(THIS_ VARIANT index, IDispatch * FAR* Item) PURE;
};


/////////////////////////////////////////////////////////////////////////////
// Platform object

// IPlatform interface

#undef INTERFACE
#ifndef __IPLATFORM_INTERFACE_DEFINED__
#define __IPLATFORM_INTERFACE_DEFINED__
#define INTERFACE IPLATFORM

DECLARE_INTERFACE_(IPlatform, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

    /* IPLATFORM methods */
    STDMETHOD(get_Name)(THIS_ BSTR FAR* Name) PURE;
    STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application) PURE;
    STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* Parent) PURE;
    STDMETHOD(get_IncludeFiles)(THIS_ IDispatch * FAR* IncludeFiles) PURE;
    STDMETHOD(get_LibraryFiles)(THIS_ IDispatch * FAR* LibraryFiles) PURE;
    STDMETHOD(get_SourceFiles)(THIS_ IDispatch * FAR* SourceFiles) PURE;
    STDMETHOD(get_Path)(THIS_ IDispatch * FAR* Path) PURE;
};

#endif // __IPLATFORM_INTERFACE_DEFINED__


#ifndef __IEnvironment_INTERFACE_DEFINED__
#define __IEnvironment_INTERFACE_DEFINED__



EXTERN_C const IID IID_IEnvironment;

#undef INTERFACE
#define INTERFACE IEnvironment

DECLARE_INTERFACE_(IEnvironment, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

	/* IEnvironment methods */
    public:
        STDMETHOD(get_Application)(THIS_  IDispatch * FAR *pVal) PURE;
        STDMETHOD(get_Parent)(THIS_ IDispatch * FAR *pVal) PURE;
        STDMETHOD(get_Count)(THIS_ long FAR *pVal) PURE;
        STDMETHOD(get__NewEnum)(THIS_ IUnknown * FAR *pVal) PURE;
        STDMETHOD(Item)(THIS_ VARIANT index, IDispatch * FAR *ppDisp) PURE;

        STDMETHOD(Add)(THIS_ BSTR bstrName, BSTR bstrValue) PURE;
        STDMETHOD(Expand)(THIS_  BSTR bstrSrc, BSTR FAR *bstrRes) PURE;
    };
    

#endif 	/* __IEnvironment_INTERFACE_DEFINED__ */


#ifndef __IEnvironmentVariable_INTERFACE_DEFINED__
#define __IEnvironmentVariable_INTERFACE_DEFINED__


EXTERN_C const IID IID_IEnvironmentVariable;

#undef INTERFACE
#define INTERFACE IEnvironmentVariable

DECLARE_INTERFACE_(IEnvironmentVariable, IDispatch)
{
BEGIN_INTERFACE
#ifndef NO_BASEINTERFACE_FUNCS

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;

    /* IDispatch methods */
    STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo) PURE;

    STDMETHOD(GetTypeInfo)(
      THIS_
      UINT itinfo,
      LCID lcid,
      ITypeInfo FAR* FAR* pptinfo) PURE;

    STDMETHOD(GetIDsOfNames)(
      THIS_
      REFIID riid,
      OLECHAR FAR* FAR* rgszNames,
      UINT cNames,
      LCID lcid,
      DISPID FAR* rgdispid) PURE;

    STDMETHOD(Invoke)(
      THIS_
      DISPID dispidMember,
      REFIID riid,
      LCID lcid,
      WORD wFlags,
      DISPPARAMS FAR* pdispparams,
      VARIANT FAR* pvarResult,
      EXCEPINFO FAR* pexcepinfo,
      UINT FAR* puArgErr) PURE;
#endif

    /* IEnvironmentVariable methods */
    public:
        STDMETHOD(get_Name)(THIS_ BSTR FAR *pVal) PURE;
        STDMETHOD(get_Application)(THIS_ IDispatch * FAR *pVal) PURE;
        STDMETHOD(get_Parent)(THIS_ IDispatch * FAR *pVal) PURE;
        STDMETHOD(get_Value)(THIS_ BSTR FAR *pVal) PURE;
        STDMETHOD(put_Value)(THIS_ BSTR newVal) PURE;
        STDMETHOD(Remove)(THIS) PURE;
        
    };
    

#endif 	/* __IEnvironmentVariable_INTERFACE_DEFINED__ */




#endif //__BLDAUTO_H__
