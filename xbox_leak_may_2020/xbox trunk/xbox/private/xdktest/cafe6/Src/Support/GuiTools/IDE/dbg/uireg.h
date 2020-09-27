///////////////////////////////////////////////////////////////////////////////
//  UIREG.H
//
//  Created by :            Date :
//      WayneBr             1/14/94
//
//  Description :
//      Declaration of the UIRegisters class
//

#ifndef __UIREG_H__
#define __UIREG_H__

#include "..\sym\qcqp.h"
#include "..\shl\udockwnd.h"

#include "dbgxprt.h"

#ifndef __UIDOCKWND_H__
	#error include 'udockwnd.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//  UIRegisters symbols

// enums must start at 1
const MaxReg=24;   

// Intel Specific
enum {EAX=1 , EBX , ECX , EDX , ESI , EDI , EIP , EBP , ESP , EFL , CS , DS , ES , SS , FS , GS };

// 68k Specific
enum {D0=1, D1, D2, D3, D4, D5, D6, D7,
	   A0,   A1, A2, A3, A4, A5, A6, A7,
	   PC,   SR,  X,  N,  Z,  V,  C };


///////////////////////////////////////////////////////////////////////////////
//  UIRegisters class

// BEGIN_CLASS_HELP
// ClassName: UIRegisters
// BaseClass: UIDockWindow
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS UIRegisters : public UIDockWindow

	{
	UIWND_COPY_CTOR(UIRegisters, UIDockWindow);

	virtual BOOL Activate(void);
	virtual UINT GetID(void) const
		{	return IDW_CPU_WIN; }

	virtual BOOL AttachActive(void);
	virtual BOOL Find(void);

	// Initialization
	public:

	// Data
	private:
		int m_option;

	// UIWindow overrides
	public:
		virtual BOOL IsValid(void) const;
		virtual BOOL IsActive(void);	// derived classes must provide their own Activate (usually a DoCommand)


	// Utilities
	public:
		int GetRegister(int reg);
		BOOL SetRegister(int reg, LPCSTR value);

		BOOL ToggleFloatingPoint(void);
		CString GetRegister(CString csRegister, int *pnValue);
		BOOL SetRegister(CString csRegister, CString szValue, int nValue, BOOL bValueIsCString = TRUE);
		BOOL RegisterDisplayed(CString csRegister);
	};

#endif
