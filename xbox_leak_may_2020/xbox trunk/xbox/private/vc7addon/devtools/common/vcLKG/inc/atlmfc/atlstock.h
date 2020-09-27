// This is a part of the Active Template Library.
// Copyright (C) 1996-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLSTOCK_H__
#define __ATLSTOCK_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#include "atlsafe.h"

namespace ATL
{

/////////////////////////////////////////////////////////////////////////////
// Stock Property types and defines

enum stkDISPIDS
{
	// custom properties
	DISPID_ALIGNMENT = 1,
	DISPID_BUTTONALIGNMENT,
	DISPID_HIDESELECTION,
	DISPID_LOCKED,
	DISPID_COMBOSTYLE,
	DISPID_INTEGRALHEIGHT,
	DISPID_SORTED,

	// custom events
	DISPID_CHANGE = 101
};

enum stkALIGN
{
	alLeft,
	alCenter,
	alRight
};

enum stkBUTTALIGN
{
	baLeft,
	baRight
};

enum stkAPPEAR
{
	apFlat,
	apThreed
};

enum stkBORDER
{
	boNone,
	boSingle,
	boSizable,
	boFixedDialog,
	boFixedToolWindow,
	boSizableToolWindow
};

enum stkMULTISELECT
{
	msNone,
	msSimple,
	msExtended
};

enum stkCOMBOSTYLE
{
	csDropdownCombo,
	csSimpleCombo,
	csDropdownList
};

enum stkMOUSEPTR
{
	mpDefault = 0,
	mpArrow,
	mpCross,
	mpQuestion,
	mpIbeam,
	mpNodrop,
	mpSizeAll,
	mpSizeNESW,
	mpSizeNS,
	mpSizeNWSE,
	mpSizeWE,
	mpUpArrow,
	mpHourglass,
	mpCustom = 99
};

enum stkSCROLLBARS
{
	sbNone,
	sbHorizontal,
	sbVertical,
	sbBoth
};

#define DECL_PROP(name, type) \
	virtual HRESULT STDMETHODCALLTYPE get_##name(type *m_data) \
	{ \
		return m_prop##name.GetData(m_data); \
	} \
	virtual HRESULT STDMETHODCALLTYPE put_##name(type m_data) \
	{ \
		return m_prop##name.SetVal(m_data); \
	} 

#define DECL_PROP_RO(name, type) \
	virtual HRESULT STDMETHODCALLTYPE get_##name(type *m_data) \
	{ \
		return m_prop##name.GetData(m_data); \
	} 

#define DECL_PROP_REF(name, type) \
	virtual HRESULT STDMETHODCALLTYPE get_##name(type *m_data) \
	{ \
		return m_prop##name.GetData(m_data); \
	} \
	virtual HRESULT STDMETHODCALLTYPE put_##name(type m_data) \
	{ \
		return m_prop##name.SetVal(m_data, true); \
	} \
	virtual HRESULT STDMETHODCALLTYPE putref_##name(type m_data) \
	{ \
		return m_prop##name.SetVal(m_data, false); \
	} 

#define GETCOLORVAL(c) ((c&0x80000000) ? GetSysColor(c&0x7fffffff) : c)
#define SYSCOLOR(c) (c|0x80000000)

_declspec(selectany) TCHAR *StdCursors[] = {
	IDC_APPSTARTING, IDC_ARROW, IDC_CROSS,
	IDC_HELP, IDC_IBEAM, IDC_NO, IDC_SIZEALL,
	IDC_SIZENESW, IDC_SIZENS, IDC_SIZENWSE, IDC_SIZEWE, IDC_UPARROW, IDC_WAIT
};

#define DEFINE_GETSHIFTVAL()	\
static int GetShiftVal() \
{ \
	int shiftval = 0; \
	if ( 0x80000000 & GetKeyState(VK_SHIFT)) \
		shiftval |= 1; \
	if ( 0x80000000 & GetKeyState(VK_CONTROL)) \
		shiftval |= 2; \
	if ( 0x80000000 & GetKeyState(VK_MENU)) \
		shiftval |= 4; \
	\
	return shiftval; \
}

/////////////////////////////////////////////////////////////////////////////
// Stock Property classes

class CStockFakeControl		// allows normal ATL windows to use stock properties
{
public:
	// dummy routines for stock properties (since we are not a control)
	BOOL m_bRequiresSave;
	HRESULT FireOnRequestEdit(DISPID) { return S_OK; }
	HRESULT FireOnChanged(DISPID) { return S_OK; }
	HRESULT FireViewChange() { return S_OK; }
	HRESULT SendOnDataChange(DISPID) { return S_OK; }
    BOOL DesignMode() { return FALSE; }
	HRESULT GetAmbientForeColor(int) { return E_FAIL; }
	HRESULT GetAmbientBackColor(int) { return E_FAIL; }
	HRESULT GetAmbientFontDisp(IFontDisp **) { return E_FAIL; }
	void RecreateWindow()
	{
		//REVIEW
		//Destroy();
		//Create();
	}
};

//	CPropertyHolder usages for Stock Properties:
//		CPropertyHolder<long>
//		CPropertyHolder<short>
//		CPropertyHolder<OLECOLOR>
//		CPropertyHolder<BSTR, CComBSTR>
//
//	CPropertyHolder specializations
//		CPropertyHolder<VARIANT_BOOL, bool>
//		CPropertyHolder<IFontDisp *, CComPtr<IFontDisp>>
//		CPropertyHolder<IPictureDisp *, CComPtr<IPictureDisp>>
template <class PARAMTYPE, class STORETYPE=PARAMTYPE>
class CPropertyHolder
{
public:
	STORETYPE m_data;

	// Most specializations will change "GetData()" and "StoreData()".
	// putref-capable properties need to supply "CloneData()" for "put" operations

	inline HRESULT STDMETHODCALLTYPE GetData(PARAMTYPE *pRetVal)
	{
		ATLASSERT(pRetVal != NULL);
		*pRetVal = m_data;
		return S_OK;
	}

	inline HRESULT STDMETHODCALLTYPE StoreData(PARAMTYPE NewData)
	{
		m_data = NewData;
		return S_OK;
	}

	inline HRESULT STDMETHODCALLTYPE CloneData(PARAMTYPE origval, PARAMTYPE *pRetVal)
	{
		ATLASSERT(pRetVal != NULL);
		*pRetVal = origval;
		return S_OK;
	}
};

template <class PARAMTYPE, class STORETYPE=PARAMTYPE>
class CPropertyHolderRO		// readonly properties
{
public:
	STORETYPE m_data;

	inline HRESULT STDMETHODCALLTYPE GetData(PARAMTYPE *pRetVal)
	{
		ATLASSERT(pRetVal != NULL);
		*pRetVal = m_data;
		return S_OK;
	}
};

// CPropertyHolder specializations

template <>
class CPropertyHolder<VARIANT_BOOL, bool>
{
public:
	bool m_data;

	HRESULT STDMETHODCALLTYPE StoreData(VARIANT_BOOL NewData)
	{
		m_data = NewData != ATL_VARIANT_FALSE ? true : false;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetData(VARIANT_BOOL *pRetVal)
	{
		ATLASSERT(pRetVal != NULL);
		*pRetVal = m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
		return S_OK;
	}	

	inline HRESULT STDMETHODCALLTYPE CloneData(VARIANT_BOOL origval, VARIANT_BOOL *pRetVal)
	{
		ATLASSERT(pRetVal != NULL);
		*pRetVal = origval;
		return S_OK;
	}
};

template <>
class CPropertyHolder<BSTR, CComBSTR>
{
public:
	CComBSTR m_data;

	HRESULT STDMETHODCALLTYPE StoreData(BSTR NewData)
	{
		m_data = NewData;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetData(BSTR *pRetVal)
	{
		ATLASSERT(pRetVal != NULL);
		return m_data.CopyTo(pRetVal);
	}	

	//REVIEW
	inline HRESULT STDMETHODCALLTYPE CloneData(BSTR origval, BSTR *pRetVal)
	{
		ATLASSERT(pRetVal != NULL);
		CComBSTR bstrOrig(origval);
		if (bstrOrig.m_str == NULL)
			E_OUTOFMEMORY;

		*pRetVal = bstrOrig.Detach();
		return S_OK;
	}
};

template <>
class CPropertyHolder<IFontDisp *, CComPtr<IFontDisp> > 
{
public:
	CComPtr<IFontDisp> m_data;

	HRESULT STDMETHODCALLTYPE CloneData(IFontDisp * origval, IFontDisp **ppRetVal)
	{
		ATLASSERT(ppRetVal != NULL);
		*ppRetVal = NULL;

		if (origval == NULL)
			return E_INVALIDARG;

		CComPtr<IFont> spOrigFont;
		HRESULT hr = origval->QueryInterface(&spOrigFont);
		if (SUCCEEDED(hr))
		{
			CComPtr<IFont> spClonedFont;
			hr = spOrigFont->Clone(&spClonedFont);
			if (SUCCEEDED(hr))
				hr = spClonedFont->QueryInterface(ppRetVal);
		}
		return hr;
	}

	HRESULT STDMETHODCALLTYPE GetData(IFontDisp **ppRetVal)
	{
		ATLASSERT(ppRetVal != NULL);
		return m_data.CopyTo(ppRetVal);
	}	

	inline HRESULT STDMETHODCALLTYPE StoreData(IFontDisp *NewData)
	{
		m_data = NewData;
		return S_OK;
	}
};

template <>
class CPropertyHolder<IPictureDisp *, CComPtr<IPictureDisp> >
{
public:
	CComPtr<IPictureDisp> m_data;

	HRESULT STDMETHODCALLTYPE CloneData(IPictureDisp * origval, IPictureDisp **ppRetVal)
	{
		ATLASSERT(ppRetVal != NULL);
		*ppRetVal = NULL;

		if (origval == NULL)
			return E_INVALIDARG;

		CComPtr<IPersistStream> spOrigPersistStream;
		HRESULT hr = origval->QueryInterface(&spOrigPersistStream);
		if (SUCCEEDED(hr))
		{
			CComPtr<IStream> spStream;
			hr = CreateStreamOnHGlobal(NULL, TRUE, &spStream);
			if (SUCCEEDED(hr))
			{
				hr = OleSaveToStream(spOrigPersistStream, spStream);
				if (SUCCEEDED(hr))
				{
					// reset the stream back to the beginning
					LARGE_INTEGER Li;
					Li.HighPart = 0;
					Li.LowPart = 0;
					hr = spStream->Seek(Li,STREAM_SEEK_SET,NULL);
					if (SUCCEEDED(hr))
						hr = OleLoadFromStream(spStream, _ATL_IIDOF(IPictureDisp), reinterpret_cast<void**>(ppRetVal));
				}
			}
		}
		return hr;
	}

	HRESULT STDMETHODCALLTYPE GetData(IPictureDisp **ppRetVal)
	{
		ATLASSERT(ppRetVal != NULL);
		*ppRetVal = NULL;

		return m_data.CopyTo(ppRetVal);
	}

	inline HRESULT STDMETHODCALLTYPE StoreData(IPictureDisp * NewData)
	{
		m_data = NewData;
		return S_OK;
	}
};

template <typename T = ContainerPair<> , class Derived, class DATATYPE, DISPID dispid, class STORETYPE=DATATYPE>
class CStockProp  :
	public OuterClassHelper<T>,
	public CPropertyHolder<DATATYPE, STORETYPE>
{
public:
	HRESULT STDMETHODCALLTYPE SetVal(DATATYPE NewData, bool bClone=false, bool bForce=false)
	{
		HRESULT hr;
		Derived* pDerived = static_cast<Derived*>(this);

		__if_exists(T::_ContainingClass::FireOnRequestEdit)
		{
			if (outer->FireOnRequestEdit(dispid) == S_FALSE)
				return S_FALSE;
		}
		
		if (bClone)
		{
			DATATYPE tempval;

			hr = pDerived->CloneData(NewData, &tempval);
			if (SUCCEEDED(hr))
				hr = pDerived->StoreData(tempval);
		}
		else
			hr = pDerived->StoreData(NewData);
		if (FAILED(hr))
			return hr;
		
		__if_exists(T::_ContainingClass::m_bRequiresSave)
		{
			outer->m_bRequiresSave = TRUE;
		}
		__if_exists(T::_ContainingClass::FireOnChanged)
		{
			outer->FireOnChanged(dispid);
		}
		__if_exists(T::_ContainingClass::FireViewChange)
		{
			outer->FireViewChange();
		}
		__if_exists(T::_ContainingClass::SendOnDataChange)
		{
			outer->SendOnDataChange(NULL);
		}

		return S_OK;
	}

    BOOL DesignMode()
	{
		__if_exists(T::_ContainingClass::GetAmbientProperty)
		{
			CComVariant var;

			HRESULT hr = outer->GetAmbientProperty(DISPID_AMBIENT_USERMODE, var);
			if (SUCCEEDED(hr) && var.vt == VT_BOOL && !var.boolVal)
				return TRUE;
		}
		
		return FALSE;
	}

	operator DATATYPE()
	{
		return m_data;
	}

	Derived& operator=(DATATYPE newVal)
	{
		SetVal(NewData);
		return *this;
	}

	// allows expresions like 'if(m_propEnabled) dosomething();'
	operator bool()
	{
		return m_data != 0;
	}

	//REVIEW
	// comparison operators. Specializations of more complex
	// types will need their own versions of these
	bool operator==(DATATYPE cmpVal)
	{
		return m_data == cmpVal;
	}

	bool operator!=(DATATYPE cmpVal)
	{
		return m_data != cmpVal;
	}

	bool operator<(DATATYPE cmpVal)
	{
		return m_data < cmpVal;
	}
	
	bool operator>(DATATYPE cmpVal)
	{
		return m_data > cmpVal;
	}

	void RecreateWindow()
	{
		__if_exists(T::_ContainingClass::InPlaceDeactivate)
		{
			outer->InPlaceDeactivate();
			outer->InPlaceActivate(OLEIVERB_INPLACEACTIVATE);
		}
	}
};

template <typename T = ContainerPair<> , class SP, class DATATYPE, DISPID dispid, class STORETYPE=DATATYPE>
class CStockPropRO :
	public CPropertyHolderRO<DATATYPE, STORETYPE>
{
public:
    BOOL DesignMode()
	{
		__if_exists(T::_ContainingClass::GetAmbientProperty)
		{
			CComVariant var;

			HRESULT hr = outer->GetAmbientProperty(DISPID_AMBIENT_USERMODE, var);
			if (SUCCEEDED(hr) && var.vt == VT_BOOL && !var.boolVal)
				return TRUE;
			
		}
		return FALSE;
	}
};

//   CStockProp specializations

template <typename T = ContainerPair<> >
class CStockFont :
	public CStockProp<T, CStockFont<T>, IFontDisp *, DISPID_FONT, CComPtr<IFontDisp> > 
{
public:
	CStockFont()
	{
//		m_data = NULL;
		m_bInitialized = false;
	}

	HRESULT StoreData(IFontDisp *NewData)
	{
		HRESULT hr = S_OK;

		// once StoreData is called, m_bInitialized is always true
		m_bInitialized = true;

		m_data = NewData;

		// We have a window
		if ((m_data) && (::IsWindow(outer->m_hWnd)))
		{
			HFONT hfont = NULL;
			CComPtr<IFont> spFont;
			hr = m_data->QueryInterface(&spFont);
			if (SUCCEEDED(hr))
				hr = spFont->get_hFont(&hfont);
			if ((SUCCEEDED(hr)) && (hfont))
				outer->SetFont(hfont);
		}
		
		return hr;
	}

	BEGIN_MSG_MAP(CStockFont<CStockFont<T> >)
		MESSAGE_HANDLER(WM_POSTCREATE, PostCreate)
	END_MSG_MAP()

   	LRESULT PostCreate(UINT /*uMsg*/, WPARAM /*wparam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (!m_bInitialized && !m_data)		// set default value
		{
			__if_exists(T::_ContainingClass::GetAmbientFontDisp)
			{
				if ((FAILED(outer->GetAmbientFontDisp(&m_data))) || (! m_data))
					GetDefaultFont(&m_data);
			}
			__if_not_exists(T::_ContainingClass::GetAmbientFontDisp)
			{
				GetDefaultFont(&m_data);
			}
		}

		StoreData(m_data);

		bHandled = FALSE;
		return 0;
	}

	HRESULT CreateFont (HFONT hFont, IFontDisp** ppFont)
	{
		ATLASSERT(ppFont != NULL);
		*ppFont = NULL;

		if (hFont == NULL)
			return E_INVALIDARG;

		USES_CONVERSION;

		LOGFONT logfont;
		GetObject(hFont, sizeof(logfont), &logfont);
		FONTDESC fd;
		fd.cbSizeofstruct = sizeof(FONTDESC);
		fd.lpstrName = T2OLE(logfont.lfFaceName);
		fd.sWeight = (short)logfont.lfWeight;
		fd.sCharset = logfont.lfCharSet;
		fd.fItalic = logfont.lfItalic;
		fd.fUnderline = logfont.lfUnderline;
		fd.fStrikethrough = logfont.lfStrikeOut;
		long lfHeight = logfont.lfHeight;
		if (lfHeight < 0)
			lfHeight = -lfHeight;
		int ppi;
		HDC hdc;
		if (::IsWindow(outer->m_hWnd))
		{
			hdc = ::GetDC(outer->m_hWnd);
			ppi = GetDeviceCaps(hdc, LOGPIXELSY);
			::ReleaseDC(outer->m_hWnd, hdc);
		}
		else
		{
			hdc = ::GetDC(GetDesktopWindow());
			ppi = GetDeviceCaps(hdc, LOGPIXELSY);
			::ReleaseDC(GetDesktopWindow(), hdc);
		}

		fd.cySize.Lo = lfHeight * 720000 / ppi;
		fd.cySize.Hi = 0;

		return OleCreateFontIndirect(&fd, IID_IFontDisp, (void**)ppFont);
	}

	HRESULT GetDefaultFont(IFontDisp** ppFont)
	{	
		USES_CONVERSION;
		HFONT hSystemFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
		if (hSystemFont == NULL)
			hSystemFont = (HFONT) GetStockObject(SYSTEM_FONT);
		return CreateFont(hSystemFont, ppFont);
	}

	operator HFONT()
	{
		CComQIPtr<IFont> pFont(m_data);
		HFONT hFont;
		HRESULT hr = pFont->get_hFont(&hFont);
		if(FAILED(hr))
		{
			hFont = NULL;
		}
		return hFont;
	}

	CStockFont<T>& operator=( IFontDisp* NewData )
	{
		SetVal(NewData);
		return *this;
	}

	CStockFont<T>& operator=(HFONT hFont)
	{
		HRESULT hr = CreateFont(hFont, &m_data);
		ATLASSERT(SUCCEEDED(hr));
		hr;
		return *this;
	}

	operator bool()
	{
		return m_data != 0;
	}

	bool m_bInitialized;
};

#define NOTYETSET L"__notyetset__"

template <DISPID dispid=DISPID_CAPTION, typename T = ContainerPair<> >
class CStockCaption :
	public CStockProp<T, CStockCaption<dispid, T>, BSTR, dispid, CComBSTR>
{
public:
	CStockCaption()
	{
		m_data = NOTYETSET;   		// sep. bool would not get persisted w/o more work
	}

	HRESULT GetData(BSTR *pRetVal)
	{
		ATLASSERT(pRetVal != NULL);
		*pRetVal = NULL;

		if (m_data == NOTYETSET)
			return S_OK;

		return m_data.CopyTo(pRetVal);
	}

	HRESULT StoreData(BSTR NewData)
	{
		m_data = NewData;

		if (::IsWindow(outer->m_hWnd))
			::SetWindowTextW(outer->m_hWnd, NewData);
		return S_OK;
	}

	inline bool NotYetSet()
	{
		return (m_data == NOTYETSET);
	}

	typedef CStockCaption<dispid, T> thisClass;

	BEGIN_MSG_MAP(thisClass)
		MESSAGE_HANDLER(WM_PRECREATE, PreCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

	LRESULT PreCreate(UINT /*uMsg*/, WPARAM wparam, LPARAM lparam, BOOL& bHandled)
	{
		USES_CONVERSION;
		LPCTSTR name;
		PreCreateStruct *pcs = (PreCreateStruct *)lparam;

		if (NotYetSet())
		{
			__if_exists(T::_ContainingClass::GetAmbientDisplayName)
			{
				CComBSTR bstr;
				if (SUCCEEDED(outer->GetAmbientDisplayName(bstr.m_str)))
					name = OLE2T(bstr);
				else
					name = _T("");
			}
			__if_not_exists(T::_ContainingClass::GetAmbientDisplayName)
			{
				name = T::_ContainingClass::GetWndCaption();
				if (name == NULL)
					name = _T("");
			}
		}
		else
			name = OLE2T(m_data);

		if (pcs->text == NULL)
		{
			ATLTRY(pcs->text = new TCHAR[lstrlen(name)+1]);		// caller will free
			if (pcs->text != NULL)
				lstrcpy(pcs->text, name);
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(::IsWindow(outer->m_hWnd))
			outer->GetWindowText(&m_data);
		bHandled = FALSE;
		return 0;
	}

	CStockCaption<dispid,T>& operator=(BSTR NewData)
	{
		SetVal(NewData);
		return *this;
	}
	CStockCaption<dispid,T>& operator=(LPCTSTR szVal)
	{
		SetVal(CComBSTR(szVal));
		return *this;
	}
	operator BSTR()
	{
		return m_data;
	}

};

template <typename T = ContainerPair<> >
class CStockText :
	public CStockCaption<DISPID_TEXT, T>
{
public:
	CStockText<T>& operator=(BSTR NewData)
	{
		SetVal(NewData);
		return *this;
	}
	CStockText<T>& operator=(LPCTSTR szVal)
	{
		SetVal(CComBSTR(szVal));
		return *this;
	}
};

template <typename T = ContainerPair<> >
class CStockEnabled : public CStockProp<T, CStockEnabled<T>, VARIANT_BOOL, DISPID_ENABLED, bool>
{
public:

	CStockEnabled()
	{
		m_data = true;
	}

	HRESULT GetData(VARIANT_BOOL *pRetVal)
	{
		if ((::IsWindow(outer->m_hWnd)) && (! DesignMode()))
		{
			m_data = outer->IsWindowEnabled() ? true : false;
		}

		*pRetVal = m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
		return S_OK;
	}

	HRESULT StoreData(VARIANT_BOOL NewData)
	{
		// Always succeeds
		CPropertyHolder<VARIANT_BOOL, bool>::StoreData(NewData);

		if ((::IsWindow(outer->m_hWnd)) && (! DesignMode()))
		{
			outer->EnableWindow(m_data ? TRUE : FALSE);
		}
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockEnabled<T>)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		StoreData(m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		bHandled = FALSE;
		return 0;
	}

	CStockEnabled<T>& operator=(VARIANT_BOOL NewData)
	{
		SetVal(NewData);
		return *this;
	}

	CStockEnabled<T>& operator=(BOOL NewData)
	{
		SetVal(NewData == TRUE ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	CStockEnabled<T>& operator=(bool NewData)
	{
		SetVal(NewData ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	operator VARIANT_BOOL()
	{
		return m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
	}

	operator BOOL()
	{
		return (m_data) ? TRUE : FALSE;
	}

	operator bool()
	{
		return m_data;
	}
};

template <typename T = ContainerPair<> >
class CStockAlignment : public CStockProp<T, CStockAlignment<T>, stkALIGN, DISPID_ALIGNMENT, stkALIGN>
{
public:
	CStockAlignment()
	{
		m_data = alLeft;
		m_bInitialized = false;
	}

	HRESULT GetData(stkALIGN *pRetVal)
	{
		*pRetVal = m_data;
		return S_OK;
	}

	HRESULT StoreData(stkALIGN NewData)
	{
		m_bInitialized = true;
		m_data = NewData;
		if (::IsWindow(outer->m_hWnd))
			RecreateWindow();
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockAlignment<T>)
		MESSAGE_HANDLER(WM_PRECREATE, PreCreate)
	END_MSG_MAP()

   	LRESULT PreCreate(UINT /*uMsg*/, WPARAM wparam, LPARAM lparam, BOOL& bHandled)
	{
		PreCreateStruct *pcs = (PreCreateStruct *)lparam;
		
		//REVIEW: uh?
		pcs->style &= ~(0x3);
		pcs->style |= m_data;

		bHandled = FALSE;
		return 0;
	}

	operator stkALIGN()
	{
		return m_data;
	}

	CStockAlignment<T>& operator=(stkALIGN NewData)
	{
		SetVal(NewData);
		return *this;
	}
	bool m_bInitialized;

};

//REVIEW: uh?
template <typename T = ContainerPair<> >
class CStockButtonAlignment : public CStockProp<T, CStockButtonAlignment<T>, stkBUTTALIGN, DISPID_BUTTONALIGNMENT, stkBUTTALIGN>
{
public:
	CStockButtonAlignment()
	{
		m_data = baLeft;
	}

	HRESULT GetData(stkBUTTALIGN *pRetVal)
	{
		if (::IsWindow(outer->m_hWnd))
			m_data = (outer->GetStyle() & BS_LEFTTEXT) ? baRight : baLeft;

		*pRetVal = m_data;
		return S_OK;
	}

	HRESULT StoreData(stkBUTTALIGN NewData)
	{
		m_data = NewData;
		if (::IsWindow(outer->m_hWnd))
			RecreateWindow();
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockButtonAlignment<T>)
		MESSAGE_HANDLER(WM_PRECREATE, PreCreate)
	END_MSG_MAP()

   	LRESULT PreCreate(UINT /*uMsg*/, WPARAM wparam, LPARAM lparam, BOOL& bHandled)
	{
		PreCreateStruct *pcs = (PreCreateStruct *)lparam;

		if (m_data == baLeft)
			pcs->style &= ~(BS_LEFTTEXT);
		else
			pcs->style |= BS_LEFTTEXT;

		bHandled = FALSE;
		return 0;
	}
//REVIEW: markkra Raid: 7641 and 5462
//TODO: assignment
};

template <typename T = ContainerPair<> >
class CStockAppearance : public CStockProp<T, CStockAppearance<T>, stkAPPEAR, DISPID_APPEARANCE, stkAPPEAR>
{
public:
	CStockAppearance()
	{
		m_data = apThreed;
		m_bInitialized = false;
	}

	HRESULT GetData(stkAPPEAR *pRetVal)
	{
		if (::IsWindow(outer->m_hWnd))
			m_data = (outer->GetExStyle() & WS_EX_CLIENTEDGE) ? apThreed : apFlat;

		*pRetVal = m_data;
		return S_OK;
	}

	HRESULT StoreData(stkAPPEAR NewData)
	{
		m_bInitialized = true;
		m_data = NewData;
		if (::IsWindow(outer->m_hWnd))
		{
			if (m_data == apFlat)
				outer->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
			else
				outer->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
		}
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockAppearance<T>)
		MESSAGE_HANDLER(WM_PRECREATE, PreCreate)
	END_MSG_MAP()

   	LRESULT PreCreate(UINT /*uMsg*/, WPARAM /*wparam*/, LPARAM lparam, BOOL& bHandled)
	{
		PreCreateStruct *pcs = (PreCreateStruct *)lparam;

		if (m_data == apFlat)
			pcs->exstyle &= ~(WS_EX_CLIENTEDGE);
		else
			pcs->exstyle |= WS_EX_CLIENTEDGE;

		bHandled = FALSE;
		return 0;
	}

//REVIEW: markkra Raid: 7641 and 5462
	CStockAppearance<T>& operator=(stkAPPEAR NewData)
	{
		SetVal(NewData);
		return *this;
	}
	bool m_bInitialized;

};

template <typename T = ContainerPair<> >
class CStockMultiSelect : public CStockProp<T, CStockMultiSelect<T>, stkMULTISELECT, DISPID_MULTISELECT, stkMULTISELECT>
{
public:
	CStockMultiSelect()
	{
		m_data = msNone;
	}

	HRESULT StoreData(stkMULTISELECT NewData)
	{
		m_data = NewData;

		if (::IsWindow(outer->m_hWnd))
			RecreateWindow();
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockMultiSelect<T>)
		MESSAGE_HANDLER(WM_PRECREATE, PreCreate)
	END_MSG_MAP()

   	LRESULT PreCreate(UINT /*uMsg*/, WPARAM /*wparam*/, LPARAM lparam, BOOL& bHandled)
	{
		PreCreateStruct *pcs = (PreCreateStruct *)lparam;

		pcs->style &= ~(LBS_MULTIPLESEL | LBS_EXTENDEDSEL);

		if (m_data == msSimple)
			pcs->style |= LBS_MULTIPLESEL;
		else if (m_data == msExtended)
			pcs->style |= LBS_EXTENDEDSEL;

		bHandled = FALSE;
		return 0;
	}
//REVIEW: markkra Raid: 7641 and 5462
//TODO: assignment operator
};

template <typename T = ContainerPair<> >
class CStockComboStyle : public CStockProp<T, CStockComboStyle<T>, stkCOMBOSTYLE, DISPID_COMBOSTYLE, stkCOMBOSTYLE>
{
public:
	CStockComboStyle()
	{
		m_data = csDropdownCombo;
	}

	HRESULT StoreData(stkCOMBOSTYLE NewData)
	{
		m_data = NewData;
		if (::IsWindow(outer->m_hWnd))
			RecreateWindow();
		return S_OK;
	}


	BEGIN_MSG_MAP(CStockComboStyle<T>)
		MESSAGE_HANDLER(WM_PRECREATE, PreCreate)
		MESSAGE_HANDLER(WM_POSTCREATE, PostCreate)
	END_MSG_MAP()

   	LRESULT PreCreate(UINT /*uMsg*/, WPARAM wparam, LPARAM lparam, BOOL& bHandled)
	{
		PreCreateStruct *pcs = (PreCreateStruct *)lparam;

		pcs->style &= ~(0x3);

		if (m_data == csDropdownCombo)
			pcs->style |= CBS_DROPDOWN;
		else if (m_data == csSimpleCombo)
			pcs->style |= CBS_SIMPLE;
		else
			pcs->style |= CBS_DROPDOWNLIST;

		bHandled = FALSE;
		return 0;
	}

   	LRESULT PostCreate(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
//REVIEW
#if 0
		// subclass child windows, also (for proper color, etc. behavior)
		HWND child = ::GetWindow((HWND)wparam, GW_CHILD);
		while (child)
		{
			CWndProcThunk m_thunk;
			m_thunk.Init(outer->WindowProc, this);
			WNDPROC pProc = (WNDPROC)&m_thunk.thunk;
			::SetWindowLongPtr(child, GWLP_WNDPROC, (LONG_PTR)pProc);
			child = ::GetNextWindow(child, GW_HWNDNEXT);
		}
#endif

		bHandled = FALSE;
		return 0;
	}
//REVIEW: markkra Raid: 7641 and 5462
//TODO: assignment operator

};

template <typename T = ContainerPair<> >
class CStockBorderStyle : public CStockProp<T, CStockBorderStyle<T>, stkBORDER, DISPID_BORDERSTYLE, stkBORDER>
{
public:
	CStockBorderStyle()
	{
		m_data = boSingle;		// most common case
	}

	HRESULT StoreData(stkBORDER NewData)
	{
		m_data = NewData;
		if (::IsWindow(outer->m_hWnd))
		{
			if (m_data == boSingle)
				outer->ModifyStyle(0, WS_BORDER, SWP_FRAMECHANGED);
			else
				outer->ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
		}
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockBorderStyle<T>)
		MESSAGE_HANDLER(WM_PRECREATE, PreCreate)
	END_MSG_MAP()

   	LRESULT PreCreate(UINT /*uMsg*/, WPARAM /*wparam*/, LPARAM lparam, BOOL& bHandled)
	{
		PreCreateStruct *pcs = (PreCreateStruct *)lparam;

		//REVIEW: get these style from VB
		//TODO: boNone, boSingle, boSizable, boFixedDialog, boFixedToolWindow, boSizableToolWindow
		if (m_data == boSingle)
			pcs->style |= (WS_BORDER);
		else
			pcs->style &= ~(WS_BORDER);

		bHandled = FALSE;
		return 0;
	}

	CStockBorderStyle<T>& operator=(stkBORDER NewData)
	{
		SetVal(NewData);
		return *this;
	}
};

template <typename T = ContainerPair<> >
class CStockBackColor : public CStockProp<T, CStockBackColor<T>, OLE_COLOR, DISPID_BACKCOLOR, OLE_COLOR>
{
public:
	CStockBackColor()
	{	
		m_data = -1;			// not yet set
		m_bInitialized = false;
	}
	
	~CStockBackColor()
	{	
		::DeleteObject(hbrush);
	}

	HRESULT StoreData(OLE_COLOR NewData)
	{
		m_bInitialized = true;

		m_data = NewData;

		::DeleteObject(hbrush);
		hbrush = CreateSolidBrush(GETCOLORVAL(m_data));

		if (::IsWindow(outer->m_hWnd))
		{
			outer->InvalidateRect(NULL, TRUE);
		}
		else 
		{
			__if_exists(T::_ContainingClass::m_spInPlaceSite)
			{
				if(outer->m_spInPlaceSite)
				{
					outer->m_spInPlaceSite->InvalidateRect(NULL, TRUE);
				}
			}
		}
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockBackColor<T>)
		MESSAGE_RANGE_HANDLER(OCM__BASE+WM_CTLCOLORMSGBOX,		// reflected from parent
			OCM__BASE+WM_CTLCOLORSTATIC, OnColorQuery)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX,				// send from our children (combo)
			WM_CTLCOLORSTATIC, OnColorQuery)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBknd)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (m_data == -1)		// not yet set
		{
			LPCTSTR pszClassName;

			pszClassName = outer->GetWndClassName();
			__if_exists(T::_ContainingClass::GetAmbientBackColor)
			{
				if (pszClassName)
					if ((_stricmp(pszClassName, "static")==0) || (_stricmp(pszClassName, "button")==0))
						outer->GetAmbientBackColor(m_data);
			}
	
			if (m_data == -1)			// ambient failed or its not a static control
				m_data = 0x80000000 | COLOR_WINDOW;

			StoreData(m_data);
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT OnEraseBknd(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		RECT rect;
		HDC hdc = (HDC)wparam;
	
		//REVIEW: what happens if the control is windowless?
		outer->GetClientRect(&rect);
		FillRect(hdc, &rect, hbrush);

		// mark this msg as handled
		return 1;			// erase msg processed
	}

    LRESULT OnColorQuery(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		HDC hdc = (HDC)wparam;

		SetBkColor(hdc, GETCOLORVAL(m_data));
		// mark as handled (omit "bHandled = FALSE;")
		return (LRESULT)hbrush;
	}

	CStockBackColor<T>& operator=(OLE_COLOR NewData)
	{
		SetVal(NewData);
		return *this;
	}

	HBRUSH hbrush;
	bool m_bInitialized;
};

template <typename T = ContainerPair<> >
class CStockForeColor : public CStockProp<T, CStockForeColor<T>, OLE_COLOR, DISPID_FORECOLOR, OLE_COLOR>
{
public:

	CStockForeColor()
	{	
		m_data = -1;		// not yet set 
		m_bInitialized = false;
	}
	
	HRESULT StoreData(OLE_COLOR NewData)
	{
		m_bInitialized = true;
		m_data = NewData;
		if (::IsWindow(outer->m_hWnd))
		{
			outer->InvalidateRect(NULL, TRUE);
		}
		else 
		{
			__if_exists(T::_ContainingClass::m_spInPlaceSite)
			{
				if(outer->m_spInPlaceSite)
				{
					outer->m_spInPlaceSite->InvalidateRect(NULL, TRUE);
				}
			}
		}
		return S_OK;
	}


	BEGIN_MSG_MAP(CStockForeColor<T>)
		MESSAGE_RANGE_HANDLER(OCM__BASE+WM_CTLCOLORMSGBOX,		// reflected from parent
			OCM__BASE+WM_CTLCOLORSTATIC, OnColorQuery)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX,				// send from our children (combo)
			WM_CTLCOLORSTATIC, OnColorQuery)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (m_data == -1)		// not yet set
		{
			__if_exists(T::_ContainingClass::GetAmbientForeColor)
			{
				outer->GetAmbientForeColor(m_data);
			}
	
			if (m_data == -1)			// ambient failed or its not a static control
				m_data = 0x80000000 | COLOR_WINDOWTEXT;

			StoreData(m_data);
		}

		bHandled = FALSE;
		return 0;
	}
    
	LRESULT OnColorQuery(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		HDC hdc = (HDC)wparam;

		::SetTextColor(hdc, GETCOLORVAL(m_data));
		__if_exists(T::_ContainingClass::m_propBackColor)
		{
			bHandled = FALSE;
			return 0;
		}

		//---- forecolor being used without backcolor ----
		bHandled = TRUE;
		return GetClassLong(outer->m_hWnd, GCL_HBRBACKGROUND);
	}

	CStockForeColor<T>& operator=(OLE_COLOR NewData)
	{
		SetVal(NewData);
		return *this;
	}

	bool m_bInitialized;
};

template <typename T = ContainerPair<> >
class CStockHideSelection : public CStockProp<T, CStockHideSelection<T>, VARIANT_BOOL, DISPID_HIDESELECTION, bool>
{
public:
	CStockHideSelection()
	{	
		m_data = true;
	}
	
	HRESULT GetData(VARIANT_BOOL *pRetVal)
	{
		if (::IsWindow(outer->m_hWnd))
			m_data = (outer->GetStyle() & ES_NOHIDESEL) ? false : true;

		*pRetVal = m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
		return S_OK;
	}

	HRESULT StoreData(VARIANT_BOOL NewData)
	{
		// Always succeeds
		CPropertyHolder<VARIANT_BOOL, bool>::StoreData(NewData);
		if (::IsWindow(outer->m_hWnd))
		{
			if (m_data)
				outer->ModifyStyle(ES_NOHIDESEL, 0, SWP_FRAMECHANGED);
			else
				outer->ModifyStyle(0, ES_NOHIDESEL, SWP_FRAMECHANGED);
		}
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockHideSelection<T>)
		MESSAGE_HANDLER(WM_PRECREATE, PreCreate)
	END_MSG_MAP()

   	LRESULT PreCreate(UINT /*uMsg*/, WPARAM /*wparam*/, LPARAM lparam, BOOL& bHandled)
	{
		PreCreateStruct *pcs = (PreCreateStruct *)lparam;

		if (m_data)
			pcs->style &= ~(ES_NOHIDESEL);
		else
			pcs->style |= ES_NOHIDESEL;

		bHandled = FALSE;
		return 0;
	}

	CStockHideSelection<T>& operator=(VARIANT_BOOL NewData)
	{
		SetVal(NewData != ATL_VARIANT_FALSE ? true : false);
		return *this;
	}

	CStockHideSelection<T>& operator=(BOOL NewData)
	{
		SetVal(NewData == TRUE ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	CStockHideSelection<T>& operator=(bool NewData)
	{
		SetVal(NewData ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	operator VARIANT_BOOL()
	{
		return m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
	}

	operator BOOL()
	{
		return (m_data) ? TRUE : FALSE;
	}

	operator bool()
	{
		return m_data;
	}
};

template <typename T = ContainerPair<> >
class CStockIntegralHeight : public CStockProp<T, CStockIntegralHeight<T>, VARIANT_BOOL, DISPID_INTEGRALHEIGHT, bool>
{
public:
	CStockIntegralHeight()
	{	
		m_data = true;
	}
	
	HRESULT StoreData(VARIANT_BOOL NewData)
	{
		// Always succeeds
		CPropertyHolder<VARIANT_BOOL, bool>::StoreData(NewData);

		m_data = (NewData != 0);
		if (::IsWindow(outer->m_hWnd))
			RecreateWindow();
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockIntegralHeight<T>)
		MESSAGE_HANDLER(WM_PRECREATE, PreCreate)
	END_MSG_MAP()

   	LRESULT PreCreate(UINT /*uMsg*/, WPARAM /*wparam*/, LPARAM lparam, BOOL& bHandled)
	{
		PreCreateStruct *pcs = (PreCreateStruct *)lparam;

		if (m_data)
			pcs->style &= ~(CBS_NOINTEGRALHEIGHT);
		else
			pcs->style |= CBS_NOINTEGRALHEIGHT;

		bHandled = FALSE;
		return 0;
	}

	CStockIntegralHeight<T>& operator=(VARIANT_BOOL NewData)
	{
		SetVal(NewData != ATL_VARIANT_FALSE ? true : false);
		return *this;
	}

	CStockIntegralHeight<T>& operator=(BOOL NewData)
	{
		SetVal(NewData == TRUE ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	CStockIntegralHeight<T>& operator=(bool NewData)
	{
		SetVal(NewData ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	operator VARIANT_BOOL()
	{
		return m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
	}

	operator BOOL()
	{
		return (m_data) ? TRUE : FALSE;
	}

	operator bool()
	{
		return m_data;
	}
};

template <typename T = ContainerPair<> >
class CStockSorted : public CStockProp<T, CStockSorted<T>, VARIANT_BOOL,	DISPID_SORTED, bool>
{
public:
	CStockSorted()
	{	
		m_data = ATL_VARIANT_FALSE;
	}
	
	HRESULT StoreData(VARIANT_BOOL NewData)
	{
		m_data = (NewData != 0);
		if (::IsWindow(outer->m_hWnd))
			RecreateWindow();
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockSorted<T>)
		MESSAGE_HANDLER(WM_PRECREATE, PreCreate)
	END_MSG_MAP()

   	LRESULT PreCreate(UINT /*uMsg*/, WPARAM /*wparam*/, LPARAM lparam, BOOL& bHandled)
	{
		PreCreateStruct *pcs = (PreCreateStruct *)lparam;

		if (m_data)
			pcs->style |= CBS_SORT;
		else
			pcs->style &= ~(CBS_SORT);

		bHandled = FALSE;
		return 0;
	}

	CStockSorted<T>& operator=(VARIANT_BOOL NewData)
	{
		SetVal(NewData != ATL_VARIANT_FALSE ? true : false);
		return *this;
	}

	CStockSorted<T>& operator=(BOOL NewData)
	{
		SetVal(NewData == TRUE ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	CStockSorted<T>& operator=(bool NewData)
	{
		SetVal(NewData ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	operator VARIANT_BOOL()
	{
		return m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
	}

	operator BOOL()
	{
		return (m_data) ? TRUE : FALSE;
	}

	operator bool()
	{
		return m_data;
	}
};

template <typename T = ContainerPair<> >
class CStockLocked : public CStockProp<T, CStockLocked<T>, VARIANT_BOOL,	DISPID_LOCKED, bool>
{
public:
	CStockLocked()
	{	
		m_data = ATL_VARIANT_FALSE;
	}
	
	HRESULT StoreData(VARIANT_BOOL NewData)
	{
		CPropertyHolder<VARIANT_BOOL, bool>::StoreData(NewData);
		if (::IsWindow(outer->m_hWnd))
		{
			if (m_data)
				outer->ModifyStyle(0, ES_READONLY, SWP_FRAMECHANGED);
			else
				outer->ModifyStyle(ES_READONLY, 0, SWP_FRAMECHANGED);
		}
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockLocked<T>)
		MESSAGE_HANDLER(WM_PRECREATE, PreCreate)
	END_MSG_MAP()

   	LRESULT PreCreate(UINT /*uMsg*/, WPARAM /*wparam*/, LPARAM lparam, BOOL& bHandled)
	{
		PreCreateStruct *pcs = (PreCreateStruct *)lparam;

		if (m_data)
			pcs->style |= ES_READONLY;
		else
			pcs->style &= ~(ES_READONLY);

		bHandled = FALSE;
		return 0;
	}

	CStockLocked<T>& operator=(VARIANT_BOOL NewData)
	{
		SetVal(NewData != ATL_VARIANT_FALSE ? true : false);
		return *this;
	}

	CStockLocked<T>& operator=(BOOL NewData)
	{
		SetVal(NewData == TRUE ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	CStockLocked<T>& operator=(bool NewData)
	{
		SetVal(NewData ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	operator VARIANT_BOOL()
	{
		return m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
	}

	operator BOOL()
	{
		return (m_data) ? TRUE : FALSE;
	}

	operator bool()
	{
		return m_data;
	}
};

template <typename T = ContainerPair<> >
class CStockMaxLength : public CStockProp<T, CStockMaxLength<T>, long, DISPID_MAXLENGTH, long>
{
public:
	CStockMaxLength()
	{	
		m_data = 0;
	}
	
	HRESULT StoreData(long NewData)
	{
		m_data = NewData;
		if (::IsWindow(outer->m_hWnd))
			outer->SendMessage(EM_LIMITTEXT, m_data, 0);
		
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockMaxLength<T>)
		MESSAGE_HANDLER(WM_POSTCREATE, OnCreateDone)
	END_MSG_MAP()

    LRESULT OnCreateDone(UINT /*uMsg*/, WPARAM /*wparam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		StoreData(m_data);

		bHandled = FALSE;
		return 0;
	}

	CStockMaxLength<T>& operator=(long NewData)
	{
		SetVal(NewData);
		return *this;
	}
};

template <typename T = ContainerPair<> >
class CStockMultiLine : public CStockProp<T, CStockMultiLine<T>, VARIANT_BOOL,	DISPID_MULTILINE, bool>
{
public:
	CStockMultiLine()
	{	
		m_data = ATL_VARIANT_FALSE;
	}
	
	HRESULT StoreData(VARIANT_BOOL NewData)
	{
		// Always succeeds
		CPropertyHolder<VARIANT_BOOL, bool>::StoreData(NewData);
		if (::IsWindow(outer->m_hWnd))
			RecreateWindow();
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockMultiLine<T>)
		MESSAGE_HANDLER(WM_PRECREATE, PreCreate)
	END_MSG_MAP()

   	LRESULT PreCreate(UINT /*uMsg*/, WPARAM /*wparam*/, LPARAM lparam, BOOL& bHandled)
	{
		PreCreateStruct *pcs = (PreCreateStruct *)lparam;

		if (m_data)
			pcs->style |= ES_MULTILINE;
		else
			pcs->style &= ~(ES_MULTILINE);

		bHandled = FALSE;
		return 0;
	}

	CStockMultiLine<T>& operator=(VARIANT_BOOL NewData)
	{
		SetVal(NewData != ATL_VARIANT_FALSE ? true : false);
		return *this;
	}

	CStockMultiLine<T>& operator=(BOOL NewData)
	{
		SetVal(NewData == TRUE ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	CStockMultiLine<T>& operator=(bool NewData)
	{
		SetVal(NewData ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	operator VARIANT_BOOL()
	{
		return m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
	}

	operator BOOL()
	{
		return (m_data) ? TRUE : FALSE;
	}

	operator bool()
	{
		return m_data;
	}
};

template <typename T = ContainerPair<> >
class CStockMousePointer : public CStockProp<T, CStockMousePointer<T>, stkMOUSEPTR,	DISPID_MOUSEPOINTER, stkMOUSEPTR>
{
public:
	CStockMousePointer()
	{	
		m_data = mpDefault;
		outer->mpCursor = NULL;
	}
	
	HRESULT StoreData(stkMOUSEPTR NewData)
	{
		if (m_data == mpCustom)
			DeleteObject(outer->mpCursor);

		m_data = NewData;
		if (m_data == mpDefault)
			outer->mpCursor = NULL;
		else if (m_data != mpCustom)
			outer->mpCursor = ::LoadCursor(NULL, StdCursors[m_data]);

		outer->mpChanged();
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockMousePointer<T>)
		MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
	END_MSG_MAP()

    LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM /*wparam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (outer->mpCursor)
		{
			::SetCursor(outer->mpCursor);
			bHandled = TRUE;
		}
		else
		{
			bHandled = FALSE;
		}
		return 0;
	}

	CStockMousePointer<T>& operator=(stkMOUSEPTR NewData)
	{
		SetVal(NewData);
		return *this;
	}
};

template <typename T = ContainerPair<> > 
class CStockMouseIcon : public CStockProp<T, CStockMouseIcon<T>, IPictureDisp *, DISPID_MOUSEICON, 	CComPtr<IPictureDisp> >
{
public:
	CStockMouseIcon()
	{
		hpicture = NULL;
	}

	HRESULT StoreData(IPictureDisp *NewData)
	{
		HRESULT hr = S_OK;

		hpicture = NULL;
		m_data = NewData;

		CComQIPtr<IPicture, &IID_IPicture> spPicture(NewData);
		if (spPicture)
			hr = spPicture->get_Handle(&hpicture);

		outer->mpChanged();
		
		return hr;
	}

	UINT hpicture;
};

template <typename T = ContainerPair<> >
class CStockPasswordChar : public CStockProp<T, CStockPasswordChar<T>, BSTR, DISPID_PASSWORDCHAR, CComBSTR>
{
public:
	HRESULT StoreData(BSTR NewData)
	{
		int charval;

		m_data = NewData;

		if (m_data.Length() == 0)
			charval = 0;
		else
			charval = m_data[0];
		
		if (::IsWindow(outer->m_hWnd))
			outer->SendMessage(EM_SETPASSWORDCHAR, charval, 0);
		
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockPasswordChar<T>)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		StoreData(m_data);
		bHandled = FALSE;
		return 0;
	}

	CStockPasswordChar<T>& operator=(BSTR NewData)
	{
		SetVal(NewData);
		return *this;
	}
	CStockPasswordChar<T>& operator=(LPCTSTR szVal)
	{
		CComBSTR t(szVal);
		SetVal(t);
		return *this;
	}
};

template <typename T = ContainerPair<> >
class CStockRightToLeft : public CStockProp<T, CStockRightToLeft<T>, VARIANT_BOOL,	DISPID_RIGHTTOLEFT, bool>
{
public:
	CStockRightToLeft()
	{
		m_data = ATL_VARIANT_FALSE;
	}
	
	HRESULT StoreData(VARIANT_BOOL NewData)
	{
		// Always succeeds
		CPropertyHolder<VARIANT_BOOL, bool>::StoreData(NewData);


		if (::IsWindow(outer->m_hWnd))
		{
			if (m_data)
				outer->ModifyStyleEx(0, WS_EX_RTLREADING, SWP_FRAMECHANGED);
			else
				outer->ModifyStyleEx(WS_EX_RTLREADING, 0, SWP_FRAMECHANGED);
		}
		
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockRightToLeft<T>)
		MESSAGE_HANDLER(WM_PRECREATE, PreCreate)
	END_MSG_MAP()

   	LRESULT PreCreate(UINT /*uMsg*/, WPARAM /*wparam*/, LPARAM lparam, BOOL& bHandled)
	{
		PreCreateStruct *pcs = (PreCreateStruct *)lparam;

		if (m_data)
			pcs->exstyle |= WS_EX_RTLREADING;
		else
			pcs->exstyle &= ~(WS_EX_RTLREADING);

		bHandled = FALSE;
		return 0;
	}

	CStockRightToLeft<T>& operator=(VARIANT_BOOL NewData)
	{
		SetVal(NewData != ATL_VARIANT_FALSE ? true : false);
		return *this;
	}

	CStockRightToLeft<T>& operator=(BOOL NewData)
	{
		SetVal(NewData == TRUE ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	CStockRightToLeft<T>& operator=(bool NewData)
	{
		SetVal(NewData ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	operator VARIANT_BOOL()
	{
		return m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
	}

	operator BOOL()
	{
		return (m_data) ? TRUE : FALSE;
	}

	operator bool()
	{
		return m_data;
	}
};

template <typename T = ContainerPair<> >
class CStockScrollBars : public CStockProp<T, CStockScrollBars<T>, stkSCROLLBARS, DISPID_SCROLLBARS, stkSCROLLBARS>
{
public:
	CStockScrollBars()
	{
		m_data = sbNone;
	}

	HRESULT StoreData(stkSCROLLBARS NewData)
	{
		m_data = NewData;
	
		if (::IsWindow(outer->m_hWnd))
			RecreateWindow();
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockScrollBars<T>)
		MESSAGE_HANDLER(WM_PRECREATE, PreCreate)
	END_MSG_MAP()

   	LRESULT PreCreate(UINT /*uMsg*/, WPARAM /*wparam*/, LPARAM lparam, BOOL& bHandled)
	{
		PreCreateStruct *pcs = (PreCreateStruct *)lparam;

		pcs->style &= ~(WS_VSCROLL | WS_HSCROLL);

		if ((m_data == sbBoth) || (m_data == sbVertical))
			pcs->style |= WS_VSCROLL;
		if ((m_data == sbBoth) || (m_data == sbHorizontal))
			pcs->style |= WS_HSCROLL;

		bHandled = FALSE;
		return 0;
	}

	CStockScrollBars<T>& operator=(stkSCROLLBARS NewData)
	{
		SetVal(NewData);
		return *this;
	}
};

template <typename T = ContainerPair<> >
class CStockHwnd :
	public OuterClassHelper<T>
{
public:
	inline HRESULT STDMETHODCALLTYPE GetData(int *pRetVal)
	{
		*pRetVal = (int) outer->m_hWnd;
		return S_OK;
	}

	operator HWND()
	{
		return outer->m_hWnd;
	}
};

template <typename T = ContainerPair<> >
class CStockThis :
	public OuterClassHelper<T>
{
public:
	inline HRESULT STDMETHODCALLTYPE GetData(IUnknown **ppRetVal)
	{
		__if_exists(T::_ContainingClass::QueryInterface)
		{
			return outer->QueryInterface(_ATL_IIDOF(IUnknown), (void **)ppRetVal);
		}
		*ppRetVal = NULL;
		return E_NOINTERFACE;
	}

	operator IUnknown*()
	{
		IUnknown* pUnk;
		__if_exists(T::_ContainingClass::QueryInterface)
		{
			HRESULT hr = outer->QueryInterface(_ATL_IIDOF(IUnknown), (void **)&pUnk);
			if(FAILED(hr))
			{
				pUnk = NULL;
			}
			return pUnk;
		}
		return pUnk = NULL;
	}

};

template <typename T = ContainerPair<> >
class CStockListIndex :
	public OuterClassHelper<T>
{
public:
	inline HRESULT STDMETHODCALLTYPE GetData(int *RetVal)
	{
		if (::IsWindow(outer->m_hWnd))
			*RetVal = outer->SendMessage(LB_GETCURSEL, 0, 0);
		else
			*RetVal = -1;

		return S_OK;
	}
};

template <typename T = ContainerPair<> >
class CStockListCount :
	public OuterClassHelper<T>
{
public:
	inline HRESULT STDMETHODCALLTYPE GetData(int *pRetVal)
	{
		if (::IsWindow(outer->m_hWnd))
		{
			*pRetVal = outer->SendMessage(LB_GETCOUNT, 0, 0);
			if (*pRetVal < 0)
				*pRetVal = 0;
		}
		else
			*pRetVal = 0;

		return S_OK;
	}
};

template <typename T = ContainerPair<> >
class CStockTabStop : public CStockProp<T, CStockTabStop<T>, VARIANT_BOOL, DISPID_TABSTOP, bool>
{
public:
	CStockTabStop()
	{	
		m_data = true;
	}
	
	HRESULT GetData(VARIANT_BOOL *pRetVal)
	{
		if (::IsWindow(outer->m_hWnd))
		{
			m_data = ( (((outer->GetStyle()) & WS_TABSTOP) != 0) ? true : false);
		}

		*pRetVal = m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
		return S_OK;
	}

	HRESULT StoreData(VARIANT_BOOL NewData)
	{
		// Always succeeds
		CPropertyHolder<VARIANT_BOOL, bool>::StoreData(NewData);

		if (::IsWindow(outer->m_hWnd))
		{
			if (m_data)
				outer->ModifyStyle(0, WS_TABSTOP, 0);
			else
				outer->ModifyStyle(WS_TABSTOP, 0, 0);
		}
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockTabStop<T>)
		MESSAGE_HANDLER(WM_PRECREATE, PreCreate)
	END_MSG_MAP()

   	LRESULT PreCreate(UINT /*uMsg*/, WPARAM /*wparam*/, LPARAM lparam, BOOL& bHandled)
	{
		PreCreateStruct *pcs = (PreCreateStruct *)lparam;

		if (m_data)
			pcs->style |= WS_TABSTOP;
		else
			pcs->style &= ~WS_TABSTOP;

		bHandled = FALSE;
		return 0;
	}

	CStockTabStop<T>& operator=(bool NewData)
	{
		SetVal(NewData ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	CStockTabStop<T>& operator=(BOOL NewData)
	{
		SetVal(NewData == TRUE ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	operator VARIANT_BOOL()
	{
		return m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
	}

	operator BOOL()
	{
		return (m_data) ? TRUE : FALSE;
	}

	operator bool()
	{
		return m_data;
	}
};

template <typename T = ContainerPair<> >
class CStockBorderVisible : public CStockProp<T, CStockBorderVisible<T>, VARIANT_BOOL, DISPID_BORDERVISIBLE, bool>
{
public:
	DWORD m_dwStyle;

	CStockBorderVisible()
	{	
		m_data = true;
	}
	
	HRESULT StoreData(VARIANT_BOOL NewData)
	{
		// Always succeeds
		CPropertyHolder<VARIANT_BOOL, bool>::StoreData(NewData);

		if (::IsWindow(outer->m_hWnd))
		{
			if (m_data)
				outer->ModifyStyle(0, WS_BORDER, SWP_FRAMECHANGED);
			else
				outer->ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
		}
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockBorderVisible<T>)
		MESSAGE_HANDLER(WM_PRECREATE, PreCreate)
	END_MSG_MAP()

   	LRESULT PreCreate(UINT /*uMsg*/, WPARAM /*wparam*/, LPARAM lparam, BOOL& bHandled)
	{
		PreCreateStruct *pcs = (PreCreateStruct *)lparam;

		if (m_data)
			pcs->style |= WS_BORDER;
		else
			pcs->style &= ~WS_BORDER;

		bHandled = FALSE;
		return 0;
	}

	CStockBorderVisible<T>& operator=(VARIANT_BOOL NewData)
	{
		SetVal(NewData != ATL_VARIANT_FALSE ? true : false);
		return *this;
	}

	CStockBorderVisible<T>& operator=(BOOL NewData)
	{
		SetVal(NewData == TRUE ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	CStockBorderVisible<T>& operator=(bool NewData)
	{
		SetVal(NewData ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	operator VARIANT_BOOL()
	{
		return m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
	}

	operator BOOL()
	{
		return (m_data) ? TRUE : FALSE;
	}

	operator bool()
	{
		return m_data;
	}
};

template <typename T = ContainerPair<> >
class CStockSelText : public CStockProp<T, CStockSelText<T>, BSTR, DISPID_SELTEXT, CComBSTR>
{
public:
	HRESULT GetData(BSTR *pRetVal)
	{
		int start, end, totlen, sellen;
		OLECHAR *wbuff = NULL, *pw;

		*pRetVal = NULL;		// default case

		if (::IsWindow(outer->m_hWnd))
		{
			outer->SendMessage(EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
			sellen = end - start;
			if (sellen)
			{
				// EM_GETSELTEXT only works for richedit controls
				// so we use WM_GETTEXT and crop off unselected text
				totlen = outer->GetWindowTextLength();
				ATLTRY(wbuff = new OLECHAR[totlen+1]);		// terminating null
				if (wbuff == NULL)
					return E_OUTOFMEMORY;

				::SendMessageW(outer->m_hWnd, WM_GETTEXT, totlen, (LPARAM)wbuff);
				pw = &wbuff[start];
				pw[sellen] = 0;
				*pRetVal = ::SysAllocString(pw);
				delete [] wbuff;
			}
		}

		return S_OK;
	}

	HRESULT StoreData(BSTR NewData)
	{
		CPropertyHolder<BSTR, CComBSTR>::StoreData(NewData);
		if (::IsWindow(outer->m_hWnd))
			::SendMessageW(outer->m_hWnd, EM_REPLACESEL, TRUE, (LPARAM)NewData);
		return S_OK;
	}

	CStockSelText<T>& operator=(BSTR NewData)
	{
		SetVal(NewData);
		return *this;
	}
	CStockSelText<T>& operator=(LPCTSTR szVal)
	{
		CComBSTR t(szVal);
		SetVal(t);
		return *this;
	}
};

template <typename T = ContainerPair<> >
class CStockSelStart : public CStockProp<T, CStockSelStart<T>, int, DISPID_SELSTART, int>
{
public:
	CStockSelStart()
	{
		m_data = 0;
	}
	
	HRESULT GetData(int *pRetVal)
	{
		if (::IsWindow(outer->m_hWnd))
			outer->SendMessage(EM_GETSEL, (WPARAM)&m_data, NULL);

		*pRetVal = m_data;
		return S_OK;
	}

	HRESULT StoreData(int NewData)
	{
		m_data = NewData;
		if (::IsWindow(outer->m_hWnd))
			outer->SendMessage(EM_SETSEL, m_data, m_data);		// resets selection
		
		return S_OK;
	}

	CStockSelStart<T>& operator=( int NewData )
	{
		SetVal(NewData);
		return *this;
	}

};

template <typename T = ContainerPair<> >
class CStockSelLength : public CStockProp<T, CStockSelLength<T>, int, DISPID_SELLENGTH, int>
{
public:
	CStockSelLength()
	{
		m_data = 2;
	}
	
	HRESULT GetData(int *pRetVal)
	{
		int start, end;

		if (::IsWindow(outer->m_hWnd))
		{
			outer->SendMessage(EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
			m_data = end - start;
		}

		*pRetVal = m_data;
		return S_OK;
	}

	HRESULT StoreData(int NewData)
	{
		int start;

		m_data = NewData;
		if (::IsWindow(outer->m_hWnd))
		{
			outer->SendMessage(EM_GETSEL, (WPARAM)&start, NULL);
			outer->SendMessage(EM_SETSEL, start, start+m_data);
		}
		
		return S_OK;
	}

	CStockSelLength<T>& operator=( int NewData )
	{
		SetVal(NewData);
		return *this;
	}
};

template <typename T = ContainerPair<> >
class CStockAutoSize : public CStockProp<T, CStockAutoSize<T>, VARIANT_BOOL, DISPID_AUTOSIZE, bool>
{
public:

	CStockAutoSize()
	{
		m_data = ATL_VARIANT_FALSE;
	}

	HRESULT StoreData(VARIANT_BOOL NewData, void *createparam=NULL)
	{
		// Always succeeds
		CPropertyHolder<VARIANT_BOOL, bool>::StoreData(NewData);


		if ((m_data) && (::IsWindow(outer->m_hWnd)))
		{
			int cx, cy;
			if (ComputeNewClientArea(cx, cy, createparam))
			{
				// compute window frame/titlebar sizes
				RECT crect, wrect;
				int xdelta, ydelta;

				outer->GetWindowRect(&wrect);
				outer->GetParent().ScreenToClient(&wrect);
				outer->GetClientRect(&crect);

				xdelta = (wrect.right - wrect.left) - crect.right;
				ydelta = (wrect.bottom - wrect.top) - crect.bottom;

				Resize(cx + xdelta, cy + ydelta);
			}
		}
		return S_OK;
	}

	bool ComputeNewClientArea(int &cx, int &cy, void *createparam=NULL)
	{
		int dtFlags = 0;		// used in DrawText() call to calc client rect
		int style = outer->GetStyle();
		LPCTSTR pszClassName = outer->GetWndClassName();

		if ((pszClassName) && (_stricmp(pszClassName, "edit")==0))		// its an edit control
		{
			dtFlags |= DT_EDITCONTROL;
			if (! (style & ES_MULTILINE))
				dtFlags |= DT_SINGLELINE | DT_WORDBREAK;
			if (style & ES_RIGHT)
				dtFlags |= DT_RIGHT;
			else if (style & ES_LEFT)
				dtFlags |= DT_CENTER;
			else
				dtFlags |= DT_LEFT;
		}
		else		// assume its a static control
		{
			if ((style & SS_TYPEMASK) == SS_BITMAP)			// its a picture
			{
				HANDLE handle;

				if (style & SS_REALSIZEIMAGE)		// a flag from "Picture" prop to prevent resize
					return false;

				if (createparam)
					handle = (HANDLE) createparam;
				else
					handle = (HANDLE) outer->SendMessage(STM_GETIMAGE, IMAGE_BITMAP, 0);
				if (! handle)
					return false;

				BITMAP bm;
				::GetObject(handle, sizeof(bm), &bm);
				cx = bm.bmWidth;
				cy = bm.bmHeight;
				return true;
			}
			if ((style & SS_TYPEMASK) == SS_LEFTNOWORDWRAP)
				dtFlags |= DT_SINGLELINE | DT_LEFT;
			else
			{
				dtFlags |= DT_WORDBREAK;
				 if (style & SS_RIGHT)
					dtFlags |= DT_RIGHT;
				else if (style & ES_LEFT)
					dtFlags |= DT_CENTER;
				else
					dtFlags |= DT_LEFT;
			}
		}

		// common flag code
		dtFlags |= DT_CALCRECT;
		if (outer->GetExStyle() & WS_EX_RTLREADING)
			dtFlags |= DT_RTLREADING;
	
		TCHAR szBuf[4906];		// longer text will be allocated on demand
		TCHAR* pszBuf, *pszDynBuf = NULL;
		int len, blen;

		if (createparam)
		{
			pszBuf = (TCHAR*)createparam;
			blen = 0;
		}
		else
		{
			len = outer->GetWindowTextLength();
			if (len < sizeof(szBuf))
			{
				pszBuf = szBuf;
				blen = sizeof(szBuf);
			}
			else
			{
				ATLTRY(pszDynBuf = new TCHAR[len+1]);
				if (pszDynBuf == NULL)
					return false;
				pszBuf = pszDynBuf;
				blen = len+1;
			}
			outer->GetWindowText(pszBuf, blen);
		}

		if (! *pszBuf)
			pszBuf = " ";			// ensure control is still visible

		RECT crect;
		outer->GetClientRect(&crect);

		// "crect" is set here by DrawText()
		HDC hdc = outer->GetWindowDC();
		HANDLE oldfont = ::SelectObject(hdc, outer->GetFont());

		DrawText(hdc, pszBuf, -1, &crect, dtFlags);

		::SelectObject(hdc, oldfont);
		outer->ReleaseDC(hdc);
		delete [] pszDynBuf;

		cx = crect.right - crect.left;
		cy = crect.bottom - crect.top;
		return true;
	}

	void Resize(int cx, int cy)
	{
		outer->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE);

		__if_exists(T::_ContainingClass::m_sizeExtent)
		{
			// update our size info
			outer->m_sizeExtent.cx = cx;
			outer->m_sizeExtent.cy = cy;
		}

		// tell container that we changed
		RECT wrect;
		outer->GetWindowRect(&wrect);
		outer->GetParent().ScreenToClient(&wrect);
		wrect.right = wrect.left + cx;
		wrect.bottom = wrect.top + cy;

		__if_exists(T::_ContainingClass::m_spClientSite)
		{
			CComQIPtr<IOleInPlaceSite> spSite(outer->m_spClientSite);
			if (spSite != NULL)
			{
				spSite->OnPosRectChange(&wrect);
				spSite.Release();
			}
		}
	}

	BEGIN_MSG_MAP(CStockAutoSize<T>)
		MESSAGE_HANDLER(WM_SETTEXT, OnTextOrImage)
		MESSAGE_HANDLER(STM_SETIMAGE, OnTextOrImage)
	END_MSG_MAP()

    LRESULT OnTextOrImage(UINT msg, WPARAM /*wparam*/, LPARAM lparam, BOOL& bHandled)
	{
		if (m_data)
			StoreData(m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE,(void *)lparam);

		bHandled = FALSE;
		return 0;
	}

	CStockAutoSize<T>& operator=(VARIANT_BOOL NewData)
	{
		SetVal(NewData != ATL_VARIANT_FALSE ? true : false);
		return *this;
	}

	CStockAutoSize<T>& operator=(BOOL NewData)
	{
		SetVal(NewData == TRUE ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	CStockAutoSize<T>& operator=(bool NewData)
	{
		SetVal(NewData ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	operator VARIANT_BOOL()
	{
		return m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
	}

	operator BOOL()
	{
		return (m_data) ? TRUE : FALSE;
	}

	operator bool()
	{
		return m_data;
	}
};

template <typename T = ContainerPair<> > 
class CStockPicture : public CStockProp<T, CStockPicture<T>, IPictureDisp *, DISPID_PICTURE, 
	CComPtr<IPictureDisp> >
{
public:
	CStockPicture()
	{
	}

	HRESULT StoreData(IPictureDisp *NewData)
	{
		UINT hpicture = NULL;
		HRESULT hr = S_OK;

		m_data = NewData;

		CComQIPtr<IPicture, &IID_IPicture> spPicture(NewData);
		if ((spPicture) && (::IsWindow(outer->m_hWnd)))
		{
			hr = spPicture->get_Handle(&hpicture);
			if (SUCCEEDED(hr))
			{
				// Normally, when you set a static control's bitmap, it resizes to fit its picture
				// (after the resize, you can then resize it to any size).
				// "SS_REALSIZEIMAGE" is supposed to prevent this auto-resize but doesn't.
				// Only "SS_CENTERIMAGE" seems to prevent this

				// So, we use "SS_CENTERIMAGE" only during creation to prevent control's self resize
				// and we use "SS_REALSIZEIMAGE" as a flag to our "AutoSize" property
				// to prevent its action during our WM_CREATE-time work
				int newstyle = SS_CENTERIMAGE | SS_BITMAP;
				newstyle |= SS_REALSIZEIMAGE;
				outer->ModifyStyle(SS_TYPEMASK, newstyle, 0);
				outer->SendMessage(STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hpicture);
				outer->ModifyStyle(SS_CENTERIMAGE | SS_REALSIZEIMAGE, 0, 0);		// turn off trick bits
				outer->InvalidateRect(NULL, TRUE);
			}
		}
		
		return hr;
	}

	BEGIN_MSG_MAP(CStockPicture<T>)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		StoreData(m_data);
		bHandled = FALSE;
		return 0;
	}

	CStockPicture<T>& operator=( LPCTSTR bmpName )
	{
		CreateBitmap(bmpName);
		return *this;
	}

	HRESULT CreateBitmap( LPCTSTR bmpName )
	{
		HRESULT hr = S_OK;
		HBITMAP hBitmap = (HBITMAP)::LoadImage(_pModule->GetModuleInstance(),bmpName,IMAGE_BITMAP,0,0, LR_CREATEDIBSECTION | LR_LOADFROMFILE );
		if( hBitmap )
		{
			CComQIPtr<IPictureDisp> pNewPict;
			static PICTDESC _pictDesc =
			{
				sizeof(PICTDESC),
				PICTYPE_BITMAP,
				hBitmap,
				NULL
			};
			hr = OleCreatePictureIndirect(&_pictDesc,IID_IPictureDisp,1,(void**)&pNewPict);
			SetVal(pNewPict);
		}
		else
		{
			hr = E_FAIL;
		}

		return hr;
	}
};
template <typename T = ContainerPair<> >
class CStockWordWrap : public CStockProp<T, CStockWordWrap<T>, VARIANT_BOOL, DISPID_WORDWRAP, bool>
{
public:

	CStockWordWrap()
	{
		m_data = true;
	}

	HRESULT StoreData(VARIANT_BOOL NewData)
	{
		// Always succeeds
		CPropertyHolder<VARIANT_BOOL, bool>::StoreData(NewData);


		if (::IsWindow(outer->m_hWnd)) 
		{
			if (m_data)
			{
				// only clear SS_LEFTNOWORDWRAP if its a "text" static
				if ((outer->GetStyle() & SS_TYPEMASK) == SS_LEFTNOWORDWRAP)
					outer->ModifyStyle(SS_LEFTNOWORDWRAP, SS_LEFT, 0);
			}
			else
				outer->ModifyStyle(SS_TYPEMASK, SS_LEFTNOWORDWRAP, 0);
		}
		
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockWordWrap<T>)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		StoreData(m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		bHandled = FALSE;
		return 0;
	}

	CStockWordWrap<T>& operator=(VARIANT_BOOL NewData)
	{
		SetVal(NewData != ATL_VARIANT_FALSE ? true : false);
		return *this;
	}

	CStockWordWrap<T>& operator=(BOOL NewData)
	{
		SetVal(NewData == TRUE ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	CStockWordWrap<T>& operator=(bool NewData)
	{
		SetVal(NewData ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE);
		return *this;
	}

	operator VARIANT_BOOL()
	{
		return m_data ? ATL_VARIANT_TRUE : ATL_VARIANT_FALSE;
	}

	operator BOOL()
	{
		return (m_data) ? TRUE : FALSE;
	}

	operator bool()
	{
		return m_data;
	}
};

template <typename T = ContainerPair<> >
class CStockSelected : public CStockProp<T, CStockSelected<T>, SAFEARRAY **, DISPID_SELECTED, 
	CComSafeArray<VARIANT_BOOL> >
{
public:
	
	HRESULT GetData(SAFEARRAY **ppRetVal)
	{
		if (::IsWindow(outer->m_hWnd))			// update m_data from listbox
		{
			m_data.Destroy();
			int cnt = outer->SendMessage(LB_GETCOUNT, 0, 0);
			VARIANT_BOOL sel;

			for (int i=0; i < cnt; i++)
			{
				sel = (outer->SendMessage(LB_GETSEL, i, 0) != 0);
				m_data.Add(sel);
			}
		}

		*ppRetVal = m_data.m_psa;

		return S_OK;
	}

	HRESULT StoreData(SAFEARRAY **psa)
	{
		m_data.Attach(*psa);

		if (::IsWindow(outer->m_hWnd))			// update listbox from m_data
		{
			int cnt = outer->SendMessage(LB_GETCOUNT, 0, 0);
			VARIANT_BOOL sel;

			for (int i=0; i < cnt; i++)
			{
				m_data.GetAt(i, &sel);
				outer->SendMessage(LB_SETSEL, sel, i);
			}
		}
		
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockSelected<T>)
		MESSAGE_HANDLER(WM_POSTCREATE, OnCreate)
	END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		StoreData(&m_data.m_psa);
		bHandled = FALSE;
		return 0;
	}

};

template <typename T = ContainerPair<> >
class CStockList : 
	public CStockProp<T, CStockList<T>, SAFEARRAY **, DISPID_LIST, CComSafeArray<BSTR> >
{
public:
	
	HRESULT GetData(SAFEARRAY **ppRetVal)
	{
		if (::IsWindow(outer->m_hWnd))			// update m_data from listbox
		{
			int len, i, cnt;
			TCHAR* buff = NULL;
			USES_CONVERSION;
	
			m_data.Destroy();
			cnt = outer->SendMessage(LB_GETCOUNT, 0, 0);

			for (i=0; i < cnt; i++)
			{
				len = outer->SendMessage(LB_GETTEXTLEN, i, 0);
				ATLTRY(buff = new TCHAR[len+1]);
				if (buff != NULL)
				{
					outer->SendMessage(LB_GETTEXT, i, (LPARAM)buff);
					m_data.Add(::SysAllocString(A2OLE(buff)));
					delete [] buff;
				}
				else
					return E_OUTOFMEMORY;
			}
		}

		*ppRetVal = m_data.m_psa;

		return S_OK;
	}

	HRESULT StoreData(SAFEARRAY **psa)
	{
		m_data.Attach(*psa);

		if (::IsWindow(outer->m_hWnd))			// update listbox from m_data
		{
			int i, cnt;
			USES_CONVERSION;

			cnt = outer->SendMessage(LB_GETCOUNT, 0, 0);

			for (i=0; i < cnt; i++)
			{
				CComBSTR bstr;
				m_data.GetAt(i, &bstr.m_str);
				outer->SendMessage(LB_DELETESTRING, i, 0);
				outer->SendMessage(LB_INSERTSTRING, i, (LPARAM)OLE2T(bstr));
			}
		}
		
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockList<T>)
		MESSAGE_HANDLER(WM_POSTCREATE, OnCreate)
	END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		StoreData(&m_data.m_psa);
		bHandled = FALSE;
		return 0;
	}
};

template <typename T = ContainerPair<> >
class CStockBackStyle : public CStockProp<T, CStockBackStyle<T>, long, DISPID_BACKSTYLE, long>
{
public:
	CStockBackStyle()
	{	
		m_data = 0;
	}
	
	HRESULT StoreData(long NewData)
	{
		m_data = NewData;
		
		return S_OK;
	}

	BEGIN_MSG_MAP(CStockBackStyle<T>)
		MESSAGE_HANDLER(WM_POSTCREATE, OnCreateDone)
	END_MSG_MAP()

    LRESULT OnCreateDone(UINT /*uMsg*/, WPARAM /*wparam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		StoreData(m_data, true);

		bHandled = FALSE;
		return 0;
	}

	CStockBackStyle<T>& operator=(long NewData)
	{
		SetVal(NewData);
		return *this;
	}
};

/////////////////////////////////////////////////////////////////////////////
//    stock event classes

template <typename T = ContainerPair<> >
class CStockKeyPress :
	public OuterClassHelper<T>
{
public:

	BEGIN_MSG_MAP(CStockKeyPress<T>)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
	END_MSG_MAP()

    LRESULT OnChar(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		outer->Fire_KeyPress(wparam);

		bHandled = FALSE;
		return 0;
	}
};

template <typename T = ContainerPair<> >
class CStockMouseMove :
	public OuterClassHelper<T>
{
public:

	BEGIN_MSG_MAP(CStockMouseMove<T>)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
	END_MSG_MAP()

	DEFINE_GETSHIFTVAL();

    LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wparam, LPARAM lparam, BOOL& bHandled)
	{
		int buttonmask = 0;
		
		if (wparam & MK_LBUTTON)
			buttonmask |= 1;
		if (wparam & MK_RBUTTON)
			buttonmask |= 2;
		if (wparam & MK_MBUTTON)
			buttonmask |= 4;

		outer->Fire_MouseMove(buttonmask, GetShiftVal(), LOWORD(lparam), HIWORD(lparam));

		bHandled = FALSE;
		return 0;
	}
};

template <typename T = ContainerPair<> >
class CStockMouseDown :
	public OuterClassHelper<T>
{
public:

	BEGIN_MSG_MAP(CStockMouseDown<T>)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnMouseDown)
		MESSAGE_HANDLER(WM_MBUTTONDOWN, OnMouseDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnMouseDown)
	END_MSG_MAP()

	DEFINE_GETSHIFTVAL();

    LRESULT OnMouseDown(UINT umsg, WPARAM /*wparam*/, LPARAM lparam, BOOL &bHandled)
	{
		int buttonid;

		if (umsg == WM_LBUTTONDOWN)
			buttonid = 1;
		else if (umsg == WM_RBUTTONDOWN)
			buttonid = 2;
		else
			buttonid = 4;

		outer->Fire_MouseDown(buttonid, GetShiftVal(), LOWORD(lparam), HIWORD(lparam));

		bHandled = FALSE;
		return 0;
	}
};

template <typename T = ContainerPair<> >
class CStockDblClick :
	public OuterClassHelper<T>
{
public:

	BEGIN_MSG_MAP(CStockDblClick<T>)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnDblClk)
		MESSAGE_HANDLER(WM_MBUTTONDBLCLK, OnDblClk)
		MESSAGE_HANDLER(WM_RBUTTONDBLCLK, OnDblClk)
	END_MSG_MAP()

    LRESULT OnDblClk(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		outer->Fire_DblClick();

		bHandled = FALSE;
		return 0;
	}
};

template <typename T = ContainerPair<> >
class CStockMouseUp :
	public OuterClassHelper<T>
{
public:

	BEGIN_MSG_MAP(CStockMouseUp<T>)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnMouseUp)
		MESSAGE_HANDLER(WM_MBUTTONUP, OnMouseUp)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnMouseUp)
	END_MSG_MAP()

	DEFINE_GETSHIFTVAL();

    LRESULT OnMouseUp(UINT umsg, WPARAM /*wparam*/, LPARAM lparam, BOOL& bHandled)
	{
		int buttonid;

		if (umsg == WM_LBUTTONUP)
			buttonid = 1;
		else if (umsg == WM_RBUTTONUP)
			buttonid = 2;
		else
			buttonid = 4;

		outer->Fire_MouseUp(buttonid, GetShiftVal(), LOWORD(lparam), HIWORD(lparam));

		bHandled = FALSE;
		return 0;
	}
};

template <typename T = ContainerPair<> >
class CStockClick :
	public OuterClassHelper<T>
{
public:

	// these entries must be at end of msg map chain
	BEGIN_MSG_MAP(CStockClick<T>)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnMouseUp)
		MESSAGE_HANDLER(WM_MBUTTONUP, OnMouseUp)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnMouseUp)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnMouseDown)
		MESSAGE_HANDLER(WM_MBUTTONDOWN, OnMouseDown)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnMouseDown)
	END_MSG_MAP()

	bool bGotCapture;

    LRESULT OnMouseDown(UINT umsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled)
	{
		// two cases: windowless or not
		if( outer->m_bWndLess )
		{
			// we are a windowless control
			if( outer->m_spInPlaceSite != NULL )
			{
				LRESULT dontcare;
				outer->m_spInPlaceSite->OnDefWindowMessage(umsg, wparam, lparam, &dontcare);

				if( outer->m_spInPlaceSite->GetCapture() == S_FALSE )
				{
					if( outer->m_spInPlaceSite->SetCapture(TRUE) == S_OK )
					{
						bGotCapture = true;
					}
				}
				else
				{
					bGotCapture = false;
				}
			}

		}
		else
		{
			// we have a window
			// let control set capture if it is going to
			outer->DefWindowProc(umsg, wparam, lparam);

			if (! ::GetCapture())
			{
				outer->SetCapture();
				bGotCapture = true;
			}
			else
				bGotCapture = false;
		}

		// don't process anymore
		bHandled = TRUE;
		return 1;
	}
	
    LRESULT OnMouseUp(UINT umsg, WPARAM wparam, LPARAM lparam, BOOL& bHandled)
	{
		POINT pt;
		RECT rect;

		// two cases: windowless or not
		if( outer->m_bWndLess )
		{
			// we are a windowless control
			if( outer->m_spInPlaceSite != NULL )
			{
				LRESULT dontcare;
				outer->m_spInPlaceSite->OnDefWindowMessage(umsg, wparam, lparam, &dontcare);

				if( bGotCapture && outer->m_spInPlaceSite->GetCapture() == S_OK )
				{
					if( outer->m_spInPlaceSite->SetCapture(FALSE) == S_OK )
					{
						bGotCapture = false;
					}
				}
			}

		}
		else
		{
			// let control release capture if it is going to
			outer->DefWindowProc(umsg, wparam, lparam);

			if ((bGotCapture) && (::GetCapture()))
				::ReleaseCapture();

			bGotCapture = false;
		}

		pt.x = LOWORD(lparam);
		pt.y = HIWORD(lparam);

		if( ::IsWindow(outer->m_hWnd) )
		{
			outer->GetClientRect(&rect);	// captured msgs coords are window relative
		}
		else
		{
			rect = outer->m_rcPos;
		}

		if (::PtInRect(&rect, pt))
			outer->Fire_Click();

		// don't process anymore
		bHandled = TRUE;
		return 1;
	}
};

template <typename T = ContainerPair<> >
class CStockKeyDown :
	public OuterClassHelper<T>
{
public:

	BEGIN_MSG_MAP(CStockKeyDown<T>)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	END_MSG_MAP()

	DEFINE_GETSHIFTVAL();

    LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		outer->Fire_KeyDown(wparam, GetShiftVal());

		bHandled = FALSE;
		return 0;
	}
};

template <typename T = ContainerPair<> >
class CStockKeyUp :
	public OuterClassHelper<T>
{
public:

	BEGIN_MSG_MAP(CStockKeyUp<T>)
		MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
	END_MSG_MAP()

	DEFINE_GETSHIFTVAL();

    LRESULT OnKeyUp(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		outer->Fire_KeyUp(wparam, GetShiftVal());

		bHandled = FALSE;
		return 0;
	}
};

template <typename T = ContainerPair<> >
class CStockChange :
	public OuterClassHelper<T>
{
public:

	BEGIN_MSG_MAP(CStockChange<T>)
		MESSAGE_HANDLER(OCM_COMMAND, Changed)
	END_MSG_MAP()

	LRESULT Changed(UINT /*uMsg*/, WPARAM wparam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if (HIWORD(wparam) == EN_CHANGE)
			outer->Fire_Change();

		bHandled = FALSE;
		return 0;
	}
};

}; //namespace ATL

#endif //__ATLSTOCK_H__
