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

#ifndef __ATLPRINT_H__
#define __ATLPRINT_H__

#pragma once

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

namespace ATL
{

//This class wraps all of the PRINTER_INFO_* structures
//and provided by ::GetPrinter.
class CPrinterInfo
{
public:
	PRINTER_INFO_1* pi1;
	PRINTER_INFO_2* pi2;
	PRINTER_INFO_3* pi3;
	PRINTER_INFO_4* pi4;
	PRINTER_INFO_5* pi5;
	CPrinterInfo()
	{
		pi1 = NULL;
		pi2 = NULL;
		pi3 = NULL;
		pi4 = NULL;
		pi5 = NULL;
	}
	~CPrinterInfo()
	{
		Cleanup();
	}
	bool GetPrinterInfo(HANDLE hPrinter, int nTypes)
	{
		Cleanup();
		bool b = true;
		if (nTypes & 0x1)
			b &= GetPrinterInfoHelper(hPrinter, (BYTE**)&pi1, 1);
		if (nTypes & 0x2)
			b &= GetPrinterInfoHelper(hPrinter, (BYTE**)&pi2, 2);
		if (nTypes & 0x4)
			b &= GetPrinterInfoHelper(hPrinter, (BYTE**)&pi3, 3);
		if (nTypes & 0x8)
			b &= GetPrinterInfoHelper(hPrinter, (BYTE**)&pi4, 4);
		if (nTypes & 0x10)
			b &= GetPrinterInfoHelper(hPrinter, (BYTE**)&pi5, 5);
		return b;
	}

//Implementation
protected:
	void Cleanup()
	{
		delete [] (BYTE*)pi1;
		delete [] (BYTE*)pi2;
		delete [] (BYTE*)pi3;
		delete [] (BYTE*)pi4;
		delete [] (BYTE*)pi5;
		pi1 = NULL;
		pi2 = NULL;
		pi3 = NULL;
		pi4 = NULL;
		pi5 = NULL;
	}
	static bool GetPrinterInfoHelper(HANDLE hPrinter, BYTE** pi, int nIndex)
	{
		DWORD dw = 0;
		BOOL b = ::GetPrinter(hPrinter, nIndex, NULL, 0, &dw);
		DWORD dwErr = GetLastError();
		BYTE* pb = NULL;
		ATLTRY(pb = new BYTE[dw]);
		if (pb != NULL)
		{
			memset(pb, 0, dw);
			DWORD dwNew;
			if (!::GetPrinter(hPrinter, nIndex, pb, dw, &dwNew))
			{
				delete [] pb;
				pb = NULL;
			}
		}
		*pi = pb;
		return (pb != NULL);
	}
};

//Provides a wrapper class for a HANDLE to a printer.
template <bool t_bManaged>
class CPrinterT
{
public:
	HANDLE m_hPrinter;
	CPrinterT()
	{
		m_hPrinter = NULL;
	}
	CPrinterT& operator=(HANDLE hPrinter)
	{
		if (hPrinter != m_hPrinter)
		{
			ClosePrinter();
			m_hPrinter = hPrinter;
		}
		return *this;
	}

	bool OpenPrinter(HANDLE hDevNames, const DEVMODE* pDevMode = NULL)
	{
		bool b = false;
		DEVNAMES* pdn = (DEVNAMES*)GlobalLock(hDevNames);
		if (pdn != NULL)
		{
			LPTSTR lpszPrinterName = (LPTSTR)pdn + pdn->wDeviceOffset;
			b = OpenPrinter(lpszPrinterName, pDevMode);
			GlobalUnlock(hDevNames);
		}
		return b;
	}
	bool OpenPrinter(LPCTSTR lpszPrinterName, const DEVMODE* pDevMode = NULL)
	{
		ClosePrinter();
		PRINTER_DEFAULTS pdefs = {NULL, (DEVMODE*)pDevMode, PRINTER_ACCESS_USE};
		::OpenPrinter((LPTSTR) lpszPrinterName, &m_hPrinter, 
			(pDevMode == NULL) ? NULL : &pdefs);

		return (m_hPrinter != NULL);
	}
	bool OpenPrinter(LPCTSTR lpszPrinterName, PRINTER_DEFAULTS* pprintdefs)
	{
		ClosePrinter();
		::OpenPrinter((LPTSTR) lpszPrinterName, &m_hPrinter, pprintdefs);
		return (m_hPrinter != NULL);
	}
	bool OpenDefaultPrinter(const DEVMODE* pDevMode = NULL)
	{
		ClosePrinter();
		TCHAR buffer[512];
		buffer[0] = 0;
		GetProfileString("windows", "device", ",,,", buffer, sizeof(buffer));
		int nLen = lstrlen(buffer);
		if (nLen != 0)
		{
			LPTSTR lpsz = buffer;
			while (*lpsz)
			{
				if (*lpsz == ',')
				{
					*lpsz = 0;
					break;
				}
				lpsz = CharNext(lpsz);
			}
			PRINTER_DEFAULTS pdefs = {NULL, (DEVMODE*)pDevMode, PRINTER_ACCESS_USE};
			::OpenPrinter(buffer, &m_hPrinter, 
				(pDevMode == NULL) ? NULL : &pdefs);
		}
		return m_hPrinter != NULL;
	}
	void ClosePrinter()
	{
		if (m_hPrinter != NULL)
		{
			if (t_bManaged)
				::ClosePrinter(m_hPrinter);
			m_hPrinter = NULL;
		}
	}
	~CPrinterT()
	{
		ClosePrinter();
	}
	bool GetPrinterInfo(CPrinterInfo* pinfo, int nTypes) const
	{
		return pinfo->GetPrinterInfo(m_hPrinter, nTypes);
	}
	bool PrinterProperties(HWND hWnd = NULL)
	{
		if (hWnd == NULL)
			hWnd = ::GetActiveWindow();
		return !!::PrinterProperties(hWnd, m_hPrinter);
	}
	HANDLE CopyToHDEVNAMES() const
	{
		HANDLE h = NULL;
		CPrinterInfo pinfon;
		LPTSTR lpszPrinterName = NULL;
		//Some printers fail for PRINTER_INFO_5 in some situations
		if (GetPrinterInfo(&pinfon, 0x10))
			lpszPrinterName = pinfon.pi5->pPrinterName;
		else if (GetPrinterInfo(&pinfon, 0x2))
			lpszPrinterName = pinfon.pi2->pPrinterName;
		if (lpszPrinterName != NULL)
		{
			int nLen = sizeof(DEVNAMES)+ (lstrlen(lpszPrinterName)+1)*sizeof(TCHAR);
			h = GlobalAlloc(GMEM_MOVEABLE, nLen);
			BYTE* pv = (BYTE*)GlobalLock(h);
			DEVNAMES* pdev = (DEVNAMES*)pv;
			if (pv != NULL)
			{
				memset(pv, 0, nLen);
				pdev->wDeviceOffset = sizeof(DEVNAMES)/sizeof(TCHAR);
				pv = pv + sizeof(DEVNAMES); //now points to end
				lstrcpy((LPTSTR)pv, lpszPrinterName);
				GlobalUnlock(h);
			}
		}
		return h;
	}
	HDC CreatePrinterDC(const DEVMODE* pdm = NULL)
	{
		CPrinterInfo pinfon;
		HDC hDC = NULL;
		LPTSTR lpszPrinterName = NULL;
		//Some printers fail for PRINTER_INFO_5 in some situations
		if (GetPrinterInfo(&pinfon, 0x10))
			lpszPrinterName = pinfon.pi5->pPrinterName;
		else if (GetPrinterInfo(&pinfon, 0x2))
			lpszPrinterName = pinfon.pi2->pPrinterName;
		if (lpszPrinterName != NULL)
			hDC = ::CreateDC(NULL, lpszPrinterName, NULL, pdm);
		return hDC;
	}
	HDC CreatePrinterIC(const DEVMODE* pdm = NULL)
	{
		CPrinterInfo pinfon;
		HDC hDC = NULL;
		LPTSTR lpszPrinterName = NULL;
		if (GetPrinterInfo(&pinfon, 0x10))
			lpszPrinterName = pinfon.pi5->pPrinterName;
		else if (GetPrinterInfo(&pinfon, 0x2))
			lpszPrinterName = pinfon.pi2->pPrinterName;
		if (lpszPrinterName != NULL)
			hDC = ::CreateIC(NULL, lpszPrinterName, NULL, pdm);
		return hDC;
	}

	void Attach(HANDLE hPrinter)
	{
		ClosePrinter();
		m_hPrinter = hPrinter;
	}

	HANDLE Detach()
	{
		HANDLE hPrinter = m_hPrinter;
		m_hPrinter = NULL;
		return hPrinter;
	}
	operator HANDLE() const {return m_hPrinter;}
};

typedef CPrinterT<true> CPrinter;
typedef CPrinterT<false> CPrinterTemp;

template <bool t_bManaged>
class CDevModeT
{
public:
	HANDLE m_hDevMode;
	DEVMODE* m_pDevMode;
	CDevModeT(HANDLE hDevMode = NULL)
	{
		m_hDevMode = hDevMode;
		m_pDevMode = (m_hDevMode != NULL) ? (DEVMODE*)GlobalLock(m_hDevMode) : NULL;
	}
	~CDevModeT()
	{
		Cleanup();
	}
	CDevModeT<t_bManaged>& operator=(HANDLE hDevMode)
	{
		Attach(hDevMode);
		return *this;
	}

	void Attach(HANDLE hDevModeNew)
	{
		Cleanup();
		m_hDevMode = hDevModeNew;
		m_pDevMode = (m_hDevMode != NULL) ? (DEVMODE*)GlobalLock(m_hDevMode) : NULL;
	}

	HANDLE Detach()
	{
		if (m_hDevMode != NULL)
			GlobalUnlock(m_hDevMode);
		HANDLE hDevMode = m_hDevMode;
		m_hDevMode = NULL;
		return hDevMode;
	}
	bool CopyFromPrinter(HANDLE hPrinter)
	{
		CPrinterInfo pinfo;
		bool b = pinfo.GetPrinterInfo(hPrinter, 0x2);
		if (b)
		 b = CopyFromDEVMODE(pinfo.pi2->pDevMode);
		return b;
	}
	bool CopyFromDEVMODE(const DEVMODE* pdm)
	{
		if (pdm == NULL)
			return false;
		int nSize = pdm->dmSize + pdm->dmDriverExtra;
		HANDLE h = GlobalAlloc(GMEM_MOVEABLE, nSize);
		if (h != NULL)
		{
			void* p = GlobalLock(h);
			memcpy(p, pdm, nSize);
			GlobalUnlock(h);
		}
		Attach(h);
		return (h != NULL);
	}
	bool CopyFromHDEVMODE(HANDLE hdm)
	{
		bool b = false;
		if (hdm != NULL)
		{
			DEVMODE* pdm = (DEVMODE*)GlobalLock(hdm);
			b = CopyFromDEVMODE(pdm);
			GlobalUnlock(hdm);
		}
		return b;
	}
	HANDLE CopyToHDEVMODE()
	{
		if ((m_hDevMode == NULL) || (m_pDevMode == NULL))
			return NULL;
		int nSize = m_pDevMode->dmSize + m_pDevMode->dmDriverExtra;
		HANDLE h = GlobalAlloc(GMEM_MOVEABLE, nSize);
		if (h != NULL)
		{
			void* p = GlobalLock(h);
			memcpy(p, m_pDevMode, nSize);
		}
		return h;
	}
	//If this devmode was for another printer, this will create a new devmode
	//based on the existing devmode, but retargeted at the new printer
	bool UpdateForNewPrinter(HANDLE hPrinter)
	{
		LONG nLen = ::DocumentProperties(NULL, hPrinter, NULL, NULL, NULL, 0);
		DEVMODE* pdm = (DEVMODE*) alloca(nLen);
		memset(pdm, 0, nLen);
		LONG l = ::DocumentProperties(NULL, hPrinter, NULL, pdm, m_pDevMode,
			DM_IN_BUFFER|DM_OUT_BUFFER);
		bool b = false;
		if (l == IDOK)
			b = CopyFromDEVMODE(pdm);
		return b;
	}
	bool DocumentProperties(HANDLE hPrinter, HWND hWnd = NULL)
	{
		CPrinterInfo pi;
		pi.GetPrinterInfo(hPrinter, 0x1);
		if (hWnd == NULL)
			hWnd = ::GetActiveWindow();

		LONG nLen = ::DocumentProperties(hWnd, hPrinter, pi.pi1->pName, NULL, NULL, 0);
		DEVMODE* pdm = (DEVMODE*) alloca(nLen);
		memset(pdm, 0, nLen);
		LONG l = ::DocumentProperties(hWnd, hPrinter, pi.pi1->pName, pdm,
			m_pDevMode, DM_IN_BUFFER|DM_OUT_BUFFER|DM_PROMPT);
		bool b = false;
		if (l == IDOK)
			b = CopyFromDEVMODE(pdm);
		return b;
	}
	operator HANDLE() const {return m_hDevMode;}
	operator DEVMODE*() const {return m_pDevMode;}
protected:
	void Cleanup()
	{
		if (m_hDevMode != NULL)
		{
			GlobalUnlock(m_hDevMode);
			if(t_bManaged)
				GlobalFree(m_hDevMode);
			m_hDevMode = NULL;
		}
	}
};

typedef CDevModeT<true> CDevMode;
typedef CDevModeT<false> CDevModeTemp;

class CWinPrinterDC : public CWinManagedDC
{
public:

	CWinPrinterDC(bool bAutoRestore = true) : CWinManagedDC(NULL, bAutoRestore)
	{
		CPrinter printer;
		printer.OpenDefaultPrinter();
		Attach(printer.CreatePrinterDC());
	}

	CWinPrinterDC(HANDLE hPrinter, const DEVMODE* pdm = NULL, bool bAutoRestore = true) : CWinManagedDC(NULL, bAutoRestore)
	{
		CPrinterTemp p;
		p.Attach(hPrinter);
		Attach(p.CreatePrinterDC(pdm));
	}
	~CWinPrinterDC()
	{
		ATLASSERT(m_hDC != NULL);

		if(m_bAutoRestore)
			RestoreAllObjects();

		DeleteDC();
	}
};

//Defines callbacks used by CPrintJob
//This is not a COM interface
class __declspec(novtable) IPrintJobInfo
{
public:
	virtual void BeginPrintJob(HDC hDC)=0; //allocate handles needed, etc
	virtual void EndPrintJob(HDC hDC, bool bAborted)=0;	// free handles, etc
	virtual void PrePrintPage(UINT nPage, HDC hDC)=0;
	virtual bool PrintPage(UINT nPage, HDC hDC)=0;
	virtual void PostPrintPage(UINT nPage, HDC hDC)=0;
	//GetNewDevModeForPage allows only a change to be provided
	virtual DEVMODE* GetNewDevModeForPage(UINT nLastPage, UINT nPage)=0;
};

//Provides a default implementatin for IPrintJobInfo
//Typically, MI'd into a document or view class
class __declspec(novtable) CPrintJobInfo : public IPrintJobInfo
{
public:
	virtual void BeginPrintJob(HDC hDC) //allocate handles needed, etc
	{
	}
	virtual void EndPrintJob(HDC hDC, bool bAborted)	// free handles, etc
	{
	}
	virtual void PrePrintPage(UINT nPage, HDC hDC)
	{
		m_nPJState = ::SaveDC(hDC);
	}
	virtual bool PrintPage(UINT nPage, HDC hDC) = 0;
	virtual void PostPrintPage(UINT nPage, HDC hDC)
	{
		RestoreDC(hDC, m_nPJState);
	}
	virtual DEVMODE* GetNewDevModeForPage(UINT nLastPage, UINT nPage)
	{
		return NULL;
	}
private:
	int m_nPJState;
};

//Wraps a set of tasks for a specific printer (StartDoc/EndDoc)
//Handles aborting, background printing
class CPrintJob
{
public:
	CPrinterTemp m_printer;
	IPrintJobInfo* m_pInfo;
	DEVMODE* m_pDefDevMode;
	DOCINFO m_docinfo;
	DWORD m_dwJobID;
	bool m_bCancel;
	bool m_bComplete;
	bool m_bMultiDevMode;
	unsigned long m_nStartPage;
	unsigned long m_nEndPage;
	CPrintJob()
	{
		m_dwJobID = 0;
		m_bCancel = false;
		m_bComplete = true;
	}
	~CPrintJob()
	{
		ATLASSERT(IsJobComplete()); //premature destruction?
	}
	bool IsJobComplete() {return m_bComplete;}
	bool StartPrintJob(bool bBackground, HANDLE hPrinter, DEVMODE* pDefaultDevMode,
		IPrintJobInfo* pInfo, LPCTSTR lpszDocName, 
		unsigned long nStartPage, unsigned long nEndPage)
	{
		ATLASSERT(m_bComplete); //previous job not done yet?
		if (pInfo == NULL)
			return false;
		memset(&m_docinfo, 0, sizeof(m_docinfo));
		m_docinfo.cbSize = sizeof(m_docinfo);
		m_docinfo.lpszDocName = lpszDocName;
		m_pInfo = pInfo;
		m_nStartPage = nStartPage;
		m_nEndPage = nEndPage;
		m_printer.Attach(hPrinter);
		m_pDefDevMode = pDefaultDevMode;
		DEVMODE* pdm = m_pInfo->GetNewDevModeForPage(0, 1);
		m_bMultiDevMode = (pdm != NULL);
		m_bComplete = false;
		if (!bBackground)
		{
			m_bComplete = true;
			return StartHelper();
		}

		//Create a thread and return

		DWORD dwThreadID = 0;
		HANDLE hThread = CreateThread(NULL, 0, StartProc, (void*)this, 0, &dwThreadID);
		CloseHandle(hThread);
		return (hThread != NULL);
	}
//Implementation
protected:
	static DWORD WINAPI StartProc(void* p)
	{
		CPrintJob* pThis = (CPrintJob*)p;
		pThis->StartHelper();
		pThis->m_bComplete = true;
		return 0;
	}
	bool StartHelper()
	{
		CWinManagedDC dcPrinter;
		dcPrinter.Attach(m_printer.CreatePrinterDC(m_pDefDevMode));
		if (dcPrinter.m_hDC == NULL)
			return false;
			
		m_dwJobID = ::StartDoc(dcPrinter, &m_docinfo);
		if (m_dwJobID == 0)
			return false;

		m_pInfo->BeginPrintJob(dcPrinter);

		//print all the pages now
		unsigned long nPage;
		unsigned long nLastPage=0;
		for (nPage = m_nStartPage; nPage <= m_nEndPage; nPage++)
		{
			if (m_bMultiDevMode)
			{
				DEVMODE* pdm = m_pInfo->GetNewDevModeForPage(nLastPage, nPage);
				if (pdm == NULL)
					pdm = m_pDefDevMode;
				dcPrinter.ResetDC(pdm);
			}
			dcPrinter.StartPage();
			m_pInfo->PrePrintPage(nPage, dcPrinter);
			if (!m_pInfo->PrintPage(nPage, dcPrinter))
				m_bCancel = true;
			m_pInfo->PostPrintPage(nPage, dcPrinter);
			dcPrinter.EndPage();
			if (m_bCancel)
				break;
			nLastPage = nPage;
		}

		m_pInfo->EndPrintJob(dcPrinter, m_bCancel);
		if (m_bCancel)
			::AbortDoc(dcPrinter);
		else
			::EndDoc(dcPrinter);
		m_dwJobID = 0;
		return true;
	}
	//Cancels a print job.  Can be called asynchronously.
	bool CancelPrintJob()
	{
		m_bCancel = 1;
	}
};

class CPrintPreview //-- Adds print preview support to an existing window
{
public:
	IPrintJobInfo* m_pInfo;
	CPrinterTemp m_printer;
	CEnhMetaFile meta;
	DEVMODE* m_pDefDevMode;
	DEVMODE* m_pCurDevMode;
	SIZE m_sizeCurPhysOffset;
	void SetPrintPreviewInfo(HANDLE hPrinter, DEVMODE* pDefaultDevMode, IPrintJobInfo* pji)
	{
		m_printer.Attach(hPrinter);
		m_pDefDevMode = pDefaultDevMode;
		m_pInfo = pji;
		m_nCurPage = 0;
		m_pCurDevMode = NULL;
	}
	void SetEnhMetaFile(HENHMETAFILE hEMF)
	{
		meta = hEMF;
	}
	void SetPage(int nPage)
	{
		m_nCurPage = nPage;
		m_pCurDevMode = m_pInfo->GetNewDevModeForPage(0, nPage);
		if (m_pCurDevMode == NULL)
			m_pCurDevMode = m_pDefDevMode;
		CWinManagedDC dcPrinter = m_printer.CreatePrinterDC(m_pCurDevMode);

		int iWidthMM = dcPrinter.GetDeviceCaps(HORZSIZE)*100; 
		int iHeightMM = dcPrinter.GetDeviceCaps(VERTSIZE)*100; 
		RECT rcMM = {0,0, iWidthMM, iHeightMM};

		m_sizeCurPhysOffset.cx = dcPrinter.GetDeviceCaps(PHYSICALOFFSETX);
		m_sizeCurPhysOffset.cy = dcPrinter.GetDeviceCaps(PHYSICALOFFSETY);
		
		CWinEnhMetaFileDC dcMeta(dcPrinter, &rcMM);
		m_pInfo->PrePrintPage(nPage, dcMeta);
		m_pInfo->PrintPage(nPage, dcMeta);
		m_pInfo->PostPrintPage(nPage, dcMeta);
		meta.Attach(dcMeta.Close());
	}
	void GetPageRect(RECT& rc, RECT* prc)
	{
		int x1 = rc.right-rc.left;
		int y1 = rc.bottom - rc.top;
		if ((x1 < 0) || (y1 < 0))
			return;

		CEnhMetaFileInfo emfinfo(meta);
		ENHMETAHEADER* pmh = emfinfo.GetEnhMetaFileHeader();

		//Compute whether we are OK vertically or horizontally
		int x2 = pmh->szlDevice.cx;
		int y2 = pmh->szlDevice.cy;
		int y1p = MulDiv(x1, y2, x2);
		int x1p = MulDiv(y1, x2, y2);
		ATLASSERT( (x1p <= x1) || (y1p <= y1));
		if (x1p <= x1)
		{
			prc->left = rc.left + (x1 - x1p)/2;
			prc->right = prc->left + x1p;
			prc->top = rc.top;
			prc->bottom = rc.bottom;
		}
		else
		{
			prc->left = rc.left;
			prc->right = rc.right;
			prc->top = rc.top + (y1 - y1p)/2;
			prc->bottom = prc->top + y1p;
		}
	}
	void DoPaint(HDC hDC, RECT& rc)
	{
		CEnhMetaFileInfo emfinfo(meta);
		ENHMETAHEADER* pmh = emfinfo.GetEnhMetaFileHeader();
		int nOffsetX = MulDiv(m_sizeCurPhysOffset.cx, rc.right-rc.left, pmh->szlDevice.cx);
		int nOffsetY = MulDiv(m_sizeCurPhysOffset.cy, rc.bottom-rc.top, pmh->szlDevice.cy);

		CWinDC dc = hDC;
		dc.OffsetWindowOrg(-nOffsetX, -nOffsetY);
		dc.PlayMetaFile(meta, &rc);
	}
protected:
	int m_nCurPage;
};

struct _ATL_PP_BUTTON_INFO
{
public:
	UINT nID;
	TCHAR szText[128];
	int nWidth;
};


class CPrintPreviewWindow : public CWindowImpl<CPrintPreviewWindow>, 
	public CPrintPreview
{
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP(CPrintPreviewWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	END_MSG_MAP()

	CPrintPreviewWindow()
	{
		m_pArrBI = NULL;
		m_nMaxPage = m_nMinPage = 0;
	}
	~CPrintPreviewWindow()
	{
		delete [] m_pArrBI;
	}

	bool NextPage()
	{
		if (m_nCurPage == m_nMaxPage)
			return false;
		SetPage(m_nCurPage+1);
		Invalidate();
		return true;
	}
	bool PrevPage()
	{
		if (m_nCurPage == m_nMinPage)
			return false;
		SetPage(m_nCurPage-1);
		Invalidate();
		return true;
	}
	void SetPrintPreviewInfo(HANDLE hPrinter, DEVMODE* pDefaultDevMode, 
		IPrintJobInfo* pji, HMENU hMenu, int nMinPage, int nMaxPage)
	{
		CPrintPreview::SetPrintPreviewInfo(hPrinter, pDefaultDevMode, pji);
		//pick apart menu and create buttons for the top of the window
		delete [] m_pArrBI;
		m_pArrBI = NULL;
		m_nMinPage = nMinPage;
		m_nMaxPage = nMaxPage;
		if (hMenu != NULL)
		{
			CWinWindowDC dc(m_hWnd);
			SIZE size;
			dc.GetTextExtent("X", 1, &size);
			m_nButtonHeight = size.cy + size.cy/2;
			m_nButtonBuffer = size.cx;
			CWinMenu menu = hMenu;
			m_nCountButtons = menu.GetMenuItemCount();
			m_pArrBI = NULL;
			ATLTRY(m_pArrBI = new _ATL_PP_BUTTON_INFO[m_nCountButtons]);
			for (int i=0;i<m_nCountButtons;i++)
			{
				m_pArrBI[i].nID = menu.GetMenuItemID(i);
				m_pArrBI[i].szText[0] = NULL;
				menu.GetMenuString(i, m_pArrBI[i].szText, sizeof(m_pArrBI[i].szText)/sizeof(TCHAR), MF_BYPOSITION);
				SIZE size;
				dc.GetTextExtent(m_pArrBI[i].szText, lstrlen(m_pArrBI[i].szText), &size);
				m_pArrBI[i].nWidth = size.cx + 2*m_nButtonBuffer;
			}
		}
	}
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		int nBeg=0;
		for (int i=0;i<m_nCountButtons;i++)
		{
			CWindow wnd;
			RECT rc = {nBeg, 0, nBeg+m_pArrBI[i].nWidth, m_nButtonHeight};
			wnd.Create("Button", m_hWnd, rc, m_pArrBI[i].szText, 
				WS_VISIBLE|WS_CLIPSIBLINGS|WS_CHILD|BS_PUSHBUTTON,0, m_pArrBI[i].nID);
			wnd.ShowWindow(SW_SHOW);
			nBeg += m_pArrBI[i].nWidth;
		}
		return 0;
	}
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 1;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CWinPaintDC dc(m_hWnd);
		RECT rc, rcClient, rcArea;
		GetClientRect(&rcClient);
		memcpy(&rcArea, &rcClient, sizeof(RECT));
		rcArea.top += m_nButtonHeight+10;
		rcArea.bottom -= 10;
		rcArea.left += 10;
		rcArea.right -= 10;
		GetPageRect(rcArea, &rc);
		CWinManagedRgn rgn1, rgn2;
		rgn1.CreateRectRgnIndirect(&rc);
		rgn2.CreateRectRgnIndirect(&rcClient);
		rgn2.CombineRgn(rgn1, RGN_DIFF);
		dc.SelectClipRgn(rgn2);
		dc.FillRect(&rcClient, (HBRUSH)(COLOR_BTNFACE+1));
		dc.SelectClipRgn(NULL);
		dc.FillRect(&rc, (HBRUSH)WHITE_BRUSH);
		DoPaint(dc, rc);
		return 0;
	}
protected:
	_ATL_PP_BUTTON_INFO* m_pArrBI;
	int m_nCountButtons;
	int m_nButtonHeight;
	int m_nButtonBuffer;
	int m_nMinPage;
	int m_nMaxPage;
};

}; //namespace ATL

#endif // __ATLPRINT_H__
