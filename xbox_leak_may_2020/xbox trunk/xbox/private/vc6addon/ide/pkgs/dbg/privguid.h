#ifndef __PRIVGUID_H
#define __PRIVGUID_H

// Copyright 1996, Microsoft Corp.

// This file contains dispatch GUID's for the debugger automation object model that
// we don't want to expose to the public.

// Debugger object
// dispatch interface GUID
// {34C63002-AE64-11cf-AB59-00AA00C091A1}
DEFINE_GUID(IID_IDispDebugger,0x34C63002L,0xAE64,0x11CF,0xAB,0x59,0x00,0xAA,0x00,0xC0,0x91,0xA1);

// Debugger object
// event source dispatch interface GUID
// {34C6301B-AE64-11cf-AB59-00AA00C091A1}
DEFINE_GUID(IID_IDispDebuggerEvents,0x34C6301BL,0xAE64,0x11CF,0xAB,0x59,0x00,0xAA,0x00,0xC0,0x91,0xA1);

// Breakpoint object
// dispatch interface GUID
// {34C63005-AE64-11cf-AB59-00AA00C091A1}
DEFINE_GUID(IID_IDispBreakpoint,0x34C63005L,0xAE64,0x11CF,0xAB,0x59,0x00,0xAA,0x00,0xC0,0x91,0xA1);

// Breakpoints collection
// dispatch interface GUID
// {34C63008-AE64-11cf-AB59-00AA00C091A1}
DEFINE_GUID(IID_IDispBreakpoints,0x34C63008L,0xAE64,0x11CF,0xAB,0x59,0x00,0xAA,0x00,0xC0,0x91,0xA1);

#endif // __PRIVGUID_H
