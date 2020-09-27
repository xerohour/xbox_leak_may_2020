//NONSHIP

// This is a part of the Active Template Library.
// Copyright (C) 1996-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef __ATLHTMLEDIT_H__
#define __ATLHTMLEDIT_H__

#pragma once

#include <htmledbase.h>
#include <dhtmled.h>
#include <triedcid.h>

namespace ATL
{

#define ID_HTMLEDIT 1212

//REVIEW
//get rid of these after VID gets their uuids into UUID.h
extern "C" const __declspec(selectany) GUID DIID___DHTMLEditEvents ={0x588D5040,0xCF28,0x11D1,{0x8C,0xD3,0x00,0xA0,0xC9,0x59,0xBC,0x0A}};
extern "C" const __declspec(selectany) GUID LIBID__DHTMLEDLib = {0x683364A1,0xB37D,0x11D1,{0xAD,0xC5,0x00,0x60,0x08,0xA5,0x84,0x8C}};

template <class T>
class CHtmlEditCtrlImpl :
 public CWindowImpl<T,CAxWindow>,
 public IDispEventImpl<ID_HTMLEDIT,CHtmlEditCtrlImpl,&DIID___DHTMLEditEvents ,&LIBID__DHTMLEDLib,1,0>,
 public CHtmlEditCtrlBase<CHtmlEditCtrlImpl>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CAxWindow::GetWndClassName())

	BEGIN_MSG_MAP(CHtmlEditCtrlImpl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()
//creation
	HWND Create(HWND hWndParent, _U_RECT rect = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		AtlAxWinInit();
		return CWindowImpl<T, CAxWindow>::Create(hWndParent, rect, _T("DHTMLEdit.DHTMLEdit"), dwStyle, dwExStyle, nID, lpCreateParam);
	}
	LPUNKNOWN GetControlUnknown()
	{
		return dynamic_cast<IUnknown*>(m_spDoc.p);
	}
private:
	//Data
	CComQIPtr<IDHTMLEdit> m_spDoc;
public:
	//Event Processing
	BEGIN_SINK_MAP(CHtmlEditCtrlImpl)
		SINK_ENTRY_INFO(ID_HTMLEDIT, DIID___DHTMLEditEvents, 0x1, _OnDocumentComplete, NULL)
		SINK_ENTRY_INFO(ID_HTMLEDIT, DIID___DHTMLEditEvents, 0x2, _OnDisplayChanged, NULL)
		SINK_ENTRY_INFO(ID_HTMLEDIT, DIID___DHTMLEditEvents, 0x3, _OnShowContextMenu, NULL)
		SINK_ENTRY_INFO(ID_HTMLEDIT, DIID___DHTMLEditEvents, 0x4, _OnContextMenuAction, NULL)
		SINK_ENTRY_INFO(ID_HTMLEDIT, DIID___DHTMLEditEvents, 0x5, _onmousedown, NULL)
		SINK_ENTRY_INFO(ID_HTMLEDIT, DIID___DHTMLEditEvents, 0x6, _onmousemove, NULL)
		SINK_ENTRY_INFO(ID_HTMLEDIT, DIID___DHTMLEditEvents, 0x7, _onmouseup, NULL)
		SINK_ENTRY_INFO(ID_HTMLEDIT, DIID___DHTMLEditEvents, 0x8, _onmouseout, NULL)
		SINK_ENTRY_INFO(ID_HTMLEDIT, DIID___DHTMLEditEvents, 0x9, _onmouseover, NULL)
		SINK_ENTRY_INFO(ID_HTMLEDIT, DIID___DHTMLEditEvents, 0xa, _onclick, NULL)
		SINK_ENTRY_INFO(ID_HTMLEDIT, DIID___DHTMLEditEvents, 0xb, _ondblclick, NULL)
		SINK_ENTRY_INFO(ID_HTMLEDIT, DIID___DHTMLEditEvents, 0xc, _onkeydown, NULL)
		SINK_ENTRY_INFO(ID_HTMLEDIT, DIID___DHTMLEditEvents, 0xd, _onkeypress, NULL)
		SINK_ENTRY_INFO(ID_HTMLEDIT, DIID___DHTMLEditEvents, 0xe, _onkeyup, NULL)
		SINK_ENTRY_INFO(ID_HTMLEDIT, DIID___DHTMLEditEvents, 0xf, _onblur, NULL)
		SINK_ENTRY_INFO(ID_HTMLEDIT, DIID___DHTMLEditEvents, 0x10, _onreadystatechanged, NULL)
	END_SINK_MAP()

	void __stdcall _OnDocumentComplete()
	{
		T* pT = (T*)this;
		pT->OnDocumentComplete();
	}
	void __stdcall _OnDisplayChanged()
	{
		T* pT = (T*)this;
		pT->OnDisplayChanged();

	}
	void __stdcall _OnShowContextMenu(long x, long y)
	{
		T* pT = (T*)this;
		pT->OnShowContextMenu(x,y);
	}
	void __stdcall _OnContextMenuAction(long index)
	{
		T* pT = (T*)this;
		pT->OnContextMenuAction(index);
	}
	void __stdcall _onmousedown()
	{
		T* pT = (T*)this;
		pT->onmousedown();	
	}
	void __stdcall _onmousemove()
	{
		T* pT = (T*)this;
		pT->onmousemove();
	}
	void __stdcall _onmouseup()
	{
		T* pT = (T*)this;
		pT->onmouseup();	
	}
	void __stdcall _onmouseout()
	{
		T* pT = (T*)this;
		pT->onmouseout();
	}
	void __stdcall _onmouseover()
	{
		T* pT = (T*)this;
		pT->onmouseover();
	}
	void __stdcall _onclick()
	{
		T* pT = (T*)this;
		pT->onclick();
	}
	void __stdcall _ondblclick()
	{
		T* pT = (T*)this;
		pT->ondblclick();
	}
	void __stdcall _onkeydown()
	{
		T* pT = (T*)this;
		pT->onkeydown();
	}
	void __stdcall _onkeypress()
	{
		T* pT = (T*)this;
		pT->onkeypress();
	}
	void __stdcall _onkeyup()
	{
		T* pT = (T*)this;
		pT->onkeyup();
	}
	void __stdcall _onblur()
	{
		T* pT = (T*)this;
		pT->onblur();
	}
	void __stdcall _onreadystatechanged()
	{
		T* pT = (T*)this;
		pT->onreadystatechanged();
	}
	void OnDocumentComplete()
	{
		ATLTRACE("OnDocumentComplete\n");
	}
	void OnDisplayChanged()
	{
		ATLTRACE("OnDisplayChanged\n");
	}
	void OnShowContextMenu(long x, long y)
	{
		ATLTRACE("OnShowContextMenu %d,%d\n",x,y);
	}
	void OnContextMenuAction(long index)
	{
		ATLTRACE("OnContextMenuAction %d\n", index);
	}
	void onmousedown()
	{
		ATLTRACE("onmousedown\n");
	}
	void onmousemove()
	{
		ATLTRACE("onmousemove\n");
	}
	void onmouseup()
	{
		ATLTRACE("onmouseup\n");
	}
	void onmouseout()
	{
		ATLTRACE("onmouseout\n");
	}
	void onmouseover()
	{
		ATLTRACE("onmouseover\n");
	}
	void onclick()
	{
		ATLTRACE("onclick\n");
	}
	void ondblclick()
	{
		ATLTRACE("ondblclick\n");
	}
	void onkeydown()
	{
		ATLTRACE("onkeydown\n");
	}
	void onkeypress()
	{
		ATLTRACE("onkeypress\n");
	}
	void onkeyup()
	{
		ATLTRACE("onkeyup\n");
	}
	void onblur()
	{
		ATLTRACE("onblur\n");
	}
	void onreadystatechanged()
	{
		ATLTRACE("onreadystatechanged\n");
	}

//Implementation
		LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		LRESULT nRet = DefWindowProc();
		if(nRet != -1)
		{
			CComPtr<IUnknown> spUnk;
			if(S_OK == AtlAxGetControl(m_hWnd, &spUnk))
			{
				m_spDoc = spUnk;
				DispEventAdvise(spUnk,&DIID___DHTMLEditEvents);
			}
		}
		return nRet;
	}
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if(m_spDoc)
		{
			DispEventUnadvise((IUnknown*)m_spDoc.p, &DIID___DHTMLEditEvents);
			m_spDoc.Release();
		}
		bHandled=FALSE;
		return 1;
	}

}; //CHtmlEditCtrlImpl

}; //namespace ATL

#endif //__ATLHTMLEDIT_H__
