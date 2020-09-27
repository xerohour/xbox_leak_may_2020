/************************************************************************
*                                                                       *
*   DMUSProd.h -- This module contains the API for the                  *
*                 Framework interfaces                                  *
*                                                                       *
*   Copyright (c) 1998-1999, Microsoft Corp. All rights reserved.       *
*                                                                       *
************************************************************************/

#ifndef __DMUSProd_h__
#define __DMUSProd_h__

#include <windows.h>

#define COM_NO_WINDOWS_H
#include <objbase.h>

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 
interface IDMUSProdFramework;
interface IDMUSProdNode;
interface IDMUSProdPropPageManager;
interface IDMUSProdPropPageObject;
interface IDMUSProdProject;
interface IDMUSProdComponent;
interface IDMUSProdRIFFExt;
interface IDMUSProdPersistInfo;
interface IDMUSProdDocType;
interface IDMUSProdEditor;
interface IDMUSProdSortNode;
interface IDMUSProdMenu;
interface IDMUSProdToolBar;
interface IDMUSProdPropSheet;
interface IDMUSProdFileRefChunk;
interface IDMUSProdLoaderRefChunk;
interface IDMUSProdNodeDrop;
interface IDMUSProdNotifySink;
interface IDMUSProdPChannelName;

#ifndef __cplusplus 
typedef interface IDMUSProdFramework IDMUSProdFramework;
typedef interface IDMUSProdNode IDMUSProdNode;
typedef interface IDMUSProdPropPageManager IDMUSProdPropPageManager;
typedef interface IDMUSProdPropPageObject IDMUSProdPropPageObject;
typedef interface IDMUSProdProject IDMUSProdProject;
typedef interface IDMUSProdComponent IDMUSProdComponent;
typedef interface IDMUSProdRIFFExt IDMUSProdRIFFExt;
typedef interface IDMUSProdPersistInfo IDMUSProdPersistInfo;
typedef interface IDMUSProdDocType IDMUSProdDocType;
typedef interface IDMUSProdEditor IDMUSProdEditor;
typedef interface IDMUSProdSortNode IDMUSProdSortNode;
typedef interface IDMUSProdMenu IDMUSProdMenu;
typedef interface IDMUSProdToolBar IDMUSProdToolBar;
typedef interface IDMUSProdPropSheet IDMUSProdPropSheet;
typedef interface IDMUSProdFileRefChunk IDMUSProdFileRefChunk;
typedef interface IDMUSProdLoaderRefChunk IDMUSProdLoaderRefChunk;
typedef interface IDMUSProdNodeDrop IDMUSProdNodeDrop;
typedef interface IDMUSProdNotifySink IDMUSProdNotifySink;
typedef interface IDMUSProdPChannelName IDMUSProdPChannelName;
#endif /* __cplusplus */


#define CF_DMUSPROD_FILE "DMUSProd File v.1"
#define FOURCC_DMUSPROD_FILEREF mmioFOURCC('j','z','f','r')

typedef enum enumStatusBarStyle
{
	SBS_NOBORDERS	= 0,
	SBS_RAISED		= SBS_NOBORDERS + 1,
	SBS_SUNKEN		= SBS_RAISED + 1
} StatusBarStyle;

typedef enum enumStatusBarLifeSpan
{
	SBLS_APPLICATION	= 0,
	SBLS_EDITOR			= 1000,
	SBLS_CONTROL		= 2000,
	SBLS_MOUSEDOWNUP	= 3000
} StatusBarLifeSpan;

typedef enum enumFileType
{
	FT_UNKNOWN	= 0,
	FT_DESIGN	= FT_UNKNOWN + 1,
	FT_RUNTIME	= FT_DESIGN + 1
}FileType;

typedef enum enumWhichLoader
{
	WL_DIRECTMUSIC	= 0,
	WL_PRODUCER	= WL_DIRECTMUSIC + 1
} WhichLoader;

typedef struct _DMUSProdListInfo
{
    WORD wSize;
    BSTR bstrName;
    BSTR bstrDescriptor;
    GUID guidObject;
} DMUSProdListInfo;

typedef struct _DMUSProdStreamInfo
{
    FileType ftFileType;
    GUID guidDataFormat;
    IDMUSProdNode __RPC_FAR *pITargetDirectoryNode;
} DMUSProdStreamInfo;

/*////////////////////////////////////////////////////////////////////
// IDMUSProdFramework */
#undef  INTERFACE
#define INTERFACE  IDMUSProdFramework
DECLARE_INTERFACE_(IDMUSProdFramework, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)			(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)			(THIS) PURE;
    STDMETHOD_(ULONG,Release)			(THIS) PURE;

    /* IDMUSProdFramework */
    STDMETHOD(FindComponent)			(THIS_ REFCLSID rclsid,
											   IDMUSProdComponent __RPC_FAR *__RPC_FAR *ppIComponent) PURE;
    STDMETHOD(GetFirstComponent)		(THIS_ IDMUSProdComponent __RPC_FAR *__RPC_FAR *ppIFirstComponent) PURE;
    STDMETHOD(GetNextComponent)			(THIS_ IDMUSProdComponent __RPC_FAR *pIComponent,
											   IDMUSProdComponent __RPC_FAR *__RPC_FAR *ppINextComponent) PURE;
    STDMETHOD(AddDocType)				(THIS_ IDMUSProdDocType __RPC_FAR *pIDocType) PURE;
    STDMETHOD(FindDocTypeByExtension)	(THIS_ BSTR bstrExt,
											   IDMUSProdDocType __RPC_FAR *__RPC_FAR *ppIDocType) PURE;
    STDMETHOD(FindDocTypeByNodeId)		(THIS_ REFGUID rguid,
											   IDMUSProdDocType __RPC_FAR *__RPC_FAR *ppIDocType) PURE;
    STDMETHOD(GetFirstDocType)			(THIS_ IDMUSProdDocType __RPC_FAR *__RPC_FAR *ppIFirstDocType) PURE;
    STDMETHOD(GetNextDocType)			(THIS_ IDMUSProdDocType __RPC_FAR *pIDocType,
											   IDMUSProdDocType __RPC_FAR *__RPC_FAR *ppINextDocType) PURE;
    STDMETHOD(CreateNewFile)			(THIS_ IDMUSProdDocType __RPC_FAR *pIDocType,
											   IDMUSProdNode __RPC_FAR *pITreePositionNode,
											   IDMUSProdNode __RPC_FAR *__RPC_FAR *ppIDocRootNode) PURE;
    STDMETHOD(OpenFile)					(THIS_ IDMUSProdDocType __RPC_FAR *pIDocType,
											   BSTR bstrTitle,
											   IDMUSProdNode __RPC_FAR *pITreePositionNode,
											   IDMUSProdNode __RPC_FAR *__RPC_FAR *ppIDocRootNode) PURE;
    STDMETHOD(CopyFile)					(THIS_ IDMUSProdNode __RPC_FAR *pIDocRootNode,
											   IDMUSProdNode __RPC_FAR *pITreePositionNode,
											   IDMUSProdNode __RPC_FAR *__RPC_FAR *ppIDocRootNode) PURE;
    STDMETHOD(ShowFile)					(THIS_ BSTR bstrFileName) PURE;
    STDMETHOD(RegisterClipFormatForFile)(THIS_ UINT uClipFormat,
											   BSTR bstrExt) PURE;
    STDMETHOD(SaveClipFormat)			(THIS_ UINT uClipFormat,
											   IDMUSProdNode __RPC_FAR *pINode,
											   IStream __RPC_FAR *__RPC_FAR *ppIStream) PURE;
    STDMETHOD(GetDocRootNodeFromData)	(THIS_ IDataObject __RPC_FAR *pIDataObject,
											   IDMUSProdNode __RPC_FAR *__RPC_FAR *ppIDocRootNode) PURE;
    STDMETHOD(AllocFileStream)			(THIS_ BSTR bstrFileName,
											   DWORD dwDesiredAccess,
											   FileType ftFileType,
											   GUID guidDataFormat,
											   IDMUSProdNode __RPC_FAR *pITargetFolderNode,
											   IStream __RPC_FAR *__RPC_FAR *ppIStream) PURE;
    STDMETHOD(AllocMemoryStream)		(THIS_ FileType ftFileType,
											   GUID guidDataFormat,
											   IStream __RPC_FAR *__RPC_FAR *ppIStream) PURE;
    STDMETHOD(GetFirstProject)			(THIS_ IDMUSProdProject __RPC_FAR *__RPC_FAR *ppIFirstProject) PURE;
    STDMETHOD(GetNextProject)			(THIS_ IDMUSProdProject __RPC_FAR *pIProject,
											   IDMUSProdProject __RPC_FAR *__RPC_FAR *ppINextProject) PURE;
    STDMETHOD(FindProject)				(THIS_ IDMUSProdNode __RPC_FAR *pINode,
											   IDMUSProdProject __RPC_FAR *__RPC_FAR *ppIProject) PURE;
    STDMETHOD(IsProjectEqual)			(THIS_ IDMUSProdNode __RPC_FAR *pIDocRootNode1,
											   IDMUSProdNode __RPC_FAR *pIDocRootNode2) PURE;
    STDMETHOD(AddNodeImageList)			(THIS_ HANDLE hImageList,
											   short __RPC_FAR *pnNbrFirstImage) PURE;
    STDMETHOD(GetNodeFileName)			(THIS_ IDMUSProdNode __RPC_FAR *pINode,
											   BSTR __RPC_FAR *pbstrFileName) PURE;
    STDMETHOD(GetNodeFileGUID)			(THIS_ IDMUSProdNode __RPC_FAR *pINode,
											   GUID __RPC_FAR *pguidFile) PURE;
    STDMETHOD(RevertFileToSaved)		(THIS_ IDMUSProdNode __RPC_FAR *pINode) PURE;
    STDMETHOD(SaveNode)					(THIS_ IDMUSProdNode __RPC_FAR *pINode) PURE;
    STDMETHOD(SaveNodeAsNewFile)		(THIS_ IDMUSProdNode __RPC_FAR *pINode) PURE;
    STDMETHOD(AddNode)					(THIS_ IDMUSProdNode __RPC_FAR *pINode,
											   IDMUSProdNode __RPC_FAR *pIParentNode) PURE;
    STDMETHOD(RemoveNode)				(THIS_ IDMUSProdNode __RPC_FAR *pINode,
											   BOOL fPromptUser) PURE;
    STDMETHOD(FindDocRootNode)			(THIS_ GUID guidProject,
											   IDMUSProdDocType __RPC_FAR *pIDocType,
											   BSTR bstrNodeName,
											   BSTR bstrNodeDescriptor,
											   IDMUSProdNode __RPC_FAR *__RPC_FAR *ppIDocRootNode) PURE;
    STDMETHOD(FindDocRootNodeByFileGUID)(THIS_ GUID guidFile,
											   IDMUSProdNode __RPC_FAR *__RPC_FAR *ppIDocRootNode) PURE;
    STDMETHOD(GetBestGuessDocRootNode)	(THIS_ IDMUSProdDocType __RPC_FAR *pIDocType,
											   BSTR bstrNodeName,
											   IDMUSProdNode __RPC_FAR *pITreePositionNode,
											   IDMUSProdNode __RPC_FAR *__RPC_FAR *ppIDocRootNode) PURE;
    STDMETHOD(GetSelectedNode)			(THIS_ IDMUSProdNode __RPC_FAR *__RPC_FAR *ppINode) PURE;
    STDMETHOD(SetSelectedNode)			(THIS_ IDMUSProdNode __RPC_FAR *pINode) PURE;
    STDMETHOD(RefreshNode)				(THIS_ IDMUSProdNode __RPC_FAR *pINode) PURE;
    STDMETHOD(SortChildNodes)			(THIS_ IDMUSProdNode __RPC_FAR *pINode) PURE;
    STDMETHOD(EditNodeLabel)			(THIS_ IDMUSProdNode __RPC_FAR *pINode) PURE;
    STDMETHOD(OpenEditor)				(THIS_ IDMUSProdNode __RPC_FAR *pINode) PURE;
    STDMETHOD(CloseEditor)				(THIS_ IDMUSProdNode __RPC_FAR *pINode) PURE;
    STDMETHOD(AddToNotifyList)			(THIS_ IDMUSProdNode __RPC_FAR *pIDocRootNode,
											   IDMUSProdNode __RPC_FAR *pINotifyThisNode) PURE;
    STDMETHOD(RemoveFromNotifyList)		(THIS_ IDMUSProdNode __RPC_FAR *pIDocRootNode,
											   IDMUSProdNode __RPC_FAR *pINotifyThisNode) PURE;
    STDMETHOD(NotifyNodes)				(THIS_ IDMUSProdNode __RPC_FAR *pIChangedDocRootNode,
											   GUID guidUpdateType,
											   void __RPC_FAR *pData) PURE;
    STDMETHOD(AddMenuItem)				(THIS_ IDMUSProdMenu __RPC_FAR *pIMenu) PURE;
    STDMETHOD(RemoveMenuItem)			(THIS_ IDMUSProdMenu __RPC_FAR *pIMenu) PURE;
    STDMETHOD(AddToolBar)				(THIS_ IDMUSProdToolBar __RPC_FAR *pIToolBar) PURE;
    STDMETHOD(RemoveToolBar)			(THIS_ IDMUSProdToolBar __RPC_FAR *pIToolBar) PURE;
    STDMETHOD(SetNbrStatusBarPanes)		(THIS_ int nCount,
											   short nLifeSpan,
											   HANDLE __RPC_FAR *phKey) PURE;
    STDMETHOD(SetStatusBarPaneInfo)		(THIS_ HANDLE hKey,
											   int nIndex,
											   StatusBarStyle sbStyle,
											   int nMaxChars) PURE;
    STDMETHOD(SetStatusBarPaneText)		(THIS_ HANDLE hKey,
											   int nIndex,
											   BSTR bstrText,
											   BOOL bUpdate) PURE;
    STDMETHOD(RestoreStatusBar)			(THIS_ HANDLE hKey) PURE;
    STDMETHOD(StartProgressBar)			(THIS_ int nLower,
											   int nUpper,
											   BSTR bstrPrompt,
											   HANDLE __RPC_FAR *phKey) PURE;
    STDMETHOD(SetProgressBarPos)		(THIS_ HANDLE hKey,
											   int nPos) PURE;
    STDMETHOD(SetProgressBarStep)		(THIS_ HANDLE hKey,
											   UINT nValue) PURE;
    STDMETHOD(StepProgressBar)			(THIS_ HANDLE hKey) PURE;
    STDMETHOD(EndProgressBar)			(THIS_ HANDLE hKey) PURE;
    STDMETHOD(GetSharedObject)			(THIS_ REFCLSID rclsid,
											   REFIID riid,
											   void __RPC_FAR *__RPC_FAR *ppvObject) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdNode */
#undef  INTERFACE
#define INTERFACE  IDMUSProdNode
DECLARE_INTERFACE_(IDMUSProdNode, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)			(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)			(THIS) PURE;
    STDMETHOD_(ULONG,Release)			(THIS) PURE;

    /* IDMUSProdNode */
    STDMETHOD(GetNodeImageIndex)		(THIS_ short __RPC_FAR *pnNbrFirstImage) PURE;
    
    STDMETHOD(UseOpenCloseImages)		(THIS_ BOOL __RPC_FAR *pfUseOpenCloseImages) PURE;
    
    STDMETHOD(GetFirstChild)			(THIS_ IDMUSProdNode __RPC_FAR *__RPC_FAR *ppIFirstChildNode) PURE;
    
    STDMETHOD(GetNextChild)				(THIS_ IDMUSProdNode __RPC_FAR *pIChildNode,
											   IDMUSProdNode __RPC_FAR *__RPC_FAR *ppINextChildNode) PURE;
    
    STDMETHOD(GetComponent)				(THIS_ IDMUSProdComponent __RPC_FAR *__RPC_FAR *ppIComponent) PURE;
    
    STDMETHOD(GetDocRootNode)			(THIS_ IDMUSProdNode __RPC_FAR *__RPC_FAR *ppIDocRootNode) PURE;
    
    STDMETHOD(SetDocRootNode)			(THIS_ IDMUSProdNode __RPC_FAR *pIDocRootNode) PURE;
    
    STDMETHOD(GetParentNode)			(THIS_ IDMUSProdNode __RPC_FAR *__RPC_FAR *ppIParentNode) PURE;
    
    STDMETHOD(SetParentNode)			(THIS_ IDMUSProdNode __RPC_FAR *pIParentNode) PURE;
    
    STDMETHOD(GetNodeId)				(THIS_ GUID __RPC_FAR *pguid) PURE;
    
    STDMETHOD(GetNodeName)				(THIS_ BSTR __RPC_FAR *pbstrName) PURE;
    
    STDMETHOD(GetNodeNameMaxLength)		(THIS_ short __RPC_FAR *pnMaxLength) PURE;
    
    STDMETHOD(ValidateNodeName)			(THIS_ BSTR bstrName) PURE;
    
    STDMETHOD(SetNodeName)				(THIS_ BSTR bstrName) PURE;
    
    STDMETHOD(GetNodeListInfo)			(THIS_ DMUSProdListInfo __RPC_FAR *pListInfo) PURE;
    
    STDMETHOD(GetEditorClsId)			(THIS_ CLSID __RPC_FAR *pclsidEditor) PURE;
    
    STDMETHOD(GetEditorTitle)			(THIS_ BSTR __RPC_FAR *pbstrName) PURE;
    
    STDMETHOD(GetEditorWindow)			(THIS_ HWND __RPC_FAR *hWndEditor) PURE;
    
    STDMETHOD(SetEditorWindow)			(THIS_ HWND hWndEditor) PURE;
    
    STDMETHOD(GetRightClickMenuId)		(THIS_ HINSTANCE __RPC_FAR *phInstance,
											   UINT __RPC_FAR *pnResourceId) PURE;
    
    STDMETHOD(OnRightClickMenuInit)		(THIS_ HMENU hMenu) PURE;
    
    STDMETHOD(OnRightClickMenuSelect)	(THIS_ long lCommandId) PURE;
    
    STDMETHOD(DeleteChildNode)			(THIS_ IDMUSProdNode __RPC_FAR *pIChildNode,
											   BOOL fPromptUser) PURE;
    
    STDMETHOD(InsertChildNode)			(THIS_ IDMUSProdNode __RPC_FAR *pIChildNode) PURE;
    
    STDMETHOD(DeleteNode)				(THIS_ BOOL fPromptUser) PURE;
    
    STDMETHOD(OnNodeSelChanged)			(THIS_ BOOL fSelected) PURE;
    
    STDMETHOD(CreateDataObject)			(THIS_ IDataObject __RPC_FAR *__RPC_FAR *ppIDataObject) PURE;
    
    STDMETHOD(CanCut)					(THIS) PURE;
    
    STDMETHOD(CanCopy)					(THIS) PURE;
    
    STDMETHOD(CanDelete)				(THIS) PURE;
    
    STDMETHOD(CanDeleteChildNode)		(THIS_ IDMUSProdNode __RPC_FAR *pIChildNode) PURE;
    
    STDMETHOD(CanPasteFromData)			(THIS_ IDataObject __RPC_FAR *pIDataObject,
											   BOOL __RPC_FAR *pfWillSetReference) PURE;
    
    STDMETHOD(PasteFromData)			(THIS_ IDataObject __RPC_FAR *pIDataObject) PURE;
    
    STDMETHOD(CanChildPasteFromData)	(THIS_ IDataObject __RPC_FAR *pIDataObject,
											   IDMUSProdNode __RPC_FAR *pIChildNode,
											   BOOL __RPC_FAR *pfWillSetReference) PURE;
    
    STDMETHOD(ChildPasteFromData)		(THIS_ IDataObject __RPC_FAR *pIDataObject,
											   IDMUSProdNode __RPC_FAR *pIChildNode) PURE;
    
    STDMETHOD(GetObject)				(THIS_ REFCLSID rclsid,
											   REFIID riid,
											   void __RPC_FAR *__RPC_FAR *ppvObject) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdPropPageManager */
#undef  INTERFACE
#define INTERFACE  IDMUSProdPropPageManager
DECLARE_INTERFACE_(IDMUSProdPropPageManager, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)			(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)			(THIS) PURE;
    STDMETHOD_(ULONG,Release)			(THIS) PURE;

    /* IDMUSProdPropPageManager */
    STDMETHOD(GetPropertySheetTitle)	(THIS_ BSTR __RPC_FAR *pbstrTitle,
											   BOOL __RPC_FAR *pfAddPropertiesText) PURE;
    
    STDMETHOD(GetPropertySheetPages)	(THIS_ IDMUSProdPropSheet __RPC_FAR *pIPropSheet,
											   LONG __RPC_FAR *__RPC_FAR hPropSheetPage[  ],
											   short __RPC_FAR *pnNbrPages) PURE;
    
    STDMETHOD(OnRemoveFromPropertySheet)(THIS) PURE;
    
    STDMETHOD(SetObject)				(THIS_ IDMUSProdPropPageObject __RPC_FAR *pIPropPageObject) PURE;
    
    STDMETHOD(RemoveObject)				(THIS_ IDMUSProdPropPageObject __RPC_FAR *pIPropPageObject) PURE;
    
    STDMETHOD(IsEqualObject)			(THIS_ IDMUSProdPropPageObject __RPC_FAR *pIPropPageObject) PURE;
    
    STDMETHOD(RefreshData)				(THIS) PURE;
    
    STDMETHOD(IsEqualPageManagerGUID)	(THIS_ REFGUID rguidPageManager) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdPropPageObject */
#undef  INTERFACE
#define INTERFACE  IDMUSProdPropPageObject
DECLARE_INTERFACE_(IDMUSProdPropPageObject, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)			(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)			(THIS) PURE;
    STDMETHOD_(ULONG,Release)			(THIS) PURE;

    /* IDMUSProdPropPageObject */
    STDMETHOD(GetData)					(THIS_ void __RPC_FAR *__RPC_FAR *ppData) PURE;
    
    STDMETHOD(SetData)					(THIS_ void __RPC_FAR *pData) PURE;
    
    STDMETHOD(OnShowProperties)			(THIS) PURE;
    
    STDMETHOD(OnRemoveFromPageManager)	(THIS) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdProject */
#undef  INTERFACE
#define INTERFACE  IDMUSProdProject
DECLARE_INTERFACE_(IDMUSProdProject, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)			(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)			(THIS) PURE;
    STDMETHOD_(ULONG,Release)			(THIS) PURE;

    /* IDMUSProdProject */
    STDMETHOD(GetName)					(THIS_ BSTR __RPC_FAR *pbstrName) PURE;
    
    STDMETHOD(GetGUID)					(THIS_ GUID __RPC_FAR *pguid) PURE;
    
    STDMETHOD(GetFirstFileByDocType)	(THIS_ IDMUSProdDocType __RPC_FAR *pIDocType,
											   IDMUSProdNode __RPC_FAR *__RPC_FAR *ppIFirstFileNode) PURE;
    
    STDMETHOD(GetNextFileByDocType)		(THIS_ IDMUSProdNode __RPC_FAR *pIFileNode,
											   IDMUSProdNode __RPC_FAR *__RPC_FAR *ppINextFileNode) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdComponent */
#undef  INTERFACE
#define INTERFACE  IDMUSProdComponent
DECLARE_INTERFACE_(IDMUSProdComponent, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)		(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)		(THIS) PURE;
    STDMETHOD_(ULONG,Release)		(THIS) PURE;

    /* IDMUSProdComponent */
    STDMETHOD(Initialize)			(THIS_ IDMUSProdFramework __RPC_FAR *pIFramework,
										   BSTR __RPC_FAR *pbstrErrMsg) PURE;
    
    STDMETHOD(CleanUp)				(THIS) PURE;
    
    STDMETHOD(GetName)				(THIS_ BSTR __RPC_FAR *pbstrName) PURE;
    
    STDMETHOD(AllocReferenceNode)	(THIS_ GUID guidRefNodeId,
										   IDMUSProdNode __RPC_FAR *__RPC_FAR *ppIRefNode) PURE;
    
    STDMETHOD(OnActivateApp)		(THIS_ BOOL fActivate) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdRIFFExt */
#undef  INTERFACE
#define INTERFACE  IDMUSProdRIFFExt
DECLARE_INTERFACE_(IDMUSProdRIFFExt, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)	(THIS) PURE;
    STDMETHOD_(ULONG,Release)	(THIS) PURE;

    /* IDMUSProdRIFFExt */
    STDMETHOD(LoadRIFFChunk)	(THIS_ IStream __RPC_FAR *pIStream,
									   IDMUSProdNode __RPC_FAR *__RPC_FAR *ppINode) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdPersistInfo */
#undef  INTERFACE
#define INTERFACE  IDMUSProdPersistInfo
DECLARE_INTERFACE_(IDMUSProdPersistInfo, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)	(THIS) PURE;
    STDMETHOD_(ULONG,Release)	(THIS) PURE;

    /* IDMUSProdPersistInfo */
    STDMETHOD(GetFileName)		(THIS_ BSTR __RPC_FAR *pbstrFileName) PURE;
    
    STDMETHOD(GetStreamInfo)	(THIS_ DMUSProdStreamInfo __RPC_FAR *pStreamInfo) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdDocType */
#undef  INTERFACE
#define INTERFACE  IDMUSProdDocType
DECLARE_INTERFACE_(IDMUSProdDocType, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)		(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)		(THIS) PURE;
    STDMETHOD_(ULONG,Release)		(THIS) PURE;

    /* IDMUSProdDocType */
    STDMETHOD(GetResourceId) (THIS_ HINSTANCE __RPC_FAR *phInstance,
        UINT __RPC_FAR *pnResourceId) PURE;
    
    STDMETHOD(DoesExtensionMatch)	(THIS_ BSTR bstrExt) PURE;
    
    STDMETHOD(DoesIdMatch)			(THIS_ REFGUID rguid) PURE;
    
    STDMETHOD(AllocNode)			(THIS_ REFGUID rguid,
										   IDMUSProdNode __RPC_FAR *__RPC_FAR *ppINode) PURE;
    
    STDMETHOD(OnFileNew)			(THIS_ IDMUSProdProject __RPC_FAR *pITargetProject,
										   IDMUSProdNode __RPC_FAR *pITargetDirectoryNode,
										   IDMUSProdNode __RPC_FAR *__RPC_FAR *ppIDocRootNode) PURE;
    
    STDMETHOD(OnFileOpen)			(THIS_ IStream __RPC_FAR *pIStream,
										   IDMUSProdProject __RPC_FAR *pITargetProject,
										   IDMUSProdNode __RPC_FAR *pITargetDirectoryNode,
										   IDMUSProdNode __RPC_FAR *__RPC_FAR *ppIDocRootNode) PURE;
    
    STDMETHOD(OnFileSave)			(THIS_ IStream __RPC_FAR *pIStream,
										   IDMUSProdNode __RPC_FAR *pIDocRootNode) PURE;
    
    STDMETHOD(GetListInfo)			(THIS_ IStream __RPC_FAR *pIStream,
										   DMUSProdListInfo __RPC_FAR *pListInfo) PURE;
    
    STDMETHOD(IsFileTypeExtension)	(THIS_ FileType ftFileType,
										   BSTR bstrExt) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdEditor */
#undef  INTERFACE
#define INTERFACE  IDMUSProdEditor
DECLARE_INTERFACE_(IDMUSProdEditor, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)				(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)				(THIS) PURE;
    STDMETHOD_(ULONG,Release)				(THIS) PURE;

    /* IDMUSProdEditor */
    STDMETHOD(AttachObjects)				(THIS_ IDMUSProdNode __RPC_FAR *pINode) PURE;
    
    STDMETHOD(OnInitMenuFilePrint)			(THIS_ HMENU hMenu,
												   UINT nMenuID) PURE;
    
    STDMETHOD(OnInitMenuFilePrintPreview)	(THIS_ HMENU hMenu,
        UINT nMenuID) PURE;
    
    STDMETHOD(OnFilePrint)					(THIS) PURE;
    
    STDMETHOD(OnFilePrintPreview)			(THIS) PURE;
    
    STDMETHOD(OnViewProperties)				(THIS) PURE;
    
    STDMETHOD(OnF1Help)						(THIS) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdReferenceNode */
#undef  INTERFACE
#define INTERFACE  IDMUSProdReferenceNode
DECLARE_INTERFACE_(IDMUSProdReferenceNode, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)		(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)		(THIS) PURE;
    STDMETHOD_(ULONG,Release)		(THIS) PURE;

    /* IDMUSProdReferenceNode */
    STDMETHOD(GetReferencedFile)	(THIS_ IDMUSProdNode __RPC_FAR *__RPC_FAR *ppIDocRootNode) PURE;
    
    STDMETHOD(SetReferencedFile)	(THIS_ IDMUSProdNode __RPC_FAR *pIDocRootNode) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdSortNode */
#undef  INTERFACE
#define INTERFACE  IDMUSProdSortNode
DECLARE_INTERFACE_(IDMUSProdSortNode, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)	(THIS) PURE;
    STDMETHOD_(ULONG,Release)	(THIS) PURE;

    /* IDMUSProdSortNode */
    STDMETHOD(CompareNodes)		(THIS_ IDMUSProdNode __RPC_FAR *pINode1,
									   IDMUSProdNode __RPC_FAR *pINode2,
									   int __RPC_FAR *pnResult) PURE;
    
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdMenu */
#undef  INTERFACE
#define INTERFACE  IDMUSProdMenu
DECLARE_INTERFACE_(IDMUSProdMenu, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)	(THIS) PURE;
    STDMETHOD_(ULONG,Release)	(THIS) PURE;

    /* IDMUSProdMenu */
    STDMETHOD(GetMenuText)		(THIS_ BSTR __RPC_FAR *pbstrText) PURE;
    
    STDMETHOD(GetMenuHelpText)	(THIS_ BSTR __RPC_FAR *pbstrHelpText) PURE;
    
    STDMETHOD(OnMenuInit)		(THIS_ HMENU hMenu,
									   UINT nMenuID) PURE;
    
    STDMETHOD(OnMenuSelect)		(THIS) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdToolBar */
#undef  INTERFACE
#define INTERFACE  IDMUSProdToolBar
DECLARE_INTERFACE_(IDMUSProdToolBar, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)	(THIS) PURE;
    STDMETHOD_(ULONG,Release)	(THIS) PURE;

    /* IDMUSProdToolBar */
    STDMETHOD(GetInfo)			(THIS_ HWND __RPC_FAR *phWndOwner,
									   HINSTANCE __RPC_FAR *phInstance,
									   UINT __RPC_FAR *pnResourceId,
									   BSTR __RPC_FAR *pbstrTitle) PURE;
    
    STDMETHOD(GetMenuText)		(THIS_ BSTR __RPC_FAR *pbstrText) PURE;
    
    STDMETHOD(GetMenuHelpText)	(THIS_ BSTR __RPC_FAR *pbstrHelpText) PURE;
    
    STDMETHOD(Initialize)		(THIS_ HWND hWndToolBar) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdPropSheet */
#undef  INTERFACE
#define INTERFACE  IDMUSProdPropSheet
DECLARE_INTERFACE_(IDMUSProdPropSheet, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)				(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)				(THIS) PURE;
    STDMETHOD_(ULONG,Release)				(THIS) PURE;

    /* IDMUSProdPropSheet */
    STDMETHOD(SetPageManager)				(THIS_ IDMUSProdPropPageManager __RPC_FAR *pINewPageManager) PURE;
    
    STDMETHOD(RemovePageManager)			(THIS_ IDMUSProdPropPageManager __RPC_FAR *pIPageManager) PURE;
    
    STDMETHOD(RemovePageManagerByObject)	(THIS_ IDMUSProdPropPageObject __RPC_FAR *pIPropPageObject) PURE;
    
    STDMETHOD(RefreshTitleByObject)			(THIS_ IDMUSProdPropPageObject __RPC_FAR *pIPropPageObject) PURE;
    
    STDMETHOD(RefreshActivePageByObject)	(THIS_ IDMUSProdPropPageObject __RPC_FAR *pIPropPageObject) PURE;
    
    STDMETHOD(IsEqualPageManagerObject)		(THIS_ IDMUSProdPropPageObject __RPC_FAR *pIPropPageObject) PURE;
    
    STDMETHOD(RefreshTitle)					(THIS) PURE;
    
    STDMETHOD(RefreshActivePage)			(THIS) PURE;
    
    STDMETHOD(GetActivePage)				(THIS_ short __RPC_FAR *pnIndex) PURE;
    
    STDMETHOD(SetActivePage)				(THIS_ short nIndex) PURE;
    
    STDMETHOD(Show)							(THIS_ BOOL fShow) PURE;
    
    STDMETHOD(IsShowing)					(THIS) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdFileRefChunk */
#undef  INTERFACE
#define INTERFACE  IDMUSProdFileRefChunk
DECLARE_INTERFACE_(IDMUSProdFileRefChunk, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)	(THIS) PURE;
    STDMETHOD_(ULONG,Release)	(THIS) PURE;

    /* IDMUSProdFileRefChunk */
    STDMETHOD(LoadRefChunk)		(THIS_ IStream __RPC_FAR *pIStream,
									   IDMUSProdNode __RPC_FAR *__RPC_FAR *ppIDocRootNode) PURE;
    
    STDMETHOD(SaveRefChunk)		(THIS_ IStream __RPC_FAR *pIStream,
									   IDMUSProdNode __RPC_FAR *pIRefNode) PURE;
    
    STDMETHOD(GetChunkSize)		(THIS_ DWORD __RPC_FAR *pdwSize) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdLoaderRefChunk */
#undef  INTERFACE
#define INTERFACE  IDMUSProdLoaderRefChunk
DECLARE_INTERFACE_(IDMUSProdLoaderRefChunk, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)			(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)			(THIS) PURE;
    STDMETHOD_(ULONG,Release)			(THIS) PURE;

    /* IDMUSProdLoaderRefChunk */
    STDMETHOD(SaveRefChunkForLoader)	(THIS_ IStream __RPC_FAR *pIStream,
											   IDMUSProdNode __RPC_FAR *pIRefNode,
											   REFCLSID rclsid,
											   void __RPC_FAR *pObjectDesc,
											   WhichLoader wlWhichLoader) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdNodeDrop */
#undef  INTERFACE
#define INTERFACE  IDMUSProdNodeDrop
DECLARE_INTERFACE_(IDMUSProdNodeDrop, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)	(THIS) PURE;
    STDMETHOD_(ULONG,Release)	(THIS) PURE;

    /* IDMUSProdNodeDrop */
    STDMETHOD(OnDropFiles)		(THIS_ HANDLE hDropInfo) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdNotifySink */
#undef  INTERFACE
#define INTERFACE  IDMUSProdNotifySink
DECLARE_INTERFACE_(IDMUSProdNotifySink, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)	(THIS) PURE;
    STDMETHOD_(ULONG,Release)	(THIS) PURE;

    /* IDMUSProdNotifySink */
    STDMETHOD(OnUpdate)			(THIS_ IDMUSProdNode __RPC_FAR *pIDocRootNode,
									   GUID guidUpdateType,
									   void __RPC_FAR *pData) PURE;
};

/*////////////////////////////////////////////////////////////////////
// IDMUSProdPChannelName */
#undef  INTERFACE
#define INTERFACE  IDMUSProdPChannelName
DECLARE_INTERFACE_(IDMUSProdPChannelName, IUnknown)
{
    /* IUnknown */
    STDMETHOD(QueryInterface)	(THIS_ REFIID, LPVOID FAR *) PURE;
    STDMETHOD_(ULONG,AddRef)	(THIS) PURE;
    STDMETHOD_(ULONG,Release)	(THIS) PURE;

    /* IDMUSProdPChannelName */
    STDMETHOD(GetPChannelName)	(THIS_ DWORD dwPChannel, WCHAR __RPC_FAR *pwszName) PURE;

    STDMETHOD(SetPChannelName)	(THIS_ DWORD dwPChannel, WCHAR __RPC_FAR *pwszName) PURE;
};

/* GUID's */
DEFINE_GUID(FRAMEWORK_FileDeleted, 0xd6e0ada0,0x30ce,0x11d1,0x89,0xae,0x0,0xa0,0xc9,0x5,0x41,0x29);
DEFINE_GUID(FRAMEWORK_FileReplaced, 0xd6e0ada1,0x30ce,0x11d1,0x89,0xae,0x0,0xa0,0xc9,0x5,0x41,0x29);
DEFINE_GUID(FRAMEWORK_FileClosed, 0x8743aec0,0x3338,0x11d1,0x89,0xae,0x0,0xa0,0xc9,0x5,0x41,0x29);
DEFINE_GUID(FRAMEWORK_FileNameChange, 0xdd581b01,0x5463,0x11d2,0x89,0xb7,0x0,0xc0,0x4f,0xd9,0x12,0xc8);
DEFINE_GUID(DOCROOT_GuidChange, 0x592f8420,0x643b,0x11d2,0x89,0xb7,0x0,0xc0,0x4f,0xd9,0x12,0xc8);
DEFINE_GUID(GUID_DirectMusicObject, 0x102125e0,0x98b7,0x11d1,0x89,0xaf,0x00,0xa0,0xc9,0x05,0x41,0x29);
DEFINE_GUID(GUID_CurrentVersion, 0x5cbdd400,0x35cc,0x11d1,0x89,0xae,0x00,0xa0,0xc9,0x05,0x41,0x29);
DEFINE_GUID(GUID_CurrentVersion_OnlyUI, 0x0bf78e00,0x4484,0x11d1,0x89,0xae,0x00,0xa0,0xc9,0x05,0x41,0x29);
DEFINE_GUID(GUID_Bookmark, 0xed259580,0xb1ea,0x11d2,0x85,0x3a,0x00,0x10,0x5a,0x27,0x96,0xde);
DEFINE_GUID(GUID_AllZeros, 0x00000000,0x0000,0x0000,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00);

/* IID's */
DEFINE_GUID(IID_IDMUSProdFramework,0x3b8d0e01,0x46b1,0x11d0,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(IID_IDMUSProdNode,0xda821fc1,0x4cef,0x11d0,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(IID_IDMUSProdPropPageManager,0x3095F6E1,0xC160,0x11d0,0x89,0xAE,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(IID_IDMUSProdPropPageObject,0x3095F6E2,0xC160,0x11d0,0x89,0xAE,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(IID_IDMUSProdProject,0xA03AA040,0xE63B,0x11d0,0x89,0xAE,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(IID_IDMUSProdComponent,0x9F3ED901,0x46B7,0x11d0,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(IID_IDMUSProdRIFFExt,0xD913AC41,0x8411,0x11d0,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(IID_IDMUSProdPersistInfo,0xA8AE1161,0x99FD,0x11d0,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(IID_IDMUSProdDocType,0x5c9a32a1,0x4c6d,0x11d0,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(IID_IDMUSProdEditor,0xee3402a1,0x5405,0x11d0,0x89,0xac,0x00,0xa0,0xc9,0x05,0x41,0x29);
DEFINE_GUID(IID_IDMUSProdReferenceNode,0xC483EFC0,0xEC89,0x11d0,0x89,0xAE,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(IID_IDMUSProdSortNode,0x5662F480,0x65D9,0x11d2,0x89,0xB7,0x00,0xC0,0x4F,0xD9,0x12,0xC8);
DEFINE_GUID(IID_IDMUSProdMenu,0xf5d7ce01,0x4cef,0x11d0,0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(IID_IDMUSProdToolBar,0x44D1A761,0xC5FE,0x11d0,0x89,0xAE,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(IID_IDMUSProdPropSheet,0x3095F6E0,0xC160,0x11d0,0x89,0xAE,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(IID_IDMUSProdFileRefChunk,0x7B0FC840,0xE66D,0x11d0,0x89,0xAE,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(IID_IDMUSProdLoaderRefChunk,0xA6403B00,0xB1D6,0x11d1,0x89,0xAF,0x00,0xC0,0x4F,0xD9,0x12,0xC8);
DEFINE_GUID(IID_IDMUSProdNodeDrop,0x24990B00,0xC287,0x11d2,0x85,0x45,0x00,0x10,0x5A,0x27,0x96,0xDE);
DEFINE_GUID(IID_IDMUSProdNotifySink,0x28275880,0x2E9F,0x11d1,0x89,0xAE,0x00,0xA0,0xC9,0x05,0x41,0x29);
DEFINE_GUID(IID_IDMUSProdPChannelName,0x69509a6b,0x1ff8,0x11d2,0x88,0xf4,0x00,0xc0,0x4f,0xbf,0x8d,0x15);
struct __declspec(uuid("{3095F6E2-C160-11d0-89AE-00A0C9054129}")) IDMUSProdPropPageObject;

#ifdef __cplusplus
}; /* extern "C" */
#endif

#endif /* __DMUSProd_h__ */
