/*----------------------------------------------------------------------------
Microsoft Sterling

Microsoft Confidential
Copyright 1994-1996 Microsoft Corporation.  All Rights Reserved.

File:		oleipc.h
Contents:	This file describes the OLE in-place component interfaces
----------------------------------------------------------------------------*/

#ifndef _OLEIPC_H_
#define _OLEIPC_H_


#if 0    // Now defined in ocidl.h   [fabriced] 1 jul 96
//////////////////////////////////////////////////////////////////////////////
//  CALPOLESTR structure - a counted array of LPOLESTRs
//	Duplicated from olectl.h
#ifndef _tagCALPOLESTR_DEFINED
#define _tagCALPOLESTR_DEFINED
#define _CALPOLESTR_DEFINED

typedef struct tagCALPOLESTR
{
	ULONG cElems;
	LPOLESTR FAR* pElems;

} CALPOLESTR;

#endif


//////////////////////////////////////////////////////////////////////////////
//  CAUUID structure - a counted array of DWORDs
//	Duplicated from olectl.h
#ifndef _tagCADWORD_DEFINED
#define _tagCADWORD_DEFINED
#define _CADWORD_DEFINED

typedef struct tagCADWORD
{
	ULONG cElems;
	DWORD FAR* pElems;

} CADWORD;

#endif

#endif  //   0

//////////////////////////////////////////////////////////////////////////////
// Forward declarations
interface IOleInPlaceComponent;
interface IOleInPlaceComponentSite;
interface IOleComponentUIManager;
interface IOleInPlaceComponentUIManager;


//////////////////////////////////////////////////////////////////////////////
// Useful combinations of flags for IMsoCommandTarget
#define MSOCMDSTATE_DISABLED	MSOCMDF_SUPPORTED
#define MSOCMDSTATE_UP			(MSOCMDF_SUPPORTED | MSOCMDF_ENABLED)
#define MSOCMDSTATE_DOWN		(MSOCMDF_SUPPORTED | MSOCMDF_ENABLED | MSOCMDF_LATCHED)
#define MSOCMDSTATE_NINCHED 	(MSOCMDF_SUPPORTED | MSOCMDF_ENABLED | MSOCMDF_NINCHED)


//////////////////////////////////////////////////////////////////////////////
// OLECMDROUTEFLAG enumeration
// Note that these flags are not used directly in the interfaces, but
// are used by implementations of IOleComponentUIManager to control
// command routing.
//
typedef enum tagOLECMDROUTEFLAG {
	OLECMDROUTEFLAG_CONTAINER = 1,
	OLECMDROUTEFLAG_SHELLFIRST = 2
} OLECMDROUTEFLAG;

//////////////////////////////////////////////////////////////////////////////
// OLEROLE enumeration
typedef enum tagOLEROLE {
	OLEROLE_UNKNOWN				=  ((DWORD)-1),
	OLEROLE_COMPONENTHOST		=  0,
	OLEROLE_MAINCOMPONENT		=  1,
	OLEROLE_SUBCOMPONENT		=  2,
	OLEROLE_COMPONENTCONTROL	=  3,
	OLEROLE_TOPLEVELCOMPONENT	=  4
} OLEROLE;


//////////////////////////////////////////////////////////////////////////////
// OLECOMPFLAG enumeration
typedef enum tagOLECOMPFLAG {
	OLECOMPFLAG_ROUTEACTIVEASCNTRCMD	= 1,
	OLECOMPFLAG_INHIBITNESTEDCOMPUI 	= 2,
	OLECOMPFLAG_ROUTEACCELTOCNTR		= 4
} OLECOMPFLAG;


//////////////////////////////////////////////////////////////////////////////
// OLEMENU enumeration
//
// The hardwired in-place component returns these flags from
// GetCntrContextMenu to control how the component control's
// context menu is merged into the container's.
//
// OLEMENU_MENUMERGE - merge the two menus
// OLEMENU_CNTRMENUONLY -	suppress the component control's menu
//							in favor of the main component's.
// OLEMENU_OBJECTMENUONLY - display only the component control's menu
// these three flags are mutually exclusive; if none are set, then
// no context menu is displayed.
//
// OLEMENU_ROUTEACTIVEASCNTRCMD is analogous to
// OLECOMPFLAG_ROUTEACTIVEASCNTRCMD
typedef enum tagOLEMENU {
	OLEMENU_MENUMERGE				= 1,
	OLEMENU_CNTRMENUONLY			= 2,
	OLEMENU_OBJECTMENUONLY			= 4,
	OLEMENU_ROUTEACTIVEASCNTRCMD	= 8
} OLEMENU;

#define OLECONTEXT_NULLMENU 		-1


//////////////////////////////////////////////////////////////////////////////
// OLEACTIVATE enumeration
typedef enum tagOLEACTIVATE {
	OLEACTIVATE_FRAMEWINDOW = 1,
	OLEACTIVATE_DOCWINDOW	= 2
} OLEACTIVATE;


//////////////////////////////////////////////////////////////////////////////
// OLEMENUID enumeration
typedef struct tagOLEMENUID {
	LONG nMenuId;			// Menu ID; -1 if specified by name
	ULONG cwBuf;			// size, in characters, of the buffer
	LPOLESTR pwszNameBuf;	// buffer for menu name
} OLEMENUID;


//////////////////////////////////////////////////////////////////////////////
// OLEMSGBUTTON enumeration
// Buttons to display with a message. This controls the button set used in
// the message.
typedef enum {
	OLEMSGBUTTON_OK					= 0,
	OLEMSGBUTTON_OKCANCEL			= 1,
	OLEMSGBUTTON_ABORTRETRYIGNORE	= 2,
	OLEMSGBUTTON_YESNOCANCEL		= 3,
	OLEMSGBUTTON_YESNO				= 4,
	OLEMSGBUTTON_RETRYCANCEL		= 5,
	OLEMSGBUTTON_YESALLNOCANCEL		= 6
} OLEMSGBUTTON;


//////////////////////////////////////////////////////////////////////////////
// OLEMSGDEFBUTTON enumeration
// Which button is the default. This determines which button is the default.
typedef enum {
	OLEMSGDEFBUTTON_FIRST	= 0,
	OLEMSGDEFBUTTON_SECOND	= 1,
	OLEMSGDEFBUTTON_THIRD	= 2,
	OLEMSGDEFBUTTON_FOURTH	= 3
} OLEMSGDEFBUTTON;


//////////////////////////////////////////////////////////////////////////////
// OLEMSGICON enumeration
// Icon to display and message type. This controls the icon displayed in a 
// message and indicates the type of message.
typedef enum {
	OLEMSGICON_NOICON	= 0,	// Informational
	OLEMSGICON_CRITICAL	= 1,	// Critical
	OLEMSGICON_QUERY	= 2,	// Query (Informational)
	OLEMSGICON_WARNING	= 3,	// Warning
	OLEMSGICON_INFO		= 4		// Informational
} OLEMSGICON;


//////////////////////////////////////////////////////////////////////////////
// OLEHELPCMD enumeration
// Specifies the type of help requested for IOleComponentUIManager::ShowHelp.
// This enumeration gives the valid values for the dwHelpCmd parameter.
typedef enum {
	OLEHELPCMD_CONTEXT   	= 0,	// Display help topic (for a context id)
	OLEHELPCMD_CONTEXTPOPUP	= 1 	// Display help in popup window
} OLEHELPCMD;


//////////////////////////////////////////////////////////////////////////////
// OLEUIEVENTSTATUS enumeration
// Indicates the status of a UI Event
// used with IOLEComponentUIManager::OnUIEvent method
typedef enum tagOLEUIEVENTSTATUS {
	OLEUIEVENTSTATUS_OCCURRED 				= 0,
	OLEUIEVENTSTATUS_STARTNODIALOG			= 1,
	OLEUIEVENTSTATUS_STOP	 				= 2,
	OLEUIEVENTSTATUS_STARTBEFOREDIALOG		= 3,
	OLEUIEVENTSTATUS_CONTINUEINDIALOG		= 4,
	OLEUIEVENTSTATUS_CONTINUEAFTERDIALOG	= 5
} OLEUIEVENTSTATUS;


//////////////////////////////////////////////////////////////////////////////
// OLEUIEVENTFREQ enumeration
// Indicates the frequency that a particular UI Event normally occurs in 
// the component used with IOleComponentUIManager::OnUIEvent method
typedef enum tagOLEUIEVENTFREQ {
	OLEUIEVENTFREQ_NULL			= 0,	// frequency not applicable
	OLEUIEVENTFREQ_CONSISTENTLY	= 2,	// high frequency
	OLEUIEVENTFREQ_FREQUENTLY 	= 4,	// ...
	OLEUIEVENTFREQ_OFTEN	 	= 6,	// ...
	OLEUIEVENTFREQ_SOMETIMES 	= 8,	// ...
	OLEUIEVENTFREQ_SELDOM 		= 10	// low frequency
} OLEUIEVENTFREQ;


//////////////////////////////////////////////////////////////////////////////
// OLEIPCOMPERR_E_xxxx result codes
// NOTE: All three interfaces share a common per-interface error space
#define OLEIPCOMPERR_E_FIRST			MAKE_SCODE(SEVERITY_ERROR, FACILITY_ITF, 0x200)
#define OLEIPCOMPERR_E_MUSTBEMAINCOMP	(OLEIPCOMPERR_E_FIRST)


//////////////////////////////////////////////////////////////////////////////
//
//    IOleInPlaceComponent
//
//////////////////////////////////////////////////////////////////////////////
#undef	INTERFACE
#define	INTERFACE	IOleInPlaceComponent
DECLARE_INTERFACE_(IOleInPlaceComponent, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, VOID **ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IOleInPlaceComponent methods
    STDMETHOD(UseComponentUIManager)(THIS_ DWORD dwCompRole, 
					DWORD *pgrfCompFlags,
        			IOleComponentUIManager *pCompUIMgr, 
					IOleInPlaceComponentSite *pIPCompSite ) PURE;
	STDMETHOD(OnWindowActivate)(THIS_ DWORD dwWindowType,BOOL fActivate) PURE;
	STDMETHOD_(void,OnEnterState)(THIS_ DWORD dwStateId, BOOL fEnter) PURE;
	STDMETHOD_(BOOL,FDoIdle)(THIS_ DWORD grfidlef) PURE;
	STDMETHOD_(BOOL,FQueryClose)(THIS_ BOOL fPromptUser) PURE;
	STDMETHOD(TranslateCntrAccelerator)(THIS_ MSG *pMsg) PURE;
	STDMETHOD(GetCntrContextMenu)(THIS_ DWORD dwRoleActiveObj,
					REFCLSID rclsidActiveObj,
        			LONG nMenuIdActiveObj, POINTS &pos, CLSID *pclsidCntr,
					OLEMENUID *menuid, DWORD *pgrf) PURE;
	STDMETHOD(GetCntrHelp)(THIS_ DWORD *pdwRole, CLSID *pclsid,
					POINT posMouse, DWORD dwHelpCmd,
					LPOLESTR pszHelpFileIn, LPOLESTR *ppszHelpFileOut,
					DWORD dwDataIn, DWORD *pdwDataOut) PURE;
	STDMETHOD(GetCntrMessage)(THIS_ DWORD *pdwRole, CLSID *pclsid,
					LPOLESTR pszTitleIn, LPOLESTR pszTextIn,
					LPOLESTR pszHelpFileIn,
					LPOLESTR *ppszTitleOut, LPOLESTR *ppszTextOut,
					LPOLESTR *ppszHelpFileOut,
					DWORD *pdwHelpContextID,
					OLEMSGBUTTON *pmsgbtn, OLEMSGDEFBUTTON *pmsgdefbtn,
					OLEMSGICON *pmsgicon, BOOL *pfSysAlert) PURE;
};


//////////////////////////////////////////////////////////////////////////////
//
//    IOleInPlaceComponentSite
//
//////////////////////////////////////////////////////////////////////////////
#undef	INTERFACE
#define	INTERFACE	IOleInPlaceComponentSite
DECLARE_INTERFACE_(IOleInPlaceComponentSite, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, VOID **ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IOleInPlaceComponentSite methods
	STDMETHOD(SetUIMode)(THIS_ DWORD dwUIMode) PURE;
};


//////////////////////////////////////////////////////////////////////////////
//
//    IOleComponentUIManager
//
//////////////////////////////////////////////////////////////////////////////
#undef	INTERFACE
#define	INTERFACE	IOleComponentUIManager
DECLARE_INTERFACE_(IOleComponentUIManager, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, VOID **ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IOleComponentUIManager methods
	STDMETHOD(GetPredefinedStrings)(THIS_ DISPID dispid, 
					CALPOLESTR *pcaStringsOut, CADWORD *pcaCookiesOut) PURE;
	STDMETHOD(GetPredefinedValue)(THIS_ DISPID dispid, DWORD dwCookie, 
					VARIANT *pvarOut) PURE;
	STDMETHOD_(void, OnUIEvent)(THIS_ DWORD dwCompRole, REFCLSID rclsidComp, 
					const GUID *pguidUIEventGroup, DWORD nUIEventId,
					DWORD dwUIEventStatus, 
					DWORD dwEventFreq, RECT *prcEventRegion, 
					VARIANT *pvarEventArg) PURE;
	STDMETHOD(OnUIEventProgress)(THIS_ DWORD *pdwCookie, BOOL fInProgress,
					LPOLESTR pwszLabel, ULONG nComplete, ULONG nTotal) PURE;
	STDMETHOD(SetStatus)(THIS_ LPCOLESTR pwszStatusText, 
					DWORD dwReserved) PURE;
	STDMETHOD(ShowContextMenu)(THIS_ DWORD dwCompRole, REFCLSID rclsidActive,
        			LONG nMenuId, POINTS &pos, 
					IMsoCommandTarget *pCmdTrgtActive) PURE;
	STDMETHOD(ShowHelp)(THIS_ DWORD dwCompRole, REFCLSID rclsidComp,
					POINT posMouse, DWORD dwHelpCmd,
					LPOLESTR pszHelpFile, DWORD dwData) PURE;
	STDMETHOD(ShowMessage)(THIS_ DWORD dwCompRole, REFCLSID rclsidComp,
					LPOLESTR pszTitle, LPOLESTR pszText,
					LPOLESTR pszHelpFile, DWORD dwHelpContextID,
					OLEMSGBUTTON msgbtn, OLEMSGDEFBUTTON msgdefbtn,
					OLEMSGICON msgicon, BOOL fSysAlert, LONG *pnResult) PURE;
};


//////////////////////////////////////////////////////////////////////////////
//
//    IOleInPlaceComponentUIManager
//
//////////////////////////////////////////////////////////////////////////////
#undef	INTERFACE
#define	INTERFACE	IOleInPlaceComponentUIManager
DECLARE_INTERFACE_(IOleInPlaceComponentUIManager, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, VOID **ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IOleInPlaceComponentUIManager methods
    STDMETHOD(UIActivateForMe)(THIS_ DWORD dwCompRole, REFCLSID rclsidActive,
        			IOleInPlaceActiveObject *pIPActObj, 
					IMsoCommandTarget *pCmdTrgtActive,
        			ULONG cCmdGrpId, LONG *rgnCmdGrpId) PURE;
	STDMETHOD(UpdateUI)(THIS_ DWORD dwCompRole, BOOL fImmediateUpdate, 
					DWORD dwReserved) PURE;
    STDMETHOD(SetActiveUI)(THIS_ DWORD dwCompRole, REFCLSID rclsid,
					ULONG cCmdGrpId, LONG *rgnCmdGrpId) PURE;
	STDMETHOD_(void,OnUIComponentEnterState)(THIS_ DWORD dwCompRole, 
					DWORD dwStateId, DWORD dwReserved) PURE;
	STDMETHOD_(BOOL,FOnUIComponentExitState)(THIS_ DWORD dwCompRole, 
					DWORD dwStateId, DWORD dwReserved) PURE;
	STDMETHOD_(BOOL,FUIComponentInState)(THIS_ DWORD dwStateId) PURE;
	STDMETHOD_(BOOL,FContinueIdle)(THIS) PURE;
};


//////////////////////////////////////////////////////////////////////////////
//
//    IOleHostRegistry
//
//////////////////////////////////////////////////////////////////////////////
#undef	INTERFACE
#define	INTERFACE	IOleHostRegistry
DECLARE_INTERFACE_(IOleHostRegistry, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, VOID **ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// IOleHostRegistry methods
	STDMETHOD(GetObjectKey)(THIS_ REFCLSID rclsidObject, 
					LPOLESTR *ppszKey) PURE;
};


#endif //_OLEIPC_H_
