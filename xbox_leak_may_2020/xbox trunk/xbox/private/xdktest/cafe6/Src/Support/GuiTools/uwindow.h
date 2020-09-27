///////////////////////////////////////////////////////////////////////////////
//      UIWINDOW.H
//
//      Created by :                    Date :
//              DavidGa                                 9/20/93
//
//      Description :
//              Declaration of the UIWindow class
//
//              The UIWindow is the basis of all utility classes associated with a
//              specific window.  Derive a new class from UIWindow for each new window
//              tpye necessary.
//

#ifndef __UIWINDOW_H__
#define __UIWINDOW_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "guixprt.h"

enum UIWND_SPECIAL { UIWND_NONE, UIWND_SELF, UIWND_ACTIVE };

enum WINDOW_SIDE		{SLeft, STop, SRight, SBottom};
enum MOVE_DIRECTION	{DLeft, DUp, DRight, DDown};


///////////////////////////////////////////////////////////////////////////////
//      UIWindow class

// BEGIN_CLASS_HELP
// ClassName: UIWindow
// BaseClass: None
// Category: General
// END_CLASS_HELP
class GUI_CLASS UIWindow
{
public:
	void SetHWnd(HWND hWnd) { m_hWnd = hWnd; };
	UIWindow(HWND hwnd = NULL);
	HWND operator=(HWND hwnd);
	operator HWND ( ) {return HWnd();} ;
	operator BOOL() {return  m_hWnd == 0;} ;
	BOOL Attach(HWND hwnd);
	virtual inline BOOL AttachActive(void);
	virtual BOOL Find(void);
	inline BOOL Detach(void)
		{       return Attach(NULL); }
protected:
	virtual void OnUpdate(void);

// Data
private:
	HWND m_hWnd;

// Attributes
public:
	virtual BOOL IsValid(void) const;

	inline HWND HWnd(void) const
		{       return m_hWnd; }
	inline CWnd* PWnd(void) const
		{       return CWnd::FromHandle(m_hWnd); }
	inline operator HWND() const
		{       return m_hWnd; }

	inline BOOL operator==(HWND hwnd)
		{       return hwnd == HWnd(); }
	inline BOOL operator!=(HWND hwnd)
		{       return hwnd != HWnd(); }


// Utilities
public:
	LPCSTR GetText(LPSTR sz, int cMax) const;
	CString GetText(void) const;
	virtual BOOL IsActive(void);
	virtual BOOL Activate(void);
	virtual BOOL BlockingActivate(void);
	inline BOOL IsVisible(void) const
		{       return ::IsWindowVisible(HWnd()); }
	BOOL HasFocus(void) const;
	HWND WaitAttachActive(DWORD dwMilliseconds);
	BOOL WaitUntilGone(DWORD dwMilliSeconds);
	int ResizeWindow(BOOL bIsDocked, WINDOW_SIDE wSide, MOVE_DIRECTION wDirection, int count, BOOL DragFullWindows);
	int MoveWindow(BOOL bIsDocked, MOVE_DIRECTION wDirection, int count, BOOL DragFullWindows);

};

#define ExpectValid() EXPECT(IsValid())

/*      UIWND_COPY_CTOR
 *              Macro that makes constructors and operator= effectively virtual.
 *              Both functions operate by calling their base class versions until
 *              UIWindow is reached, which sets m_hWnd to the new value.
 *              On the way back up, each class's OnUpdate is called, so that any other
 *              member variables can be set up.
 */
#define UIWND_COPY_CTOR(this_class, base_class) \
	public: \
	inline this_class(HWND hwnd = NULL) \
		: base_class(hwnd) \
		{       this_class::OnUpdate(); } \
	inline HWND operator=(HWND hwnd) \
		{       base_class::operator=(hwnd); \
			/*this_class::OnUpdate();*/ \
			return hwnd; } \

#endif //__UIWINDOW_H__
