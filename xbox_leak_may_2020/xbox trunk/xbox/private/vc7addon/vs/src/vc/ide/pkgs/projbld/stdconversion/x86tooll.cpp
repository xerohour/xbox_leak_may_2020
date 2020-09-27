//
// Intelx86 Linker Tool
//
// [colint]
//

#include "stdafx.h"		// our standard AFX include
#pragma hdrstop
#include "x86tooll.h"	// our local header file

IMPLEMENT_DYNAMIC (CLinkerX86Tool, CLinkerNTTool)

CLinkerX86Tool::CLinkerX86Tool() : CLinkerNTTool()
{
	m_strToolExeName = _T("link.exe");
}
