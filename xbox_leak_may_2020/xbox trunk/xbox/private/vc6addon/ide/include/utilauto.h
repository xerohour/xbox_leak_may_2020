/*

	Copyright 1996, Microsoft Corp.

*/

#include <ObjModel\appdefs.h>
#include <ObjModel\appauto.h> // from pkgs\include\ObjModel
#include <ObjModel\appguid.h>	// Shell Automation Guids
#include <objext.h>
#include <aut1api.h>

/*
This header contains what was once:
	dualimpl.h
	autoapp.h
	autocol.h
	autowins.h
*/

// WARNING!!!  This file is now including GUIDs defined via
//  DEFINE_GUID.  This means that if you #include this file
//  anywhere from your package, you must also #include it in
//  exactly one of your package's other files with a 
//  "#include <initguid.h>" beforehand: i.e.,
//		#include <initguid.h>
//		#include <utilauto.h>

// {AE166B02-A9A0-11cf-AD07-00A0C9034965}
DEFINE_GUID(IID_IDispApplicationEvents,
0xAE166B02L,0xA9A0,0x11CF,0xAD,0x07,0x00,0xA0,0xC9,0x03,0x49,0x65);

// {B3CF8E20-19B6-11cf-8E4D-00AA004254C4}
DEFINE_GUID(LIBID_Shell,
0xb3cf8e20, 0x19b6, 0x11cf, 0x8e, 0x4d, 0x0, 0xaa, 0x0, 0x42, 0x54, 0xc4);
#define LIBID_Shell_STRING "TypeLib\\{B3CF8E20-19B6-11cf-8E4D-00AA004254C4}\\6.0"

#ifndef __UTILAUTO_H__
#define __UTILAUTO_H__

// This macro is the same as IMPLEMENT_OLECREATE, except it passes TRUE
//  for the bMultiInstance parameter to the COleObjectFactory constructor.
//  We want a separate instance of this application to be launched for
//  each Application object requested by automation controllers.

#define DS_DECLARE_OLECREATE(objfactory_class_name, class_name) \
public: \
	static AFX_DATA objfactory_class_name factory; \
	static AFX_DATA const GUID guid; \

#define DS_IMPLEMENT_OLECREATE(objfactory_class_name, class_name, external_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	AFX_DATADEF objfactory_class_name class_name::factory(class_name::guid, \
		RUNTIME_CLASS(class_name), TRUE, _T(external_name)); \
	const AFX_DATADEF GUID class_name::guid = \
		{ l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }; \

/////////////////////////////////////////////////////////////////////////////
//
//
/*
	The MsDev needs a different GUID for the debug build. This is handled inside
	of autoapp.cpp. You should use msdevD.Application to get the debug version
	and msdev.Application to use the retail version.
	
	Without separate GUIDs it is not possible to debug the debug build with VB.
	Because VB will connect to the debugger.

	Packages generally will not need the debug CLSID. However, if a package does
	need it. It is define below.

	The real CLSID_ApplicationDebug is in autoapp.cpp. Make sure that these two
	versions are in-sync!
*/
#ifdef DEBUG_CLSID

#if 0 
// {9FD2DF21-190D-11CF-8E4D-00AA004254C4}
static const GUID CLSID_ApplicationDebug =  
{ 0x9fd2df21, 0x190d, 0x11cf, { 0x8e, 0x4d, 0x0, 0xaa, 0x0, 0x42, 0x54, 0xc4 }};
#endif

// {E8BEA480-DDE4-11cf-9C4A-00A0C90A632C}
static const GUID CLSID_ApplicationDebug  = 
{ 0xe8bea480, 0xdde4, 0x11cf, { 0x9c, 0x4a, 0x0, 0xa0, 0xc9, 0xa, 0x63, 0x2c } };

#endif

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

/////////////////////////////////////////////////////////////////////////////
// dualimpl.h

// NOTE:  You must include afxctl.h (preferably in your .pch) before
//  including this file.

// This file declares the functions, classes, and preprocessor macros
//  you need to create OLE Automation / dual interface objects that
//  expose functionality FROM Developer Studio packages TO macros
//  and addins.  The code in this file was taken from MFC and the
//  ACDUAL samples, with revisions made.  Functions & macros revised
//  for use in Developer Studio are prefixed with "Ds" and "DS_",
//  respectively.
// All implementation of these functions & classes is in
//  shell\dualimpl.cpp.
// For examples of usage, see the sample package in
//  dev\ide\pkgs\pkg.  Check out auto*.*.
// For more info, see dualimpl.doc, checked in to the VBA subproject of
//  the V5Spec project.  There's a link to it on
//  http://devstudio/devstudio/automation.


// CONTENTS:
//  DsThrowOleDispatchException:
//		Use this instead of AfxThrowOleDispatchException for throwing
//		errors inside your dispatch handlers.
//  DsThrowShellOleDispatchException:
//		Use this instead of AfxThrowOleDispatchException for throwing
//		system-defined errors inside your dispatch handlers.
//  DsThrowCannedOleDispatchException:
//		Use this instead of AfxThrowOleDispatchException for throwing
//		system-defined errors inside your dispatch handlers.
//  CAutoObj:
//		Derive your OLE Automation / dual interface objects from this.
//  CAutoWindowObj:
//		Derive your IGenericWindow implementing automation objects from this.
//  CMyOleDispatchImpl:
//		Used in CAutoObj.
//	General macros used for implementing dual interfaces.
//		Stolen from ACDUAL
//  New macros for implementing dual interfaces.
//		Not stolen from ACDUAL, these are brand new.
//  New macros for firing events


// To throw an error, use this function, and NOT AfxThrowOleDispatchException.
void DsThrowOleDispatchException(HRESULT hr, UINT nDescriptionID, UINT nHelpID=-1);

// Use this when you construct the prompt string yourself
void DsThrowOleDispatchException(HRESULT hr, LPCTSTR szPrompt, UINT nHelpID=-1);

// This function grabs a system-defined HRESULT & returns the sys string for the current locale
void DsThrowShellOleDispatchException(HRESULT hr, UINT nHelpID=0);

// This function grabs a system-defined HRESULT & returns the sys string for the current locale
void DsThrowCannedOleDispatchException(HRESULT hr, UINT nHelpID=0);

/////////////////////////////////////////////////////////////////////////////
// CAutoObj command target

// All OLE Automation / dual interface objects in DevStudio packages
//  should be derived from CAutoObj instead of CCmdTarget.
// We can't just use CCmdTarget, since we have to create a special
//  CMyOleDispatchImpl class (for why, see the comment above its
//  declaration), and the CMyOleDispatchImpl requires a member variable
//  of some CCmdTarget-derived type which it uses to call some protected
//  members of CCmdTarget.  Thus, this "CCmdTarget-derived type" must
//  be a new class of which CMyOleDispatchImpl is a friend.  Thus,
//  we make a special CAutoObj to fill this role.

class CMyOleDispatchImpl;

class CAutoObj : public CCmdTarget
{
	friend class CMyOleDispatchImpl;
	DECLARE_DYNCREATE(CAutoObj)

public:
// Added by DER - Called by Associated Real object such as
// CPartDoc or CPartFrame to inform the Automation object
// to clear its back pointer. Override and clear the back pointer.
	virtual void AssociatedObjectReleased() {} // DYNACREATE doesn't like virtuals...

	// Override this if you actually want your object to be usable
	//  after the user shuts the mainfrm.  Typically, you don't override
	//  this.  If you do, you may simply return without throwing an
	//  exception.  Then, in the implementation of each of your
	//  object's members you must either 1) be sure your implementation
	//  will work even if the user has shut down the UI, or 2) manually
	//  throw an exception warning the user that that member can
	//  not be accessed since the UI was shut down.  (See the base class
	//  implementation for how to do this in ide\shell\dualimpl.cpp.)
	// If you choose not to override this, then your object will automatically
	//  throw an exception if any of its members are accessed after the user
	//  has shut down the UI.  This is accomplished through:
	//  1) Code in CMyOleDispatchImpl::Invoke, and
	//  2) Code in the TRY_DUAL macro you should be using in your dual
	//	   interface vtbl handler.
	virtual void ThrowZombifiedException();

// Implementation
protected:
	virtual ~CAutoObj();
};



/////////////////////////////////////////////////////////////////////////////
// COleDispatchImpl - IDispatch implementation

// This was stolen & modified from MFC.  The change is that this version
//  of the class receives a pThis pointer on initialization rather than
//  constructing it.  It can't construct it, since this class no longer
//  lives at CCmdTarget::m_xDispatch--the implementation
//  of the dual interface lives there instead.
// Another change is the removal of the virtual Disconnect

class CMyOleDispatchImpl : public IDispatch
{
public:
	CAutoObj* m_pThis;
	CMyOleDispatchImpl(CAutoObj* pThis)
	{ m_pThis = pThis; }

	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(QueryInterface)(REFIID, LPVOID*);

	STDMETHOD(GetTypeInfoCount)(UINT*);
	STDMETHOD(GetTypeInfo)(UINT, LCID, LPTYPEINFO*);
	STDMETHOD(GetIDsOfNames)(REFIID, LPOLESTR*, UINT, LCID, DISPID*);
	STDMETHOD(Invoke)(DISPID, REFIID, LCID, WORD, DISPPARAMS*, LPVARIANT,
		LPEXCEPINFO, UINT*);
};


/////////////////////////////////////////////////////////////////////////////
// CHaltRecording
//
// PROBLEM 1:
//  This class fixes the "recording reentrancy" problem:
//  1) You're recording macro A.
//  2) You manipulate DevStudio in such a way that package X calls
//     into package Y's dual interface.
//  3) Package Y thinks it might have been called by UI interaction,
//     and so it says, "hey, we're recording, so I'd better record
//     what I'm doing"
//    RESULT: Package Y records something it shouldn't have, because
//     package X likely already recorded what the user just did that caused
//     package X to call into package Y.
// SOLUTION 1:
//  1) If you're using CAutoObj and all the stuff in this file, YOU DON'T
//     HAVE TO WORRY about this.  The TRY_DUAL macro below will be in
//     all your vtbl handlers, and that macro takes care of instantiating
//     a CHaltRecording.
//  2) If you're using some other way to automate your objects, then
//     you'll want to put the line "CHaltRecording someCoolVariableName"
//     at the top of your vtbl handler.
//
// PROBLEM 2:
//  You're recording macro A, and while you're recording, you run macro B.
//  If we're not careful, all the things that macro B causes the IDE
//  to do will get recorded as well.
// SOLUTION 2:
//  This is handled elsewhere in the shell, and you need not worry about
//  this at all in your package.  Just make sure you check
//  theApp.ShouldRecordText() before recording text, and you'll be fine.


class CHaltRecording
{
public:
	// Constructor halts recording (CTheApp::ShouldRecordText returns FALSE,
	//  but CTheApp::GetRecordState remains unchanged, so UI is unaffected)
	CHaltRecording()
	{ m_bSaveShouldRecordText = theApp.m_bShouldRecordText;
	  theApp.m_bShouldRecordText = FALSE; }

	// Destructor resumes previous behavior of CTheApp::ShouldRecordText.
	~CHaltRecording() { theApp.m_bShouldRecordText = m_bSaveShouldRecordText; }

protected:
	BOOL m_bSaveShouldRecordText;
};

// the HALT_RECORDING() macro is nice for method prologues
#define HALT_RECORDING()	CHaltRecording _HaltRecordingTempFromQCQP_H;


/////////////////////////////////////////////////////////////////////////////
// Lots of fun macros.  The following was stolen and modified from the ACDUAL
//  sample's mfcdual.h.



/////////////////////////////////////////////////////////////////////
// TRY_DUAL and CATCH_ALL_DUAL are used to provide exception handling
// for your dual interface methods. CATCH_ALL_DUAL takes care of 
// returning the appropriate error code.

#define TRY_DUAL(iidSource) \
	CHaltRecording _haltRecording;	\
	HRESULT _hr = S_OK; \
    REFIID  _riidSource = iidSource; \
	TRY \
	{ \
		if (theApp.m_pAutoApp == NULL || theApp.m_pAutoApp->IsZombified()) \
			pThis->ThrowZombifiedException(); \


#define CATCH_ALL_DUAL \
	} \
	CATCH(COleException, e) \
	{ \
		_hr = e->m_sc; \
	} \
	AND_CATCH_ALL(e) \
	{ \
		AFX_MANAGE_STATE(pThis->m_pModuleState); \
		_hr = DsDualHandleException(_riidSource, e); \
	} \
	END_CATCH_ALL \
	return _hr; \

/////////////////////////////////////////////////////////////////////
// DsDualHandleException is a helper function used to set the system's 
// error object, so that container applications that call through 
// VTBLs can retrieve rich error information		
HRESULT DsDualHandleException(REFIID riidSource, const CException* pAnyException);

/////////////////////////////////////////////////////////////////////
// DECLARE_DUAL_ERRORINFO expands to declare the ISupportErrorInfo
// support class. It works together with DUAL_ERRORINFO_PART and
// IMPLEMENT_DUAL_ERRORINFO defined below.
#define DECLARE_DUAL_ERRORINFO() \
    BEGIN_INTERFACE_PART(SupportErrorInfo, ISupportErrorInfo) \
        STDMETHOD(InterfaceSupportsErrorInfo)(THIS_ REFIID riid); \
    END_INTERFACE_PART(SupportErrorInfo) \

/////////////////////////////////////////////////////////////////////
// DUAL_ERRORINFO_PART adds the appropriate entry to the interface map
// for ISupportErrorInfo, if you used DECLARE_DUAL_ERRORINFO.
#define DUAL_ERRORINFO_PART(objectClass) \
    INTERFACE_PART(objectClass, IID_ISupportErrorInfo, SupportErrorInfo) \

/////////////////////////////////////////////////////////////////////
// IMPLEMENT_DUAL_ERRORINFO expands to an implementation of 
// ISupportErrorInfo which matches the declaration in 
// DECLARE_DUAL_ERRORINFO.
#define IMPLEMENT_DUAL_ERRORINFO(objectClass, riidSource) \
    STDMETHODIMP_(ULONG) objectClass::XSupportErrorInfo::AddRef() \
    { \
        METHOD_PROLOGUE(objectClass, SupportErrorInfo) \
	    return pThis->ExternalAddRef(); \
    } \
    STDMETHODIMP_(ULONG) objectClass::XSupportErrorInfo::Release() \
    { \
	    METHOD_PROLOGUE(objectClass, SupportErrorInfo) \
	    return pThis->ExternalRelease(); \
    } \
    STDMETHODIMP objectClass::XSupportErrorInfo::QueryInterface( \
	    REFIID iid, LPVOID* ppvObj) \
    { \
	    METHOD_PROLOGUE(objectClass, SupportErrorInfo) \
	    return pThis->ExternalQueryInterface(&iid, ppvObj); \
    } \
    STDMETHODIMP objectClass::XSupportErrorInfo::InterfaceSupportsErrorInfo( \
        REFIID iid) \
    { \
        METHOD_PROLOGUE(objectClass, SupportErrorInfo) \
        return (iid == riidSource) ? S_OK : S_FALSE; \
    }



/////////////////////////////////////////////////////////////////////


// Added for DevStudio

#define DS_DELEGATE_DUAL_INTERFACE(objectClass, dualClass) \
	STDMETHODIMP objectClass::X##dualClass::QueryInterface (REFIID riid, LPVOID FAR* ppvObj) \
	{ \
		METHOD_PROLOGUE(objectClass, Dispatch) \
		return CMyOleDispatchImpl(pThis).QueryInterface (riid, ppvObj); \
	} \
	STDMETHODIMP_(ULONG) objectClass::X##dualClass::AddRef() \
	{ \
		METHOD_PROLOGUE(objectClass, Dispatch) \
		return CMyOleDispatchImpl(pThis).AddRef(); \
	} \
	STDMETHODIMP_(ULONG) objectClass::X##dualClass::Release() \
	{ \
		METHOD_PROLOGUE(objectClass, Dispatch) \
		return CMyOleDispatchImpl(pThis).Release(); \
	} \
	STDMETHODIMP objectClass::X##dualClass::GetTypeInfoCount(UINT FAR* pctinfo) \
	{ \
		METHOD_PROLOGUE(objectClass, Dispatch) \
		return CMyOleDispatchImpl(pThis).GetTypeInfoCount(pctinfo); \
	} \
	STDMETHODIMP objectClass::X##dualClass::GetTypeInfo( \
	  UINT itinfo, \
	  LCID lcid, \
	  ITypeInfo FAR* FAR* pptinfo) \
	{ \
		METHOD_PROLOGUE(objectClass, Dispatch) \
		return CMyOleDispatchImpl(pThis).GetTypeInfo(itinfo, lcid, pptinfo); \
	} \
	STDMETHODIMP objectClass::X##dualClass::GetIDsOfNames( \
	  REFIID riid, \
	  OLECHAR FAR* FAR* rgszNames, \
	  UINT cNames, \
	  LCID lcid, \
	  DISPID FAR* rgdispid) \
	{ \
		METHOD_PROLOGUE(objectClass, Dispatch) \
		return CMyOleDispatchImpl(pThis).GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid); \
	} \
	STDMETHODIMP objectClass::X##dualClass::Invoke( \
	  DISPID dispidMember, \
	  REFIID riid, \
	  LCID lcid, \
	  WORD wFlags, \
	  DISPPARAMS FAR* pdispparams, \
	  VARIANT FAR* pvarResult, \
	  EXCEPINFO FAR* pexcepinfo, \
	  UINT FAR* puArgErr) \
	{ \
		METHOD_PROLOGUE(objectClass, Dispatch) \
		return CMyOleDispatchImpl(pThis).Invoke(dispidMember, riid, lcid, wFlags, pdispparams, \
											pvarResult, pexcepinfo, puArgErr); \
	}


// This is brand new!!  This adjusts m_xDispatch so that it doesn't
//  point to a COleDispatchDriverImpl.  Instead, it points directly
//  to the dual interface implemented by this object.  Unlike the
//  ACDUAL sample, which maintains two separate vtbls (dual and
//  just dispatch), DevStudio saves an extra DWORD per object
//  instance by only maintaining one vtbl: the dual interface.
// This also corrects the problem reported in Tech Note 65 about
//  passing dual interface pointers to/from functions that expect
//  FromIDispatch and GetIDispatch to work right.  By having
//  m_xDispatch point to the full dual interface, everything
//  just works.

#define DS_IMPLEMENT_ENABLE_DUAL(objectClass, dualClass) \
	void objectClass::EnableDualAutomation() \
	{ \
		ASSERT(GetDispatchMap() != NULL); \
		X##dualClass dual; \
		ASSERT(m_xDispatch.m_vtbl == NULL|| \
			*(DWORD*)&dual == m_xDispatch.m_vtbl); \
		ASSERT(sizeof(m_xDispatch) == sizeof(X##dualClass)); \
		m_xDispatch.m_vtbl = *(DWORD*)&dual; \
		*(X##dualClass*)&m_xDispatch = dual; \
	} \
	objectClass* PASCAL objectClass::FromIDispatch(I##dualClass* lpDual) \
	{ \
		X##dualClass dual; \
		ASSERT(*(DWORD*)&dual != 0); \
		if (*(DWORD*)lpDual != *(DWORD*)&dual) \
			return NULL; \
		objectClass* pTarget = (objectClass*) \
			((BYTE*)lpDual - offsetof(objectClass, m_xDispatch)); \
		ASSERT_VALID(pTarget); \
		return pTarget; \
	}


#define DS_DECLARE_ENABLE_DUAL(objectClass, dualClass) \
		friend class CMyOleDispatchImpl; \
	protected: \
		void EnableDualAutomation(); \
	public: \
		static objectClass* PASCAL FromIDispatch(I##dualClass* lpDual);


#define DS_BEGIN_DUAL_INTERFACE_PART(dualClass) \
	class X##dualClass : I##dualClass \
	{ \
		STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj); \
		STDMETHOD_(ULONG, AddRef)(THIS); \
		STDMETHOD_(ULONG, Release)(THIS); \
		STDMETHOD(GetTypeInfoCount)(THIS_ UINT FAR* pctinfo); \
		STDMETHOD(GetTypeInfo)( \
		  THIS_ \
		  UINT itinfo, \
		  LCID lcid, \
		  ITypeInfo FAR* FAR* pptinfo); \
		STDMETHOD(GetIDsOfNames)( \
		  THIS_ \
		  REFIID riid, \
		  OLECHAR FAR* FAR* rgszNames, \
		  UINT cNames, \
		  LCID lcid, \
		  DISPID FAR* rgdispid); \
		STDMETHOD(Invoke)( \
		  THIS_ \
		  DISPID dispidMember, \
		  REFIID riid, \
		  LCID lcid, \
		  WORD wFlags, \
		  DISPPARAMS FAR* pdispparams, \
		  VARIANT FAR* pvarResult, \
		  EXCEPINFO FAR* pexcepinfo, \
		  UINT FAR* puArgErr);

// Note that we don't add a member variable of this type like MFC does.  We're
//  using m_xDispatch to store the implementation class, and don't want to waste
//  another DWORD inside the object class.
#define DS_END_DUAL_INTERFACE_PART(dualClass) \
	}; \
	friend class X##dualClass;



/////////////////////////////////////////////////////////////////////////////
// EVENTS, EVENTS, EVENTS!!
// These are brand new macros to fire dispatch, then dual interface events.

// This macro is not used directly.  Rather, it's used within other macros.
//  This iterates through all sinks attached to the DUAL connection point,
//  and fires them in turn.  If a cancelable event is canceled, the iteration
//  stops.  The code to do that is in the macro that uses this macro.
#define BEGIN_FIRE_DUAL_EVENT(connPt, interfaceType) \
	{ \
		const CPtrArray* _pConnections = (connPt)->GetConnections(); \
		ASSERT(_pConnections != NULL); \
		CPtrArray _ConnectionsCopy; \
		_ConnectionsCopy.Copy(*_pConnections); \
		int i; \
		int _cConnections = _ConnectionsCopy.GetSize(); \
		IUnknown *pUnk; \
		for(i = 0; i < _cConnections; i++) { \
			pUnk = (LPUNKNOWN)(_ConnectionsCopy.GetAt(i)); \
			ASSERT(pUnk != NULL); \
			pUnk->AddRef(); \
		} \
		for (i = 0; i < _cConnections; i++) \
		{ \
			interfaceType *_pEvents = (interfaceType *) (_ConnectionsCopy.GetAt(i)); \
			ASSERT(_pEvents != NULL);


#define END_FIRE_DUAL_EVENT } \
		for(i = 0; i < _cConnections; i++) { \
			pUnk = (LPUNKNOWN)(_ConnectionsCopy.GetAt(i)); \
			ASSERT(pUnk != NULL); \
			pUnk->Release(); \
		} } }

#define EVENT_PARAM(vtsParams) (BYTE*)(vtsParams)
BOOL FireDispatchEvent(BOOL bCancelable, CConnectionPoint* pCP,
	DISPID dispid, BYTE* pbParams, ...);

// Event queueing macros
#define DS_QUEUE_EVENT0(connPt, dualConnPt, interfaceType, eventName, dispid) \
	if (theApp.m_pAutoApp->ShouldQueueEvents()) \
		theApp.m_pAutoApp->EventQueueAdd((connPt), (dualConnPt), (HRESULT (_stdcall interfaceType::*)()) (&interfaceType::eventName), (dispid), EVENT_PARAM(VTS_NONE), NULL); \
	else \
	{

#define DS_QUEUE_EVENT1(connPt, dualConnPt, interfaceType, eventName, dispid, paramType, param1) \
	if (theApp.m_pAutoApp->ShouldQueueEvents()) \
	{ \
		DWORD dwArgs[2] = { (DWORD) (param1), 0 }; \
		theApp.m_pAutoApp->EventQueueAdd((connPt), (dualConnPt), (HRESULT (_stdcall interfaceType::*)()) (&interfaceType::eventName), (dispid), (paramType), dwArgs); \
	} \
	else \
	{

#define DS_QUEUE_EVENT2(connPt, dualConnPt, interfaceType, eventName, dispid, paramType, param1, param2) \
	if (theApp.m_pAutoApp->ShouldQueueEvents()) \
	{ \
		DWORD dwArgs[2] = { (DWORD) (param1), (DWORD) (param2) }; \
		theApp.m_pAutoApp->EventQueueAdd((connPt), (dualConnPt), (HRESULT (_stdcall interfaceType::*)()) (&interfaceType::eventName), (dispid), (paramType), dwArgs); \
	} \
	else \
	{


// Use THESE macros directly.  Note that the macros are named by whether the
//  event is cancelable, and by how many parameters the event takes.  If
//  an event is cancelable, the BOOL* parameter at the end IS COUNTED as
//  a parameter in naming these macros:
//		FIRE_(<cancelable?>)_EVENT(<# parameters, including Cancel Boolean>)
// PLEASE consult the documentation in dualimpl.doc (discussed at the top of this
//  file) before using these macros.  There are caveats.

// NOTE1!!! (11/26/96) DO NOT FIRE EVENTS WITH MORE THAN 2 PARAMETERS.
//  To fix DevStudio bug 16185, we need to queue events in certain
//  situations.  The queue is prepared only to store events which pass
//  0, 1, or 2 parameters.  Currently the devstudio events fall into
//  this range.  If a new event is added which passes >2 parameters, the
//  event queue code needs to change.  SIMILARLY, "CANCELABLE" EVENTS
//  ARE NOT SUPPORTED in the event queue in V5, since no devstudio
//  events are cancelable.
// NOTE2!!! (11/26/96) DO NOT FIRE EVENTS WHICH PASS A STRING AS A PARAMETER
//  (Same bug.)  The queue code doesn't yet know how to make local copies of string arguments
//  so that they're not destroyed by the event-firing code after the event is
//  queued but before the event is actually fired from the queue.  If an event
//  which passes a string argument is created, code needs to be added to copy and
//  destroy all string arguments.  See the FUTURE comments below in the definitions
//  of the event queue functions.

// 0 Parameters

#define DS_FIRE_EVENT0(connPt, dualConnPt, interfaceType, eventName, dispid) \
	DS_QUEUE_EVENT0(connPt, dualConnPt, interfaceType, eventName, dispid) \
	FireDispatchEvent(FALSE, connPt, dispid, EVENT_PARAM(VTS_NONE)); \
	BEGIN_FIRE_DUAL_EVENT(dualConnPt, interfaceType) \
	_pEvents->eventName(); \
	END_FIRE_DUAL_EVENT

// 1 Parameter

#define DS_FIRE_EVENT1(connPt, dualConnPt, interfaceType, eventName, dispid, paramType, param1) \
	DS_QUEUE_EVENT1(connPt, dualConnPt, interfaceType, eventName, dispid, paramType, param1) \
	FireDispatchEvent(FALSE, connPt, dispid, paramType, param1); \
	BEGIN_FIRE_DUAL_EVENT(dualConnPt, interfaceType) \
	_pEvents->eventName(param1); \
	END_FIRE_DUAL_EVENT

// 2 Parameters

#define DS_FIRE_EVENT2(connPt, dualConnPt, interfaceType, eventName, dispid, paramType, param1, param2) \
	DS_QUEUE_EVENT2(connPt, dualConnPt, interfaceType, eventName, dispid, paramType, param1, param2) \
	FireDispatchEvent(FALSE, connPt, dispid, paramType, param1, param2); \
	BEGIN_FIRE_DUAL_EVENT(dualConnPt, interfaceType) \
	_pEvents->eventName(param1, param2); \
	END_FIRE_DUAL_EVENT

// These macros add 10 reserved spaces in the vtbl of the IGenericDocument,
//  IGenericWindow, and IGenericProject interfaces.
#define DS_DECLARE_VTBL_PAD_10() \
    STDMETHOD(Reserved1)(THIS); \
    STDMETHOD(Reserved2)(THIS); \
    STDMETHOD(Reserved3)(THIS); \
    STDMETHOD(Reserved4)(THIS); \
    STDMETHOD(Reserved5)(THIS); \
    STDMETHOD(Reserved6)(THIS); \
    STDMETHOD(Reserved7)(THIS); \
    STDMETHOD(Reserved8)(THIS); \
    STDMETHOD(Reserved9)(THIS); \
    STDMETHOD(Reserved10)(THIS);

// Reserved spots will intentionally cause a crash to prevent anyone from calling them.
//  4 is my lucky number.
#define DS_IMPLEMENT_VTBL_PAD_10(className, interfaceName) \
	STDMETHODIMP className::X##interfaceName::Reserved1(THIS) { *(char*)0=4; return E_NOTIMPL; } \
	STDMETHODIMP className::X##interfaceName::Reserved2(THIS) { *(char*)0=4; return E_NOTIMPL; } \
	STDMETHODIMP className::X##interfaceName::Reserved3(THIS) { *(char*)0=4; return E_NOTIMPL; } \
	STDMETHODIMP className::X##interfaceName::Reserved4(THIS) { *(char*)0=4; return E_NOTIMPL; } \
	STDMETHODIMP className::X##interfaceName::Reserved5(THIS) { *(char*)0=4; return E_NOTIMPL; } \
	STDMETHODIMP className::X##interfaceName::Reserved6(THIS) { *(char*)0=4; return E_NOTIMPL; } \
	STDMETHODIMP className::X##interfaceName::Reserved7(THIS) { *(char*)0=4; return E_NOTIMPL; } \
	STDMETHODIMP className::X##interfaceName::Reserved8(THIS) { *(char*)0=4; return E_NOTIMPL; } \
	STDMETHODIMP className::X##interfaceName::Reserved9(THIS) { *(char*)0=4; return E_NOTIMPL; } \
	STDMETHODIMP className::X##interfaceName::Reserved10(THIS) { *(char*)0=4; return E_NOTIMPL; }

////////////////////////////////////////////////////////////////////////////////////////
//
// AUTOAPP.H --- This header was called autoapp.h
//

// The Application object is so special, it gets its own factory class.
//  The only difference between this and COleObjectFactory is that this
//  overrides OnCreateObject, so it calls CApplication::Create(), rather
//  than going through the MFC DYNCREATE stuff.  Thus, we're assured of only
//  having only one instance of the application object around.
class CApplicationFactory : public COleObjectFactory
{
public:
	CApplicationFactory(REFCLSID clsid, CRuntimeClass* pRuntimeClass,
		BOOL bMultiInstance, LPCTSTR lpszProgID)
			: COleObjectFactory(clsid, pRuntimeClass,
			bMultiInstance, lpszProgID) {}

	virtual CCmdTarget* OnCreateObject();
	virtual BOOL Register();
};

////////////////////////////////////////////////////////////////////////////////////////
// Event Queue

// [These structures should just be private types defined inside CApplication,
//  but that causes compiler ICEs since it's a template feature not yet
//  fully supported by our compiler.]

// Entries in the queue need to know the max # of args that an event
//  can pass.  If this number changes, some code needs to change as well
//  (see the MAX_EVENT_ARGS ASSERTs in dualimpl.cpp).
#define MAX_EVENT_ARGS 2

// Abstract base class of event queue entries
class CEventEntry
{
public:
	virtual void FireDispatchEvent() = 0;
	virtual void FireDualEvent() = 0;	// Type-dependent; defined in template derived class
	virtual void DestroyArgs() = 0;
};

// Class for each of the event queue's entries, templatized on the type of the
//  dual interface source (i.e., T=IApplicationEvents or IDebuggerEvents, etc.)
//  [This code needs to remain inlined, since the compiler support for templates
//  doesn't handle non-inlined member functions well.]
template <class T> class CTypedEventEntry : public CEventEntry
{
public:
	virtual void FireDispatchEvent()
	{
		if (m_cArgs == 0)
		{
			::FireDispatchEvent(FALSE, m_pDispCP, m_dispid,
				m_pbParams);
		}
		else if (m_cArgs == 1)
		{
			::FireDispatchEvent(FALSE, m_pDispCP, m_dispid,
				m_pbParams, m_dwArgs[0]);
		}
		else
		{
			ASSERT (m_cArgs == 2);
			ASSERT (MAX_EVENT_ARGS == 2);	// This code must change if MAX_EVENT_ARGS changes
			::FireDispatchEvent(FALSE, m_pDispCP, m_dispid,
				m_pbParams, m_dwArgs[0], m_dwArgs[1]);
		}
	}

	virtual void FireDualEvent()
	{
		const CPtrArray* pConnections = m_pDualCP->GetConnections();
		ASSERT(pConnections != NULL);
		int cConnections = pConnections->GetSize();

		for (int i = 0; i < cConnections; i++)
		{
			T *pEvents = (T*) (pConnections->GetAt(i)); \
			ASSERT(pEvents != NULL);
			
			typedef HRESULT (_stdcall T::*ONEARG) (DWORD);
			typedef HRESULT (_stdcall T::*TWOARGS) (DWORD, DWORD);

			if (m_cArgs == 0)
				(pEvents->*m_pDualFcn)();
			else if (m_cArgs == 1)
				(pEvents->* ((ONEARG) m_pDualFcn))(m_dwArgs[0]);
			else
			{
				ASSERT (m_cArgs == 2);
				ASSERT (MAX_EVENT_ARGS == 2);	// This code must change if MAX_EVENT_ARGS changes
				(pEvents->* ((TWOARGS) m_pDualFcn))(m_dwArgs[0], m_dwArgs[1]);
			}
		}
	}

	// FUTURE: We currently don't fire any events that pass strings.  If/when
	//  we do add such events, we must add code here to destroy a copy of the
	//  BSTR we made when we added the entry to the event queue.
	virtual void DestroyArgs()
	{
		if (m_pDispCP != NULL)
		{
			m_pDispCP->ExternalRelease();
			m_pDispCP = NULL;
		}
		if (m_pDualCP != NULL)
		{
			m_pDualCP->ExternalRelease();
			m_pDualCP = NULL;
		}

		for (int i=0; i < m_cArgs; i++)
		{
			if (m_pbParams[i] == VT_DISPATCH && m_dwArgs[i] != 0)
			{
				((LPDISPATCH) m_dwArgs[i])->Release();
				m_dwArgs[i] = 0;
			}
		}
	}

	// This is the actual information that's stored so we know how to
	//  fire the "event" that's stored in this structure.  These pieces
	//  describe what the arguments are, and how to fire both the dispatch
	//  and the dual events.
	CConnectionPoint* m_pDispCP;
	CConnectionPoint* m_pDualCP;
	HRESULT (_stdcall T::*m_pDualFcn)();	// Template-ized virtual function pointer member variable of an embedded template-ized class.  Say that four times fast.
	DISPID m_dispid;
	DWORD m_dwArgs[MAX_EVENT_ARGS];
	BYTE m_pbParams[MAX_EVENT_ARGS+1];
	unsigned char m_cArgs;
};

/////////////////////////////////////////////////////////////////////////////
// CApplication command target

interface IAddIn;
interface IAutoBld;

class CApplication : public CAutoObj
{
	DECLARE_DYNCREATE(CApplication)

	CApplication();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
    DECLARE_DUAL_ERRORINFO()

	static CApplication* Create();
	void Zombify();
	void Destroy();
	BOOL IsZombified(void) { return m_bZombified; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApplication)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CApplication();

	// Memembers for PrintToOutputWindow
	static UINT s_idOutputWindow;
	static BOOL s_bOutputWindow;

	// Some Application method calls delegate down to devaut1.pkg
	//  We access through these.
	IAddIn* m_pAddIn;
	IAutoBld* m_pBld;
	IAddIn* GetAddInInterface();
	IAutoBld* GetBldInterface();

    static CApplication* m_pAppObj;
    static DWORD m_dwCookie;
	BOOL m_bZombified;
	UINT m_nEnableModelessLevels;

	DS_BEGIN_DUAL_INTERFACE_PART(Application)
		STDMETHOD(get_Height)(THIS_ long FAR* Height);
		STDMETHOD(put_Height)(THIS_ long Height);
		STDMETHOD(get_Width)(THIS_ long FAR* Width);
		STDMETHOD(put_Width)(THIS_ long Width);
		STDMETHOD(get_Top)(THIS_ long FAR* Top);
		STDMETHOD(put_Top)(THIS_ long Top);
		STDMETHOD(get_Left)(THIS_ long FAR* Left) ;
		STDMETHOD(put_Left)(THIS_ long Left) ;
		STDMETHOD(get_Name)(THIS_ BSTR FAR* Name) ;
		STDMETHOD(get_Debugger)(THIS_ IDispatch * FAR* texteditor);
		STDMETHOD(get_Version)(THIS_ BSTR FAR* Version) ;
		STDMETHOD(get_Path)(THIS_ BSTR FAR* Path) ;
		STDMETHOD(get_CurrentDirectory)(THIS_ BSTR FAR* CurrentDirectory) ;
		STDMETHOD(put_CurrentDirectory)(THIS_ BSTR CurrentDirectory) ;
		STDMETHOD(get_FullName)(THIS_ BSTR FAR* FullName) ;
		STDMETHOD(get_Application)(THIS_ IDispatch * FAR* Application) ;
		STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* Parent) ;
		STDMETHOD(get_ActiveDocument)(THIS_ IDispatch * FAR* ActiveDocument) ;
		STDMETHOD(get_Windows)(THIS_ IDispatch * FAR* Windows) ;
		STDMETHOD(get_Documents)(THIS_ IDispatch * FAR* Documents) ;
		STDMETHOD(get_ActiveWindow)(THIS_ IDispatch * FAR* ActiveWindow) ;
		STDMETHOD(get_WindowState)(THIS_ DsWindowState FAR* windowstate);
		STDMETHOD(put_WindowState)(THIS_ DsWindowState windowstate);
		STDMETHOD(get_TextEditor)(THIS_ IDispatch * FAR* texteditor);
		STDMETHOD(get_Projects)(THIS_ IDispatch * FAR* Projects);
		STDMETHOD(get_ActiveConfiguration)(THIS_ IDispatch * FAR* ActiveConfiguration);
		STDMETHOD(put_ActiveConfiguration)(THIS_ IDispatch * ActiveConfiguration);
		STDMETHOD(get_ActiveProject)(THIS_ IDispatch * FAR* ActiveProject);
		STDMETHOD(put_ActiveProject)(THIS_ IDispatch * ActiveProject);
	    STDMETHOD(get_Visible)(THIS_ VARIANT_BOOL FAR* Visible);
	    STDMETHOD(put_Visible)(THIS_ VARIANT_BOOL Visible);
		STDMETHOD(put_Active)(THIS_ VARIANT_BOOL bActive) ;
		STDMETHOD(get_Active)(THIS_ VARIANT_BOOL FAR* pbActive) ;
		STDMETHOD(GetPackageExtension)(THIS_ BSTR szExtensionName, IDispatch * FAR* pExt);
		STDMETHOD(Quit)(THIS);
		STDMETHOD(PrintToOutputWindow)(THIS_ BSTR Message);
	    STDMETHOD(ExecuteCommand)(THIS_ BSTR szCommandName);
	    STDMETHOD(AddCommandBarButton)(THIS_ long nButtonType, BSTR szCmdName, long dwCookie);
	    STDMETHOD(AddKeyBinding)(THIS_ BSTR szKey, BSTR szCommandName, BSTR szEditor);
		STDMETHOD(Build)(THIS_ VARIANT configuration);
	    STDMETHOD(RebuildAll)(THIS_ VARIANT configuration);
	    STDMETHOD(ExecuteConfiguration)(THIS_ VARIANT Reserved);
		STDMETHOD(SetAddInInfo)(THIS_ long nInstanceHandle, IDispatch * pCmdDispatch, long nIDBitmapResourceMedium, long nIDBitmapResourceLarge, long dwCookie);
		STDMETHOD(AddCommand)(THIS_ BSTR szCmdName, BSTR szMethodName, long nBitmapOffset, long dwCookie, VARIANT_BOOL FAR* pbResult);
	    STDMETHOD(EnableModeless)(THIS_ VARIANT_BOOL bEnable);
		STDMETHOD(Clean)(THIS_ VARIANT configuration);
		STDMETHOD(get_Errors)(THIS_ long FAR* nErrors);
		STDMETHOD(get_Warnings)(THIS_ long FAR* nWarnings);
		STDMETHOD(AddProject)(THIS_ BSTR szName, BSTR szPath, BSTR szType, VARIANT_BOOL bAddDefaultFolders);
DS_END_DUAL_INTERFACE_PART(Application)

	DS_DECLARE_ENABLE_DUAL(CApplication, Application)

	// Generated message map functions
	//{{AFX_MSG(CApplication)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DS_DECLARE_OLECREATE(CApplicationFactory, CApplication)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CApplication)
	afx_msg long GetHeight();
	afx_msg void SetHeight(long nNewValue);
	afx_msg long GetWidth();
	afx_msg void SetWidth(long nNewValue);
	afx_msg long GetTop();
	afx_msg void SetTop(long nNewValue);
	afx_msg long GetLeft();
	afx_msg void SetLeft(long nNewValue);
	afx_msg BSTR GetName();
	afx_msg LPDISPATCH GetDebugger();
	afx_msg BSTR GetVersion();
	afx_msg BSTR GetPath();
	afx_msg BSTR GetCurrentDirectory();
	afx_msg void SetCurrentDirectory(LPCTSTR lpszNewValue);
	afx_msg BSTR GetFullName();
	afx_msg LPDISPATCH GetApplication();
	afx_msg LPDISPATCH GetParent();
	afx_msg LPDISPATCH GetActiveDocument();
	afx_msg LPDISPATCH GetWindows();
	afx_msg LPDISPATCH GetDocuments();
	afx_msg LPDISPATCH GetActiveWindow();
	//afx_msg long GetWindowState();
	//afx_msg void SetWindowState(long nNewValue);
	afx_msg LPDISPATCH GetTextEditor();
	afx_msg LPDISPATCH GetProjects();
	afx_msg LPDISPATCH GetActiveConfiguration();
	afx_msg void SetActiveConfiguration(LPDISPATCH newValue);
	afx_msg LPDISPATCH GetActiveProject();
	afx_msg void SetActiveProject(LPDISPATCH newValue);
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);
	afx_msg LPDISPATCH GetPackageExtension(LPCTSTR szExtensionName);
	afx_msg void Quit();
	afx_msg void PrintToOutputWindow(LPCTSTR Message);
	afx_msg void ExecuteCommand(LPCTSTR szCommandName);
	afx_msg void AddCommandBarButton(long nButtonType, LPCTSTR szCmdName, long dwCookie);
	afx_msg void AddKeyBinding(LPCTSTR szKey, LPCTSTR szCmdName, LPCTSTR szPrompt);
	afx_msg void Build(const VARIANT FAR& configuration);
	afx_msg void RebuildAll(const VARIANT FAR& configuration);
	afx_msg void ExecuteConfiguration(const VARIANT FAR& configuration);
	//}}AFX_DISPATCH
	afx_msg BOOL AddCommand(LPCTSTR szCommandName, LPCTSTR szMethodName, long nBitmapOffset, long dwCookie);
	afx_msg void SetAddInInfo(long nInstanceHandle, LPDISPATCH pCmdDispatch, long nIDBitmapResourceMedium, long nIDBitmapResourceLarge, long dwCookie);
	afx_msg void EnableModeless(BOOL bEnable);

	afx_msg BOOL GetActive();
	afx_msg void SetActive(BOOL bNewValue);

	afx_msg DsWindowState GetWindowState();
	afx_msg void SetWindowState(DsWindowState nNewValue);
	afx_msg void Clean(const VARIANT FAR& configuration);
	afx_msg long GetErrors();
	afx_msg long GetWarnings();
	afx_msg void AddProject(LPCTSTR szName, LPCTSTR szPath, LPCTSTR szType, VARIANT_BOOL bAddDefaultFolders);

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(ServiceProvider, IServiceProvider)
		STDMETHOD(QueryService) (REFGUID guidService, REFIID iid, void ** ppvObj);
	END_INTERFACE_PART(ServiceProvider)

protected:
	// Event queue implementation
	
	// The event queue fixes devstudio bug 16185, which causes a msgbox in one
	//  event handler to allow processing of windows messages we posted to
	//  ourselves, which can result in another event being thrown.  In this
	//  situation, we should not fire the event immediately.  Instead we now add it
	//  to the EventQueue, which is emptied after all macro/add-in message boxes
	//  have been dismissed, and also if the queue gets filled up (which is
	//  usually because the macro/add-in didn't pair its calls to EnableModeless
	//  correctly--EnableModeless is our cue that a message box is being
	//  displayed or was dismissed).

	// Queue size
	#define MAX_EVENT_QUEUE_ENTRIES 16

	BOOL m_bEventQueueEmptying;		// Flag to remember if we're emptying the queue
	int m_nEventQueueHead;
	int m_nEventQueueTail;

	void EventQueueInit();
	void EventQueueDestroy();
	void EventQueueAdd(CEventEntry* pEventEntry);
	CEventEntry* EventQueueRemove();// Caller must delete the returned CEventEntry*
	void EventQueueRemoveAll();		// Fires each event entry and destroys it
	CEventEntry* m_pEventQueue[MAX_EVENT_QUEUE_ENTRIES];

public:
	// Events

	// This is how clients will add an event to the queue.  It's called from
	//  the DS_FIRE_EVENT macros.  Type T represents the event dual interface
	//  (i.e., T=IApplicationEvents, IDebuggerEvents, etc.)
	// [Rather than taking a DWORD*, this should be a vararg function, consistent with
	//  FireDispatchEvent, whose latter arguments represent the event's parameters.
	//  However, that causes ICEs (devstudio 96 bug 17808), and the fixed compiler is not
	//  yet dropped, so this takes an array of DWORD-casted arguments at the end.]
	template<class T>
		void EventQueueAdd(CConnectionPoint* pDispCP, CConnectionPoint* pDualCP,
		HRESULT (_stdcall T::*pDualFcn)(), DISPID dispid, BYTE* pbParams, DWORD* pdwArgs)
	{
		CTypedEventEntry<T>* pEventEntry = new CTypedEventEntry<T>;

		pDispCP->ExternalAddRef();
		pEventEntry->m_pDispCP = pDispCP;

		pDualCP->ExternalAddRef();
		pEventEntry->m_pDualCP = pDualCP;

		pEventEntry->m_pDualFcn = pDualFcn;
		pEventEntry->m_dispid = dispid;

		// Fill in arguments (may be 0, 1, or 2 arguments)
		pEventEntry->m_cArgs = 0;
		if (pbParams != NULL)
		{
			const BYTE* pb = pbParams;
			while (*pb != 0)
			{
				ASSERT (pEventEntry->m_cArgs < MAX_EVENT_ARGS);

				// AddRef any dispatch arguments to this event (they're released
				//  after the event entry is dequeued)
				// FUTURE: We currently don't fire any events that pass strings.  If/when
				//  we do add such events, we must add code here to set aside a copy
				//  of the string, which we'll destroy after the evetn entry
				//  is dequeued
				if (*pb == VT_DISPATCH)
					((LPDISPATCH) pdwArgs[pEventEntry->m_cArgs])->AddRef();

				pEventEntry->m_dwArgs[pEventEntry->m_cArgs] = pdwArgs[pEventEntry->m_cArgs];
				pEventEntry->m_pbParams[pEventEntry->m_cArgs++] = *pb;
				pb++;
			}
		}
		pEventEntry->m_pbParams[pEventEntry->m_cArgs] = 0;	// pbParams ends in 0

		EventQueueAdd(pEventEntry);
	}

	// Queue events, rather than fire events, if a macro or add-in has a message
	//  box up.  This occurs when m_nEnableModelessLevels is nonzero.
	BOOL ShouldQueueEvents() { return m_nEnableModelessLevels != 0; }
		
	// Events we fire

	// Connection point for the dual event interface
	BEGIN_CONNECTION_PART(CApplication, ApplicationCP)
        CONNECTION_IID(IID_IApplicationEvents)
    END_CONNECTION_PART(ApplicationCP)

	// Connection point for the dispatch event interface
	BEGIN_CONNECTION_PART(CApplication, DispApplicationCP)
        CONNECTION_IID(IID_IDispApplicationEvents)
    END_CONNECTION_PART(DispApplicationCP)

    DECLARE_CONNECTION_MAP()

public:
	BOOL m_bMaximizeApp;		// Initialized in InitInstance, maintained in SetVisible

public:
	// Event firing functions

	void FireBeforeBuildStart()
		{ DS_FIRE_EVENT0(&m_xDispApplicationCP, &m_xApplicationCP, IApplicationEvents,
		BeforeBuildStart, 1); }
    void FireBuildFinish(long nNumErrors, long nNumWarnings)
		{ DS_FIRE_EVENT2(&m_xDispApplicationCP, &m_xApplicationCP, IApplicationEvents,
		BuildFinish, 2, EVENT_PARAM(VTS_I4  VTS_I4), nNumErrors, nNumWarnings); }
	void FireBeforeApplicationShutDown()
		{ DS_FIRE_EVENT0(&m_xDispApplicationCP, &m_xApplicationCP, IApplicationEvents,
		BeforeApplicationShutDown, 3); }
	void FireDocumentOpen(LPDISPATCH theDocument)
		{ DS_FIRE_EVENT1(&m_xDispApplicationCP, &m_xApplicationCP, IApplicationEvents,
		DocumentOpen, 4, EVENT_PARAM(VTS_DISPATCH), theDocument); }
	void FireBeforeDocumentClose(LPDISPATCH theDocument)
		{ DS_FIRE_EVENT1(&m_xDispApplicationCP, &m_xApplicationCP, IApplicationEvents,
		BeforeDocumentClose, 5, EVENT_PARAM(VTS_DISPATCH), theDocument); }
	void FireDocumentSave(LPDISPATCH theDocument)
		{ DS_FIRE_EVENT1(&m_xDispApplicationCP, &m_xApplicationCP, IApplicationEvents,
		DocumentSave, 6, EVENT_PARAM(VTS_DISPATCH), theDocument); }
	void FireNewDocument(LPDISPATCH theDocument)
		{ DS_FIRE_EVENT1(&m_xDispApplicationCP, &m_xApplicationCP, IApplicationEvents,
		NewDocument, 7, EVENT_PARAM(VTS_DISPATCH), theDocument); }
	void FireWindowActivate(LPDISPATCH theWindow)
		{ DS_FIRE_EVENT1(&m_xDispApplicationCP, &m_xApplicationCP, IApplicationEvents,
		WindowActivate, 8, EVENT_PARAM(VTS_DISPATCH), theWindow); }
	void FireWindowDeactivate(LPDISPATCH theWindow)
		{ DS_FIRE_EVENT1(&m_xDispApplicationCP, &m_xApplicationCP, IApplicationEvents,
		WindowDeactivate, 9, EVENT_PARAM(VTS_DISPATCH), theWindow); }
	void FireWorkspaceOpen()
		{ DS_FIRE_EVENT0(&m_xDispApplicationCP, &m_xApplicationCP, IApplicationEvents,
		WorkspaceOpen, 10); }
	void FireWorkspaceClose()
		{ DS_FIRE_EVENT0(&m_xDispApplicationCP, &m_xApplicationCP, IApplicationEvents,
		WorkspaceClose, 11); }
	void FireNewWorkspace()
		{ DS_FIRE_EVENT0(&m_xDispApplicationCP, &m_xApplicationCP, IApplicationEvents,
		NewWorkspace, 12); }
};

inline CCmdTarget* CApplicationFactory::OnCreateObject() { return CApplication::Create(); }

/////////////////////////////////////////////////////////////////////////////
// CAutoWindowObj command target

// This class is the abstract base class for automation objects that implement the 
// IGenericWindow interface and derived interfaces. When subclassing this object, you must 
// override the event firing functions to fire events as appropriate.

// CPartFrame contains a pointer to one of these objects, and uses it to fire events

class CAutoWindowObj : public CAutoObj
{
	friend class CMyOleDispatchImpl;
	DECLARE_DYNCREATE(CAutoWindowObj)

public:
	// Events we fire

	// Connection point for the dual event interface
	/* BEGIN_CONNECTION_PART(CAutoWindowObj, WindowCP)
        CONNECTION_IID(IID_IWindowEvents)
    END_CONNECTION_PART(WindowCP)

	// Connection point for the dispatch event interface
	BEGIN_CONNECTION_PART(CAutoWindowObj, DispWindowCP)
        CONNECTION_IID(IID_IDispWindowEvents)
    END_CONNECTION_PART(DispWindowCP)

    DECLARE_CONNECTION_MAP()*/

public:
	// Event firing functions
	void FireActivate()
		{ /*DS_FIRE_EVENT0(&m_xDispWindowCP, &m_xWindowCP, IWindowEvents,
		Activate, 1); */}
	void FireDeactivate()
		{ /*DS_FIRE_EVENT0(&m_xDispWindowCP, &m_xWindowCP, IWindowEvents,
		Deactivate, 2); */}

// Implementation
protected:
	virtual ~CAutoWindowObj();
public:
	static BOOL RemoveChangeIndicatorFromWindowCaption(CString &strCaption); // returns whether " *" was stripped or not
};

/////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
//
// AUTOCOL.H --- This header was called autocol.h
//
//DER: 12 Apr 96 - Changed IEnumVarient to store IDispatch*'s instead of
// CAutoObj pointers. The following typedef will keep you honest.
typedef CTypedPtrList<CPtrList, IDispatch*> CDispatchList;

/////////////////////////////////////////////////////////////////////////////
// PLEASE NOTE!!!
//
// This OLE automation collection is provided as an example collection.
//  Typically, you will need to copy the code that implements this collection
//  to implement your own collection class.  All code that is commented out
//  with the note "// UNCOMMENT IF COPYING" above it should be uncommented
//  in your copy.  After copying and uncommenting, you will have to modify
//  the class to implement your specific collection object's behavior.
// For an example of how this can be done, see the sample
//  package: pkgs\pkg\autoicol.*, which implements a collection of integers.
//
// IF the collection class you're implementing is a collection of objects,
//  AND you require no new members, AND no changes to any existing members
//  (e.g., such as changing the behavior of Index() to deal with strings
//  as indices as well as integers), then you may derive from this class
//  directly.
// For an example of how this can be done, see the sample
//  package: pkgs\pkg\autofoos.*.
//
// For information on how to implement an Automation Collection class,
//  please see dualimpl.doc, checked in to the VBA subproject of
//  the V5Spec project.  There's a link to it on
//  http://devstudio/devstudio/automation.



/////////////////////////////////////////////////////////////////////////////
// IDualAutoCollection interface

DECLARE_INTERFACE_(IDualObjs, IDispatch)
{
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

    /* IDualAutoCollection methods */
    STDMETHOD(get_Count)(THIS_ long FAR* Count) PURE;
    STDMETHOD(get__NewEnum)(THIS_ IUnknown * FAR* _NewEnum) PURE;
    STDMETHOD(Item)(THIS_ VARIANT index, IDispatch * FAR* Item) PURE;
};

class CEnumVariantObjs : public CCmdTarget
{
	DECLARE_DYNAMIC(CEnumVariantObjs)

// Attributes
public:
	CEnumVariantObjs(CDispatchList* pPtrlData);

// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnumVariantObjs)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

	// Implementation
public:
	BEGIN_INTERFACE_PART(EnumVariant, IEnumVARIANT)
		STDMETHOD(Next)(THIS_ unsigned long celt, VARIANT FAR* rgvar, unsigned long FAR* pceltFetched);
		STDMETHOD(Skip)(THIS_ unsigned long celt) ;
		STDMETHOD(Reset)(THIS) ;
		STDMETHOD(Clone)(THIS_ IEnumVARIANT FAR* FAR* ppenum) ;
	END_INTERFACE_PART(EnumVariant)    


protected:
	~CEnumVariantObjs();
	POSITION m_posCurrent;
	CDispatchList* m_pPtrlData;

	// Generated message map functions
	//{{AFX_MSG(CEnumVariantObjs)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	DECLARE_INTERFACE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CAutoCollection command target

class CAutoCollection : public CAutoObj
{
	friend class CEnumVariantObjs;

	DECLARE_DYNCREATE(CAutoCollection)

// Attributes
public:
	CAutoCollection(CDispatchList* pPtrlData);


// Operations
public:
	// UNCOMMENT IF COPYING
	// See note at the top of ide\include\autocol.h for info on how
	//  to use this collection class as an example
    //DECLARE_DUAL_ERRORINFO()


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoCollection)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDispatchList* m_pPtrlData;
	virtual ~CAutoCollection();

	DS_BEGIN_DUAL_INTERFACE_PART(DualObjs)
		STDMETHOD(get_Count)(THIS_ long FAR* Count);
		STDMETHOD(get__NewEnum)(THIS_ IUnknown * FAR* _NewEnum);
	    STDMETHOD(Item)(THIS_ VARIANT index, IDispatch * FAR* Item);
	DS_END_DUAL_INTERFACE_PART(DualObjs)

	DS_DECLARE_ENABLE_DUAL(CAutoCollection, DualObjs)

	// Generated message map functions
	//{{AFX_MSG(CAutoCollection)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAutoCollection)
	afx_msg long GetCount();
	afx_msg LPDISPATCH Item(const VARIANT FAR& index);
	//}}AFX_DISPATCH
	afx_msg LPUNKNOWN _NewEnum();
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//
// AutoWins.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAutoWindows command target

class CAutoWindows : public CAutoObj
{
	friend CApplication ;	
	//DECLARE_DYNCREATE(CAutoWindows)
	DECLARE_DYNAMIC(CAutoWindows)

	// CAutoWindows can operate in two modes: MDI mode and Owned mode.
	// Owned mode is the only mode used by packages, such as the edit packae.
	// In owned mode, Arrange and CloseAll only affect the members actually
	// stored in the array which are the windows owned by a particular document.
	// In MDI mode, which is used only by Application.Windows, Arrange and 
	// CloseAll operator over all MDI child windows whether they are in the
	// collection or not.
	// MDI mode is determined by the status of pOwnerDoc. 
	// If m_pOwnerDoc is NULL, we will work with the entire MDI space.
	// If m_pOwnerDoc is non-NULL, it points to a document whose windows we will use.
	CAutoWindows(CPartDoc* pOwnerDoc);           // protected constructor used by dynamic creation

// Attributes
public:

protected:
	// See comment above.
	CPartDoc* m_pOwnerDoc ;

	void DeleteCollection() ; // Delete the collection.
	void Sync() ;	// Update the collection.

// Operations
public:
	DECLARE_DUAL_ERRORINFO()

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoWindows)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CAutoWindows();

	CDispatchList* m_pPtrlData;

	DS_BEGIN_DUAL_INTERFACE_PART(Windows)
		STDMETHOD(get_Count)(THIS_ long FAR* Count);
		STDMETHOD(get_Application)(THIS_ IDispatch * FAR* ppApplication) ;
		STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* ppParent) ;
		STDMETHOD(get__NewEnum)(THIS_ IUnknown * FAR* _NewEnum) ;
		STDMETHOD(Item)(THIS_ VARIANT index, IDispatch * FAR* Item) ;
		STDMETHOD(Arrange)(THIS_ VARIANT vtArrangeStyle) ;
		STDMETHOD(CloseAll)(THIS_ VARIANT vtSaveChanges, DsSaveStatus FAR* pSaved) ;
	DS_END_DUAL_INTERFACE_PART(Windows)

	DS_DECLARE_ENABLE_DUAL(CAutoWindows, Windows)

	// Generated message map functions
	//{{AFX_MSG(CAutoWindows)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// See DualImpl.doc -- DECLARE_OLECREATE(CAutoWindows)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAutoWindows)
	afx_msg long GetCount();
	afx_msg LPDISPATCH GetApplication();
	afx_msg LPDISPATCH GetParent();
	afx_msg LPDISPATCH Item(const VARIANT FAR& index);
	afx_msg void Arrange(const VARIANT FAR& vtArrangeStyle) ;
	//}}AFX_DISPATCH
	afx_msg LPUNKNOWN _NewEnum();
	afx_msg DsSaveStatus CloseAll(const VARIANT FAR& vtSaveChanges);

	// Not In Spec -- Removed afx_msg LPDISPATCH Add(BSTR FAR* docType);

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()


};

/////////////////////////////////////////////////////////////////////////////


/* begin dual interface implementation macros */

/*

	A note on the implementation strategy here.  Most of dual interface delegation methods are
	identical except for the handler & interface names, so instead of redundantly expanding every
	single delegation, we use macros to factor out much common functionality.  These could be
	factored even further, although that would probably obscure these macros further.

 DELEGATE_NO_PARAMS delegates methods with no parameters to pass on
 DELEGATE_NO_PARAMS_WITH_RETVAL delegates methods with no parameters but a return value
 DELEGATE_NO_PARAMS_WITH_BOOL_RETVAL delegates methods with no parameters but a VARIANT_BOOL return value
 DELEGATE_ONE_PARAM delegates methods with a single parameter
 DELEGATE_ONE_PARAM_WITH_RETVAL delegates methods with one parameter and a return value
 DELEGATE_ONE_PARAM_WITH_BOOL_RETVAL delegates methods with one parameter and a VARIANT_BOOL return value
 DELEGATE_TWO_PARAMS delegates methods with two parameters
 DELEGATE_TWO_PARAMS_WITH_RETVAL
 DELEGATE_BSTR_PUT delegates the setting of a BSTR property.  Because we have both the
		_Text and Text methods, we allow for the prop name & handler name to be different.
		Also, BSTR's need additional BSTR->LPCTSTR mapping code, so different macros.
 DELEGATE_BSTR_GET delegates the getting of a BSTR property.  Because we have both the
		_Text and Text methods, we allow for the prop name & handler name to be different.
		Also, BSTR's need additional BSTR->LPCTSTR mapping code, so different macros.
 DELEGATE_BOOL_PUT delegates the setting for VARIANT_BOOLs
 DELEGATE_BOOL_GET delegate the getting for VARIANT_BOOLs
 DELEGATE_PUT delegates the setting of a non-BSTR property
 DELEGATE_GET delegates the getting of a non-BSTR property
 
	Sample macro usage:

DELEGATE_GET(CAutoTextSel,TextSelection,Parent,LPDISPATCH)
DELEGATE_NO_PARAMS(CAutoTextDoc,TextDocument,Activate)

*/



// DELEGATE_NO_PARAMS delegates methods with no parameters to pass on

#define DELEGATE_NO_PARAMS(CLASS, INTERFACE, NAME) \
STDMETHODIMP CLASS::X##INTERFACE##::NAME(void) {	METHOD_PROLOGUE(CLASS, Dispatch)	\
	TRY_DUAL(IID_I##INTERFACE##) \
	{	\
		pThis->NAME(); \
		return NOERROR;	\
	}	\
	CATCH_ALL_DUAL \
}

// DELEGATE_NO_PARAMS_WITH_RETVAL delegates methods with no parameters but a return value

#define DELEGATE_NO_PARAMS_WITH_RETVAL(CLASS, INTERFACE, NAME, RETVAL_TYPE) \
STDMETHODIMP CLASS::X##INTERFACE##::NAME(THIS_ RETVAL_TYPE *pRet) {	METHOD_PROLOGUE(CLASS, Dispatch)	\
	TRY_DUAL(IID_I##INTERFACE##) \
	{	\
		*pRet = pThis->NAME();	\
		return NOERROR;	\
	}	\
	CATCH_ALL_DUAL \
}

// DELEGATE_NO_PARAMS_WITH_BOOL_RETVAL delegates methods with no parameters but a VARIANT_BOOL return value

#define DELEGATE_NO_PARAMS_WITH_BOOL_RETVAL(CLASS, INTERFACE, NAME) \
STDMETHODIMP CLASS::X##INTERFACE##::NAME(THIS_ VARIANT_BOOL *pRet) {	METHOD_PROLOGUE(CLASS, Dispatch)	\
	TRY_DUAL(IID_I##INTERFACE##) \
	{	\
		BOOL b = pThis->NAME();	\
		*pRet = ((b == TRUE) ? VARIANT_TRUE : VARIANT_FALSE) ;\
		return NOERROR;	\
	}	\
	CATCH_ALL_DUAL \
}

// DELEGATE_ONE_PARAM delegates methods with a single parameter

#define DELEGATE_ONE_PARAM(CLASS, INTERFACE, NAME, PARAM_TYPE) \
STDMETHODIMP CLASS::X##INTERFACE##::NAME(THIS_ PARAM_TYPE Param) { METHOD_PROLOGUE(CLASS, Dispatch) \
	TRY_DUAL(IID_I##INTERFACE##)	\
	{	\
		pThis->NAME(Param);	\
		return NOERROR;	\
	}	\
	CATCH_ALL_DUAL	\
}

// DELEGATE_ONE_PARAM_WITH_RETVAL delegates methods with a single parameter and a return value

#define DELEGATE_ONE_PARAM_WITH_RETVAL(CLASS, INTERFACE, NAME, PARAM_TYPE, RETVAL_TYPE) \
STDMETHODIMP CLASS::X##INTERFACE##::NAME(THIS_ PARAM_TYPE Param, RETVAL_TYPE *pRet) { METHOD_PROLOGUE(CLASS, Dispatch) \
	TRY_DUAL(IID_I##INTERFACE##)	\
	{	\
		*pRet = pThis->NAME(Param);	\
		return NOERROR;	\
	}	\
	CATCH_ALL_DUAL	\
}


// DELEGATE_ONE_PARAM_WITH_BOOL_RETVAL delegates methods with a single parameter and a BOOL return value

#define DELEGATE_ONE_PARAM_WITH_BOOL_RETVAL(CLASS, INTERFACE, NAME, PARAM_TYPE) \
STDMETHODIMP CLASS::X##INTERFACE##::NAME(THIS_ PARAM_TYPE Param, VARIANT_BOOL *plRet) { METHOD_PROLOGUE(CLASS, Dispatch) \
	TRY_DUAL(IID_I##INTERFACE##)	\
	{	\
		BOOL b = pThis->NAME(Param);										\
		*plRet = ((b == TRUE) ? VARIANT_TRUE : VARIANT_FALSE) ;				\
		return NOERROR;	\
	}	\
	CATCH_ALL_DUAL	\
}



// DELEGATE_TWO_PARAMS delegates methods with two parameters

#define DELEGATE_TWO_PARAMS(CLASS, INTERFACE, NAME, PARAM1_TYPE, PARAM2_TYPE) \
STDMETHODIMP CLASS::X##INTERFACE##::NAME(THIS_ PARAM1_TYPE Param1, PARAM2_TYPE Param2) { METHOD_PROLOGUE(CLASS, Dispatch) \
	TRY_DUAL(IID_I##INTERFACE##)	\
	{	\
		pThis->NAME(Param1, Param2);	\
		return NOERROR;	\
	}	\
	CATCH_ALL_DUAL	\
}

// DELEGATE_TWO_PARAMS_WITH_RETVAL delegates methods with two parameters
#define DELEGATE_TWO_PARAMS_WITH_RETVAL(CLASS, INTERFACE, NAME, PARAM1_TYPE, PARAM2_TYPE, RETVAL_TYPE) \
STDMETHODIMP CLASS::X##INTERFACE##::NAME(THIS_ PARAM1_TYPE Param1, PARAM2_TYPE Param2, RETVAL_TYPE *pRet) { METHOD_PROLOGUE(CLASS, Dispatch) \
	TRY_DUAL(IID_I##INTERFACE##)	\
	{	\
		*pRet = pThis->NAME(Param1, Param2);	\
		return NOERROR;	\
	}	\
	CATCH_ALL_DUAL	\
}

// DELEGATE_BSTR_PUT delegates the setting of a BSTR property.  Because we have both the
// _Text and Text methods, we allow for the prop name & handler name to be different.
// Also, BSTR's need additional BSTR->LPCTSTR mapping code, so different macros.

#define DELEGATE_BSTR_PUT(CLASS,INTERFACE,PROP,HANDLER) \
STDMETHODIMP CLASS::X##INTERFACE##::put_ ## PROP(THIS_ BSTR bstrText) { METHOD_PROLOGUE(CLASS, Dispatch) \
	TRY_DUAL(IID_I##INTERFACE##)	\
	{	\
		CString strText(bstrText); \
		pThis->Set ## HANDLER(strText);	\
		return NOERROR;	\
	}	\
	CATCH_ALL_DUAL	\
}

// DELEGATE_BSTR_GET delegates the getting of a BSTR property.  Because we have both the
// _Text and Text methods, we allow for the prop name & handler name to be different.
// Also, BSTR's need additional BSTR->LPCTSTR mapping code, so different macros.

#define DELEGATE_BSTR_GET(CLASS,INTERFACE,PROP,HANDLER) \
STDMETHODIMP CLASS::X##INTERFACE##::get_ ## PROP(THIS_ BSTR *pbstrText) { METHOD_PROLOGUE(CLASS, Dispatch) \
	TRY_DUAL(IID_I##INTERFACE##)	\
	{	\
		*pbstrText = pThis->Get ## HANDLER();	\
		return NOERROR;	\
	}	\
	CATCH_ALL_DUAL	\
}

// DELEGATE_PUT delegates the setting of a non-BSTR property

#define DELEGATE_PUT(CLASS,INTERFACE,PROP,TYPE) \
STDMETHODIMP CLASS::X##INTERFACE##::put_ ## PROP(THIS_ TYPE lVal) { METHOD_PROLOGUE(CLASS, Dispatch) \
	TRY_DUAL(IID_I##INTERFACE##)	\
	{	\
		pThis->Set ## PROP(lVal);	\
		return NOERROR;	\
	}	\
	CATCH_ALL_DUAL	\
}

// DELEGATE_GET delegates the getting of a non-BSTR property

#define DELEGATE_GET(CLASS,INTERFACE,PROP,TYPE) \
STDMETHODIMP CLASS::X##INTERFACE##::get_ ## PROP(THIS_ TYPE *plRet) { METHOD_PROLOGUE(CLASS, Dispatch) \
	TRY_DUAL(IID_I##INTERFACE##)	\
	{	\
		*plRet = pThis->Get ## PROP();	\
		return NOERROR;	\
	}	\
	CATCH_ALL_DUAL	\
}

// DELEGATE_BOOL_GET delegates the getting of VARIANT_BOOL property.

#define DELEGATE_BOOL_GET(CLASS,INTERFACE,PROP)								\
STDMETHODIMP CLASS::X##INTERFACE##::get_ ## PROP(THIS_ VARIANT_BOOL* plRet) \
{ METHOD_PROLOGUE(CLASS, Dispatch)											\
	TRY_DUAL(IID_I##INTERFACE##)											\
	{																		\
		BOOL b = pThis->Get ## PROP();										\
		*plRet = ((b == TRUE) ? VARIANT_TRUE : VARIANT_FALSE) ;				\
		return NOERROR;														\
	}																		\
	CATCH_ALL_DUAL															\
}

// DELEGATE_BOOL_PUT delegates the setting of VARIANT_BOOL properties

#define DELEGATE_BOOL_PUT(CLASS,INTERFACE,PROP) \
STDMETHODIMP CLASS::X##INTERFACE##::put_ ## PROP(THIS_ VARIANT_BOOL bVal) { METHOD_PROLOGUE(CLASS, Dispatch) \
	TRY_DUAL(IID_I##INTERFACE##)	\
	{	\
		BOOL b = (bVal == VARIANT_TRUE) ? TRUE : FALSE ; \
		pThis->Set ## PROP(b);	\
		return NOERROR;	\
	}	\
	CATCH_ALL_DUAL	\
}

// old macros for backward compatibility

//#define DELEGATE_NO_PARAMS_BOOL_RETVAL(c,i,n) DELEGATE_NO_PARAMS_WITH_RETVAL(c,i,n,VARIANT_BOOL)
#define DELEGATE_NO_PARAMS_IDISPATCH_RETVAL(c,i,n) DELEGATE_NO_PARAMS_WITH_RETVAL(c,i,n,LPDISPATCH)
#define DELEGATE_VARIANT_PARAM(c,i,n) DELEGATE_ONE_PARAM(c,i,n,VARIANT)
#define DELEGATE_LONG_PUT(c,i,p) DELEGATE_PUT(c,i,n,long)
#define DELEGATE_LONG_GET(c,i,p) DELEGATE_PUT(c,i,n,long)
// Incorrect #define DELEGATE_BOOL_PUT(c,i,p) DELEGATE_PUT(c,i,n,VARIANT_BOOL)
// Incorrect #define DELEGATE_BOOL_GET(c,i,p) DELEGATE_PUT(c,i,n,VARIANT_BOOL)
#define DELEGATE_IDISPATCH_GET(c,i,p) DELEGATE_PUT(c,i,n,LPDISPATCH)

/* end dual interface implementation macros */

/////////////////////////////////////////////////////////////////////////////
//  ShellAutomationState Object
//
//
//
/*
If you were writing a macro for DevStudio, you wouldn't want to see errors 
pop up in message boxes. If would like to get errors back which you could handle 
programmatically, wouldn't you? Well, that's that this mail is about.
A macro calls a Shell Automation methods or properties which calls functions in the 
shell and in YOUR package. The functions which the shell call should NOT display UI, 
but instead return error messages to the Shell Automation code. This includes YOUR functions.

In addition, DevStudio should not display dialog boxes asking the user if they want 
to save changes, if the function was invoked from a macro.



Which functions are affected?
-----------------------------

Currently, the Shell Automation model directly calls the following functions:
	CPartDoc::DoSave
	CPartDoc::OnCloseDocument
	CMainFrame::OnWindowCloseAll
	CPartTemplate::OpenDocumentFile
	theApp.SaveAll
	theApp.OpenFile

This results in the following functions getting called indirectly:
	CPartDoc::OnSaveDocument
	CPartDoc::SaveModified

  

What you need to do.
--------------------

1) Any function reachable from the functions listed in the first section above should 
not display UI. I have already done the main ones, you are left with the esoteric ones.

2) In some cases, I returned DS_E_UNKNOWN for the error message. You might want to 
provide a more detailed error message.



When to display UI
------------------

The following code shows how to determine if you should display UI.

#include "utilauto.h"
.
.
.

if (g_pAutomationState->DisplayUI())
{
	AfxMessageBox("Its okay to display UI.") ;
}
else
{
	TRACE0("Don't display any ui.") ;
}

BONUS: If you use FileErrorMessageBox, MsgBox, or any of its cousins, I already 
correctly check the automation state.



When to discard changes
-----------------------

If g_pAutomationState->DisplayUI is FALSE, you should NOT ask the user if they 
want to save changes. Instead you should use the SaveChanges functions.

if (g_pAutomationState->DisplayUI())
{
	//Ask the user if they want to save changes.
}
else
{
	// Don't ask the user if the want to save changes.
	if (g_pAutomationState->SaveChanges())
	{
		//The macro wants changes saved.
	}
	else
	{
		//The macro wants changes discarded.
	}
}

This kind of code happens most often in SaveModified. I have modified all of these.



How to return errors
--------------------

There are three ways to return errors.

1) The first way is to use an HRESULT defined specifically for the Shell Automation
Model. This is not for random HRESULTs. It needs to be defined in one of the *.defs.h 
headers in pkgs\include\objmodel. See me if you want to add some HRESULTs. 

	g_pAutomationState->SetError(DS_E_UNKNOWN) ;

2) Use a CFileException type error.

	g_pAutomationState->SetFileError(CFileException::badPath) ;

3) Use the global SetFileError in msgboxes.h.

BONUS: If you use SetFileError and FileErrorMessageBox, everything is done for you!!!




*/

interface IDsAutomationState
{
	// If TRUE, you can display UI. If FALSE do not display any UI.
	virtual BOOL DisplayUI() = 0;

	// If TRUE, you should save changes, if FALSE you should discard changes.
	virtual BOOL SaveChanges()  = 0 ;

	// Use one of the the following functions to set the error code.
	//
	// Set the error with a CFileException::m_cause type error. 
	// Translates the error into an HRESULT for automation purposes.
	// Used by the global function SetError
	virtual void SetFileError(int FileExceptionError) = 0;

	// Set the return value of the current operation.
	virtual void SetError(HRESULT hResult) = 0;

	// Set Prompt Canceled ;
	virtual void SetSavePromptCanceled() = 0 ;
	
	virtual void EnableUI() = 0 ;
	virtual void DisableUI() = 0 ;
};

// Use this to access the global shell automation state.
extern AFX_DATA IDsAutomationState* g_pAutomationState;
/////////////////////////////////////////////////////////////////////////////
//  Variant Conversion template function.
//
// PARAMETERS:
//	vInput			- The variant that you want to convert.
//	tReturnValue	- The variable you want to hold the converted value.
//  tDefaultValue	- The default value for the variable if the user didn't set it.
//						You should be explicit for the compiler to resolve the function.
//	vt				- The type of the paramter. The following types are supported:
//						VT_BOOL, VT_I4, VT_BSTR
//
// RETURN VALUES:
//	S_OK	- Successful conversion.
//	S_FALSE	- The default value was used.
//	E_FAIL	- The conversion failed...only happens if bThrowException is FALSE.
//
// Class T is the type of the out value.
//
//

ConvertVariantToCString(/*in*/	const VARIANT &vInput, 
					   /*out*/	CString& tReturnValue, 
					   /*in*/	CString tDefaultValue, 
					   /*in*/	BOOL bThrowException = TRUE);


ConvertVariantToLong( /*in*/	const VARIANT &vInput, 
					   /*out*/	long& tReturnValue, 
					   /*in*/	long tDefaultValue, 
					   /*in*/	BOOL bThrowException = TRUE);

ConvertVariantToBOOL(/*in*/	const VARIANT &vInput, 
					   /*out*/	BOOL& tReturnValue, 
					   /*in*/	BOOL tDefaultValue, 
					   /*in*/	BOOL bThrowException = TRUE);

#undef AFX_DATA
#define AFX_DATA NEAR



/*

	The following section contains declarations for some general helpers that are
	useful for doing macro recording.  Created from pkgs\edit\genrec.h. [CFlaat]

*/


class CRecArg
{
public: // methods

	virtual ~CRecArg(void) {}

	virtual void GetText(CString &str) = 0;
	virtual CRecArg *Clone(void) = 0;
};

class CIntArg : public CRecArg
{
protected: // data

	int m_nData;

private: // unused ctor

	CIntArg(void) { ASSERT(FALSE); /* this ctor shouldn't be used!! */ }

public: // methods
	
	CIntArg(int nVal) { m_nData = nVal; }

	virtual void GetText(CString &str) { str.Format("%d", m_nData); }
	virtual CRecArg *Clone(void) { return new CIntArg(m_nData); }

	int GetValue(void) { return m_nData; }
	void SetValue(int n) { m_nData = n; }
};


#if 0
// this class isn't used yet, but probably will be down the line
class CRawStringArg : public CRecArg
{
protected: // data

	CString m_strData;

private: // unused ctor

	CRawStringArg(void) { ASSERT(FALSE); /* this ctor shouldn't be used!! */ }
	virtual ~CRawStringArg(void) {}

public: // methods

	CRawStringArg(LPCTSTR szData) : m_strData(szData) {}

	virtual void GetText(CString &str) { str = m_strData; }
	virtual CRecArg *Clone(void) { return new CRawStringArg(m_strData); }
};
#endif

class CQuotedStringArg : public CRecArg
{
protected: // data

	CString m_strData;

private: // unused ctor

	CQuotedStringArg(void) { ASSERT(FALSE); /* this ctor shouldn't be used!! */ }

protected: // internal methods
	void GetQuotedBasicString(CString &strOut, LPCTSTR szData);

public: // methods
	
	CQuotedStringArg(LPCTSTR szData) : m_strData(szData) {}
	virtual ~CQuotedStringArg(void) {}

	virtual void GetText(CString &str) { GetQuotedBasicString(str, m_strData); }
	//{ str = (_T('\"') + m_strData) + _T('\"'); }
	virtual CRecArg *Clone(void) { return new CQuotedStringArg(m_strData); }

	LPCTSTR GetValue(void) { return m_strData; }
	void SetValue(LPCTSTR sz) { m_strData = sz; }
};

/*
	CGeneralRecorder
	================

	Some notes are due on this class.  Its purpose is to be a base class to be
	inherited from by specialized recording classes.  It's not a base class with
	a specific vtable-defined interface for two reasons.  First, we want type
	safety with the enum param strategy we use here.  Second, we don't want the
	overhead of a virtual function, since these methods generally get called with
	every operation, not just when recording is on (since the code-emitting fn's
	test for recording internally).

	For the future we may want to make the interfaces be more uniform and then
	merge IMacroRecorder and this class to create a more handy-dandy recording
	utility for package partners.  But for now, the two will stand separately.
*/

class CGeneralRecorder
{
protected: // methods

	virtual void SetMacRec(IMacroRecorder *pMacRec) = 0;
	virtual IMacroRecorder *GetMacRec(void) const = 0;
	virtual LPCTSTR GetPkgName(void) const = 0;

public: // methods

	CGeneralRecorder(void) {}
	virtual ~CGeneralRecorder(void) {}

	BOOL ShouldEmitCode(void) const { return theApp.ShouldRecordText(); }
	BOOL InRecordingMode() const { return theApp.GetRecordState() == CTheApp::RecordOn; }
	void RecordNothing(void) { GetMacRec()->RecordText(0, GetPkgName()); }


};




#endif //__UTILAUTO_H__
