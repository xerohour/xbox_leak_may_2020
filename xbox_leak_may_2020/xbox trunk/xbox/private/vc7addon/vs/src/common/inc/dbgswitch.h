////////////////////////////////////////////////////////////////////////////////
// DBGSWITCH.H
//
// You must include this file per instructions below in order to get DBGOUT.H's
// debug output routines to work.
//
// 1) Define DBGOUT_MODULE to be the name of your module
// 2) Include this file in ONE of your source files
//
// This header file defines a debug switch (found in Help.Debug Options/Switches)
// with the name "Debug spew" under the group called DBGOUT_MODULE.  It also
// initializes DBGOUT.H's 'g_pfEnableDebugOutput' pointer to point to the value
// of the switch.

#ifdef DEBUG

#ifndef _DBGOUT_INCLUDED
#error You must include dbgout.h before this file!
#endif

#ifndef DBGOUT_MODULE
#define DBGOUT_MODULE "<unknown module>"
#endif

VSDEFINE_SWITCH(gfDBGOUT, DBGOUT_MODULE, "Debug spew");

class _DbgOutEnabler
{
public:
    _DbgOutEnabler() { g_pfEnableDebugOutput = &g_Switch_gfDBGOUT.m_fSet; }
};

_DbgOutEnabler  _enabler;       // Create one of these to initialize g_pfEnableDebugOutput

#endif // ifdef DEBUG
